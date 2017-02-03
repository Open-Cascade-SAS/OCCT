// Created on: 1992-10-28
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

#ifndef _Transfer_TransferIterator_HeaderFile
#define _Transfer_TransferIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Transfer_HSequenceOfBinder.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Type.hxx>
#include <Standard_Boolean.hxx>
#include <Transfer_StatusExec.hxx>
class Standard_NoSuchObject;
class Transfer_Binder;
class Standard_Transient;
class Interface_Check;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Defines an Iterator on the result of a Transfer
//! Available for Normal Results or not (Erroneous Transfer)
//! It gives several kinds of Informations, and allows to consider
//! various criteria (criteria are cumulative)
class Transfer_TransferIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty Iterator
  Standard_EXPORT Transfer_TransferIterator();
  
  //! Adds a Binder to the iteration list (construction)
  Standard_EXPORT void AddItem (const Handle(Transfer_Binder)& atr);
  
  //! Selects Items on the Type of Binder : keep only
  //! Binders which are of a given Type (if keep is True) or
  //! reject only them (if keep is False)
  Standard_EXPORT void SelectBinder (const Handle(Standard_Type)& atype, const Standard_Boolean keep);
  
  //! Selects Items on the Type of Result. Considers only Unique
  //! Results. Considers Dynamic Type for Transient Result,
  //! Static Type (the one given to define the Binder) else.
  //!
  //! Results which are of a given Type (if keep is True) or reject
  //! only them (if keep is False)
  Standard_EXPORT void SelectResult (const Handle(Standard_Type)& atype, const Standard_Boolean keep);
  
  //! Select Items according Unicity : keep only Unique Results (if
  //! keep is True) or keep only Multiple Results (if keep is False)
  Standard_EXPORT void SelectUnique (const Standard_Boolean keep);
  
  //! Selects/Unselect (according to <keep> an item designated by
  //! its rank <num> in the list
  //! Used by sub-classes which have specific criteria
  Standard_EXPORT void SelectItem (const Standard_Integer num, const Standard_Boolean keep);
  
  //! Returns count of Binders to be iterated
  Standard_EXPORT Standard_Integer Number() const;
  
  //! Clears Iteration in progress, to allow it to be restarted
  Standard_EXPORT void Start();
  
  //! Returns True if there are other Items to iterate
  Standard_EXPORT Standard_Boolean More();
  
  //! Sets Iteration to the next Item
  Standard_EXPORT void Next();
  
  //! Returns the current Binder
  Standard_EXPORT const Handle(Transfer_Binder)& Value() const;
  
  //! Returns True if current Item brings a Result, Transient
  //! (Handle) or not or Multiple. That is to say, if it corresponds
  //! to a normally acheived Transfer, Transient Result is read by
  //! specific TransientResult below.
  //! Other kind of Result must be read specifically from its Binder
  Standard_EXPORT Standard_Boolean HasResult() const;
  
  //! Returns True if Current Item has a Unique Result
  Standard_EXPORT Standard_Boolean HasUniqueResult() const;
  
  //! Returns the Type of the Result of the current Item, if Unique.
  //! If No Unique Result (Error Transfert or Multiple Result),
  //! returns a Null Handle
  //! The Type is : the Dynamic Type for a Transient Result,
  //! the Type defined by the Binder Class else
  Standard_EXPORT Handle(Standard_Type) ResultType() const;
  
  //! Returns True if the current Item has a Transient Unique
  //! Result (if yes, use TransientResult to get it)
  Standard_EXPORT Standard_Boolean HasTransientResult() const;
  
  //! Returns the Transient Result of the current Item if there is
  //! (else, returns a null Handle)
  //! Supposes that Binding is done by a SimpleBinderOfTransient
  Standard_EXPORT const Handle(Standard_Transient)& TransientResult() const;
  
  //! Returns Execution Status of current Binder
  //! Normal transfer corresponds to StatusDone
  Standard_EXPORT Transfer_StatusExec Status() const;
  
  //! Returns True if Fail Messages are recorded with the current
  //! Binder. They can then be read through Check (see below)
  Standard_EXPORT Standard_Boolean HasFails() const;
  
  //! Returns True if Warning Messages are recorded with the current
  //! Binder. They can then be read through Check (see below)
  Standard_EXPORT Standard_Boolean HasWarnings() const;
  
  //! Returns Check associated to current Binder
  //! (in case of error, it brings Fail messages)
  //! (in case of warnings, it brings Warning messages)
  Standard_EXPORT const Handle(Interface_Check) Check() const;




protected:



  Standard_Integer thecurr;


private:



  Handle(Transfer_HSequenceOfBinder) theitems;
  Handle(TColStd_HSequenceOfInteger) theselect;
  Standard_Integer themaxi;


};







#endif // _Transfer_TransferIterator_HeaderFile
