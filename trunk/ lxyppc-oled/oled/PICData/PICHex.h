#pragma once

#include "HexFileLine.h"
//Define the ROM
const	int		ROMStart=0;
const	int		MaxROM=0x2000;
const	WORD	ROMMask=0x3FFF;

//define the EEPROM
const	int		EEStart=0x2100;
const	int		MaxEE=0x100;
const	UCHAR	EEMask=0xFF;

//Define the ID bytes
const	int		IDStart=0x2000;
const	int		IDSize=4;

//define  the Config bytes
const	int		ConfigStart=0x2007;
const	int		ConfigSize=1;

//define the OSCCAL data
const	int		OSCCALStart=0x0;
const	int		OSCCALSize=1;

//define the Device ID
const	int		DevIDStart=0x2006;
const	int		DevIDSize=1;

class	AddrRange
{
public:
	AddrRange()
	:Low(0)
	,High(0)
	{
	};
	AddrRange(int low)
		:Low(low)
		,High(low)
	{};
	AddrRange(int low,int high)
		:Low(low)
		,High(high)
	{};
	AddrRange(const AddrRange& tp)
	{
		this->High = tp.High;
		this->Low= tp.Low;
	};
AddrRange&   operator= (const AddrRange& tp)
	{
		this->High = tp.High;
		this->Low = tp.Low;
		return *this;
	};
//闭区间判断是否在low与high之间
bool	IsIn(int addr)
	{
		if((Low<=addr) && (addr<=High))
			return true;
		else
			return false;
	};
//闭区间判断是否在low+OffSet与high+OffSet之间
bool	IsIn(int addr,int OffSet)
	{
		if(((Low+OffSet)<=addr) && (addr<=(High+OffSet)))
			return true;
		else
			return false;
	};
CString		FormatRange()
	{
		CString	ts;
		ts.Format(_T("Rang: 0x%04X   0x%04X\r\n"),Low,High);
		return ts;
	};
size_t	GetSize() const { return High - Low + 1;}
	~AddrRange()
	{
	};
	int		Low;
	int		High;
};
//在这里定义的Range为数据的绝对地址，不带任何的偏移量
class	DeviceInfo
{
public:
			DeviceInfo();
			DeviceInfo(CString DeviceName,int ROMSize,int  EESize,int OSCCALAddr,int configSize=ConfigSize,int idSize=IDSize);
			DeviceInfo(const DeviceInfo& tp);
			DeviceInfo(CString DevName,int romSize,int eeSize);
			DeviceInfo(CString	DeviceName,AddrRange ROMRange,AddrRange EERange,
				AddrRange ConfigRange,AddrRange OSCCALRange,AddrRange IDRange, AddrRange DevIDRange);
DeviceInfo&	operator=(const DeviceInfo& tp);
			~DeviceInfo()
			{};
bool		IsROM(int addr)     { return ROM.IsIn(addr);    }
bool		IsEE(int addr)      { return EE.IsIn(addr);     }
bool		IsConfig(int addr)  { return Config.IsIn(addr); }
bool		IsID(int addr)      { return ID.IsIn(addr);     }
bool		IsOSCCAL(int addr)  { return OSCCAL.IsIn(addr); }
bool        IsDevID(int addr)   { return DevID.IsIn(addr);  }
CString		FormatInfo();
	CString		Name;
	AddrRange	ROM;
	AddrRange	EE;
	AddrRange	Config;
	AddrRange	ID;
	AddrRange	OSCCAL;
    AddrRange   DevID;

};

template  <class   Type>
class	PICData
{
public:
	PICData(void)
		:OffSet(0)
		,Name("PIC Data")
		,Range(0,0)
		,Data(NULL)
	{
	};
	//给出绝对位置，计算出OffSet和Range
	PICData(CString name,AddrRange  rg,Type MASK=Type(-1))
		:Name(name)
		,Range(0,rg.High-rg.Low)
		,OffSet(rg.Low)
		,Data(NULL)
	{
		Data = new Type[rg.GetSize()];
		FillData(MASK);
	}
void	SetData(CString name,AddrRange  rg,const CHexFile &hFile,Type MASK=Type(-1))
	{
		Name=name;
		AddrRange  trg(0,rg.High-rg.Low);
		Range=trg;
		OffSet=rg.Low;
		delete [] Data;
		Data = NULL;
		Data = new Type[rg.GetSize()];
		FillData(MASK);

		for(int i=0;i<hFile.LineCount;i++)
		{
			ImportLine(hFile.Lines[i],MASK);
		}
	};
void	SetData(CString name,AddrRange  rg,Type MASK=Type(-1))
	{
		Name=name;
		AddrRange  trg(0,rg.High-rg.Low);
		Range=trg;
		OffSet=rg.Low;
		delete [] Data;
		Data = NULL;
		Data = new Type[rg.GetSize()];
		FillData(MASK);
	};
	~PICData(void)
	{
		delete [] Data;
		Data = NULL;
	}
	//设定每行显示的数据个数
CString		FormatData(int	DataPerLine=8)
{	
	CString		ss,ts,as,fs1,fs2;
	fs1.Format(_T("\t%%0%dX "),sizeof(Type)*2);
	fs2.Format(_T("%%0%dX "),sizeof(Type)*2);
	ts.Format(_T("%s: Range( 0x%04X  0x%04X )\r\n"),Name,Range.Low+OffSet,Range.High+OffSet);
	for(int i=Range.Low;i<=Range.High / DataPerLine;i++)
	{
		ss.Format(_T("0x%04X: "),i*DataPerLine+OffSet);
		for(int j=0;(j<DataPerLine)&&(i*DataPerLine+j<=Range.High);j++)
		{
			if(j==8)
				as.Format(fs1,Data[i*DataPerLine+j]);
			else
				as.Format(fs2,Data[i*DataPerLine+j]);
			ss+=as;
		}
		ss+=_T("\r\n");
		ts=ts+ss;
	}
	return ts;
}
    void GetData(const void *pData,int iDataWide = 2, int iStartBits = 1, int iStopBits =1)
    {
        const unsigned char *pBuf = (unsigned char*)pData;
        DWORD   mask = (1<<(8*iDataWide))-1 ;
        while(iStopBits--)
        {
            DWORD tmp = DWORD(1<<((iDataWide)*8-iStopBits-1));
            mask -= tmp;
        }
        mask>>=iStartBits;
	    for(DWORD i = 0; i< Range.GetSize(); i++)
	    {
            DWORD   temp = pBuf[i*iDataWide];
            for(int j=1;j<iDataWide;j++)
            {
                temp += (DWORD)pBuf[i*iDataWide+j]<<(8*j);
            }
		    //DWORD	temp = pBuf[i*2]+((Type)pBuf[i*2+1]<<8);
		    //temp <<= iStopBits;
            //Data[i] &= (Type)(temp>>iStartBits);
		    Data[i] = (Type)((temp>>(iStartBits)) & mask);
	    }
    }

    void    SetData(void *pData,int iDataWide = 2, int iStartBits = 1, int iStopBits =1) const
    {
        unsigned char *pBuf = (unsigned char*)pData;
	    for(DWORD i = 0; i< Range.GetSize() ; i++)
	    {
		    DWORD	temp = Data[i];
		    temp<<=iStartBits;
            for(int j=0; j<iDataWide; j++)
            {
                pBuf[i*2+j]= (unsigned char)((temp >> (8*j) )& 0xFF);
            }
	    }
    }
	//Type	Data[Size];
	Type	*Data;
	AddrRange	Range;
	CString		Name;
	int			OffSet;
	PICData<Type>& operator= (const PICData<Type>& rhs)
	{
		Range = rhs.Range;
		delete [] Data;
		Data = NULL;
		Data = new Type [Range.GetSize()];
		Name = rhs.Name;
		OffSet = rhs.OffSet;
		for(int i=Range.Low;i<=Range.High;i++)
			Data[i]=rhs.Data[i];

		return	*this;
	}
private:
	void FillData(Type  tp)
	{
		for(int i=Range.Low;i<=Range.High;i++)
			Data[i]=tp;
	}
// Sometimes the PICC will generate the wrong Hex code in the hex file
	void	ImportLine(const CHexFileLine  &line,Type MASK)
	{
	//if(Range.IsIn((line.RecordAddress/2),OffSet))
		for(int i=0;i<line.RecordNumber;i++)
				if(Range.IsIn(((line.RecordAddress+i)/2),OffSet))
					Data[(line.RecordAddress+i)/2-OffSet]=(Type)(line.Records[i++]+line.Records[i+1]*256) & MASK;
		if(OffSet==0x3FF)
			int i=0;
	}
};

class CPICHex
{
public:
	CPICHex(void);
	CPICHex(const CHexFile&,const DeviceInfo&);
	CPICHex(const DeviceInfo&);
	~CPICHex(void);
	//PICData<UCHAR,MaxEE>	DataEE;
	//PICData<WORD,MaxROM>	DataROM;
	//PICData<WORD,ConfigSize>	DataConfig;
	//PICData<WORD,IDSize>	DataID;
	//PICData<WORD,OSCCALSize>	DataOSCCAL;

	PICData<UCHAR>	DataEE;
	PICData<WORD>	DataROM;
	PICData<WORD>	DataConfig;
	PICData<WORD>	DataID;
	PICData<WORD>	DataOSCCAL;
    PICData<WORD>   DataDevID;
	DeviceInfo	Device;
	CString	FormatDevice(){return Device.FormatInfo();}
	LONG    SetDevice(const DeviceInfo& Dev);
	LONG    LoadFile(const CHexFile&);
    LONG    FormatData(CString &str);
    DWORD   GetEESize()    const { return (DWORD)DataEE.Range.GetSize();    }
    DWORD   GetROMSize ()  const { return (DWORD)DataROM.Range.GetSize();   }
    DWORD   GetIDSize()    const { return (DWORD)DataID.Range.GetSize();    }
    DWORD   GetConfigSize()const { return (DWORD)DataConfig.Range.GetSize();}
    DWORD   GetDevIDSize()const { return (DWORD)DataDevID.Range.GetSize();  }
private:
};


