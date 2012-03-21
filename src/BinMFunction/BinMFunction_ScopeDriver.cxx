// Created on: 2008-05-11
// Created by: Vlad Romashko
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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



#include <BinMFunction_ScopeDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <TFunction_Scope.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

//=======================================================================
//function : BinMFunction_GraphNodeDriver
//purpose  : 
//=======================================================================

BinMFunction_ScopeDriver::BinMFunction_ScopeDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TFunction_Scope)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMFunction_ScopeDriver::NewEmpty() const
{
  return new TFunction_Scope();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMFunction_ScopeDriver::Paste(const BinObjMgt_Persistent&  theSource,
						 const Handle(TDF_Attribute)& theTarget,
						 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TFunction_Scope) S = Handle(TFunction_Scope)::DownCast(theTarget);

  Standard_Integer nb;
  if ( !(theSource >> nb) )
    return Standard_False;
  if (!nb)
    return Standard_True;

  TFunction_DoubleMapOfIntegerLabel& map = S->ChangeFunctions();

  // IDs
  TColStd_Array1OfInteger IDs(1, nb);
  theSource.GetIntArray (&IDs(1), nb);

  // Labels
  Standard_Integer freeID = 0;
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    TCollection_AsciiString entry;
    if ( !(theSource >> entry) )
      return Standard_False;
    TDF_Label L;
    TDF_Tool::Label(S->Label().Data(), entry, L, Standard_True);
    if (!L.IsNull())
    {
      map.Bind(IDs.Value(i), L);
      if (IDs.Value(i) > freeID)
	freeID = IDs.Value(i);
    }
  }

  // Free ID
  freeID++;
  S->SetFreeID(freeID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMFunction_ScopeDriver::Paste (const Handle(TDF_Attribute)& theSource,
				      BinObjMgt_Persistent&        theTarget,
				      BinObjMgt_SRelocationTable&  ) const
{
  Handle(TFunction_Scope) S = Handle(TFunction_Scope)::DownCast(theSource);
  const TFunction_DoubleMapOfIntegerLabel& map = S->GetFunctions();
  const Standard_Integer nb = map.Extent();

  // Number of functions
  theTarget << nb;
  if (!nb)
    return;

  // IDs
  {
    TColStd_Array1OfInteger aSourceArray(1, nb);
    TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel itr(map);
    for (Standard_Integer i = 1; itr.More(); itr.Next(), i++)
    {
      aSourceArray.SetValue(i, itr.Key1());
    }
    Standard_Integer *aPtr = (Standard_Integer *) &aSourceArray(1);
    theTarget.PutIntArray(aPtr, nb);
  }

  // Labels
  {
    TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel itr(map);
    for (; itr.More(); itr.Next())
    {
      TDF_Label L = itr.Key2();
      if (!L.IsNull())
      {
	TCollection_AsciiString entry;
	TDF_Tool::Entry(L, entry);
	theTarget << entry;
      }
    }
  }
}

