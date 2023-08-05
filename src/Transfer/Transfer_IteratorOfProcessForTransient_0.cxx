// Created on: 1992-02-03
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Transfer_IteratorOfProcessForTransient.hxx>

#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_ProcessForTransient.hxx>
#include <Transfer_TransferMapOfProcessForTransient.hxx>
#include <Transfer_ActorOfProcessForTransient.hxx>
#include <Transfer_Binder.hxx>

//=======================================================================
//function : Transfer_IteratorOfProcessForTransient
//purpose  : 
//=======================================================================
Transfer_IteratorOfProcessForTransient::Transfer_IteratorOfProcessForTransient(const Standard_Boolean withstarts)
  : Transfer_TransferIterator()
{
  if (withstarts) thestarts = new TColStd_HSequenceOfTransient();
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void  Transfer_IteratorOfProcessForTransient::Add
(const Handle(Transfer_Binder)& binder)
{
  if (!thestarts.IsNull()) throw Standard_NoSuchObject("Transfer_IteratorOfProcessForTransient : Add, Starting Object required not provided");
  AddItem(binder);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void  Transfer_IteratorOfProcessForTransient::Add
(const Handle(Transfer_Binder)& binder, const Handle(Standard_Transient)& start)
{
  AddItem(binder);
  if (!thestarts.IsNull()) thestarts->Append(start);
}

//=======================================================================
//function : Filter
//purpose  : 
//=======================================================================
void  Transfer_IteratorOfProcessForTransient::Filter
(const Handle(TColStd_HSequenceOfTransient)& list, const Standard_Boolean keep)
{
  if (list.IsNull() || thestarts.IsNull()) return;
  Standard_Integer i, j, nb = thestarts->Length();
  if (nb == 0) return;
  Handle(Transfer_Binder) factice;
  Transfer_TransferMapOfProcessForTransient amap(nb);
  for (i = 1; i <= nb; i++)
  {
    j = amap.Add(thestarts->Value(i), factice);
    SelectItem(j, !keep);
  }

  //  Comparison
  nb = list->Length();
  for (i = 1; i <= nb; i++)
  {
    j = amap.FindIndex(list->Value(i));
    if (j > 0) SelectItem(j, keep);
  }
}

//=======================================================================
//function : HasStarting
//purpose  : 
//=======================================================================
Standard_Boolean  Transfer_IteratorOfProcessForTransient::HasStarting() const
{
  return (!thestarts.IsNull());
}

//=======================================================================
//function : Starting
//purpose  : 
//=======================================================================
const Handle(Standard_Transient)& Transfer_IteratorOfProcessForTransient::Starting() const
{
  if (thestarts.IsNull()) throw Standard_NoSuchObject("TransferIterator : No Starting defined at all");
  return thestarts->Value(thecurr);
}
