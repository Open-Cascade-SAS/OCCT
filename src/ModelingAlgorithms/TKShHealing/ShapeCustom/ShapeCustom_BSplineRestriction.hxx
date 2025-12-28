// Created on: 1999-06-18
// Created by: Galina Koulikova
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

#ifndef _ShapeCustom_BSplineRestriction_HeaderFile
#define _ShapeCustom_BSplineRestriction_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <ShapeCustom_Modification.hxx>
class ShapeCustom_RestrictionParameters;
class TopoDS_Face;
class Geom_Surface;
class TopLoc_Location;
class TopoDS_Edge;
class Geom_Curve;
class Geom2d_Curve;
class TopoDS_Vertex;
class gp_Pnt;

//! this tool intended for approximation surfaces, curves and pcurves with
//! specified degree , max number of segments, tolerance 2d, tolerance 3d. Specified
//! continuity can be reduced if approximation with specified continuity was not done.
class ShapeCustom_BSplineRestriction : public ShapeCustom_Modification
{

public:
  //! Empty constructor.
  Standard_EXPORT ShapeCustom_BSplineRestriction();

  //! Initializes with specified parameters of approximation.
  Standard_EXPORT ShapeCustom_BSplineRestriction(const bool          anApproxSurfaceFlag,
                                                 const bool          anApproxCurve3dFlag,
                                                 const bool          anApproxCurve2dFlag,
                                                 const double        aTol3d,
                                                 const double        aTol2d,
                                                 const GeomAbs_Shape aContinuity3d,
                                                 const GeomAbs_Shape aContinuity2d,
                                                 const int           aMaxDegree,
                                                 const int           aNbMaxSeg,
                                                 const bool          Degree,
                                                 const bool          Rational);

  //! Initializes with specified parameters of approximation.
  Standard_EXPORT ShapeCustom_BSplineRestriction(
    const bool                                            anApproxSurfaceFlag,
    const bool                                            anApproxCurve3dFlag,
    const bool                                            anApproxCurve2dFlag,
    const double                                          aTol3d,
    const double                                          aTol2d,
    const GeomAbs_Shape                                   aContinuity3d,
    const GeomAbs_Shape                                   aContinuity2d,
    const int                                             aMaxDegree,
    const int                                             aNbMaxSeg,
    const bool                                            Degree,
    const bool                                            Rational,
    const occ::handle<ShapeCustom_RestrictionParameters>& aModes);

  //! Returns true if the face <F> has been
  //! modified. In this case, <S> is the new geometric
  //! support of the face, <L> the new location,
  //! <Tol> the new tolerance. <RevWires> has to be set to
  //! true when the modification reverses the
  //! normal of the surface. (the wires have to be
  //! reversed). <RevFace> has to be set to
  //! true if the orientation of the modified
  //! face changes in the shells which contain it.
  //!
  //! Otherwise, returns false, and <S>, <L>,
  //! <Tol>, <RevWires>, <RevFace> are not significant.
  Standard_EXPORT bool NewSurface(const TopoDS_Face&         F,
                                  occ::handle<Geom_Surface>& S,
                                  TopLoc_Location&           L,
                                  double&                    Tol,
                                  bool&                      RevWires,
                                  bool&                      RevFace) override;

  //! Returns true if curve from the edge <E> has been
  //! modified. In this case, <C> is the new geometric
  //! support of the edge, <L> the new location, <Tol>
  //! the new tolerance.
  //! Otherwise, returns true if Surface is modified or
  //! one of pcurves of edge is modified. In this case C is copy of
  //! geometric support of the edge.
  //! In other cases returns false, and <C>, <L>, <Tol> are not
  //! significant.
  Standard_EXPORT bool NewCurve(const TopoDS_Edge&       E,
                                occ::handle<Geom_Curve>& C,
                                TopLoc_Location&         L,
                                double&                  Tol) override;

  //! Returns true if the edge <E> has been modified.
  //! In this case,if curve on the surface is modified, <C>
  //! is the new geometric support of the edge, <L> the
  //! new location, <Tol> the new tolerance. If curve on the surface
  //! is not modified C is copy curve on surface from the edge <E>.
  //!
  //! Otherwise, returns false, and <C>, <L>,
  //! <Tol> are not significant.
  //!
  //! <NewE> is the new edge created from <E>. <NewF>
  //! is the new face created from <F>. They may be useful.
  Standard_EXPORT bool NewCurve2d(const TopoDS_Edge&         E,
                                  const TopoDS_Face&         F,
                                  const TopoDS_Edge&         NewE,
                                  const TopoDS_Face&         NewF,
                                  occ::handle<Geom2d_Curve>& C,
                                  double&                    Tol) override;

  //! Returns true if the surface has been modified.
  //! if flag IsOf equals true Offset surfaces are approximated to Offset
  //! if false to BSpline
  Standard_EXPORT bool ConvertSurface(const occ::handle<Geom_Surface>& aSurface,
                                      occ::handle<Geom_Surface>&       S,
                                      const double                     UF,
                                      const double                     UL,
                                      const double                     VF,
                                      const double                     VL,
                                      const bool                       IsOf = true);

  //! Returns true if the curve has been modified.
  //! if flag IsOf equals true Offset curves are approximated to Offset
  //! if false to BSpline
  Standard_EXPORT bool ConvertCurve(const occ::handle<Geom_Curve>& aCurve,
                                    occ::handle<Geom_Curve>&       C,
                                    const bool                     IsConvert,
                                    const double                   First,
                                    const double                   Last,
                                    double&                        TolCur,
                                    const bool                     IsOf = true);

  //! Returns true if the pcurve has been modified.
  //! if flag IsOf equals true Offset pcurves are approximated to Offset
  //! if false to BSpline
  Standard_EXPORT bool ConvertCurve2d(const occ::handle<Geom2d_Curve>& aCurve,
                                      occ::handle<Geom2d_Curve>&       C,
                                      const bool                       IsConvert,
                                      const double                     First,
                                      const double                     Last,
                                      double&                          TolCur,
                                      const bool                       IsOf = true);

  //! Sets tolerance of approximation for curve3d and surface
  void SetTol3d(const double Tol3d);

  //! Sets tolerance of approximation for curve2d
  void SetTol2d(const double Tol2d);

  //! Returns (modifiable) the flag which defines whether the
  //! surface is approximated.
  bool& ModifyApproxSurfaceFlag();

  //! Returns (modifiable) the flag which defines whether the
  //! curve3d is approximated.
  bool& ModifyApproxCurve3dFlag();

  //! Returns (modifiable) the flag which defines whether the curve2d is approximated.
  bool& ModifyApproxCurve2dFlag();

  //! Sets continuity3d for approximation curve3d and surface.
  void SetContinuity3d(const GeomAbs_Shape Continuity3d);

  //! Sets continuity3d for approximation curve2d.
  void SetContinuity2d(const GeomAbs_Shape Continuity2d);

  //! Sets max degree for approximation.
  void SetMaxDegree(const int MaxDegree);

  //! Sets max number of segments for approximation.
  void SetMaxNbSegments(const int MaxNbSegments);

  //! Sets priority for approximation curves and surface.
  //! If Degree is True approximation is made with degree less
  //! then specified MaxDegree at the expense of number of spanes.
  //! If Degree is False approximation is made with number of
  //! spans less then specified MaxNbSegment at the expense of
  //! specified MaxDegree.
  void SetPriority(const bool Degree);

  //! Sets flag for define if rational BSpline or Bezier is
  //! converted to polynomial. If Rational is True approximation
  //! for rational BSpline and Bezier is made to polynomial even
  //! if degree is less then MaxDegree and number of spans is less
  //! then specified MaxNbSegment.
  void SetConvRational(const bool Rational);

  //! Returns the container of modes which defines
  //! what geometry should be converted to BSplines.
  occ::handle<ShapeCustom_RestrictionParameters> GetRestrictionParameters() const;

  //! Sets the container of modes which defines
  //! what geometry should be converted to BSplines.
  void SetRestrictionParameters(const occ::handle<ShapeCustom_RestrictionParameters>& aModes);

  //! Returns error for approximation curve3d.
  double Curve3dError() const;

  //! Returns error for approximation curve2d.
  double Curve2dError() const;

  //! Returns error for approximation surface.
  double SurfaceError() const;

  Standard_EXPORT bool NewPoint(const TopoDS_Vertex& V, gp_Pnt& P, double& Tol) override;

  Standard_EXPORT bool NewParameter(const TopoDS_Vertex& V,
                                    const TopoDS_Edge&   E,
                                    double&              P,
                                    double&              Tol) override;

  Standard_EXPORT GeomAbs_Shape Continuity(const TopoDS_Edge& E,
                                           const TopoDS_Face& F1,
                                           const TopoDS_Face& F2,
                                           const TopoDS_Edge& NewE,
                                           const TopoDS_Face& NewF1,
                                           const TopoDS_Face& NewF2) override;

  //! Returns error for approximation surface, curve3d and curve2d.
  Standard_EXPORT double MaxErrors(double& aCurve3dErr, double& aCurve2dErr) const;

  //! Returns number for approximation surface, curve3d and curve2d.
  Standard_EXPORT int NbOfSpan() const;

  DEFINE_STANDARD_RTTIEXT(ShapeCustom_BSplineRestriction, ShapeCustom_Modification)

private:
  GeomAbs_Shape                                  myContinuity3d;
  GeomAbs_Shape                                  myContinuity2d;
  int                                            myMaxDegree;
  int                                            myNbMaxSeg;
  double                                         myTol3d;
  double                                         myTol2d;
  double                                         mySurfaceError;
  double                                         myCurve3dError;
  double                                         myCurve2dError;
  int                                            myNbOfSpan;
  bool                                           myApproxSurfaceFlag;
  bool                                           myApproxCurve3dFlag;
  bool                                           myApproxCurve2dFlag;
  bool                                           myDeg;
  bool                                           myConvert;
  bool                                           myRational;
  occ::handle<ShapeCustom_RestrictionParameters> myParameters;
};

#include <ShapeCustom_BSplineRestriction.lxx>

#endif // _ShapeCustom_BSplineRestriction_HeaderFile
