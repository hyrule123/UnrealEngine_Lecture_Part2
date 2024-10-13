// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter_NonPlayer.h"

void AABCharacter_NonPlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimer{};
	GetWorld()->GetTimerManager().SetTimer(
		DeadTimer,
		FTimerDelegate::CreateLambda(	//람다 함수를 통해 빠르게 함수를 생성할 수 있다.
			[&]()
			{
				this->Destroy();
			}),
		DeadDestroyDelayTime,
		false
	);
}
