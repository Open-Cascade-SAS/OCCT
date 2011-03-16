// File:	BinMXCAFDoc_DocumentToolDriver.cxx
// Created:	Tue May 17 13:11:45 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005

#include <BinMXCAFDoc_DocumentToolDriver.ixx>
#include <XCAFDoc_DocumentTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_DocumentToolDriver::BinMXCAFDoc_DocumentToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver) 
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_DocumentTool)->Name()) {
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_DocumentToolDriver::NewEmpty() const {
  return new XCAFDoc_DocumentTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_DocumentToolDriver::Paste(const BinObjMgt_Persistent& /*theSource*/,
							const Handle(TDF_Attribute)& theTarget,
							BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_DocumentTool) T = Handle(XCAFDoc_DocumentTool)::DownCast(theTarget);
  T->Init();
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_DocumentToolDriver::Paste(const Handle(TDF_Attribute)& /*theSource*/,
					   BinObjMgt_Persistent& /*theTarget*/,
					   BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
}
