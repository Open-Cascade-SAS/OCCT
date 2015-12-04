// Created on: 1993-04-07
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

#ifndef _Transfer_MultipleBinder_HeaderFile
#define _Transfer_MultipleBinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_Binder.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
class Transfer_TransferFailure;
class Standard_OutOfRange;
class Standard_Transient;


class Transfer_MultipleBinder;
DEFINE_STANDARD_HANDLE(Transfer_MultipleBinder, Transfer_Binder)

//! Allows direct binding between a starting Object and the Result
//! of its transfer, when it can be made of several Transient
//! Objects. Compared to a Transcriptor, it has no Transfer Action
//!
//! Result is a list of Transient Results. Unique Result is not
//! available : SetResult is redefined to start the list on the
//! first call, and refuse the other times.
//!
//! rr
//!
//! Remark : MultipleBinder itself is intended to be created and
//! filled by TransferProcess itself (method Bind). In particular,
//! conflicts between Unique (Standard) result and Multiple result
//! are avoided through management made by TransferProcess.
//!
//! Also, a Transcriptor (with an effective Transfer Method) which
//! can produce a Multiple Result, may be defined as a sub-class
//! of MultipleBinder by redefining method Transfer.
class Transfer_MultipleBinder : public Transfer_Binder
{

public:

  
  //! normal standard constructor, creates an empty MultipleBinder
  Standard_EXPORT Transfer_MultipleBinder();
  
  //! Returns True if a starting object is bound with SEVERAL
  //! results : Here, returns allways True
  Standard_EXPORT virtual Standard_Boolean IsMultiple() const Standard_OVERRIDE;
  
  //! Returns the Type permitted for Results, i.e. here Transient
  Standard_EXPORT Handle(Standard_Type) ResultType() const Standard_OVERRIDE;
  
  //! Returns the Name of the Type which characterizes the Result
  //! Here, returns "(list)"
  Standard_EXPORT Standard_CString ResultTypeName() const Standard_OVERRIDE;
  
  //! Adds a new Item to the Multiple Result
  Standard_EXPORT void AddResult (const Handle(Standard_Transient)& res);
  
  //! Returns the actual count of recorded (Transient) results
  Standard_EXPORT Standard_Integer NbResults() const;
  
  //! Returns the value of the recorded result n0 <num>
  Standard_EXPORT Handle(Standard_Transient) ResultValue (const Standard_Integer num) const;
  
  //! Returns the Multiple Result, if it is defined (at least one
  //! Item). Else, returns a Null Handle
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) MultipleResult() const;
  
  //! Defines a Binding with a Multiple Result, given as a Sequence
  //! Error if a Unique Result has yet been defined
  Standard_EXPORT void SetMultipleResult (const Handle(TColStd_HSequenceOfTransient)& mulres);




  DEFINE_STANDARD_RTTIEXT(Transfer_MultipleBinder,Transfer_Binder)

protected:




private:


  Handle(TColStd_HSequenceOfTransient) themulres;


};







#endif // _Transfer_MultipleBinder_HeaderFile
