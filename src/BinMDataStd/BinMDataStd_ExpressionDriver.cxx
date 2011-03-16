// File:        BinMDataStd_ExpressionDriver.cxx
// Created:     Wed Sep 12 14:07:32 2001
// Author:      Julia DOROVSKIKH
// Copyright:   Open Cascade 2001
// History:

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
