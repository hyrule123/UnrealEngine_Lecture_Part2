// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/ABCharacterBase.h"

#include "Character/ABCharacterControlData.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnSetting();
	DefaultCapsuleSetting();
	DefaultMovementSetting();
	DefaultMeshSetting();
	DefaultCharacterControlDataSetting();
}

void AABCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SetCameraViewMode(CurCamViewMode);
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* ControlData)
{
	//input 담당(Controller 액터)를 가져와서 만든 키매핑을 등록한다.
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	check(ControlData->InputMappingContext);

	//Input Mapping을 실제로 운용하는 Subsystem을 가져온다.
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>
		(PlayerController->GetLocalPlayer()))
	{
		//0: 우선순위 -> 입력 충돌이 있을경우 우선순위 가 높은 입력을 우선 처리
		SubSystem->AddMappingContext(ControlData->InputMappingContext, 0);
	}

	bUseControllerRotationYaw = ControlData->bUseControllerRotationYaw;

	GetCharacterMovement()->bOrientRotationToMovement = ControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = ControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = ControlData->RotationRate;
}

void AABCharacterBase::SetCameraViewMode(ECameraViewMode Mode)
{
	check(CameraModeSettings.IsValidIndex((int32)Mode) && CameraModeSettings[(int32)Mode]);
	CurCamViewMode = Mode;
	SetCharacterControlData(CameraModeSettings[(int32)Mode]);
}

void AABCharacterBase::ProcessComboCommand()
{
	//콤보 값이 0일때, 즉 콤보공격이 아직 시작되지 않았을 때
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
	}
}

void AABCharacterBase::ComboActionBegin()
{
	CurrentCombo = 1;

	//공격 중에는 움직임을 제한한다.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	//공속: 일단은 상수값으로 지정
	constexpr float AttackSpeedRate = 1.0f;

	//Montage는 AnimInst에서 재생이 가능하다.
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	AnimInst->Montage_Play(ComboActionMontage);

	//끝났을 때 호출할 델리게이트 등록
	FOnMontageEnded EndDelegate{};
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInst->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsproperlyEnded)
{
	ensure(CurrentCombo != 0);
	CurrentCombo = 0;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::DefaultPawnSetting()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AABCharacterBase::DefaultCapsuleSetting()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
}

void AABCharacterBase::DefaultMovementSetting()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 100.f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

void AABCharacterBase::DefaultMeshSetting()
{
	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	//InfinityBladeWarriors의 모델링으로 바꿔치기
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	//직접 생성한 Animation Blueprint로부터 AnimInstance 정보를 가져온다.
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_AnimCharacter.ABP_AnimCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
}

void AABCharacterBase::DefaultCharacterControlDataSetting()
{
	CurCamViewMode = ECameraViewMode::Shoulder;

	CameraModeSettings.SetNum((int32)ECameraViewMode::END);

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderViewSettingRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	check(ShoulderViewSettingRef.Succeeded());
	CameraModeSettings[(int32)ECameraViewMode::Shoulder] = ShoulderViewSettingRef.Object;

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuarterViewSettingRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quarter.ABC_Quarter'"));
	check(QuarterViewSettingRef.Succeeded());
	CameraModeSettings[(int32)ECameraViewMode::Quarter] = QuarterViewSettingRef.Object;
}
