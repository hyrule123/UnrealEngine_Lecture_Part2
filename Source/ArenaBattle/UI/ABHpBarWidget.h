// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABHpBarWidget.generated.h"

/**
 * 이 클래스를 직접 생성하는 것이 아니라,
 * 이 클래스를 상속받은 "블루프린트 클래스" 를 생성할 예정임.
 */
UCLASS()
class ARENABATTLE_API UABHpBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	//UUserWidget에서 제공하는 가상함수
	//이 함수가 호출 될 즈음이면 UI 오브젝트들의 초기화 작업이 완료된 상황이므로
	//이 때 ProgressBar 포인터를 가져오면 된다.
	virtual void NativeConstruct() override;

public:
	void SetMaxHp(float InMaxHp) { MaxHp = InMaxHp; }
	void UpdateHpBar(float NewCurrentHp);

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
