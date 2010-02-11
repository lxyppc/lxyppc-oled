#include "StdAfx.h"
#include ".\hexfileline.h"
CHexFileLine::CHexFileLine()
:RecordNumber(0)
,RecordAddress(0)
,RecordType(0)
,SumAll(0xFF)
{
	for(int i=0;i<LineMaxNum;i++)
		Records[i]=0;
}

CHexFileLine::~CHexFileLine(void)
{
}
CHexFileLine::CHexFileLine(const CHexFileLine& tp)
{
	this->RecordAddress =tp.RecordAddress;
	this->RecordNumber =tp.RecordNumber ;
	this->RecordType =tp.RecordType ;
	this->SumAll =tp.SumAll ;
	for(int i=0;i<LineMaxNum;i++)
	this->Records [i]=tp.Records [i];
}

CHexFileLine&	CHexFileLine::operator=(const CHexFileLine& tp)
{
	if(this!=&tp)
	{
	this->RecordAddress =tp.RecordAddress;
	this->RecordNumber =tp.RecordNumber ;
	this->RecordType =tp.RecordType ;
	this->SumAll =tp.SumAll ;
	for(int i=0;i<LineMaxNum;i++)
	this->Records [i]=tp.Records [i];
	}
	return *this;
}

CHexFile::CHexFile(const CString fname)
:LineCount(0)
,Lines(NULL)
{
	FILE	*fp=_tfopen(fname,_T("r"));
	char	ts[50];
	if(!fp)
	{
		//AfxMessageBox("unable to open file");
	}
	else
	{
		//第一次循环，获得文件的总行数
		size_t i = 0;
		while(!feof(fp))
		{
			char ch;
			//for(size_t i=0;i<100;){
				fread(&ch,1,1,fp);
				if((i==0) && (ch!=':')){
					LineCount = 0;
					return;
				}
				if(i>50){
					LineCount = 0;
					return;
				}
				if(ch=='\n'){
					LineCount++;
					i=0;
				}
				i++;
			//}
			//fscanf(fp,"%s\n",ts);
			//LineCount++;

		}
		if(LineCount){
			Lines=new CHexFileLine[LineCount];
			int  LineIndex=0;
			rewind(fp);
			while(!feof(fp))
			{
				fscanf(fp,"%s\n",ts);
				GetOneLine(ts,Lines[LineIndex]);
				LineIndex++;

			}
			fclose(fp);
		}
		//sprintf(ts,"%d",LineIndex);
		//AfxMessageBox(ts);

	}
}
CHexFile::~CHexFile(void)
{
	if(Lines){
		delete [] Lines;
	}
}
void	CHexFile::GetLineHead(char* fp,CHexFileLine& HexLine)
{
	int a=0,b=0,c=0;
	sscanf(fp,":%2X%4X%2X%s\n",&a,&b,&c,fp);
	HexLine.RecordNumber = a;
	HexLine.RecordAddress = b;
	HexLine.RecordType = c;
	//sscanf(fp,":%2X%4X%2X",&HexLine.RecordNumber,&HexLine.RecordAddress,&HexLine.RecordType);
}
void	CHexFile::GetOneLine(char* fp,CHexFileLine& HexLine)
{
	GetLineHead(fp,HexLine);
	int a=0;
	for(int i=0;i<HexLine.RecordNumber;i++)
	{
		sscanf(fp,"%2X%s\n",&a,fp);
		//sscanf(fp,"%2X",&HexLine.SumAll);
		HexLine.Records[i]=a;
	}
	sscanf(fp,"%2X",&a);
	HexLine.SumAll = a;
}
CString		CHexFile::FormatLine(int LineIndex)
{	
	CString	ts,ss;
	ts.Format(_T("Error Line !"));
	if(LineIndex>(LineCount-1))
		return ts;
	else
	{
		ts.Format(_T(":%02X%04X%02X"),
            Lines[LineIndex].RecordNumber,
            Lines[LineIndex].RecordAddress ,
            Lines[LineIndex].RecordType);
		for(int i=0;i<Lines[LineIndex].RecordNumber;i++)
		{
			ss.Format(_T("%02X"),Lines[LineIndex].Records[i]);
			ts+=ss;
		}
		ss.Format(_T("%02X\r\n"),Lines[LineIndex].SumAll);
		ts+=ss;
		if(LineIndex==129)
		{
			int j=0;
		}
		return ts;
	}
}
