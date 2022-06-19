#pragma once

#include <source_location>
#include <functional>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using Formatter = std::function<std::string(const std::string&, const std::string&, unsigned int, unsigned int)>;

inline std::string DefaultFormatter(const std::string& file, const std::string& function, unsigned int line, unsigned int col)
{
	((void) file);
	std::stringstream ss;
	ss << function << ":";
	ss << line;
	ss << "(" << col << ")";
	return ss.str(); 
}


class RandomStatistics
{
public:
	struct StatEntry
	{
		unsigned int callCount = 0;    // Number of time it was call
		unsigned int numbersCount = 0; // Number of number asked (one call can represent multiple numbers)
	};

	RandomStatistics(const std::string& fname);

	void AddCall(const std::source_location& location, unsigned int n);
	void Write();

	~RandomStatistics();
private:
	StatEntry total;
	std::map<std::string,StatEntry> calls;

	std::ofstream outFile;
	Formatter formatter;
};