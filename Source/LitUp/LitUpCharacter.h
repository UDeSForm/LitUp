// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/OutputDeviceNull.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "LitUpCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ALitUpCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	
public:
	ALitUpCharacter();

protected:
	virtual void BeginPlay();

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	/** Is character currently controlling an object? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ObjectControl)
		bool bIsInObject;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = ObjectControl)
		void SetIsInObject(bool bNewIsInObject);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = ObjectControl)
		bool GetIsInObject();

	/** Is character currently moving an object? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ObjectControl)
		bool bIsMovingObject;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = ObjectControl)
		void SetIsMovingObject(bool bNewIsMovingObject);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = ObjectControl)
		bool GetIsMovingObject();

	/** Is character currently editing an index value? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ObjectControl)
		bool bIsEditingIndex;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = ObjectControl)
		void SetIsEditingIndex(bool bNewIsEditingIndex);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = ObjectControl)
		bool GetIsEditingIndex();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(EditAnywhere, Category = "BlueprintClass")
		AActor* bpActor;
};

