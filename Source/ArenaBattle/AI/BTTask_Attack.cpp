// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/BTTask_Attack.h"
#include "Interface/ABCharacterAIInterface.h"

#include "AIController.h"

UBTTask_Attack::UBTTask_Attack()
{
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	if (EBTNodeResult::Succeeded != Result) 
	{
		return Result;
	}

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn) 
	{
		return EBTNodeResult::Failed;
	}

	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn) 
	{
		return EBTNodeResult::Failed;
	}

	//공격이 끝났을때 시행할 행동 바인딩
	FAICharacterAttackFinished OnAttackFinished;
	OnAttackFinished.BindLambda(
		[&]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	);
	AIPawn->SetAIAttackDelegate(OnAttackFinished);

	//'공격' 액션은 바로 끝나는 액션이 아님.
	//공격 몽타주가 재생되고, 끝나야 비로소 공격이 끝났다고 할 수 있음.
	AIPawn->AttackByAI();
	return EBTNodeResult::InProgress;
}
