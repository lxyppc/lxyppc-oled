#include "StdAfx.h"
#include ".\pichex.h"



DeviceInfo::DeviceInfo()
		:ROM(ROMStart)
		,EE(EEStart)
		,Config(ConfigStart)
		,ID(IDStart,IDStart+IDSize-1)
		,OSCCAL(OSCCALStart)
		,Name("PIC Device")
        ,DevID(DevIDStart,DevIDStart+DevIDSize-1)
{}
DeviceInfo::DeviceInfo(CString DevName,int ROMSize,int  EESize,int OSCCALAddr,int configSize,int idSize)
		:ROM(ROMStart,ROMStart+ROMSize-1)
		,EE(EEStart,EEStart+EESize-1)
		,Config(ConfigStart,ConfigStart+configSize-1)
		,OSCCAL(OSCCALAddr,OSCCALAddr+OSCCALSize-1)
		,ID(IDStart,IDStart+idSize-1)
		,Name(DevName)
        ,DevID(DevIDStart,DevIDStart+DevIDSize-1)
{
}
DeviceInfo::DeviceInfo(CString	DevName,int romSize,int EESize)
		:ROM(ROMStart,ROMStart+romSize-1)
		,EE(EEStart,EEStart+EESize-1)
		,Config(ConfigStart,ConfigStart+ConfigSize-1)
		,OSCCAL(romSize-1,romSize+OSCCALSize-2)
		,ID(IDStart,IDStart+IDSize-1)
		,Name(DevName)
        ,DevID(DevIDStart,DevIDStart+DevIDSize-1)
{
}
DeviceInfo::DeviceInfo(CString	DevName,AddrRange ROMRange,AddrRange EERange,
				AddrRange ConfigRange,AddrRange OSCCALRange,AddrRange IDRange, AddrRange DevIDRange)
		:ROM(ROMRange)
		,EE(EERange)
		,Config(ConfigRange)
		,OSCCAL(OSCCALRange)
		,ID(IDRange)
		,Name(DevName)
        ,DevID(DevIDRange)
{
}
DeviceInfo::DeviceInfo(const DeviceInfo& tp)
{
		this->Config = tp.Config ;
		this->EE= tp.EE ;
		this->ID = tp.ID;
		this->OSCCAL = tp.OSCCAL;
		this->ROM = tp.ROM;
		this->Name=tp.Name;
        this->DevID = tp.DevID;
}
DeviceInfo&	DeviceInfo::operator=(const DeviceInfo& tp)
{
		this->Config = tp.Config ;
		this->EE= tp.EE ;
		this->ID = tp.ID;
		this->OSCCAL = tp.OSCCAL;
		this->ROM = tp.ROM;
		this->Name=tp.Name;
        this->DevID = tp.DevID;
		return *this;
}
CString		DeviceInfo::FormatInfo()
{
	CString		ts;
	ts.Format(_T("Device: %s\r\nROM: \t%sEEPROM: \t%sConfig: \t%sID Add:\t%sOSCCAL: \t%sDeviceID: \t%s"),Name,ROM.FormatRange(),
		EE.FormatRange(),Config.FormatRange(),ID.FormatRange(),OSCCAL.FormatRange(),DevID.FormatRange());
	return ts;
}



CPICHex::CPICHex(void)
{
}
CPICHex::CPICHex(const CHexFile&  hFile,const DeviceInfo &Dev)
:Device(Dev)
{
	DataEE.SetData(_T("EEPROM"),Device.EE,hFile);
	DataROM.SetData(_T("ROM"),Device.ROM ,hFile,ROMMask);
	DataConfig.SetData(_T("Config"),Device.Config,hFile);
	DataID.SetData(_T("User ID"),Device.ID,hFile);
	DataOSCCAL.SetData(_T("OSCCAL"),Device.OSCCAL,hFile,ROMMask);
    DataDevID.SetData(_T("Device ID"),Device.DevID, ROMMask);
}
CPICHex::CPICHex(const DeviceInfo &Dev)
:Device(Dev)
{
	DataEE.SetData(_T("EEPROM"),Device.EE);
	DataROM.SetData(_T("ROM"),Device.ROM ,ROMMask);
	DataConfig.SetData(_T("Config"),Device.Config,ROMMask);
	DataID.SetData(_T("User ID"),Device.ID,ROMMask);
	DataOSCCAL.SetData(_T("OSCCAL"),Device.OSCCAL,ROMMask);
    DataDevID.SetData(_T("Device ID"),Device.DevID, ROMMask);
}
CPICHex::~CPICHex(void)
{
}
LONG	CPICHex::SetDevice(const DeviceInfo& Dev)
{
	Device = Dev;
	DataEE.SetData(_T("EEPROM"),Device.EE);
	DataROM.SetData(_T("ROM"),Device.ROM ,ROMMask);
	DataConfig.SetData(_T("Config"),Device.Config,ROMMask);
	DataID.SetData(_T("User ID"),Device.ID,ROMMask);
	DataOSCCAL.SetData(_T("OSCCAL"),Device.OSCCAL,ROMMask);
    DataDevID.SetData(_T("Device ID"),Device.DevID, ROMMask);
	return 0L;
}
LONG	CPICHex::LoadFile(const CHexFile& hFile)
{
	DataEE.SetData(_T("EEPROM"),Device.EE,hFile);
	DataROM.SetData(_T("ROM"),Device.ROM ,hFile,ROMMask);
	DataConfig.SetData(_T("Config"),Device.Config,hFile,ROMMask);
	DataID.SetData(_T("User ID"),Device.ID,hFile,ROMMask);
	DataOSCCAL.SetData(_T("OSCCAL"),Device.OSCCAL,hFile,ROMMask);
    DataDevID.SetData(_T("Device ID"),Device.DevID, ROMMask);
	return 0L;
}
LONG    CPICHex::FormatData(CString &strTemp)
{
        strTemp+=DataConfig.FormatData();
		strTemp+=DataID.FormatData();
        strTemp+=DataDevID.FormatData();
		strTemp+=DataEE.FormatData(16);
		strTemp+=DataROM.FormatData();
		strTemp+=DataOSCCAL.FormatData();
        return 0L;
}
