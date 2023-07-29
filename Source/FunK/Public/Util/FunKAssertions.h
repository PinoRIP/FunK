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
	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertTrue(bool Condition, const FString& Message, TScriptInterface<IFunKSink> Sink);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertTrueInContext(bool Condition, const FString& Message, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertFalse(bool Condition, const FString& Message, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertFalseInContext(bool Condition, const FString& Message, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertIsValid(UObject* Object, const FString& Message, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertIsValidInContext(UObject* Object, const FString& Message, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertInt(int32 Actual, EFunKComparisonMethod ShouldBe, int32 Expected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertIntInContext(int32 Actual, EFunKComparisonMethod ShouldBe, int32 Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertFloat(float Actual, EFunKComparisonMethod ShouldBe, float Expected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertFloatInContext(float Actual, EFunKComparisonMethod ShouldBe, float Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertDouble(double Actual, EFunKComparisonMethod ShouldBe, double Expected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertDoubleInContext(double Actual, EFunKComparisonMethod ShouldBe, double Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertDateTime(FDateTime Actual, EFunKComparisonMethod ShouldBe, FDateTime Expected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertDateTimeInContext(FDateTime Actual, EFunKComparisonMethod ShouldBe, FDateTime Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertTransformEqual(const FTransform& Actual, const FTransform& Expected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertTransformEqualInContext(const FTransform& Actual, const FTransform& Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertTransformNotEqual(const FTransform& Actual, const FTransform& NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertTransformNotEqualInContext(const FTransform& Actual, const FTransform& NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertFloatEqual(float Actual, float Expected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertFloatEqualInContext(float Actual, float Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertFloatNotEqual(float Actual, float NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertFloatNotEqualInContext(float Actual, float NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertDoubleEqual(double Actual, double Expected, const FString& What, TScriptInterface<IFunKSink> Sink, double Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertDoubleEqualInContext(double Actual, double Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, double Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertDoubleNotEqual(double Actual, double NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink, double Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertDoubleNotEqualInContext(double Actual, double NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, double Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertNameEqual(FName Actual, FName Expected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertNameEqualInContext(FName Actual, FName Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertNameNotEqual(FName Actual, FName NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertNameNotEqualInContext(FName Actual, FName NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertStringEqual(FString Actual, FString Expected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertStringEqualInContext(FString Actual, FString Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertStringNotEqual(FString Actual, FString NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertStringNotEqualInContext(FString Actual, FString NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertObjectEqual(UObject* Actual, UObject* Expected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertObjectEqualInContext(UObject* Actual, UObject* Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertObjectNotEqual(UObject* Actual, UObject* NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertObjectNotEqualInContext(UObject* Actual, UObject* NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertRotatorEqual(FRotator Actual, FRotator Expected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertRotatorEqualInContext(FRotator Actual, FRotator Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertRotatorNotEqual(FRotator Actual, FRotator NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertRotatorNotEqualInContext(FRotator Actual, FRotator NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVectorEqual(FVector Actual, FVector Expected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVectorEqualInContext(FVector Actual, FVector Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVectorNotEqual(FVector Actual, FVector NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVectorNotEqualInContext(FVector Actual, FVector NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVector2DEqual(FVector2D Actual, FVector2D Expected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVector2DEqualInContext(FVector2D Actual, FVector2D Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVector2DNotEqual(FVector2D Actual, FVector2D NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVector2DNotEqualInContext(FVector2D Actual, FVector2D NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVector4Equal(FVector4 Actual, FVector4 Expected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVector4EqualInContext(FVector4 Actual, FVector4 Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVector4NotEqual(FVector4 Actual, FVector4 NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertVector4NotEqualInContext(FVector4 Actual, FVector4 NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AsserQuatEqual(FQuat Actual, FQuat Expected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertQuatEqualInContext(FQuat Actual, FQuat Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertQuatNotEqual(FQuat Actual, FQuat NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertQuatNotEqualInContext(FQuat Actual, FQuat NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AsserMatrixEqual(FMatrix Actual, FMatrix Expected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertMatrixEqualInContext(FMatrix Actual, FMatrix Expected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertMatrixNotEqual(FMatrix Actual, FMatrix NotExpected, const FString& What, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "FunK|Assertions", meta = ( HidePin = "Sink", DefaultToSelf = "Sink"))
	static bool AssertMatrixNotEqualInContext(FMatrix Actual, FMatrix NotExpected, const FString& What, UObject* Context, TScriptInterface<IFunKSink> Sink, float Tolerance = 1.e-4f);


private:
	static bool Assert(TFunctionRef<bool()> Assertion, const FString& Message, TScriptInterface<IFunKSink> Sink, int32 stackOffset = 2);
	static bool Assert(TFunctionRef<bool()> Assertion, const FString& Message, TScriptInterface<IFunKSink> Sink, UObject* Context, int32 stackOffset = 2);

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
