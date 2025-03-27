// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABItemIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABItemIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UABItemIconWidget(const FObjectInitializer& ObjectInitializer);

	void SetIconImage(class UTexture2D* InImage);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnSetIconImageCpp")
	void K2_OnSetIconImage(class UTexture2D* InImage);
};
