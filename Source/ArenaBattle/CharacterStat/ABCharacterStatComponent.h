// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameData/ABCharacterStat.h"

#include "ABCharacterStatComponent.generated.h"

//델리게이트 선언
DECLARE_MULTICAST_DELEGATE(FOnCurHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurHpChangedDelegate, float /*CurrentHp*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FABCharacterStat& /*BaseStat*/, const FABCharacterStat& /*ModifierStat*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	/**
	 * Initializes the component.  Occurs at level startup or actor spawn. This is before BeginPlay (Actor or Component).
	 * All Components in the level will be Initialized on load before any Actor/Component gets BeginPlay
	 * Requires component to be registered, and bWantsInitializeComponent to be true.
	 */
	virtual void InitializeComponent() override;
	
	// Called when the game starts: 내부 코드를 InitializeComponent로 옮김
	//virtual void BeginPlay() override;

public:
	FOnCurHpZeroDelegate OnCurHpZero;
	FOnCurHpChangedDelegate OnCurHpChanged;
	FOnStatChangedDelegate OnStatChanged;

	void SetLevelStat(int32 InNewLevel);
	void SetBaseStat(const FABCharacterStat& InBaseStat) { 
		BaseStat = InBaseStat; 
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}
	void SetModifierStat(const FABCharacterStat& InModifierStat) { 
		ModifierStat = InModifierStat; 
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}

	const FABCharacterStat& GetBaseStat() const { return BaseStat; }
	const FABCharacterStat& GetModifierStat() const { return ModifierStat; }
	FABCharacterStat GetTotalStat() const { return BaseStat + ModifierStat; }

	float GetCurrentLevel() const { return CurrentLevel; }
	float GetCurrentHp() const { return CurrentHp; }
	float GetMaxHp() const { return BaseStat.MaxHp + ModifierStat.MaxHp; }
	float GetAttackRadius() const { return AttackRadius; }

	float ApplyDamage(float InDamage);
	void HealHp(float InHealAmount);

protected:
	//HP 변경은 반드시 이 함수를 통해서 설정
	void SetCurHp(float NewHp);

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
