// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABItemData.generated.h"

//BlueprintType으로 지정하면 블루프린터에서 해당 값을 설정할 수 있다.
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon = 0,
	Potion,
	Scroll,
	END
};

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	//UPrimaryDataAsset 클래스에 정의된 Asset Id를 반환해주는 함수
	virtual FPrimaryAssetId GetPrimaryAssetId() const override 
	{
		//두번째 인자는 보통 GetFName으로 지정을 함
		return FPrimaryAssetId(TEXT("ABItemData"), GetFName());
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EItemType Type;
};
