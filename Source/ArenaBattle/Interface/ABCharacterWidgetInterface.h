// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ABCharacterWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UABCharacterWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Widget Component에 부착되는 Widget들에 대한 의존성을 줄이기 위한 인터페이스 클래스
 */
class ARENABATTLE_API IABCharacterWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetupCharacterWidget(class UABUserWidget* InUserWidget) = 0;
};
