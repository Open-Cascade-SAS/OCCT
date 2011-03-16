// File:	BinMXCAFDoc_VolumeDriver.cxx
// Created:	Tue May 17 15:16:35 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005

#include <BinMXCAFDoc_VolumeDriver.ixx>
#include <XCAFDoc_Volume.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_VolumeDriver::BinMXCAFDoc_VolumeDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Volume)->Name()) {
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_VolumeDriver::NewEmpty() const {
  return new XCAFDoc_Volume();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_VolumeDriver::Paste(const BinObjMgt_Persistent& theSource,
						 const Handle(TDF_Attribute)& theTarget,
						 BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Volume) anAtt = Handle(XCAFDoc_Volume)::DownCast(theTarget);
  Standard_Real aVol;
  Standard_Boolean isOk = theSource >> aVol;
  if(isOk)
    anAtt->Set(aVol);
  return isOk;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_VolumeDriver::Paste(const Handle(TDF_Attribute)& theSource,
				     BinObjMgt_Persistent& theTarget,
				     BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Volume) anAtt = Handle(XCAFDoc_Volume)::DownCast(theSource);
  theTarget << anAtt->Get();
}

