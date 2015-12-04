// Created on: 2008-05-11
// Created by: Vlad Romashko
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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


#include <BinMDF_ADriver.hxx>
#include <BinMFunction_ScopeDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel.hxx>
#include <TFunction_Scope.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMFunction_ScopeDriver,BinMDF_ADriver)

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

