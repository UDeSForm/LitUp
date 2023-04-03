// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpPrism.h"

// Sets default values
ALitUpPrism::ALitUpPrism()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//static ConstructorHelpers::FObjectFinder<UMaterial>prismMaterial(TEXT("/Script/Engine.Material'/Game/CustomActors/Materials/RefractionLayerMaterial.RefractionLayerMaterial'"));
	//dynamicPrismMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(prismMaterial.Object, PrismFaceTop);

	//PrismFaceTop->SetMaterial(0, dynamicPrismMaterialInstanceDynamic);
	//PrismFaceFront->SetMaterial(0, dynamicPrismMaterialInstanceDynamic);
	//PrismFaceBack->SetMaterial(0, dynamicPrismMaterialInstanceDynamic);
	//PrismFaceLeft->SetMaterial(0, dynamicPrismMaterialInstanceDynamic);
	//PrismFaceRight->SetMaterial(0, dynamicPrismMaterialInstanceDynamic);
	//PrismFaceBottom->SetMaterial(0, dynamicPrismMaterialInstanceDynamic);
}

// Called when the game starts or when spawned
void ALitUpPrism::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALitUpPrism::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//dynamicPrismMaterialInstanceDynamic->SetScalarParameterValue(FName("Refraction"), RefractionIndex);
}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
//But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool ALitUpPrism::ShouldTickIfViewportsOnly() const
{
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float ALitUpPrism::getRefractionIndex()
{
	return RefractionIndex;
}