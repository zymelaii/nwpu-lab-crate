# GPU 并行程序设计

## 课程介绍

安排：理论课+两次实验

考查方式：考试+实验汇报

关于考试：填空+简答+代码，倒数第二节课课内考试，考完可润

关于实验：自己选个 cuda 的题目，写完代码做个 10 分钟的 PPT 主要讲解代码实现，倒数第一节课汇报

关于上课：人少，所以前几次都会点名，回答问题轮流，每个人大概一学期会被叫到 4~5 次

关于问题：没啥太大的难度，看着代码讲代码比较多，或者上去写代码

## 关于代码

课上没啥需要写的，基本都是老师发的代码，整理了下——哦好吧，前两次课的代码好像是需要填空的，填完的也放上了。

- [课内代码](./sample)
- [个人实验代码](./expr)
- [课内代码实现](./impl)

> 实现对应课内代码下的同名源文件。

> 课内代码的一部分代码是从 CUDA 官网截取的，缺少头文件，无法直接编译，瞅瞅就行，或者自己去官网下载完整示例。
>
> 其余的文件都是单文件源码。

## 环境配置

需要有一张 NVIDIA 的显卡，下完装完配下 nvcc 的路径就行（可能都不需要配，忘了）

[下载地址](https://developer.nvidia.com/cuda-downloads)

> 大部分人应该直到最后一次汇报都没装 cuda dev，平时上课都是颅内编译运行；也可能最后一次汇报也没装，毕竟这个 md 是报告前两天写的。

## 课内知识点

- CUDA 执行环境标识符
  - \_\_global__
  - \_\_device__
  - \_\_host__
- 内核函数
  - 定义方法
  - 基本调用形式 `kernel<<< gridDim, blockDim >>>(arguments...)`
    - 拓展 `kernel<<< gridDim, blockDim, sharedMem, stream >>>(arguments...)`
  - 内建参数 gridDim/blockDim/blockIdx/threadIdx
- [CUDA 基本结构](https://en.wikipedia.org/wiki/CUDA#Ontology)
  - host: 宿主机（CPU + 内存）
  - device: GPU 设备
  - SM: Streaming Multiprocessor ~ block
  - [warp](https://en.wikipedia.org/wiki/Thread_block_(CUDA_programming)#Warps): SM 的最小执行单位 ~ 32 threads
  - SP ~ thread ~ cuda core
- 共享内存
  - 创建方法 \_\_shared__
    - 动态大小 `extern __shared__ int mem[];`
  - 使用范围 同一个 block 内的线程
  - Bank 定义
  - Bank Conflict
- Warp Divergence
- CUDA 流处理与异步操作
- 内存
  - CUDA 全局内存
  - CUDA 共享内存
  - 锁页内存 -> zero-copy
- 基本操作
  - 同步
    - 块内线程同步 `__syncthreads()`
    - 设备同步 `cudaDeviceSynchronize()`
    - 流同步 `cudaStreamSynchronize(stream)`
  - 内存
    - 设备内存分配 `cudaMalloc/cudaFree`
    - 内存拷贝 `cudaMemcpy/cudaMemcpyAsync`
    - 锁页内存分配 `cudaMallocHost/cudaFreeHost`
    - 统一内存分配 `cudaMallocManaged/cudaFree`
- [设备设置与属性获取](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__DEVICE.html)
  - 获取设备总数 `cudaGetDeviceCount(&count)`
  - 设置当前设备 `cudaSetDevice(device)`

## 考题参考

- **填空题** 定义内核函数使用的标识符
- **填空题** 定义共享内存
- **填空题** 计算线程索引
- **填空题** 设置设备使用的函数
- **简答题** 以矩阵乘法为例，解释为什么需要共享内存
- **简答题** 以归并排序为例，解释什么是 warp divergence
- **简答题** CUDA 的主要应用领域
- **简答题** 什么是流处理器
- **简答题** 什么是 zero-copy
- **简答题** 什么是原子操作
- **代码编写** 编写 1024 长度数组求和的内核函数及其调用代码
- **代码编写** 编写 1024x1024 矩阵乘法的内核函数及其调用代码
- **代码编写** 编写使用流操作的 1024 维向量点积及其调用代码
- **代码编写** 编写 1024 长度数组排序的内核函数及其调用代码

> 填空题忘差不多了，反正基本在知识点里，大概有 10 道填空？

> 这个调用代码有点难理解，到底是只有那一句内核调用呢，还是最小的整个配套代码呢？反正要保险的话就把函数都背背吧，如果按照配套代码来写这玩意的代码量在内核函数的 1~2 倍不等。
>
> 反正我是没记函数，名字靠猜，参数靠猜，不确定的类型画个占位符，把执行逻辑串了串。如果对题目理解没问题的话应该基本都能做到至多只有函数名猜错。
>
> 最后回来一看，流那题理解错了，标准解应该是单流配上异步拷贝和内核执行两个任务，我按多设备的思路创建了两个流来写了。

## 参考

- [CUDA](https://en.wikipedia.org/wiki/CUDA)
- [CUDA Thread block](https://en.wikipedia.org/wiki/Thread_block_(CUDA_programming))
