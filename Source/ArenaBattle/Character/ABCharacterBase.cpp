// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/ABCharacterBase.h"

#include "Character/ABCharacterControlData.h"
#include "Character/ABComboActionData.h"
#include "CharacterStat/ABCharacterStatComponent.h"

#include "Physics/ABCollision.h"
#include "UI/ABWidgetComponent.h"
#include "UI/ABHpBarWidget.h"

#include "Item/ABItems.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"

DEFINE_LOG_CATEGORY(LogABCharacter);

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	{//Pawn Setting
		//이 값을 true로 바꾸면 즉시 입력 컨트롤 값에 폰이 동기화 된다
		//쉽게말해 카메라를 돌리면 폰도 바로 회전한다.
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	}

	{//Capsule Component Setting
		UCapsuleComponent* CapsuleCom = GetCapsuleComponent();
		CapsuleCom->InitCapsuleSize(42.f, 96.0f);

		//충돌 프로필을 프로젝트 내부 지정
		CapsuleCom->SetCollisionProfileName(CPROFILE_ABCAPSULE);
	}


	{//Character Movement Setting
		UCharacterMovementComponent* MoveCom = GetCharacterMovement();
		MoveCom->bOrientRotationToMovement = true;
		MoveCom->RotationRate = FRotator(0.0f, 500.f, 0.0f);
		MoveCom->JumpZVelocity = 700.f;
		MoveCom->AirControl = 0.35f;
		MoveCom->MaxWalkSpeed = 500.f;
		MoveCom->MinAnalogWalkSpeed = 20.f;
		MoveCom->BrakingDecelerationWalking = 2000.f;
	}


	{
		static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboActionMontageRef
		(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack'"));
		check(ComboActionMontageRef.Succeeded());
		ComboActionMontage = ComboActionMontageRef.Object;

		static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef
		(TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_ComboAttack.ABA_ComboAttack'"));
		check(ComboActionDataRef.Succeeded());
		ComboActionData = ComboActionDataRef.Object;


		static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef
		(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'"));
		check(DeadMontageRef.Succeeded());
		DeadMontage = DeadMontageRef.Object;
	}

	{
		// Mesh
		USkeletalMeshComponent* SkltMesh = GetMesh();
		SkltMesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
		SkltMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		//메쉬는 충돌판정 제거
		SkltMesh->SetCollisionProfileName(TEXT("NoCollision"));

		//InfinityBladeWarriors의 모델링으로 바꿔치기
		{
			static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
			if (CharacterMeshRef.Object)
			{
				SkltMesh->SetSkeletalMesh(CharacterMeshRef.Object);
			}
		}

		{
			//직접 생성한 Animation Blueprint로부터 AnimInstance 정보를 가져온다.
			static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_AnimCharacter.ABP_AnimCharacter_C"));
			if (AnimInstanceClassRef.Class)
			{
				SkltMesh->SetAnimInstanceClass(AnimInstanceClassRef.Class);
			}
		}
	}
	
	{//Stat Component
		Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));
	}

	//Widget Component
	{
		//UI Component는 UI를 담는 그릇임
		HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));

		//Transform 지정
		HpBar->SetupAttachment(GetMesh());
		HpBar->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

		/*
		* 알멩이는 아까 만든 HP Bar 블루프린트인데,
		* WidgetComponent는 오브젝트가 월드에 생성될 때(인스턴스화) 될 때 등록된 UI 클래스 정보를 통해 인스턴스화를 진행하는 방식이다.
		* ConstructorHelper를 통해서 작업을 진행해 주자
		*/
		{
			static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef
			(TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C"));
			ensure(HpBarWidgetRef.Succeeded());
			HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		}

		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.f, 15.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	DeadDestroyDelayTime = 5.f;

	//Item Actions
	//각각의 Enum값에 대응되는 델리게이트 함수를 연결한다.
	TakeItemActions.SetNum((int32)EItemType::END);
	TakeItemActions[(int32)EItemType::Weapon].ItemDelegate = 
		FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipWeapon);
	TakeItemActions[(int32)EItemType::Potion].ItemDelegate =
		FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::DrinkPotion);
	TakeItemActions[(int32)EItemType::Scroll].ItemDelegate =
		FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::ReadScroll);

	//Weapon Component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));

	//인피니티 메시에 지정된 소켓 이름에 붙여 준다.
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	//Evade 
	EvadeDir = FVector::BackwardVector;
	EvadeCoolTime = 1.0f;
	EvadeCastingTime = 0.2f;
}

bool AABCharacterBase::TryReserveAction(ECharacterAction InAction)
{
	if (ReserveActionTimer.IsValid())
	{
		ReservedAction = InAction;
		return true;
	}
	return false;
}


void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnCurHpZero.AddUObject(this, &AABCharacterBase::SetDead);
	Stat->OnStatChanged.AddUObject(this, &AABCharacterBase::ApplyStat);

	//추가 스탯 초기화와 동시에 등록한 OnStatChanged 델리게이트 함수들 호출하기 위함
	Stat->SetModifierStat({});
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* ControlData)
{
	bUseControllerRotationYaw = ControlData->bUseControllerRotationYaw;

	GetCharacterMovement()->bOrientRotationToMovement = ControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = ControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = ControlData->RotationRate;
}

void AABCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//회피 방향은 반대편
	EvadeDir = GetMovementComponent()->GetForwardVector();
	EvadeDir.Z = 0.0f;
	EvadeDir = -EvadeDir;
}

void AABCharacterBase::ProcessComboCommand()
{
	//콤보 진행을 할 수 없는 상활일 때는 진행하지 않음
	if (CurrentState < 0) { return; }
	//Idle 상태일 때에서만 콤보 공격을 진행할 수 있음.
	if (CurrentState == 0)
	{
		ComboActionBegin();
		return;
	}

	TryReserveAction(ECharacterAction::Attack);
}

void AABCharacterBase::ComboActionBegin()
{
	CurrentState = 1;

	//공격 중에는 움직임을 제한한다.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	//Montage는 AnimInst에서 재생이 가능하다.
	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	AnimInst->Montage_Play(ComboActionMontage, AttackSpeedRate);

	//끝났을 때 호출할 델리게이트 등록
	FOnMontageEnded EndDelegate{};
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInst->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);
	
	//타이머 재시작
	ReserveActionTimer.Invalidate();
	SetReserveActionTimer_ComboAttack();
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsproperlyEnded)
{
	if (CurrentState < 1) { return; }
	CurrentState = IdleState;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	NotifyComboActionEnd();
}

void AABCharacterBase::SetReserveActionTimer_ComboAttack()
{
	int32 CurrentComboIdx = CurrentState - 1;
	ensure(ComboActionData && ComboActionData->EffectiveFrameCount.IsValidIndex(CurrentComboIdx));

	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;

	//제한 프레임 장수 / 초당 프레임 시간 / 공속
	float ComboEffectiveTime = ComboActionData->EffectiveFrameCount[CurrentComboIdx] / ComboActionData->FrameRate / AttackSpeedRate;

	if (0.f < ComboEffectiveTime)
	{
		//타이머 핸들을 활용하여 ComboEffectiveTime 시간 후에 호출되는 함수를 등록
		GetWorld()->GetTimerManager().SetTimer(ReserveActionTimer, this, &AABCharacterBase::CheckReservedAction, ComboEffectiveTime, false);
	}
}

void AABCharacterBase::CheckReservedAction()
{
	//선입력 종료 타이밍 델리게이트 호출
	NotifyReserveTimeEnd();

	//타이머 OFF
	ReserveActionTimer.Invalidate();

	if (ReservedAction == ECharacterAction::None) { return; }

	//시간 안에 다음 콤보 커맨드가 들어왔으면 다음 콤보 몽타주로 넘어간다.
	if (ReservedAction == ECharacterAction::Attack)
	{
		ReservedAction = ECharacterAction::None;

		UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();

		//최대 콤보 범위 안으로 제한을 걸어준다.
		CurrentState = FMath::Clamp(CurrentState + 1, 1, ComboActionData->MaxComboCount);

		//몽타주에 저장된 Section 이름은 FName 형태이다.
		FName NextSectionName = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentState);
		AnimInst->Montage_JumpToSection(NextSectionName, ComboActionMontage);

		//다음 콤보까지 제한시간을 새로 등록한다.
		SetReserveActionTimer_ComboAttack();
	}
	else if (ReservedAction == ECharacterAction::Evade)
	{
		EvadeNow();
	}

	ReservedAction = ECharacterAction::None;
}

//애니메이션 노티파이 클래스 AnimNotify_AttackHitCheck 클래스에서 호출됨
void AABCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	//인자1: 언리얼 엔진 충돌 분석 툴에 사용되는 '태그' 
	//인자2: 복잡한 형태의 충돌체에 충돌 처리를 할 것인지 말 것인지
	//		이 때는 복잡한 형태의 충돌체에 올라서는 등의 행위만 할 수 있다.
	//인자3: 
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	//공격 정보
	const float AttackRange = Stat->GetTotalStat().AttackRange;
	const float AttackRadius = Stat->GetAttackRadius();
	const float AttackDamage = Stat->GetTotalStat().Attack;

	//Sweep 이므로 시작점과 끝점이 필요.
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	//Sweep 충돌 방식으로, 하나만, 채널 기준으로 충돌 확인.
	//충돌 시 OutHitResult에 결과 값이 들어온다.
	//Start, End: 충돌 시작, 끝 지점
	//Rot: 회전
	//TraceChannel: 아까 만들었던 그 트레이스 채널과 연동된 Enum 값
	//투사시킬 충돌체: 50짜리로 새로 생성
	//Params: 위쪽에 설명함
	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere(AttackRadius), Params);

	//충돌했는지 확인
	if (HitDetected)
	{
		//공격이 무언가와 충돌했을 시 데미지를 상대방에게 전달한다.
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

#if ENABLE_DRAW_DEBUG
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(
		GetWorld(), 
		CapsuleOrigin, 
		CapsuleHalfHeight, 
		AttackRadius, 
		//캡슐을 눕혀줘야 하므로(Z축의 회전 상태만 가져온다.)
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		DrawColor, 
		false, //계속해서 그릴 것인지
		3.0f);	//지속 시간
#endif
}

//
float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//Stat에 받은 데미지를 적용
	return Stat->ApplyDamage(DamageAmount);
}

void AABCharacterBase::TryEvade()
{
	if (CurrentState == EvadeState) { return; }
	if (GetWorld()->GetTimerManager().IsTimerActive(EvadeCoolTimer)) { return; }

	if (CurrentState == IdleState)
	{
		EvadeNow();
	}

	if (IdleState < CurrentState)
	{
		//선입력 가능한 시간일 경우 선입력
		if (ReserveActionTimer.IsValid())
		{
			ReservedAction = ECharacterAction::Evade;
		}
		//선입력 시간이 아닐 경우, 회피 가능한 시간이므로 회피
		else
		{
			EvadeNow();
		}
	}
}

void AABCharacterBase::EvadeNow()
{
	if (IdleState < CurrentState)
	{
		//몽타주 재생 시 기본 액션을 덮어쓰므로 몽타주 재생을 중단
		UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
		AnimInst->Montage_Stop(0.0f, ComboActionMontage);

		//콤보액션 종료 함수를 명시적으로 호출
		ComboActionEnd(ComboActionMontage, false);
	}

	CurrentState = EvadeState;
	GetWorld()->GetTimerManager().SetTimer(EvadeCoolTimer, EvadeCoolTime, false);
	GetWorld()->GetTimerManager().SetTimer(EvadeCastTimer,
		[this]()
		{
			CurrentState = IdleState;
		}
	, EvadeCastingTime, false);

	GetMovementComponent()->StopMovementImmediately();

	//XY 좌표 오버라이드
	LaunchCharacter(EvadeDir * 1000.0f, true, false);
}

void AABCharacterBase::SetDead()
{
	//일단 이동 제한을 걸어준다.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	PlayDeadAnimation();

	//모든 충돌 판정 OFF
	SetActorEnableCollision(false);

	//HP바는 숨겨준다
	HpBar->SetHiddenInGame(true);
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	//기존의 애니메이션 정지
	AnimInstance->StopAllMontages(0.0f);

	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

int32 AABCharacterBase::GetLevel() const
{
	return Stat->GetCurrentLevel();
}

void AABCharacterBase::SetLevel(int32 InNewLevel)
{
	Stat->SetLevelStat(InNewLevel);
}

void AABCharacterBase::ApplyStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	FABCharacterStat TotalStat = BaseStat + ModifierStat;
	GetCharacterMovement()->MaxWalkSpeed = TotalStat.MovementSpeed;
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	//HP바를 설정해줄 것므로 Cast를 통해서 Hp Bar로 변경 후 값 설정
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->SetMaxHp(Stat->GetBaseStat(), Stat->GetModifierStat());
		HpBarWidget->SetCurHp(Stat->GetCurrentHp());
		Stat->OnCurHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::SetCurHp);
		Stat->OnStatChanged.AddUObject(HpBarWidget, &UABHpBarWidget::SetMaxHp);
	}
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	if (InItemData)
	{
		TakeItemActions[(uint8)InItemData->GetItemType()].ItemDelegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	UABPotionItemData* PotionItemData = Cast<UABPotionItemData>(InItemData);
	if (PotionItemData)
	{
		Stat->HealHp(PotionItemData->GetHealAmount());
	}
}

void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
	UABWeaponItemData* WeaponItemData = Cast<UABWeaponItemData>(InItemData);
	if (WeaponItemData)
	{
		//아직 로드되지 않았다면
		if (WeaponItemData->WeaponMesh.IsPending()) 
		{
			//로드한다.
			WeaponItemData->WeaponMesh.LoadSynchronous();
		}
		Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh.Get());
		Stat->SetModifierStat(WeaponItemData->ModifierStat);
	}
}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
	UABScrollItemData* ScrollItemData = Cast<UABScrollItemData>(InItemData);
	if (ScrollItemData)
	{
		FABCharacterStat NewStat = Stat->GetBaseStat() + ScrollItemData->GetAddStats();
		Stat->SetBaseStat(NewStat);
	}
}
