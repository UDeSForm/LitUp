// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpLightRay.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

ALitUpLightRay::ALitUpLightRay()
{
	//static ConstructorHelpers::FObjectFinder<UStaticMesh>CylinderMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	//Cylinder->SetStaticMesh(CylinderMeshAsset.Object);
	//Cylinder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//static ConstructorHelpers::FObjectFinder<UMaterial>laserMaterial(TEXT("Material '/Game/Materials/M_Laser.M_Laser'"));
	//Cylinder->SetMaterial(0, laserMaterial.Object);
}

ALitUpLightRay::~ALitUpLightRay()
{
}

FHitResult ALitUpLightRay::getResult(const float& length, const FVector& Start, const FVector& ForwardVector, const AActor* parent, const UWorld* world)
{
	FHitResult OutHit;
	FVector End = ((ForwardVector * length) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(parent);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1, 0, 5);

	if (world->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		Cylinder->SetWorldTransform(FTransform(FRotator(90, 0, 0) + ForwardVector.Rotation(), (OutHit.Location - Start) / 2.f + Start, FVector(0.05, 0.05, OutHit.Distance / 100.f)));
	}
	else
	{
		Cylinder->SetRelativeTransform(FTransform(FRotator(90, 0, 0), FVector(length / 2.f, 0, 0), FVector(0.05, 0.05, length / 100.f)));
	}

	return OutHit;
}