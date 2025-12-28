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

#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Expression.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_Attribute.hxx>
#include <NCollection_List.hxx>
#include <XmlMDataStd_ExpressionDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_ExpressionDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(VariablesString, "variables")

//=================================================================================================

XmlMDataStd_ExpressionDriver::XmlMDataStd_ExpressionDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataStd_ExpressionDriver::NewEmpty() const
{
  return (new TDataStd_Expression());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDataStd_ExpressionDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                         const occ::handle<TDF_Attribute>& theTarget,
                                         XmlObjMgt_RRelocationTable&       theRelocTable) const
{
  occ::handle<TDataStd_Expression> aC     = occ::down_cast<TDataStd_Expression>(theTarget);
  const XmlObjMgt_Element&         anElem = theSource;

  int                        aNb;
  TCollection_ExtendedString aMsgString;

  // expression
  TCollection_ExtendedString aString;
  if (!XmlObjMgt::GetExtendedString(theSource, aString))
  {
    myMessageDriver->Send("error retrieving ExtendedString for type TDataStd_Expression",
                          Message_Fail);
    return false;
  }
  aC->SetExpression(aString);

  // variables
  XmlObjMgt_DOMString aDOMStr = anElem.getAttribute(::VariablesString());
  if (aDOMStr != NULL)
  {
    const char* aVs = static_cast<const char*>(aDOMStr.GetString());

    // first variable
    if (!XmlObjMgt::GetInteger(aVs, aNb))
    {
      aMsgString =
        TCollection_ExtendedString(
          "XmlMDataStd_ExpressionDriver: Cannot retrieve reference on first variable from \"")
        + aDOMStr + "\"";
      myMessageDriver->Send(aMsgString, Message_Fail);
      return false;
    }
    while (aNb > 0)
    {
      occ::handle<TDF_Attribute> aV;
      if (theRelocTable.IsBound(aNb))
        aV = occ::down_cast<TDataStd_Variable>(theRelocTable.Find(aNb));
      else
      {
        aV = new TDataStd_Variable;
        theRelocTable.Bind(aNb, aV);
      }
      aC->GetVariables().Append(aV);

      // next variable
      if (!XmlObjMgt::GetInteger(aVs, aNb))
        aNb = 0;
    }
  }

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ExpressionDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                         XmlObjMgt_Persistent&             theTarget,
                                         XmlObjMgt_SRelocationTable&       theRelocTable) const
{
  occ::handle<TDataStd_Expression> aC     = occ::down_cast<TDataStd_Expression>(theSource);
  XmlObjMgt_Element&               anElem = theTarget;

  int                        aNb;
  occ::handle<TDF_Attribute> TV;

  // expression
  XmlObjMgt::SetExtendedString(theTarget, aC->Name());

  // variables
  int nbvar = aC->GetVariables().Extent();
  if (nbvar >= 1)
  {
    TCollection_AsciiString                                aGsStr;
    NCollection_List<occ::handle<TDF_Attribute>>::Iterator it;
    for (it.Initialize(aC->GetVariables()); it.More(); it.Next())
    {
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
      else
        aGsStr += "0 ";
    }
    anElem.setAttribute(::VariablesString(), aGsStr.ToCString());
  }
}
