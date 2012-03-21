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

