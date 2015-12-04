// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Type.hxx>
#include <STEPSelections_SelectForTransfer.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPSelections_SelectForTransfer,XSControl_SelectForTransfer)

//=======================================================================
//function : STEPSelections_SelectForTransfer
//purpose  : 
//=======================================================================
STEPSelections_SelectForTransfer::STEPSelections_SelectForTransfer()
{
  
}
//=======================================================================
//function : STEPSelections_SelectForTransfer
//purpose  : 
//=======================================================================

STEPSelections_SelectForTransfer::STEPSelections_SelectForTransfer(const Handle(XSControl_TransferReader)& TR)
{
  SetReader(TR);
}

//=======================================================================
//function : RootResult
//purpose  : 
//=======================================================================

 Interface_EntityIterator STEPSelections_SelectForTransfer::RootResult(const Interface_Graph& /*G*/) const
{
  Interface_EntityIterator iter;
  Handle(TColStd_HSequenceOfTransient) roots = Reader()->TransientProcess()->RootsForTransfer();
  Standard_Integer nb = roots->Length();
    for(Standard_Integer i = 1; i <= nb ; i++) 
      iter.GetOneItem(roots->Value(i));
  return iter;
}

