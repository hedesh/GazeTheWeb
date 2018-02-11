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

std::wstring u16string_to_wstring(std::u16string str)
{
	std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>, wchar_t> conv;
	return conv.from_bytes(
		reinterpret_cast<const char*> (&str[0]),
			reinterpret_cast<const char*> (&str[0] + str.size()));

}
void DOMTextInputInteraction::InputText(std::u16string text, bool submit)
{
	// Focus input node
	_pTab->ExecuteCorrespondingJavascriptFunction(getBasePtr(), "focusNode");

	// Clear text content and afterwards enter input
	_pTab->EmulateSelectAll();
	_pTab->EmulateKeyboardStrokes(text);

	// Emulate Enter key, when submitting
	if (submit)
	{
		_pTab->EmulateEnterKey();
	}

	_pTab->ExecuteCorrespondingJavascriptFunction(getBasePtr(), "setText", text);


}
