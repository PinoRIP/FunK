// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKLogging.h"
#include "FunKAssertions.generated.h"

class IFunKSink;

UENUM(BlueprintType)
enum class EFunKComparisonMethod : uint8
{
	None					UMETA(Hidden),
	EqualTo					UMETA(DisplayName="Equal to"),
	NotEqualTo				UMETA(DisplayName="Not equal to"),
	GreaterThanOrEqualTo	UMETA(DisplayName="Greater than or equal to"),
	LessThanOrEqualTo		UMETA(DisplayName="Less than or equal to"),
	GreaterThan				UMETA(DisplayName="Greater than"),
	LessThan				UMETA(DisplayName="Less than")
};

/**
 * 
 */
UCLASS()
class FUNK_API UFunKAssertions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertTrue(bool Condition, const FString& Message, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertFalse(bool Condition, const FString& Message, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertIsValid(UObject* Object, const FString& Message, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertInt(int32 Actual, EFunKComparisonMethod ShouldBe, int32 Expected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertFloat(float Actual, EFunKComparisonMethod ShouldBe, float Expected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertDouble(double Actual, EFunKComparisonMethod ShouldBe, double Expected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertDateTime(FDateTime Actual, EFunKComparisonMethod ShouldBe, FDateTime Expected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertTransformEqual(const FTransform& Actual, const FTransform& Expected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertTransformNotEqual(const FTransform& Actual, const FTransform& NotExpected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertFloatEqual(float Actual, float Expected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertFloatNotEqual(float Actual, float NotExpected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertDoubleEqual(double Actual, double Expected, const FString& What, UObject* Context, double Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertDoubleNotEqual(double Actual, double NotExpected, const FString& What, UObject* Context, double Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertNameEqual(FName Actual, FName Expected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertNameNotEqual(FName Actual, FName NotExpected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertStringEqual(FString Actual, FString Expected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertStringNotEqual(FString Actual, FString NotExpected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertObjectEqual(UObject* Actual, UObject* Expected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertObjectNotEqual(UObject* Actual, UObject* NotExpected, const FString& What, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertRotatorEqual(FRotator Actual, FRotator Expected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertRotatorNotEqual(FRotator Actual, FRotator NotExpected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertVectorEqual(FVector Actual, FVector Expected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertVectorNotEqual(FVector Actual, FVector NotExpected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertVector2DEqual(FVector2D Actual, FVector2D Expected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertVector2DNotEqual(FVector2D Actual, FVector2D NotExpected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertVector4Equal(FVector4 Actual, FVector4 Expected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertVector4NotEqual(FVector4 Actual, FVector4 NotExpected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AsserQuatEqual(FQuat Actual, FQuat Expected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertQuatNotEqual(FQuat Actual, FQuat NotExpected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AsserMatrixEqual(FMatrix Actual, FMatrix Expected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool AssertMatrixNotEqual(FMatrix Actual, FMatrix NotExpected, const FString& What, UObject* Context, float Tolerance = 1.e-4f);


private:
	static bool Assert(TFunctionRef<bool()> Assertion, const FString& Message, UObject* Context, int32 stackOffset = 2);

	FORCEINLINE static FString BuildMessage(const FString& What, const FString& Expected, const FString& Actual, EFunKComparisonMethod ComparisonMethod = EFunKComparisonMethod::None, const FString Ext = "");
	FORCEINLINE static FString GetObjectName(UObject* object);
	
	static FString GetComparisonName(EFunKComparisonMethod comparison);
	template <typename T>
	static bool PerformComparison(const T& lhs, const T& rhs, EFunKComparisonMethod comparison)
	{
		switch (comparison)
		{
		case EFunKComparisonMethod::EqualTo:
			return lhs == rhs;

		case EFunKComparisonMethod::NotEqualTo:
			return lhs != rhs;

		case EFunKComparisonMethod::GreaterThanOrEqualTo:
			return lhs >= rhs;

		case EFunKComparisonMethod::LessThanOrEqualTo:
			return lhs <= rhs;

		case EFunKComparisonMethod::GreaterThan:
			return lhs > rhs;

		case EFunKComparisonMethod::LessThan:
			return lhs < rhs;
		default:
			UE_LOG(FunKLog, Error, TEXT("Invalid comparison method: %s"), *GetComparisonName(comparison));
			return false;
		}
	}
};
