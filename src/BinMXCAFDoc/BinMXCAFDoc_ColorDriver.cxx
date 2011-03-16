// File:	BinMXCAFDoc_ColorDriver.cxx
// Created:	Tue May 17 14:34:24 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005

#include <BinMXCAFDoc_ColorDriver.ixx>
#include <XCAFDoc_Color.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_ColorDriver::BinMXCAFDoc_ColorDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Color)->Name()) {
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_ColorDriver::NewEmpty() const {
  return new XCAFDoc_Color();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_ColorDriver::Paste(const BinObjMgt_Persistent& theSource,
						const Handle(TDF_Attribute)& theTarget,
						BinObjMgt_RRelocationTable& /*theRelocTable*/) const 
{
  Handle(XCAFDoc_Color) anAtt = Handle(XCAFDoc_Color)::DownCast(theTarget);
  Standard_Real R, G, B;
  Standard_Boolean isOk = theSource >> R >> G >> B;
  if(isOk) {
    anAtt->Set(R, G, B);
  }
  return isOk;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_ColorDriver::Paste(const Handle(TDF_Attribute)& theSource,
				    BinObjMgt_Persistent& theTarget,
				    BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Color) anAtt = Handle(XCAFDoc_Color)::DownCast(theSource);
  Standard_Real R, G, B;
  anAtt->GetRGB(R, G, B);
  theTarget << R << G << B;
}

