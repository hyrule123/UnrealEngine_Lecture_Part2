// Fill out your copyright notice in the Description page of Project Settings.
#include "Animation/ABAnimInstance.h"

#include "Animation/AnimMontage.h"
#include "Character/ABCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UABAnimInstance::UABAnimInstance()
{
	MoveThreshold = 3.0f;
	JumpThreshold = 100.f;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> EvadeMontageRef
	(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Evade.AM_Evade'"));
	ensure(EvadeMontageRef.Succeeded());
	EvadeMontage = EvadeMontageRef.Object;
}

void UABAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningAABCharacter = Cast<AABCharacterBase>(GetOwningActor());
	if (OwningAABCharacter)
	{
		MovementCom = OwningAABCharacter->GetCharacterMovement();
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwningAABCharacter)
	{
		bIsEvading = OwningAABCharacter->IsEvading();
	}

	if (MovementCom)
	{
		Velocity = MovementCom->Velocity;
		GroundSpeed = Velocity.Size2D();	//X, Y축에 대해서만 크기를 구한다.
		bIsIdle = GroundSpeed < MoveThreshold && !bIsEvading;
		bIsFalling = MovementCom->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpThreshold);
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Velocity: %f, %f, %f"), Velocity.X, Velocity.Y, Velocity.Z)); 
	}

	if (bIsEvading)
	{
		//PlayAnimMontage(EvadeMontage, 1.0f, TEXT("EvadeMotionStart"));
		//Montage_loop(EvadeMontage);
	}
}
