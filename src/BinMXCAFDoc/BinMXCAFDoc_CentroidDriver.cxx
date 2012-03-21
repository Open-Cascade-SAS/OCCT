// Created on: 2005-05-17
// Created by: Eugeny NAPALKOV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

