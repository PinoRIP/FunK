// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/FunKServiceProviderSubsystem.h"

UObject* UFunKServiceProviderSubsystem::Service(const UClass* ServiceClass)
{
	UObject* Service = FindService(ServiceClass);
	return Service
		? Service
		: CreateService(ServiceClass);
}

UObject* UFunKServiceProviderSubsystem::FindService(const UClass* ServiceClass)
{
	for (UObject* ServiceInstance : ServiceInstances)
	{
		if (ServiceInstance->GetClass() == ServiceClass)
			return ServiceInstance;
	}

	return nullptr;
}

UObject* UFunKServiceProviderSubsystem::CreateService(const UClass* ServiceClass)
{
	UObject* Service = NewObject<UObject>(this, ServiceClass);
	ServiceInstances.Add(Service);

	return Service;
}
