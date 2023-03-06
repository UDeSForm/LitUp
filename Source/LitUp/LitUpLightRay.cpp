// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpLightRay.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

//Ajouter les include des Actors ici
#include "LitUpLightTarget.h"

// Sets default values
ALitUpLightRay::ALitUpLightRay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Origin = CreateDefaultSubobject<USceneComponent>(TEXT("Origin"));
	SetRootComponent(Origin);

	LightRay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightRay"));
	LightRay->SetupAttachment(Origin);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CylinderMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	LightRay->SetStaticMesh(CylinderMeshAsset.Object);
	static ConstructorHelpers::FObjectFinder<UMaterial>laserMaterial(TEXT("Material '/Game/Materials/M_Laser.M_Laser'"));
	LightRay->SetMaterial(0, laserMaterial.Object);
	LightRay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LightRay->SetRelativeTransform(FTransform(FRotator(90, 0, 0), FVector(length / 2.f, 0, 0), FVector(0.05, 0.05, length / 100.f)));
	LightRay->CastShadow = false;
	LightRay->bEmissiveLightSource = true;
}

// Called when the game starts or when spawned
void ALitUpLightRay::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALitUpLightRay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Start = Origin->GetComponentLocation();
	FVector ForwardVector = Origin->GetForwardVector();
	FHitResult OutHit;
	FVector End = ((ForwardVector * length) + Start);
	FCollisionQueryParams CollisionParams;

	CollisionParams.AddIgnoredActor(GetParentActor());

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.04, 0, 10);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		LightRay->SetWorldTransform(FTransform(FRotator(90, 0, 0) + ForwardVector.Rotation(), (OutHit.Location - Start) / 2.f + Start, FVector(0.05, 0.05, OutHit.Distance / 100.f)));

		if (OutHit.GetActor()->IsA(ALitUpLightTarget::StaticClass()))
		{
			Cast<ALitUpLightTarget>(OutHit.GetActor())->exec();
		}

		/*
		Exemple pour ajouter des objets qui interagissent avec la lumière

		Prendre exemple sur LightTarget ci-haut et dans LitUpLightTarget.h et LitUpLightTarget.cpp

		Créer une classe Actor
		Ajouter le include du .h en ici en haut
		Ajouter un else if suivant le template ci-dessous
		Ajouter une fonction nommée exec dans la classe de l'Actor
		Écrire le code à exécuter lorsque l'Actor est touché dans la fonction exec

		else if (OutHit.GetActor()->IsA(/La classe de l'Actor/::StaticClass()))
		{
			Cast</La classe de l'Actor/>(OutHit.GetActor())->exec();
		}
		*/

		else
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("No hit!"));
		}
	}
	else
	{
		LightRay->SetRelativeTransform(FTransform(FRotator(90, 0, 0), FVector(length / 2.f, 0, 0), FVector(0.05, 0.05, length / 100.f)));
	}
}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
//But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool ALitUpLightRay::ShouldTickIfViewportsOnly() const
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