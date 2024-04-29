#include "stddefs.h"

#pragma pack(1)

#pragma script("lzf_functions.js")

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

// Virtual bytes workaround which are not bound to actual address and don't have any length
[display(format("{0w2f0arb016X}",value))]
struct DecodedByte
{
	$assert(array_index >= 0, "Invalid DecodedByte(1)");
	$print("value", bytesBuffer[array_index]);
};

[display(format("Display of {0b016X} bytes",bytesCount))]
struct DecodedString
{
	var formatString = "{0w2f0arb16X}";
	var groupLength = 0x10;
	var remainingBytes;
	var i;
	var processedBytes = 0;
	for (remainingBytes = bytesCount; remainingBytes >= groupLength; remainingBytes = remainingBytes - groupLength) {
		var result = "";
		for (i = 0; i < groupLength; i = i + 1) {
			result = result + " " + format(formatString,bytesBuffer[processedBytes + i]);
		}
		processedBytes = processedBytes + groupLength;
		$print("", result);
	}
	var result = "";
	for (i = 0; i < remainingBytes; i = i + 1) {
		result = result + " " + format(formatString,bytesBuffer[processedBytes + i]);
	}
	processedBytes = processedBytes + remainingBytes;
	$print("", result);
};

struct IndirectBytesDecoder
{
	$assert(array_index >= 0, "Invalid IndirectBytesDecoder(1)");
	var fetchPos = outPos - distance;
	$print("fetchPos", fetchPos);
	var groupLength = distance + 1;
	var remainingBytes;
	var i;
	var processedBytes = 0;
	for (remainingBytes = count; remainingBytes >= groupLength; remainingBytes = remainingBytes - groupLength) {
		for (i = 0; i < groupLength; i = i + 1) {
			bytesBuffer[processedBytes + i] = decodedChunkBytes[fetchPos + i - 1];
		}
		processedBytes = processedBytes + groupLength;
	}
	for (i = 0; i < remainingBytes; i = i + 1) {
		bytesBuffer[processedBytes + i] = decodedChunkBytes[fetchPos + i - 1];
	}
};

struct DirectBytesDecoder
{
	BYTE rawBytes[count];
	for (var i = 0; i < count; i = i + 1) {
		bytesBuffer[i] = rawBytes[i];
	}
};

struct LZFBlock
{
	$print("outPos", outPos);
	BYTE high_distance : 5;
	BYTE len : 3;
	inPos = inPos + 1;
	var count = 0;
	var distance = 0;
	switch (len)
	{
		case 0:
			count = high_distance + 1;
			$print("count", count);
			var bytesBuffer[count] = {0};
			DirectBytesDecoder bytesDecoder;
			inPos = inPos + count;
			DecodedByte decodedBlockBytes[count];
			break;
		case 7:
			BYTE length;
			inPos = inPos + 1;
			BYTE low_distance;
			inPos = inPos + 1;
			count = length + 9;
			$print("count", count);
			distance = low_distance + (high_distance << 8);
			$print("distance", distance);
			var bytesBuffer[count] = {0};
			IndirectBytesDecoder bytesDecoder;
			DecodedByte decodedBlockBytes[count];
			break;
		default:
			BYTE low_distance;
			inPos = inPos + 1;
			count = len + 2;
			$print("count", count);
			distance = low_distance + (high_distance << 8);
			$print("distance", distance);
			var bytesBuffer[count] = {0};
			IndirectBytesDecoder bytesDecoder;
			DecodedByte decodedBlockBytes[count];
	}
	for (var i = 0; i < count; i = i + 1) {
		decodedChunkBytes[outPos + i] = decodedBlockBytes[i].value;
	}
	outPos = outPos + count;
	if (chunkLength <= inPos) {
		$break_array(true);
	}
};

public struct LZFChunk
{
	CHAR magic[2];
	$assert(magic[0] == 'Z', "Invalid LZF(1)");
	$assert(magic[1] == 'V', "Invalid LZF(2)");
	BYTE type;
	$assert(type == 1, "Invalid LZF(3)");
#pragma byte_order(BigEndian)
	USHORT chunkLength;
	USHORT originalLength;
#pragma byte_order(LittleEndian)
	var decodedChunkBytes[originalLength] = {0};
	var decodedChunkBytes[originalLength] = {0};
	for (var i = 0; i < originalLength; i = i + 1) {
		decodedChunkBytes[i] = 0;
	}
	var inPos = 0;
	var outPos = 0;
	var chunkLength = chunkLength;
	var blocks_offset = this + 7;
	LZFBlock blocks[*];
	var bytesCount = originalLength;
	var bytesBuffer[bytesCount] = {0};
	for (var i = 0; i < bytesCount; i = i + 1) {
		bytesBuffer[i] = decodedChunkBytes[i];
	}
	DecodedByte decodedBytes[bytesCount];
	DecodedString decodedString;
};
