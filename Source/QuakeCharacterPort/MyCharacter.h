#pragma once

#include "CoreMinimal.h"
#include "QuakeCharacter/QuakeCharacter.h"
#include "MyCharacter.generated.h"

UCLASS()
class QUAKECHARACTERPORT_API AMyCharacter : public AQuakeCharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(const float DeltaTime) override;

private:
	FVector2D AnalogMoveVector = FVector2D::ZeroVector;

private:
	void InputMoveForward(const float Axis) { AnalogMoveVector.Y = Axis; }
	void InputMoveRight(const float Axis) { AnalogMoveVector.X = Axis; }
};
