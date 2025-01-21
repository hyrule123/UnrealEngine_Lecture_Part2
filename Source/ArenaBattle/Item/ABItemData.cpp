// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABItemData.h"

UABItemData::UABItemData()
{
	Type = EItemType::END;
}

void UABItemData::PostInitProperties()
{
	Super::PostInitProperties();

//개발용 빌드에서
#if !UE_BUILD_SHIPPING
	//파생 클래스인데 아이템 타입이 지정되지 않은 경우 에러를 발생시킨다.
	if (GetClass() != UABItemData::StaticClass())
	{
		checkf(Type != EItemType::END, TEXT("아이템 타입이 지정되지 않음."));
	}
#endif
}
