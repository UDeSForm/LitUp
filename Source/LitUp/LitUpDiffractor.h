// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LitUpDiffractor.generated.h"

UCLASS()
class LITUP_API ALitUpDiffractor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent* Diffractor;

	UPROPERTY(EditAnywhere)
		UStaticMesh* DiffractorMeshAsset;

	/** Allows Tick To happen in the editor viewport*/
	virtual bool ShouldTickIfViewportsOnly() const override;
public:	
	// Sets default values for this actor's properties
	ALitUpDiffractor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void exec();
};
