// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/ABCharacterPlayer.h"

#include "Character/ABCharacterControlData.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ShoulderCameraBoom"));
	//루트컴포넌트에 붙인다
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Cam"));
	//카메라는 셀카봉 끝에 붙인다(소켓 이름을 지정 시 해당 소켓 위치에 가서 붙음)
	Cam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Cam->bUsePawnControlRotation = false;

	//Input: 레퍼런스로부터 가져온다.
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderMoveActionRef
		(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
		check(ShoulderMoveActionRef.Succeeded());
		ShoulderMoveAction = ShoulderMoveActionRef.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction> SharedJumpActionRef
		(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump'"));
		check(SharedJumpActionRef.Succeeded());
		SharedJumpAction = SharedJumpActionRef.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderLookActionRef
		(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
		check(ShoulderLookActionRef.Succeeded());
		ShoulderLookAction = ShoulderLookActionRef.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction> QuarterMoveActionRef
		(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_QuarterMove.IA_QuarterMove'"));
		check(QuarterMoveActionRef.Succeeded());
		QuarterMoveAction = QuarterMoveActionRef.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction> SwitchViewActionRef
		(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_SwitchMode.IA_SwitchMode'"));
		check(SwitchViewActionRef.Succeeded());
		SwitchViewAction = SwitchViewActionRef.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction> AttackActionRef
		(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Attack.IA_Attack'"));
		check(AttackActionRef.Succeeded());
		AttackAction = AttackActionRef.Object;
	}
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//들어온 PlayerInputComponent이 EnhancedInputComponent인지 확인
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	//점프는 부모 클래스에 정의된 함수를 사용
	EnhancedInputComponent->BindAction(SharedJumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(SharedJumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	//이동과 Look는 이 클래스에서 새로 정의한 함수를 사용
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);

	EnhancedInputComponent->BindAction(QuarterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuarterMove);

	EnhancedInputComponent->BindAction(SwitchViewAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::SwitchCameraViewMode);

	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Attack);
}

void AABCharacterPlayer::SwitchCameraViewMode()
{
	ECameraViewMode ViewMode = GetCameraViewMode();
	if (ViewMode == ECameraViewMode::Shoulder)
	{
		ViewMode = ECameraViewMode::Quarter;
	}
	else
	{
		ViewMode = ECameraViewMode::Shoulder;
	}
	SetCameraViewMode(ViewMode);
}



void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* ControlData)
{
	Super::SetCharacterControlData(ControlData);

	CameraBoom->SetRelativeRotation(ControlData->RelativeRotation);
	CameraBoom->TargetArmLength = ControlData->TargetArmLength;
	CameraBoom->bUsePawnControlRotation = ControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = ControlData->bInheritPitch;
	CameraBoom->bInheritYaw = ControlData->bInheritYaw;
	CameraBoom->bInheritRoll = ControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = ControlData->bDoCollisionTest;
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	//Yaw: 주위를 둘러보는 회전
	const FRotator YawRotation = { 0, Rotation.Yaw, 0 };

	//좌우값만 남은 회전행렬에서 X축 방향을 가져와서 해당 방향으로 이동
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.X);

	//
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AABCharacterPlayer::QuarterMove(const FInputActionValue& Value)
{
	//월드 방향을 그대로 반영
	FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(FVector::ForwardVector, MovementVector.X);
	AddMovementInput(FVector::RightVector, MovementVector.Y);
}

void AABCharacterPlayer::Attack()
{
	ProcessComboCommand();
}
