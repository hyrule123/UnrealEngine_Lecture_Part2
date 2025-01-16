// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GameData/ABCharacterStat.h"

#include "ABCharacterStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
	

public:
	//TODO: 선언만 해놓은 상태, 차후 구현 예정
	void UpdateStatWidget(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);
};
