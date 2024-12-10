// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/BTService_Detect.h"

#include "AIController.h"
#include "Interface/ABCharacterAIInterface.h"
#include "Physics/ABCollision.h"
#include "AI/ABAI.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) { return; }

	FVector Center = ControllingPawn->GetActorLocation();
	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) { return; }

	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn) { return; }

	float DetectRadius = AIPawn->GetAIDetectRange();
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		CCHANNEL_ABACTION,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	APawn* FoundPlayer = nullptr;
	if (bResult)
	{
		for (const auto& OverlapResult : OverlapResults)
		{
			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
			//플레이어를 찾았을 시
			if (Pawn && Pawn->GetController()->IsPlayerController())
			{
				FoundPlayer = Pawn;
				break;

			}
		}
	}

	//타겟 지정
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, FoundPlayer);

	if (FoundPlayer)
	{
		//디버그 드로잉
		DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, Super::Interval);
		DrawDebugPoint(World, FoundPlayer->GetActorLocation(), 10.f, FColor::Green, false, 0.2f);
		DrawDebugLine(World, ControllingPawn->GetActorLocation(), FoundPlayer->GetActorLocation(), FColor::Green, false, 0.27f);
	}
	//플레이어를 찾지 못했을 시
	else
	{
		DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, Super::Interval);
	}
}
