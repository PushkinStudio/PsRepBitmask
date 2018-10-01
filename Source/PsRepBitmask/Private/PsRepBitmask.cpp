// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsRepBitmaskPrivatePCH.h"
#include "PsRepBitmask.h"

#define LOCTEXT_NAMESPACE "PsRemoteImageModule"

void FRepBitmaskPair::WriteToBitmask(const int32& Index, uint64& Mask) const
{
	const uint64 BitIn64 = 1;
	if (*Value) 
	{
		Mask |= BitIn64 << Index;
	}
	else
	{
		Mask &= ~(BitIn64 << Index);
	}
}

void FRepBitmaskPair::ReadFromBitmask(const int32& Index, const uint64& Mask)
{
	const bool NewValue = (bool)((Mask >> Index) & 1);
	bool& CurrentValue = *Value;
	if (CurrentValue != NewValue)
	{
		CurrentValue = NewValue;
		if (Delegate.IsBound())
		{
			Delegate.ProcessDelegate<UObject>(nullptr);
		}
	}
}

FRepBitmask::FRepBitmask() : Bitmask(0), bBuild(false)
{
	Pairs.Reserve(32);
}

FRepBitmask& FRepBitmask::Add(bool* Value, UObject* Object, FName FunctionName)
{
	checkf(!bBuild, TEXT("Can't add property after build"))
	Pairs.Add(FRepBitmaskPair(Value, Object, FunctionName));
	return *this;
}

FRepBitmask& FRepBitmask::Add(bool* Value)
{
	checkf(!bBuild, TEXT("Can't add property after build"))
	Pairs.Add(FRepBitmaskPair(Value));
	return *this;
}

void FRepBitmask::Build()
{
	if (!bBuild)
	{
		bBuild = true;
		InternalSerialize();
	}
}

void FRepBitmask::InternalSerialize()
{
	Bitmask = GetBitmask();
}

void FRepBitmask::InternalDeserialize()
{
	for(int32 i = 0; i < Pairs.Num(); ++i)
	{
		Pairs[i].ReadFromBitmask(i, Bitmask);
	}
}

uint64 FRepBitmask::GetBitmask() const
{
	if (bBuild)
	{
		uint64 Result = 0;
		for(int32 i = 0; i < Pairs.Num(); ++i)
		{
			Pairs[i].WriteToBitmask(i, Result);
		}
		return Result;
	}
	else
	{
		return Bitmask;
	}
}

FRepBitmask::FRepBitmask(const FRepBitmask& Other)
{
	*this = Other;
}

void FRepBitmask::operator=(const FRepBitmask& Other)
{
	Bitmask = Other.GetBitmask();
}

bool FRepBitmask::operator==(const FRepBitmask& Other) const
{
	return GetBitmask() == Other.GetBitmask();
}

bool FRepBitmask::operator!=(const FRepBitmask& Other) const
{
	return GetBitmask() != Other.GetBitmask();
}

bool FRepBitmask::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsLoading())
	{
		NetSerialize_Load(Ar);
	}
	else if (Ar.IsSaving())
	{
		NetSerialize_Save(Ar);
	}
	bOutSuccess = true;
	return true;
}

void FRepBitmask::NetSerialize_Save(FArchive& Ar)
{
	InternalSerialize();
	
	int32 Bits = 0;
	while(Bits < Pairs.Num())
	{
		uint8 Byte = (uint8)((Bitmask >> Bits) & 0xFF);
		Ar << Byte;
		Bits += 8;
	}
}

void FRepBitmask::NetSerialize_Load(FArchive& Ar)
{
	Bitmask = 0;
	
	int32 Bits = 0;
	while(Bits < Pairs.Num())
	{
		uint8 Byte = 0;
		Ar << Byte;
		uint64 ByteIn64 = Byte;
		Bitmask |= (ByteIn64 << Bits);
		
		Bits += 8;
	}
	
	InternalDeserialize();
}

FBitmaskIndices::FBitmaskIndices(const FBitmaskIndices& Other)
{
	*this = Other;
}

void FBitmaskIndices::Add(const uint8 Index)
{
	if (Index < GetMaxStorageIndex())
	{
		const uint64 BitIn64 = 1;
		Bitmask |= BitIn64 << Index;
	}
}

void FBitmaskIndices::Remove(const uint8 Index)
{
	if (Index < GetMaxStorageIndex())
	{
		const uint64 BitIn64 = 1;
		Bitmask &= ~(BitIn64 << Index);
	}
}

bool FBitmaskIndices::Contains(const uint8 Index) const
{
	if (Index < GetMaxStorageIndex())
	{
		return (bool)((Bitmask >> Index) & 1);
	}

	return false;
}

void FBitmaskIndices::Empty()
{
	Bitmask = 0;
}

TSet<uint8> FBitmaskIndices::GetIndices() const
{
	TSet<uint8> Indices;

	for (uint8 i = 0; i < GetMaxStorageIndex(); ++i)
	{
		if (Contains(i))
		{
			Indices.Add(i);
		}
	}

	return Indices;
}

uint64 FBitmaskIndices::GetBitmask() const
{
	return Bitmask;
}

void FBitmaskIndices::operator=(const FBitmaskIndices& Other)
{
	Bitmask = Other.Bitmask;
}

bool FBitmaskIndices::operator==(const FBitmaskIndices& Other) const
{
	return GetBitmask() == Other.GetBitmask();
}

bool FBitmaskIndices::operator!=(const FBitmaskIndices& Other) const
{
	return GetBitmask() != Other.GetBitmask();
}

bool FBitmaskIndices::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsLoading())
	{
		NetSerialize_Load(Ar);
	}
	else if (Ar.IsSaving())
	{
		NetSerialize_Save(Ar);
	}
	bOutSuccess = true;
	return true;
}

uint8 FBitmaskIndices::GetMaxStorageIndex() const
{
	return 64;
}

void FBitmaskIndices::NetSerialize_Save(FArchive& Ar)
{
	int32 Bits = 0;
	while (Bits < GetMaxStorageIndex())
	{
		uint8 Byte = (uint8)((Bitmask >> Bits) & 0xFF);
		Ar << Byte;
		Bits += 8;
	}
}

void FBitmaskIndices::NetSerialize_Load(FArchive& Ar)
{
	Bitmask = 0;

	int32 Bits = 0;
	while (Bits < GetMaxStorageIndex())
	{
		uint8 Byte = 0;
		Ar << Byte;
		uint64 ByteIn64 = Byte;
		Bitmask |= (ByteIn64 << Bits);

		Bits += 8;
	}
}

#undef LOCTEXT_NAMESPACE
