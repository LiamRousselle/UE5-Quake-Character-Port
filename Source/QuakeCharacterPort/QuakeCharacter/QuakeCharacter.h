#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "QuakeCharacter.generated.h"

class UQuakeCharacterMovementComponent;
class UArrowComponent;
class UCapsuleComponent;

//Quake unit to Unreal Unit conversion -> (QU * 2.54 = UU)
#define QUAKE_CHARACTER_BODY_HEIGHT			142.24f //(56 quake units)
#define	QUAKE_CHARACTER_RADIUS				40.64f //(16 quake units)

UCLASS()
class QUAKECHARACTERPORT_API AQuakeCharacter : public APawn
{
	GENERATED_BODY()

public:
	AQuakeCharacter();

	/** Getter for "QuakeCharacterMovementComp" */
	UFUNCTION(BlueprintPure)
	UQuakeCharacterMovementComponent* GetQuakeCharacterMovementComp() const { return QuakeCharacterMovementComp; }

	/**
	 * Responsible for informing the QuakeCharacterMovementComp that we want to move.
	 * @note MoveDirection passed should be the direction relative to the world, not the Character
	 * @note bForce parameter should be disregarded, because I don't want to implement it
	 */
	virtual void AddMovementInput(
		FVector WorldDirection,
		float ScaleValue = 1.0f,
		bool bForce = false
	) override;

	/**
	 * Responsible for making the quake character controller jump by informing the QuakeCharacterMovementComp
	 * to jump.
	 * @note Called this multiple times will not fling the player up into the sky, ground check is done here
	 */
	UFUNCTION(BlueprintCallable)
	virtual void Jump();
	
protected:
	/** The body capsule collider of the quake character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	UCapsuleComponent* BodyCollider;

	/**
	 * Movement isn't controlled directly from this Quake Character, instead
	 * it is controlled via a QuakeCharacterMovementComponent (similar to Unreal's UCharacterMovementComponent)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	UQuakeCharacterMovementComponent* QuakeCharacterMovementComp;
	
private:
	/** The arrow sticking out of the body capsule collider that is displayed in the editor */
	UPROPERTY()
	TObjectPtr<UArrowComponent> EditorForwardArrow;
};