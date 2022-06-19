#include "RandomStatistics.h"

#include "G4Exception.hh"

RandomStatistics::RandomStatistics(const std::string& fname)
{
	// TODO : Expose the formatting function ? 

	formatter = &DefaultFormatter;
	outFile.open(fname);

	if (!outFile.is_open())
    {
        std::string msg = "[RandomStatistics]: Can not open file " + fname + ", the profiler will not output its results";
        G4Exception(
            __FILE__, __PRETTY_FUNCTION__, 
            G4ExceptionSeverity::FatalErrorInArgument, msg.c_str()
        );
    }	
}

void RandomStatistics::AddCall(const std::source_location& location, unsigned int n)
{
	const std::string name = formatter(
		location.file_name(), location.function_name(),
		location.line(), location.column()
	);

	total.callCount ++;
	total.numbersCount += n;

	calls[name].callCount ++;
	calls[name].numbersCount += n;
}

void RandomStatistics::Write()
{
	// Write header
	outFile << "Name;calls;numbers" << "\n";
	outFile << formatter("", "Total", 0, 0) << ";" << total.callCount << ";" << total.numbersCount << "\n";

	for (const auto& stat : calls)
	{
		outFile << stat.first << ";" << stat.second.callCount << ";" << stat.second.numbersCount << "\n";
	}
}

RandomStatistics::~RandomStatistics()
{
	Write();
}