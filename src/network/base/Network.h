#pragma once

#include "base/LogStream.h"



#define NETWORK_LOG_TRACE \
    if (tmms::base::g_logger) \
        tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kTrace, __func__)

#define NETWORK_LOG_DEBUG \
    if (tmms::base::g_logger) \
        tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kDebug, __func__)

#define NETWORK_LOG_INFO \
    if (tmms::base::g_logger) \
        tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kInfo,  __func__)

#define NETWORK_LOG_WARN \
    if (tmms::base::g_logger) \
        tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kWarn,  __func__)

#define NETWORK_LOG_ERROR \
    if (tmms::base::g_logger) \
        tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kError, __func__)
