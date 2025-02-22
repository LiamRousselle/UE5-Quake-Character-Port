﻿#include "QuakeCharacter.h"
#include "QuakeCharacterMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

AQuakeCharacter::AQuakeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_StartPhysics;

	// Create body collider
	BodyCollider = CreateDefaultSubobject<UCapsuleComponent>(FName("BodyCollider"));
	SetRootComponent(BodyCollider);
	BodyCollider->SetCapsuleHalfHeight(QUAKE_CHARACTER_BODY_HEIGHT);
	BodyCollider->SetCapsuleRadius(QUAKE_CHARACTER_RADIUS);

	// Create Quake Movement Component
	QuakeCharacterMovementComp = CreateDefaultSubobject<UQuakeCharacterMovementComponent>(FName("QuakeMovementComp"));
	
	// Create Editor Forward Arrow
#if WITH_EDITORONLY_DATA
	EditorForwardArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(FName("EditorForwardArrow"));
	if (EditorForwardArrow.Get())
	{
		EditorForwardArrow->SetupAttachment(BodyCollider);
		EditorForwardArrow->SetArrowColor(FColor::Cyan);
		EditorForwardArrow->SetIsScreenSizeScaled(true);
	}
#endif // WITH_EDITORONLY_DATA
}

void AQuakeCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	if (IsValid(QuakeCharacterMovementComp))
		QuakeCharacterMovementComp->UpdateWishMoveDirection(WorldDirection * ScaleValue);
}

void AQuakeCharacter::Jump()
{
	if (IsValid(QuakeCharacterMovementComp))
		QuakeCharacterMovementComp->InputJump();
}
