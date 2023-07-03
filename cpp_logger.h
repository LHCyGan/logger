// cpp_logger.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <string>
using std::string;

#include <fstream>
using std::ofstream;

namespace lh {
	namespace logger {

#define log_debug(format, ...) \
	Logger::getInstance()->log(Logger::LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_info(format, ...) \
	Logger::getInstance()->log(Logger::LOG_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_WARN(format, ...) \
	Logger::getInstance()->log(Logger::LOG_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_error(format, ...) \
	Logger::getInstance()->log(Logger::LOG_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_fatal(format, ...) \
	Logger::getInstance()->log(Logger::LOG_FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)


		class Logger {

		public:
			enum Level {
				LOG_DEBUG = 0,
				LOG_INFO,
				LOG_WARN,
				LOG_ERROR,
				LOG_FATAL,
				LOG_COUNT
			};

			static Logger* getInstance();
			void open(const string& filename);
			void close();
			void log(Level level, const char* file, int line, const char* format, ...);
			void max(int bytes);
			void level(int level);

		private:
			string filename_;
			ofstream fout_;
			int max_;
			int len_;
			int level_;
			static const char* levelArr[LOG_COUNT];
			static Logger* instance_;

			Logger();
			~Logger();
			Logger(const Logger& other) = delete;
			Logger& operator=(const Logger& other) = delete;
			void rotate();

		};
	}
}


// TODO: 在此处引用程序需要的其他标头。
