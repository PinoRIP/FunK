// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/Extensions/FunKTestVariationsWorldActor.h"


// Sets default values
AFunKTestVariationsWorldActor::AFunKTestVariationsWorldActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AFunKTestVariationsWorldActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFunKTestVariationsWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

