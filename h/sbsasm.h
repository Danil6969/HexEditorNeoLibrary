#include "stddefs.h"

#pragma pack(1)

#pragma script("sbsasm_functions.js")

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
	while (i < size) {
		var charCode = chars[i].charCode;
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
	while (i < size) {
		var charCode = chars[i].charCode;
		if ((charCode & 0xF800) == 0xD800) {
			i = i + 1;
			$assert(i < size, "Invalid UTF16StringNoSize (1)");
			text = text + decodeChar(surrogatePairToCodePoint(charCode, chars[i].charCode));
		}
		else {
			text = text + decodeChar(charCode);
		}
		i = i + 1;
	}
	$print("text", text);
};

[display(charValue)]
struct UTF16AlignedCharacter
{
	wchar_t charCode;
	var charValue = decodeChar(charCode);
	$print("charValue", charValue);
	BYTE padding[2];
};

[display(text)]
struct UTF16AlignedString
{
	ULONG size;
	UTF16AlignedCharacter chars[size];
	var i = 0;
	var text = "";
	while (i < size) {
		var charCode = chars[i].charCode;
		if ((charCode & 0xF800) == 0xD800) {
			i = i + 1;
			$assert(i < size, "Invalid UTF16AlignedString (1)");
			text = text + decodeChar(surrogatePairToCodePoint(charCode, chars[i].charCode));
		}
		else {
			text = text + decodeChar(charCode);
		}
		i = i + 1;
	}
	$print("text", text);
};

[display(text)]
struct NameRecord
{
	WORD platformID;
	WORD encodingID;
	WORD languageID;
	WORD nameID;
	WORD length;
	var text = "";
	if (encodingID == 0) {
		var size = length;
		WORD stringOffset as UTF8StringNoSize *(baseNamingTable + storageOffset);
		text = readUTF8StringNoSize(baseNamingTable + storageOffset + stringOffset, length, 1);
	}
	if (encodingID == 1 || encodingID == 3) {
		var size = length / 2; // pass size to unsized string
		WORD stringOffset as UTF16StringNoSize *(baseNamingTable + storageOffset);
		text = readUTF16StringNoSize(baseNamingTable + storageOffset + stringOffset, length, 1);
	}
	$print("text", text);
};

struct NamingTable
{
#pragma byte_order(BigEndian)
	var baseNamingTable = this;
	WORD version;
	$assert(version == 0 || version == 1, "Invalid NamingTable (1)");
	WORD count;
	WORD storageOffset;
	NameRecord nameRecord[count];
#pragma byte_order(LittleEndian)
};

[display(tableTag)]
struct TableRecord
{
#pragma byte_order(BigEndian)
	char tableTag[4];
	UINT checksum;
	if (tableTag == "name") {
		UINT offset as NamingTable *(baseTTF);
	}
	else {
		UINT offset;
	}
	UINT length;
	currentTableIndex = currentTableIndex + 1;
#pragma byte_order(LittleEndian)
};

[display(fontFamily + " : " + fontSubfamily)]
public struct TTF
{
#pragma byte_order(BigEndian)
	var baseTTF = this;
	UINT sfntVersion;
	$assert(sfntVersion == 0x10000, "Invalid TTF (1)");
	WORD numTables;
	WORD searchRange;
	WORD entrySelector;
	WORD rangeShift;
	var currentTableIndex = 0;
	TableRecord tableRecords[numTables];

	var offNamingTable = TTF_NamingTableOffset(baseTTF);
	$assert(offNamingTable != 0, "Invalid TTF (2)");
	var valStorageOffset = NamingTable_storageOffset(offNamingTable);
	var offNameRecord0 = NamingTable_nameRecordOffset(offNamingTable, 0);

	var offNameRecord1 = NamingTable_nameRecordOffset(offNamingTable, 1);
	var lengthString1 = readNumber(offNameRecord1 + 8, 2, 1);
	var valStringOffset1 = readNumber(offNameRecord1 + 10, 2, 1);
	var offString1 = offNamingTable + valStorageOffset + valStringOffset1;
	var fontFamily = readUTF16StringNoSize(offString1, lengthString1, 1);
	$print("fontFamily", fontFamily);

	var offNameRecord2 = NamingTable_nameRecordOffset(offNamingTable, 2);
	var lengthFontSubfamily = readNumber(offNameRecord2 + 8, 2, 1);
	var valStringOffset2 = readNumber(offNameRecord2 + 10, 2, 1);
	var offFontSubfamily = offNamingTable + valStorageOffset + valStringOffset2;
	var fontSubfamily = readUTF16StringNoSize(offFontSubfamily, lengthFontSubfamily, 1);
	$print("fontSubfamily", fontSubfamily);
#pragma byte_order(LittleEndian)
};

[display(red + " " + green + " " + blue + " " + alpha)]
struct Float4
{
	FLOAT red;
	FLOAT green;
	FLOAT blue;
	FLOAT alpha;
};

struct parameterText
{
	UINT constantValueString as UTF16AlignedString *(0x34);
};

enum filterType : BYTE
{
	FILTER_UNIFORM =	0x01,
	FILTER_TEXT =		0x03,
	FILTER_BLUR =		0x0b,
};

struct filterUniform
{
	UINT unk1;
	Float4 outputcolor;
};

struct filterText
{
	BYTE unk1 : 2;
	BYTE flag_background : 2;
	BYTE unk3 : 4;
	BYTE flag_fontsize : 1;
	BYTE unk4 : 7;
	BYTE unk5;
	BYTE unk6;
	UINT unk8;
	UINT val;
	if (val == 0) {
		$remove_to(ref(val));
		[onread("\"\"")]
		UINT text;
	}
	else {
		$remove_to(ref(val));
		parameterText text;
	}
	UINT unk9;
	UINT unk10;
	if (flag_background == 3) {
		UINT input;
	}
	else if (flag_background != 0) {
		$assert(0, "Invalid filterText(1)");
	}
	if (flag_fontsize == 1) {
		FLOAT fontsize;
	}
};

struct filterBlur
{
	UINT unk1;
};

public struct compFilter
{
	BYTE unk1;
	BYTE unk2;
	BYTE unk3;
	$assert(unk2 == 0x88, "Invalid filter (1)");
	$assert(unk3 == 0x09, "Invalid filter (2)");
	BYTE filterType;
	switch (filterType)
	{
		case FILTER_UNIFORM:
			filterUniform filter;
			break;
		case FILTER_TEXT:
			filterText filter;
			break;
		case FILTER_BLUR:
			filterBlur filter;
			break;
		default:
			$assert(0, "Invalid filter (3)");
	}
};
