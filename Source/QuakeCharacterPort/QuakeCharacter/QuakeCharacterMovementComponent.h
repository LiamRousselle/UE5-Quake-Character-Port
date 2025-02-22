#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuakeCharacterMovementComponent.generated.h"

#define DEFAULT_QUAKE_MAX_WALK_SPEED			32.0f //320.0f in Quake units
#define DEFAULT_QUAKE_MAX_JUMP_POWER			25.0f //250.0f in Quake units
#define DEFAULT_QUAKE_ACCELERATION_SPEED		10.0f
#define DEFAULT_QUAKE_STOP_SPEED				10.0f //100.0f in Quake units
#define DEFAULT_QUAKE_FRICTION					4.0f

class UCapsuleComponent;

UENUM()
enum FQuakeCharacterMovementCompDebugLevels
{
	QCMCDL_Disabled UMETA(DisplayName = "Disabled"),
	QCMCDL_Basic UMETA(DisplayName = "Basic"),
	QCMCDL_All UMETA(DisplayName = "All")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class QUAKECHARACTERPORT_API UQuakeCharacterMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuakeCharacterMovementComponent();
	
	/**
	 * Setter for "WishMoveDirection"
	 * @note This method scales "Dir" by MaxWalkSpeed (not anymore)
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateWishMoveDirection(const FVector& Dir);

	/** Called to set "bRememberJump" to true */
	UFUNCTION(BlueprintCallable)
	void InputJump() { bRememberJump = true; OwnerPawn->AddActorWorldOffset(FVector::ZAxisVector * 100.0f); }

	/**
	 * Used to see if the Owner Pawn is moving on the Ground
	 * 
	 * @return boolean | Is On Ground
	 */
	UFUNCTION(BlueprintPure)
	bool IsMovingOnGround() const;
	
protected:
	/** The max speed which the pawn can travel at */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float MaxWalkSpeed = DEFAULT_QUAKE_MAX_WALK_SPEED;

	/** The acceleration speed which the pawn will accelerate at (Accel to MaxWalkSpeed in 0.1s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float AccelerationSpeed = DEFAULT_QUAKE_ACCELERATION_SPEED;

	/** The rate which the pawn will de-accelerate at */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float StopSpeed = DEFAULT_QUAKE_STOP_SPEED;

	/** Friction applied on ground movements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPreserveRatio = true))
	float Friction = DEFAULT_QUAKE_FRICTION;

	/** The developer log level for the quake character movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debugging")
	TEnumAsByte<FQuakeCharacterMovementCompDebugLevels> DebugLevel;
	
	/**
	 * This field should ONLY BE SET USING "SetWishMoveDirection"
	 * @note This direction is relative to the world, not the owning pawn
	 */
	FVector WishMoveDirection = FVector::ZeroVector;

	/** The current velocity which the pawn is moving at */
	FVector Velocity = FVector::ZeroVector;
	
protected:
	/** Called every frame, tick group is set to TG_StartPhysics */
	virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* Func) override;

	/** Called when the component is spawned within it's owner pawn */
	virtual void BeginPlay(void) override;
	
	/**
	 * 
	 * @note Called every tick.
	 * @note For more information, check out "https://github.com/id-Software/Quake/blob/master/WinQuake/sv_user.c#L170"
	 */
	void GroundAccelerate(void);

	/**
	 * Prevents awkward acceleration movement when releasing inputs.
	 * @note Called every tick.
	 * @note For more information, check out "https://github.com/id-Software/Quake/blob/master/WinQuake/sv_user.c#L122"
	 */
	void UserFriction(void);
	
	/**
	 *
	 * @note Called every tick.
	 * @note For more information, check out "https://github.com/id-Software/Quake/blob/master/WinQuake/sv_user.c#L207"
	 */
	void AirAccelerate(void);

	/**
	 * Called every tick, used to move the pawn in the direction of "Velocity"
	 * @note Collision with objects like walls are detected in here
	 */
	void MovePawn(void) const;

	/**
	 * Called every tick, used to display important debug information to the screen.
	 * @note 
	 * @note This should only be called in the
	 */
	void DumpDebugLogs() const;
	
private:
	bool bOnGround = true;
	bool bRememberJump = false;

	/**
	 * Direct reference to the owning pawn of this actor.
	 * @note Never reference the "QuakeCharacter" class, because sometimes people will want to implement there own usage of this component.
	 */
	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn = nullptr;

	/**
	 * Direct reference to the CapsuleComponent in the pawn.
	 * @note If this component is not attached to a Quake Character pawn, and there is no CapsuleComponent in the owner,
	 * unexpected behaviour will occur.
	 */
	UPROPERTY()
	TObjectPtr<UCapsuleComponent> OwnerPawnBodyCollider = nullptr;
};
