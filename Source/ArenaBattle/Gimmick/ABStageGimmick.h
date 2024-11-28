// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

//상태 관리
UENUM(BlueprintType)
enum class EStageState : uint8
{
	Ready = 0,
	Fight,
	Reward,
	Next
};

DECLARE_DELEGATE(FOnStageChangedDelegate);
USTRUCT(BlueprintType)
struct FStageChangedDelegateWrapper
{
	GENERATED_BODY()
	FStageChangedDelegateWrapper() 
	{}
	FStageChangedDelegateWrapper(const FOnStageChangedDelegate& InDelegate)
		: StageDelegate(InDelegate) 
	{}

	FOnStageChangedDelegate StageDelegate;
};

UCLASS()
class ARENABATTLE_API AABStageGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABStageGimmick();

public:
	int32 GetStageNum() const { return CurrentStageNum; }
	void SetStageNum(int32 NewStageNum) { CurrentStageNum = NewStageNum; }

protected:
	//초기화되기 이전, 생성 직후 호출.
	//에디터에서 설정한 클래스 정보도 반영된다.
	//https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-actor-lifecycle?application_version=5.0
	virtual void OnConstruction(const FTransform& Transform) override;

//Stage Section
protected:
	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Stage;

	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> StageTrigger;

	//OnTriggerBegin delegate에 등록할 함수
	UFUNCTION()
	void OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

//Gate Section
protected:
	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TMap<FName, TObjectPtr<class UStaticMeshComponent>> Gates;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UBoxComponent>> GateTriggers;

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	void OpenAllGates();
	void CloseAllGates();

//State Section
protected:
	UPROPERTY(EditAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	EStageState CurrentState;

	void SetState(EStageState InNewState);

	UPROPERTY()
	TMap<EStageState, FStageChangedDelegateWrapper> StateChangeActions;

	//Delegate 등록용 함수
	void SetReady();	//플레이어 진입 시 전투 준비
	void SetFight();
	void SetChooseReward();
	void SetChooseNext();

//Fight Section
protected:
	//TSubclassOf
	// 스폰시킬 액터의 클래스를 지정한 클래스로 한정하여 선택 가능하도록 제한
	UPROPERTY(EditAnywhere, Category = Fight, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AABCharacter_NonPlayer> OpponentClass;

	UPROPERTY(EditAnywhere, Category = Fight, Meta = (AllowPrivateAccess = "true"))
	float OpponentSpawnTime;

	//NPC 사망 시
	UFUNCTION()
	void OnOpponentDestroyed(AActor* DestroyedActor);

	FTimerHandle OpponentTimerHandle;
	void OnOpponentSpawn();

//Reward Section
protected:
	//스폰할 박스
	UPROPERTY(VisibleAnywhere, Category = Reward, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AABItemBox> RewardBoxClass;

	//약참조(weak_ptr)
	//액터와 무관하게 동작해야 하는 다른 액터의 경우에는 약참조를 사용한다.
	UPROPERTY(VisibleAnywhere, Category = Reward, Meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<class AABItemBox>> RewardBoxes;

	TMap<FName, FVector> RewardBoxLocations;

	UFUNCTION()
	void OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	void SpawnRewardBoxes();

//Stage Stat
protected:
	//스테이지가 생길떄마다 +1(레벨 지정에 사용
	UPROPERTY(VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentStageNum;
};
