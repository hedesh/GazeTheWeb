//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "LabStreamMailer2.h"
#include "src/Utils/Logger.h"

void LabStreamMailer2::Send(std::string message)
{
	_labStream.Send(message);
}

void LabStreamMailer2::Update()
{
	// Poll incoming messages
	LogInfo("Before Poll");
	auto messages = _labStream.Poll();
	LogInfo("After Poll");
	// Go over callbacks and use them, if messages are available
	if (!messages.empty())
	{
		std::vector<int> toBeRemoved;
		int i = 0;
		for (auto& rwpCallback : _callbacks)
		{
			if (auto spCallback = rwpCallback.lock())
			{
				spCallback->Receive(messages);
			}
			else
			{
				// Weak pointer got invalid, so remove it later
				toBeRemoved.push_back(i);
			}
			i++; // increment index
		}

		// Remove dead weak pointers
		for (int j = toBeRemoved.size() - 1; j >= 0; j--) // do it backwards
		{
			_callbacks.erase(_callbacks.begin() + toBeRemoved.at(j));
		}
	}
}

void LabStreamMailer2::RegisterCallback(std::weak_ptr<LabStreamCallback2> wpCallback)
{
	_callbacks.push_back(wpCallback);
}