// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/ABCharacterPlayer.h"

#include "Character/ABCharacterControlData.h"
#include "UI/ABHUDWidget.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "Interface/ABGameInterface.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/GameModeBase.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	{//Camera
		CurCamViewMode = ECameraViewMode::Quarter;

		CameraModeSettings.SetNum((int32)ECameraViewMode::END);

		{
			static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderViewSettingRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
			check(ShoulderViewSettingRef.Succeeded());
			CameraModeSettings[(int32)ECameraViewMode::Shoulder] = ShoulderViewSettingRef.Object;
		}

		{
			static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuarterViewSettingRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quarter.ABC_Quarter'"));
			check(QuarterViewSettingRef.Succeeded());
			CameraModeSettings[(int32)ECameraViewMode::Quarter] = QuarterViewSettingRef.Object;
		}
	}

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ShoulderCameraBoom"));
	//루트컴포넌트에 붙인다
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Cam"));
	//카메라는 셀카봉 끝에 붙인다(소켓 이름을 지정 시 해당 소켓 위치에 가서 붙음)
	Cam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Cam->bUsePawnControlRotation = false;

	
	{//Input: 레퍼런스로부터 가져온다.
		static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderMoveActionRef
		(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
		check(ShoulderMoveActionRef.Succeeded());
		ShoulderMoveAction = ShoulderMoveActionRef.Object;
	}

	{//Jump Action
		static ConstructorHelpers::FObjectFinder<UInputAction> SharedJumpActionRef
		(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump'"));
		check(SharedJumpActionRef.Succeeded());
		SharedJumpAction = SharedJumpActionRef.Object;
	}

	{//Should Look Action
		static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderLookActionRef
		(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
		check(ShoulderLookActionRef.Succeeded());
		ShoulderLookAction = ShoulderLookActionRef.Object;
	}

	{//Quarter Move Action
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

	{
		static ConstructorHelpers::FObjectFinder<UInputAction> EvadeActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Evade.IA_Evade'"));
		check(EvadeActionRef.Succeeded());
		EvadeAction = EvadeActionRef.Object;
	}
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}

	SetCameraViewMode(CurCamViewMode);
}

void AABCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bIsMoveActionPressed)
	{
		EvadeDir = LastInputDirection;
		bIsMoveActionPressed = false;

		//방향 초기화
		LastInputDirection = {};
	}
}

void AABCharacterPlayer::SetDead()
{
	Super::SetDead();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);

		IABGameInterface* ABGameMode = Cast<IABGameInterface>(GetWorld()->GetAuthGameMode());
		if (ABGameMode)
		{
			ABGameMode->OnPlayerDead();
		}
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

	EnhancedInputComponent->BindAction(EvadeAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::HandleEvadeInputAction);
}

void AABCharacterPlayer::SwitchCameraViewMode()
{
	if (CurCamViewMode == ECameraViewMode::Shoulder)
	{
		SetCameraViewMode(ECameraViewMode::Quarter);
	}
	else
	{
		SetCameraViewMode(ECameraViewMode::Shoulder);
	}
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

	//input 담당(Controller 액터)를 가져와서 만든 키매핑을 등록한다.
	//플레이어 캐릭터만 키매핑에 반응해야 하므로 이 작업은 여기서 해줘야 함.
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	check(ControlData->InputMappingContext);

	//Input Mapping을 실제로 운용하는 Subsystem을 가져온다.
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>
		(PlayerController->GetLocalPlayer()))
	{
		//기존의 키매핑 제거
		SubSystem->ClearAllMappings();

		//0: 우선순위 -> 입력 충돌이 있을경우 우선순위 가 높은 입력을 우선 처리
		SubSystem->AddMappingContext(ControlData->InputMappingContext, 0);
	}
}

void AABCharacterPlayer::SetCameraViewMode(ECameraViewMode Mode)
{
	check(CameraModeSettings.IsValidIndex((int32)Mode));
	check(CameraModeSettings[(int32)Mode]);

	CurCamViewMode = Mode;
	SetCharacterControlData(CameraModeSettings[(int32)Mode]);
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Log, TEXT("%s"), *(Value.ToString()));

	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	//Yaw: 주위를 둘러보는 회전
	const FRotator YawRotation = { 0, Rotation.Yaw, 0 };

	//좌우값만 남은 회전행렬에서 X축 방향을 가져와서 해당 방향으로 이동
	const FRotationMatrix RotMat = { YawRotation };
	const FVector ForwardDirection = RotMat.GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.X);

	const FVector RightDirection = RotMat.GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.Y);

	bIsMoveActionPressed = true;
	float DT = GetWorld()->GetDeltaSeconds();
	LastInputDirection = 
		ForwardDirection * MovementVector.X * DT + //정면 이동거리 계산
		RightDirection * MovementVector.Y * DT;//측면(우측) 이동거리 계산
	LastInputDirection.Normalize();
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

	bIsMoveActionPressed = true;
	LastInputDirection.X = MovementVector.X;
	LastInputDirection.Y = MovementVector.Y;
	LastInputDirection.Z = 0.0f;
};


void AABCharacterPlayer::Attack()
{
	ProcessComboCommand();
}

void AABCharacterPlayer::HandleEvadeInputAction()
{
	TryEvade();
}

void AABCharacterPlayer::SetupHUDWidget(UABHUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		InHUDWidget->UpdateStatWidget(Stat->GetBaseStat(), Stat->GetModifierStat());
		InHUDWidget->UpdateHpBar(Stat->GetCurrentHp());

		//델리게이트에 UI 업데이트 함수를 바인딩
		Stat->OnStatChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateStatWidget);
		Stat->OnCurHpChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateHpBar);
	}
}
