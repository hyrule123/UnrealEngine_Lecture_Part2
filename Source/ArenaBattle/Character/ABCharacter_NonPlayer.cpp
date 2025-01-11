// Fill out your copyright notice in the Description page of Project Settings.
#include "ABCharacter_NonPlayer.h"

#include "Engine/AssetManager.h"
#include "AI/ABAIController.h"
#include "CharacterStat/ABCharacterStatComponent.h"

AABCharacter_NonPlayer::AABCharacter_NonPlayer()
{
	GetMesh()->SetHiddenInGame(true);

	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AABCharacter_NonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ensure(0 < NPCMeshes.Num());

	int32 RandIdx = FMath::RandRange(0, NPCMeshes.Num() - 1);
	NPCMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		NPCMeshes[RandIdx], 
		FStreamableDelegate::CreateUObject(this, &AABCharacter_NonPlayer::NPCMeshLoadCompleted)
	);
}

void AABCharacter_NonPlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimer{};
	GetWorld()->GetTimerManager().SetTimer(
		DeadTimer,
		FTimerDelegate::CreateLambda(	//람다 함수를 통해 빠르게 함수를 생성할 수 있다.
			[&]()
			{
				this->Destroy();
			}),
		DeadDestroyDelayTime,
		false
	);
}

void AABCharacter_NonPlayer::NPCMeshLoadCompleted()
{
	//비동기 로딩이 완료되면 이 함수에 들어오게 된다.
	if (NPCMeshHandle.IsValid()) 
	{
		USkeletalMesh* NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());
		if (NPCMesh)
		{
			GetMesh()->SetSkeletalMesh(NPCMesh);
			GetMesh()->SetHiddenInGame(false);
		}
	}
	NPCMeshHandle->ReleaseHandle();
}

float AABCharacter_NonPlayer::GetAIPatrolRadius()
{
	return 800.f;
}

float AABCharacter_NonPlayer::GetAIDetectRange()
{
	return 400.f;
}

float AABCharacter_NonPlayer::GetAIAttackRange()
{
	return Stat->GetTotalStat().AttackRange + Stat->GetAttackRadius() * 2;
}

float AABCharacter_NonPlayer::GetAITurnSpeed()
{
	return 2.0f;
}

void AABCharacter_NonPlayer::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AABCharacter_NonPlayer::AttackByAI()
{
	ProcessComboCommand();
}

void AABCharacter_NonPlayer::NotifyComboActionEnd()
{
	Super::NotifyComboActionEnd();
	OnAttackFinished.ExecuteIfBound();
}
