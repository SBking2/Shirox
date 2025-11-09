#pragma once

namespace srx
{
	enum class LogLevel
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical,
		off
	};

	class Log final
	{
	public:
		static void Init(const std::string& file_path);
		static void SetLevel(LogLevel level);
		static inline void LogTrace(const std::string& message) { Write(LogLevel::Trace, message); };
		static inline void LogDebug(const std::string& message) { Write(LogLevel::Debug, message); };
		static inline void LogInfo(const std::string& message) { Write(LogLevel::Info, message); };
		static inline void LogWarn(const std::string& message) { Write(LogLevel::Warn, message); };
		static inline void LogError(const std::string& message) { Write(LogLevel::Error, message); };
		static inline void LogCritical(const std::string& message) { Write(LogLevel::Critical, message); };
	private:
		static char* LevelToString(LogLevel level);
		static char* LevelToColor(LogLevel level);
		static void Write(LogLevel level, const std::string& message);
		static std::ofstream s_log_file;
		static LogLevel s_level;
	};
}