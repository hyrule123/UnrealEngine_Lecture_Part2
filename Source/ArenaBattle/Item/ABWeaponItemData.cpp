// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABWeaponItemData.h"

UABWeaponItemData::UABWeaponItemData()
{
	Type = EItemType::Weapon;

	static ConstructorHelpers::FObjectFinder<UTexture2D> Img(TEXT("/Script/Engine.Texture2D'/Game/Icons/sword_185.sword_185'"));
	ensure(Img.Succeeded());

	Super::IconImage = Img.Object;
}
