// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABScrollItemData.h"

UABScrollItemData::UABScrollItemData()
{
	Type = EItemType::Scroll;

	static ConstructorHelpers::FObjectFinder<UTexture2D> Img(TEXT("/Script/Engine.Texture2D'/Game/Icons/olden_treasure_map.olden_treasure_map'"));
	ensure(Img.Succeeded());

	Super::IconImage = Img.Object;
}
