#include "Logger.hpp"
#include <amx/amx.h>
#include <chrono>
#include <iomanip>

void DebugInfoManager::Update(AMX * const amx, const char *func)
{
	m_Amx = amx;
	m_NativeName = func;
	m_Info.clear();
	m_Available = samplog::Api::Get()->GetAmxFunctionCallTrace(amx, m_Info);
}

void DebugInfoManager::Clear()
{
	m_Amx = nullptr;
	m_NativeName = nullptr;
	m_Available = false;
}

Logger::~Logger()
{
	if (m_LogFile.is_open())
		m_LogFile.close();
}

void Logger::Initialize(const std::string& log_path)
{
	m_LogFile.open(log_path, std::ios::out | std::ios::app);
	if (m_LogFile.is_open())
	{
		Log(samplog_LogLevel::INFO, "File logger initialized: {}", log_path);
	}
}

void Logger::LogToAll(samplog_LogLevel level, const char* msg)
{
	m_Logger.Log(level, msg);

	if (m_LogFile.is_open())
	{
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto level_str = "INFO";
		switch (level)
		{
		case samplog_LogLevel::DEBUG: level_str = "DEBUG"; break;
		case samplog_LogLevel::WARNING: level_str = "WARNING"; break;
		case samplog_LogLevel::ERROR: level_str = "ERROR"; break;
		case samplog_LogLevel::FATAL: level_str = "FATAL"; break;
		}

		m_LogFile << "[" << std::put_time(std::localtime(&now), "%H:%M:%S") << "] [" << level_str << "] " << msg << std::endl;
		m_LogFile.flush();
	}
}

ScopedDebugInfo::ScopedDebugInfo(AMX * const amx, const char *func,
	cell * const params, const char *params_format /* = ""*/)
{
	DebugInfoManager::Get()->Update(amx, func);

	if (Logger::Get()->IsLogLevel(samplog_LogLevel::DEBUG))
		Logger::Get()->m_Logger.LogNativeCall(amx, params, func, params_format);
}
