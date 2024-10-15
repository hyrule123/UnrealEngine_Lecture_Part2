// Fill out your copyright notice in the Description page of Project Settings.
#include "Item/ABItemBox.h"

#include "Physics/ABCollision.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

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
	//참고)매크로라서 인텔리센스는 오류를 띄움.
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnOverlapBegin);

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

}

void AABItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
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

