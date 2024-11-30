// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/BTTask_FindPatrolPos.h"

#include "AI/ABAI.h"
#include "AI/ABAIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (EBTNodeResult::Failed == Super::ExecuteTask(OwnerComp, NodeMemory))
	{
		return EBTNodeResult::Failed;
	}

	APawn* Controlling = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == Controlling) 
	{
		return EBTNodeResult::Failed;
	}

	//조종중인 폰이 위치한 월드를 NavMesh에 전달한다.(어디서 길을 찾을지를 지정하는 듯.)
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(Controlling->GetWorld());

	//현재 위치를 받아온다.
	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_HOMEPOS);
	FNavLocation NextPatrolPos;

	//현재 위치로부터 이동 가능한 범위 내에서 랜덤한 위치를 받아온다.
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, 500.f, NextPatrolPos))
	{
		//true가 반환되면 랜덤 위치를 받아온것이다.
		//이걸 행동트리의 블랙보드에 전달한다.
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
