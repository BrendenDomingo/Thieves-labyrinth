#include "Trace.h"

std::string	Trace::trace_file_ = "";
int		Trace::depth_ = 0;
const char*  Trace::nest_ = "| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | ";
time_t       Trace::last_invoke_time_ = 0;


Trace::Trace(const char * func_name, const char * argsfmt)
{
	char fmt[256] = { 0 };
	sprintf(fmt, "%s%s", func_name, argsfmt);
	va_list arglist = (char*)argsfmt;
	LogMsg(depth_, depth_ * 2, fmt, arglist);
	
	++depth_;
}

void Trace::LogToFile(const char * trace_file)
{
	trace_file_ = trace_file;
}

Trace::~Trace()
{
	--depth_;
}

void Trace::LogMsg(int depth, int align, const char * fmt, va_list args)
{
	FILE	*fp = fopen(trace_file_.c_str(), "a+");
	if (fp == NULL)
	{
		return;
	}


	time_t		curTime;
	time(&curTime);

	char	timeStamp[32] = { 0 };
	strftime(timeStamp, sizeof(timeStamp),
		"%Y%m%d.%H%M%S", localtime(&curTime));

	// only log the timestamp when the time changes
	unsigned int len = fprintf(fp, "%s %*.*s> (%d)",
		(last_invoke_time_ != curTime) ?
		timeStamp : "               ",
		2 * depth,
		2 * depth,
		nest_,
		depth);
	last_invoke_time_ = curTime;
	len += vfprintf(fp, fmt, args);
	len += fwrite("\n", 1, 1, fp);
	fflush(fp);
	fclose(fp);
}
