// Created on: 1993-06-09
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Transfer_Binder_HeaderFile
#define _Transfer_Binder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Transfer_StatusResult.hxx>
#include <Transfer_StatusExec.hxx>
#include <Standard_Transient.hxx>
class Interface_Check;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class Transfer_Binder;
DEFINE_STANDARD_HANDLE(Transfer_Binder, Standard_Transient)

//! A Binder is an auxiliary object to Map the Result of the
//! Transfer of a given Object : it records the Result of the
//! Unitary Transfer (Resulting Object), status of progress and
//! error (if any) of the Process
//!
//! The class Binder itself makes no definition for the Result :
//! it is defined by sub-classes : it can be either Simple (and
//! has to be typed : see generic class SimpleBinder) or Multiple
//! (see class MultipleBinder).
//!
//! In principle, for a Transfer in progress, Result cannot be
//! accessed : this would cause an exception raising.
//! This is controlled by the value if StatusResult : if it is
//! "Used", the Result cannot be changed. This status is normally
//! controlled by TransferProcess but can be directly (see method
//! SetAlreadyUsed)
//!
//! Checks can be completed by a record of cases, as string which
//! can be used as codes, but not to be printed
//!
//! In addition to the Result, a Binder can bring a list of
//! Attributes, which are additional data, each of them has a name
class Transfer_Binder : public Standard_Transient
{

public:

  
  //! Merges basic data (Check, ExecStatus) from another Binder but
  //! keeps its result. Used when a binder is replaced by another
  //! one, this allows to keep messages
  Standard_EXPORT void Merge (const Handle(Transfer_Binder)& other);
  
  //! Returns True if a Binder has several results, either by itself
  //! or because it has next results
  //! Can be defined by sub-classes.
  Standard_EXPORT virtual Standard_Boolean IsMultiple() const;
  
  //! Returns the Type which characterizes the Result (if known)
  Standard_EXPORT virtual Handle(Standard_Type) ResultType() const = 0;
  
  //! Returns the Name of the Type which characterizes the Result
  //! Can be returned even if ResultType itself is unknown
  Standard_EXPORT virtual Standard_CString ResultTypeName() const = 0;
  
  //! Adds a next result (at the end of the list)
  //! Remark : this information is not processed by Merge
  Standard_EXPORT void AddResult (const Handle(Transfer_Binder)& next);
  
  //! Returns the next result, Null if none
  Standard_EXPORT Handle(Transfer_Binder) NextResult() const;
  
  //! Returns True if a Result is available (StatusResult = Defined)
  //! A Unique Result will be gotten by Result (which must be
  //! defined in each sub-class according to result type)
  //! For a Multiple Result, see class MultipleBinder
  //! For other case, specific access has to be forecast
  Standard_EXPORT Standard_Boolean HasResult() const;
  
  //! Declares that result is now used by another one, it means that
  //! it cannot be modified (by Rebind)
  Standard_EXPORT void SetAlreadyUsed();
  
  //! Returns status, which can be Initial (not yet done), Made (a
  //! result is recorded, not yet shared), Used (it is shared and
  //! cannot be modified)
  Standard_EXPORT Transfer_StatusResult Status() const;
  
  //! Returns execution status
  Standard_EXPORT Transfer_StatusExec StatusExec() const;
  
  //! Modifies execution status; called by TransferProcess only
  //! (for StatusError, rather use SetError, below)
  Standard_EXPORT void SetStatusExec (const Transfer_StatusExec stat);
  
  //! Used to declare an individual transfer as being erroneous
  //! (Status is set to Void, StatusExec is set to Error, <errmess>
  //! is added to Check's list of Fails)
  //! It is possible to record several messages of error
  //!
  //! It has same effect for TransferProcess as raising an exception
  //! during the operation of Transfer, except the Transfer tries to
  //! continue (as if ErrorHandle had been set)
  Standard_EXPORT void AddFail (const Standard_CString mess, const Standard_CString orig = "");
  
  //! Used to attach a Warning Message to an individual Transfer
  //! It has no effect on the Status
  Standard_EXPORT void AddWarning (const Standard_CString mess, const Standard_CString orig = "");
  
  //! Returns Check which stores Fail messages
  //! Note that no Entity is associated in this Check
  Standard_EXPORT const Handle(Interface_Check) Check() const;
  
  //! Returns Check which stores Fail messages, in order to modify
  //! it (adding messages, or replacing it)
  Standard_EXPORT Handle(Interface_Check) CCheck();

  //! Destructor
  Standard_EXPORT ~Transfer_Binder();

  DEFINE_STANDARD_RTTIEXT(Transfer_Binder,Standard_Transient)

protected:

  
  //! Sets fields at initial values
  Standard_EXPORT Transfer_Binder();
  
  //! Used to declare that a result is recorded for an individual
  //! transfer (works by setting StatusResult to Defined)
  //!
  //! This Method is to be called once a Result is really recorded
  //! (see sub-classes of Binder, especially SimpleBinder) : it is
  //! senseless if called isolately
  Standard_EXPORT void SetResultPresent();



private:

  
  //! Called by AddResult, to keep unicity of each item in the list
  Standard_EXPORT void CutResult (const Handle(Transfer_Binder)& next);

  Transfer_StatusResult thestatus;
  Transfer_StatusExec theexecst;
  Handle(Interface_Check) thecheck;
  Handle(Transfer_Binder) thenextr;
  Handle(Transfer_Binder) theendr;


};







#endif // _Transfer_Binder_HeaderFile
