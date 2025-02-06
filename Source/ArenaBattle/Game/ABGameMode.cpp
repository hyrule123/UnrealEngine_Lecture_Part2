// Fill out your copyright notice in the Description page of Project Settings.
#include "Game/ABGameMode.h"
#include "Player/ABPlayerController.h"

//여기서 설정해주어야 하는 것들
//1. DefaultPawnClass
//2. Player/ABPlayerController.h
AABGameMode::AABGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> ThirdPersonClassRef(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/BP_ABCharacterPlayer.BP_ABCharacterPlayer_C'"));
	check(ThirdPersonClassRef.Class);
	DefaultPawnClass = ThirdPersonClassRef.Class;

	//이렇게 헤더를 참조하여 직접 받아올 수도 있고,
	//PlayerControllerClass = AABPlayerController::StaticClass();

	//아래처럼 생성자에서는 ConstructorHelpers 클래스를 활용하여 레퍼런스 문자열을 통해 클래스를 받아올 수도 있다.
	static ConstructorHelpers::FClassFinder<APlayerController> Controller(TEXT("/Script/ArenaBattle.ABPlayerController"));
	check(Controller.Succeeded());
	PlayerControllerClass = Controller.Class;

	ClearScore = 3;
	CurrentScore = 0;
	bIsCleared = false;
}

void AABGameMode::OnPlayerScoreChanged(int32 NewPlayerScore)
{
	CurrentScore = NewPlayerScore;

	//현재는 '싱글 게임' 이기 때문에 플레이어 컨트롤러 중 '첫 번째' 컨트롤러만 받아오면 된다.
	AABPlayerController* ABPlayerController = Cast<AABPlayerController>(GetWorld()->GetFirstPlayerController());
	if (ABPlayerController)
	{
		ABPlayerController->ScoreChanged(CurrentScore);
	}


	if (CurrentScore >= ClearScore)
	{
		bIsCleared = true;

		if (ABPlayerController)
		{
			ABPlayerController->GameClear();
		}
	}
}

void AABGameMode::OnPlayerDead()
{
	AABPlayerController* ABPlayerController = Cast<AABPlayerController>(GetWorld()->GetFirstPlayerController());
	if (ABPlayerController)
	{
		ABPlayerController->GameOver();
	}
}

bool AABGameMode::IsGameCleared()
{
	return bIsCleared;
}
