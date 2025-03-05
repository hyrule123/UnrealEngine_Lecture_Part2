// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "Interface/ABCharacterAIInterface.h"
#include "Engine/StreamableManager.h"	//TSTreamableHandle
#include "ABCharacter_NonPlayer.generated.h"

/**
 * 
 */

//DefaultArenaBattle.ini 파일을 사용하겠다고 전달
UCLASS(config=ArenaBattle)
class ARENABATTLE_API AABCharacter_NonPlayer 
	: public AABCharacterBase
	, public IABCharacterAIInterface
{
	GENERATED_BODY()
public:
	AABCharacter_NonPlayer();

protected:
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void SetDead() override;

	//비동기 로딩이 완료되었을 때 호출할 함수
	void NPCMeshLoadCompleted();

	//config 파일로부터 불러오겠다고 전달
	//주소를 저장해 놓고, 필요할때 로딩하기 위한 용도
	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	//비동기 로딩을 위한 핸들
	TSharedPtr<FStreamableHandle> NPCMeshHandle;

//AI Section
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;
	virtual void AttackStop() override;

	FAICharacterAttackFinished OnAttackFinished;

	virtual void NotifyComboActionEnd() override;
};
