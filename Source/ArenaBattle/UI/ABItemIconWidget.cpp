// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/ABItemIconWidget.h"

UABItemIconWidget::UABItemIconWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UABItemIconWidget::SetIconImage(class UTexture2D* InImage)
{
	K2_OnSetIconImage(InImage);
}
