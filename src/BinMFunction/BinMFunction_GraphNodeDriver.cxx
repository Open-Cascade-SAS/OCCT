// File:	BinMFunction_GraphNodeDriver.cxx
// Created:	Thu May 11 11:26:02 2008
// Author:	Vlad Romashko <vladislav.romashko@opencascade.com>
// Copyright:	Open CasCade S.A. 2008


#include <BinMFunction_GraphNodeDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <TFunction_GraphNode.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>

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

