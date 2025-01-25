// Created on: 1995-10-13
// Created by: Laurent BOURESCHE
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

#ifndef _GeomFill_ConstrainedFilling_HeaderFile
#define _GeomFill_ConstrainedFilling_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <GeomFill_CornerState.hxx>
#include <gp_Vec.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray2OfPnt.hxx>
class GeomFill_CoonsAlgPatch;
class GeomFill_TgtField;
class Geom_BSplineSurface;
class GeomFill_Boundary;
class GeomFill_BoundWithSurf;

//! An algorithm for constructing a BSpline surface filled
//! from a series of boundaries which serve as path
//! constraints and optionally, as tangency constraints.
//! The algorithm accepts three or four curves as the
//! boundaries of the target surface.
//! The only FillingStyle used is Coons.
//! A ConstrainedFilling object provides a framework for:
//! -   defining the boundaries of the surface
//! -   implementing the construction algorithm
//! -   consulting the result.
//! Warning
//! This surface filling algorithm is specifically designed to
//! be used in connection with fillets. Satisfactory results
//! cannot be guaranteed for other uses.
class GeomFill_ConstrainedFilling
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty framework for filling a surface from boundaries.
  //! The boundaries of the surface will be defined, and the
  //! surface will be built by using the function Init.
  //! The surface will respect the following constraints:
  //! -   its degree will not be greater than MaxDeg
  //! -   the maximum number of segments MaxSeg which
  //! BSpline surfaces can have.
  Standard_EXPORT GeomFill_ConstrainedFilling(const Standard_Integer MaxDeg,
                                              const Standard_Integer MaxSeg);

  Standard_EXPORT void Init(const Handle(GeomFill_Boundary)& B1,
                            const Handle(GeomFill_Boundary)& B2,
                            const Handle(GeomFill_Boundary)& B3,
                            const Standard_Boolean           NoCheck = Standard_False);

  //! Constructs a BSpline surface filled from the series of
  //! boundaries B1, B2, B3 and, if need be, B4, which serve:
  //! -   as path constraints
  //! -   and optionally, as tangency constraints if they are
  //! GeomFill_BoundWithSurf curves.
  //! The boundaries may be given in any order: they are
  //! classified and if necessary, reversed and reparameterized.
  //! The surface will also respect the following constraints:
  //! -   its degree will not be greater than the maximum
  //! degree defined at the time of construction of this framework, and
  //! -   the maximum number of segments MaxSeg which BSpline surfaces can have
  Standard_EXPORT void Init(const Handle(GeomFill_Boundary)& B1,
                            const Handle(GeomFill_Boundary)& B2,
                            const Handle(GeomFill_Boundary)& B3,
                            const Handle(GeomFill_Boundary)& B4,
                            const Standard_Boolean           NoCheck = Standard_False);

  //! Allows to modify domain on witch the blending function
  //! associated to  the constrained boundary B  will propag
  //! the  influence   of the  field   of  tangency.  Can be
  //! useful to  reduce  influence of boundaries  on which
  //! the Coons compatibility  conditions are not respected.
  //! l is a  relative value of  the parametric range of  B.
  //! Default value for l is 1 (used in Init).
  //! Warning: Must be called after  Init with a constrained boundary
  //! used in the call to Init.
  Standard_EXPORT void SetDomain(const Standard_Real l, const Handle(GeomFill_BoundWithSurf)& B);

  //! Computes the  new poles  of  the surface using the  new
  //! blending  functions set by several calls to SetDomain.
  Standard_EXPORT void ReBuild();

  //! Returns the bound of index i after sort.
  Standard_EXPORT Handle(GeomFill_Boundary) Boundary(const Standard_Integer I) const;

  //! Returns the BSpline surface after computation of the fill by this framework.
  Standard_EXPORT Handle(Geom_BSplineSurface) Surface() const;

  //! Internal use for Advmath approximation call.
  Standard_EXPORT Standard_Integer Eval(const Standard_Real    W,
                                        const Standard_Integer Ord,
                                        Standard_Real&         Result) const;

  //! Computes the fields of tangents on 30 points along the
  //! bound  I, these  are  not the  constraint tangents but
  //! gives an idea of the coonsAlgPatch regularity.
  Standard_EXPORT void CheckCoonsAlgPatch(const Standard_Integer I);

  //! Computes  the fields  of tangents  and  normals on  30
  //! points along the bound  I, draw them, and computes the
  //! max dot product that must be near than 0.
  Standard_EXPORT void CheckTgteField(const Standard_Integer I);

  //! Computes  values  and normals  along  the bound  I and
  //! compare  them to the  approx  result curves (bound and
  //! tgte field) , draw  the normals and tangents.
  Standard_EXPORT void CheckApprox(const Standard_Integer I);

  //! Computes values and normals along the  bound I on both
  //! constraint  surface    and result  surface,  draw  the
  //! normals, and  computes the max distance between values
  //! and the max angle  between normals.
  Standard_EXPORT void CheckResult(const Standard_Integer I);

protected:
private:
  //! Performs the approximation an compute  the poles of the
  //! surface.
  Standard_EXPORT void Build();

  //! Performs  the  parallel approximation  on two  oppsite
  //! bounds
  Standard_EXPORT void PerformApprox();

  //! matches  the nodal vectors  of the  blending functions
  //! and the results  of the approx   to allow the  surface
  //! computation.
  Standard_EXPORT void MatchKnots();

  //! performs the poles of the partial construction S0.
  Standard_EXPORT void PerformS0();

  //! performs the poles of the partial construction S1.
  Standard_EXPORT void PerformS1();

  //! performs  the poles of  the  surface using the partial
  //! constructions S0 and S1.
  Standard_EXPORT void PerformSurface();

  //! Checks if the field of tangency doesn t twist along the
  //! boundary.
  Standard_EXPORT Standard_Boolean CheckTgte(const Standard_Integer I);

  //! Evaluates  the min magnitude  of  the field of tangency
  //! along bound  I  to allow a   simple evaluation of  the
  //! tolerance needed for the approximation of the field of
  //! tangency.
  Standard_EXPORT void MinTgte(const Standard_Integer I);

  Standard_Integer                 degmax;
  Standard_Integer                 segmax;
  Handle(GeomFill_CoonsAlgPatch)   ptch;
  Handle(GeomFill_TgtField)        tgalg[4];
  Standard_Real                    mig[4];
  GeomFill_CornerState             stcor[4];
  gp_Vec                           v[4];
  Standard_Boolean                 appdone;
  Standard_Integer                 degree[2];
  Handle(TColgp_HArray1OfPnt)      curvpol[4];
  Handle(TColgp_HArray1OfPnt)      tgtepol[4];
  Handle(TColStd_HArray1OfInteger) mults[2];
  Handle(TColStd_HArray1OfReal)    knots[2];
  Handle(TColStd_HArray1OfReal)    ab[4];
  Handle(TColStd_HArray1OfReal)    pq[4];
  Standard_Real                    dom[4];
  Handle(TColgp_HArray1OfPnt)      ncpol[4];
  Handle(TColgp_HArray1OfPnt)      ntpol[4];
  Handle(TColStd_HArray1OfInteger) nm[2];
  Handle(TColStd_HArray1OfReal)    nk[2];
  Standard_Integer                 ibound[2];
  Standard_Integer                 ctr[2];
  Standard_Integer                 nbd3;
  Handle(TColgp_HArray2OfPnt)      S0;
  Handle(TColgp_HArray2OfPnt)      S1;
  Handle(Geom_BSplineSurface)      surf;
};

#endif // _GeomFill_ConstrainedFilling_HeaderFile
