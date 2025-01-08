// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_AttackInRange.h"

#include "ABAI.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/ABCharacterAIInterface.h"

UBTDecorator_AttackInRange::UBTDecorator_AttackInRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_AttackInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	//이 AI가 조종중인 폰 주소 확인
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	//그 폰이 이 게임의 캐릭터 클래스인지 확인
	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return false;
	}

	//공격 대상(플레이어) 확인
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (nullptr == Target)
	{
		return false;
	}

	float DistanceToTarget = ControllingPawn->GetDistanceTo(Target);
	//인터페이스로부터 거리 받아온다
	float AttackRangeWithRadius = AIPawn->GetAIAttackRange();

	//범위 안에 있는지 결과 반환
	bResult = (DistanceToTarget <= AttackRangeWithRadius);
	return bResult;
}
