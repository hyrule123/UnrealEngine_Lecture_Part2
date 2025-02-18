// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/ABPlayerController.h"

#include "UI/ABHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ABSaveGame.h"

DEFINE_LOG_CATEGORY(LogABPlayerController);

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

	//플레이어 컨트롤러처럼, 자신의 인덱스에 해당하는 번호에서 저장 데이터를 불러올 수 있다.

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

	SaveGameInstance = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Player0"), 0));

	if (nullptr == SaveGameInstance)
	{
		SaveGameInstance = NewObject<UABSaveGame>();
		SaveGameInstance->RetryCount = 0;
	}
	
	K2_OnGameRetryCountChanged(SaveGameInstance->RetryCount);
}

void AABPlayerController::AddRetryCount()
{
	SaveGameInstance->RetryCount++;
	K2_OnGameRetryCountChanged(SaveGameInstance->RetryCount);
}

void AABPlayerController::ScoreChanged(int32 CurrentScore)
{
	K2_OnScoreChanged(CurrentScore);
}

void AABPlayerController::GameClear()
{
	//클리어 시에는 Retry Count를 초기화
	SaveGameInstance->RetryCount = 0;
	K2_OnGameClear();
}

void AABPlayerController::GameOver()
{
	//게임 데이터를 저장
	check(SaveGameInstance);
	SaveGameInstance->RetryCount++;
	UE_LOG(LogABPlayerController, Log, TEXT("GAME SAVING..."));
	if (false == UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Player0"), 0))
	{
		UE_LOG(LogABPlayerController, Error, TEXT("FAILED TO SAVE GAME"));
	}
	
	//버튼 눌렀을떄 실행되도록(BP) 변경
	//K2_OnGameRetryCountChanged(SaveGameInstance->RetryCount);
	K2_OnGameOver();
}
