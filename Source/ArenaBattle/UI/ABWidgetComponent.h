// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ABWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
protected:
	/*
	* 이 함수는 WidgetComponent에 등록된 위젯의 인스턴스가 생성된 이후에 호출된다.
	* 또한 캐릭터 액터의 BeginPlay() 이후에 호출된다.
	* 캐릭터 액터의 BeginPlay() 함수에서는 캐릭터 액터 컴포넌트 자신의 주소를 위젯 인스턴스에게 전달해 줄 방법이 없다는 것.
	* 이 클래스는 위젯 인스턴스에게 주소를 전달해주기 위해 만들어졌다.
	*/
	virtual void InitWidget() override;
};
