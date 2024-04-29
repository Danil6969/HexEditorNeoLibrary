#include "stddefs.h"

#pragma pack(1)

#pragma script("nvidia_tegra_kernel_functions.js")

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

[display(chars.text)]
struct UTF8Hexademical8
{
	var size = 8;
	UTF8StringNoSize chars;
	var value = parseInteger(chars.text, 16);
	$print("value", value);
};

public struct CpioHeader
{
	CHAR magic[6];
	$assert(magic[0] == '0', "invalid CpioHeader(1)");
	$assert(magic[1] == '7', "invalid CpioHeader(2)");
	$assert(magic[2] == '0', "invalid CpioHeader(3)");
	$assert(magic[3] == '7', "invalid CpioHeader(4)");
	$assert(magic[4] == '0', "invalid CpioHeader(5)");
	$assert(magic[5] == '1', "invalid CpioHeader(6)");
	UTF8Hexademical8 ino;
	UTF8Hexademical8 mode;
	UTF8Hexademical8 uid;
	UTF8Hexademical8 gid;
	UTF8Hexademical8 nlink;
	UTF8Hexademical8 mtime;
	UTF8Hexademical8 filesize;
	UTF8Hexademical8 devmajor;
	UTF8Hexademical8 devminor;
	UTF8Hexademical8 rdevmajor;
	UTF8Hexademical8 rdevminor;
	UTF8Hexademical8 namesize;
	UTF8Hexademical8 check;
	var size = namesize.value - 1;
	UTF8StringNoSize name;
	var isTrailer = namesize.value == 11;
	if (isTrailer) {
		isTrailer = isTrailer && name.chars[0].charCode == 'T';
		isTrailer = isTrailer && name.chars[1].charCode == 'R';
		isTrailer = isTrailer && name.chars[2].charCode == 'A';
		isTrailer = isTrailer && name.chars[3].charCode == 'I';
		isTrailer = isTrailer && name.chars[4].charCode == 'L';
		isTrailer = isTrailer && name.chars[5].charCode == 'E';
		isTrailer = isTrailer && name.chars[6].charCode == 'R';
		isTrailer = isTrailer && name.chars[7].charCode == '!';
		isTrailer = isTrailer && name.chars[8].charCode == '!';
		isTrailer = isTrailer && name.chars[9].charCode == '!';
	}
	BYTE terminator;
	var pad = getPadding(&terminator + 1, 4);
	BYTE padding1[pad];
	BYTE file[filesize.value];
	if (filesize.value > 0) {
		pad = getPadding(&file + filesize.value, 4);
		BYTE padding2[pad];
	}
	if (isTrailer) {
		$break_array(true);
	}
};

// Virtual bytes workaround which are not bound to actual address and don't have any length
[display(format("{0w2f0arb16x}",value))]
struct DecodedByte
{
	$assert(array_index >= 0, "Invalid DecodedByte(1)");
	$print("value", bytesBuffer[array_index]);
};

struct DirectBytesDecoder
{
	BYTE rawBytes[count];
	for (var i = 0; i < count; i = i + 1) {
		bytesBuffer[i] = rawBytes[i];
	}
};

struct IndirectBytesDecoder
{
	$assert(array_index >= 0, "Invalid IndirectBytesDecoder(1)");
	var fetchPos = outPos - distance;
	$print("fetchPos", fetchPos);
	for (var i = 0; i < count; i = i + 1) {
		bytesBuffer[i] = decodedChunkBytes[fetchPos + i - 1];
	}
};

struct tegrabl_ecdsasig
{
	BYTE r[48];
	BYTE s[48];
};

struct tegrabl_cryptosignature
{
	BYTE cryptohash[16];
	BYTE rsapsssig[256];
	tegrabl_ecdsasig ecdsasig;
};

public struct tegrabl_sigheader
{
	CHAR headermagic[4];
	$assert(headermagic == "GSHV", "Invalid tegrabl_sigheader(1)");
	UINT headerversion;
	tegrabl_cryptosignature signatures;
	BYTE padding[8];
	BYTE binarymagic[4];
	UINT signtype;
	UINT binarylength;
	USHORT fw_ratchet_level;
	BYTE reserved[2];
	ULONGLONG compression[14];
	var isCompressed = true;
	//isCompressed = isCompressed && compression[0] == 1;
	isCompressed = isCompressed && compression[1] == 0;
	isCompressed = isCompressed && compression[2] == 0;
	isCompressed = isCompressed && compression[3] == 0;
	isCompressed = isCompressed && compression[4] == 0;
	isCompressed = isCompressed && compression[5] == 0;
	isCompressed = isCompressed && compression[6] == 0;
	isCompressed = isCompressed && compression[7] == 0;
	isCompressed = isCompressed && compression[8] == 0;
	isCompressed = isCompressed && compression[9] == 0;
	isCompressed = isCompressed && compression[10] == 0;
	isCompressed = isCompressed && compression[11] == 0;
	isCompressed = isCompressed && compression[12] == 0;
	isCompressed = isCompressed && compression[13] == 0;
	if (!isCompressed) {
		$remove_to(ref(compression));
	}
	BYTE file[binarylength];
};
