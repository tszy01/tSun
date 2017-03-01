#ifndef _TSZIP_H_
#define _TSZIP_H_
#include "TSMemFile.h"
#include "TSList.h"

namespace TSun{

	class Zip
	{
	public:
		Zip(TVOID);
		~Zip(TVOID);
	public:
		// --------- 以下为静态方法 --------------

		// 压缩
		static TS32 def(TUByte* src, TU32 srcSize, TUByte* dest, TU32* destSize,TS32 level);
		// 解压缩
		static TS32 inf(TUByte* src, TU32 srcSize, TUByte* dest, TU32 origSize);
		// 添加待压缩文件到内存
		TS32 AddCompressFile(TCHAR* filename);
		// 生成压缩文件
		TS32 CompressToFile(TCHAR* zipname);
		// 解压缩文件到内存
		// note, use getBlockMemAllocator to release buf
		TU64 DecompressToMem(TCHAR* zipname, TUByte** buf);
		// 分割解压缩得文件
		TS32 CopyMemToFile(TCHAR* dirname,TBOOL bBuildFile,List<MemFile*>& memFileList);
		// 清除数据
		TVOID ClearBuffer();

		// ---------------------------------------
	protected:
		// -------- 以下为静态成员 ---------------
		// 缓冲区
		TUByte* m_TempBuf;
		// 缓冲区大小
		TSIZE m_TempSize;
		// 文件数量
		TU32 m_FileNum;
		// ---------------------------------------

		static const TU32 ZIP_READ_FILE_LENGTH = 16384;
	};

}

#endif