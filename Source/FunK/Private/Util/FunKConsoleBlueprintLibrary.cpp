// Fill out your copyright notice in the Description page of Project Settings.


#include "Util/FunKConsoleBlueprintLibrary.h"
#include "FunKLogging.h"

#define GetConsoleVar(Name, IsValid, Default) IConsoleVariable* Variable = GetVariable(Name, IsValid); if (!IsValid) return Default;
#define ProxyGetConsoleVariable(Name, Func) bool IsValid = false; auto Result = Func(Name, IsValid); if (!IsValid) UE_LOG(FunKLog, Warning, TEXT("Invalid console variable %s"), *Name); return Result;
#define ProxySetConsoleVariable(Name, Value, Func) bool IsValid = false; auto Result = Func(Name, Value, IsValid); if (!IsValid) UE_LOG(FunKLog, Warning, TEXT("Invalid console variable %s"), *Name); return Result;

FString UFunKConsoleBlueprintLibrary::GetConsoleVariable(const FString& Name, bool& IsValid)
{
	GetConsoleVar(Name, IsValid, "");
	return Variable->GetString();	
}

FString UFunKConsoleBlueprintLibrary::GetConsoleVariable(const FString& Name)
{
	ProxyGetConsoleVariable(Name, GetConsoleVariable);
}

FString UFunKConsoleBlueprintLibrary::SetConsoleVariable(const FString& Name, const FString& Value, bool& IsValid)
{
	GetConsoleVar(Name, IsValid, "");

	FString OldValue = Variable->GetString();

	Variable->Set(*Value);
	
	return OldValue;
}

FString UFunKConsoleBlueprintLibrary::SetConsoleVariable(const FString& Name, const FString& Value)
{
	ProxySetConsoleVariable(Name, Value, SetConsoleVariable);
}

float UFunKConsoleBlueprintLibrary::GetConsoleVariableFloat(const FString& Name, bool& IsValid)
{
	GetConsoleVar(Name, IsValid, 0.f);
	return Variable->GetFloat();	
}

float UFunKConsoleBlueprintLibrary::GetConsoleVariableFloat(const FString& Name)
{
	ProxyGetConsoleVariable(Name, GetConsoleVariableFloat);
}

float UFunKConsoleBlueprintLibrary::SetConsoleVariableFloat(const FString& Name, float Value, bool& IsValid)
{
	GetConsoleVar(Name, IsValid, 0.f);

	const float OldValue = Variable->GetFloat();

	Variable->Set(Value);
	
	return OldValue;
}

float UFunKConsoleBlueprintLibrary::SetConsoleVariableFloat(const FString& Name, float Value)
{
	ProxySetConsoleVariable(Name, Value, SetConsoleVariableFloat);
}

bool UFunKConsoleBlueprintLibrary::GetConsoleVariableBool(const FString& Name, bool& IsValid)
{
	GetConsoleVar(Name, IsValid, false);
	return Variable->GetBool();	
}

bool UFunKConsoleBlueprintLibrary::GetConsoleVariableBool(const FString& Name)
{
	ProxyGetConsoleVariable(Name, GetConsoleVariableBool);
}

bool UFunKConsoleBlueprintLibrary::SetConsoleVariableBool(const FString& Name, bool Value, bool& IsValid)
{
	GetConsoleVar(Name, IsValid, false);

	const bool OldValue = Variable->GetBool();

	Variable->Set(Value);
	
	return OldValue;
}

bool UFunKConsoleBlueprintLibrary::SetConsoleVariableBool(const FString& Name, bool Value)
{
	ProxySetConsoleVariable(Name, Value, SetConsoleVariableBool);
}

int32 UFunKConsoleBlueprintLibrary::GetConsoleVariableInt(const FString& Name, bool& IsValid)
{
	GetConsoleVar(Name, IsValid, 0);
	return Variable->GetInt();	
}

int32 UFunKConsoleBlueprintLibrary::GetConsoleVariableInt(const FString& Name)
{
	ProxyGetConsoleVariable(Name, GetConsoleVariableInt);
}

int32 UFunKConsoleBlueprintLibrary::SetConsoleVariableInt(const FString& Name, int32 Value, bool& IsValid)
{
	GetConsoleVar(Name, IsValid, 0);

	const int32 OldValue = Variable->GetInt();

	Variable->Set(Value);
	
	return OldValue;
}

int32 UFunKConsoleBlueprintLibrary::SetConsoleVariableInt(const FString& Name, int32 Value)
{
	ProxySetConsoleVariable(Name, Value, SetConsoleVariableInt);
}

IConsoleVariable* UFunKConsoleBlueprintLibrary::GetVariable(const FString& Name, bool& IsValid)
{
	IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(*Name);
	IsValid = !!Variable;
	return Variable;
}
