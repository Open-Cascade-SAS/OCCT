// Created on: 1996-03-18
// Created by: Stagiaire Frederic CALOONE
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _GeomPlate_BuildAveragePlane_HeaderFile
#define _GeomPlate_BuildAveragePlane_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_HArray1OfPnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_SequenceOfVec.hxx>
#include <GeomPlate_SequenceOfAij.hxx>
class Geom_Plane;
class Geom_Line;

//! This class computes an average inertial plane with an
//! array of points.
//! Computes the initial surface (average plane) in the cases
//! when the initial surface is not given.
class GeomPlate_BuildAveragePlane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Tol is a Tolerance to make the difference between
  //! the result plane and the result line.
  //! if POption = 1 : automatic parametrisation
  //! if POption = 2 : parametrisation by eigen vectors
  //! if NOption = 1 : the average plane is the inertial plane.
  //! if NOption = 2 : the average plane is the plane of max. flux.
  Standard_EXPORT GeomPlate_BuildAveragePlane(const Handle(TColgp_HArray1OfPnt)& Pts,
                                              const Standard_Integer             NbBoundPoints,
                                              const Standard_Real                Tol,
                                              const Standard_Integer             POption,
                                              const Standard_Integer             NOption);

  //! Creates the plane from the "best vector"
  Standard_EXPORT GeomPlate_BuildAveragePlane(const TColgp_SequenceOfVec&        Normals,
                                              const Handle(TColgp_HArray1OfPnt)& Pts);

  //! Return the average Plane.
  Standard_EXPORT Handle(Geom_Plane) Plane() const;

  //! Return a Line when 2 eigenvalues are null.
  Standard_EXPORT Handle(Geom_Line) Line() const;

  //! return OK if is a plane.
  Standard_EXPORT Standard_Boolean IsPlane() const;

  //! return OK if is a line.
  Standard_EXPORT Standard_Boolean IsLine() const;

  //! computes the   minimal box  to include  all normal
  //! projection points of the initial array  on the plane.
  Standard_EXPORT void MinMaxBox(Standard_Real& Umin,
                                 Standard_Real& Umax,
                                 Standard_Real& Vmin,
                                 Standard_Real& Vmax) const;

  Standard_EXPORT static Standard_Boolean HalfSpace(const TColgp_SequenceOfVec& NewNormals,
                                                    TColgp_SequenceOfVec&       Normals,
                                                    GeomPlate_SequenceOfAij&    Bset,
                                                    const Standard_Real         LinTol,
                                                    const Standard_Real         AngTol);

protected:
private:
  //! Computes a base of the average plane defined by (myG,N)
  //! using eigen vectors
  Standard_EXPORT void BasePlan(const gp_Vec& N);

  //! Defines the average plane.
  //! if NOption = 1 : the average plane is the inertial plane.
  //! if NOption = 2 : the average plane is the plane of max. flux.
  Standard_EXPORT gp_Vec DefPlan(const Standard_Integer NOption);

  Handle(TColgp_HArray1OfPnt) myPts;
  Standard_Real               myUmax;
  Standard_Real               myVmax;
  Standard_Real               myVmin;
  Standard_Real               myUmin;
  Handle(Geom_Plane)          myPlane;
  Standard_Real               myTol;
  Handle(Geom_Line)           myLine;
  gp_Vec                      myOX;
  gp_Vec                      myOY;
  gp_Pnt                      myG;
  Standard_Integer            myNbBoundPoints;
};

#endif // _GeomPlate_BuildAveragePlane_HeaderFile
