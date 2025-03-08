#include "AI/BTDecorator_BBCanAttack.h"
// Fill out your copyright notice in the Description page of Project Settings.

//Proj Header
#include "AI/ABAI.h"
#include "AI/ABAIController.h"
#include "Interface/ABCharacterAIInterface.h"

//Unreal Header
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UBTDecorator_BBCanAttack::UBTDecorator_BBCanAttack()
{
	BlackboardKey.AddNameFilter(this, BBKEY_CAN_ATTACK);
}

bool UBTDecorator_BBCanAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (false == Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		return false;
	}

	//블랙보드에서 현재 공격 가능한지 정보를 가져온다.
	return OwnerComp.GetBlackboardComponent()->GetValue< UBlackboardKeyType_Bool>(BlackboardKey.GetSelectedKeyID());
}

EBlackboardNotificationResult UBTDecorator_BBCanAttack::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	Super::OnBlackboardKeyValueChange(Blackboard, ChangedKeyID);

	IABCharacterAIInterface* ControllingPawn = nullptr;
	if (Blackboard.GetOwner<AABAIController>())
	{
		ControllingPawn =
			Blackboard.GetOwner<AABAIController>()->GetPawn< IABCharacterAIInterface>();
	}

	if (nullptr == ControllingPawn)
	{
		return EBlackboardNotificationResult::ContinueObserving;
	}

	bool CanAttack = Blackboard.GetValue<UBlackboardKeyType_Bool>(ChangedKeyID);

	//false -> true
	if (CanAttack)
	{
		UE_LOG(LogTemp, Log, TEXT("CanAttack"));

		ControllingPawn->AI_Attack();
	}

	//true -> false
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Cannot Attack"));

		ControllingPawn->AI_Idle();
	}

	return EBlackboardNotificationResult::ContinueObserving;
}
