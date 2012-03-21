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


#include <BinMDataStd_ExpressionDriver.ixx>
#include <TDataStd_Expression.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>

//=======================================================================
//function : BinMDataStd_ExpressionDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_ExpressionDriver::BinMDataStd_ExpressionDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_ExpressionDriver::NewEmpty() const
{
  return (new TDataStd_Expression());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_ExpressionDriver::Paste
                        (const BinObjMgt_Persistent&  theSource,
                         const Handle(TDF_Attribute)& theTarget,
                         BinObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(TDataStd_Expression) aC = 
    Handle(TDataStd_Expression)::DownCast(theTarget);

  // variables
  Standard_Integer nbvar;
  if (! (theSource >> nbvar) || nbvar < 0)
    return Standard_False;
  TDF_AttributeList& aList = aC->GetVariables();
  for (; nbvar > 0; nbvar--)
  {
    Handle(TDF_Attribute) aV;
    Standard_Integer aNb;
    if (! (theSource >> aNb))
      return Standard_False;
    if (aNb > 0)
    {
      if (theRelocTable.IsBound(aNb))
        aV = Handle(TDataStd_Variable)::DownCast(theRelocTable.Find(aNb));
      else
      {
        aV = new TDataStd_Variable;
        theRelocTable.Bind(aNb, aV);
      }
    }
    aList.Append(aV);
  }

  // expression
  TCollection_ExtendedString aString;
  if (! (theSource >> aString))
    return Standard_False;
  aC->SetExpression(aString);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_ExpressionDriver::Paste
                        (const Handle(TDF_Attribute)& theSource,
                         BinObjMgt_Persistent&        theTarget,
                         BinObjMgt_SRelocationTable&  theRelocTable) const
{
  Handle(TDataStd_Expression) aC =
    Handle(TDataStd_Expression)::DownCast(theSource);

  // variables
  const TDF_AttributeList& aList = aC->GetVariables();
  Standard_Integer nbvar = aList.Extent();
  theTarget << nbvar;
  TDF_ListIteratorOfAttributeList it;
  for (it.Initialize(aList); it.More(); it.Next())
  {
    const Handle(TDF_Attribute)& TV = it.Value();
    Standard_Integer aNb;
    if (!TV.IsNull())
      aNb = theRelocTable.Add(TV);
    else
      aNb = -1;
    theTarget << aNb;
  }

  // expression
  TCollection_ExtendedString aName = aC->Name();
  theTarget << aName;
}
