# Gauss_OpenMP
高斯消去OpenMP实验


**x86文件夹**：
- Guass文件夹存放了高斯消去串行算法与不同线程数下并行优化算法的源码。
- Blocks_Circles文件夹存放了不同chunk_size下并行算法的源码。
- Dimensions文件夹存放有高斯消去行划分算法与列划分算法的源码。
- OpenMP_SIMD文件夹存放有OpenMP与SSE,AVX结合的两种算法的源码。

**arm文件夹**：
- Guass.cpp为高斯消去串行算法与不同线程数下并行优化算法的源码。
- Blocks_Circles.cpp是不同chunk_size下并行算法的源码。
- Dimensions.cpp为高斯消去行划分算法与列划分算法的源码。
- OpenMP_SIMD.cpp为OpenMP与Neon结合的算法的源码。

**DevCloud文件夹**：
- Offload文本文件是在Intel DevCloud平台进行卸载到GPU实验时运行的代码。
