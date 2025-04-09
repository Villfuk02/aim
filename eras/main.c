#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TEST(func, num) do {                  \
    assert(func(0) == 0);                \
    assert(func(1) == 0);                \
    assert(func(2) == 1);              \
    assert(func(5) == 3);              \
    assert(func(10) == 4);              \
    assert(func(100) == 25);                \
    assert(func(1000) == 168);           \
    assert(func(10000) == 1229);           \
    assert(func(1000000) == 78498);           \
    assert(func(1000000000) == 50847534);           \
    struct timespec start, end;                                       \
    clock_gettime(CLOCK_MONOTONIC, &start);                           \
    uint64_t res = func(num);                                    \
    clock_gettime(CLOCK_MONOTONIC, &end);                             \
    uint64_t result = (end.tv_sec - start.tv_sec) * 1000000000ull + end.tv_nsec - start.tv_nsec; \
    printf("%s result: %llu, time: %f ms\n", #func, res, result /(double)1000000); \
    } while(0)


void set_bit(uint32_t field[], uint64_t index) {
    field[index / 32] |= 1 << (index % 32);
}
bool get_bit(const uint32_t field[], uint64_t index) {
    return field[index/32] & (1 << (index % 32));
}
uint32_t count_bits(const uint32_t field[], uint64_t arr_size) {
    uint32_t res = 0;
    for (uint64_t i = 0; i < arr_size; ++i)
        res += __builtin_popcount(field[i]);
    return res;
}

uint32_t eras_bitfield(uint64_t limit) {
    if(limit <= 1)
        return 0;

    uint64_t arr_size = limit / 32 + 1;
    uint32_t* field = malloc(sizeof(uint32_t) * arr_size);
    if(field == NULL)
        exit(1);

    memset (field, 0, sizeof (uint32_t) * arr_size);

    uint64_t prime = 2;
    while(prime * prime <= limit) {
        for (uint64_t i = 2 * prime; i <= limit; i += prime)
            set_bit(field, i);
        do
            prime++;
        while (get_bit(field, prime));
    }

    uint32_t set_bits = count_bits(field, arr_size);
    free(field);
    return limit - 1 - set_bits;
}

uint32_t eras_bitfield2(uint64_t limit) {
    if (limit <= 1)
        return 0;
    if (limit == 2)
        return 1;

    uint64_t arr_size = limit / 64 + 1;
    uint32_t* field = malloc(sizeof(uint32_t) * arr_size);
    if(field == NULL)
        exit(1);

    memset (field, 0, sizeof (uint32_t) * arr_size);

    uint64_t prime = 3;
    while(prime * prime <= limit) {
        for (uint64_t i = prime * prime / 2; i < limit / 2; i += prime)
            set_bit(field, i);
        do
            prime += 2;
        while (get_bit(field, prime / 2));
    }

    uint64_t set_bits = count_bits(field, arr_size);
    free(field);
    return (limit + 1) / 2 - set_bits;
}

int main(void) {

    TEST(eras_bitfield2, 1000000000);
    TEST(eras_bitfield, 1000000000);
    return 0;
}
