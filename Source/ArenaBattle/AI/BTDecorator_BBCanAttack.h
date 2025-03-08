// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_BBCanAttack.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTDecorator_BBCanAttack : public UBTDecorator_Blackboard
{
	GENERATED_BODY()
	
public:
	UBTDecorator_BBCanAttack();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	/*
	* 관찰자 중단(태스크 노드가 실행 중에도 중단이 가능하며 이 함수에 진입할 수 있음)

	* 참고사항
		이 함수는 태스크 노드가 '실행 중'에 관찰자가 해당 태스크를 중단함과 동시에 호출되는 함수이다.
		다시 말해, 태스크 노드가 '실행 중이지 않을 때'는 관찰자 노드가 작동하지 않는다는 뜻이다.
		예를 들어서, 'foo'라는 bool 값이 true일 때 작동하는 노드라고 치자
		관찰자 중단은 단순히 보면 foo값이 true -> false, false -> true 값으로 변경되는 경우에 모두 실행되어야 하겠지만
		실제로는 true -> false로 변경될 때만 이 함수가 호출된다.

		이유는 상술한 바와 같이 foo값이 true일 때에만 활성화되는 노드이기 때문이다.
		true일때만 활성화 -> false로 변하면 관찰자에서 해당 노드를 중단 -> OnValueChanged 함수를 호출
	*/
	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) override;
};
