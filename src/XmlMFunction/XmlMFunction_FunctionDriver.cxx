// Created on: 2001-09-04
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <XmlMFunction_FunctionDriver.ixx>

#include <XmlObjMgt.hxx>

#include <TFunction_Function.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_DOMSTRING (GuidString, "guid")
IMPLEMENT_DOMSTRING (FailureString, "failure")

//=======================================================================
//function : XmlMFunction_FunctionDriver
//purpose  : Constructor
//=======================================================================
XmlMFunction_FunctionDriver::XmlMFunction_FunctionDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMFunction_FunctionDriver::NewEmpty() const
{
  return (new TFunction_Function());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMFunction_FunctionDriver::Paste
                (const XmlObjMgt_Persistent&  theSource,
                 const Handle(TDF_Attribute)& theTarget,
                 XmlObjMgt_RRelocationTable&  ) const
{
  Handle(TFunction_Function) aF = Handle(TFunction_Function)::DownCast(theTarget);

  // function GUID
  XmlObjMgt_DOMString aGuidDomStr =
    theSource.Element().getAttribute(::GuidString());
  Standard_CString aGuidStr = (Standard_CString)aGuidDomStr.GetString();
  if (aGuidStr[0] == '\0')
  {
    WriteMessage ("error retrieving GUID for type TFunction_Function");
    return Standard_False;
  }
  aF->SetDriverGUID(aGuidStr);

  // failure
  Standard_Integer aValue;
  XmlObjMgt_DOMString aFStr = theSource.Element().getAttribute(::FailureString());
  if (!aFStr.GetInteger(aValue))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString
        ("Cannot retrieve failure number for TFunction_Function attribute from \"")
          + aFStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  aF->SetFailure(aValue);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMFunction_FunctionDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                         XmlObjMgt_Persistent&        theTarget,
                                         XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TFunction_Function) aF =
    Handle(TFunction_Function)::DownCast(theSource);
  if (!aF.IsNull())
  {
    //convert GUID into attribute value
    Standard_Character aGuidStr [40];
    Standard_PCharacter pGuidStr;
    //
    pGuidStr=aGuidStr;
    aF->GetDriverGUID().ToCString(pGuidStr);
    theTarget.Element().setAttribute(::GuidString(), aGuidStr);

    //integer value of failure
    theTarget.Element().setAttribute(::FailureString(), aF->GetFailure());
  }
}
