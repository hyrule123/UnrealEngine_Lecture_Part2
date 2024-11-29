// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/ABAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

AABAIController::AABAIController()
{
	ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/ArenaBattle/AI/BB_ABCharacter.BB_ABCharacter'"));
	ensure(BBAssetRef.Succeeded());
	BBAsset = BBAssetRef.Object;

	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/ArenaBattle/AI/BT_ABCharacter.BT_ABCharacter'"));
	ensure(BTAssetRef.Succeeded());
	BTAsset = BTAssetRef.Object;
}

void AABAIController::RunAI()
{
	//부모 클래스에 Blackboard라는 변수가 존재
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();

	if (UseBlackboard(BBAsset, BlackboardPtr)) {
		bool result = RunBehaviorTree(BTAsset);
		ensure(result);
	}
}

void AABAIController::StopAI()
{
	//부모 클래스의 BrainComponent 변수에 BehaviorTree 주소가 저장된다.
	//변수명을 보면 알겠듯이 BT 외에도 여러가지 AI 방식들을 지원한다.
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent) 
	{
		BTComponent->StopTree();
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}
