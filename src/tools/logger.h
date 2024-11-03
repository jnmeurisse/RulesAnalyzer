/*!
* This file is part of FortiRDP
*
* Copyright (C) 2022 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <string>
#include <list>
#include <fstream>


// forward declaration
class LogWriter;


/**
* The application Logger.
*/
class Logger final
{
public:
	enum class Level { LL_DEBUG=0, LL_INFO=1, LL_WARNING=2, LL_ERROR=3};

	/* Logs a message
	*/
	void debug(const char* format, ...);
	void info(const char* format, ...);
	void warning(const char* format, ...);
	void error(const char* format, ...);

	/* Flushes all writers
	*/
	void flush();

	/* Sets the threshold to 'level'. Logging message that are less severe than
	*  the specified level are ignored
	*/
	void set_level(Level level);

	/* Returns the current level
	*/
	inline Level get_level() const { return _level; }

	/* tests if the specified level is more severe than the current level
	*/
	inline bool is_enabled(Level level) const { return level >= _level; }

	/* adds a writer to this logger
	*/
	void add_writer(LogWriter* writer);

	/* removes a writer from this logger
	*/
	void remove_writer(LogWriter* writer);

	/* Creates or returns the instance of the logger.
	*  Note : This function is not thread safe.
	*/
	static Logger* get_logger();

private:
	Logger();

	void log(Level level, const std::string& text);
	void log(Level level, const char* format, ...);
	void log(Level level, const char* format, va_list args);

	/* Writes a message to the log writers
	*/
	void write(Logger::Level level, const char* text);

	/* Formats an error message. The caller is responsible to free
	*  the temporary buffer used to store the message.  The function returns
	*  a null pointer if an error occurred.
	*/
	char* fmt(const char* format, va_list args);

private:
	// a list of writers
	std::list<LogWriter *> _writers;

	// current logger level
	Level _level;
};


/**
* An abstract log writer
*/
class LogWriter
{
public:
	virtual ~LogWriter() {}

	virtual void write(Logger::Level level, const char* text) = 0;
	virtual void flush() { return; }

protected:
	explicit LogWriter() {}
	const std::string& level_msg(Logger::Level level) const;
};


/**
* A file log writer
*/
class FileLogWriter final: public LogWriter
{
public:
	explicit FileLogWriter();
	virtual ~FileLogWriter();

	bool open(const std::string& filename);
	virtual void write(Logger::Level level, const char* text) override;
	virtual void flush() override;

private:
	std::ofstream _ofs;
};


/**
* A console log writer
*/
class ConsoleLogWriter final : public LogWriter
{
public:
	ConsoleLogWriter() = default;

	virtual void write(Logger::Level level, const char* text) override;
	virtual void flush() override;
};
