// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "ABCharacter_NonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABCharacter_NonPlayer : public AABCharacterBase
{
	GENERATED_BODY()
	
protected:
	virtual void SetDead() override;
};
