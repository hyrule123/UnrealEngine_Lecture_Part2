// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABUserWidget.generated.h"

/**
 * Actor 주소를 받아놓고, 나중에 사용하기 위한 UserWidget 확장
 * 5.1 버전에선 자신을 소유한 캐릭터 액터의 주소를 받아오는 API가 준비되어 있지 않다고 한다.
 * 그래서 WidgetComponent에서 캐릭터 액터의 주소를 받아놓고, 이 클래스에 그 주소를 전달해준다.
 * 해당 주소를 저장하기 위한 확장 클래스
 */
UCLASS()
class ARENABATTLE_API UABUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetOwningActor(AActor* NewOwner) {
		OwningActor = NewOwner;
	}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor")
	TObjectPtr<AActor> OwningActor;
};
