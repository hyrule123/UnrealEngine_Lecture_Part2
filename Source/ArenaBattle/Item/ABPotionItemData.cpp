// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABPotionItemData.h"
#include "ABPotionItemData.h"

UABPotionItemData::UABPotionItemData()
{
	Super::Type = EItemType::Potion;

	static ConstructorHelpers::FObjectFinder<UTexture2D> Img(TEXT("/Script/Engine.Texture2D'/Game/Icons/MediumHealthPotion.MediumHealthPotion'"));
	ensure(Img.Succeeded());

	Super::IconImage = Img.Object;
}
