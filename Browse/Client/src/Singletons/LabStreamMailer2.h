//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Singleton to send messages into LabStreamingLayer and receive incoming
// ones. A receiver has to create a LabStreamCallback object and register
// it in the mailer. This saves one from unregistering callbacks, because
// weak pointers are used.

#include "src/Singletons/LabStream/LabStream2.h"
#include <functional>

// Class to abstract callbacks when message is received
class LabStreamCallback2
{
public:

	// Constructor taking function that is called at callback
	LabStreamCallback2(std::function<void(std::vector<std::string>)> callbackFunction)
	{
		_callbackFunction = callbackFunction;
	}

private:

	// LabStreamMailer may send messages to this
	friend class LabStreamMailer2;

	// Receive message from lab streaming layer
	void Receive(std::vector<std::string> messages) const
	{
		_callbackFunction(messages);
	}

	// Function to callback
	std::function<void(std::vector<std::string>)> _callbackFunction;
};

// Acutal class
class LabStreamMailer2
{
public:

	// Get instance
	static LabStreamMailer2& instance()
	{
		static LabStreamMailer2 _instance;
		return _instance;
	}

	// Destructor
	~LabStreamMailer2() {}

	// Send message
	void Send(std::string message);

	// Someone has to poll this so new messages are read and sent to callbacks. Should be done by master.
	void Update();

	// Register callback to receive messages. If weak pointer is invalid, callback is removed
	void RegisterCallback(std::weak_ptr<LabStreamCallback2> wpCallback);

private:

	// LabStreamingLayer connection
	LabStream2 _labStream;

	// Vector of registered callbacks
	std::vector<std::weak_ptr<LabStreamCallback2> > _callbacks;

	// Private copy / asignment constructors
	LabStreamMailer2() {}
	LabStreamMailer2(const LabStreamMailer2&) {}
	LabStreamMailer2& operator = (const LabStreamMailer2 &) {}
};