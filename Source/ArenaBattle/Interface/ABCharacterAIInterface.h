// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Character/ABCharacter_Common.h"

#include "ABCharacterAIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UABCharacterAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Non-Player의 인터페이스 클래스
 */
class ARENABATTLE_API IABCharacterAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float GetAIPatrolRadius() = 0;
	virtual float GetAIDetectRange() = 0;
	virtual float GetAIAttackRange() = 0;
	virtual float GetAITurnSpeed() = 0;

	/*
	'공격' 액션은 바로 끝나는 액션이 아님.
	공격 몽타주가 재생되고, 끝나야 비로소 공격이 끝났다고 할 수 있음.
	공격 몽타주 재생이 끝났는지를 확인할 수 있는 Delegate를 구현하여 상태정보를 전달하는데 사용
	*/
	virtual void SetAIAttackDelegate(const FSimpleDelegate& InOnAttackFinished) = 0;
	virtual void SetAIReserveTimeFinishedDelegate(const FOnReserveTimeEndDelegate& OnReserveTimeFinished) = 0;
	virtual void AI_Attack() = 0;
	virtual void AI_Idle() = 0;
};
