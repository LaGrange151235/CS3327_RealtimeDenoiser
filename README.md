# Denoise
This repository is for the course project of CS3327. The main tasks are:
1. Implement a `Filter` function at `src/denoiser.cpp` to achieve single-frame noise reduction.
    - Filter Core: $J(i, j)=exp(-\frac{||i-j||^2}{2\sigma_p^2}-\frac{||\tilde C[i]-\tilde C[j]||^2}{2\sigma_c^2}-\frac{D_{normal}(i,j)^2}{2\sigma_n^2}-\frac{D_{plane}(i,j)^2}{2\sigma_d^2})$
    - $D_{normal}(i,j)=arc\cos(Normal[i]\cdot Normal[j])$
    - $D_{plane}(i,j)=Normal[i]\cdot\frac{Position[j]-Position[i]}{||Position[j]-Position[i]||}$
2. Implement a `Reprojection` function at `src/denoiser.cpp` to project information from the previous frame to the current frame and check whether the previous frame is valid to help filter the noise or not.
    - The equation to find the corresponding pixel at the previous frame is: 
    $Screen_{i-1}=P_{i-1}V_{i-1}M_{i-1}M^{-1}World_i$
        - $P_{i-1}$: Matrix from camera to screen at previous frame
        - $V_{i-1}$: Matrix from world to camera at previous frame
        - $M_{i-1}$: Matrix from object to world at previous frame
        - $M$: Matrix from object to world at current frame
3. Implement a `TemporalAccumulation` function at `src/denoiser.cpp` to utilize information from `Reprojection` denoise the current frame by combining current frame with valid previous frame incrementally.
    - The equation of combination is: 
    $\overline{C}_i\leftarrow\alpha\overline{C}_i+(1-\alpha)Clamp(\overline{C}_{i-1})$
        - $\overline{C}_i$: Current frame
        - $\overline{C}_{i-1}$: Previous frame which has already denoised

## Environme
### Linux (WSL)
- Ubuntu22.04.1 LTS @ WSL2
- gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0
- cmake version 3.22.1
- ffmpeg version 4.4.2-0ubuntu0.22.04.1
### Windows
- Windows 10 x86_64
- gcc version 13.1.0 (x86_64-posix-seh-rev1, Built by MinGW-Builds project)
- cmake version 3.26.4
- ffmpeg version 6.0-full_build-www.gyan.dev

## Install
### Linux (WSL)
```bash
sudo apt install gcc
sudo apt install cmake
sudo apt install ffmpeg
bash build.sh
```
### Windows
```bash
scoop install mingw
scoop install ffmpeg
build.bat
```
---
此仓库为CS3327的大作业，主要任务有:
1. 实现`src/denoiser.cpp`中的`Filter`函数，完成单帧降噪。
    - 滤波核函数： $J(i, j)=exp(-\frac{||i-j||^2}{2\sigma_p^2}-\frac{||\tilde C[i]-\tilde C[j]||^2}{2\sigma_c^2}-\frac{D_{normal}(i,j)^2}{2\sigma_n^2}-\frac{D_{plane}(i,j)^2}{2\sigma_d^2})$
    - $D_{normal}(i,j)=arc\cos(Normal[i]\cdot Normal[j])$
    - $D_{plane}(i,j)=Normal[i]\cdot\frac{Position[j]-Position[i]}{||Position[j]-Position[i]||}$
2. 实现`src/denoiser.cpp`中的`Reprojection`函数，将前一帧的信息投影到当前帧，检查前一帧是否可以用于降噪。
    - 找到前一帧中对应当前帧像素的公式是：
    $Screen_{i-1}=P_{i-1}V_{i-1}M_{i-1}M^{-1}World_i$
        - $P_{i-1}$：前一帧摄像机坐标系到屏幕坐标系的矩阵
        - $V_{i-1}$：前一帧世界坐标系到摄像机坐标系的矩阵
        - $M_{i-1}$：前一帧示物体坐标系到世界坐标系的矩阵
        - $M$: 当前帧示物体坐标系到世界坐标系的矩阵
3. 实现`src/denoiser.cpp`中的`TemporalAccumulation`函数，利用`Reprojection`得到的信息通过增量式的将当前帧和已经降噪的前一帧结合进行降噪。
    - 两帧结合的公式是：
    $\overline{C}_i\leftarrow\alpha\overline{C}_i+(1-\alpha)Clamp(\overline{C}_{i-1})$
        - $\overline{C}_i$：当前帧
        - $\overline{C}_{i-1}$：已经降噪的前一帧

## 环境
### Linux (WSL)
- Ubuntu22.04.1 LTS @ WSL2
- gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0
- cmake version 3.22.1
- ffmpeg version 4.4.2-0ubuntu0.22.04.1
### Windows
- Windows 10 x86_64
- gcc version 13.1.0 (x86_64-posix-seh-rev1, Built by MinGW-Builds project)
- cmake version 3.26.4
- ffmpeg version 6.0-full_build-www.gyan.dev
## 安装
### Linux (WSL)
```bash
sudo apt install gcc
sudo apt install cmake
sudo apt install ffmpeg
bash build.sh
```
### Windows
```bash
scoop install mingw
scoop install ffmpeg
build.bat
```