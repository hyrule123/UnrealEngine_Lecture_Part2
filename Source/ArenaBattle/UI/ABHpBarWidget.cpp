// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/ABHpBarWidget.h"

#include "Interface/ABCharacterWidgetInterface.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"



UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//설정하지 않았을 경우 문제 발생시키기 위해 -1로 설정
	CurHp = MaxHp = -1.f;

}

void UABHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//블루프린트 클래스
	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HpBar")));
	ensure(HpProgressBar);

	HpText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtHpStat")));
	ensure(HpText);

	//자신을 소유한 캐릭터 액터의 컴포넌트 주소를 받아온다.
	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UABHpBarWidget::SetMaxHp(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	MaxHp = BaseStat.MaxHp + ModifierStat.MaxHp;
	ensure(0 < MaxHp);
	UpdateHpBar();
	UpdateHpText();
}

void UABHpBarWidget::SetCurHp(float InCurHp)
{
	ensure(0 <= InCurHp);
	CurHp = FMath::Clamp(InCurHp, 0.0f, MaxHp);
	UpdateHpBar(); 
	UpdateHpText();
}

void UABHpBarWidget::UpdateHpBar()
{
	if (HpProgressBar)
	{
		//함수를 사용하려면 헤더("Components/ProgressBar.h") 
		//및 모듈("UMG") 추가 필수
		HpProgressBar->SetPercent(CurHp / MaxHp);
	}
}

void UABHpBarWidget::UpdateHpText()
{
	if (HpText)
	{
		FString NewText = FString::Printf(TEXT("%d / %d"), (int)CurHp, (int)MaxHp);
		HpText->SetText(FText::FromString(NewText));
	}
}

