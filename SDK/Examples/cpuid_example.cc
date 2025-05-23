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

#include "Au/Cpuid/CacheInfo.hh"
#include "Au/Cpuid/X86Cpu.hh"
#include "Au/Memory/BufferView.hh"

#include <iostream>

using namespace Au;

static void
display_core_info()
{
    X86Cpu cpu{ 0 };

    if (cpu.isAMD()) {
        std::cout << "AMD CPU detected..." << std::endl;
    } else if (cpu.isIntel()) {
        std::cout << "Intel CPU detected..." << std::endl;
    } else {
        std::cout << "Unknown CPU detected..." << std::endl;
    }

    std::cout << "\nGetting CPU information for core 0" << std::endl;
    auto vInfo = cpu.getVendorInfo();
    std::cout << "Vendor   : " << static_cast<Uint32>(vInfo.m_mfg) << std::endl;
    std::cout << "Family   : " << static_cast<Uint32>(vInfo.m_family)
              << std::endl;
    std::cout << "Model    : " << vInfo.m_model << std::endl;
    std::cout << "Stepping : " << vInfo.m_stepping << std::endl;
    std::cout << "Uarch    : " << static_cast<Uint32>(vInfo.m_uarch)
              << std::endl;
}

static void
display_isa_info()
{
    X86Cpu cpu{ 0 };

    std::cout << "\nGetting ISA information for core 0" << std::endl;
    if (cpu.isX86_64v4()) {
        std::cout << "CPU supports x86-64v4 ISA" << std::endl;
    }
    if (cpu.isX86_64v3()) {
        std::cout << "CPU supports x86-64v3 ISA" << std::endl;
    }
    if (cpu.isX86_64v2()) {
        std::cout << "CPU supports x86-64v2 ISA" << std::endl;
    } else {
        std::cout << "CPU does not support x86-64v2 or higher ISA" << std::endl;
    }

    if (cpu.isZenFamily()) {
        std::cout << "CPU belongs to Zen family" << std::endl;
    } else {
        std::cout << "CPU does not belongs to Zen family" << std::endl;
    }

    ECpuidFlag   flags[]    = { ECpuidFlag::tsc_adjust,
                                ECpuidFlag::avx,
                                ECpuidFlag::avxvnni,
                                ECpuidFlag::avx2,
                                ECpuidFlag::avx512f };
    const size_t flags_size = sizeof(flags) / sizeof(flags[0]);
    Au::Memory::BufferView<ECpuidFlag> flags_view{ flags, flags_size };

    std::cout << std::endl << "Flags selected" << std::endl;
    // Print selected flags
    for (auto flag : flags_view) {
        std::cout << "Flag : " << ECpuidFlagtoString(static_cast<Uint32>(flag))
                  << " ";
    }
    std::cout << std::endl;

    std::cout << "Cpu Flag Group Check..." << std::endl;
    std::cout << "Has all flags : "
              << (cpu.hasFlags(flags_view, Au::HasFlagsMode::All) ? "yes"
                                                                  : "no")
              << std::endl;
    std::cout << "Has any flag  : "
              << (cpu.hasFlags(flags_view, Au::HasFlagsMode::Any) ? "yes"
                                                                  : "no")
              << std::endl;

    std::cout << "\nCpu Feature Identification..." << std::endl;
    // clang-format off
    std::cout << "TSC_ADJUST : " << (cpu.hasFlag(ECpuidFlag::tsc_adjust) ? "yes" : "no") << std::endl;
    std::cout << "AVX        : " << (cpu.hasFlag(ECpuidFlag::avx) ? "yes" : "no") << std::endl;
    std::cout << "AVXVNNI    : " << (cpu.hasFlag(ECpuidFlag::avxvnni) ? "yes" : "no") << std::endl;
    std::cout << "AVX2       : " << (cpu.hasFlag(ECpuidFlag::avx2) ? "yes" : "no") << std::endl;
    std::cout << "AVX512     : " << (cpu.hasFlag(ECpuidFlag::avx512f) ? "yes" : "no") << std::endl;
    // clang-format on
}

int
main(void)
{
    display_core_info();

    display_isa_info();

    return 0;
}
