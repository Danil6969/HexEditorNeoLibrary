#include "stddefs.h"

#pragma pack(1)

#pragma script("upk_functions.js")

[display(charValue)]
struct UTF8Character
{
	UCHAR charCode;
	var charValue = decodeChar(charCode);
	$print("charValue", charValue);
};

[display(text)]
struct UTF8StringNoSize
{
	$print("size", size);
	UTF8Character chars[size];
	var i = 0;
	var text = "";
	while (i < size)
	{
		var charCode = chars[i].charCode;
		if (charCode == 0)
		{
			break;
		}
		text = text + decodeChar(charCode);
		i = i + 1;
	}
	$print("text", text);
};

[display(charValue)]
struct UTF16Character
{
	WCHAR charCode;
	var charValue = decodeChar(charCode);
	$print("charValue", charValue);
};

[display(text)]
struct UTF16StringNoSize
{
	$print("size", size);
	UTF16Character chars[size];
	var i = 0;
	var text = "";
	while (i < size)
	{
		var charCode = chars[i].charCode;
		if (charCode == 0)
		{
			break;
		}
		if ((charCode & 0xF800) == 0xD800)
		{
			i = i + 1;
			$assert(i < size, "Invalid UTF16StringNoSize (1)");
			text = text + decodeChar(surrogatePairToCodePoint(charCode, chars[i].charCode));
		}
		else
		{
			text = text + decodeChar(charCode);
		}
		i = i + 1;
	}
	$print("text", text);
};

[display(text)]
struct String
{
	var size;
	if (length == 0)
	{
		$print("text", "");
	}
	else
	{
		if (length > 0)
		{
			size = length;
			UTF8StringNoSize utf8String;
			$print("text", utf8String.text);
		}
		else
		{
			size = -length;
			UTF16StringNoSize utf16String;
			$print("text", utf16String.text);
		}
	}
};

[display(string.text)]
struct Text
{
	INT unfixedSize;
	var length = unfixedSize;
	String string;
};

struct PackageFileSummary
{
	const VHeaderSize = 249;
	const VFolderName = 269;
	const maxLegacyVersion = -7;

	INT legacyVersion;
	if (legacyVersion < 0 && legacyVersion >= maxLegacyVersion)
	{
		
	}
	else
	{
		$remove_to(ref(legacyVersion));
		if (BigEndianCode)
		{
			USHORT LicenseeVersion;
			USHORT Version;
		}
		else
		{
			USHORT Version;
			USHORT LicenseeVersion;
		}
		$print("Package Version:", Version + "/" + LicenseeVersion);
	}
	if (Version >= VHeaderSize)
	{
		INT HeaderSize;
	}
	if (Version >= VFolderName)
	{
		Text FolderName;
	}
};

public struct UnrealPackage
{
	BYTE readSignature[4];
	var LittleEndianCode = true;
	var BigEndianCode = true;

	LittleEndianCode = LittleEndianCode && readSignature[0] == 0xC1;
	LittleEndianCode = LittleEndianCode && readSignature[1] == 0x83;
	LittleEndianCode = LittleEndianCode && readSignature[2] == 0x2A;
	LittleEndianCode = LittleEndianCode && readSignature[3] == 0x9E;
	BigEndianCode = BigEndianCode && readSignature[0] == 0x9E;
	BigEndianCode = BigEndianCode && readSignature[1] == 0x2A;
	BigEndianCode = BigEndianCode && readSignature[2] == 0x83;
	BigEndianCode = BigEndianCode && readSignature[3] == 0xC1;

	if (BigEndianCode)
	{
#pragma byte_order(BigEndian)
		$print("Encoding", "BigEndian");
	}
	else
	{
		$assert(LittleEndianCode, "This isn't an UnrealPackage!");
#pragma byte_order(LittleEndian)
		$print("Encoding", "LittleEndian");
	}

	PackageFileSummary Summary;
};
