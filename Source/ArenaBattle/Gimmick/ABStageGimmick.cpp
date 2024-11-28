// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Physics/ABCollision.h"
#include "Character/ABCharacter_NonPlayer.h"
#include "Item/ABItemBox.h"
#include "Engine/OverlapResult.h"

// Sets default values
AABStageGimmick::AABStageGimmick()
{
	// Stage Section
	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	RootComponent = Stage;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE'"));
	if (StageMeshRef.Object)
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
	}

	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTrigger->SetBoxExtent(FVector(775.0, 775.0f, 300.0f));
	StageTrigger->SetupAttachment(Stage);
	StageTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnStageTriggerBeginOverlap);

	// Gate Section
	const static FName GateSockets[] = { TEXT("+XGate"), TEXT("-XGate"), TEXT("+YGate"), TEXT("-YGate") };
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_GATE.SM_GATE'"));
	for (const FName& GateSocket : GateSockets)
	{
		UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GateSocket);
		Gate->SetStaticMesh(GateMeshRef.Object);
		Gate->SetupAttachment(Stage, GateSocket);
		Gate->SetRelativeLocation(FVector(0.0f, -80.5f, 0.0f));
		Gate->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		Gates.Add(GateSocket, Gate);

		FName TriggerName = *GateSocket.ToString().Append(TEXT("Trigger"));
		UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		GateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		GateTrigger->SetupAttachment(Stage, GateSocket);
		GateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnGateTriggerBeginOverlap);
		GateTrigger->ComponentTags.Add(GateSocket);

		GateTriggers.Add(GateTrigger);
	}

	// State Section
	CurrentState = EStageState::Ready;
	StateChangeActions.Add(EStageState::Ready, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetReady)));
	StateChangeActions.Add(EStageState::Fight, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetFight)));
	StateChangeActions.Add(EStageState::Reward, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseReward)));
	StateChangeActions.Add(EStageState::Next, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseNext)));

	// Fight Section
	OpponentSpawnTime = 2.0f;
	OpponentClass = AABCharacter_NonPlayer::StaticClass();

	// Reward Section: Gate Socket 위치를 참조해 스폰 위치를 계산한다.
	RewardBoxClass = AABItemBox::StaticClass();
	for (FName GateSocket : GateSockets)
	{
		FVector BoxLocation = Stage->GetSocketLocation(GateSocket) / 2;
		RewardBoxLocations.Add(GateSocket, BoxLocation);
	}
}

void AABStageGimmick::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetState(CurrentState);
}

void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetState(EStageState::Fight);
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	check(OverlappedComponent->ComponentTags.Num() == 1);
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	check(Stage->DoesSocketExist(SocketName));

	FVector NewLocation = Stage->GetSocketLocation(SocketName);
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(GateTrigger), false, this);
	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams::InitType::AllObjects,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParam
	);

	if (!bResult)
	{
		FTransform NewTransform(NewLocation);
		AABStageGimmick* NewGimmick = GetWorld()->SpawnActorDeferred<AABStageGimmick>(AABStageGimmick::StaticClass(), NewTransform);
		if (NewGimmick) 
		{
			NewGimmick->SetStageNum(CurrentStageNum + 1);
			NewGimmick->FinishSpawning(NewTransform);
		}
	}

	CurrentStageNum = 0;
}

void AABStageGimmick::OpenAllGates()
{
	for (auto Gate : Gates)
	{
		(Gate.Value)->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void AABStageGimmick::CloseAllGates()
{
	for (auto Gate : Gates)
	{
		(Gate.Value)->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void AABStageGimmick::SetState(EStageState InNewState)
{
	CurrentState = InNewState;

	if (StateChangeActions.Contains(InNewState))
	{
		StateChangeActions[CurrentState].StageDelegate.ExecuteIfBound();
	}
}

void AABStageGimmick::SetReady()
{
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	for (const auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	OpenAllGates();
}

void AABStageGimmick::SetFight()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (const auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGates();

	GetWorld()->GetTimerManager().SetTimer(OpponentTimerHandle, this, &AABStageGimmick::OnOpponentSpawn, OpponentSpawnTime, false);
}

void AABStageGimmick::SetChooseReward()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (const auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGates();
	SpawnRewardBoxes();
}

void AABStageGimmick::SetChooseNext()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	}

	OpenAllGates();
}

void AABStageGimmick::OnOpponentDestroyed(AActor* DestroyedActor)
{
	SetState(EStageState::Reward);
}

void AABStageGimmick::OnOpponentSpawn()
{
	const FTransform SpawnLocation(GetActorLocation() + FVector::UpVector * 88.0f);
	AABCharacter_NonPlayer* ABOpponentCharacter = GetWorld()->SpawnActorDeferred<AABCharacter_NonPlayer>(OpponentClass, SpawnLocation);

	if (ABOpponentCharacter)
	{
		ABOpponentCharacter->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOpponentDestroyed);
		ABOpponentCharacter->SetLevel(CurrentStageNum);

		//SpawnActorDeferred를 호출한 경우 반드시 FinishSpawning을 통해 준비가 완료되었음을 알려주어야 한다.
		ABOpponentCharacter->FinishSpawning(SpawnLocation);
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for (const auto& RewardBox : RewardBoxes)
	{
		//약참조이므로 반드시 박스 포인터가 유효한지 검사 후 사용
		if (RewardBox.IsValid())
		{
			AABItemBox* ValidItemBox = RewardBox.Get();
			AActor* OverlappedBox = OverlappedComponent->GetOwner();

			//한 가지 리워드만 선택 가능(나머지 리워드는 모두 제거)
			if (OverlappedBox != ValidItemBox)
			{
				ValidItemBox->Destroy();
			}
		}
	}

	//다음 스테이트로 상태 전환
	SetState(EStageState::Next);
}

void AABStageGimmick::SpawnRewardBoxes()
{
	for (const auto& RewardBoxLocation : RewardBoxLocations)
	{
		FTransform SpawnTransform(GetActorLocation() + RewardBoxLocation.Value + FVector(0.0f, 0.0f, 30.0f));

		AABItemBox* RewardBoxActor = GetWorld()->SpawnActorDeferred<AABItemBox>(RewardBoxClass, SpawnTransform);
		if (RewardBoxActor)
		{
			RewardBoxActor->Tags.Add(RewardBoxLocation.Key);
			RewardBoxActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnRewardTriggerBeginOverlap);
			RewardBoxes.Add(RewardBoxActor);
		}
	}

	for (const auto& RewardBox : RewardBoxes) 
	{
		if (RewardBox.IsValid()) 
		{
			RewardBox.Get()->FinishSpawning(RewardBox.Get()->GetActorTransform());
		}
	}
}