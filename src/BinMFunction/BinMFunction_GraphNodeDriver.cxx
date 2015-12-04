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
#include <BinMFunction_GraphNodeDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TDF_Attribute.hxx>
#include <TFunction_GraphNode.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMFunction_GraphNodeDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMFunction_GraphNodeDriver
//purpose  : 
//=======================================================================
BinMFunction_GraphNodeDriver::BinMFunction_GraphNodeDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TFunction_GraphNode)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMFunction_GraphNodeDriver::NewEmpty() const
{
  return new TFunction_GraphNode();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMFunction_GraphNodeDriver::Paste(const BinObjMgt_Persistent&  theSource,
						     const Handle(TDF_Attribute)& theTarget,
						     BinObjMgt_RRelocationTable&  ) const
{
  Handle(TFunction_GraphNode) GN = Handle(TFunction_GraphNode)::DownCast(theTarget);

  Standard_Integer intStatus, nb_previous, nb_next;
  if (! (theSource >> intStatus >> nb_previous >> nb_next))
    return Standard_False;

  // Execution status
  GN->SetStatus((TFunction_ExecutionStatus) intStatus);
  
  // Previous functions
  if (nb_previous)
  {
    TColStd_Array1OfInteger aTargetArray(1, nb_previous);
    theSource.GetIntArray (&aTargetArray(1), nb_previous);
    
    for (Standard_Integer i = 1; i <= nb_previous; i++)
    {
      GN->AddPrevious(aTargetArray.Value(i));
    }
  }
  
  // Next functions
  if (nb_next)
  {
    TColStd_Array1OfInteger aTargetArray(1, nb_next);
    theSource.GetIntArray (&aTargetArray(1), nb_next);
    
    for (Standard_Integer i = 1; i <= nb_next; i++)
    {
      GN->AddNext(aTargetArray.Value(i));
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMFunction_GraphNodeDriver::Paste (const Handle(TDF_Attribute)& theSource,
					  BinObjMgt_Persistent&        theTarget,
					  BinObjMgt_SRelocationTable&  ) const
{
  Handle(TFunction_GraphNode) GN = Handle(TFunction_GraphNode)::DownCast(theSource);

  // Execution status
  theTarget << (Standard_Integer) GN->GetStatus();
  // Number of previous functions
  theTarget << GN->GetPrevious().Extent();
  // Number of next functions
  theTarget << GN->GetNext().Extent();

  // Previous functions
  Standard_Integer nb  = GN->GetPrevious().Extent();
  if (nb)
  {
    TColStd_Array1OfInteger aSourceArray(1, nb);
    TColStd_MapIteratorOfMapOfInteger itr(GN->GetPrevious());
    for (Standard_Integer i = 1; itr.More(); itr.Next(), i++)
    {
      aSourceArray.SetValue(i, itr.Key());
    }
    Standard_Integer *aPtr = (Standard_Integer *) &aSourceArray(1);
    theTarget.PutIntArray(aPtr, nb);
  }

  // Next functions
  nb  = GN->GetNext().Extent();
  if (nb)
  {
    TColStd_Array1OfInteger aSourceArray(1, nb);
    TColStd_MapIteratorOfMapOfInteger itr(GN->GetNext());
    for (Standard_Integer i = 1; itr.More(); itr.Next(), i++)
    {
      aSourceArray.SetValue(i, itr.Key());
    }
    Standard_Integer *aPtr = (Standard_Integer *) &aSourceArray(1);
    theTarget.PutIntArray(aPtr, nb);
  }
}

