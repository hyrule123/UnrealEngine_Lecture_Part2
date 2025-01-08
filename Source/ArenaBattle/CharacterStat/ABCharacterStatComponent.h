// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameData/ABCharacterStat.h"

#include "ABCharacterStatComponent.generated.h"

//델리게이트 선언
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

	void SetLevelStat(int32 InNewLevel);
	void SetModifierStat(const FABCharacterStat& InModifierStat) {
		ModifierStat = InModifierStat;
	}
	FABCharacterStat GetTotalStat() const {
		return BaseStat + ModifierStat;
	}
	float GetCurrentLevel() const { return CurrentLevel; }
	float GetCurrentHp() const { return CurrentHp; }
	float GetAttackRadius() const { return AttackRadius; }

	float ApplyDamage(float InDamage);

protected:
	//HP 변경은 반드시 이 함수를 통해서 설정
	void SetHp(float NewHp);

	//Transient: 디스크에 저장할 필요가 없는 변수
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentLevel;

	UPROPERTY(VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	float AttackRadius;

	//캐릭터 기본 스탯
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	FABCharacterStat BaseStat;

	//장비 스탯
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	FABCharacterStat ModifierStat;
};
