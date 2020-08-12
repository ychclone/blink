#ifndef CUITLS_H
#define CUITLS_H

#include <QDir>
#include <QDirIterator>

#include "commonType.h"

class CUtils
{
public:
	CUtils();
	~CUtils();

	// recursively remove a directory
	static bool removeDirectory(QDir &aDir);

	static void bufLog(unsigned char* buffer, long size);
	static QString bufToQString(unsigned char* buffer, long size);

	// for convenient use
	static void encode(BYTE *buffer, long& bufLen);
    static void decode(BYTE *buffer, long& bufLen);

	// max allowed bit for each long here is 30, as using 2 bit for header
	// note: big endian is expected, be careful of little endian
	static bool encodeDWORDSeq(BYTE *inBuf, long inBufLen, BYTE* outBuf, long& outBufLen);
	static bool decodeDWORDSeq(BYTE *inBuf, long inBufLen, BYTE* outBuf, long& outBufLen);

	static void putByte(BYTE* buffer, BYTE in);
	static void putWord(BYTE* buffer, WORD in);
	static void putDWord(BYTE* buffer, DWORD in);

	static BYTE getByte(BYTE* buffer);
	static WORD getWord(BYTE* buffer);
	static DWORD getDWord(BYTE* buffer);
	
private:

	// for encode
	static const DWORD ENCODE_USE_1_BYTE = 0x7F;        // 01111111
	static const DWORD ENCODE_USE_2_BYTE = 0x3FFF;      // 00111111 11111111
	static const DWORD ENCODE_USE_4_BYTE = 0x3FFFFFFF;  // 00111111 11111111 11111111 11111111

	static const BYTE MASK_HEADER_BYTE = 0x00;         // 00 i.e. 00000000
	static const WORD MASK_HEADER_WORD = 0x8000;       // 10 i.e. 10000000 00000000
	static const DWORD MASK_HEADER_DWORD = 0xC0000000; // 11 i.e. 11000000 00000000 00000000 00000000

	// for decode
	static const BYTE HEADER_BIT_1_MASK = 0x80; // 10000000
	static const BYTE HEADER_BIT_2_MASK = 0x40; // 01000000

	static const BYTE MASK_WITHOUT_HEADER_BYTE  = 0x7F;       // 01111111 
	static const WORD MASK_WITHOUT_HEADER_WORD  = 0x3FFF;     // 00111111 11111111
	static const DWORD MASK_WITHOUT_HEADER_DWORD = 0x3FFFFFFF; // 00111111 11111111 11111111 11111111

	static const long ENCODE_DECODE_BUFFER_LEN = 5000;
	
	
};


#endif
