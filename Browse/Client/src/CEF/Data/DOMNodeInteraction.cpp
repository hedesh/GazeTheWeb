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


void DOMTextInputInteraction::InputText(std::u16string text, bool submit)
{
	// Focus input node
	_pTab->ExecuteCorrespondingJavascriptFunction(getBasePtr(), "focusNode");

	// Clear text content and afterwards enter input
	_pTab->EmulateSelectAll();
	_pTab->EmulateKeyboardStrokes(text, submit);

	// Emulate Enter key, when submitting
	if (submit)
	{
		_pTab->EmulateEnterKey();
	}

	// TODO: Special handling needed due to utf16
	_pTab->ExecuteCorrespondingJavascriptFunction(
		getBasePtr(),"setText", "TODO: TabDOMNodeInterface setText needed");
}
