/*
 * Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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
 */

#pragma once

#include "Au/Interface/IError.hh"
#include "Au/Types.hh"

namespace Au {
/**
 * @name ErrorBase
 * @detail
 * ErrorBase class is designed to work with different Error classes.
 * The errors are extendable to support dynamic module loading (plugin system).
 *
 */
class ErrorBase : public IError
{

  public:
    typedef Uint16 ModuleType;

    ErrorBase()
        : m_error{ 0 }
    {
    }

    ErrorBase(Uint16 code)
        : ErrorBase{}
    {
        m_error.field.module_error = code;
    }

    ErrorBase(Uint64 code)
        : m_error{ code }
    {
    }

    /**
     * @brief     Overriden function to return code as 64-bit integer
     * @param     none
     * @return    A combined Uint64
     */
    virtual Uint64 code() const override final { return m_error.val; }

    /**
     * @brief   Register a module specific error handler
     *
     * @param[in]       mt      Module type
     * @param[in]       ie      Error Interface to the module error handler
     *
     * @return  boolean Status of whether the registration was success
     */
    static bool registerModuleError(ModuleType mt, IError& ie);

  protected:
    ErrorBase(Uint16 module_id, Uint16 module_error)
        : ErrorBase{}
    {
        m_error.field.module_id    = module_id;
        m_error.field.module_error = module_error;
    }

    void   setModuleError(Uint16 error) { m_error.field.module_error = error; }
    Uint16 getModuleError() const { return m_error.field.module_error; }
    Uint16 getBaseError() const { return m_error.field.base_error; }

    /**
     * @brief Getter function for module id
     *
     * @return Module id of a particular module
     */
    void setModuleId(Uint16 mid) { m_error.field.module_id = mid; }

    /**
     * @brief   virtual function to get derived class's module id
     *
     * @return  an Uint16 compatible with module_id
     */
    virtual Uint16 getModuleId() const { return m_error.field.module_id; };

    virtual bool isEq(IError const& lhs, IError const& rhs) const override final
    {
        return lhs.code() == rhs.code();
    }

  protected:
    union
    {
        Uint64 val;

        struct
        {
            Uint64 base_error   : 16;
            Uint64 module_error : 16;
            Uint64 module_id    : 16;
            Uint64 __reserved   : 16;
        } field;
    } m_error;

    //   static std::unordered_map<Uint16, IError> m_dispatcher_map;
};
} // namespace Au
