// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	float GetMaxHp() { return MaxHp; }
	float GetCurrentHp() { return CurrentHp; }
	float ApplyDamage(float InDamage);

protected:
	//HP 변경은 반드시 이 함수를 통해서 설정
	void SetHp(float NewHp);

	//인스턴스 별로 달라질 수 있는 값이므로 VisibleInstanceOnly 를 사용한다.
	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHp;

	//Transient: 디스크에 저장할 필요가 없는 변수
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;
};
