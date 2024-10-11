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
};
