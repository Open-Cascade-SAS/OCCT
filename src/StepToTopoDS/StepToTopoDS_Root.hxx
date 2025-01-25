// Created on: 1993-07-28
// Created by: Martine LANGLOIS
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

#ifndef _StepToTopoDS_Root_HeaderFile
#define _StepToTopoDS_Root_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

//! This class implements the common services for
//! all classes of StepToTopoDS which report error
//! and sets and returns precision.
class StepToTopoDS_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_Boolean IsDone() const;

  //! Returns the value of "MyPrecision"
  Standard_Real Precision() const;

  //! Sets the value of "MyPrecision"
  void SetPrecision(const Standard_Real preci);

  //! Returns the value of "MaxTol"
  Standard_Real MaxTol() const;

  //! Sets the value of MaxTol
  void SetMaxTol(const Standard_Real maxpreci);

protected:
  Standard_EXPORT StepToTopoDS_Root();

  Standard_Boolean done;

private:
  Standard_Real myPrecision;
  Standard_Real myMaxTol;
};

#include <StepToTopoDS_Root.lxx>

#endif // _StepToTopoDS_Root_HeaderFile
