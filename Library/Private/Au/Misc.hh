/*
 * Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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

#pragma once

#include "Au/Assert.hh"
#include "Au/Au.hh"

#include <iostream>
#include <limits>
#include <type_traits>
/* Note: This file contains miscellaneous utility functions that are used
 * across the library. The correctness of the inputs to be verified by the
 * caller no validation is done in the functions.
 */
namespace Au {

/**
 * @brief       Extract specified bits from 32-bit value.
 *
 * Extracts length number of bits starting from bit position
 * start in 32-bit value.
 *
 * @param[in]   value  32-bit value.
 * @param[in]   start  Starting bit position.
 * @param[in]   length Number of bits to be extracted.
 *
 * @return      integer Extracted value.
 */
inline Uint32
extract32(Uint32 value, int start, int length)
{
    AUD_ASSERT(start >= 0 && length > 0 && length <= 32 - start,
               "Invalid start/size");

    return (value >> start) & (~0U >> (32 - length));
}

/**
 * @brief        Helper function to convert Enum->Int
 *
 * @return       Integer type of T
 */
template<typename T, class=typename std::enable_if<std::is_enum<T>::value>::type>
inline constexpr auto
operator*(T const e) noexcept
    -> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
{
    return static_cast<std::underlying_type_t<T>>(e);
}

template<typename EnumType, typename UnderlyingType>
UnderlyingType
enumToValue(EnumType enumValue)
{
    if (static_cast<UnderlyingType>(enumValue)
            < (std::numeric_limits<UnderlyingType>::min)()
        || static_cast<UnderlyingType>(enumValue)
               > (std::numeric_limits<UnderlyingType>::max)()) {
        AUD_ASSERT(0,
                   "Enum value out of range for conversion to underlying type");
    }

    return static_cast<UnderlyingType>(enumValue);
}

template<typename EnumType, typename UnderlyingType>
EnumType
valueToEnum(UnderlyingType value)
{
    /*if (value < EnumMin<EnumType> || value > EnumMax<EnumType>)
        return static_cast<EnumType>();*/

    return static_cast<EnumType>(value);
}

} // namespace Au
