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
