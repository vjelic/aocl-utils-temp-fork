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

#pragma once

#include "Au/Logger/LogWriter.hh"

namespace Au::Logger {

/**
 * @class LogManager
 * @brief Manages buffered logging workflows and forwards them to a LogWriter.
 */
class LogManager
{
  private:
    static thread_local std::vector<Message>
        m_storage; ///< Thread local storage for log messages
    std::shared_ptr<LogWriter> m_logWriter; ///< Shared pointer to LogWriter

    // Disable copy constructor and assignment operator
    LogManager(const LogManager&)            = delete;
    LogManager& operator=(const LogManager&) = delete;

  public:
    /**
     * @brief Creates a LogManager and associates it with a given LogWriter.
     * @param logWriter Shared pointer to the LogWriter instance.
     */
    explicit LogManager(std::shared_ptr<LogWriter> logWriter);

    /**
     * @brief Appends a single message to the thread-local storage.
     * @param msg The message to log.
     */
    void log(Message& msg);

    /**
     * @brief Flushes the thread-local messages to the LogWriter.
     */
    void flush();

    /**
     * @brief Destructor for LogManager.
     */
    ~LogManager();

    /**
     * @brief Operator << appends a single Message to the current thread's
     * buffer.
     * @param msg The message to log.
     * @return Reference to this LogManager.
     */
    LogManager& operator<<(const Message& msg);

    /**
     * @brief Operator << appends a string as a Message to the current thread's
     * buffer.
     * @param msg String to be logged.
     * @return Reference to this LogManager.
     */
    LogManager& operator<<(const std::string& msg);
};
} // namespace Au::Logger
