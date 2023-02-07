// Fill out your copyright notice in the Description page of Project Settings.

#include "LitUpLightEmitter.h"
#include "GameFramework/Actor.h"

// Sets default values
ALitUpLightEmitter::ALitUpLightEmitter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshOne = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh One"));
	RootComponent = MeshOne;

	MeshTwo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Two"));
	MeshTwo->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALitUpLightEmitter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALitUpLightEmitter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult OutHit;
	FVector Start = GetActorLocation();

	Start.Z += 50.f;
	Start.X += 200.f;

	FVector ForwardVector = GetActorForwardVector();
	FVector End = ((ForwardVector * 500.f) + Start);
	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);

	if (ActorLineTraceSingle(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("The Component Being Hit is: %s"), *OutHit.GetComponent()->GetName()));
		}
	}
}

