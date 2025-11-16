#include "PCH.h"
#include "Log.h"

namespace srx
{
	LogLevel Log::s_level = LogLevel::off;
	std::ofstream Log::s_log_file;
	bool Log::s_inited = false;

	void Log::Init(const std::string& file_path)
	{
		s_log_file.open(file_path, std::ios::app);
	}

	void Log::SetLevel(LogLevel level)
	{
		s_level = level;
	}

	char* Log::LevelToString(LogLevel level)
	{
		switch (level)
		{
			case LogLevel::Trace:    return "TRACE";
			case LogLevel::Debug:    return "Debug";
			case LogLevel::Info:     return "INFO";
			case LogLevel::Warn:     return "WARN";
			case LogLevel::Error:    return "ERROR";
			case LogLevel::Critical: return "CRITICAL";
			default: return "UNKNOWN";
		}
	}

	char* Log::LevelToColor(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Trace:    return "\033[0m";   // 黑色
		case LogLevel::Debug:    return "\033[32m";   // 绿色
		case LogLevel::Info:     return "\033[36m";   // 青色（更亮）
		case LogLevel::Warn:     return "\033[33m";   // 黄色
		case LogLevel::Error:    return "\033[91m";   // 亮红色（原 31 在 Windows 太暗）
		case LogLevel::Critical: return "\033[95m";   // 亮洋红
		default: return "\033[0m";
		}
	}

	void Log::Write(LogLevel level, const std::string& message)
	{
		if (level < s_level)
			return;

		std::time_t now = std::time(nullptr);
		char timeStr[20];
		std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", std::localtime(&now));

		std::cout << LevelToColor(level)
			<< "[" << timeStr << "] [" << LevelToString(level) << "] "
			<< message << "\033[0m" << std::endl;

		if (s_log_file.is_open())
		{
			s_log_file << "[" << timeStr << "] [" << LevelToString(level) << "] "
				<< message << std::endl;
		}
	}

}