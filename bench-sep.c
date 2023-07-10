#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#ifdef __cplusplus
#include <algorithm>
#endif

#ifndef STEP0
#define STEP0 1
#endif

#define KSORT2_CHECK(prefix, type_t, __sort_lt) \
	int prefix##_is_sorted(type_t *st, type_t *en) \
	{ \
		type_t *p; \
		for (p = st + 1; p < en; ++p) \
			if (__sort_lt(*p, *(p-1))) \
				return 0; \
		return 1; \
	}

#define KSORT2_INSERTION(prefix, type_t, __sort_lt) \
	void prefix##_insertion_sort(type_t *beg, type_t *end) \
	{ \
		type_t *i; \
		for (i = beg + 1; i < end; ++i) { \
			if (__sort_lt(*i, *(i-1))) { \
				type_t *j, tmp = *i; \
				for (j = i; j > beg && __sort_lt(tmp, *(j-1)); --j) \
					*j = *(j - 1); \
				*j = tmp; \
			} \
		} \
	}

#define KSORT2_MERGE(prefix, type_t, __sort_lt) \
	void prefix##_merge_sort(type_t *st, type_t *en, type_t *tmp) \
	{ \
		type_t *a2[2], *a, *b; \
		size_t n = en - st, step = STEP0; \
		int curr; \
		if (step > 1) { /* insertion sort to replace first few rounds of merge sort */ \
			for (a = st; a < en - step; a += step) \
				prefix##_insertion_sort(a, a + step); \
			prefix##_insertion_sort(a, en); \
		} \
		a2[0] = st, a2[1] = tmp; \
		for (curr = 0; step < n; step += step) { \
			size_t i; \
			a = a2[curr]; b = a2[1-curr]; \
			for (i = 0; i < n; i += step<<1) { \
				type_t *p, *j, *k; \
				type_t *ea = n < i + step? a + n : a + i + step; \
				type_t *eb = n < i + step? a : a + (n < i + (step<<1)? n : i + (step<<1)); \
				j = a + i; k = a + i + step; p = b + i; \
				while (j < ea && k < eb) { \
					if (__sort_lt(*k, *j)) *p++ = *k++; \
					else *p++ = *j++; \
				} \
				while (j < ea) *p++ = *j++; \
				while (k < eb) *p++ = *k++; \
			} \
			curr = 1 - curr; \
		} \
		if (curr == 1) memcpy(st, tmp, n * sizeof(*st)); \
	}

#define ks_generic_lt(a, b) ((a) < (b))
KSORT2_INSERTION(sc, int32_t, ks_generic_lt)
KSORT2_MERGE(sc, int32_t, ks_generic_lt)
