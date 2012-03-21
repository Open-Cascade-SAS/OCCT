// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <Transfer_Binder.ixx>
#include <Transfer_VoidBinder.hxx>
#include <Transfer_TransferFailure.hxx>

#include <TCollection_HAsciiString.hxx>
#include <Interface_IntVal.hxx>
#include <Geom2d_CartesianPoint.hxx>


//=======================================================================
//function : Transfer_Binder
//purpose  : 
//=======================================================================

Transfer_Binder::Transfer_Binder ()
{  
  thestatus = Transfer_StatusVoid; 
  theexecst = Transfer_StatusInitial;
  thecheck = new Interface_Check;
} 

//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

void Transfer_Binder::Merge (const Handle(Transfer_Binder)& other)
{
  if (other.IsNull()) return;
  if ((int) theexecst < (int) other->StatusExec()) theexecst = other->StatusExec();
  thecheck->GetMessages (other->Check());
}

//=======================================================================
//function : IsMultiple
//purpose  : 
//=======================================================================

Standard_Boolean  Transfer_Binder::IsMultiple () const
{
  if (thenextr.IsNull()) return Standard_False;
  if (!HasResult()) return thenextr->IsMultiple();

  Handle(Transfer_Binder) next = thenextr;
  while (!next.IsNull()) {
    if (next->HasResult()) return Standard_True;
    next = next->NextResult();
  }
  return Standard_False;
}

//=======================================================================
//function : AddResult
//purpose  : 
//=======================================================================

void  Transfer_Binder::AddResult (const Handle(Transfer_Binder)& next)
{
  if (next == this || next.IsNull()) return;
  next->CutResult(this);
  if (thenextr.IsNull()) 
    thenextr = next;
  else {
    //Modification of recursive to cycle
    Handle(Transfer_Binder) theBinder = thenextr;
    while( theBinder != next ) { 
      if( theBinder->NextResult().IsNull() ) {
        theBinder->AddResult(next);
        return;
      }
      else
        theBinder = theBinder->NextResult();
    }
  }
  //former recursive
  // if (thenextr.IsNull()) thenextr = next;
  // else if (thenextr == next) return;
  // else thenextr->AddResult (next);
}

//=======================================================================
//function : CutResult
//purpose  : 
//=======================================================================

void  Transfer_Binder::CutResult (const Handle(Transfer_Binder)& next)
{
  if (thenextr.IsNull()) return;
  if (thenextr == next) thenextr.Nullify();
  //else thenextr->CutResult (next);
  else {
    Handle(Transfer_Binder) currBinder = thenextr, currNext;
    while( !( (currNext = currBinder->NextResult()) == next ) ) {
      if( currNext.IsNull() )
        return;
      currBinder = currNext;
    }
    currBinder->CutResult(next);
  }
}

//=======================================================================
//function : NextResult
//purpose  : 
//=======================================================================

Handle(Transfer_Binder)  Transfer_Binder::NextResult () const
      {  return thenextr;  }


//=======================================================================
//function : SetResultPresent
//purpose  : 
//=======================================================================

void Transfer_Binder::SetResultPresent ()
{
  if (thestatus == Transfer_StatusUsed) Transfer_TransferFailure::Raise
    ("Binder : SetResult, Result is Already Set and Used");
  theexecst = Transfer_StatusDone;
  thestatus = Transfer_StatusDefined;
}

//=======================================================================
//function : HasResult
//purpose  : 
//=======================================================================

Standard_Boolean Transfer_Binder::HasResult () const
      {  return (thestatus != Transfer_StatusVoid);  }

//=======================================================================
//function : SetAlreadyUsed
//purpose  : 
//=======================================================================

void Transfer_Binder::SetAlreadyUsed ()
{  if (thestatus != Transfer_StatusVoid) thestatus = Transfer_StatusUsed;  }


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Transfer_StatusResult Transfer_Binder::Status () const
{  return thestatus;  }


//  ############    Checks    ############

//=======================================================================
//function : StatusExec
//purpose  : 
//=======================================================================

Transfer_StatusExec Transfer_Binder::StatusExec () const
{  return theexecst;  }

//=======================================================================
//function : SetStatusExec
//purpose  : 
//=======================================================================

void Transfer_Binder::SetStatusExec (const Transfer_StatusExec stat)
{  theexecst = stat;  }

//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void Transfer_Binder::AddFail
  (const Standard_CString mess, const Standard_CString orig)
{
  theexecst = Transfer_StatusError;
  thecheck->AddFail(mess,orig);
}

//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void Transfer_Binder::AddWarning
  (const Standard_CString mess, const Standard_CString orig)
{
//  theexecst = Transfer_StatusError;
  thecheck->AddWarning(mess,orig);
}

//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

const Handle(Interface_Check) Transfer_Binder::Check () const
{
  return thecheck;
}

//=======================================================================
//function : CCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check) Transfer_Binder::CCheck ()
{
  return thecheck;
}

