#include "denoiser.h"
#include "util/mathutil.h"

Denoiser::Denoiser() : m_useTemportal(false) {}

void Denoiser::Reprojection(const FrameInfo &frameInfo) {
    int height = m_accColor.m_height;
    int width = m_accColor.m_width;
    Matrix4x4 preWorldToScreen =
        m_preFrameInfo.m_matrix[m_preFrameInfo.m_matrix.size() - 1]; // P_{i-1}V_{i-1}
    Matrix4x4 preWorldToCamera =
        m_preFrameInfo.m_matrix[m_preFrameInfo.m_matrix.size() - 2]; // V_{i-1}
#pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // TODO: Reproject
            m_valid(x, y) = false;
            m_misc(x, y) = Float3(0.f);

            int id = frameInfo.m_id(x, y);
            if (id > -1) {
                Matrix4x4 object_to_world = frameInfo.m_matrix[id]; // M
                Matrix4x4 pre_object_to_world = m_preFrameInfo.m_matrix[id]; // M_{i-1}
                
                auto position = frameInfo.m_position(x, y);
                auto position_object = Inverse(object_to_world)(position, Float3::Point);
                auto pre_position_world = pre_object_to_world(position_object, Float3::Point);
                auto pre_position_screen = preWorldToScreen(pre_position_world, Float3::Point);
                if (pre_position_screen.x < 0 ||pre_position_screen.x >= width ||
                pre_position_screen.y < 0 || pre_position_screen.y >= height) {
                    continue;
                }
                else {
                    int pre_id = m_preFrameInfo.m_id(pre_position_screen.x, pre_position_screen.y);
                    if (pre_id == id) {
                        m_valid(x, y) = true;
                        m_misc(x, y) =m_accColor(pre_position_screen.x, pre_position_screen.y);
                    }
                }
            }
        }
    }
    std::swap(m_misc, m_accColor);
}

void Denoiser::TemporalAccumulation(const Buffer2D<Float3> &curFilteredColor) {
    int height = m_accColor.m_height;
    int width = m_accColor.m_width;
    int kernelRadius = 3;
#pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // TODO: Temporal clamp
            Float3 color = m_accColor(x, y);
            // TODO: Exponential moving average
            float alpha = 1.0f;
            m_misc(x, y) = Lerp(color, curFilteredColor(x, y), alpha);
        }
    }
    std::swap(m_misc, m_accColor);
}

Buffer2D<Float3> Denoiser::Filter(const FrameInfo &frameInfo) {
    int height = frameInfo.m_beauty.m_height;
    int width = frameInfo.m_beauty.m_width;
    Buffer2D<Float3> filteredImage = CreateBuffer2D<Float3>(width, height);
    int kernelRadius = 16;
#pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // TODO: Joint bilateral filter
            // filteredImage(x, y) = frameInfo.m_beauty(x, y);

            int x_min = std::max(0, x - kernelRadius);
            int x_max = std::min(width - 1, x + kernelRadius);
            int y_min = std::max(0, y - kernelRadius);
            int y_max = std::min(height - 1, y + kernelRadius);

            auto postion_i = frameInfo.m_position(x, y);
            auto normal_i = frameInfo.m_normal(x, y);
            auto color_i = frameInfo.m_beauty(x, y);

            Float3 filtered_color;
            float weights = .0f;

            for (int x_j = x_min; x_j <= x_max; x_j++) {
                for (int y_j = y_min; y_j <= y_max; y_j++) {
                    if (x_j == x && y_j == y) {
                        weights += 1.0;
                        filteredImage(x, y) += color_i;
                    }
                    else {
                        auto postion_j = frameInfo.m_position(x_j, y_j);
                        auto normal_j = frameInfo.m_normal(x_j, y_j);
                        auto color_j = frameInfo.m_beauty(x_j, y_j);

                        float D_Coord = SqrDistance(postion_i, postion_j) / (2.0f * Sqr(m_sigmaCoord));
                    
                        float D_Color = SqrDistance(color_i, color_j) / (2.0f * Sqr(m_sigmaColor));
                    
                        float D_Normal = Sqr(SafeAcos(Dot(normal_i, normal_j))) / (2.0f * Sqr(m_sigmaNormal));
                    
                        float D_Plane = 0.0f;
                        if (D_Coord > 0.0f) {
                            D_Plane = Sqr(Dot(normal_i, Normalize(postion_j - postion_i)));
                        }
                        D_Plane /= (2.0f * Sqr(m_sigmaPlane));

                        float weight = std::exp(-D_Coord - D_Color - D_Normal - D_Plane);
                        weights += weight;
                        filtered_color += color_j * weight;
                    }
                }
            }
            filteredImage(x, y) = filtered_color / weights;
        }
    }
    return filteredImage;
}

void Denoiser::Init(const FrameInfo &frameInfo, const Buffer2D<Float3> &filteredColor) {
    m_accColor.Copy(filteredColor);
    int height = m_accColor.m_height;
    int width = m_accColor.m_width;
    m_misc = CreateBuffer2D<Float3>(width, height);
    m_valid = CreateBuffer2D<bool>(width, height);
}

void Denoiser::Maintain(const FrameInfo &frameInfo) { m_preFrameInfo = frameInfo; }

Buffer2D<Float3> Denoiser::ProcessFrame(const FrameInfo &frameInfo) {
    // Filter current frame
    Buffer2D<Float3> filteredColor;
    filteredColor = Filter(frameInfo);

    // Reproject previous frame color to current
    if (m_useTemportal) {
        Reprojection(frameInfo);
        TemporalAccumulation(filteredColor);
    } else {
        Init(frameInfo, filteredColor);
    }

    // Maintain
    Maintain(frameInfo);
    if (!m_useTemportal) {
        m_useTemportal = true;
    }
    return m_accColor;
}
