// Created on: 1995-03-22
// Created by: Laurent BUCHARD
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

#ifndef _IntTools_FClass2d_HeaderFile
#define _IntTools_FClass2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepClass_FaceExplorer.hxx>
#include <CSLib_Class2d.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_State.hxx>

#include <memory>

class gp_Pnt2d;

//! Class provides an algorithm to classify a 2d Point
//! in 2d space of face using boundaries of the face.
class IntTools_FClass2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT IntTools_FClass2d();

  //! Initializes algorithm by the face F
  //! and tolerance Tol
  Standard_EXPORT IntTools_FClass2d(const TopoDS_Face& F, const double Tol);

  //! Initializes algorithm by the face F
  //! and tolerance Tol
  Standard_EXPORT void Init(const TopoDS_Face& F, const double Tol);

  //! Returns state of infinite 2d point relatively to (0, 0)
  Standard_EXPORT TopAbs_State PerformInfinitePoint() const;

  //! Returns state of the 2d point Puv.
  //! If RecadreOnPeriodic is true (default value),
  //! for the periodic surface 2d point, adjusted to period, is
  //! classified.
  Standard_EXPORT TopAbs_State
    Perform(const gp_Pnt2d& Puv, const bool RecadreOnPeriodic = true) const;

  //! Destructor
  Standard_EXPORT ~IntTools_FClass2d();

  //! Test a point with +- an offset (Tol) and returns
  //! On if some points are OUT an some are IN
  //! (Caution: Internal use. see the code for more details)
  Standard_EXPORT TopAbs_State
    TestOnRestriction(const gp_Pnt2d&        Puv,
                      const double    Tol,
                      const bool RecadreOnPeriodic = true) const;

  Standard_EXPORT bool IsHole() const;

private:
  NCollection_Sequence<CSLib_Class2d> TabClass;
  NCollection_Sequence<int>           TabOrien;
  double                       Toluv;
  TopoDS_Face                         Face;
  double                       U1;
  double                       V1;
  double                       U2;
  double                       V2;
  double                       Umin;
  double                       Umax;
  double                       Vmin;
  double                       Vmax;
  bool                    myIsHole;

  mutable std::unique_ptr<BRepClass_FaceExplorer> myFExplorer;
};

#endif // _IntTools_FClass2d_HeaderFile
