// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABItemIconBillboard.h"

UABItemIconBillboard::UABItemIconBillboard()
{
}

void UABItemIconBillboard::SetIconImage(TObjectPtr<class UTexture2D> InIconImage)
{
	IconImage = InIconImage;
}
