#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define REPS (UINT32_MAX / 16)

#define MEASURE(func, reps) do {                                      \
    uint64_t chksum = 0;                                              \
    struct timespec start, end;                                       \
    clock_gettime(CLOCK_MONOTONIC, &start);                           \
    for (uint32_t i = 0; i < reps; ++i)                               \
        chksum += func(values[i]);                                    \
    clock_gettime(CLOCK_MONOTONIC, &end);                             \
    uint64_t result = (end.tv_sec - start.tv_sec) * 1000000000ull + end.tv_nsec - start.tv_nsec; \
    printf("%s checksum: %llu, time: %f ms, %f ns per iteration\n", #func, chksum, result /(double)1000000, result / (double)REPS); \
    } while(0)

#define TEST(func) do {                  \
    assert(func(0) == 0);                \
    assert(func(1) == 1);                \
    assert(func(-1) == 32);              \
    assert(func(5) == 2);                \
    assert(func(0x77777777) == 24);      \
    } while(0)

uint8_t lut[1 << 16];
uint32_t values[REPS];

uint32_t popcnt_naive(uint32_t x) {
    uint32_t res = 0;
    do {
        res += x & 1u;
    } while(x >>= 1);
    return res;
}

uint32_t popcnt_lut8(uint32_t x) {
    uint32_t res = 0;
    res += lut[x & 0xFF];
    res += lut[(x >> 8) & 0xFF];
    res += lut[(x >> 16) & 0xFF];
    res += lut[(x >> 24) & 0xFF];
    return res;
}

uint32_t popcnt_lut16(uint32_t x) {
    uint32_t res = 0;
    res += lut[x & 0xFFFF];
    res += lut[(x >> 16) & 0xFFFF];
    return res;
}

uint32_t popcnt_recursive(uint32_t x) {
    x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
    x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
    x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);
    return x;
}

uint32_t popcnt_asm(uint32_t x) {
    uint32_t count;
    __asm__ volatile (
            "popcnt %1, %0"
            : "=r" (count)
            : "r" (x)
            );
    return count;
}

uint32_t popcnt_kernaghan(uint32_t x) {
    uint32_t res = 0;
    while(x) {
        res++;
        x &= x-1;
    }
    return res;
}

uint32_t ror(uint32_t x, uint32_t rot) {
    return (x >> rot) | (x << (32-rot));
}

uint32_t rol(uint32_t x, uint32_t rot) {
    return (x << rot) | (x >> (32-rot));
}

uint32_t mult(uint32_t a, uint32_t b) {
    uint32_t res = 0;
    for (int i = 0; i < 32; ++i)
        if ((b >> i) & 1)
            res += a << i;
    return res;
}

uint32_t lsb(uint32_t x) {
    return x & ~(x - 1);
}

bool is_pow2(uint32_t x) {
    return x && x == lsb(x);
}

uint32_t mirror(uint32_t x) {
    x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
    x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
    x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
    x = ((x & 0x00FF00FF) << 8) | ((x >> 8) & 0x00FF00FF);
    x = ((x & 0x0000FFFF) << 16) | ((x >> 16) & 0x0000FFFF);
    return x;
}

int main(void) {

    for (uint32_t i = 0; i <= 32; ++i) {
        uint32_t test = i ^ 0xDEADBEEF;
        assert(ror(rol(test,i),i) == test);
    }

    for (uint32_t i = 0; i < 255; ++i) {
        for (uint32_t j = 0; j < 255; ++j) {
            assert(mult(i, j) == i * j);
        }
    }

    for (int i = 0; i < 32; ++i) {
        assert(lsb(0xDEADBEEF << i) == 1u << i);
        assert(lsb(0x43210 << i) == 0x10u << i);
    }

    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 32; ++j) {
            assert(is_pow2((1 << i) | (1 << j)) == (i == j));
        }
    }

    assert(mirror(0xDEADBEEF) == 0xF77DB57B);

    printf("Popcnt comparison:\n");
    for (uint32_t i = 0; i < (1 << 16); ++i) {
        lut[i] = (uint8_t)__builtin_popcount(i);
    }

    TEST(__builtin_popcount);
    TEST(popcnt_naive);
    TEST(popcnt_lut8);
    TEST(popcnt_lut16);
    TEST(popcnt_recursive);
    TEST(popcnt_asm);
    TEST(popcnt_kernaghan);

    printf("Precomputing test values\n");

    srand(42);
    for (uint32_t i = 0; i < REPS; ++i) {
        values[i] = rand() + (rand() << 16);
    }

    printf("Measuring\n");

    MEASURE(__builtin_popcount, REPS);
    MEASURE(popcnt_naive, REPS);
    MEASURE(popcnt_lut8, REPS);
    MEASURE(popcnt_lut16, REPS);
    MEASURE(popcnt_recursive, REPS);
    MEASURE(popcnt_asm, REPS);
    MEASURE(popcnt_kernaghan, REPS);
}
