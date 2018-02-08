//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "DOMNodeInteraction.h"
#include "src/CEF/Mediator.h"
#include <locale>
#include <memory>
#include <codecvt>
#include <string>
#include <iostream>

bool DOMJavascriptCommunication::SendProcessMessageToRenderer(CefRefPtr<CefProcessMessage> msg)
{
	return _sendRenderMessage(msg);
}

CefRefPtr<CefProcessMessage> DOMJavascriptCommunication::SetupExecuteFunctionMessage(
	std::string func_name, 
	CefRefPtr<CefListValue> param)
{
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ExecuteJavascriptFunction");
	const auto& args = msg->GetArgumentList();
	CefRefPtr<CefListValue> header = CefListValue::Create();
	header->SetString(0, func_name);
	header->SetInt(1, GetType());
	header->SetInt(2, GetId());

	args->SetList(0, header);
	args->SetList(1, param);
	return msg;
}

void DOMTextInputInteraction::InputText(std::u16string text, bool submit)
{
	// Focus input node
	SendExecuteFunctionMessage("focusNode");

	// Clear text content and afterwards enter input
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("SelectAll");
	_sendRenderMessage(msg);

	// Emulate keyboard streaks (Shift+Enter for line break?)
	std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>, wchar_t> conv;
	std::wstring ws = conv.from_bytes(
		reinterpret_cast<const char*> (&text[0]),
		reinterpret_cast<const char*> (&text[0] + text.size()));
	_mediator->EmulateKeyboardStrokes(ws);

	//msg = CefProcessMessage::Create("EmulateKeyboardStrokes");
	//const auto& args = msg->GetArgumentList();
	//args->SetString16(0, text);
	//_sendRenderMessage(msg);
	

	// Emulate Enter key, when submitting
	if (submit)
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("EmulateEnterKey");
		_sendRenderMessage(msg);
	}

	// TODO: Special handling needed due to utf16
	SendExecuteFunctionMessage("setText", text);
}
