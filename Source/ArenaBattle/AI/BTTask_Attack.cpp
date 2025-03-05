// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/BTTask_Attack.h"

#include "AI/ABAI.h"
#include "Interface/ABCharacterAIInterface.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
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

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	//공격 타겟이 있는지 확인, 없을 경우 return
	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn) { return; }

	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn) { return; }

	//'공격' 액션은 바로 끝나는 액션이 아님.
	//공격 몽타주가 재생되고, 끝나야 비로소 공격이 끝났다고 할 수 있음.
	if (TargetPawn)
	{
		AIPawn->AttackByAI();
	}
	else
	{
		//AbortTask();
	}
}
