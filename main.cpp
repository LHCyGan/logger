#include <iostream>
using namespace std;

#include "cpp_logger.h"
using namespace lh::logger;

int main() {
	Logger::getInstance()->open("E:\\CODE\\C++\\cpp_logger\\test.log");
	Logger::getInstance()->max(1024);

	log_debug("hello world!");
	log_error("hello world!");
	log_fatal("hello world!");
	log_info("hello world!");

	return 0;
}