#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <string>

class Trace
{
public:

	explicit Trace(const char *func_name, const char* argsfmt);

	static void LogToFile(const char *trace_file);

	~Trace();

	void LogMsg(int depth, int align, const char *fmt, va_list args);

	static std::string	trace_file_;

	static int		depth_;
	static const char*  nest_;
	static time_t       last_invoke_time_;
};

	



