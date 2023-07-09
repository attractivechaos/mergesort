This repo attempts to evaluate if using `void*` for mergesort implementation
may affect performance. The following table shows the timing for sorting 50
million random `int32_t`. Tests on x64 CPUs were performed on two Cent OS 7
machines. Cent OS 7 comes with gcc-4.8.5 by default. I compiled gcc-10.3.0 from
the source a few years ago.

|CPU|Compiler|Mode|macro|void\*|std::stable\_sort|
|:--|:-------|:---|----:|-----:|----------------:|
|Apple M1         |clang-14.0.0|C  |3.4| 9.6|   |
|Apple M1         |clang-14.0.0|C++|3.4| 4.0|3.8|
|AMD EPYC 7301    |gcc-4.8.5   |C  |4.2|12.8|   |
|AMD EPYC 7301    |gcc-4.8.5   |C++|4.3|12.8|4.3|
|Intel Silver 4116|gcc-4.8.5   |C  |4.7|16.5|   |
|Intel Silver 4116|gcc-4.8.5   |C++|4.7|16.0|4.5|
|AMD EPYC 7301    |gcc-10.3.0  |C  |7.1|12.3|   |
|AMD EPYC 7301    |gcc-10.3.0  |C++|7.3|12.1|6.9|
|Intel Silver 4116|gcc-10.3.0  |C  |8.4|16.5|   |
|Intel Silver 4116|gcc-10.3.0  |C++|9.6|16.9|7.6|

My gcc-10.3.0 installation may have issues as it is slower than gcc-4.8.5 for
std::stable\_sort. With gcc-4.8.5 or clang-14.0.0 in the C mode, the macro
implementation is a few times faster than the void\* version. clang-14.0.0 in
the C++ mode can optimize void\*, though it still takes 18% more time than the
macro version.
