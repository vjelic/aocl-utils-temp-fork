/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "Au/ThreadPinning.hh"
#include "Au/ThreadPinning/ThreadPinning.hh"
#include "Au/Types.hh"
#include "gtest/gtest.h"

namespace {
using namespace Au;

typedef std::tuple<String,
                   int,
                   int,
                   std::vector<std::vector<CoreMask>>,
                   std::vector<std::vector<CoreMask>>,
                   std::vector<CoreMask>,
                   std::vector<int>,
                   std::vector<int>,
                   std::vector<int>>
    testParameterType;
class ThreadPinningTest : public ::testing::TestWithParam<testParameterType>
{};

/**
 * @brief                    getCoreMask
 *
 * @details                  Get the core mask for the given core count, group
 * count and hyperthreading status
 *
 * @param coreCount          Number of items in the return vector
 *
 * @param groupCount         group number of the cores
 *
 * @param isHyperThreading   Hyperthreading status
 *
 * @param isLinux            OS type
 *
 * @param groupOffset        group offset
 *
 * @return                   vector of core mask
 *
 */
std::vector<std::vector<CoreMask>>
getCoreMask(int  coreCount,
            int  groupCount,
            bool isHyperThreading,
            bool isLinux,
            int  groupOffset)
{
    std::vector<std::vector<CoreMask>> coreMask;
    if (!isHyperThreading) {
        for (int i = 0; i < coreCount; i++) {
            coreMask.push_back({ std::make_pair(1ULL << i, groupCount) });
        }
        return coreMask;
    } else {
        if (isLinux) {
            for (int i = 0; i < coreCount; i++) {
                coreMask.push_back(
                    { std::make_pair(1ULL << i, groupCount),
                      std::make_pair(1ULL << i, groupCount + groupOffset) });
            }
            return coreMask;
        } else {
            for (int i = 0; i < coreCount; i++) {
                coreMask.push_back(
                    { std::make_pair(0b11ULL << i * 2, groupCount) });
            }
            return coreMask;
        }
    }
}

/**
 * @brief                   concatCoreMasks
 *
 * @details                 Concatenate two core masks
 *
 * @param coremask1         core mask 1
 *
 * @param coremask2         core mask 2
 *
 * @return                  vector of core mask
 */
std::vector<std::vector<CoreMask>>
concatCoreMasks(std::vector<std::vector<CoreMask>> coremask1,
                std::vector<std::vector<CoreMask>> coremask2)
{
    std::vector<std::vector<CoreMask>> coremask;
    coremask.insert(coremask.end(), coremask1.begin(), coremask1.end());
    coremask.insert(coremask.end(), coremask2.begin(), coremask2.end());
    return coremask;
}
// clang-format off
auto testParametersCpuTopology = {
    // Test case 1: No hyperThreading Singlecache number of threads less than
    // number of processors
    std::make_tuple(
        String("Test1"), 8, 2,
        getCoreMask(8, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{ { std::make_pair(0b11111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b11111111, 8) },
        std::vector<int>{ 0, 1 },
        std::vector<int>{ 0, 1 },
        std::vector<int>{ 0, 4 }),

    // Test case 2: No hyperThreading Singlecache number of threads greater than
    // number of processors
    std::make_tuple(
        String("Test2"), 8, 16,
        getCoreMask(8, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{ { std::make_pair(0b11111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b11111111, 8) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 },
        std::vector<int>{ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7 }),

    // Test case 3: No hyperThreading Singlecache number of threads equal to
    // number of processors
    std::make_tuple(
        String("Test3"), 8, 8,
        getCoreMask(8, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{ { std::make_pair(0b11111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b11111111, 8) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7 }),

    // Test case 4: No hyperThreading Multicache number of threads less than
    // number of processors
    std::make_tuple(
        String("Test4"), 32, 15,
        getCoreMask(32, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111111111, 0), },
            { std::make_pair(0b11111111111111110000000000000000, 0), },
        },
        std::vector<CoreMask>{ std::make_pair(0b11111111111111111111111111111111, 32) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 }),

    // Test case 5: No hyperThreading Multicache number of threads greater than
    // number of processors
    std::make_tuple(
        String("Test5"), 16, 35,
        getCoreMask(16, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111, 0), },
            { std::make_pair(0b1111111100000000, 0), },
        },
        std::vector<CoreMask>{ std::make_pair(0b111111111111111, 16) },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8, 9, 10, 11,
                          12, 13, 14, 15, 0,  1,  2,  3,  4, 5, 6,  7,
                          8,  9,  10, 11, 12, 13, 14, 15, 0, 1, 2 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8, 9, 10, 11,
                          12, 13, 14, 15, 0,  1,  2,  3,  4, 5, 6,  7,
                          8,  9,  10, 11, 12, 13, 14, 15, 0, 1, 2 },
        std::vector<int>{ 0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4, 4,
                          5,  5,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10,
                          10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15 }),

    // Test case 6: No HyperThreading Multicache number of threads equal to
    // number of processors
    std::make_tuple(String("Test6"), 16, 16,
        getCoreMask(16, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111, 0), },
            { std::make_pair(0b1111111100000000, 0),},
        },
        std::vector<CoreMask>{ std::make_pair(0b111111111111111, 16) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }),

    // Test case 7: No HyperThreading Multicache multicore group number of
    // threads less than number of processors
    std::make_tuple(
        String("Test7"), 96, 15,
        concatCoreMasks(getCoreMask(64, 0, false, false, 0),
                        getCoreMask(32, 1, false, false, 64)),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 0)},
            { std::make_pair((1ULL << 32) - 1, 1)}
        },
        std::vector<CoreMask>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 64)},
            std::make_pair((1ULL << 32) - 1, 32)
        },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 68, 72, 76, 80, 84, 88 }),

    // Test case 8: No HyperThreading Multicache multicore group number of
    // threads greater than number of processors
    std::make_tuple(
        String("Test8"), 96, 100,
        concatCoreMasks(getCoreMask(64, 0, false, false, 0),
                        getCoreMask(32, 1, false, false, 64)),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 0)},
            { std::make_pair((1ULL << 32) - 1, 1)}
        },
        std::vector<CoreMask>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 64)},
            std::make_pair((1ULL << 32) - 1, 32)
        },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
            85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 0,  1,  2,  3 },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
            85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 0,  1,  2,  3 },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  8,  9,  10, 12, 13, 14, 16, 17, 18, 20,
            21, 22, 24, 25, 26, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38, 40, 41,
            42, 44, 45, 46, 48, 49, 50, 52, 53, 54, 56, 57, 58, 60, 61, 62, 64,
            64, 65, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74,
            75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85,
            86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95 }),

    // Test case 9:  No HyperThreading Multicache multicore group number of
    // threads equal to number of processors
    std::make_tuple(
        String("Test9"), 96, 96,
        concatCoreMasks(getCoreMask(64, 0, false, false, 0),
                        getCoreMask(32, 1, false, false, 64)),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 0)},
            { std::make_pair((1ULL << 32) - 1, 1)}
        },
        std::vector<CoreMask>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 64)},
            std::make_pair((1ULL << 32) - 1, 32)
        },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
            85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95},
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
            85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95},
        std::vector<int>{
            0 , 1 , 2 , 4 , 5 , 6 , 8 , 9 , 10, 12, 13, 14, 16, 17, 18, 20, 21,
            22, 24, 25, 26, 28, 29, 30, 32, 33, 34, 36, 37, 38, 40, 41, 42, 44,
            45, 46, 48, 49, 50, 52, 53, 54, 56, 57, 58, 60, 61, 62, 64, 64, 65,
            66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 76, 76,
            77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 86, 87, 88,
            88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95}),
    // Test case 10: No HyperThreading Singlecache multicore group number of
    // threads less than number of processors
    /** The current facilities in windows indicates that an L3 cache is shared by
     * maximum of 64 cores, see GroupMask.Mask from _CACHE_RELATIONSHIP structure
     * for now we will test core group containing 16 cores each. for all of them
     * sharing same cache
     */
    std::make_tuple(
        String("Test 10"), 32, 15,
        concatCoreMasks(getCoreMask(16, 0, false, false, 0),
                        getCoreMask(16, 1, false, false, 0)),
        std::vector<std::vector<CoreMask>>{ { std::make_pair((1ULL << 32) - 1, 0) } },
        std::vector<CoreMask>{ std::make_pair((1ULL << 16) - 1, 16),
                               std::make_pair((1ULL << 16) - 1, 16) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 }),

    // Test case 11: No HyperThreading Singlecache multicore group number of
    // threads greater than number of processors
    std::make_tuple(
        String("Test11"), 32, 35,
        concatCoreMasks(getCoreMask(16, 0, false, false, 0),
                        getCoreMask(16, 1, false, false, 0)),
        std::vector<std::vector<CoreMask>>{ { std::make_pair((1ULL << 32) - 1, 0) } },
        std::vector<CoreMask>{ std::make_pair((1ULL << 16) - 1, 16),
                               std::make_pair((1ULL << 16) - 1, 16) },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31, 0,  1,  2 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31, 0,  1,  2 },
        std::vector<int>{ 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,
                          10, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20,
                          21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 }),

    // Test case 12: No HyperThreading Singlecache multicore group number of
    // threads equal to number of processors
    std::make_tuple(
        String("Test12"), 32, 32,
        concatCoreMasks(getCoreMask(16, 0, false, false, 0),
                        getCoreMask(16, 1, false, false, 0)),
        std::vector<std::vector<CoreMask>>{ { std::make_pair((1ULL << 32) - 1, 0) } },
        std::vector<CoreMask>{ std::make_pair((1ULL << 16) - 1, 16),
                               std::make_pair((1ULL << 16) - 1, 16) },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31},
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31},
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31}),

    // Test case 13: HyperThreading Singlecache number of threads less than
    // number of processors
    std::make_tuple(
        String("Test13"), 16, 9,
        getCoreMask(8, 0, true, false, 0),
        std::vector<std::vector<CoreMask>>{{ std::make_pair((1ULL << 16) - 1, 0) } },
        std::vector<CoreMask>{ std::make_pair((1ULL << 16) - 1, 16) },
        std::vector<int>{ 0, 2, 4, 6, 8, 10, 12, 14, 1 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 },
        std::vector<int>{ 0, 1, 2, 4, 6, 8, 10, 12, 14 }),

    // Test case 14: HyperThreading Singlecache number of threads greater than
    // number of processors
    std::make_tuple(
        String("Test14"), 16, 18,
        getCoreMask(8, 0, true, false, 0),
        std::vector<std::vector<CoreMask>>{ { std::make_pair((1ULL << 16) - 1, 0) } },
        std::vector<CoreMask>{ std::make_pair((1ULL << 16) - 1, 16) },
        std::vector<int>{ 0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15, 0, 2 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1 },
        std::vector<int>{ 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 15 }),

    // Test case 15: HyperThreading Singlecache number of threads equal to
    // number of processors
    std::make_tuple(
        String("Test15"), 16, 16,
        getCoreMask(8, 0, true, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }),

    // Test case 16: HyperThreading Multicache Singlecore group number of
    // threads less than number of processors
    std::make_tuple(
        String("Test16"), 32, 15,
        getCoreMask(32, 0, true, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111111111, 0), },
            { std::make_pair(0b11111111111111110000000000000000, 0), },
        },
        std::vector<CoreMask>{ std::make_pair(0b11111111111111111111111111111111, 32) },
        std::vector<int>{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 }),

    // Test case 17: HyperThreading Multicache Singlecore group number of
    // threads greater than number of processors
    std::make_tuple(
        String("Test17"), 16, 35,
        getCoreMask(8, 0, true, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111, 0),},
            { std::make_pair(0b1111111100000000, 0), },
        },
        std::vector<CoreMask>{ std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0, 2,  4,  6,  8, 10, 12, 14, 1, 3,  5,  7,
                          9, 11, 13, 15, 0, 2,  4,  6,  8, 10, 12, 14,
                          1, 3,  5,  7,  9, 11, 13, 15, 0, 2,  4 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8, 9, 10, 11,
                          12, 13, 14, 15, 0,  1,  2,  3,  4, 5, 6,  7,
                          8,  9,  10, 11, 12, 13, 14, 15, 0, 1, 2 },
        std::vector<int>{ 0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4, 4,
                          5,  5,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10,
                          10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15 }),

    // Test case 18: HyperThreading Multicache Singlecore group number of
    // threads equal to number of processors
    std::make_tuple(
        String("Test18"), 16, 16,
        getCoreMask(8, 0, true, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111, 0), },
            { std::make_pair(0b1111111100000000, 0), }
        },
        std::vector<CoreMask>{ std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }),

    // Test case 19: HyperThreading Multicache multicore group number of threads
    // less than number of processors
    std::make_tuple(
        String("Test19"), 96, 15,
        concatCoreMasks(getCoreMask(32, 0, true, false, 0),
                        getCoreMask(16, 1, true, false, 0)),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 0)},
            { std::make_pair((1ULL << 32) - 1, 1) } },
        std::vector<CoreMask>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 64)},
            std::make_pair((1ULL << 32) - 1, 32) },
        std::vector<int>{
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{
            0, 8, 16, 24, 32, 40, 48, 56, 64, 68, 72, 76, 80, 84, 88 }),

    // Test case 20: HyperThreading Multicache multicore group number of threads
    // greater than number of processors
    std::make_tuple(
        String("Test20"), 96, 100,
        concatCoreMasks(getCoreMask(32, 0, true, false, 0),
                        getCoreMask(16, 1, true, false, 0)),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 0)},
            { std::make_pair((1ULL << 32) - 1, 1) } },
        std::vector<CoreMask>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 64)},
            std::make_pair((1ULL << 32) - 1, 32) },
        std::vector<int>{
            0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
            34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66,
            68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 1,  3,  5,
            7,  9,  11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39,
            41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73,
            75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 0,  2,  4,  6 },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
            85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 0,  1,  2,  3 },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  8,  9,  10, 12, 13, 14, 16, 17, 18, 20,
            21, 22, 24, 25, 26, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38, 40, 41,
            42, 44, 45, 46, 48, 49, 50, 52, 53, 54, 56, 57, 58, 60, 61, 62, 64,
            64, 65, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74,
            75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85,
            86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95 }),

    // Test case 21: HyperThreading Multicache multicore group number of threads
    // equal to number of processors
    std::make_tuple(
        String("Test21"), 96, 96,
        concatCoreMasks(getCoreMask(32, 0, true, false, 0),
                        getCoreMask(16, 1, true, false, 0)),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 0)},
            { std::make_pair((1ULL << 32) - 1, 1) } },
        std::vector<CoreMask>{
            { std::make_pair((((1ULL << 63) - 1) << 1) | 1, 64)},
            std::make_pair((1ULL << 32) - 1, 32) },
        std::vector<int>{
            0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
            34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66,
            68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 1,  3,  5,
            7,  9,  11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39,
            41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73,
            75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95},
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
            85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95},
        std::vector<int>{
            0 , 1 , 2 , 4 , 5 , 6 , 8 , 9 , 10, 12, 13, 14, 16, 17, 18, 20, 21,
            22, 24, 25, 26, 28, 29, 30, 32, 33, 34, 36, 37, 38, 40, 41, 42, 44,
            45, 46, 48, 49, 50, 52, 53, 54, 56, 57, 58, 60, 61, 62, 64, 64, 65,
            66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 76, 76,
            77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 86, 87, 88,
            88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95}),

    // Test case 22: HyperThreading Singlecache multicore group number of
    // threads less than number of processors
    std::make_tuple(
        String("Test22"), 32, 30,
        concatCoreMasks(getCoreMask(8, 0, true, false, 0),
                        getCoreMask(8, 1, true, false, 0)),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111111111111111111111111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b1111111111111111, 16),
                               std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0,  2,  4,  6,  8,  10, 12, 14, 16, 18,
                          20, 22, 24, 26, 28, 30, 1,  3,  5,  7,
                          9,  11, 13, 15, 17, 19, 21, 23, 25, 27 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                          10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                          20, 21, 22, 23, 24, 25, 26, 27, 28, 29 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                          10, 11, 12, 13, 14, 16, 17, 18, 19, 20,
                          21, 22, 23, 24, 25, 26, 27, 28, 29, 30 }),

    // Test case 23: HyperThreading Singlecache multicore group number of
    // threads greater than number of processors
    std::make_tuple(
        String("Test23"), 32, 35,
        concatCoreMasks(getCoreMask(8, 0, true, false, 0),
                        getCoreMask(8, 1, true, false, 0)),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111111111111111111111111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b1111111111111111, 16),
                               std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22,
                          24, 26, 28, 30, 1,  3,  5,  7,  9,  11, 13, 15,
                          17, 19, 21, 23, 25, 27, 29, 31, 0,  2,  4 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31, 0,  1,  2 },
        std::vector<int>{ 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,
                          10, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20,
                          21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 }),

    // Test case 24: HyperThreading Singlecache multicore group number of
    // threads equal to number of processors
    std::make_tuple(
        String("Test23"), 32, 32,
        concatCoreMasks(getCoreMask(8, 0, true, false, 0),
                        getCoreMask(8, 1, true, false, 0)),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111111111111111111111111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b1111111111111111, 16),
                               std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22,
                          24, 26, 28, 30, 1,  3,  5,  7,  9,  11, 13, 15,
                          17, 19, 21, 23, 25, 27, 29, 31},
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31},
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31}),

    // Test case 25: No HyperThreading multicache uneven distribution single
    // coregroup. Number of threads less than number of processors
    std::make_tuple(
        String("Test25"), 15, 2,
        getCoreMask(15, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111, 0), },
            { std::make_pair(0b111110000000000, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b111111111111111, 15) },
        std::vector<int>{ 0, 1 },
        std::vector<int>{ 0, 1 },
        std::vector<int>{ 0, 10 }),
    std::make_tuple(
        String("Test25b"), 15, 7,
        getCoreMask(15, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111, 0) },
            { std::make_pair(0b111110000000000, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b111111111111111, 15) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6 },
        std::vector<int>{ 0, 3, 5, 8, 10, 12, 13 }),

    // Test case 26: No HyperThreading multicache uneven distribution single
    // coregroup. Number of threads greater than number of processors
    std::make_tuple(
        String("Test26"), 15, 18,
        getCoreMask(15, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b111111111111, 0), },
            { std::make_pair(0b111000000000000, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b111111111111111, 15) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 1, 2 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 1, 2 },
        std::vector<int>{ 0, 1, 2, 3, 5, 6, 8, 9, 11, 12, 12, 12, 13, 13, 14, 14, 14, 14 }),

    // Test case 27: No HyperThreading multicache uneven distribution single
    // coregroup. Number of threads equal to number of processors
    std::make_tuple(
        String("Test27"), 15, 15,
        getCoreMask(15, 0, false, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111, 0) },
            { std::make_pair(0b111110000000000, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b111111111111111, 15) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12, 12, 13, 13, 14 }),

    // Test case 28: HyperThreading multicache uneven distribution single
    // coregroup. Number of threads less than number of processors
    std::make_tuple(String("Test28"), 30, 2,
         getCoreMask(15, 0, true, false, 0),
         std::vector<std::vector<CoreMask>>{
             { std::make_pair(0b1111111111, 0),},
             { std::make_pair(0b111111111111111111110000000000, 0) },
         },
         std::vector<CoreMask>{ std::make_pair(0b1111111111111111111111111111111, 30) },
         std::vector<int>{ 0, 2 },
         std::vector<int>{ 0, 1 },
         std::vector<int>{ 0, 10 }),

    // Test case 29: HyperThreading multicache uneven distribution single
    // coregroup. Number of threads greater than number of processors
    std::make_tuple(
        String("Test29"), 30, 32,
        getCoreMask(15, 0, true, false, 0),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111, 0), },
            { std::make_pair(0b111111111111111111110000000000, 0) },
        },
        std::vector<CoreMask>{
            std::make_pair(0b1111111111111111111111111111111, 30) },
        std::vector<int>{ 0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20,
                          22, 24, 26, 28, 1,  3,  5,  7,  9,  11, 13,
                          15, 17, 19, 21, 23, 25, 27, 29, 0,  2 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                          11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                          22, 23, 24, 25, 26, 27, 28, 29, 0,  1 },
        std::vector<int>{ 0,  1,  2,  2,  3,  3,  4,  4,  5,  6,  7,
                          7,  8,  8,  9,  9,  10, 12, 13, 14, 15, 17,
                          18, 19, 20, 22, 23, 24, 25, 27, 28, 29 }),

    // Test case 30: HyperThreading multicache uneven distribution single
    // coregroup. Number of threads equal to number of processors
    std::make_tuple(String("Test30"), 30, 30,
         getCoreMask(15, 0, true, false, 0),
         std::vector<std::vector<CoreMask>>{
             { std::make_pair(0b1111111111, 0), },
             { std::make_pair(0b111111111111111111110000000000, 0) },
         },
         std::vector<CoreMask>{
             std::make_pair(0b1111111111111111111111111111111, 30) },
         std::vector<int>{ 0,  2,  4,  6,  8,  10, 12, 14, 16, 18,
                           20, 22, 24, 26, 28, 1,  3,  5,  7,  9,
                           11, 13, 15, 17, 19, 21, 23, 25, 27, 29 },
         std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                           10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                           20, 21, 22, 23, 24, 25, 26, 27, 28, 29 },
         std::vector<int>{ 0,  1,  2,  2,  3,  3,  4,  4,  5,  6,
                           7,  7,  8,  8,  9,  10, 12, 13, 14, 15,
                           17, 18, 19, 20, 22, 23, 24, 25, 27, 28 }),


    // # ***************Tests targetted for linux systems******************** #/
    // Test case 31: HyperThreading singlecache nonneighbouring siblings and
    // total number of cores less than 32.
    // Number of threads greater than number of processors
    std::make_tuple(
        String("Test31"), 12, 15,
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1000001, 0), },
            { std::make_pair(0b10000010, 0), },
            { std::make_pair(0b100000100, 0), },
            { std::make_pair(0b1000001000, 0), },
            { std::make_pair(0b10000010000, 0), },
            { std::make_pair(0b100000100000, 0), },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b111111111111, 0) },
        },
        std::vector<CoreMask>{ std::make_pair(0b111111111111, 12) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2 },
        std::vector<int>{ 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11 }),

    // Test case 31: HyperThreading singlecache nonneighbouring siblings and
    // total number of cores less than 32.
    // Number of threads less than number of processors
    std::make_tuple(
        String("Test32"), 12, 10,
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1000001, 0), },
            { std::make_pair(0b10000010, 0), },
            { std::make_pair(0b100000100, 0), },
            { std::make_pair(0b1000001000, 0), },
            { std::make_pair(0b10000010000, 0), },
            { std::make_pair(0b100000100000, 0), },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b111111111111, 0) },
        },
        std::vector<CoreMask>{ std::make_pair(0b111111111111, 12) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        std::vector<int>{ 0, 1, 2, 3, 5, 6, 7, 8, 9, 11 }),

    // Test case 33: HyperThreading singlecache nonneighbouring siblings and
    // total number of cores less than 32.
    // Number of threads equal to number of processors
    std::make_tuple(
        String("Test33"), 12, 12,
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1000001, 0),},
            { std::make_pair(0b10000010, 0),},
            { std::make_pair(0b100000100, 0),},
            { std::make_pair(0b1000001000, 0),},
            { std::make_pair(0b10000010000, 0),},
            { std::make_pair(0b100000100000, 0),},
        },
        std::vector<std::vector<CoreMask>>{ { std::make_pair(0b111111111111, 0)},},
        std::vector<CoreMask>{ std::make_pair(0b111111111111, 12) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }),

    std::make_tuple(
        String("Test34"), 12, 9,
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1000001, 0),},
            { std::make_pair(0b10000010, 0),},
            { std::make_pair(0b100000100, 0),},
            { std::make_pair(0b1000001000, 0),},
            { std::make_pair(0b10000010000, 0),},
            { std::make_pair(0b100000100000, 0),},
        },
        std::vector<std::vector<CoreMask>>{ { std::make_pair(0b111111111111, 0)},},
        std::vector<CoreMask>{ std::make_pair(0b111111111111, 12) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 },
        std::vector<int>{ 0, 1, 2, 3, 5, 6, 8, 9, 11 }),

    std::make_tuple(
        String("Test35"), 12, 16,
        getCoreMask(6, 0, true, false, 0),
        std::vector<std::vector<CoreMask>>{ { std::make_pair(0b111111111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b11111111111111, 12) },
        std::vector<int>{ 0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3 },
        std::vector<int>{ 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 11 }),
    /*
     * Test case 42 : lib-genoa-09
     * CPU(s):                  384
     * On-line CPU(s) list:   0-191
     * Off-line CPU(s) list:  192-383
     * Vendor ID:               AuthenticAMD
     * Model name:            AMD Eng Sample: 100-000000999
     * CPU family:          25
     * Model:               17
     * Thread(s) per core:  1
     * Core(s) per socket:  96
     * Socket(s):           2
     * Stepping:            1
     * Caches (sum of all):
     * L1d:                   6 MiB (192 instances)
     * L1i:                   6 MiB (192 instances)
     * L2:                    192 MiB (192 instances)
     * L3:                    768 MiB (24 instances)
     */
    std::make_tuple(
        String("Test38: lib-genoa-09"), 192, 16,
        concatCoreMasks(
        concatCoreMasks(
        concatCoreMasks(getCoreMask(32, 0, false, false, 0),
                        getCoreMask(32, 1, false, false, 1)),
        concatCoreMasks(getCoreMask(32, 0, false, false, 2),
                        getCoreMask(32, 1, false, false, 3))),
        concatCoreMasks(getCoreMask(32, 0, false, false, 4),
                        getCoreMask(32, 1, false, false, 5))),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b00000000000000000000000011111111, 0) },
            { std::make_pair(0b00000000000000001111111100000000, 0) },
            { std::make_pair(0b00000000111111110000000000000000, 0) },
            { std::make_pair(0b11111111000000000000000000000000, 0) },
            { std::make_pair(0b00000000000000000000000011111111, 1) },
            { std::make_pair(0b00000000000000001111111100000000, 1) },
            { std::make_pair(0b00000000111111110000000000000000, 1) },
            { std::make_pair(0b11111111000000000000000000000000, 1) },
            { std::make_pair(0b00000000000000000000000011111111, 2) },
            { std::make_pair(0b00000000000000001111111100000000, 2) },
            { std::make_pair(0b00000000111111110000000000000000, 2) },
            { std::make_pair(0b11111111000000000000000000000000, 2) },
            { std::make_pair(0b00000000000000000000000011111111, 3) },
            { std::make_pair(0b00000000000000001111111100000000, 3) },
            { std::make_pair(0b00000000111111110000000000000000, 3) },
            { std::make_pair(0b11111111000000000000000000000000, 3) },
            { std::make_pair(0b00000000000000000000000011111111, 4) },
            { std::make_pair(0b00000000000000001111111100000000, 4) },
            { std::make_pair(0b00000000111111110000000000000000, 4) },
            { std::make_pair(0b11111111000000000000000000000000, 4) },
            { std::make_pair(0b00000000000000000000000011111111, 5) },
            { std::make_pair(0b00000000000000001111111100000000, 5) },
            { std::make_pair(0b00000000111111110000000000000000, 5) },
            { std::make_pair(0b11111111000000000000000000000000, 5) },
        },
        std::vector<CoreMask>{
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
        },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 16, 24, 40, 48, 64, 72, 88, 96, 112, 120, 136, 144, 160, 168, 184 }),
    std::make_tuple(
        String("Test43: lib-genoa-09-tweak-cachegroups"), 192, 16,
        concatCoreMasks(
        concatCoreMasks(
        concatCoreMasks(getCoreMask(32, 0, false, false, 0),
                        getCoreMask(32, 1, false, false, 1)),
        concatCoreMasks(getCoreMask(32, 0, false, false, 2),
                        getCoreMask(32, 1, false, false, 3))),
        concatCoreMasks(getCoreMask(32, 0, false, false, 4),
                        getCoreMask(32, 1, false, false, 5))),
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b00000000000000000000000011111111, 0),
              std::make_pair(0b00000000000000000000000011111111, 3) },
            { std::make_pair(0b00000000000000001111111100000000, 0),
              std::make_pair(0b00000000000000001111111100000000, 3) },
            { std::make_pair(0b00000000111111110000000000000000, 0),
              std::make_pair(0b00000000111111110000000000000000, 3) },
            { std::make_pair(0b11111111000000000000000000000000, 0),
              std::make_pair(0b11111111000000000000000000000000, 3) },
            { std::make_pair(0b00000000000000000000000011111111, 1),
              std::make_pair(0b00000000000000000000000011111111, 4) },
            { std::make_pair(0b00000000000000001111111100000000, 1),
              std::make_pair(0b00000000000000001111111100000000, 4) },
            { std::make_pair(0b00000000111111110000000000000000, 1),
              std::make_pair(0b00000000111111110000000000000000, 4) },
            { std::make_pair(0b11111111000000000000000000000000, 1),
              std::make_pair(0b11111111000000000000000000000000, 4) },
            { std::make_pair(0b00000000000000000000000011111111, 2),
              std::make_pair(0b00000000000000000000000011111111, 5) },
            { std::make_pair(0b00000000000000001111111100000000, 2),
              std::make_pair(0b00000000000000001111111100000000, 5) },
            { std::make_pair(0b00000000111111110000000000000000, 2),
              std::make_pair(0b00000000111111110000000000000000, 5) },
            { std::make_pair(0b11111111000000000000000000000000, 2),
              std::make_pair(0b11111111000000000000000000000000, 5) },
        },
        std::vector<CoreMask>{
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
        },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 8, 16, 112, 24, 32, 40, 136, 48, 56, 64, 160, 72, 80, 88, 184 }),
    std::make_tuple(
        String("Test44: lib-genoa-10"), 512, 16,
        concatCoreMasks(
        concatCoreMasks(
        concatCoreMasks(getCoreMask(16, 0, true, true, 8),
                        getCoreMask(16, 1, true, true, 8)
                        ),
        concatCoreMasks(getCoreMask(16, 2, true, true, 8),
                         getCoreMask(16, 3, true, true, 8)
                        )
                        ),
        concatCoreMasks(
        concatCoreMasks(getCoreMask(16, 4, true, true, 8),
                        getCoreMask(16, 5, true, true, 8)
                        ),
        concatCoreMasks(getCoreMask(16, 6, true, true, 8),
                         getCoreMask(16, 7, true, true, 8)
                        )
                        )
                        ),
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b00000000000000000000000011111111, 0),
                std::make_pair(0b00000000000000000000000011111111, 8),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 0),
                std::make_pair(0b00000000000000001111111100000000, 8),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 0),
                std::make_pair(0b00000000111111110000000000000000, 8),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 0),
                std::make_pair(0b11111111000000000000000000000000, 8),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 1),
                std::make_pair(0b00000000000000000000000011111111, 9),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 1),
                std::make_pair(0b00000000000000001111111100000000, 9),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 1),
                std::make_pair(0b00000000111111110000000000000000, 9),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 1),
                std::make_pair(0b11111111000000000000000000000000, 9),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 2),
                std::make_pair(0b00000000000000000000000011111111, 10),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 2),
                std::make_pair(0b00000000000000001111111100000000, 10),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 2),
                std::make_pair(0b00000000111111110000000000000000, 10),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 2),
                std::make_pair(0b11111111000000000000000000000000, 10),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 3),
                std::make_pair(0b00000000000000000000000011111111, 11),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 3),
                std::make_pair(0b00000000000000001111111100000000, 11),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 3),
                std::make_pair(0b00000000111111110000000000000000, 11),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 3),
                std::make_pair(0b11111111000000000000000000000000, 11),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 4),
                std::make_pair(0b00000000000000000000000011111111, 12),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 4),
                std::make_pair(0b00000000000000001111111100000000, 12),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 4),
                std::make_pair(0b00000000111111110000000000000000, 12),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 4),
                std::make_pair(0b11111111000000000000000000000000, 12),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 5),
                std::make_pair(0b00000000000000000000000011111111, 13),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 5),
                std::make_pair(0b00000000000000001111111100000000, 13),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 5),
                std::make_pair(0b00000000111111110000000000000000, 13),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 5),
                std::make_pair(0b11111111000000000000000000000000, 13),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 6),
                std::make_pair(0b00000000000000000000000011111111, 14),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 6),
                std::make_pair(0b00000000000000001111111100000000, 14),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 6),
                std::make_pair(0b00000000111111110000000000000000, 14),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 6),
                std::make_pair(0b11111111000000000000000000000000, 14),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 7),
                std::make_pair(0b00000000000000000000000011111111, 15),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 7),
                std::make_pair(0b00000000000000001111111100000000, 15),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 7),
                std::make_pair(0b00000000111111110000000000000000, 15),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 7),
                std::make_pair(0b11111111000000000000000000000000, 15),
            },
        },
        std::vector<CoreMask>{
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
        },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224,240 }),
};
// clang-format on
} // namespace
