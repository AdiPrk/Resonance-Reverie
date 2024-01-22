#include <Engine/PCH/pch.h>
#include "components.h"
#include "../scriptableEntity.h"

namespace Dog {

	void NativeScriptComponent::SetVariable(const std::string& name, const std::any& value) {
		if (Instance) {
			Instance->SetVariable(name, value);
		}
		else {
			TemporaryVariables[name] = value;
		}
	}

}