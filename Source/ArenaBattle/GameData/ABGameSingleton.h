// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterStat.h"
#include "ABGameSingleton.generated.h"

//로그 카테고리 등록(기본 수준을 Error로 지정)
DECLARE_LOG_CATEGORY_EXTERN(LogABGameSingleton, Error, All);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UABGameSingleton();
	static UABGameSingleton& Get();

//Character Stat Data Section
public:
	FORCEINLINE FABCharacterStat GetCharacterStat(int32 InLevel) const {
		return (CharacterStatTable.IsValidIndex(InLevel) ? CharacterStatTable[InLevel] : FABCharacterStat{});
	}
	int32 GetCharacterMaxLevel() const { return CharacterMaxLevel; }

private:
	UPROPERTY()
	int32 CharacterMaxLevel;

	UPROPERTY()
	TArray<FABCharacterStat> CharacterStatTable;
	
};
