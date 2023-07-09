Mac M1 with clang-14.0.0:
```txt
merge sort macro [1]: 3.425224 sec
merge sort void [1]: 4.104437 sec
FreeBSD mergesort void [1]: 6.367003 sec
std::stable_sort [1]: 3.857534 sec
```
Linux AMD EPYC 7301 with gcc-4.8.5 (slower with gcc-10.3.0 or with -O2):
```txt
merge sort macro [1]: 4.248351 sec
merge sort void [1]: 12.802520 sec
FreeBSD mergesort void [1]: 11.521694 sec
std::stable_sort [1]: 4.287288 sec
```
Linux Linux Intel Silver 4116 @ 2.10 GHz:
```txt
merge sort macro [1]: 4.669559 sec
merge sort void [1]: 16.371968 sec
FreeBSD mergesort void [1]: 12.704502 sec
std::stable_sort [1]: 4.241645 sec
```
