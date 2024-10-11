// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABCharacterBase.generated.h"

UENUM()
enum class ECameraViewMode : uint32
{
	Shoulder,
	Quarter,
	END
};

class UABCharacterControlData;

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
	virtual void BeginPlay() override;

	virtual void SetCharacterControlData(const UABCharacterControlData* ControlData);

protected:
	void SetCameraViewMode(ECameraViewMode Mode);
	ECameraViewMode GetCameraViewMode() { return CurCamViewMode; }

	//등록된 몽타주를 재생
	void ProcessComboCommand();

	void ComboActionBegin();

	//이 함수는 
	//DECLARE_DELEGATE_TwoParams( FOnMontageEnded, class UAnimMontage*, bool /*bInterrupted*/) 
	//이 델리게이트에 맞춘 함수이다.
	void ComboActionEnd(class UAnimMontage* TargetMontage, bool IsproperlyEnded);
	void SetComboCheckTimer();	//입력 제한시간 지정
	void ComboCheck();			//콤보 확인

private:
	void DefaultPawnSetting();
	void DefaultCapsuleSetting();
	void DefaultMovementSetting();
	void DefaultMeshSetting();
	void DefaultCharacterControlDataSetting();

private:
	UPROPERTY(EditDefaultsOnly, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	ECameraViewMode CurCamViewMode;

	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UABCharacterControlData>> CameraModeSettings;

	//콤보 몽타주 정보, 블루프린트에서 지정할 예정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	//콤보 액션 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABComboActionData> ComboActionData;

	//internal montage process values
	int32 CurrentCombo = 0;	//현재 콤보 단계 수(0 == 콤보 중 아님)
	FTimerHandle ComboTimerHandle;	//시간이 지나면 함수를 호출해주는 핸들
	bool HasNextComboCommand = false;	//제한시간 안에(선입력 타임) 다음 콤보 입력이 들어왔는지
};
