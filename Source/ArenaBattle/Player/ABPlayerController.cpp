// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//해당 클래스에는 Game Only 입력모드에 대한 설정값이 들어있다.
	FInputModeGameOnly GameOnlyInputMode;

	//이 값을 SetInputMode에 넘겨준다.
	SetInputMode(GameOnlyInputMode);
}
