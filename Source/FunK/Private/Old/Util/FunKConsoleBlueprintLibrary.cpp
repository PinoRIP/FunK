// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/Util/FunKConsoleBlueprintLibrary.h"
#include "Old/FunKLogging.h"

#define GET_CONSOLE_VAR(Name, IsValid, Default) IConsoleVariable* Variable = GetVariable(Name, IsValid); if (!IsValid) return Default;
#define PROXY_GET_CONSOLE_VARIABLE(Name, Func) bool IsValid = false; auto Result = Func(Name, IsValid); if (!IsValid) UE_LOG(FunKLog, Warning, TEXT("Invalid console variable %s"), *Name); return Result;
#define PROXY_SET_CONSOLE_VARIABLE(Name, Value, Func) bool IsValid = false; auto Result = Func(Name, Value, IsValid); if (!IsValid) UE_LOG(FunKLog, Warning, TEXT("Invalid console variable %s"), *Name); return Result;

FString UFunKConsoleBlueprintLibrary::GetConsoleVariable(const FString& Name, bool& IsValid)
{
	GET_CONSOLE_VAR(Name, IsValid, "");
	return Variable->GetString();	
}

FString UFunKConsoleBlueprintLibrary::GetConsoleVariable(const FString& Name)
{
	PROXY_GET_CONSOLE_VARIABLE(Name, GetConsoleVariable);
}

FString UFunKConsoleBlueprintLibrary::SetConsoleVariable(const FString& Name, const FString& Value, bool& IsValid)
{
	GET_CONSOLE_VAR(Name, IsValid, "");

	FString OldValue = Variable->GetString();

	Variable->Set(*Value);
	
	return OldValue;
}

FString UFunKConsoleBlueprintLibrary::SetConsoleVariable(const FString& Name, const FString& Value)
{
	PROXY_SET_CONSOLE_VARIABLE(Name, Value, SetConsoleVariable);
}

float UFunKConsoleBlueprintLibrary::GetConsoleVariableFloat(const FString& Name, bool& IsValid)
{
	GET_CONSOLE_VAR(Name, IsValid, 0.f);
	return Variable->GetFloat();	
}

float UFunKConsoleBlueprintLibrary::GetConsoleVariableFloat(const FString& Name)
{
	PROXY_GET_CONSOLE_VARIABLE(Name, GetConsoleVariableFloat);
}

float UFunKConsoleBlueprintLibrary::SetConsoleVariableFloat(const FString& Name, float Value, bool& IsValid)
{
	GET_CONSOLE_VAR(Name, IsValid, 0.f);

	const float OldValue = Variable->GetFloat();

	Variable->Set(Value);
	
	return OldValue;
}

float UFunKConsoleBlueprintLibrary::SetConsoleVariableFloat(const FString& Name, float Value)
{
	PROXY_SET_CONSOLE_VARIABLE(Name, Value, SetConsoleVariableFloat);
}

bool UFunKConsoleBlueprintLibrary::GetConsoleVariableBool(const FString& Name, bool& IsValid)
{
	GET_CONSOLE_VAR(Name, IsValid, false);
	return Variable->GetBool();	
}

bool UFunKConsoleBlueprintLibrary::GetConsoleVariableBool(const FString& Name)
{
	PROXY_GET_CONSOLE_VARIABLE(Name, GetConsoleVariableBool);
}

bool UFunKConsoleBlueprintLibrary::SetConsoleVariableBool(const FString& Name, bool Value, bool& IsValid)
{
	GET_CONSOLE_VAR(Name, IsValid, false);

	const bool OldValue = Variable->GetBool();

	Variable->Set(Value);
	
	return OldValue;
}

bool UFunKConsoleBlueprintLibrary::SetConsoleVariableBool(const FString& Name, bool Value)
{
	PROXY_SET_CONSOLE_VARIABLE(Name, Value, SetConsoleVariableBool);
}

int32 UFunKConsoleBlueprintLibrary::GetConsoleVariableInt(const FString& Name, bool& IsValid)
{
	GET_CONSOLE_VAR(Name, IsValid, 0);
	return Variable->GetInt();	
}

int32 UFunKConsoleBlueprintLibrary::GetConsoleVariableInt(const FString& Name)
{
	PROXY_GET_CONSOLE_VARIABLE(Name, GetConsoleVariableInt);
}

int32 UFunKConsoleBlueprintLibrary::SetConsoleVariableInt(const FString& Name, int32 Value, bool& IsValid)
{
	GET_CONSOLE_VAR(Name, IsValid, 0);

	const int32 OldValue = Variable->GetInt();

	Variable->Set(Value);
	
	return OldValue;
}

int32 UFunKConsoleBlueprintLibrary::SetConsoleVariableInt(const FString& Name, int32 Value)
{
	PROXY_SET_CONSOLE_VARIABLE(Name, Value, SetConsoleVariableInt);
}

IConsoleVariable* UFunKConsoleBlueprintLibrary::GetVariable(const FString& Name, bool& IsValid)
{
	IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(*Name);
	IsValid = !!Variable;
	return Variable;
}
