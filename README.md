|CPU|Compiler|Mode|macro|void\*|std::stable\_sort|
|:--|:-------|:--:|----:|-----:|----------------:|
|Apple M1         |clang-14.0.0|C  |3.4| 9.6|   |
|Apple M1         |clang-14.0.0|C++|3.4| 4.0|3.8|
|AMD EPYC 7301    |gcc-4.8.5   |C  |4.2|12.8|   |
|AMD EPYC 7301    |gcc-4.8.5   |C++|4.3|12.8|4.3|
|AMD EPYC 7301    |gcc-10.3.0  |C  |7.1|12.3|   |
|AMD EPYC 7301    |gcc-10.3.0  |C++|7.3|12.1|6.9|
|Intel Silver 4116|gcc-4.8.5   |C  |4.7|16.5|   |
|Intel Silver 4116|gcc-4.8.5   |C++|4.7|16.0|4.5|
|Intel Silver 4116|gcc-10.3.0  |C  |8.4|16.5|   |
|Intel Silver 4116|gcc-10.3.0  |C++|9.6|16.9|7.6|

Note: gcc-4.8.5 is the system default on Cent OS 7. gcc-10.3.0 was compiled
from source on Cent OS 7.
