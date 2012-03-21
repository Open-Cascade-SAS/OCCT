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



#include <XmlMXCAFDoc_DocumentToolDriver.ixx>
#include <XCAFDoc_DocumentTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_DocumentToolDriver::XmlMXCAFDoc_DocumentToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "DocumentTool")
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
 Handle(TDF_Attribute) XmlMXCAFDoc_DocumentToolDriver::NewEmpty() const {
   return new XCAFDoc_DocumentTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
 Standard_Boolean XmlMXCAFDoc_DocumentToolDriver::Paste(const XmlObjMgt_Persistent& /*theSource*/,
							const Handle(TDF_Attribute)& theTarget,
							XmlObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_DocumentTool) T = Handle(XCAFDoc_DocumentTool)::DownCast(theTarget);
  T->Init();
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_DocumentToolDriver::Paste(const Handle(TDF_Attribute)& /*theSource*/,
					   XmlObjMgt_Persistent& /*theTarget*/,
					   XmlObjMgt_SRelocationTable& /*theRelocTable*/) const
{
}

