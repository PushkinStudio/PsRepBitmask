// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PsRepBitmask.generated.h"

/** Pair for replication bool parameter as bitmask */
struct FRepBitmaskPair
{
	/** Constructor with "OnRep" function */
	FRepBitmaskPair(bool* InValue, UObject* Object, FName FunctionName) : Value(InValue)
	{
		Delegate.BindUFunction(Object, FunctionName);
	}
	
	/** Constructor without "OnRep" function */
	FRepBitmaskPair(bool* InValue) : Value(InValue) {}
	
	/** Boolean value */
	bool* Value;
	
	/** "OnRep" delegate */
	FScriptDelegate Delegate;
	
	/** Write value to bitmask */
	void WriteToBitmask(const int32& Index, uint64& Mask) const;
	
	/** Read value from bitmask and invoke "OnRep" delegate */
	void ReadFromBitmask(const int32& Index, const uint64& Mask);
};

/** Struct for replication bool parameter as bitmask */
USTRUCT(BlueprintType)
struct FRepBitmask
{
public:
	GENERATED_USTRUCT_BODY()
	
	/** Constructor */
	FRepBitmask();
	
	/** Add boolean flag with "OnRep" function */
	FRepBitmask& Add(bool* Value, UObject* Object, FName FunctionName);
	
	/** Add boolean flag without "OnRep" function */
	FRepBitmask& Add(bool* Value);
	
	/** Build bitmask. Use this method for  */
	void Build();
	
private:
	/** Bit handler array */
	TArray<FRepBitmaskPair> Pairs;
	
	/** Bitmask */
	uint64 Bitmask;
	
	/** Bitmask redy to replication */
	bool bBuild;
	
	/** Serialize to bitmask */
	FORCEINLINE void InternalSerialize();
	
	/** Deserialize from bitmask */
	FORCEINLINE void InternalDeserialize();
	
	/** Get bitmask for comparison */
	FORCEINLINE uint64 GetBitmask() const;
	
public:
	/** Copy constructor */
	FRepBitmask(const FRepBitmask& Other);
	
	/** We override the assignment operator to the transfer of the bitmask correctly */
	void operator=(const FRepBitmask& Other);
	
	/** Comparison operator */
	bool operator==(const FRepBitmask& Other) const;
	
	/** Comparison operator */
	bool operator!=(const FRepBitmask& Other) const;
	
	/** Serialize for replication */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	
private:
	/** NetSerialize: Save */
	FORCEINLINE void NetSerialize_Save(FArchive& Ar);
	
	/** NetSerialize: Load */
	FORCEINLINE void NetSerialize_Load(FArchive& Ar);
};

/** Override ustruct property for custom replication */
template<>
struct TStructOpsTypeTraits<FRepBitmask> : public TStructOpsTypeTraitsBase2<FRepBitmask>
{
	enum
	{
		WithCopy = true,
		WithIdenticalViaEquality = true,
		WithNetSerializer = true
	};
};

/** Struct for index set replication as bitmask (max index = 63) */
USTRUCT(BlueprintType)
struct FBitmaskIndices
{
public:
	GENERATED_USTRUCT_BODY()

	/** Constructor */
	FBitmaskIndices()
	{
		Bitmask = 0;
	}

	/** Adds an index */
	void Add(const uint8 Index);

	/** Removes an index */
	void Remove(const uint8 Index);

	/** Checks if given index has been added */
	bool Contains(const uint8 Index) const;

	/** Clears bit mask */
	void Empty();

	/** Returns a list of indices */
	TSet<uint8> GetIndices() const;

	/** Get bitmask for comparison */
	FORCEINLINE uint64 GetBitmask() const;

private:

	/** Bitmask */
	uint64 Bitmask;

public:
	/** Copy constructor */
	FBitmaskIndices(const FBitmaskIndices& Other);

	/** We override the assignment operator to the transfer of the bitmask correctly */
	void operator=(const FBitmaskIndices& Other);

	/** Comparison operator */
	bool operator==(const FBitmaskIndices& Other) const;

	/** Comparison operator */
	bool operator!=(const FBitmaskIndices& Other) const;

	/** Serialize for replication */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	/** Returns max index that cannot be saved to bitmask set (64) */
	uint8 GetMaxStorageIndex() const;

private:
	/** NetSerialize: Save */
	FORCEINLINE void NetSerialize_Save(FArchive& Ar);

	/** NetSerialize: Load */
	FORCEINLINE void NetSerialize_Load(FArchive& Ar);

};

/** Override ustruct property for custom replication */
template<>
struct TStructOpsTypeTraits<FBitmaskIndices> : public TStructOpsTypeTraitsBase2<FBitmaskIndices>
{
	enum
	{
		WithCopy = true,
		WithIdenticalViaEquality = true,
		WithNetSerializer = true
	};
};
