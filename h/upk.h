#include "stddefs.h"

#pragma pack(1)

public struct UnrealPackage
{
	BYTE Signature[4];
	var LittleEndianCode = true;
	var BigEndianCode = true;

	LittleEndianCode = LittleEndianCode && Signature[0] == 0xC1;
	LittleEndianCode = LittleEndianCode && Signature[1] == 0x83;
	LittleEndianCode = LittleEndianCode && Signature[2] == 0x2A;
	LittleEndianCode = LittleEndianCode && Signature[3] == 0x9E;
	BigEndianCode = BigEndianCode && Signature[0] == 0x9E;
	BigEndianCode = BigEndianCode && Signature[1] == 0x2A;
	BigEndianCode = BigEndianCode && Signature[2] == 0x83;
	BigEndianCode = BigEndianCode && Signature[3] == 0xC1;

	if (BigEndianCode) {
#pragma byte_order(BigEndian)
		$print("Encoding", "BigEndian");
	}
	else {
		$assert(LittleEndianCode, "This isn't an UnrealPackage!");
#pragma byte_order(LittleEndian)
		$print("Encoding", "LittleEndian");
	}

	if (BigEndianCode) {
		USHORT LicenseeVersion;
		USHORT Version;
	}
	else {
		USHORT Version;
		USHORT LicenseeVersion;
	}
};
