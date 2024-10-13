// Fill out your copyright notice in the Description page of Project Settings.


#include "Net/FunKSerializerService.h"

FArchive& operator<<(FArchive& Ar, FFunKTransport& Value)
{
	if (Ar.GetArchiveName() == "FFunKNetBitWriter")
	{
		FFunKNetBitWriter& FunKNetBitWriter = static_cast<FFunKNetBitWriter&>(Ar);
		FunKNetBitWriter.Net(Value);
	}
	else if (Ar.GetArchiveName() == "FFunKNetBitReader")
	{
		FFunKNetBitReader& FunKNetBitReader = static_cast<FFunKNetBitReader&>(Ar);
		FunKNetBitReader.Net(Value);
	}
	else
	{
		check(false);
	}

	return Ar;
}


FFunKNetBitWriter::FFunKNetBitWriter(const UFunKSerializerService* InSerializerService, UPackageMap* InPackageMap, int64 InMaxBits)
	: FNetBitWriter(InPackageMap, InMaxBits)
	, SerializerService(InSerializerService)
{
}

FString FFunKNetBitWriter::GetArchiveName() const
{
	return FString("FFunKNetBitWriter");
}

bool FFunKNetBitWriter::Net(FFunKTransport& Transport)
{
	return this->SerializerService->NetSerializeStruct(*this, Transport.StructType, Transport.Struct.Get());
}

FFunKNetBitReader::FFunKNetBitReader(const UFunKSerializerService* InSerializerService, UPackageMap* InPackageMap, const uint8* Src, int64 CountBits)
		: FNetBitReader(InPackageMap, Src, CountBits)
		, SerializerService(InSerializerService)
{
}

FString FFunKNetBitReader::GetArchiveName() const
{
	return FString("FFunKNetBitReader");
}

bool FFunKNetBitReader::Net(FFunKTransport& Transport)
{
	return this->SerializerService->NetDeserializeStruct(*this, Transport);
}

UFunKSerializerService::UFunKSerializerService()
{
	for (TObjectIterator<UScriptStruct> StructIt; StructIt; ++StructIt)
	{
		const UScriptStruct* Struct = *StructIt;
		if (Struct->HasMetaData("FunK"))
		{
			StructTypeMap.Add(Struct->GetUniqueID(), Struct);
		}
	}
}

bool UFunKSerializerService::NetSerializeStruct(FFunKSerialization& Serialization, const UScriptStruct* StructType, void* Struct, UPackageMap* PackageMap) const
{
	FName FunkFlag = "FunK";
	if (!StructType->HasMetaData(FunkFlag))
		return false;

	const uint32 Size = StructType->GetStructureSize() * 2;
	FFunKNetBitWriter Writer = FFunKNetBitWriter(this, PackageMap, Size);

	uint32 StructId = StructType->GetUniqueID();
	Writer << StructId;

	bool bSuccess = NetSerializeStruct(Writer, Writer.PackageMap, StructType, Struct);
	
	Serialization.Data = *Writer.GetBuffer();
	
	return bSuccess;
}

FFunKDeserializationResult UFunKSerializerService::NetDeserializeStruct(const FFunKSerialization& Serialization, UPackageMap* PackageMap, const UScriptStruct* StructType) const
{
	FFunKNetBitReader Reader = FFunKNetBitReader(this, PackageMap, Serialization.Data.GetData(), Serialization.Data.Num() * 8);

	FFunKDeserializationResult Result;
	Result.Success = NetDeserializeStruct(Reader, Result, StructType);
	
	return Result;
}

const UScriptStruct* UFunKSerializerService::GetStructType(int32 StructTypeId) const
{
	if (!StructTypeMap.Contains(StructTypeId))
		return nullptr;

	return StructTypeMap[StructTypeId];
}

bool UFunKSerializerService::NetSerializeStruct(FFunKNetBitWriter& Writer, const UScriptStruct* StructType, void* Struct) const
{
	uint32 StructId = StructType->GetUniqueID();
	Writer << StructId;

	return NetSerializeStruct(Writer, Writer.PackageMap, StructType, Struct);
}

bool UFunKSerializerService::NetDeserializeStruct(FFunKNetBitReader& Reader, FFunKTransport& Transport, const UScriptStruct* StructType) const
{
	uint32 StructId = 0;
	Reader << StructId;

	const auto ArchivedStructType = GetStructType(StructId);
	if (!ArchivedStructType)
		return false;

	if (StructType && ArchivedStructType != StructType)
		return false;

	Transport.StructType = ArchivedStructType;
	Transport.Struct = TSharedPtr<void>(FMemory::Malloc(ArchivedStructType->GetStructureSize(), ArchivedStructType->GetMinAlignment()));
	ArchivedStructType->InitializeStruct(Transport.Struct.Get());
	
	return NetSerializeStruct(Reader, Reader.PackageMap, ArchivedStructType, Transport.Struct.Get());
}

bool UFunKSerializerService::NetSerializeStruct(FArchive& Archive, UPackageMap* PackageMap, const UScriptStruct* StructType, void* Struct) const
{
	if (StructType->StructFlags & STRUCT_NetSerializeNative)
	{
		bool bSuccess = true;
		return  StructType->GetCppStructOps()->NetSerialize(Archive, PackageMap, bSuccess, Struct) && bSuccess;
	}
	
	for (TFieldIterator<FProperty> It(StructType); It; ++It)
	{
		if (It->PropertyFlags & CPF_RepSkip)
		{
			continue;
		}

		void* PropertyData = It->ContainerPtrToValuePtr<void*>(Struct);
		if (!PropertyData) continue;

		if (!NetSerializeStructProperty(Archive, PackageMap, *It, PropertyData))
			return false;
				
		if (Archive.IsError())
			return false;
	}

	return true;
}

bool UFunKSerializerService::NetSerializeStructProperty(FArchive& Archive, UPackageMap* PackageMap, const FProperty* Property, void* PropertyData) const
{
	const FStructProperty* StructProperty = CastField<FStructProperty>(Property);
		
	if (!StructProperty)
		return Property->NetSerializeItem(Archive, PackageMap, PropertyData);
	
	if (StructProperty->Struct != FFunKTransport::StaticStruct())
		return NetSerializeStruct(Archive, PackageMap, StructProperty->Struct, PropertyData);
	
	FFunKTransport& TransportStruct = *static_cast<FFunKTransport*>(PropertyData);
	Archive << TransportStruct;

	return !Archive.IsError();
}
