// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKSerializerService.generated.h"

class UScriptStruct;
class UPackageMap;
class UFunKSerializerService;
class FProperty;

USTRUCT()
struct FFunKSerialization
{
	GENERATED_BODY()

	TArray<uint8> Data;
};

USTRUCT()
struct FFunKTransport
{
	GENERATED_BODY()

	const UScriptStruct* StructType = nullptr;
	TSharedPtr<void> Struct = nullptr;

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FFunKTransport& Value);

	template <typename TStruct>
	static FFunKTransport Pack(const TStruct& Struct)
	{
		auto StructType = TStruct::StaticStruct();
		check(StructType);

		FFunKTransport Transport;
		Transport.StructType = StructType;
		Transport.Struct = TSharedPtr<void>(new TStruct(Struct));
		return Transport;
	}

	template <typename TStruct>
	bool Unpack(TStruct& OutStruct) const
	{
		auto UnpackStructType = TStruct::StaticStruct();
		check(UnpackStructType);

		if (UnpackStructType != StructType)
			return false;

		OutStruct = *static_cast<TStruct*>(Struct.Get());
		return true;
	}
};

USTRUCT()
struct FFunKDeserializationResult : public FFunKTransport
{
	GENERATED_BODY()

	bool Success = false;
};

class FFunKNetBitWriter : public FNetBitWriter
{
public:
	FFunKNetBitWriter(const UFunKSerializerService* InSerializerService, UPackageMap * InPackageMap, int64 InMaxBits);

	virtual FString GetArchiveName() const override;

	bool Net(FFunKTransport& Transport);

private:
	const UFunKSerializerService* SerializerService;
};

class FFunKNetBitReader : public FNetBitReader
{
public:
	FFunKNetBitReader(const UFunKSerializerService* InSerializerService, UPackageMap* InPackageMap, const uint8* Src, int64 CountBits);

	virtual FString GetArchiveName() const override;

	bool Net(FFunKTransport& Transport);
	
private:
	const UFunKSerializerService* SerializerService;
};


/**
 * 
 */
UCLASS()
class FUNK_API UFunKSerializerService : public UObject
{
	GENERATED_BODY()

public:
	UFunKSerializerService();
	
	bool NetSerializeStruct(FFunKSerialization& Serialization, const UScriptStruct* StructType, void* Struct, UPackageMap* PackageMap = nullptr) const;

	template <typename TStruct>
	bool NetSerializeStruct(FFunKSerialization& Serialization, TStruct& Struct, UPackageMap* PackageMap = nullptr) const
	{
		const UScriptStruct* StructType = TStruct::StaticStruct();
		if (!StructType) return false;

		return NetSerializeStruct(Serialization, StructType, &Struct, PackageMap);
	}

	FFunKDeserializationResult NetDeserializeStruct(const FFunKSerialization& Serialization, UPackageMap* PackageMap = nullptr, const UScriptStruct* StructType = nullptr) const;

	template <typename TStruct>
	bool NetDeserializeStruct(const FFunKSerialization& Serialization, TStruct& Struct, UPackageMap* PackageMap = nullptr) const
	{
		const UScriptStruct* StructType = TStruct::StaticStruct();
		if (StructType == nullptr) return false;

		FFunKDeserializationResult Result = NetDeserializeStruct(Serialization, PackageMap, StructType);
		if (!Result.Success) return false;

		return Result.Unpack(Struct);
	}

private:
	TMap<int32, const UScriptStruct*> StructTypeMap;

	const UScriptStruct* GetStructType(int32 StructTypeId) const;

	bool NetSerializeStruct(FFunKNetBitWriter& Writer, const UScriptStruct* StructType, void* Struct) const;
	bool NetDeserializeStruct(FFunKNetBitReader& Reader, FFunKTransport& Transport, const UScriptStruct* StructType = nullptr) const;

	bool NetSerializeStruct(FArchive& Archive, UPackageMap* PackageMap, const UScriptStruct* StructType, void* Struct) const;
	bool NetSerializeStructProperty(FArchive& Archive, UPackageMap* PackageMap, const FProperty* Property, void* PropertyData) const;

	friend FFunKNetBitWriter;
	friend FFunKNetBitReader;
};
