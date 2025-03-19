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

#include <Geom2d_CartesianPoint.hxx>
#include <Interface_Check.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransferFailure.hxx>
#include <Transfer_VoidBinder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_Binder, Standard_Transient)

//=================================================================================================

Transfer_Binder::Transfer_Binder()
{
  thestatus = Transfer_StatusVoid;
  theexecst = Transfer_StatusInitial;
  thecheck  = new Interface_Check;
}

//=================================================================================================

void Transfer_Binder::Merge(const Handle(Transfer_Binder)& other)
{
  if (other.IsNull())
    return;
  if ((int)theexecst < (int)other->StatusExec())
    theexecst = other->StatusExec();
  thecheck->GetMessages(other->Check());
}

//=================================================================================================

Standard_Boolean Transfer_Binder::IsMultiple() const
{
  if (thenextr.IsNull())
    return Standard_False;
  if (!HasResult())
    return thenextr->IsMultiple();

  Handle(Transfer_Binder) next = thenextr;
  while (!next.IsNull())
  {
    if (next->HasResult())
      return Standard_True;
    next = next->NextResult();
  }
  return Standard_False;
}

//=================================================================================================

void Transfer_Binder::AddResult(const Handle(Transfer_Binder)& next)
{
  if (next == this || next.IsNull())
    return;
  next->CutResult(this);
  if (thenextr.IsNull())
    thenextr = next;
  else
  {
    // Modification of recursive to cycle
    Handle(Transfer_Binder) theBinder = theendr.IsNull() ? thenextr : theendr;
    while (theBinder != next)
    {
      if (theBinder->NextResult().IsNull())
      {
        theBinder->AddResult(next);
        theendr = next;
        return;
      }
      else
        theBinder = theBinder->NextResult();
    }
  }
}

//=================================================================================================

void Transfer_Binder::CutResult(const Handle(Transfer_Binder)& next)
{
  if (thenextr.IsNull())
    return;
  if (thenextr == next)
  {
    thenextr.Nullify();
    theendr.Nullify();
  }
  // else thenextr->CutResult (next);
  else
  {
    Handle(Transfer_Binder) currBinder = thenextr, currNext;
    while (!((currNext = currBinder->NextResult()) == next))
    {
      if (currNext.IsNull())
        return;
      currBinder = currNext;
    }
    currBinder->CutResult(next);
  }
}

//=================================================================================================

Handle(Transfer_Binder) Transfer_Binder::NextResult() const
{
  return thenextr;
}

//=================================================================================================

void Transfer_Binder::SetResultPresent()
{
  if (thestatus == Transfer_StatusUsed)
    throw Transfer_TransferFailure("Binder : SetResult, Result is Already Set and Used");
  theexecst = Transfer_StatusDone;
  thestatus = Transfer_StatusDefined;
}

//=================================================================================================

Standard_Boolean Transfer_Binder::HasResult() const
{
  return (thestatus != Transfer_StatusVoid);
}

//=================================================================================================

void Transfer_Binder::SetAlreadyUsed()
{
  if (thestatus != Transfer_StatusVoid)
    thestatus = Transfer_StatusUsed;
}

//=================================================================================================

Transfer_StatusResult Transfer_Binder::Status() const
{
  return thestatus;
}

//  ############    Checks    ############

//=================================================================================================

Transfer_StatusExec Transfer_Binder::StatusExec() const
{
  return theexecst;
}

//=================================================================================================

void Transfer_Binder::SetStatusExec(const Transfer_StatusExec stat)
{
  theexecst = stat;
}

//=================================================================================================

void Transfer_Binder::AddFail(const Standard_CString mess, const Standard_CString orig)
{
  theexecst = Transfer_StatusError;
  thecheck->AddFail(mess, orig);
}

//=================================================================================================

void Transfer_Binder::AddWarning(const Standard_CString mess, const Standard_CString orig)
{
  //  theexecst = Transfer_StatusError;
  thecheck->AddWarning(mess, orig);
}

//=================================================================================================

const Handle(Interface_Check) Transfer_Binder::Check() const
{
  return thecheck;
}

//=================================================================================================

Handle(Interface_Check) Transfer_Binder::CCheck()
{
  return thecheck;
}

//=================================================================================================

Transfer_Binder::~Transfer_Binder()
{
  // To prevent stack overflow on long chains it is needed
  // to avoid recursive destruction of the field thenextr
  if (!thenextr.IsNull())
  {
    Handle(Transfer_Binder) aCurr = thenextr;
    theendr.Nullify();
    thenextr.Nullify();
    // we check GetRefCount in order to not destroy a chain if it belongs also
    // to another upper level chain (two chains continue at the same binder)
    while (!aCurr->thenextr.IsNull() && aCurr->thenextr->GetRefCount() == 1)
    {
      Handle(Transfer_Binder) aPrev = aCurr;
      aCurr                         = aCurr->thenextr;
      aPrev->thenextr.Nullify();
    }
  }
}
