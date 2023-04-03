// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LitUpPrism.generated.h"

UCLASS()
class LITUP_API ALitUpPrism : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		UStaticMesh* PrismMeshAsset;

	UPROPERTY(EditAnywhere)
		float RefractionIndex = 1.5f;

	/** Allows Tick To happen in the editor viewport*/
	virtual bool ShouldTickIfViewportsOnly() const override;
public:	
	// Sets default values for this actor's properties
	ALitUpPrism();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float getRefractionIndex();

//private:
//	UMaterialInstanceDynamic* dynamicPrismMaterialInstanceDynamic;
};
