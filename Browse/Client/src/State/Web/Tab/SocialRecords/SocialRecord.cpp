//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "SocialRecord.h"
#include <iomanip>
#include <ctime>
#include <sstream>

using json = nlohmann::json;

SocialRecord::SocialRecord(std::string URL) : _URL(URL)
{
	// Store start date
	_startDate = GetDate();

	// Store start time for duration estimation
	_startTime = std::chrono::system_clock::now();
}

SocialRecord::SocialRecord(std::string URL, SocialPlatform platform) : _URL(URL), _platform(platform)
{
	// Store start date
	_startDate = GetDate();
}

SocialRecord::~SocialRecord() {}

void SocialRecord::AddTimeInForeground(float time)
{
	_durationInForeground += time;
}

void SocialRecord::AddTimeActiveUser(float time)
{
	_durationUserActive += time;
}

void SocialRecord::AddScrollingDelta(float delta)
{
	_scrollAmount += delta;
}

void SocialRecord::AddClick()
{
	_clickCount++;
}

void SocialRecord::AddSubpage()
{
	_subpageCount++;
}

void SocialRecord::End()
{
	// Store end date
	_endDate = GetDate();

	// Store end time for duration estimation
	_endTime = std::chrono::system_clock::now();
}

nlohmann::json SocialRecord::ToJSON() const
{
	// Return JSON structure
	return {
		{ "url", _URL },
		{ "startDate", _startDate },
		{ "endDate", _endDate },
		{ "duration", std::chrono::duration<float>(_endTime-_startTime).count() },
		{ "durationInForeground", _durationInForeground },
		{ "durationUserActive", _durationUserActive },
		{ "scrollAmount", _scrollAmount },
		{ "subpageCount", _subpageCount },
		{ "clickCount", _clickCount }
	};
}

std::string SocialRecord::GetDate() const
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, DATE_FORMAT.c_str());
	return oss.str();
}