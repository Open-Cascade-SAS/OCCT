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


#include <Interface_Check.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransferIterator.hxx>

static Handle(Standard_Transient)   nultrans;  // pour retour const&(Null)


    Transfer_TransferIterator::Transfer_TransferIterator ()
{
  theitems  = new Transfer_HSequenceOfBinder();
  theselect = new TColStd_HSequenceOfInteger();
  themaxi = 0;
  thecurr = 1;
}

    void  Transfer_TransferIterator::AddItem
  (const Handle(Transfer_Binder)& atr)
{
  theitems->Append(atr);
  theselect->Append(1);
  themaxi = theselect->Length();
}

    void  Transfer_TransferIterator::SelectBinder
  (const Handle(Standard_Type)& atype, const Standard_Boolean keep)
{
  for (Standard_Integer i = theitems->Length(); i > 0; i --) {
    if (theitems->Value(i)->IsKind(atype) != keep) {
      theselect->SetValue(i,0);
      if (themaxi == i) themaxi = i-1;
    }
  }
}

    void  Transfer_TransferIterator::SelectResult
  (const Handle(Standard_Type)& atype, const Standard_Boolean keep)
{
  Standard_Integer casetype = 0;
  if (atype->SubType(STANDARD_TYPE(Standard_Transient)))  casetype = 2;

  for (Standard_Integer i = theitems->Length(); i > 0; i --) {
    Handle(Transfer_Binder) atr = theitems->Value(i);
    Handle(Standard_Type) btype = ResultType();
    Standard_Boolean matchtype;
    if      (!atr->HasResult()) matchtype = Standard_False;
    else if (atr->IsMultiple()) matchtype = Standard_False;
    else if (casetype == 0) matchtype = (atype == btype);         // Type fixe
    else                    matchtype = (btype->SubType(atype));  // Dynamique
    if (matchtype != keep) {
      theselect->SetValue(i,0);
      if (themaxi == i) themaxi = i-1;
    }
  }
}

    void  Transfer_TransferIterator::SelectUnique
  (const Standard_Boolean keep)
{
  for (Standard_Integer i = theitems->Length(); i > 0; i --) {
    Handle(Transfer_Binder) atr = theitems->Value(i);
    if (atr->IsMultiple() == keep) {
      theselect->SetValue(i,0);
      if (themaxi == i) themaxi = i-1;
    }
  }
}

    void  Transfer_TransferIterator::SelectItem
  (const Standard_Integer num, const Standard_Boolean keep)
{
  if (num < 1 || num > theselect->Length()) return;
  if (keep) theselect->SetValue (num,1);
  else theselect->SetValue (num,0);
}

//  ....                Iteration-Interrogations                ....

    Standard_Integer  Transfer_TransferIterator::Number () const
{
  Standard_Integer numb,i;  numb = 0;
  for (i = 1; i <= themaxi; i ++) {
    if (theselect->Value(i) != 0) numb ++;
  }
  return numb;
}

    void  Transfer_TransferIterator::Start ()
      {  thecurr = 0;  Next();  }

    Standard_Boolean  Transfer_TransferIterator::More ()
{
  if (thecurr > themaxi) return Standard_False;
  if (theselect->Value(thecurr) == 0) Next();
  if (thecurr > themaxi) return Standard_False;
  return (theselect->Value(thecurr) > 0);
}

    void  Transfer_TransferIterator::Next ()
{
  thecurr ++;
  if (thecurr > themaxi) return;
  if (theselect->Value(thecurr) == 0) Next();
}

    const Handle(Transfer_Binder)&  Transfer_TransferIterator::Value () const
{
  if (thecurr == 0 || thecurr > themaxi) throw Standard_NoSuchObject("TransferIterator : Value");
  if (theselect->Value(thecurr) == 0)    throw Standard_NoSuchObject("TransferIterator : Value");
  return theitems->Value(thecurr);
}

//  ....                Acces aux Donnees du Binder Courant                ....

    Standard_Boolean  Transfer_TransferIterator::HasResult () const
{
  Handle(Transfer_Binder) atr = Value();
  return atr->HasResult();
}

    Standard_Boolean  Transfer_TransferIterator::HasUniqueResult () const
{
  Handle(Transfer_Binder) atr = Value();
  if (atr->IsMultiple()) return Standard_False;
  return atr->HasResult();
}


    Handle(Standard_Type) Transfer_TransferIterator::ResultType () const
{
  Handle(Standard_Type) btype;
  Handle(Transfer_Binder) atr = Value();
  if (!atr->IsMultiple()) btype = atr->ResultType();
//  ResultType de Binder prend en compte le Type Dynamique pour les Handle
  return btype;
}


    Standard_Boolean  Transfer_TransferIterator::HasTransientResult () const
{
  Handle(Standard_Type) btype = ResultType();
  if (btype.IsNull()) return Standard_False;
  return btype->SubType(STANDARD_TYPE(Standard_Transient));
}

    const Handle(Standard_Transient)&
      Transfer_TransferIterator::TransientResult () const
{
  Handle(Transfer_SimpleBinderOfTransient) atr = 
    Handle(Transfer_SimpleBinderOfTransient)::DownCast(Value());
  if (!atr.IsNull()) return atr->Result();
  return nultrans;
}


    Transfer_StatusExec  Transfer_TransferIterator::Status () const
{
  Handle(Transfer_Binder) atr = Value();
  return atr->StatusExec();
}


    Standard_Boolean  Transfer_TransferIterator::HasFails () const
{
  Handle(Transfer_Binder) atr = Value();
  return atr->Check()->HasFailed();
}

    Standard_Boolean  Transfer_TransferIterator::HasWarnings () const
{
  Handle(Transfer_Binder) atr = Value();
  return atr->Check()->HasWarnings();
}

    const Handle(Interface_Check)  Transfer_TransferIterator::Check () const
{
  Handle(Transfer_Binder) atr = Value();
  return atr->Check();
}
