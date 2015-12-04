// Created on: 2001-09-12
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Expression.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <XmlMDataStd_ExpressionDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_ExpressionDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (VariablesString, "variables")

//=======================================================================
//function : XmlMDataStd_ExpressionDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_ExpressionDriver::XmlMDataStd_ExpressionDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ExpressionDriver::NewEmpty() const
{
  return (new TDataStd_Expression());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ExpressionDriver::Paste
                        (const XmlObjMgt_Persistent&  theSource,
                         const Handle(TDF_Attribute)& theTarget,
                         XmlObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(TDataStd_Expression) aC = 
    Handle(TDataStd_Expression)::DownCast(theTarget);
  const XmlObjMgt_Element& anElem = theSource;

  Standard_Integer aNb;
  TCollection_ExtendedString aMsgString;

  // expression
  TCollection_ExtendedString aString;
  if (!XmlObjMgt::GetExtendedString (theSource, aString))
  {
    WriteMessage("error retrieving ExtendedString for type TDataStd_Expression");
    return Standard_False;
  }
  aC->SetExpression(aString);

  // variables
  XmlObjMgt_DOMString aDOMStr = anElem.getAttribute(::VariablesString());
  if (aDOMStr != NULL)
  {
    Standard_CString aVs = Standard_CString(aDOMStr.GetString());

    // first variable
    if (!XmlObjMgt::GetInteger(aVs, aNb))
    {
      aMsgString = TCollection_ExtendedString
        ("XmlMDataStd_ExpressionDriver: Cannot retrieve reference on first variable from \"")
          + aDOMStr + "\"";
      WriteMessage (aMsgString);
      return Standard_False;
    }
    Standard_Integer i = 1;
    while (aNb > 0)
    {
      Handle(TDF_Attribute) aV;
      if (theRelocTable.IsBound(aNb))
        aV = Handle(TDataStd_Variable)::DownCast(theRelocTable.Find(aNb));
      else
      {
        aV = new TDataStd_Variable;
        theRelocTable.Bind(aNb, aV);
      }
      aC->GetVariables().Append(aV);

      // next variable
      if (!XmlObjMgt::GetInteger(aVs, aNb)) aNb = 0;
      i++;
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ExpressionDriver::Paste
                        (const Handle(TDF_Attribute)& theSource,
                         XmlObjMgt_Persistent&        theTarget,
                         XmlObjMgt_SRelocationTable&  theRelocTable) const
{
  Handle(TDataStd_Expression) aC =
    Handle(TDataStd_Expression)::DownCast(theSource);
  XmlObjMgt_Element& anElem = theTarget;

  Standard_Integer aNb;
  Handle(TDF_Attribute) TV;   

  // expression
  XmlObjMgt::SetExtendedString (theTarget, aC->Name());

  // variables
  Standard_Integer nbvar = aC->GetVariables().Extent();
  if (nbvar >= 1)
  {
    TCollection_AsciiString aGsStr;
    TDF_ListIteratorOfAttributeList it;
    Standard_Integer index = 0;
    for (it.Initialize(aC->GetVariables()); it.More(); it.Next())
    {
      index++;
      TV = it.Value(); 
      if (!TV.IsNull())
      {
        aNb = theRelocTable.FindIndex(TV);
        if (aNb == 0)
        {
          aNb = theRelocTable.Add(TV);
        }
        aGsStr += TCollection_AsciiString(aNb) + " ";
      }
      else aGsStr += "0 ";
    }
    anElem.setAttribute(::VariablesString(), aGsStr.ToCString());
  }
}
