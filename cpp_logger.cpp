// cpp_logger.cpp: 定义应用程序的入口点。
//

#include "cpp_logger.h"
using namespace lh::logger;

#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>

const char* Logger::levelArr[LOG_COUNT] = {
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL"
};

Logger* Logger::instance_ = nullptr;

Logger::Logger() : max_{ 0 }, len_{ 0 }, level_{ LOG_DEBUG } {}

Logger::~Logger() {
	close();
}

Logger* Logger::getInstance() {
	if (instance_ == nullptr)
		instance_ = new Logger();
	return instance_;
}

void Logger::open(const string& filename) {
	filename_ = filename;
	fout_.open(filename.c_str(), std::ios::app);
	if (fout_.fail()) {
		throw std::logic_error("open log file failed: " + filename);
	}
	// 将写指针移动到输出流的末尾，以便在输出流中追加新的数据。
	fout_.seekp(0, std::ios::end);
	// 获取输出流的写指针当前的位置。这个位置是相对于输出流的开头计算的偏移量
	len_ = fout_.tellp();
}

void Logger::close() {
	fout_.close();
}

void Logger::log(Level level, const char* file, int line, const char* format, ...) {
	if (level_ > level) {
		return;
	}

	if (fout_.fail()) {
		return;
	}
	// 返回当前时间的秒数
	time_t ticks = time(NULL);
	/*
	将 Unix 时间戳转换为本地时间，并返回一个指向 struct tm 结构体的指针。struct tm 结构体包含了年、月、日、时、分、秒等时间信息。*/
	struct tm* ptm = localtime(&ticks);
	char timestamp[32];
	memset(timestamp, 0, sizeof(timestamp));
	// "%Y-%m-%d %H-%M:%S", ptm) 将 struct tm 结构体中的时间信息格式化为指定的字符串格式，并将结果保存在 timestamp 字符数组中
	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H-%M:%S", ptm);

	int len = 0;
	const char* fmt = "%s %s %s:%d ";
	// snprintf() 函数，用于将指定的格式化字符串写入指定的字符缓冲区中。
	/*
	* 这里的第一个参数是 NULL，表示不需要实际写入任何字符，而是仅返回需要写入的字符数。第二个参数是 0，表示不需要写入字符的缓冲区大小
	* 第三个参数是格式化字符串，后面的参数是可变参数列表
	*/
	// 计算timestamp, levelArr[level], file, line 输出所需长度
	len = snprintf(NULL, 0, fmt, timestamp, levelArr[level], file, line);
	if (len > 0) {
		char* buffer = new char[len + 1];
		snprintf(buffer, len + 1, fmt, timestamp, levelArr[level], file, line);
		buffer[len] = 0;
		fout_ << buffer;
		delete[] buffer;
		len_ += len;
	}

	va_list arg_ptr;
	// va_start(arg_ptr, format) 宏用于初始化 arg_ptr，将其指向可变参数列表的起始位置
	va_start(arg_ptr, format);
	// 可以将格式化字符串和可变参数列表格式化为指定的字符缓冲区中
	len = vsnprintf(NULL, 0, format, arg_ptr);
	// va_end(arg_ptr) 宏用于结束可变参数列表的处理
	va_end(arg_ptr);

	if (len > 0) {
		char* content = new char[len + 1];
		va_start(arg_ptr, format);
		vsnprintf(content, len + 1, format, arg_ptr);
		va_end(arg_ptr);
		content[len] = 0;
		fout_ << content;
		delete[] content;
		len_ += len;
	}

	fout_ << "\n";
	// 用于刷新输出流的缓冲区，将缓冲区中的数据立即写入到输出设备中
	fout_.flush();

	if (max_ > 0 && len_ >= max_) {
		rotate();
	}
}

void Logger::max(int bytes) {
	max_ = bytes;
}

void Logger::level(int level) {
	level_ = level;
}

// 保存，重写
void Logger::rotate() {
	close();
	time_t ticks = time(NULL);
	struct tm* ptm = localtime(&ticks);
	char timestamp[32];
	memset(timestamp, 0, sizeof(timestamp));
	strftime(timestamp, sizeof(timestamp), ".%Y-%m-%d_%H-%M-%S", ptm);
	string filename = filename_ + timestamp;
	if (rename(filename_.c_str(), filename.c_str()) != 0) {
		throw std::logic_error("rename log file failed: " + string(strerror(errno)));
	}
	open(filename_);
}

