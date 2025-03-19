// Created on: 1993-02-24
// Created by: Laurent PAINNOT
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

#ifndef _AppParCurves_ConstraintCouple_HeaderFile
#define _AppParCurves_ConstraintCouple_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_Constraint.hxx>

//! associates an index and a constraint for an object.
//! This couple is used by AppDef_TheVariational when performing approximations.
class AppParCurves_ConstraintCouple
{
public:
  DEFINE_STANDARD_ALLOC

  //! returns an indefinite ConstraintCouple.
  Standard_EXPORT AppParCurves_ConstraintCouple();

  //! Create a couple the object <Index> will have the
  //! constraint <Cons>.
  Standard_EXPORT AppParCurves_ConstraintCouple(const Standard_Integer        TheIndex,
                                                const AppParCurves_Constraint Cons);

  //! returns the index of the constraint object.
  Standard_EXPORT Standard_Integer Index() const;

  //! returns the constraint of the object.
  Standard_EXPORT AppParCurves_Constraint Constraint() const;

  //! Changes the index of the constraint object.
  Standard_EXPORT void SetIndex(const Standard_Integer TheIndex);

  //! Changes the constraint of the object.
  Standard_EXPORT void SetConstraint(const AppParCurves_Constraint Cons);

protected:
private:
  Standard_Integer        myIndex;
  AppParCurves_Constraint myConstraint;
};

#endif // _AppParCurves_ConstraintCouple_HeaderFile
