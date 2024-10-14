// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/ABHpBarWidget.h"

#include "Components/ProgressBar.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//설정하지 않았을 경우 문제 발생시키기 위해 -1로 설정
	MaxHp = -1.f;
}

void UABHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//블루프린트 클래스
	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HpBar")));
	ensure(HpProgressBar);
}

void UABHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	//체력바 잘 적용됐는지 확인
	ensure(MaxHp > 0.f);
	if (HpProgressBar)
	{
		//함수를 사용하려면 헤더("Components/ProgressBar.h") 
		//및 모듈("UMG") 추가 필수
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}

}
