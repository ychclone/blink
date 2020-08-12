#include "CUtils.h"

#include <QDebug>

CUtils::CUtils()
{

}

CUtils::~CUtils()
{

}

// from John Suykerbuyk in lists.trolltech.com
bool CUtils::removeDirectory(QDir &aDir)
{
	bool has_err = false;
	if (aDir.exists())
	{
		QFileInfoList entries = aDir.entryInfoList(QDir::NoDotAndDotDot | 
				QDir::Dirs | QDir::Files);
		int count = entries.size();
		for (int idx = 0; ((idx < count) && (!has_err)); idx++)
		{
			QFileInfo entryInfo = entries[idx];
			QString path = entryInfo.absoluteFilePath();
			if (entryInfo.isDir())
			{
				QDir dirToRemove(path);
				has_err = CUtils::removeDirectory(dirToRemove);
			}
			else
			{
				QFile file(path);
				if (!file.remove())
					has_err = true;
			}
		}
		if (!aDir.rmdir(aDir.absolutePath()))
			has_err = true;
	}
	return(has_err);
}

void CUtils::bufLog(unsigned char* buffer, long size)
{
	long i;

	printf("size = %ld\n", size);
	for (i = 0; i < size; i++) {
		printf("%02X ", buffer[i]);
		if ((i != 0) && ((i+1) % 25 == 0)) {
			printf("\n");
		}
	}
	printf("\n");
}

QString CUtils::bufToQString(unsigned char* buffer, long size)
{
	long i;
	QString result = QString("");
	QString tmpStr;

	for (i = 0; i < size; i++) {
		tmpStr= QString("%1 ").arg(buffer[i], 2, 16, QLatin1Char('0')); // hex string
		result += tmpStr;
	}

	return result;
}


void CUtils::putByte(BYTE* buffer, BYTE in)
{
	buffer[0] = in;
}

void CUtils::putWord(BYTE* buffer, WORD in)
{
	buffer[0] = static_cast <BYTE> (in >> 8);
	buffer[1] = static_cast <BYTE> (in); 
}

void CUtils::putDWord(BYTE* buffer, DWORD in)
{
	buffer[0] = static_cast <BYTE> (in >> 24);
	buffer[1] = static_cast <BYTE> (in >> 16); 
	buffer[2] = static_cast <BYTE> (in >> 8);
	buffer[3] = static_cast <BYTE> (in); 
}

BYTE CUtils::getByte(BYTE* buffer)
{
	BYTE out;
	out = static_cast <BYTE> (buffer[0]);
	return out;	
}

WORD CUtils::getWord(BYTE* buffer)
{
	WORD out;
	out = (static_cast <WORD> (buffer[0]) << 8) + (static_cast <WORD> (buffer[1]));
	return out;
}

DWORD CUtils::getDWord(BYTE* buffer)
{
	DWORD out;
	out = (static_cast <DWORD> (buffer[0]) << 24) + (static_cast <DWORD> (buffer[1]) << 16) + 
						(static_cast <DWORD> (buffer[2]) << 8) + (static_cast <DWORD> (buffer[3])); 
	return out;
}

void CUtils::encode(BYTE *buffer, long& bufLen)
{
	long updatedBufLen; 
	static BYTE encodeDecodeBuf[ENCODE_DECODE_BUFFER_LEN]; 

	encodeDWORDSeq((BYTE*) buffer, bufLen, encodeDecodeBuf, updatedBufLen);
	
	// copy back to buffer
	memset(buffer, 0, bufLen);
	memcpy(buffer, encodeDecodeBuf, updatedBufLen);

	// update buffer length
	bufLen = updatedBufLen;
}

void CUtils::decode(BYTE *buffer, long& bufLen)
{
	long updatedBufLen; 
	static BYTE encodeDecodeBuf[ENCODE_DECODE_BUFFER_LEN];  

	decodeDWORDSeq((BYTE*) buffer, bufLen, encodeDecodeBuf, updatedBufLen);
	
	// copy back to buffer
	memset(buffer, 0, bufLen);
	memcpy(buffer, encodeDecodeBuf, updatedBufLen);

	// update buffer length
	bufLen = updatedBufLen;
}

bool CUtils::encodeDWORDSeq(BYTE *inBuf, long inBufLen, BYTE* outBuf, long& outBufLen)
{
	DWORD dwordExtracted;
	long inIdx = 0;
	long outIdx = 0;
	long dwordSize = sizeof(DWORD);

	BYTE encodedByte;
	WORD encodedWord;
	DWORD encodedDWord;

	if (inBufLen % dwordSize != 0) {
       return false;
	}

	outBufLen = 0;
	
	do {
		dwordExtracted = getDWord(&(inBuf[inIdx]));

		if (dwordExtracted <= ENCODE_USE_1_BYTE) {
			encodedByte = static_cast <BYTE> (dwordExtracted);
			encodedByte |= MASK_HEADER_BYTE;
			putByte(&(outBuf[outIdx]), encodedByte);
			outIdx += 1;
			outBufLen += 1;
		} else if (dwordExtracted <= ENCODE_USE_2_BYTE) {
			encodedWord = static_cast <WORD> (dwordExtracted);
			encodedWord |= MASK_HEADER_WORD;
			putWord(&(outBuf[outIdx]), encodedWord);
			outIdx += 2;
			outBufLen += 2;
		} else if (dwordExtracted <= ENCODE_USE_4_BYTE) {
            encodedDWord = static_cast <DWORD> (dwordExtracted); 
			encodedDWord |= MASK_HEADER_DWORD;
			putDWord(&(outBuf[outIdx]), encodedDWord);
			outIdx += 4;
			outBufLen += 4;
		}
		inIdx += dwordSize;

	} while (inIdx < inBufLen);
	return true;
}

bool CUtils::decodeDWORDSeq(BYTE *inBuf, long inBufLen, BYTE* outBuf, long& outBufLen)
{
	long inIdx = 0;
	long outIdx = 0;

	BYTE byteExtracted;
	
	BYTE decodedByte;
	WORD decodedWord;
	DWORD decodedDWord;

	outBufLen = 0;

	do {
		byteExtracted = getByte(&(inBuf[inIdx]));

		if ((byteExtracted & HEADER_BIT_1_MASK) == 0) { // 1 byte length
			decodedByte = getByte(&(inBuf[inIdx])) & MASK_WITHOUT_HEADER_BYTE;
			putDWord(&(outBuf[outIdx]), static_cast <DWORD> (decodedByte));
			inIdx += 1;
		} else if ((byteExtracted & HEADER_BIT_2_MASK) == 0) { // 2 byte length
			decodedWord = getWord(&(inBuf[inIdx])) & MASK_WITHOUT_HEADER_WORD;
			putDWord(&(outBuf[outIdx]), static_cast <DWORD> (decodedWord));
			inIdx += 2;
		} else { // 4 byte length
			decodedDWord = getDWord(&(inBuf[inIdx])) & MASK_WITHOUT_HEADER_DWORD;
			putDWord(&(outBuf[outIdx]), static_cast <DWORD> (decodedDWord));
			inIdx += 4;
		}		
		outIdx += 4; 
		outBufLen += 4;

	} while (inIdx < inBufLen);

	return true;
    

}



