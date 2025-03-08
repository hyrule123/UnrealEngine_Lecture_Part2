// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/BTTask_Attack.h"

#include "AI/ABAI.h"
#include "Interface/ABCharacterAIInterface.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	bTickIntervals = true;
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

	////선입력시간이 종료되면 실행될 함수
	//FOnReserveTimeEndDelegate OnReserveTimeFinished;
	//OnReserveTimeFinished.BindLambda(
	//	[&](ECharacterAction NextAction)->void
	//	{
	//		UE_LOG(LogTemp, Log, TEXT("OnReserveTimeEnd"));
	//		
	//		//만약 다음 액션으로 Attack이 정해졌을 경우
	//		//공격 범위 내에 있는지 다시 확인을 해야 한다
	//		//-> 현재 노드를 종료하고 재시작 한다.
	//		if (NextAction == ECharacterAction::Attack)
	//		{
	//			UE_LOG(LogTemp, Log, TEXT("CheckedFalse"));
	//			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	//		}
	//	}
	//);
	//AIPawn->SetAIReserveTimeFinishedDelegate(OnReserveTimeFinished);

	//공격이 끝났을때 시행할 행동 바인딩
	FSimpleDelegate OnAttackFinished;
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
	SetNextTickTime(NodeMemory, 0.1f);
	
	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn) { return; }

	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn) { return; }

	AIPawn->AI_Attack();
}
