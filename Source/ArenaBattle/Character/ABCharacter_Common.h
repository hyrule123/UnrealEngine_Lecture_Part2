#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ECharacterAction : uint8
{
	None UMETA(DisplayName = "None"),
	Attack UMETA(DisplayName = "Attack"),
	Evade UMETA(DisplayName = "Dodge")
};

DECLARE_DELEGATE_OneParam(FOnReserveTimeEndDelegate, ECharacterAction /*NextAction*/);