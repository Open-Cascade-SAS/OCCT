// Created on: 1995-11-16
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Transfer_ResultFromModel_HeaderFile
#define _Transfer_ResultFromModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Interface_CheckStatus.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
class Interface_InterfaceModel;
class Transfer_ResultFromTransient;
class Transfer_TransientProcess;
class Interface_CheckIterator;

class Transfer_ResultFromModel;
DEFINE_STANDARD_HANDLE(Transfer_ResultFromModel, Standard_Transient)

//! ResultFromModel is used to store a final result stored in a
//! TransientProcess, respectfully to its structuration in scopes
//! by using a set of ResultFromTransient
//! Hence, it can be regarded as a passive equivalent of the
//! stored data in the TransientProcess, while an Iterator gives
//! a flat view of it.
//!
//! A ResultFromModel is intended to be attached to the transfer
//! of one entity (typically root entity but it is not mandatory)
//!
//! It is then possible to :
//! - Create and fill a ResultFromModel from a TransientProcess,
//! by designating a starting entity
//! - Fill back the TransientProcess from a ResultFromModel, as it
//! were filled by the operation which filled it the first time
class Transfer_ResultFromModel : public Standard_Transient
{

public:
  //! Creates a ResultFromModel, empty
  Standard_EXPORT Transfer_ResultFromModel();

  //! Sets starting Model
  Standard_EXPORT void SetModel(const Handle(Interface_InterfaceModel)& model);

  //! Sets starting File Name
  Standard_EXPORT void SetFileName(const Standard_CString filename);

  //! Returns starting Model (null if not set)
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;

  //! Returns starting File Name (empty if not set)
  Standard_EXPORT Standard_CString FileName() const;

  //! Fills from a TransientProcess, with the result attached to
  //! a starting entity. Considers its Model if it is set.
  //! This action produces a structured set of ResultFromTransient,
  //! considering scopes, starting by that of <ent>.
  //! If <ent> has no recorded result, it remains empty
  //! Returns True if a result is recorded, False else
  Standard_EXPORT Standard_Boolean Fill(const Handle(Transfer_TransientProcess)& TP,
                                        const Handle(Standard_Transient)&        ent);

  //! Clears some data attached to binders used by TransientProcess,
  //! which become useless once the transfer has been done,
  //! by calling Strip on its ResultFromTransient
  //!
  //! mode = 0 : minimum, clears data remaining from TransferProcess
  //! mode = 10 : just keeps file name, label, check status ...,
  //! and MainResult but only the result (Binder)
  //! mode = 11 : also clears MainResult (status and names remain)
  Standard_EXPORT void Strip(const Standard_Integer mode);

  //! Fills back a TransientProcess from the structured set of
  //! binders. Also sets the Model.
  Standard_EXPORT void FillBack(const Handle(Transfer_TransientProcess)& TP) const;

  //! Returns True if a Result is recorded
  Standard_EXPORT Standard_Boolean HasResult() const;

  //! Returns the main recorded ResultFromTransient, or a null
  Standard_EXPORT Handle(Transfer_ResultFromTransient) MainResult() const;

  //! Sets a new value for the main recorded ResultFromTransient
  Standard_EXPORT void SetMainResult(const Handle(Transfer_ResultFromTransient)& amain);

  //! Returns the label in starting model attached to main entity
  //! (updated by Fill or SetMainResult, if Model is known)
  Standard_EXPORT Standard_CString MainLabel() const;

  //! Returns the label in starting model attached to main entity
  Standard_EXPORT Standard_Integer MainNumber() const;

  //! Searches for a key (starting entity) and returns its result
  //! Returns a null handle if not found
  Standard_EXPORT Handle(Transfer_ResultFromTransient) ResultFromKey(
    const Handle(Standard_Transient)& start) const;

  //! Internal method which returns the list of ResultFromTransient,
  //! according level (2:complete; 1:sub-level 1; 0:main only)
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Results(const Standard_Integer level) const;

  //! Returns the list of recorded starting entities, ending by the
  //! root. Entities with check but no transfer result are ignored
  //! <level> = 2 (D), considers the complete list
  //! <level> = 1      considers the main result plus immediate subs
  //! <level> = 0      just the main result
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) TransferredList(
    const Standard_Integer level = 2) const;

  //! Returns the list of starting entities to which a check status
  //! is attached.
  //! <check> = -2 , all entities whatever the check (see result)
  //! <check> = -1 , entities with no fail (warning allowed)
  //! <check> =  0 , entities with no check at all
  //! <check> =  1 , entities with warning but no fail
  //! <check> =  2 , entities with fail
  //! <result> : if True, only entities with an attached result
  //! Remark : result True and check=0 will give an empty list
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) CheckedList(
    const Interface_CheckStatus check,
    const Standard_Boolean      result) const;

  //! Returns the check-list of this set of results
  //! <erronly> true : only fails are considered
  //! <level> = 0 : considers only main binder
  //! <level> = 1 : considers main binder plus immediate subs
  //! <level> = 2 (D) : considers all checks
  Standard_EXPORT Interface_CheckIterator CheckList(const Standard_Boolean erronly,
                                                    const Standard_Integer level = 2) const;

  //! Returns the check status with corresponds to the content
  //! of this ResultFromModel; considers all levels of transfer
  //! (worst status). Returns CheckAny if not yet computed
  //! Reads it from recorded status if already computed, else
  //! recomputes one
  Standard_EXPORT Interface_CheckStatus CheckStatus() const;

  //! Computes and records check status (see CheckStatus)
  //! Does not computes it if already done and <enforce> False
  Standard_EXPORT Interface_CheckStatus ComputeCheckStatus(const Standard_Boolean enforce);

  DEFINE_STANDARD_RTTIEXT(Transfer_ResultFromModel, Standard_Transient)

protected:
private:
  Handle(Interface_InterfaceModel)     themodel;
  TCollection_AsciiString              thename;
  Handle(Transfer_ResultFromTransient) themain;
  TCollection_AsciiString              themlab;
  Standard_Integer                     themnum;
  Interface_CheckStatus                themchk;
};

#endif // _Transfer_ResultFromModel_HeaderFile
