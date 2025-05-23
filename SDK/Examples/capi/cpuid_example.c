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

#include "Capi/au/cpuid/cpuid.h"

#include <stdio.h>
#include <string.h>

#define size 16
static void
display_core_info()
{
    int         core_num = 0;
    char        buf[size];
    const char* token = NULL;
    int         index = 0;

    memset(buf, 0, size * sizeof(char));
    const char* vendorInfo[13] = { "Vendor   : ",
                                   "Family   : ",
                                   "Model    : ",
                                   "Stepping : ",
                                   "Uarch    : " };

    if (au_cpuid_is_amd(core_num)) {
        printf("AMD CPU detected...\n");
    } else {
        // Intel Apis are not exposed.
        printf("Unknown CPU detected...\n");
    }

    printf("\nGetting CPU information for core 0\n");
    au_cpuid_get_vendor(core_num, buf, size);

    token = strtok(buf, "\r\n");
    while (token != NULL) {
        printf("%s", vendorInfo[index++]);
        printf("%s\n", token);
        token = strtok(NULL, "\r\n");
    }
}

static void
display_isa_info()
{
    int core_num = 0;
    printf("\nGetting ISA information for core 0\n");
    if (au_cpuid_arch_is_zen5(core_num)) {
        printf("CPU supports zen5 ISA\n");
    } else if (au_cpuid_arch_is_zen4(core_num)) {
        printf("CPU supports zen4 ISA\n");
    } else if (au_cpuid_arch_is_zen3(core_num)) {
        printf("CPU supports zen3 ISA\n");
    } else if (au_cpuid_arch_is_zen2(core_num)) {
        printf("CPU supports zen2 ISA\n");
    } else if (au_cpuid_arch_is_zenplus(core_num)) {
        printf("CPU supports zenplus ISA\n");
    } else if (au_cpuid_arch_is_zen(core_num)) {
        printf("CPU supports zen ISA\n");
    } else {
        printf("CPU does not support zen or higher ISA or this Cpu is not "
               "supported\n");
    }
    if (au_cpuid_arch_is_x86_64v4(core_num)) {
        printf("CPU supports x86-64v4 ISA\n");
    }
    if (au_cpuid_arch_is_x86_64v3(core_num)) {
        printf("CPU supports x86-64v3 ISA\n");
    }
    if (au_cpuid_arch_is_x86_64v2(core_num)) {
        printf("CPU supports x86-64v2 ISA\n");
    } else {
        printf("CPU does not support x86-64v2 or higher ISA\n");
    }

    if (au_cpuid_arch_is_zen_family(core_num)) {
        printf("CPU belongs to the Zen family\n");
    } else {
        printf("CPU does not belong to the Zen family\n");
    }

    printf("\nCpu Feature Identification...\n");
    const char* const flags_array[] = {
        "tsc_adjust", "avx", "avxvnni", "avx2", "avx512f"
    };
    int flags_array_size = sizeof(flags_array) / sizeof(flags_array[0]);
    printf("Flags being checked: ");
    for (int i = 0; i < flags_array_size; i++) {
        printf(
            "%s%s", flags_array[i], (i < flags_array_size - 1) ? ", " : "\n");
    }
    bool result = au_cpuid_has_flags(core_num, flags_array, flags_array_size);
    printf("Checking for TSC_ADJUST, AVX, AVXVNNI, AVX2, AVX512...\n");
    printf("%s\n",
           (result ? "All are present"
                   : "One or more of the flags are not present"));

    result = au_cpuid_has_flags_all(core_num, flags_array, flags_array_size);
    printf("Checking for all flags...\n");
    printf("%s\n",
           (result ? "All are present"
                   : "One or more of the flags are not present"));

    result = au_cpuid_has_flags_any(core_num, flags_array, flags_array_size);
    printf("Checking for any flags...\n");
    printf("%s\n",
           (result ? "One or more of the flags are present"
                   : "None of the flags are present"));
}

int
main(void)
{
    display_core_info();

    display_isa_info();

    return 0;
}
