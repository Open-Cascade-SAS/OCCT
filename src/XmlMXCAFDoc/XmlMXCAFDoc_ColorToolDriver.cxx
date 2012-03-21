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



#include <XmlMXCAFDoc_ColorToolDriver.ixx>
#include <XCAFDoc_ColorTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_ColorToolDriver::XmlMXCAFDoc_ColorToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "ColorTool")
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
 Handle(TDF_Attribute) XmlMXCAFDoc_ColorToolDriver::NewEmpty() const
{
  return new XCAFDoc_ColorTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_ColorToolDriver::Paste(const XmlObjMgt_Persistent& ,
						    const Handle(TDF_Attribute)& ,
						    XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_ColorToolDriver::Paste(const Handle(TDF_Attribute)& ,
					XmlObjMgt_Persistent& ,
					XmlObjMgt_SRelocationTable& ) const
{
}

