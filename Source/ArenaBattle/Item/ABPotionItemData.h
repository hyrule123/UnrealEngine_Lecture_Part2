// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "ABPotionItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABPotionItemData : public UABItemData
{
	GENERATED_BODY()
	
public:
	UABPotionItemData();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		//두번째 인자는 보통 GetFName으로 지정을 함. 
		//첫번째 인자는 같으나 두번쨰 인자는 부모 클래스와 다르므로 각자 다르게 오버라이드 해주어야 한다.
		return FPrimaryAssetId(TEXT("ABItemData"), GetFName());
	}

	float GetHealAmount() const { return HealAmount; }

private:
	UPROPERTY(EditAnywhere, Category = Hp)
	float HealAmount;
};
