// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

	TObjectPtr<class UBoxComponent> GetTrigger() const { return Trigger; }

protected:
	//컴포넌트가 초기화된 이후 애셋 매니저에서 전체 애셋 목록을 받아온 뒤,
	//랜덤 아이템을 추가한다.
	virtual void PostInitializeComponents() override;

protected:
	//캐릭터가 닿으면 작동할 트리거 충돌체
	UPROPERTY(VisibleAnywhere, Category = "Box")
	TObjectPtr<class UBoxComponent> Trigger;

	//박스 모델링
	UPROPERTY(VisibleAnywhere, Category = "Box")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	//트리거 발동시 발동할 이펙트
	UPROPERTY(VisibleAnywhere, Category = "Effect")
	TObjectPtr<class UParticleSystemComponent> Effect;

	UPROPERTY(EditAnywhere, Category = "Item")
	TObjectPtr<class UABItemData> Item;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<class UBillboardComponent> ItemIcon;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<class UWidgetComponent> WidgetCom;

	//PrimitiveComponent에 있는 델리게이트 함수 선언
	//OnComponentBeginOverlap이라는 델리게이트 q변수에 등록할 수 있다.
	//블루프린트에서도 사용 가능한 Dynamic Delegete이므로 UFUNCION으로 지정이 필요하다.
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* ParticleSystem);
};
