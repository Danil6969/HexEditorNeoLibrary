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

function readChar(byte1, byte2, bigEndian) {
	let low;
	let high;
	if (bigEndian == 0) {
		low = byte1;
		high = byte2;
	}
	else {
		low = byte2;
		high = byte1;
	}
	return low + (high << 8);
}

function decodeChar(input) {
	if (typeof input == "number") {
		return String.fromCodePoint(input);
	}
	else {
		return "";
	}
}

function surrogatePairToCodePoint(charCode1, charCode2) {
	return ((charCode1 & 0x3FF) << 10) + (charCode2 & 0x3FF) + 0x10000;
}

// size is number of bytes
function readUTF8StringNoSize(offset, size, bigEndian) {
	let bytes = document.Read(offset, size);
	let i = 0;
	let str = "";
	while (i < size) {
		let charCode = bytes[i];
		str += decodeChar(charCode);
		i += 1;
	}
	return str;
}

// size is number of bytes
function readUTF16StringNoSize(offset, size, bigEndian) {
	let bytes = document.Read(offset, size);
	let i = 0;
	let string = "";
	while (i < size) {
		let charCode = readChar(bytes[i], bytes[i + 1], bigEndian);
		if ((charCode & 0xF800) == 0xD800) {
			i += 2;
			if (i >= size) {
				throw "trying to read surrogate beyond specified size";
			}
			charCodeNext = readChar(bytes[i], bytes[i + 1], bigEndian);
			string += decodeChar(surrogatePairToCodePoint(charCode, charCodeNext));
		}
		else {
			string += decodeChar(charCode);
		}
		i += 2;
	}
	return string;
}

function NamingTable_storageOffset(thisOffset) {
	return readNumber(thisOffset + 4, 2, 1);
}

function NamingTable_nameRecordOffset(thisOffset, index) {
	return thisOffset + 6 + 12 * index;
}

function TableRecord_offset(thisOffset) {
	return readNumber(thisOffset + 8, 4, 1);
}

function TTF_numTables(thisOffset) {
	return readNumber(thisOffset + 4, 2, 1);
}

function TTF_tableRecords_offset(thisOffset, index) {
	return thisOffset + 12 + 16 * index;
}

function TTF_tableRecords_indexBy_tag(thisOffset, tag) {
	let numTables = TTF_numTables(thisOffset);
	let i = 0;
	while (i < numTables) {
		let offset = TTF_tableRecords_offset(thisOffset, i);
		let string = readUTF8StringNoSize(offset, 4, 1);
		if (string == tag)
			return i;
		i += 1;
	}
	return -1;
}

function TTF_NamingTable_getOffset(thisOffset) {
	let index = TTF_tableRecords_indexBy_tag(thisOffset, "name");
	if (index == -1)
		return 0;
	let offset = TTF_tableRecords_offset(thisOffset, index);
	return thisOffset + TableRecord_offset(offset);
}
