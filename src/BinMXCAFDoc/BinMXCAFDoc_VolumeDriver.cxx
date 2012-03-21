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

