// File:	BinMXCAFDoc_CentroidDriver.cxx
// Created:	Tue May 17 14:20:54 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005


#include <BinMXCAFDoc_CentroidDriver.ixx>
#include <XCAFDoc_Centroid.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_CentroidDriver::BinMXCAFDoc_CentroidDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Centroid)->Name()) {
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_CentroidDriver::NewEmpty() const {
  return new XCAFDoc_Centroid();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_CentroidDriver::Paste(const BinObjMgt_Persistent& theSource,
						   const Handle(TDF_Attribute)& theTarget,
						   BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Centroid) anAtt = Handle(XCAFDoc_Centroid)::DownCast(theTarget);
  Standard_Real x, y, z;
  Standard_Boolean isOk = theSource >> x >> y >> z;
  if(isOk) {
    gp_Pnt aPnt(x, y, z);
    anAtt->Set(aPnt);
  }
  return isOk;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_CentroidDriver::Paste(const Handle(TDF_Attribute)& theSource,
				       BinObjMgt_Persistent& theTarget,
				       BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Centroid) anAtt = Handle(XCAFDoc_Centroid)::DownCast(theSource);
  gp_Pnt aPnt = anAtt->Get();
  theTarget << aPnt.X() << aPnt.Y() << aPnt.Z();
}

