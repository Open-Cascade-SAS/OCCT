// Created on: 2001-09-12
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


#include <XmlMDataStd_ExpressionDriver.ixx>
#include <TDataStd_Expression.hxx>
#include <TDataStd_Variable.hxx>

#include <TDF_ListIteratorOfAttributeList.hxx>

#include <XmlObjMgt.hxx>

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
