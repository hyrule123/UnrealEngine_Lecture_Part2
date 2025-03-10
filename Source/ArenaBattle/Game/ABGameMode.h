// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Interface/ABGameInterface.h"

#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameMode 
	: public AGameModeBase
	, public IABGameInterface
{
	GENERATED_BODY()
public:
	AABGameMode();

public:
	virtual void OnPlayerScoreChanged(int32 NewPlayerScore) override;
	virtual void OnPlayerDead() override;
	virtual bool IsGameCleared() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Game)
	int32 Score_ClearCondition;

	/*
	사실 이 값은 '플레이어'의 점수이기 때문에
	PlayerState에서 보관하는 것이 더 바람직하다.
	*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Game)
	int32 Score_Current;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Game)
	uint8 bIsCleared : 1;
};
