﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKStages.h"
#include "FunKTestBase.h"
#include "FunKStagesSetup.generated.h"

struct FFunKLatentStageSetup;
struct FFunKStageSetup;

USTRUCT()
struct FUNK_API FFunKStagesSetupBase
{
	GENERATED_BODY()

public:
	FFunKStagesSetupBase()
		: FFunKStagesSetupBase(nullptr, nullptr)
	{ }
	FFunKStagesSetupBase(FFunKStages* Stages, AFunKTestBase* TestBase)
		: TestBase(TestBase)
		, Stages(Stages)
	{ }
	FFunKStagesSetupBase(const FFunKStagesSetupBase&) = delete;
	
protected:
	UPROPERTY()
	AFunKTestBase* TestBase;
	FFunKStages* Stages;
	
	template <typename UserClass, typename... VarTypes>
	FORCEINLINE FFunKStageSetup EmplaceNewStage(const FName& stageName, int32 index, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars) const;

	template <typename UserClass, typename... VarTypes>
	FORCEINLINE FFunKLatentStageSetup EmplaceNewLatentStage(const FName& stageName, int32 index, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars) const;
};

USTRUCT()
struct FUNK_API FFunKStageSetupBase : public FFunKStagesSetupBase
{
	GENERATED_BODY()

public:
	FFunKStageSetupBase()
		: FFunKStageSetupBase(nullptr, nullptr, nullptr)
	{ }
	FFunKStageSetupBase(FFunKStage* Stage, FFunKStages* Stages, AFunKTestBase* TestBase)
		: FFunKStagesSetupBase(Stages, TestBase)
		, Stage(Stage)
	{ }
	FFunKStageSetupBase(const FFunKStageSetupBase&) = delete;

	template <typename UserClass, typename... VarTypes>
	FORCEINLINE FFunKStageSetup ThenAddNamedStage(const FName& stageName, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars);

	#define ThenAddStage(Type, InFunc) ThenAddNamedStage<Type>(STATIC_FUNCTION_FNAME(TEXT(#Type "::" #InFunc)), &Type::InFunc)

	template <typename UserClass, typename... VarTypes>
	FORCEINLINE FFunKLatentStageSetup ThenAddNamedLatentStage(const FName& stageName, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars);
	
	#define ThenAddLatentStage(Type, InFunc) ThenAddNamedLatentStage<Type>(STATIC_FUNCTION_FNAME(TEXT(#Type "::" #InFunc)), &Type::InFunc)
	
	FFunKStageSetupBase& SetRunOnStandalone(bool value)
	{
		if(Stages)
			Update(Stages->OnStandaloneCount, Stage->IsOnStandalone, value);
		
		return *this;
	}

	FFunKStageSetupBase& SetRunOnDedicated(bool value)
	{
		SetRunOnDedicatedServer(value);
		return SetRunOnDedicatedServerClient(value);
	}

	FFunKStageSetupBase& SetRunOnDedicatedServer(bool value)
	{
		if(Stages)
			Update(Stages->OnDedicatedServerCount, Stage->IsOnDedicatedServer, value);
		
		return *this;
	}

	FFunKStageSetupBase& SetRunOnDedicatedServerClient(bool value)
	{
		if(Stages)
			Update(Stages->OnDedicatedServerClientCount, Stage->IsOnDedicatedServerClient, value);
		
		return *this;
	}

	FFunKStageSetupBase& SetRunOnListen(bool value)
	{
		SetRunOnListenServer(value);
		return SetRunOnListenServerClient(value);
	}

	FFunKStageSetupBase& SetRunOnListenServer(bool value)
	{
		if(Stages)
			Update(Stages->OnListenServerCount, Stage->IsOnListenServer, value);
		
		return *this;
	}

	FFunKStageSetupBase& SetRunOnListenServerClient(bool value)
	{
		if(Stages)
			Update(Stages->OnListenServerClientCount, Stage->IsOnListenServerClient, value);
		
		return *this;
	}

protected:
	FFunKStage* Stage;

	int32 GetStageIndex() const;
	static void Update(int32& count, bool& oldValue, bool newValue);
};

USTRUCT()
struct FUNK_API FFunKStageSetup : public FFunKStageSetupBase
{
	GENERATED_BODY()

public:
	FFunKStageSetup()
		: FFunKStageSetup(nullptr, nullptr, nullptr)
	{ }
	FFunKStageSetup(FFunKStage* Stage, FFunKStages* Stages, AFunKTestBase* TestBase)
		: FFunKStageSetupBase(Stage, Stages, TestBase)
	{ }
	FFunKStageSetup(const FFunKStageSetup&) = delete;
	
	FFunKLatentStageSetup MakeLatent();
};

USTRUCT(BlueprintType)
struct FUNK_API FFunKLatentStageSetup : public FFunKStageSetupBase
{
	GENERATED_BODY()

public:
	FFunKLatentStageSetup()
		: FFunKLatentStageSetup(nullptr, nullptr, nullptr)
	{ }
	FFunKLatentStageSetup(FFunKStage* Stage, FFunKStages* Stages, AFunKTestBase* TestBase)
		: FFunKStageSetupBase(Stage, Stages, TestBase)
	{ }
	FFunKLatentStageSetup(const FFunKLatentStageSetup&) = delete;

	template <typename UserClass, typename... VarTypes>
	FORCEINLINE FFunKLatentStageSetup& WithTickDelegate(typename TMemFunPtrType<false, UserClass, void (float, VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		if(Stage)
			Stage->TickDelegate.BindUObject(Cast<UserClass, AFunKTestBase>(TestBase), InFunc, Vars...);
		
		return *this;
	}

	#define WithOptionalBpTickDelegate(Type, InFunc) Update([](FFunKLatentStageSetup& setup) { FFunKLatentStageSetupPrivateAccessHelper::WithOptionalBpTickDelegateImpl<Type>(setup, GET_FUNCTION_NAME_CHECKED(Type, InFunc), &Type::InFunc); })

	FFunKLatentStageSetup& UpdateTimeLimitTime(const float Time)
	{
		if(Stage)
			Stage->TimeLimit.Time = Time;
		
		return *this;
	}

	FFunKLatentStageSetup& UpdateTimeLimitMessage(const FText Message)
	{
		if(Stage)
			Stage->TimeLimit.Message = Message;
		
		return *this;
	}

	FFunKLatentStageSetup& UpdateTimeLimitMessage(const FString Message)
	{
		if(Stage)
			Stage->TimeLimit.Message = FText::FromString(Message);
		
		return *this;
	}

	FFunKLatentStageSetup& UpdateTimeLimitResult(EFunKTestResult Result)
	{
		if(Stage)
			Stage->TimeLimit.Result = Result;
		
		return *this;
	}

	FFunKLatentStageSetup& UpdateTimeLimit(const FFunKTimeLimit TimeLimit)
	{
		if(Stage)
			Stage->TimeLimit = TimeLimit;
		
		return *this;
	}

	FFunKLatentStageSetup& Update(void (*f)(FFunKLatentStageSetup&))
	{
		f(*this);
		
		return *this;
	}

private:
	template <typename UserClass, typename... VarTypes>
	FORCEINLINE FFunKLatentStageSetup& WithOptionalBpTickDelegateImpl(const FName& name, typename TMemFunPtrType<false, UserClass, void (float, VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		if(Stage && TestBase && TestBase->IsBpEventImplemented(name))
			Stage->TickDelegate.BindUObject(Cast<UserClass, AFunKTestBase>(TestBase), InFunc, Vars...);
		
		return *this;
	}

	friend struct FFunKLatentStageSetupPrivateAccessHelper;
};

struct FUNK_API FFunKLatentStageSetupPrivateAccessHelper
{
	/**
	 * This should never be called directly. This is a helper for the "WithOptionalBpTickDelegate"-Makro for "FFunKLatentStageSetup"
	 */
	template <typename UserClass, typename... VarTypes>
	FORCEINLINE static FFunKLatentStageSetup& WithOptionalBpTickDelegateImpl(FFunKLatentStageSetup& setup, const FName& name, typename TMemFunPtrType<false, UserClass, void (float, VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		return setup.WithOptionalBpTickDelegateImpl<UserClass, VarTypes...>(name, InFunc, Vars...);
	}
};

template <typename StageType>
struct FUNK_API TFunKStagesSetupStageIterator
{
	TFunKStagesSetupStageIterator()
		: TFunKStagesSetupStageIterator(nullptr, nullptr)
	{ }
	TFunKStagesSetupStageIterator(FFunKStages* Stages, AFunKTestBase* TestBase)
		: Index(INDEX_NONE)
		, Stage(nullptr)
		, Stages(Stages)
		, TestBase(TestBase)
	{ }

	bool Next()
	{
		if(!Stages)
			return false;
		
		for(Index++; Index < Stages->Stages.Num(); Index++)
		{
			Stage = &Stages->Stages[Index];
			if((std::is_same_v<StageType, FFunKStageSetup> && Stage->IsLatent) || (std::is_same_v<StageType, FFunKLatentStageSetup> && !Stage->IsLatent))
				continue;

			return true;
		}

		return false;
	}

	StageType Get()
	{
		return StageType(Stage, Stages, TestBase);
	}

	void Reset()
	{
		Stages = nullptr;
		Index = INDEX_NONE;
	}

private:
	int32 Index;
	FFunKStage* Stage;
	FFunKStages* Stages;
	AFunKTestBase* TestBase;
};

typedef TFunKStagesSetupStageIterator<FFunKStageSetupBase> FFunKStageSetupBaseIterator;
typedef TFunKStagesSetupStageIterator<FFunKStageSetup> FFunKStageSetupIterator;
typedef TFunKStagesSetupStageIterator<FFunKLatentStageSetup> FFunKLatentStageSetupIterator;

USTRUCT(BlueprintType)
struct FUNK_API FFunKStagesSetup : public FFunKStagesSetupBase
{
	GENERATED_BODY()

public:
	FFunKStagesSetup()
		: FFunKStagesSetup(nullptr, nullptr)
	{ }
	FFunKStagesSetup(FFunKStages* Stages, AFunKTestBase* TestBase)
		: FFunKStagesSetupBase(Stages, TestBase)
	{ }
	FFunKStagesSetup(const FFunKStagesSetup&) = delete;

	template <typename UserClass, typename... VarTypes>
	FORCEINLINE FFunKStageSetup AddNamedStage(const FName& StageName, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars);

	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning - Macro is posing as function
	#define AddStage(Type, InFunc) AddNamedStage<Type>(STATIC_FUNCTION_FNAME(TEXT(#Type "::" #InFunc)), &Type::InFunc)

	template <typename UserClass, typename... VarTypes>
	FORCEINLINE FFunKLatentStageSetup AddNamedLatentStage(const FName& StageName, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars);

	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning - Macro is posing as function
	#define AddLatentStage(Type, InFunc) AddNamedLatentStage<Type>(STATIC_FUNCTION_FNAME(TEXT(#Type "::" #InFunc)), &Type::InFunc)

	FFunKStageSetupBaseIterator GetStageSetupBaseIterator() const { return FFunKStageSetupBaseIterator(Stages, TestBase); }
	FFunKStageSetupIterator GetStageSetupIterator() const { return FFunKStageSetupIterator(Stages, TestBase); }
	FFunKLatentStageSetupIterator GetLatentStageSetupIterator() const { return FFunKLatentStageSetupIterator(Stages, TestBase); }

	int32 Num() const;
};

template <typename UserClass, typename ... VarTypes>
FFunKStageSetup FFunKStagesSetupBase::EmplaceNewStage(const FName& stageName, int32 index, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InFunc, VarTypes... Vars) const
{
	if(!Stages || !TestBase)
		return FFunKStageSetup();
	
	return FFunKStageSetup(&Stages->EmplaceStage(stageName, index, Cast<UserClass, AFunKTestBase>(TestBase), InFunc, Vars...), Stages, TestBase);
}

template <typename UserClass, typename ... VarTypes>
FFunKLatentStageSetup FFunKStagesSetupBase::EmplaceNewLatentStage(const FName& stageName, int32 index, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InFunc, VarTypes... Vars) const
{
	if(!Stages || !TestBase)
		return FFunKLatentStageSetup();
	
	return FFunKLatentStageSetup(&Stages->EmplaceLatentStage(stageName, index, Cast<UserClass, AFunKTestBase>(TestBase), InFunc, Vars...), Stages, TestBase);
}

template <typename UserClass, typename ... VarTypes>
FFunKStageSetup FFunKStagesSetup::AddNamedStage(const FName& stageName, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InFunc, VarTypes... Vars)
{
	if(!Stages || !TestBase)
		return FFunKStageSetup();
	
	return EmplaceNewStage<UserClass, VarTypes...>(stageName, Stages->Stages.Num(), InFunc, Vars...);
}

template <typename UserClass, typename ... VarTypes>
FFunKLatentStageSetup FFunKStagesSetup::AddNamedLatentStage(const FName& stageName, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InFunc, VarTypes... Vars)
{
	if(!Stages || !TestBase)
		return FFunKLatentStageSetup();
	
	return EmplaceNewLatentStage<UserClass, VarTypes...>(stageName, Stages->Stages.Num(), InFunc, Vars...);
}

template <typename UserClass, typename ... VarTypes>
FFunKStageSetup FFunKStageSetupBase::ThenAddNamedStage(const FName& stageName, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InFunc, VarTypes... Vars)
{
	if(!Stages || !TestBase)
		return FFunKStageSetup();
	
	return EmplaceNewStage<UserClass, VarTypes...>(stageName, GetStageIndex() + 1, InFunc, Vars...);
}

template <typename UserClass, typename ... VarTypes>
FFunKLatentStageSetup FFunKStageSetupBase::ThenAddNamedLatentStage(const FName& stageName, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InFunc, VarTypes... Vars)
{
	if(!Stages || !TestBase)
		return FFunKLatentStageSetup();

	return EmplaceNewLatentStage<UserClass, VarTypes...>(stageName, GetStageIndex() + 1, InFunc, Vars...);
}
