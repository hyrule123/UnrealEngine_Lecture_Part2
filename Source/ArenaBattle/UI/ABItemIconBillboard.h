// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ABUserWidget.h"
#include "ABItemIconBillboard.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABItemIconBillboard : public UABUserWidget
{
	GENERATED_BODY()
	
public:
	UABItemIconBillboard();

	void SetIconImage(TObjectPtr<class UTexture2D> InIconImage);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTexture2D> IconImage;
};
