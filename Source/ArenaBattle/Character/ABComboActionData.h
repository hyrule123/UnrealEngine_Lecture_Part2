// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABComboActionData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABComboActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UABComboActionData();

	//현재 몽타주 섹션 이름(ComboAttack 1, 2, 3, 4 -> ComboAttack)
	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix;	

	//최대 콤보 카운트
	UPROPERTY(EditAnywhere, Category = Name)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category = Name)
	float FrameRate;

	//입력이 유효한 프레임
	UPROPERTY(EditAnywhere, Category = Name)
	TArray<float> EffectiveFrameCount;
};
