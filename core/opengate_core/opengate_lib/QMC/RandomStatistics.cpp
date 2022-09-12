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

#include "CurrentTrackInformationAction.h"
void RandomStatistics::AddCall(unsigned int n, unsigned int d)
{
	const std::source_location& location = CurrentTrackInformation::currentLocation;
	const auto bounce = CurrentTrackInformation::globalStepInformations.interactionNumber;

	const std::string name = formatter(
		location.file_name(), location.function_name(),
		location.line(), location.column()
	);

	total.callCount ++;
	total.numbersCount += n;

	calls[name].callCount ++;
	calls[name].numbersCount += n;

	const std::string& className = CurrentTrackInformation::currentClassName;
	const std::string& funcName  = CurrentTrackInformation::currentFuncName;
	
	std::string idx = className + "::" + funcName + "(" + std::to_string(location.line()) + ")," + std::to_string(bounce);
	dimensions[idx].insert(d);
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

	for (const auto& dim : dimensions)
	{
		outFile << dim.first << ": ";
		for (const auto& d : dim.second)
			outFile << d << ";";
		outFile << "\n";
	}
}

RandomStatistics::~RandomStatistics()
{
	Write();
}