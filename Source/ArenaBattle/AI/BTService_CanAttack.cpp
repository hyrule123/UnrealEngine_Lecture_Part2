// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/BTService_CanAttack.h"

//My Project Headers
#include "AI/ABAI.h"
#include "AI/ABAIController.h"
#include "Interface/ABCharacterAIInterface.h"

//Unreal Headers
#include "BehaviorTree/BlackboardComponent.h"


UBTService_CanAttack::UBTService_CanAttack()
{
}

void UBTService_CanAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//이 AI가 조종중인 폰 주소 확인
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) { return; }

	//그 폰이 이 게임의 캐릭터 클래스인지 확인
	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn) { return; }

	//공격 대상(플레이어) 확인
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (nullptr == Target) { return; }

	float DistanceToTarget = ControllingPawn->GetDistanceTo(Target);
	//인터페이스로부터 거리 받아온다
	float AttackRangeWithRadius = AIPawn->GetAIAttackRange();

	//범위 안에 있는지 결과 반환
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKEY_CAN_ATTACK, (DistanceToTarget <= AttackRangeWithRadius));
}
