// Fill out your copyright notice in the Description page of Project Settings.
#include "Item/ABItemBox.h"

#include "Physics/ABCollision.h"
#include "Interface/ABCharacterItemInterface.h"
#include "Item/ABItemData.h"
#include "UI/ABItemIconWidget.h"

#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/AssetManager.h"

// Sets default values
AABItemBox::AABItemBox()
{
	//SubObject 생성
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	//트리거를 루트 컴포넌트로 지정 후 나머지를 갖다 붙임.
	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
	Effect->SetupAttachment(Trigger);

	//지난번에 생성했던 Arena Battle용 충돌 프로필로 지정
	Trigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));

	//상자 메쉬는 준비된 메쉬를 사용한다.
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'"));
		ensure(BoxMeshRef.Succeeded());
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}
	
	Mesh->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	//Effect 지정
	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/ArenaBattle/Effect/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'"));
		ensure(EffectRef.Succeeded());

		Effect->SetTemplate(EffectRef.Object);
		Effect->bAutoActivate = false;//자동 실행 x
	}

	WidgetCom = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemIconWidget"));
	WidgetCom->SetupAttachment(Trigger);

	static ConstructorHelpers::FClassFinder<UABItemIconWidget> ItemIconClassInfo(TEXT("/Game/ArenaBattle/UI/WBP_ABItemIcon.WBP_ABItemIcon_C"));
	check(ItemIconClassInfo.Succeeded());
	WidgetCom->SetWidgetClass(ItemIconClassInfo.Class);

	WidgetCom->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetCom->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetCom->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UAssetManager& Mgr = UAssetManager::Get();

	//Assets 배열에 "ABItemData"라는 태그를 가진 모든 애셋을 담아줌.
	TArray<FPrimaryAssetId> Assets;
	Mgr.GetPrimaryAssetIdList(TEXT("ABItemData"), Assets);
	
	if (false == Assets.IsEmpty()) 
	{
		//그럼 여기서 랜덤하게 하나 골라서 아이템을 지정해주면 됨
		int32 RandIdx = FMath::RandRange(0, Assets.Num() - 1);

		//SoftObjectPtr에 AssetPath를 넘겨주면 오브젝트의 주소를 확인 가능.
		TSoftObjectPtr<UObject> AssetPtr(Mgr.GetPrimaryAssetPath(Assets[RandIdx]));
		if (AssetPtr.IsPending()) 
		{
			AssetPtr.LoadSynchronous();
		}

		//Item 변수에 등록.
		Item = Cast<UABItemData>(AssetPtr.Get());
		ensure(Item);

		if (Item->GetItemImage())
		{
			WidgetCom->InitWidget();

			UABItemIconWidget* IconWidget = Cast<UABItemIconWidget>(WidgetCom->GetWidget());
			IconWidget->SetIconImage(Item->GetItemImage());

			UE_LOG(LogTemp, Log, TEXT("ITEM IMAGE!!"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("NO ITEM IMAGE!!"));
		}
	}
	else {
		ensure(false == Assets.IsEmpty());
	}

	//참고)매크로라서 인텔리센스는 오류를 띄움.
	//SpawnActorDeferred로 변경하면서 델리게이트 등록을 뒤로 미룸
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnOverlapBegin);
}


void AABItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	//꽝
	if (nullptr == Item) 
	{
		Destroy();
		return;
	}

	IABCharacterItemInterface* ItemInterface = Cast<IABCharacterItemInterface>(OtherActor);
	if (ItemInterface)
	{
		ItemInterface->TakeItem(Item);
	}

	//이펙트 발동시키고
	Effect->Activate(true);
	//메쉬 숨기고
	Mesh->SetHiddenInGame(true);
	//충돌체크 중단
	SetActorEnableCollision(false);

	//이펙트 끝나고 호출할 델리게이트 등록(자신 제거)
	Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	Destroy();
}

