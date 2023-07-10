This repo attempts to evaluate if using `void*` for implementing mergesort
may affect performance. We implemented bottom-up mergesort with void pointers
and with macro expansion. We additionally replaced the first several rounds of
mergesort with insertion sort. We also included `std::stable_sort` for a
reference.

The following table shows the CPU seconds for sorting 50 million random
`int32_t`. Tests on the AMD CPU were performed on Cent OS 7 which comes with
gcc-4.8.5 by default. gcc-6.4.0 and gcc-10.3.0 were compiled from the source
several years ago. zig-0.10.1 was built with llvm-15.0.7 and downloaded from
the zig website.

|CPU              |Compiler    |void\*|void\*+ins|macro|macro+ins|STL|
|:----------------|:-----------|-----:|---------:|----:|--------:|--:|
|Apple M1         |clang-14.0.0|4.1   |10.2      |3.6  |3.3      |3.8|
|AMD EPYC 7301    |gcc-4.8.5   |13.2  |13.6      |4.4  |4.1      |4.3|
|AMD EPYC 7301    |gcc-6.3.0   |13.5  |14.0      |5.0  |4.5      |4.9|
|AMD EPYC 7301    |gcc-10.3.0  |12.3  |12.9      |7.2  |6.7      |6.8|
|AMD EPYC 7301    |zig-0.10.1  |7.4   |6.5       |4.6  |4.4      |6.5|
|Intel E5-2683    |gcc-9.4.0   |13.1  |12.9      |6.2  |5.5      |5.7|
|Intel Gold 6230  |gcc-9.3.0   |11.4  |16.2      |6.2  |5.6      |4.8|

My gcc-6.4.0 and gcc-10.3.0 installation may have issues as they are slower
than gcc-4.8.5 for std::stable\_sort. Clang seemed able to optimize the
void\* implementation though the macro implementation is consistently faster.
gcc produced slow executable for all the void\* implmentaiton. Not sure if the
latest gcc can be better. Also note that g++ sometimes compiled faster binaries
than gcc but the table only showed the gcc results except for STL.
