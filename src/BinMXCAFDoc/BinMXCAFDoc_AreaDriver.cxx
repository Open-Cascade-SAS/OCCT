// File:	BinMXCAFDoc_AreaDriver.cxx
// Created:	Tue May 17 09:45:48 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005


#include <BinMXCAFDoc_AreaDriver.ixx>
#include <XCAFDoc_Area.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_AreaDriver::BinMXCAFDoc_AreaDriver(const Handle(CDM_MessageDriver)& theMsgDriver) 
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Area)->Name()) {
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_AreaDriver::NewEmpty() const {
  return new XCAFDoc_Area();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_AreaDriver::Paste(const BinObjMgt_Persistent& theSource,
					       const Handle(TDF_Attribute)& theTarget,
					       BinObjMgt_RRelocationTable& ) const 
{
  Handle(XCAFDoc_Area) anAtt = Handle(XCAFDoc_Area)::DownCast(theTarget);
  Standard_Real aValue;
  Standard_Boolean isOk = theSource >> aValue;
  if(isOk)
    anAtt->Set(aValue);
  return isOk;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_AreaDriver::Paste(const Handle(TDF_Attribute)& theSource,
				   BinObjMgt_Persistent& theTarget,
				   BinObjMgt_SRelocationTable& ) const 
{
  Handle(XCAFDoc_Area) anAtt = Handle(XCAFDoc_Area)::DownCast(theSource);
  theTarget << anAtt->Get();
}

