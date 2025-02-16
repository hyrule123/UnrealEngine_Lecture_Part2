// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/ABPlayerController.h"

#include "UI/ABHUDWidget.h"

AABPlayerController::AABPlayerController()
{
	/*
	//클래스 레퍼런스 받아온다
	static ConstructorHelpers::FClassFinder<UABHUDWidget> ABHUDWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_ABHUD.WBP_ABHUD_C"));
	if (ABHUDWidgetRef.Succeeded())
	{
		ABHUDWidgetClass = ABHUDWidgetRef.Class;
	}
	*/
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//해당 클래스에는 Game Only 입력모드에 대한 설정값이 들어있다.
	FInputModeGameOnly GameOnlyInputMode;

	//이 값을 SetInputMode에 넘겨준다.
	SetInputMode(GameOnlyInputMode);

	//Blueprint에서 생성하는 것으로 변경하였음.
	/*
	ABHUDWidget = CreateWidget<UABHUDWidget>(this, ABHUDWidgetClass);
	//위젯을 성공적으로 생성했으면 화면에 띄워준다
	if (ABHUDWidget)
	{
		ABHUDWidget->AddToViewport();
	}
	*/
}

void AABPlayerController::ScoreChanged(int32 CurrentScore)
{
	K2_OnScoreChanged(CurrentScore);
}

void AABPlayerController::GameClear()
{
	K2_OnGameClear();
}

void AABPlayerController::GameOver()
{
	K2_OnGameOver();
}
