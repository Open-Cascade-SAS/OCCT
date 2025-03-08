// Created on: 1999-07-19
// Created by: Denis PASCAL
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TFunction_Logbook_HeaderFile
#define _TFunction_Logbook_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_LabelMap.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class Standard_GUID;
class TFunction_Logbook;
class TDF_RelocationTable;

DEFINE_STANDARD_HANDLE(TFunction_Logbook, TDF_Attribute)

//! This class contains information which is written and
//! read during the solving process. Information is divided
//! in three groups.
//!
//! * Touched Labels  (modified by the end user),
//! * Impacted Labels (modified during execution of the function),
//! * Valid Labels    (within the valid label scope).
class TFunction_Logbook : public TDF_Attribute
{
public:
  //! Finds or Creates a TFunction_Logbook attribute at the root label accessed by <Access>.
  //! Returns the attribute.
  Standard_EXPORT static Handle(TFunction_Logbook) Set(const TDF_Label& Access);

  //! Returns the GUID for logbook attribute.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! The methods manipulating the data
  //! (touched, impacted and valid labels)
  //  ====================================

  //! Constructor (empty).
  Standard_EXPORT TFunction_Logbook();

  //! Clears this logbook to its default, empty state.
  Standard_EXPORT void             Clear();
  Standard_EXPORT Standard_Boolean IsEmpty() const;

  //! Sets the label L as a touched label in this logbook.
  //! In other words, L is understood to have been modified by the end user.
  void SetTouched(const TDF_Label& L);

  //! Sets the label L as an impacted label in this logbook.
  //! This method is called by execution of the function driver.
  Standard_EXPORT void SetImpacted(const TDF_Label&       L,
                                   const Standard_Boolean WithChildren = Standard_False);

  //! Sets the label L as a valid label in this logbook.
  Standard_EXPORT void SetValid(const TDF_Label&       L,
                                const Standard_Boolean WithChildren = Standard_False);
  Standard_EXPORT void SetValid(const TDF_LabelMap& Ls);

  //! Returns True if the label L is touched  or impacted. This method
  //! is called by <TFunction_FunctionDriver::MustExecute>.
  //! If <WithChildren> is set to true, the method checks
  //! all the sublabels of <L> too.
  Standard_EXPORT Standard_Boolean
    IsModified(const TDF_Label& L, const Standard_Boolean WithChildren = Standard_False) const;

  //! Returns the map of touched labels in this logbook.
  //! A touched label is the one modified by the end user.
  const TDF_LabelMap& GetTouched() const;

  //! Returns the map of impacted labels contained in this logbook.
  const TDF_LabelMap& GetImpacted() const;

  //! Returns the map of valid labels in this logbook.
  const TDF_LabelMap&  GetValid() const;
  Standard_EXPORT void GetValid(TDF_LabelMap& Ls) const;

  //! Sets status of execution.
  void Done(const Standard_Boolean status);

  //! Returns status of execution.
  Standard_Boolean IsDone() const;

  //! The methods inherited from TDF_Attribute
  //  ========================================

  //! Returns the ID of the attribute.
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  //! Undos (and redos) the attribute.
  Standard_EXPORT virtual void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

  //! Pastes the attribute to another label.
  Standard_EXPORT virtual void Paste(const Handle(TDF_Attribute)&       into,
                                     const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  //! Returns a new empty instance of the attribute.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Prints th data of the attributes (touched, impacted and valid labels).
  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

private:
  TDF_LabelMap     myTouched;
  TDF_LabelMap     myImpacted;
  TDF_LabelMap     myValid;
  Standard_Boolean isDone;
};

#include <TFunction_Logbook.lxx>

#endif // _TFunction_Logbook_HeaderFile
