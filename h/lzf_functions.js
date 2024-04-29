let buffer = "";

function readNumber(offset, size, bigEndian) {
	let bytes = document.Read(offset, size);
	let num = 0;
	let i = 0;
	while (i < size) {
		if (bigEndian == 0) {
			num += bytes[i] << (i * 8); // Direct order addition
		}
		else {
			num += bytes[size - 1 - i] << (i * 8); // Inverse order addition
		}
		i += 1;
	}
	return num;
}

function getPadding(offset, alignment) {
	if (offset % alignment == 0) {
		return 0;
	}
	return alignment - (offset % alignment);
}

function decodeChar(input) {
	if (typeof input == "number") {
		return String.fromCodePoint(input);
	}
	else {
		return "";
	}
}

function parseInteger(string, radix) {
	return parseInt(string, radix);
}

function LZFBlock_len(thisOffset) {
	return readNumber(thisOffset, 1, 0) >> 5;
}

function LZFBlock_high_distance(thisOffset) {
	return readNumber(thisOffset, 1, 0) & 0x1f;
}

function LZFBlock_length(thisOffset) {
	return readNumber(thisOffset + 1, 1, 0);
}

function LZFBlock_count(thisOffset) {
	let len = LZFBlock_len(thisOffset);
	if (len == 0) {
		let high_distance = LZFBlock_high_distance(thisOffset);
		return high_distance + 1;
	}
	if (len == 7) {
		let length = LZFBlock_length(thisOffset);
		return len + length;
	}
	return len + 2;
}

function LZFBlock_sizeof(thisOffset) {
	let len = LZFBlock_len(thisOffset);
	if (len == 0) {
		let high_distance = LZFBlock_high_distance(thisOffset);
		return high_distance + 2;
	}
	if (len == 7) {
		return 3;
	}
	return 2;
}

function LZFChunk_blocks_offset(thisOffset, index) {
	let currentIndex = 0;
	let currentOffset = thisOffset;
	while (currentIndex < index) {
		currentOffset += LZFBlock_sizeof(currentOffset);
		currentIndex++;
	}
	return currentOffset;
}

function LZFChunk_blocks_at_len(thisOffset, index) {
	let offset = LZFChunk_blocks_offset(thisOffset, index);
	return LZFBlock_len(offset);
}

function LZFChunk_blocks_at_count(thisOffset, index) {
	let offset = LZFChunk_blocks_offset(thisOffset, index);
	return LZFBlock_count(offset);
}
