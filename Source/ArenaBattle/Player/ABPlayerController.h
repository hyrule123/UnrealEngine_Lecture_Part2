// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABPlayerController, Log, All);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AABPlayerController();
public:
	virtual void BeginPlay() override;

	/*
	K2: 블루프린트의 전신 격인 Kismet 기능에 사용하던 접두사
	접두사만 붙여서 다른 함수를 만들어주어야 하기 때문에
	예전에 사용하던 시스템인 Kismet에서 따온
	K2_라는 접두사를 관습적으로 사용한다.
	또한 매크로를 통해서 BluePrint에서 구현할 함수라는 것을 인식하기 때문에
	굳이 정의부를 만들지 않아도 문제가 발생하지 않는다.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnScoreChangedCpp"))
	void K2_OnScoreChanged(int32 CurrentScore);

	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameClearCpp"))
	void K2_OnGameClear();

	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameOverCpp"))
	void K2_OnGameOver();

	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameRetryCountChangedCpp"))
	void K2_OnGameRetryCountChanged(int32 NewRetryCount);

	void ScoreChanged(int32 CurrentScore);
	void GameClear();
	void GameOver();

//HUD Section
protected:
	//Blueprint로 생성하는 것으로 변경하였음
	/*
	//참고: TSubclassOf: 컴파일타임 + 런타임 타입안정성 보장받는 클래스 정보 보관소
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UABHUDWidget> ABHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class UABHUDWidget> ABHUDWidget;
	*/

//SaveGame Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGame")
	TObjectPtr<class UABSaveGame> SaveGameInstance;
};

