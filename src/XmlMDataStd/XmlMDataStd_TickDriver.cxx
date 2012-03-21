// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#include <XmlMDataStd_TickDriver.ixx>
#include <TDataStd_Tick.hxx>

//=======================================================================
//function : XmlMDataStd_TickDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_TickDriver::XmlMDataStd_TickDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
 Handle(TDF_Attribute) XmlMDataStd_TickDriver::NewEmpty() const
{
  return (new TDataStd_Tick());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_TickDriver::Paste(const XmlObjMgt_Persistent&,
					       const Handle(TDF_Attribute)&,
					       XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMDataStd_TickDriver::Paste(const Handle(TDF_Attribute)&,
				   XmlObjMgt_Persistent&,
				   XmlObjMgt_SRelocationTable&  ) const
{

}
