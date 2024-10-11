// Fill out your copyright notice in the Description page of Project Settings.
#include "Animation/ABAnimInstance.h"

#include "Character/ABCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"

UABAnimInstance::UABAnimInstance()
{
	MoveThreshold = 3.0f;
	JumpThreshold = 100.f;
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

	if (MovementCom)
	{
		Velocity = MovementCom->Velocity;
		GroundSpeed = Velocity.Size2D();	//X, Y축에 대해서만 크기를 구한다.
		bIsIdle = GroundSpeed < MoveThreshold;
		bIsFalling = MovementCom->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpThreshold);
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Velocity: %f, %f, %f"), Velocity.X, Velocity.Y, Velocity.Z));
	}
}
