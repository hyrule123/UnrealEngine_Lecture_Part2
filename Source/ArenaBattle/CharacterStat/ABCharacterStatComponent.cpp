// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"

#include "GameData/ABGameSingleton.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	//tick이 필요없는 컴포넌트 이므로 다음의 값을 비활성화 한다.
	//PrimaryComponentTick.bCanEverTick = true;
	CurrentLevel = 1;
	AttackRadius = 50.f;

	//InitializeComponent() 함수를 호출하고 싶으면 이 변수를 true로 변경해주어야 한다.
	bWantsInitializeComponent = true;
}


void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetLevelStat(CurrentLevel);
	SetHp(BaseStat.MaxHp);
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevel)
{
	CurrentLevel = FMath::Clamp(InNewLevel, 1, UABGameSingleton::Get().GetCharacterMaxLevel());
	SetBaseStat(UABGameSingleton::Get().GetCharacterStat(InNewLevel));
	check(0.0f < BaseStat.MaxHp);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);

	//FLOAT_EPSILON이랑 비슷한 의미로 사용하는듯
	if (CurrentHp <= KINDA_SMALL_NUMBER) 
	{
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UABCharacterStatComponent::HealHp(float InHealAmount)
{
	if (0 < InHealAmount)
	{
		CurrentHp = FMath::Clamp(CurrentHp + InHealAmount, 0.f, GetMaxHp());
		OnHpChanged.Broadcast(CurrentHp);
	}
}

void UABCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.f, BaseStat.MaxHp);

	//체력 변경 시 등록된 델리게이트 함수를 호출한다.
	OnHpChanged.Broadcast(CurrentHp);
}
