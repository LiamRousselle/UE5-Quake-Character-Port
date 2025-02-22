#include "QuakeCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

UQuakeCharacterMovementComponent::UQuakeCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_StartPhysics;
}

void UQuakeCharacterMovementComponent::UpdateWishMoveDirection(const FVector& Dir)
{
	WishMoveDirection = Dir /* MaxWalkSpeed*/;
}

bool UQuakeCharacterMovementComponent::IsMovingOnGround() const
{
	if (!IsValid(OwnerPawnBodyCollider.Get()) || !IsValid(OwnerPawn.Get()))
		return false;

	const float BodyRadius = OwnerPawnBodyCollider->GetScaledCapsuleRadius();
	const float BodyHalfHeight = OwnerPawnBodyCollider->GetScaledCapsuleHalfHeight();

	const FVector Start = OwnerPawnBodyCollider->GetComponentLocation();
	const FVector BodyUp = OwnerPawnBodyCollider->GetUpVector();
	const FVector Goal = Start - (BodyUp * BodyHalfHeight - BodyUp * (BodyRadius * 0.5f));

	if (DebugLevel >= QCMCDL_All)
		DrawDebugSphere(GetWorld(), Goal, BodyRadius * 0.5f, 8, FColor::Green);
	
	FHitResult Result;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPawn.Get());

	return GetWorld()->SweepSingleByChannel(
		Result,
		Start,
		Goal,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(BodyRadius * 0.5f),
		QueryParams
	);
}

void UQuakeCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* Func)
{
	Super::TickComponent(DeltaTime, Type, Func);

	bOnGround = IsMovingOnGround();

	/*if (!bOnGround)
		OwnerPawn->AddActorWorldOffset(-FVector::ZAxisVector * DeltaTime * 100.0f);*/
	
	UserFriction();
	if (bOnGround)
		GroundAccelerate();
	else
		AirAccelerate();
	
	MovePawn();

	if (DebugLevel > QCMCDL_Disabled)
	{
		DumpDebugLogs();
	}
}

void UQuakeCharacterMovementComponent::BeginPlay(void)
{
	Super::BeginPlay();

	// Get Owner Pawn
	OwnerPawn = Cast<APawn>(GetOwner());

	// Get the owner pawn's BodyCollider
	if (IsValid(OwnerPawn.Get()))
	{
		OwnerPawnBodyCollider = OwnerPawn->FindComponentByClass<UCapsuleComponent>();
	}
}

void UQuakeCharacterMovementComponent::GroundAccelerate(void)
{
	const float HostFrameTime = GetWorld()->GetDeltaSeconds();

	const float Alignment = Velocity.Dot(WishMoveDirection);
	const float AddSpeed = MaxWalkSpeed - Alignment;
	if (AddSpeed <= 0)
		return;

	const float AccelSpeed = FMath::Min(AccelerationSpeed * HostFrameTime * MaxWalkSpeed, AddSpeed);
	Velocity += AccelSpeed * WishMoveDirection;
}

void UQuakeCharacterMovementComponent::UserFriction(void)
{
	const float HostFrameTime = GetWorld()->GetDeltaSeconds();

	const float Speed = (Velocity.Size() * FVector(1.0f, 1.0f, 0.0f)).Size();
	if (Speed < 1)
	{
		Velocity = FVector::ZeroVector;
		return;
	}

	float Drop = 0.0f;
	if (bOnGround)
	{
		const float Control = Speed < StopSpeed ? StopSpeed : Speed;
		Drop += Control * Friction * HostFrameTime;
	}

	const float NewSpeed = FMath::Max(Speed - Drop, 0.0f) / Speed;
	Velocity *= NewSpeed;
}

void UQuakeCharacterMovementComponent::AirAccelerate(void)
{
	const float WishSpeed = FMath::Min(WishMoveDirection.Size(), 30.0f);
	const float CurrentSpeed = Velocity.Dot(WishMoveDirection); //not actually the speed, instead how closely the two directions match
	float AddSpeed = WishSpeed - CurrentSpeed;

	if (AddSpeed <= 0.0f)
		return;

}

void UQuakeCharacterMovementComponent::MovePawn() const
{
	const UWorld* World = GetWorld();
	if (IsValid(OwnerPawn.Get()) && IsValid(OwnerPawnBodyCollider.Get()))
	{
		const FVector MoveDirection = Velocity * MaxWalkSpeed * World->GetDeltaSeconds();

		const float BodyRadius = OwnerPawnBodyCollider->GetScaledCapsuleRadius();
		const float BodyHalfHeight = OwnerPawnBodyCollider->GetScaledCapsuleHalfHeight();
		
		const FVector Start = OwnerPawn->GetActorLocation();
		const FVector Goal = Start + MoveDirection;

		FHitResult Result;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerPawn.Get());

		/*if (const bool DidCollide = World->SweepSingleByChannel(
			Result,
			Start,
			Goal,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeCapsule(BodyRadius * 0.5f, BodyHalfHeight * 0.5f),
			QueryParams
		))
		{
			UE_LOG(LogTemp, Log, TEXT("%s"), *Result.GetActor()->GetName());
			const FVector NewLocation = Result.Location;
			OwnerPawn->SetActorLocation(NewLocation, true);
		}
		else
		{*/
			//No collision, move as intended
			OwnerPawn->AddActorWorldOffset(MoveDirection, true);
		//}
	}
}

void UQuakeCharacterMovementComponent::DumpDebugLogs() const
{
	if (!IsValid(GEngine))
		return;

	if (DebugLevel >= QCMCDL_All)
	{
		const FString AlignmentDebug = FString::Format(TEXT("Wish Direction Alignment: {0}"), { Velocity.Dot(WishMoveDirection) });
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, AlignmentDebug);

		const FString OnGroundDebug = FString::Format(TEXT("Is On Ground: {0}"), { bOnGround });
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, OnGroundDebug);
	}
	
	if (DebugLevel >= QCMCDL_Basic)
	{
		const FString WishVelocityDebug = FString::Format(TEXT("Wish Move Direction: {0}"), { WishMoveDirection.ToString() });
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, WishVelocityDebug);
		
		const FString VelocityDebug = FString::Format(TEXT("Velocity: {0}"), { Velocity.ToString() });
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, VelocityDebug);

		const FString SpeedDebug = FString::Format(TEXT("Current Speed: {0}"), { Velocity.Size() });
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, SpeedDebug);
	}
}
