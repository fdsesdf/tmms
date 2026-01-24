#pragma once

#include "base/LogStream.h"



#define NETWORK_LOG_TRACE \
    tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kTrace, __func__)

#define NETWORK_LOG_DEBUG \
    tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kDebug, __func__)

#define NETWORK_LOG_INFO \
    tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kInfo,  __func__)

#define NETWORK_LOG_WARN \
    tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kWarn,  __func__)

#define NETWORK_LOG_ERROR \
    tmms::base::LogStream(tmms::base::g_logger, __FILE__, __LINE__, tmms::base::kError, __func__)
