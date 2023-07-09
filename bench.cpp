#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <algorithm>

#define KSORT2_CHECK(prefix, type_t, __sort_lt) \
	int prefix##_is_sorted(type_t *st, type_t *en) \
	{ \
		type_t *p; \
		for (p = st + 1; p < en; ++p) \
			if (__sort_lt(*p, *(p-1))) \
				return 0; \
		return 1; \
	}

#define KSORT2_MERGE(prefix, type_t, __sort_lt) \
	void prefix##_merge_sort(type_t *st, type_t *en, type_t *tmp) \
	{ \
		type_t *a2[2], *a, *b; \
		size_t n = en - st; \
		int curr, shift; \
		a2[0] = st, a2[1] = tmp; \
		for (curr = 0, shift = 0; (1ul<<shift) < n; ++shift) { \
			a = a2[curr]; b = a2[1-curr]; \
			if (shift == 0) { /* the first round; not necessary but ~5% faster this way */ \
				type_t *p = b, *i, *eb = a + (n>>1<<1); \
				for (i = a; i < eb; i += 2) { \
					if (__sort_lt(*(i+1), *i)) { \
						*p++ = *(i+1); *p++ = *i; \
					} else { \
						*p++ = *i; *p++ = *(i+1); \
					} \
				} \
				if (n&1) *p++ = *i; \
			} else { /* the general solution */ \
				size_t i, step = 1ul<<shift; \
				for (i = 0; i < n; i += step<<1) { \
					type_t *p, *j, *k, *ea, *eb; \
					if (n < i + step) { \
						ea = a + n; eb = a; \
					} else { \
						ea = a + i + step; \
						eb = a + (n < i + (step<<1)? n : i + (step<<1)); \
					} \
					j = a + i; k = a + i + step; p = b + i; \
					while (j < ea && k < eb) { \
						if (__sort_lt(*k, *j)) *p++ = *k++; \
						else *p++ = *j++; \
					} \
					while (j < ea) *p++ = *j++; \
					while (k < eb) *p++ = *k++; \
				} \
			} \
			curr = 1 - curr; \
		} \
		if (curr == 1) memcpy(st, tmp, n * sizeof(*st)); \
	}

static int int_lt(const void *a, const void *b)
{
	return (*(const int*)a < *(const int*)b);
}

static int int_cmp(const void *a, const void *b)
{
	return (*(const int*)a > *(const int*)b) - (*(const int*)a < *(const int*)b);
}

typedef int func_lt(const void *a, const void *b);

void merge_sort_void(size_t n, size_t len, void *array, void *tmp, func_lt lt)
{
	void *a2[2], *a, *b;
	int curr, shift;
	a2[0] = array, a2[1] = tmp;
	for (curr = 0, shift = 0; (1ul<<shift) < n; ++shift) {
		a = a2[curr]; b = a2[1-curr];
		if (shift == 0) {
			uint8_t *p = (uint8_t*)b, *i, *eb = (uint8_t*)a + (n>>1<<1) * len;
			for (i = (uint8_t*)a; i < eb; i += 2 * len) {
				if (lt(i + len, i)) {
					memcpy(p, i + len, len);
					p += len;
					memcpy(p, i, len);
					p += len;
				} else {
					memcpy(p, i, len * 2);
					p += 2 * len;
				}
			}
			if (n&1) memcpy(p, i, len);
		} else {
			size_t i, step = 1ul<<shift;
			for (i = 0; i < n; i += step<<1) {
				uint8_t *p, *j, *k, *ea, *eb;
				if (n < i + step) {
					ea = (uint8_t*)a + n * len; eb = (uint8_t*)a;
				} else {
					ea = (uint8_t*)a + (i + step) * len;
					eb = (uint8_t*)a + (n < i + (step<<1)? n : i + (step<<1)) * len;
				}
				j = (uint8_t*)a + i * len; k = (uint8_t*)a + (i + step) * len; p = (uint8_t*)b + i * len;
				while (j < ea && k < eb) {
					if (lt(k, j)) {
						memcpy(p, k, len);
						p += len, k += len;
					} else {
						memcpy(p, j, len);
						p += len, j += len;
					}
				}
				while (j < ea) {
					memcpy(p, j, len);
					p += len, j += len;
				}
				while (k < eb) {
					memcpy(p, k, len);
					p += len, k += len;
				}
			}
		}
		curr = 1 - curr;
	}
	if (curr == 1) memcpy(array, tmp, n * len);
}

#define ks_generic_lt(a, b) ((a) < (b))
KSORT2_MERGE(sc, int32_t, ks_generic_lt)
KSORT2_CHECK(sc, int32_t, ks_generic_lt)

#define MALLOC(type, cnt) ((type*)malloc((cnt) * sizeof(type)))

static inline uint64_t splitmix64(uint64_t x)
{
	uint64_t z = (x += 0x9E3779B97F4A7C15ULL);
	z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
	z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
	return z ^ (z >> 31);
}

double cputime(void)
{
	struct rusage r;
	getrusage(RUSAGE_SELF, &r);
	return r.ru_utime.tv_sec + r.ru_stime.tv_sec + 1e-6 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
}

extern "C" {
int mergesort_alt(void *base, size_t nmemb, size_t size, int (*cmp)(const void *, const void *));
}

int main(int argc, char *argv[])
{
	int32_t i, N = 50000001, *a, *tmp;
	uint64_t x;
	double t;

	a = MALLOC(int32_t, N);
	tmp = MALLOC(int32_t, N);

	for (i = 0, x = 1; i < N; ++i) {
		a[i] = (int32_t)x;
		x = splitmix64(x);
	}
	t = cputime();
	sc_merge_sort(a, a + N, tmp);
	t = cputime() - t;
	printf("merge sort macro [%d]: %lf sec\n", sc_is_sorted(a, a + N), t);

	for (i = 0, x = 1; i < N; ++i) {
		a[i] = (int32_t)x;
		x = splitmix64(x);
	}
	t = cputime();
	merge_sort_void(N, sizeof(*a), a, tmp, int_lt);
	t = cputime() - t;
	printf("merge sort void [%d]: %lf sec\n", sc_is_sorted(a, a + N), t);

	for (i = 0, x = 1; i < N; ++i) {
		a[i] = (int32_t)x;
		x = splitmix64(x);
	}
	t = cputime();
	mergesort_alt(a, N, sizeof(*a), int_cmp);
	t = cputime() - t;
	printf("FreeBSD mergesort void [%d]: %lf sec\n", sc_is_sorted(a, a + N), t);

	for (i = 0, x = 1; i < N; ++i) {
		a[i] = (int32_t)x;
		x = splitmix64(x);
	}
	t = cputime();
	std::stable_sort(a, a + N);
	t = cputime() - t;
	printf("std::stable_sort [%d]: %lf sec\n", sc_is_sorted(a, a + N), t);

	free(tmp); free(a);
	return 0;
}
