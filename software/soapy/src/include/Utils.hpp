#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <cstddef>
#include <cmath>

// AXI Core versioning
#define AXI_CORE_VERSION(maj, min, patch)   (((maj) << 16) | ((min) << 8) | (patch))
#define AXI_CORE_VERSION_MAJOR(v)           ((v) >> 16)
#define AXI_CORE_VERSION_MINOR(v)           (((v) >> 8) & 0xFF)
#define AXI_CORE_VERSION_PATCH(v)           ((v) & 0xFF)

#define BIT(x) (1UL << (x))

#define ABS(x)                  ((x) < 0 ? (-(x)) : (x))
#define D_ABS(x, y)             ((x) > (y) ? ((x) - (y)) : ((y) - (x)))
#define MIN(x, y)               (((x) < (y)) ? (x) : (y))
#define MIN_T(t, x, y)          (t)MIN((t)(x), (t)(y))
#define MAX(x, y)               (((x) > (y)) ? (x) : (y))
#define MAX_T(t, x, y)          (t)MAX((t)(x), (t)(y))
#define CLAMP(x, min, max)      (MAX(MIN((x), (max)), (min)))
#define CLAMP_T(t, x, min, max) (t)CLAMP((t)(x), (t)(min), (t)(max))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define DIV_CEIL(x,y)       (((x) + (y) - 1) / (y))
#define DIV_ROUND(x, y)     (((x) + (y) / 2) / (y))
namespace Utils
{
    // Get greatest common divisor of two numbers
    uint64_t GetGCD(uint64_t a, uint64_t b);
    double GetGCD(double a, double b);
    // Get least common multiple of two numbers
    uint64_t GetLCM(uint64_t a, uint64_t b);
    double GetLCM(double a, double b);
    // Get mixed number from two numbers (a / b -> return value + a % b / b)
    uint64_t GetMixedNumber(uint64_t &a, uint64_t &b);
    double GetMixedNumber(double &a, double &b);

    // Find first set bit in word
    uint32_t FindFirstSetBit(uint32_t word);
    // Find last set bit in word
    uint32_t FindLastSetBit(uint32_t word);

    uint64_t DoDiv(uint64_t* n, uint64_t base);

    uint32_t IntSqrt(uint32_t x);
    int32_t Ilog2(int32_t x);

    // Convert to/from Sign-Integer-Fraction 1.1.14 (1-bit signal, 1-bit integer, 1-bit fraction) format
    uint16_t IntToSIF1_1_14(int32_t val); // Fractional part is in micro units
    uint16_t ToSIF1_1_14(double val);
    int32_t IntFromSIF1_1_14(uint16_t val); // Fractional part is in micro units
    double FromSIF1_1_14(uint16_t val);

    // Find closest value (or its index) in vector
    template<typename T>
    size_t FindClosestIndex(std::vector<T> &vec, T value)
    {
        size_t idx = 0;
        T min_diff = std::abs(vec[0] - value);

        for(size_t i = 1; i < vec.size(); i++)
        {
            T diff = std::abs(vec[i] - value);

            if(diff < min_diff)
            {
                min_diff = diff;
                idx = i;
            }
        }

        return idx;
    }
    template<typename T>
    inline size_t FindClosestIndex(const T *vec, size_t size, T value)
    {
        std::vector<T> v(vec, vec + size);

        return Utils::FindClosestIndex(v, value);
    }
    template<typename T>
    T FindClosestValue(std::vector<T> &vec, T value)
    {
        return vec[Utils::FindClosestIndex(vec, value)];
    }
    template<typename T>
    inline T FindClosestValue(const T *vec, size_t size, T value)
    {
        return vec[Utils::FindClosestIndex(vec, size, value)];
    }

    template<typename T>
    std::pair<size_t, size_t> FindLongestSequence(std::vector<T> &vec, T value)
    {
        size_t cnt = 0;
        size_t max_cnt = 0;
        size_t start = 0;
        size_t max_start = 0;

        for(size_t i = 0; i < vec.size(); i++)
        {
            if(vec[i] == value)
            {
                if(cnt == 0)
                    start = i;

                cnt++;
            }
            else
            {
                if(cnt > max_cnt)
                {
                    max_cnt = cnt;
                    max_start = start;
                }

                cnt = 0;
            }
        }

        if(cnt > max_cnt)
        {
            max_cnt = cnt;
            max_start = start;
        }

        return std::make_pair(max_start, max_cnt);
    }
    template<typename T>
    inline std::pair<size_t, size_t> FindLongestSequence(const T *vec, size_t size, T value)
    {
        std::vector<T> v(vec, vec + size);

        return Utils::FindLongestSequence(v, value);
    }
}