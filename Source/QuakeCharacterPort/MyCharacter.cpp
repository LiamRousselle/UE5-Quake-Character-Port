#include "MyCharacter.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMyCharacter::InputMoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AMyCharacter::InputMoveRight);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &AMyCharacter::Jump);
}

void AMyCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector Forward = (GetActorForwardVector() * FVector(1.0f, 1.0f, 0.0f)).GetSafeNormal();
	const FVector Right = (GetActorRightVector() * FVector(1.0f, 1.0f, 0.0f)).GetSafeNormal();
	const FVector WishMoveDirection = (Forward * AnalogMoveVector.Y + Right * AnalogMoveVector.X).GetSafeNormal();

	AddMovementInput(WishMoveDirection);
}