#include "TSZip.h"

#include <stdio.h>
#include <string.h>
#include "zlib.h"

#include "TSString.h"

namespace TSun{
	Zip::Zip(TVOID) : m_FileNum(0), m_TempBuf(0), m_TempSize(0)
	{
	}

	Zip::~Zip(TVOID)
	{
		ClearBuffer();
	}

	TVOID Zip::ClearBuffer()
	{
		if(m_TempSize>0)
		{
			getBlockMemAllocator()->freeMem(m_TempBuf, __FILE__, __LINE__);
			m_TempBuf=0;
		}
		m_TempSize=0;
		m_FileNum=0;
	}

	TS32 Zip::def(TUByte *src, TU32 srcSize, TUByte *dest, TU32 *destSize, TS32 level)
	{
		if(!src || !dest || !destSize)
			return Z_ERRNO;
		z_stream stream;
		TS32 err;
		stream.zalloc=Z_NULL;
		stream.zfree=Z_NULL;
		stream.opaque=Z_NULL;
		err=deflateInit(&stream,level);
		if(err!=Z_OK)
		{
			return err;
		}
		stream.avail_in = srcSize;
		stream.avail_out = srcSize;
		stream.next_in = (TUByte*)src;
		stream.next_out = (TUByte*)dest;
		err=deflate(&stream,Z_FINISH);
		if(err<0)
		{
			deflateEnd(&stream);
			return err;
		}
		(*destSize)=srcSize-stream.avail_out;
		deflateEnd(&stream);
		return Z_OK;
	}

	TS32 Zip::inf(TUByte* src, TU32 srcSize, TUByte* dest, TU32 origSize)
	{
		if(!src || !dest)
			return Z_ERRNO;
		z_stream stream;
		TS32 err;
		stream.zalloc=Z_NULL;
		stream.zfree=Z_NULL;
		stream.opaque=Z_NULL;
		err=inflateInit(&stream);
		if(err!=Z_OK)
		{
			return err;
		}
		stream.avail_in = srcSize;
		stream.avail_out = origSize;
		stream.next_in = (TUByte*)src;
		stream.next_out = (TUByte*)dest;
		err=inflate(&stream,Z_NO_FLUSH);
		if(err<0)
		{
			inflateEnd(&stream);
			return err;
		}
		inflateEnd(&stream);
		return Z_OK;
	}

	TS32 Zip::AddCompressFile(TCHAR* filename)
	{
		if(!filename)
			return 0;
		TCHAR szNoPath[1024] = {0};
		TSun::String::CutFilePath(filename,szNoPath);
		// 文件名部分
		TU32 namelen = (TU32)strlen(szNoPath);
		TSIZE lastsize = m_TempSize;
		if(lastsize==0)
		{
			m_TempBuf = (TUByte*)getBlockMemAllocator()->allocateMem(sizeof(TU32) * 2 + namelen, __FILE__, __LINE__);
			memset(m_TempBuf,0,sizeof(TU32)*2+namelen);
			m_FileNum=1;
			(*((TU32*)m_TempBuf))=m_FileNum;
			m_TempSize+=sizeof(TU32);
		}
		else
		{
			TUByte* newBuf = (TUByte*)getBlockMemAllocator()->allocateMem(m_TempSize + sizeof(TU32) + namelen, __FILE__, __LINE__);
			memcpy(newBuf, m_TempBuf, m_TempSize);
			getBlockMemAllocator()->freeMem(m_TempBuf, __FILE__, __LINE__);
			m_TempBuf = newBuf;
			//m_TempBuf = (TUByte*)realloc(m_TempBuf,m_TempSize+sizeof(TU32)+namelen);
			memset(m_TempBuf+lastsize,0,sizeof(TU32)+namelen);
			m_FileNum+=1;
			(*((TU32*)m_TempBuf))=m_FileNum;
		}
		memcpy(m_TempBuf+m_TempSize,&namelen,sizeof(TU32));
		m_TempSize+=sizeof(TU32);
		memcpy(m_TempBuf+m_TempSize,szNoPath,namelen);
		m_TempSize+=namelen;
		// 文件内容部分
		FILE* file = 0;
		TS32 re = fopen_s(&file,filename,"r");
		if(re!=0)
		{
			return 0;
		}
		TSIZE readByte = 0;
		TUByte filebuf[ZIP_READ_FILE_LENGTH] ={0};
		TUByte* tmpbuf = 0;
		TSIZE tmpsize = 0;
		do
		{
			readByte=(TSIZE)fread(filebuf,1,ZIP_READ_FILE_LENGTH,file);
			if(ferror(file))
			{
				fclose(file);
				if (tmpbuf)
				{
					getBlockMemAllocator()->freeMem(tmpbuf, __FILE__, __LINE__);
				}
				return 0;
			}
			TSIZE lasttmpsize = tmpsize;
			if(lasttmpsize==0)
			{
				tmpbuf=(TUByte*)getBlockMemAllocator()->allocateMem(readByte, __FILE__, __LINE__);
				memset(tmpbuf,0,readByte);
				tmpsize+=readByte;
			}
			else
			{
				TUByte* newBuf = (TUByte*)getBlockMemAllocator()->allocateMem(tmpsize + readByte, __FILE__, __LINE__);
				memcpy(newBuf, tmpbuf, tmpsize);
				getBlockMemAllocator()->freeMem(tmpbuf, __FILE__, __LINE__);
				tmpbuf = newBuf;
				//tmpbuf=(TUByte*)realloc(tmpbuf,tmpsize+readByte);
				memset(tmpbuf+lasttmpsize,0,readByte);
				tmpsize+=readByte;
			}
			memcpy(tmpbuf+lasttmpsize,filebuf,readByte);
			memset(filebuf,0,ZIP_READ_FILE_LENGTH);
			if(feof(file))
				break;
		}while(TTRUE);

		TUByte* newBuf = (TUByte*)getBlockMemAllocator()->allocateMem(m_TempSize + sizeof(TU64) + tmpsize, __FILE__, __LINE__);
		memcpy(newBuf, m_TempBuf, m_TempSize);
		getBlockMemAllocator()->freeMem(m_TempBuf, __FILE__, __LINE__);
		m_TempBuf = newBuf;

		//m_TempBuf = (TUByte*)realloc(m_TempBuf,m_TempSize+sizeof(TU64)+tmpsize);
		memset(m_TempBuf+m_TempSize,0,sizeof(TU64)+tmpsize);
		// 注意写到文件中的SIZE要强制变成TU64
		TU64 tmpsizeForWrite = (TU64)tmpsize;
		memcpy(m_TempBuf+m_TempSize,&tmpsizeForWrite,sizeof(TU64));
		m_TempSize+=sizeof(TU64);
		memcpy(m_TempBuf+m_TempSize,tmpbuf,tmpsize);
		m_TempSize+=tmpsize;
		fclose(file);
		getBlockMemAllocator()->freeMem(tmpbuf, __FILE__, __LINE__);
		return 1;
	}

	TS32 Zip::CompressToFile(TCHAR* zipname)
	{
		if(!zipname)
			return 0;
		if(m_TempSize<=0)
			return 0;
		FILE* output = 0;
		TS32 re = fopen_s(&output,zipname,"w");
		if(re!=0)
			return 0;
		TUByte* outbuf = T_NEW_ARRAY(getBlockMemAllocator(), TUByte, m_TempSize);
		TU32 outsize = 0;
		if(def(m_TempBuf,(TU32)m_TempSize,outbuf,&outsize,Z_DEFAULT_COMPRESSION)!=Z_OK)
		{
			fclose(output);
			T_DELETE_ARRAY(getBlockMemAllocator(), TUByte, outbuf);
			return 0;
		}
		TU64 tempSizeForWrite = (TU64)m_TempSize;
		fwrite(&tempSizeForWrite,sizeof(TU64),1,output);
		if(ferror(output))
		{
			fclose(output);
			T_DELETE_ARRAY(getBlockMemAllocator(), TUByte, outbuf);
			return 0;
		}
		fwrite(outbuf,1,outsize,output);
		if(ferror(output))
		{
			fclose(output);
			T_DELETE_ARRAY(getBlockMemAllocator(), TUByte, outbuf);
			return 0;
		}
		T_DELETE_ARRAY(getBlockMemAllocator(), TUByte, outbuf);
		fclose(output);
		return 1;
	}

	TU64 Zip::DecompressToMem(TCHAR* zipname, TUByte** buf)
	{
		if(!zipname)
			return 0;
		// 读ZIP文件
		FILE* file = 0;
		TS32 re = fopen_s(&file,zipname,"r");
		if(re!=0)
			return 0;
		TSIZE readByte = 0;
		TUByte filebuf[ZIP_READ_FILE_LENGTH] ={0};
		TUByte* tmpbuf = 0;
		TSIZE tmpsize = 0;
		do
		{
			readByte=(TSIZE)fread(filebuf,1,ZIP_READ_FILE_LENGTH,file);
			if(ferror(file))
			{
				fclose(file);
				if (tmpbuf)
				{
					getBlockMemAllocator()->freeMem(tmpbuf, __FILE__, __LINE__);
				}
				return 0;
			}
			TSIZE lasttmpsize = tmpsize;
			if(lasttmpsize==0)
			{
				tmpbuf = (TUByte*)getBlockMemAllocator()->allocateMem(readByte, __FILE__, __LINE__);
				memset(tmpbuf,0,readByte);
				tmpsize+=readByte;
			}
			else
			{
				TUByte* newBuf = (TUByte*)getBlockMemAllocator()->allocateMem(tmpsize + readByte, __FILE__, __LINE__);
				memcpy(newBuf, tmpbuf, tmpsize);
				getBlockMemAllocator()->freeMem(tmpbuf, __FILE__, __LINE__);
				tmpbuf = newBuf;

				//tmpbuf=(TUByte*)realloc(tmpbuf,tmpsize+readByte);
				memset(tmpbuf+lasttmpsize,0,readByte);
				tmpsize+=readByte;
			}
			memcpy(tmpbuf+lasttmpsize,filebuf,readByte);
			memset(filebuf,0,ZIP_READ_FILE_LENGTH);
			if(feof(file))
				break;
		}while(TTRUE);
		fclose(file);
		// 读原尺寸
		TU64 origsize = *((TU64*)tmpbuf);
		if (origsize <= 0)
		{
			getBlockMemAllocator()->freeMem(tmpbuf, __FILE__, __LINE__);
			return 0;
		}
		// 解压缩
		TUByte* out = T_NEW_ARRAY(getBlockMemAllocator(), TUByte, origsize);
		if(inf(tmpbuf+sizeof(TU64),(TU32)(tmpsize-sizeof(TU64)),out,(TU32)origsize)!=Z_OK)
		{
			T_DELETE_ARRAY(getBlockMemAllocator(), TUByte, out);
			getBlockMemAllocator()->freeMem(tmpbuf, __FILE__, __LINE__);
			return 0;
		}
		if(buf)
		{
			(*buf) = (TUByte*)getBlockMemAllocator()->allocateMem(origsize, __FILE__, __LINE__);
			memcpy(*buf,out,origsize);
		}
		//else
		{
			ClearBuffer();
			m_TempBuf = (TUByte*)getBlockMemAllocator()->allocateMem(origsize, __FILE__, __LINE__);
			memcpy(m_TempBuf,out,origsize);
			m_TempSize = (TSIZE)origsize;
		}
		T_DELETE_ARRAY(getBlockMemAllocator(), TUByte, out);
		getBlockMemAllocator()->freeMem(tmpbuf, __FILE__, __LINE__);
		return origsize;
	}

	TS32 Zip::CopyMemToFile(TCHAR* dirname, TBOOL bBuildFile, List<MemFile*>& memFileList)
	{
		if(!dirname || m_TempSize<=0)
			return 0;
		TSIZE offset=0;
		// 文件数量
		m_FileNum = (TU32)(*m_TempBuf);
		offset+=sizeof(TU32);
		// 循环生成文件
		for(TU32 i=0;i<m_FileNum;i++)
		{
			MemFile* pMemFile = T_NEW(getStructMemAllocator(), MemFile);
			TU32 fileNameLen = 0;
			TCHAR szName[1024]={0};
			TCHAR szFull[1024]={0};
			TUByte* pContent=0;
			TSIZE conSize=0;

			// 文件名部分
			fileNameLen =(TU32)(*(m_TempBuf+offset));
			offset+=sizeof(TU32);
			memcpy(szName,m_TempBuf+offset, fileNameLen);
			sprintf_s(szFull,1024,"%s%s",dirname,szName);
			offset+= fileNameLen;
			// 文件内容
			TU64 fileSize=(TU64)(*(m_TempBuf+offset));
			offset+=sizeof(TU64);
			pContent=T_NEW_ARRAY(getBlockMemAllocator(), TUByte, fileSize);
			memcpy(pContent,m_TempBuf+offset, (TSIZE)fileSize);
			conSize= (TSIZE)fileSize;
			offset+= (TSIZE)fileSize;

			// 生成
			if(bBuildFile)
			{
				FILE* outfile = 0;
				TS32 re = fopen_s(&outfile,szFull,"w");
				if(re==0)
				{
					fwrite(pContent,1,conSize,outfile);
					if(ferror(outfile))
					{
						fclose(outfile);
						T_DELETE_ARRAY(getBlockMemAllocator(), TUByte, pContent);
						return 0;
					}
					fclose(outfile);
				}
			}
			if(pMemFile)
			{
				pMemFile->SetFullName(szFull);
				pMemFile->SetContent(pContent,conSize);
			}
			memFileList.push_back(pMemFile);

			T_DELETE_ARRAY(getBlockMemAllocator(), TUByte, pContent);
		}
		return 1;
	}

}