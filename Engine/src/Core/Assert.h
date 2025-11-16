#pragma once
#include "Log/Log.h"

#define SRX_BREAK __debugbreak();
#define SRX_ASSERT(condition, ...) { if(!condition) { SRX_LOG_ERROR(__VA_ARGS__) SRX_BREAK; } }