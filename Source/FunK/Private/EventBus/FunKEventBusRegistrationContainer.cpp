// Fill out your copyright notice in the Description page of Project Settings.


#include "EventBus/FunKEventBusRegistrationContainer.h"

void FFunKEventBusRegistrationContainer::Unregister()
{
	for (FFunKEventBusRegistration& Registration : Registrations)
		Registration.Unregister();
}

void FFunKEventBusRegistrationContainer::Add(const FFunKEventBusRegistration& Registration)
{
	Registrations.Add(Registration);
}
