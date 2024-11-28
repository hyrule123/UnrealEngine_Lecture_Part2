// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Interface/ABAnimationAttackInterface.h"
#include "Interface/ABCharacterWidgetInterface.h"
#include "Interface/ABCharacterItemInterface.h"

#include "ABCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABCharacter, Log, All);

UENUM()
enum class ECameraViewMode : uint32
{
	Shoulder,
	Quarter,
	END
};

//아이템 습득시 처리할 Delegate
DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UABItemData* /*InItemData*/);

//Delegate 타입은 배열로 선언할 수가 없어서 Wrapper 구조체가 필요하다.
USTRUCT(BlueprintType)
struct FOnTakeItemDelegateWrapper
{
	GENERATED_BODY()

	FOnTakeItemDelegateWrapper() {}

	FOnTakeItemDelegate ItemDelegate;
};

class UABCharacterControlData;

UCLASS()
class ARENABATTLE_API AABCharacterBase 
	: public ACharacter
	, public IABAnimationAttackInterface
	, public IABCharacterWidgetInterface
	, public IABCharacterItemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetCharacterControlData(const UABCharacterControlData* ControlData);


protected://Camera Section
	void SetCameraViewMode(ECameraViewMode Mode);
	ECameraViewMode GetCameraViewMode() { return CurCamViewMode; }

	UPROPERTY(EditDefaultsOnly, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	ECameraViewMode CurCamViewMode;

	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UABCharacterControlData>> CameraModeSettings;


protected://Combo Section
	//등록된 Attack 몽타주를 재생
	void ProcessComboCommand();
	void ComboActionBegin();

	//이 함수는 
	//DECLARE_DELEGATE_TwoParams( FOnMontageEnded, class UAnimMontage*, bool /*bInterrupted*/) 
	//이 델리게이트에 맞춘 함수이다.
	void ComboActionEnd(class UAnimMontage* TargetMontage, bool IsproperlyEnded);
	void SetComboCheckTimer();	//입력 제한시간 지정
	void ComboCheck();			//콤보 확인

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

	//IABAnimationAttackInterface
	virtual void AttackHitCheck() override;
	
	//AActor 클래스
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


protected://Dead Section
	virtual void SetDead();
	void PlayDeadAnimation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	float DeadDestroyDelayTime;

	//Stat Section
public:
	int32 GetLevel() const;
	void SetLevel(int32 InNewLevel);

protected:
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABCharacterStatComponent> Stat;

protected://UI Widget Section
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABWidgetComponent> HpBar;

	virtual void SetupCharacterWidget(class UABUserWidget* InUserWidget) override;

protected://Item Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	UPROPERTY()
	TArray<FOnTakeItemDelegateWrapper> TakeItemActions;

	virtual void TakeItem(class UABItemData* InItemData) override;
	virtual void DrinkPotion(class UABItemData* InItemData);
	virtual void EquipWeapon(class UABItemData* InItemData);
	virtual void ReadScroll(class UABItemData* InItemData);
	
};
