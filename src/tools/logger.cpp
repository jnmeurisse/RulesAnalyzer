/*!
* This file is part of FortiRDP
*
* Copyright (C) 2022 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "tools/Logger.h"

std::unique_ptr<Logger> LOGGER;

Logger::Logger() :
	_writers(),
	_level(Level::LL_INFO)
{
}


void Logger::set_level(Level level)
{
	_level = level;
}


void Logger::add_writer(LogWriter* writer)
{
	_writers.push_back(writer);
	_writers.unique();
}


void Logger::remove_writer(LogWriter* writer)
{
	writer->flush();
	_writers.remove(writer);
}


Logger* Logger::get_logger()
{
	if (LOGGER == nullptr) {
		// Note: make_unique can't be used, Logger constructor is private
		LOGGER = std::unique_ptr<Logger>(new Logger());
	}

	return LOGGER.get();
}


void Logger::write(Level level, const char* text)
{
	for (auto writer : _writers) {
		writer->write(level, text);
	}
}


void Logger::log(Level level, const std::string& text)
{
	if (is_enabled(level)) {
		write(level, text.c_str());
	}
}

	
void Logger::log(Level level, const char* format, ...)
{
	if (is_enabled(level)) {
		va_list args;
		va_start(args, format);
		log(level, format, args);
		va_end(args);
	}
}
	

void Logger::debug(const char* format, ...)
{
	if (is_enabled(Level::LL_DEBUG)) {
		va_list args;
		va_start(args, format);
		log(Level::LL_DEBUG, format, args);
		va_end(args);
	}
}


void Logger::info(const char* format, ...)
{
	if (is_enabled(Level::LL_INFO)) {
		va_list args;
		va_start(args, format);
		log(Level::LL_INFO, format, args);
		va_end(args);
	}
}


void Logger::warning(const char* format, ...)
{
	if (is_enabled(Level::LL_WARNING)) {
		va_list args;
		va_start(args, format);
		log(Level::LL_WARNING, format, args);
		va_end(args);
	}
}


void Logger::error(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	log(Level::LL_ERROR, format, args);
	va_end(args);
}


void Logger::flush()
{
	for (auto writer : _writers) {
		writer->flush();
	}
}


char* Logger::fmt(const char* format, va_list args)
{
	char* text = nullptr;
	int size = 0;

	// compute the number of characters
	size = _vscprintf(format, args) + 1;

	// allocate and format
	text = new char[size];
	vsprintf_s(text, size, format, args);

	// return the formated text
	return text;
}


void Logger::log(Level level, const char* format, va_list args)
{
	const char* const text = fmt(format, args);
	write(level, text);
	delete[] text;
}


static std::string datetime()
{
	// get current system time
	const time_t now = time(nullptr);

	// convert to local time
	tm local_time;
	localtime_s(&local_time, &now);

	// output as a string
	char buffer[128] { 0 };
	strftime(buffer, sizeof(buffer), "%F %T", &local_time);

	return std::string{ buffer };
}


const std::string& LogWriter::level_msg(Logger::Level level) const
{
	static std::string levels[4]{
		"[debug]",
		"[info]",
		"[warning]",
		"[error]"
	};

	const int level_index = static_cast<int>(level);
	if (level_index < 0 || level_index > 3)
		throw std::runtime_error("invalid log level index");

	return levels[level_index];
}


FileLogWriter::FileLogWriter() :
	_ofs()
{
}


FileLogWriter::~FileLogWriter()
{
	if (_ofs.is_open()) {
		_ofs.close();
	}
}


bool FileLogWriter::open(const std::string& filename)
{
	_ofs.open(filename, std::ostream::out);

	return !_ofs.fail();
}


void FileLogWriter::write(Logger::Level level, const char * text)
{
	if (_ofs.is_open()) {
		_ofs << datetime() << level_msg(level) << " " << text << "." << std::endl;
	}
}


void FileLogWriter::flush()
{
	if (_ofs.is_open()) {
		_ofs.flush();
	}
}


void ConsoleLogWriter::write(Logger::Level level, const char* text)
{
	std::fputs(level_msg(level).c_str(), stdout);
	std::fputs(" ", stdout);
	std::fputs(text, stdout);
	std::fputs(".\n", stdout);

	std::fflush(stdout);
}


void ConsoleLogWriter::flush()
{
	std::fflush(stdout);
}
