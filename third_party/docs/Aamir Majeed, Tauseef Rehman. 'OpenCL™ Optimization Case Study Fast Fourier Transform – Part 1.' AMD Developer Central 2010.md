# OpenCL™ Optimization Case Study Fast Fourier Transform – Part 1

### 1-D Complex to Complex FFT for AMD GPUs

This is the first article in a series of two, related to the implementation of a Fast Fourier Transform (FFT) on an AMD GPU using OpenCL™. In this article, we start by developing a 1-D complex to complex FFT GPU implementation which can also serve as a building block for 2-D FFT computations. We first provide an introduction to the FFT algorithm, as well as highlight its importance via a variety of applications. We then sketch how to build an FFT on an AMD GPU, paying attention to the efficient means of implementing the twiddle factors, indexing schemes for coalesced memory access, and a multistage approach built on small size hard coded FFT kernels. We then present performance results and relate them to fundamental bounds inherent in the GPU architecture (memory bandwidth, computational intensity). By the end of the second article, we will have reasonably high performance 1-D and 2-D FFT implementations for AMD GPUs.

### Introduction

The Fourier Transform is a well known and widely used tool in many scientific and engineering fields. A Fourier Transform converts a signal from the spatial domain to the frequency domain by representing it as a sum of properly chosen sinusoids [1]. A spatial domain signal is defined by amplitude values at specific time intervals. The frequency domain signal is defined by amplitudes and phase shifts of various sinusoids that make up the signal. Fourier transform is used in many signal processing techniques, including, frequency spectrum analysis, filtering and convolution. In this article, we are going to build an application to efficiently compute Fourier Transform on AMD GPUs using OpenCL™. But there are many types of Fourier Transforms and only one type (the discrete one) can be computed on a digital machine. We, therefore, first define all of the categories and then emphasize the Discrete Fourier Transform (DFT).

In the discrete world, we only deal with the DFT that is the Fourier Transform of a finite and periodic discrete signal computed on a digital machine. However, for completeness, we briefly describe below some other categories that form a part of this family.

#### Categories of Fourier Transform

Signals are usually classified on the basis of periodicity and continuity properties, such as a signal can either be aperiodic-continuous, periodic-continuous, aperiodic-discrete or periodic discrete[1]. A Fourier transform exists for each one of these signal types. All real-world signals fall into one of these classes:

* Aperiodic-Continuous. These are continuous signals that extend to both positive and negative infinity without repeating in a periodic fashion. This includes, for example, decaying exponentials and the Gaussian curve. The Fourier Transform for this type of signals is simply called the Fourier Transform.
* Periodic-Continuous. Continuous signals that repeat themselves after fixed time interval. Examples include sinusoids, square waves, and any waveform that repeats itself in a regular pattern from negative to positive infinity. The Fourier Transform of these signals is named as Fourier Series.
* Aperiodic-Discrete. These signals are only defined at discrete points between positive and negative infinity, and do not repeat themselves in a periodic fashion. This type of Fourier transform is called the Discrete Time Fourier Transform (DTFT). DTFT is discrete in time domain and continuous in the frequency domain.
* Periodic-Discrete.These are discrete signals that repeat themselves in a periodic fashion from negative to positive infinity. This class of Fourier Transform is called the Discrete Fourier Transform (DFT). The DFT is discrete in both time domain and the frequency domain. This particular category is the prime focus of this article and we will only restrict our further discussion to efficient methods for its computation.

### The Discrete Fourier Transform (DFT)

DFT converts a periodic-discrete time domain signal to a periodic-discrete frequency domain signal. DFT is widely used in signal processing and related fields to analyze the frequency contents of a periodically sampled signal, to solve partial differential equations, and to perform other operations such as convolutions and multiplying large integers. This makes DFT one of the most important algorithms in Digital Signal Processing (DSP). 

The DFT of a discrete signal x[n] is defined by the formula shown in Eq. (1) below.

Where N is the total number of input points (samples) and X[k] represents the discrete Fourier spectrum. j is the imaginary unit and, as there are N outputs X[k], and each output requires N additions and N multiplications. Thus, mathematical complexity of DFT is as given in [2]. 

It is clear that as the size of the input signal increases the computational complexity of DFT grows exponentially. There are, however, several methods that can compute the DFT in a much fewer operations. In general any method that can compute the DFT in operations proportional to N is referred as Fast Fourier Transform. These methods reduce the operations count by exploiting the periodic nature of DFT. The difference in speed can be substantial, especially for long data sets where N may be in the thousands or millions. In practice, the computation time can be reduced by several orders of magnitude in such cases, and the improvement is as described in [3].

### Applications of FFT
The importance of Fast Fourier Transform (FFT) in practical life can be judged by its many applications. Primarily, it forms the basis of Digital Signal Processing (DSP) where the most common application of FFT is the frequency spectrum analysis of a signal to examine the information encoded in frequency, phase and amplitude of its component sinusoids. FFT is also used as an intermediate step in more elaborate signal processing techniques including convolution, filtering, correlation, polynomial multiplication and primality testing, a few of which are described below:

#### Convolution via Frequency Domain.

For complex-valued functions ƒ and g defined on the set Z of integers, the discrete convolution of ƒ and g is given by Eq. 2 [4]:

Equation (2) requires N arithmetic operations per output value and N2 operations for N outputs. Thus, computational complexity of convolution is O(). Digital signal processing and other applications typically use fast convolution algorithms to reduce the cost of convolution.

The most common fast convolution algorithms uses a convolution theorem to reduce complexity of convolution. Eq. 3 defines the convolution theorem.

Where denotes the Fourier transform of f, and k is a constant that depends on the specific normalization of the Fourier transform. By applying the inverse Fourier transform, , we can write Eq. 4.

Thus, convolution of two finite-length complex sequences can be calculated by taking FFT of each sequence, multiplying them point wise, and then performing inverse FFT. This reduces the complexity of convolution to . For large input sequences FFT convolution is significantly faster than directly computing convolution.

#### Polynomial Multiplication.
Consider two polynomials given by Eq. 5 and Eq. 6.

Carl Friedrich Gauss was the first mathematician to discover a method to reduce the operation count of DFT in 1805. Subsequently, this method was rediscovered and popularized by many mathematicians and scientists in the latter half of 20th century. Now, as mentioned earlier any algorithm that can compute DFT in operations is regarded as a FFT algorithm. The Cooley-Tukey algorithm, Prime-Factor algorithm [7] and Rader-Brenner algorithm [8] are all examples. The Cooley-Tukey algorithm, which is by far the most commonly used FFT algorithm, becomes the basis of our further development of an FFT implementation on the GPU. Additionally, it can also be shown that the Cooley-Tukey FFT algorithm can compute DFT with higher accuracy as compared to the naïve DFT formula itself or some of the other fast algorithms [3].

### Cooley-Tukey FFT Algorithm.
J.W. Cooley and J.W. Tukey introduced the FFT algorithm in 1965 [9]. This is a divide-and-conquer algorithm that recursively breaks down a larger DFT of size N into smaller DFTs at each step. Commonly used variants of this frame work are listed here:
* Radix-2
* Radix-4
* Split Radix

In this article we are going to focus only on Radix-2 variant of the Cooley-Tukey algorithm due to its ease of implementation and numerical accuracy. The strategies developed here can be easily extended to higher Radix cases.

### Radix-2 FFT Algorithm
Radix-2 FFT algorithm recursively divides the N-point DFT into two N/2-point DFTs and a complex multiplication (Twiddle Factor) in between. The DFT of N-point time domain signal is defined by Eq. 8.

Where WN is the twiddle factor defined by Eq. 9.

Radix-2 FFT algorithm decomposes this N-point DFT into two half length DFTs with even indexed and odd indexed samples, as described by Eq. 10.

The Symmetry property of twiddle factors is defined by Eq. 12.

Applying the symmetry property to Eq. 11 above.

This decomposition recurs until the N-point DFT is divided into 2-point DFTs, hence the name Radix-2. The input size N must be an integral power of two to apply this recursive decomposition. FFT is computed in stages. Total complex multiplications are and complex additions are . The overall complexity is thus [10].

Radix-2 FFT algorithm computes FFT in following three steps.
* Decompose an N-point time domain signal into N separate signals such that each comprises of a single point. It is a multistage interlaced decomposition where even and odd indices get separated.
* Calculate the N frequency spectra corresponding to above mentioned N time domain signals.
* Synthesize the resulting N spectra into a single frequency spectrum.

We next look at each of these steps in detailed below.

#### Decomposition of Time Domain Signal. 
N-point time domain signal is decomposed into N time domain signals each composed of a single point. Decomposition is done in stages using Interlaced Decomposition at each stage [10]. Interlaced decomposition breaks the signal into its even and odd numbered samples at each stage. stages are required to decompose an N-point signal to N single-point signals. Figure 1 shows how decomposition works for a 16-point signal. The input sequence obtained by interlaced decomposition is the bit reversal of actual input sequence.

Figure 1: Interlaced Decomposition

#### Calculating Frequency Spectra. 
The second step in computing the FFT is to calculate the frequency spectra of N time domain signals each consisting of a single point. According to the Duality Principle the frequency spectrum of a 1-point time domain signal is
equal to itself [10]. A single point in the frequency domain corresponds to a sinusoid in the time domain. By duality, the inverse is also true; a single point in the time domain corresponds to a sinusoid in the frequency domain. Thus nothing is required to do this step and each of the 1-point signals is now a frequency spectrum, and not a time domain signal.

#### Frequency Spectrum Synthesis. 
To synthesize a single frequency spectrum from N frequency spectra, the N frequency spectra are combined in the exact reverse order that the time domain decomposition took place. The synthesis process is done one stage at a time. In the first stage, 16 frequency spectra (1 point each) are synthesized into 8 frequency spectra (2 points each). In the second stage, the 8 frequency spectra (2 points each) are synthesized into 4 frequency spectra (4 points each), and so on. The last stage results in the output of the FFT, a 16-point frequency spectrum. The frequency spectra are combined using a computation structure called FFT Butterfly.

##### Radix-2 FFT Butterfly.
The butterfly is the basic computational element of the FFT, transforming two complex points input into two complex point output. The Radix-2 FFT butterfly is derived by expanding the formula of DFT for two point signal (N=2) as shown in Eq. 15 through Eq. 17.

Figure 2 below, shows the structure of a Radix-2 FFT butterfly.

Figure 2: Radix-2 FFT butterfly

##### Generalized FFT Butterfly.

The general FFT butterfly is shown in figure 3. This butterfly requires two complex multiplications and two complex additions.

Figure 3: Generalized FFT butterfly

The operations count is reduced by applying the symmetry property of twiddle factor (W) as defined by Eq. 18.

The simplified FFT butterfly is shown in figure 4. This butterfly contains one complex multiplication, one complex addition and a sign change.

Figure 4: Simplified Radix-2 FFT butterfly

##### The FFT Flow Graph. 

The Radix-2 algorithm computes FFT of N points in stages. Butterflies are launched at each FFT stage. Figure 5 shows a flow graph for 8-point FFT. The input sequence is bit reversed and the output sequence is in natural order.

Figure 5: 8-point FFT Flow Grah

### OpenCL™ Implementation – Small Length FFT Kernels
Now that we have a fair understanding of the Radix-2 Cooley-Tukey 1D FFT algorithm and its different ingredients such butterflies and twiddle factors, we are ready to start an implementation using OpenCL™ specific to an AMD GPU. In the scope of this article, we will introduce the handling of complex data, the development of different hard-coded small size FFT kernels, and an indexing scheme for coalesced memory access and a multistage architecture for large size FFT computations.

#### Handling Complex Data. 
Because of its generality we picked the Complex-to-Complex FFT case for our implementations. The OpenCL™ 1.0 specifications, however, do not support complex numbers [11]. As there is no special data type for handling complex numbers, the data needs to be packaged to manage the complex arithmetic in OpenCL™. The real and complex parts of input data are single precision floating point values. Each complex number can be stored as a two element vector, where each element is a floating point value. OpenCL™ supports vector data types with floating point elements. Float2 is a built in data type in OpenCL™ which stores two floating point values in a vector. This data type is well suited for handling complex data type in OpenCL™.

```
//Zip Pattern
__kernel void Pack_Cmplx(__global float *Data_In /*SOA*/, __global float2 *Data_Out/*AOS*/, int N)
{
    int X = get_global_id(0);
    float2 V;
    V.x = Data_In[X];
    V.y = Data_In[X+N];
    Data_Out[X] = V;
}
```

#### 2-Point FFT Kernel. 

We are now ready to start with a basic FFT kernel. The 2-point FFT kernel in the listing below, loads two complex numbers (float2 vectors) from the input buffer (Data_In) to the private memory (Registers), performs the butterfly computations and writes the result to the output buffer (Data_Out).

```
__kernel void FFT_2( __global float2 *Data_In, __global float2 *Data_Out) 
{    
    int gId = get_global_id(0);

    float2 in0; 
    float2 in1;
    in0 = Data_In[gId];
    in1 = Data_In[gId+1];

    float2 V;
    V = in0;
    in0 = V + in1;
    in1 = V - in1;
    Data_Out[gId] = in0;
    Data_Out[gId+1] = in1;
}
```

##### Limitations of 2-point FFT Kernel. 

The Radix-2 FFT algorithm computes FFT of N-point signal in stages. Therefore, for a large input size, for example N=1024, 10 FFT stages are required. The 2-point FFT kernel described above performs one FFT stage per kernel launch, thus requires 10 kernel launches for 1024-point FFT. Launching too many kernels on a GPU device is expensive due to following reasons:

##### Kernel Launch Overhead. 

The kernel launch overhead includes both the time spent in the user application as well as the time spent in the runtime to launch the kernel. The overhead associated with each kernel launch degrades the performance of 2-point FFT kernel.

##### Frequent Global Memory Accesses. 

Each FFT kernel reads entire input array from global memory and writes the output back to global memory. Global memory is the least bandwidth memory available on GPUs. Therefore, frequent accesses to global memory reduce the throughput.

To improve the performance of FFT kernel, the number of kernel launches need to be reduced. One way of doing this is to perform multiple FFT stages in a single kernel which also reduces the number of global memory accesses. We will discuss the performance improvement achieved through this technique later on.

#### 4-Point FFT Kernel.

Calculating 4-point FFT using Radix-2 FFT algorithm requires two FFT stages and bit reversed input sequence as shown in figure 6.

Figure 6: 4-point FFT using Radix-2 Algorithm

The 4-point FFT kernel performs two FFT stages per kernel launch. The kernel loads four complex numbers (float2 vectors) from the input buffer (global memory) to the private memory (Registers). For effective bandwidth utilization, all accesses to global memory should be coalesced [9]. Therefore, the kernel code takes input data in normal order, instead of bit reversed order, to allow memory coalesces. The butterfly computations are arranged in such a way to work with natural input order. Figure 7 shows the flow graph for 4-point FFT kernel.

Figure 7: 4-point FFT Kernel Flow Graph

```
__kernel void FFT_4( __global float2 *Data_In, __global float2 *Data_Out) 
{
    int gId = get_global_id(0);

    //global memory coalesce
    //not bit reverse
    float2 in0 = Data_In[gId];
    float2 in1 = Data_In[gId+1];
    float2 in2 = Data_In[gId+2];
    float2 in3 = Data_In[gId+3];

    //stage 1
    //equivalent to bit reverse
    float2 v0 = in0 + in2;
    float2 v2 = in0 - in2;
    float2 v1 = in1 + in3
    float2 v3.x = in1.y - in3.y;
    float2 v3.y = in3.x - in1.x;

    //stage 2
    in0 = v0 + v1;
    in2 = v0 - v1;
    in1 = v2 + v3;
    in3 = v2 - v3;

    Data_Out[gId] = in0;
    Data_Out[gId+1] = in1;
    Data_Out[gId+2] = in2;
    Data_Out[gId+3] = in3;
}
```

Similarly, the 8-point FFT kernel can be written by expanding the DFT expression for N=8. The 8-point FFT kernel computes three FFT stages per kernel launch. FFT kernels for data sizes 16-point or larger are not hard-coded because they consume more register files per thread. This limits the total number of concurrent threads that can be launched and can potentially result in degradation of throughput. In most practical applications of computing FFTs of large signals, many of these kernels need to be launched concurrently as would be described in the next section. A performance comparison of 8-point and 16-point FFT kernels will also be done for such applications.

### Computing Large Size FFTs

Since, as discovered above we cannot hardcode a kernel for every size of signal, a logical solution would be to use a multistage architecture that recursively builds upon small size hard coded FFT kernels. Due to reasons outlined above we utilize only 2, 4 and 8 points FFT kernels at any stage of the algorithm. One such methodology is outlined below:

#### Multistage FFT Program

FFT kernels for 2, 4 and 8 points are hard-coded in our program. The code is developed by expanding the DFT formula for N = 2, 4 and 8, respectively. FFT of length 16 and above are computed by invoking a set of hard-coded, small length, FFT kernels. FFT of 16-points, for example, requires 4 stages and is calculated by launching 8-point FFT kernel once, completing first 3 stages, followed by a 2-point FFT kernel for 4th stage. FFT of 2048 points require 11 stages and is calculated by launching 8-point FFT kernel thrice, completing 9 stages, followed by a 4-point kernel for remaining two stages. Thus, multistage FFT program structure can be summarized as follows:

* 8-point FFT kernel is launched whenever the required FFT stages are greater than or equal to three.
* 4-point kernel is launched when required FFT stages are exactly two.
* 2-point FFT kernel is launched when only one FFT stage is required.

##### Generalized Data Access Pattern (Basic Indexing Scheme)
A simple case of a 16-point FFT can be used to explain the indexing scheme for large size FFTs. Figure 8 shows the flow graph for computing 16-point FFT using Radix-2 FFT algorithm. Here FFT is being computed by calculating two 8-point FFTs on a bit-reversed input sequence and then calculating eight, 2-point FFTs with an input stride of 8.

Figure 8: 16-point FFT using Radix-2 Algorithm

Multistage FFT program computes 16-point FFT by launching 8-point FFT kernel once followed by a 2-point FFT kernel. The indexing parameters used in the kernel code are defined below:

- N = Number of complex input points

- gId = Global ID of a work item

- gSize = Global work size (Total number of work items)

- Idout = Output index of each work item

- Ns = Parameter that defines output index

- K_W = Kernel Size

###### Input Indexing. 

Now we can examine the input indexing scheme for an 8-point FFT kernel in detail as this indexing scheme applies to all hard-coded FFT kernels.

For 16-point FFT example, the 8-point FFT kernel is launched with a global size (gSize) of 2. Each thread will load 8 complex input points from Global memory, compute 8 point FFT and write the output to global memory. Required input pattern for Radix-2 algorithm is shown in figure 9. This is the bit reversed sequence for 16-points.

Figure 9: Bit Reversed Input Sequence for 16-point FFT

The FFT kernels listed earlier e.g. (2-point and 4-point FFTs) do not require bit-reversed input sequence because the butterfly computations are arranged in such a way to work with standard lexicographic ordering of the input sequence. The input sequence as required by both work items (threads) is shown in figure 10.

Figure 10: Input Sequence for 8-point FFT Kernel

It can be easily identified that first data element required by each work item is located at an input index equal to the Global ID (gId) of that work item. And both work items load data with an input stride equal to the Global Size (gSize).

The portion of the kernel code for this indexing scheme is listed below:

```
    int gId = get_global_id(0);
    int gSize = N/8; 
    
    float2 in0 = datain[(0*gSize)+gId];
    float2 in1 = datain[(1*gSize)+gId];
    float2 in2 = datain[(2*gSize)+gId];
    float2 in3 = datain[(3*gSize)+gId];
    float2 in4 = datain[(4*gSize)+gId];
    float2 in5 = datain[(5*gSize)+gId];
    float2 in6 = datain[(6*gSize)+gId];
    float2 in7 = datain[(7*gSize)+gId];
```

This input indexing pattern is used by all hard-coded FFT kernels.

###### Output Indexing.

The hard-coded FFT kernels generate output in the natural order. A specific output indexing scheme is required to make the input indexing scheme consistent throughout the program. The output index (Idout) is defined by parameter Ns. Ns can be mathematically represented as Eq. 20.

Thus at first kernel launch the value of Ns is 1, as no FFT stage has completed at that point. The value of Ns is 8 after one 8-point FFT kernel stage and 64 after two 8-point FFT stages.

Next we can extend this concept to the 16-point FFT case. The output pattern generated by 8-point FFT kernel is shown in figure 11.



Figure 11: Output Sequence of 8-point FFT Kernel

By looking at the flow graph for 16-point FFT shown in Figure 8, it can be seen that this is the same pattern as required by next FFT stage (2-point FFT).

Figure 12 shows the output pattern generated by 2-point FFT kernel which is the last stage of 16-point FFT.



Figure 12: Output Pattern of 2-point FFT Kernel

By a careful analysis of output index patterns of 2 and 8 point FFTs as well some larger cases e.g. 32, 64, 128 and 256 size FFTs the following pattern emerges.

At every FFT stage, each work item writes K_W output points to the global memory with a stride equal to Ns.
For first FFT stage, the output index (Idout) of each work item is equal to the product of kernel size and Global ID (gId) of that work item.
For last FFT stage, the output index (Idout) of each work item is equal to the Global ID (gId) of that work item.
The code segment for output indexing of 8-point kernel is listed below.

```
    int Idout = (gId/Ns)*Ns*K_W+(gId%Ns);
    dataout[(0*Ns)+Idout] = in0;
    dataout[(1*Ns)+Idout] = in1;
    dataout[(2*Ns)+Idout] = in2;
    dataout[(3*Ns)+Idout] = in3;
    dataout[(4*Ns)+Idout] = in4;
    dataout[(5*Ns)+Idout] = in5;
    dataout[(6*Ns)+Idout] = in6;
    dataout[(7*Ns)+Idout] = in7;
```

###### Twiddle Factor Calculation

The twiddle factor (W) for each FFT stage is calculated on the fly. The mathematical equations for calculating twiddle factor are as follows.



Applying the identity in Eq. 22 to Eq. 21 yields equation Eq. 23 which is used to calculate twiddle factors is in the kernel.



The key variables in this expression (n, k and N)are described below.

- K is the index of frequency domain output of a kernel. The value of k ranges from one to K_W – 1 (K =0 is not included as the twiddle factor goes to 1 at this value of K).

- N is the transform size at the particular FFT stage being computed.

- n is the time domain index given by Eq. 24.



The code segment for twiddle factor calculation and multiplication with respective input elements is listed next. Note that the twiddle factor multiplications do not occur at first FFT kernel launch, as the kernel code itself includes these multiplications for initial stages.

```
#define twidle_factor(k, angle, in) 
{ \
 float2 tw, v;\
 tw.x = native_cos(k*angle);\
 tw.y = native_sin(k*angle);\
 v.x = tw.x*in.x - tw.y*in.y;\
 v.y = tw.x*in.y + tw.y*in.x;\
 in.x = v.x;\
 in.y = v.y;\
}
if (Ns!=1)
{
float angle = -2*PI*(gId%Ns)/(Ns*K_W);
twidle_factor(1, angle, in1);
twidle_factor(2, angle, in2);
twidle_factor(3, angle, in3);
twidle_factor(4, angle, in4);
twidle_factor(5, angle, in5);
twidle_factor(6, angle, in6);
twidle_factor(7, angle, in7);
}
```

###### Large Size FFT Kernel
The kernel code for large size FFT using a multistage architecture that results from all the building blocks discussed above is now listed below:
```
#define PI  3.14159265f
#define FFT2(in0, in1) {\
  float2 v0;\
  v0 = in0;\
  in0 = v0 + in1;\
  in1 = v0 - in1;\
}
#define FFT4(in0, in1, in2, in3) {\
float2 v0, v1, v2, v3;\
v0 = in0 + in2;\
v1 = in1 + in3;\
v2 = in0 – in2;\
v3.x = in1.y – in3.y;\
v3.y = in3.x – in1.x;\
in0 = v0 + v1;\
in2 = v0 – v1;\
in1 = v2 + v3;\
in3 = v2 – v3;\
}

#define twidle_factor(k, angle, in)
{ \
float2 tw, v;\
tw.x = native_cos(k*angle);\
tw.y = native_sin(k*angle);\
v.x = tw.x*in.x – tw.y*in.y;\
v.y = tw.x*in.y + tw.y*in.x;\
in.x = v.x;\
in.y = v.y;\
}

__kernel void Pack_Cmplx( __global float* Data_In,
__global float2* Data_Out, int N)
{
int X = get_global_id(0);
float2 V;
V.x = Data_In [X];
V.y = Data_In [X+N];
Data_Out [X] = V;
}

__kernel void FFT_2( __global float2* Data_In,
__global float2* Data_Out)
{
int gId = get_global_id(0);
int gSize = N/2;
float2 in0, in1;
in0 = datain[(0*gSize)+gId];
in1 = datain[(1*gSize)+gId];

if (Ns!=1)
{
float angle = -2*PI*(gId)/(N);
twidle_factor(1, angle, in1);
}

FFT2(in0, in1);
int Idout = (gId/Ns)*Ns*K_W+(gId%Ns);
dataout[(0*Ns)+Idout] = in0;
dataout[(1*Ns)+Idout] = in1;
}

__kernel void FFT_4( __global float2* Data_In,
__global float2* Data_Out)
{
int gId = get_global_id(0);
int gSize = N/4;
float2 in0, in1, in2, in3;
in0 = datain[(0*gSize)+gId];
in1 = datain[(1*gSize)+gId];
in2 = datain[(2*gSize)+gId];
in3 = datain[(3*gSize)+gId];

if (Ns!=1)
{

float angle = -2*PI*(gId)/(N);
twidle_factor(1, angle, in1);
twidle_factor(2, angle, in2);
twidle_factor(3, angle, in3);
}

FFT4(in0, in1, in2, in3);
int Idout = (gId/Ns)*Ns*K_W+(gId%Ns);
dataout[(0*Ns)+Idout] = in0;
dataout[(1*Ns)+Idout] = in1;
dataout[(2*Ns)+Idout] = in2;
dataout[(3*Ns)+Idout] = in3;
}
```

###### Performance Comparison

As discussed above, the choice of 2-point, 4-point and 8-point FFT kernels have a direct impact on performance limiting parameters such as global memory access and the number of kernel launches. Therefore, we analyze the impact of these choices empirically by testing for a range of input signal sizes.

Performance tests were carried out on a machine with an Intel Core i7 930 CPU and an
ATI Radeon™ HD5870 GPU running 64-bit Windows® 7 operating system and AMD APP SDK v2.5. The kernel execution times have been measured using the AMD APP Profiler v2.3.

Figure 13 below shows a performance comparison of varying length FFTs using 2, 4 and 8-point FFT kernels. The graph has been plotted with input size on the horizontal axis (Binary Log scale) and the number of FFTs of that size which can be computed in a second shown on the vertical axis. We can clearly see that the 8-point kernel size gives higher throughput across all data sizes tested on (Maximum data size of 8 million points).



Figure 13: Performance Comparison of 2, 4 and 8-point FFT Kernels

We now do a similar performance comparison of 8 and 16-point FFT kernels and the results are shown in figure 14 below. It is clear from this graph that the use of a 16-point kernel despite having a higher ALU-to-Fetch ratio does not perform any better than the 8-point kernel choice since the higher register usage limits the number of wave-fronts per compute unit [9]. Therefore, hard coded 16-point or higher size FFT kernels are not utilized for multistage computation on current hardware.



Figure 14: Performance Comparison of 8 and 16-point FFT Kernel

In the next part of this article we will discuss further optimization strategies involving the use of local memory, in-place computations and resident kernels. Further, we will develop a batch computation framework of multiple 1D FFTs that will be applied for 2D FFT computations.