#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "QuakeCharacter.generated.h"

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
	
protected:
	/** The body capsule collider of the quake character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	UCapsuleComponent* BodyCollider;

private:
	/** The arrow sticking out of the body capsule collider that is displayed in the editor */
	UPROPERTY()
	TObjectPtr<UArrowComponent> EditorForwardArrow;
};