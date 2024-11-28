// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/ABGameSingleton.h"

DEFINE_LOG_CATEGORY(LogABGameSingleton);

UABGameSingleton::UABGameSingleton()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/ArenaBattle/GameData/ABCharacterStatTable.ABCharacterStatTable'"));
	if (nullptr != DataTableRef.Object) 
	{
		//데이터 테이블을 받아온다.
		const UDataTable* DataTable = DataTableRef.Object;
		check(0 < DataTable->GetRowMap().Num());

		//Key-Value 쌍에서 Value만 떼어서 현재 클래스의 변수에 저장한다.
		TArray<uint8*> ValueArray;
		DataTable->GetRowMap().GenerateValueArray(ValueArray);
		Algo::Transform(ValueArray, CharacterStatTable,
			[](uint8* Value)
			{
				return *reinterpret_cast<FABCharacterStat*>(Value);
			}
		);
	}

	//배열의 갯수 = 레벨
	CharacterMaxLevel = CharacterStatTable.Num();
	ensure(0 < CharacterMaxLevel);
}

UABGameSingleton& UABGameSingleton::Get()
{
	//엔진을 통해 인스턴스 생성이 보장되기 떄문에 바로 변환해도 된다.
	//혹시 모르니 CastChecked 수행
	UABGameSingleton* Singleton = CastChecked<UABGameSingleton>(GEngine->GameSingleton);

	if (nullptr == Singleton) {
		UE_LOG(LogABGameSingleton, Error, TEXT("Invalid Game Singleton"));
	}

	return *Singleton;
}
