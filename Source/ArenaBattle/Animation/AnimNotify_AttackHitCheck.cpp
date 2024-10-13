// Fill out your copyright notice in the Description page of Project Settings.
#include "Animation/AnimNotify_AttackHitCheck.h"

#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	//메쉬 컴포넌트로부터 오너를 가져온다.
	if (MeshComp)
	{
		//인터페이스 클래스로 캐스팅
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());

		//가상함수 호출
		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
