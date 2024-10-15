// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	//tick이 필요없는 컴포넌트 이므로 다음의 값을 비활성화 한다.
	//PrimaryComponentTick.bCanEverTick = true;

	MaxHp = 200.f;
	SetHp(MaxHp);
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHp(MaxHp);
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

void UABCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.f, MaxHp);

	//체력 변경 시 등록된 델리게이트 함수를 호출한다.
	OnHpChanged.Broadcast(CurrentHp);
}