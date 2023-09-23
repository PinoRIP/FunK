// Fill out your copyright notice in the Description page of Project Settings.


#include "Internal/Events/FunKEvent.h"

bool FFunKEvent::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Type;
	Ar << Message;
	Ar << Context;

	bOutSuccess = true;
	return true;
}
