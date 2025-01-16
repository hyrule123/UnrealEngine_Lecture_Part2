// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/ABHUDWidget.h"

#include "UI/ABHpBarWidget.h"
#include "UI/ABCharacterStatWidget.h"
#include "Interface/ABCharacterHUDInterface.h"

UABHUDWidget::UABHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UABHUDWidget::UpdateStatWidget(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	FABCharacterStat TotalStat = BaseStat + ModifierStat;
	HpBar->SetMaxHp(TotalStat.MaxHp);
	
	CharacterStatWidget->UpdateStatWidget(BaseStat, ModifierStat);
}

void UABHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	HpBar->UpdateHpBar(NewCurrentHp);
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//WBP_ABHUD 블루프린트에서 만들었던 체력바와 스탯창을 거기서 설정한 이름을 통해서 가져온다.
	HpBar = Cast<UABHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	CharacterStatWidget = Cast<UABCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));

	ensure(HpBar);
	ensure(CharacterStatWidget);

	//GetOwningPlayerPawn() 함수를 통해서 컨트롤러가 조종중인 폰을 가져올 수 있다.
	//인터페이스 함수를 통해서 명령을 전달하도록 구현할 예정
	IABCharacterHUDInterface* HUDPawn = Cast<IABCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}
