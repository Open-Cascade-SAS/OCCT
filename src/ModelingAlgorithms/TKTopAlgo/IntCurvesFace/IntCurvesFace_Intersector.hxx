// Created on: 1996-05-31
// Created by: Laurent BUCHARD
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

#ifndef _IntCurvesFace_Intersector_HeaderFile
#define _IntCurvesFace_Intersector_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <NCollection_Sequence.hxx>
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Face.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>
#include <TopAbs_State.hxx>

#include <memory>

class BRepTopAdaptor_TopolTool;
class gp_Lin;
class gp_Pnt;
class IntCurveSurface_HInter;
class gp_Pnt2d;
class Bnd_Box;

class IntCurvesFace_Intersector : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(IntCurvesFace_Intersector, Standard_Transient)

public:
  DEFINE_STANDARD_ALLOC

  //! Load a Face.
  //!
  //! The Tolerance <Tol> is used to determine if the
  //! first point of the segment is near the face. In
  //! that case, the parameter of the intersection point
  //! on the line can be a negative value (greater than -Tol).
  //! If aRestr = true UV bounding box of face is used to restrict
  //! it's underlined surface,
  //! otherwise surface is not restricted.
  //! If UseBToler = false then the 2d-point of intersection is classified with null-tolerance
  //! (relative to face);
  //! otherwise it's using maximum between input tolerance(aTol) and tolerances of face bounds
  //! (edges).
  Standard_EXPORT IntCurvesFace_Intersector(const TopoDS_Face& F,
                                            const double       aTol,
                                            const bool         aRestr    = true,
                                            const bool         UseBToler = true);

  //! Perform the intersection between the
  //! segment L and the loaded face.
  //!
  //! PInf is the smallest parameter on the line
  //! PSup is the highest parameter on the line
  //!
  //! For an infinite line PInf and PSup can be
  //! +/- RealLast.
  Standard_EXPORT void Perform(const gp_Lin& L, const double PInf, const double PSup);

  //! same method for a HCurve from Adaptor3d.
  //! PInf an PSup can also be - and + INF.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>& HCu,
                               const double                        PInf,
                               const double                        PSup);

  //! Return the surface type
  Standard_EXPORT GeomAbs_SurfaceType SurfaceType() const;

  //! True is returned when the intersection have been computed.
  bool IsDone() const;

  int NbPnt() const;

  //! Returns the U parameter of the ith intersection point
  //! on the surface.
  double UParameter(const int I) const;

  //! Returns the V parameter of the ith intersection point
  //! on the surface.
  double VParameter(const int I) const;

  //! Returns the parameter of the ith intersection point
  //! on the line.
  double WParameter(const int I) const;

  //! Returns the geometric point of the ith intersection
  //! between the line and the surface.
  const gp_Pnt& Pnt(const int I) const;

  //! Returns the ith transition of the line on the surface.
  IntCurveSurface_TransitionOnCurve Transition(const int I) const;

  //! Returns the ith state of the point on the face.
  //! The values can be either TopAbs_IN
  //! ( the point is in the face)
  //! or TopAbs_ON
  //! ( the point is on a boundary of the face).
  TopAbs_State State(const int I) const;

  //! Returns true if curve is parallel or belongs face surface
  //! This case is recognized only for some pairs
  //! of analytical curves and surfaces (plane - line, ...)
  bool IsParallel() const;

  //! Returns the significant face used to determine
  //! the intersection.
  const TopoDS_Face& Face() const;

  Standard_EXPORT TopAbs_State ClassifyUVPoint(const gp_Pnt2d& Puv) const;

  Standard_EXPORT Bnd_Box Bounding() const;

  //! Sets the boundary tolerance flag
  Standard_EXPORT void SetUseBoundToler(bool UseBToler);

  //! Returns the boundary tolerance flag
  Standard_EXPORT bool GetUseBoundToler() const;

  Standard_EXPORT ~IntCurvesFace_Intersector() override;

private:
  Standard_EXPORT void InternalCall(const IntCurveSurface_HInter& HICS,
                                    const double                  pinf,
                                    const double                  psup);

  occ::handle<BRepTopAdaptor_TopolTool>                   myTopolTool;
  occ::handle<BRepAdaptor_Surface>                        Hsurface;
  double                                                  Tol;
  NCollection_Sequence<IntCurveSurface_IntersectionPoint> SeqPnt;
  NCollection_Sequence<int>                               mySeqState;
  bool                                                    done;
  bool                                                    myReady;
  int                                                     nbpnt;
  TopoDS_Face                                             face;
  std::unique_ptr<IntCurveSurface_ThePolyhedronOfHInter>  myPolyhedron;
  std::unique_ptr<Bnd_BoundSortBox>                       myBndBounding;
  bool                                                    myUseBoundTol;
  bool myIsParallel; // Curve is "parallel" face surface
                     // This case is recognized only for some pairs
                     // of analytical curves and surfaces (plane - line, ...)
};

#include <IntCurvesFace_Intersector.lxx>

#endif // _IntCurvesFace_Intersector_HeaderFile
