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


class ShapeCustom_BSplineRestriction;
DEFINE_STANDARD_HANDLE(ShapeCustom_BSplineRestriction, ShapeCustom_Modification)

//! this tool intended for approximation surfaces, curves and pcurves with
//! specified degree , max number of segments, tolerance 2d, tolerance 3d. Specified
//! continuity can be reduced if approximation with specified continuity was not done.
class ShapeCustom_BSplineRestriction : public ShapeCustom_Modification
{

public:

  
  //! Empty constructor.
  Standard_EXPORT ShapeCustom_BSplineRestriction();
  
  //! Initializes with specified parameters of approximation.
  Standard_EXPORT ShapeCustom_BSplineRestriction(const Standard_Boolean anApproxSurfaceFlag, const Standard_Boolean anApproxCurve3dFlag, const Standard_Boolean anApproxCurve2dFlag, const Standard_Real aTol3d, const Standard_Real aTol2d, const GeomAbs_Shape aContinuity3d, const GeomAbs_Shape aContinuity2d, const Standard_Integer aMaxDegree, const Standard_Integer aNbMaxSeg, const Standard_Boolean Degree, const Standard_Boolean Rational);
  
  //! Initializes with specified parameters of approximation.
  Standard_EXPORT ShapeCustom_BSplineRestriction(const Standard_Boolean anApproxSurfaceFlag, const Standard_Boolean anApproxCurve3dFlag, const Standard_Boolean anApproxCurve2dFlag, const Standard_Real aTol3d, const Standard_Real aTol2d, const GeomAbs_Shape aContinuity3d, const GeomAbs_Shape aContinuity2d, const Standard_Integer aMaxDegree, const Standard_Integer aNbMaxSeg, const Standard_Boolean Degree, const Standard_Boolean Rational, const Handle(ShapeCustom_RestrictionParameters)& aModes);
  
  //! Returns Standard_True if  the  face <F> has   been
  //! modified.  In this  case, <S> is the new geometric
  //! support of  the  face, <L> the new  location,<Tol>
  //! the new   tolerance.<RevWires>  has to be   set to
  //! Standard_True when  the modification reverses  the
  //! normal of  the   surface.(the wires  have   to  be
  //! reversed).  <RevFace>    has   to   be   set    to
  //! Standard_True if  the  orientation of the modified
  //! face changes in the shells which contain it.
  //!
  //! Otherwise, returns Standard_False, and <S>,   <L>,
  //! <Tol> , <RevWires> ,<RevFace> are not  significant.
  Standard_EXPORT Standard_Boolean NewSurface (const TopoDS_Face& F, Handle(Geom_Surface)& S, TopLoc_Location& L, Standard_Real& Tol, Standard_Boolean& RevWires, Standard_Boolean& RevFace) Standard_OVERRIDE;
  
  //! Returns Standard_True  if  curve from the edge <E> has  been
  //! modified.  In this case,  <C> is the new geometric
  //! support of the  edge, <L> the  new location, <Tol>
  //! the         new    tolerance.
  //! Otherwise, returns Standard_True if Surface is modified or
  //! one of pcurves of edge is modified. In this case C is copy of
  //! geometric support of the edge.
  //! In other cases returns Standard_False, and  <C>,  <L>,  <Tol> are not
  //! significant.
  Standard_EXPORT Standard_Boolean NewCurve (const TopoDS_Edge& E, Handle(Geom_Curve)& C, TopLoc_Location& L, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns Standard_True if  the edge  <E> has been modified.
  //! In this case,if curve on the surface is modified, <C>
  //! is the new geometric support of  the edge, <L> the
  //! new location, <Tol> the new tolerance. If curve on the surface
  //! is not modified C is copy curve on surface from the edge <E>.
  //!
  //! Otherwise, returns  Standard_False, and <C>,  <L>,
  //! <Tol> are not significant.
  //!
  //! <NewE> is the new  edge created from  <E>.  <NewF>
  //! is the new face created from <F>. They may be useful.
  Standard_EXPORT Standard_Boolean NewCurve2d (const TopoDS_Edge& E, const TopoDS_Face& F, const TopoDS_Edge& NewE, const TopoDS_Face& NewF, Handle(Geom2d_Curve)& C, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns Standard_True if  the surface has been modified.
  //! if flag IsOf equals Standard_True Offset surfaces are approximated to Offset
  //! if Standard_False to BSpline
  Standard_EXPORT Standard_Boolean ConvertSurface (const Handle(Geom_Surface)& aSurface, Handle(Geom_Surface)& S, const Standard_Real UF, const Standard_Real UL, const Standard_Real VF, const Standard_Real VL, const Standard_Boolean IsOf = Standard_True);
  
  //! Returns Standard_True if  the curve has been modified.
  //! if flag IsOf equals Standard_True Offset curves are approximated to Offset
  //! if Standard_False to BSpline
  Standard_EXPORT Standard_Boolean ConvertCurve (const Handle(Geom_Curve)& aCurve, Handle(Geom_Curve)& C, const Standard_Boolean IsConvert, const Standard_Real First, const Standard_Real Last, Standard_Real& TolCur, const Standard_Boolean IsOf = Standard_True);
  
  //! Returns Standard_True if the pcurve has been modified.
  //! if flag IsOf equals Standard_True Offset pcurves are approximated to Offset
  //! if Standard_False to BSpline
  Standard_EXPORT Standard_Boolean ConvertCurve2d (const Handle(Geom2d_Curve)& aCurve, Handle(Geom2d_Curve)& C, const Standard_Boolean IsConvert, const Standard_Real First, const Standard_Real Last, Standard_Real& TolCur, const Standard_Boolean IsOf = Standard_True);
  
  //! Sets tolerance of approximation for curve3d and surface
    void SetTol3d (const Standard_Real Tol3d);
  
  //! Sets tolerance of approximation for curve2d
    void SetTol2d (const Standard_Real Tol2d);
  
  //! Returns (modifiable) the flag which defines whether the
  //! surface is approximated.
    Standard_Boolean& ModifyApproxSurfaceFlag();
  
  //! Returns (modifiable) the flag which defines whether the
  //! curve3d is approximated.
    Standard_Boolean& ModifyApproxCurve3dFlag();
  
  //! Returns (modifiable) the flag which defines whether the curve2d is approximated.
    Standard_Boolean& ModifyApproxCurve2dFlag();
  
  //! Sets continuity3d for approximation curve3d and surface.
    void SetContinuity3d (const GeomAbs_Shape Continuity3d);
  
  //! Sets continuity3d for approximation curve2d.
    void SetContinuity2d (const GeomAbs_Shape Continuity2d);
  
  //! Sets max degree for approximation.
    void SetMaxDegree (const Standard_Integer MaxDegree);
  
  //! Sets max number of segments for approximation.
    void SetMaxNbSegments (const Standard_Integer MaxNbSegments);
  
  //! Sets priority  for approximation curves and surface.
  //! If Degree is True approximation is made with degree less
  //! then specified MaxDegree at the expense of number of spanes.
  //! If Degree is False approximation is made with number of
  //! spans less then specified MaxNbSegment at the expense of
  //! specified MaxDegree.
    void SetPriority (const Standard_Boolean Degree);
  
  //! Sets flag for define if rational BSpline or Bezier is
  //! converted to polynomial. If Rational is True approximation
  //! for rational BSpline and Bezier is made to polynomial even
  //! if degree is less then MaxDegree and number of spans is less
  //! then specified MaxNbSegment.
    void SetConvRational (const Standard_Boolean Rational);
  
  //! Returns the container of modes which defines
  //! what geometry should be converted to BSplines.
    Handle(ShapeCustom_RestrictionParameters) GetRestrictionParameters() const;
  
  //! Sets the container of modes which defines
  //! what geometry should be converted to BSplines.
    void SetRestrictionParameters (const Handle(ShapeCustom_RestrictionParameters)& aModes);
  
  //! Returns error for approximation curve3d.
    Standard_Real Curve3dError() const;
  
  //! Returns error for approximation curve2d.
    Standard_Real Curve2dError() const;
  
  //! Returns error for approximation surface.
    Standard_Real SurfaceError() const;
  
  Standard_EXPORT Standard_Boolean NewPoint (const TopoDS_Vertex& V, gp_Pnt& P, Standard_Real& Tol) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean NewParameter (const TopoDS_Vertex& V, const TopoDS_Edge& E, Standard_Real& P, Standard_Real& Tol) Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape Continuity (const TopoDS_Edge& E, const TopoDS_Face& F1, const TopoDS_Face& F2, const TopoDS_Edge& NewE, const TopoDS_Face& NewF1, const TopoDS_Face& NewF2) Standard_OVERRIDE;
  
  //! Returns error for approximation surface, curve3d and curve2d.
  Standard_EXPORT Standard_Real MaxErrors (Standard_Real& aCurve3dErr, Standard_Real& aCurve2dErr) const;
  
  //! Returns number for approximation surface, curve3d and curve2d.
  Standard_EXPORT Standard_Integer NbOfSpan() const;




  DEFINE_STANDARD_RTTIEXT(ShapeCustom_BSplineRestriction,ShapeCustom_Modification)

protected:




private:


  GeomAbs_Shape myContinuity3d;
  GeomAbs_Shape myContinuity2d;
  Standard_Integer myMaxDegree;
  Standard_Integer myNbMaxSeg;
  Standard_Real myTol3d;
  Standard_Real myTol2d;
  Standard_Real mySurfaceError;
  Standard_Real myCurve3dError;
  Standard_Real myCurve2dError;
  Standard_Integer myNbOfSpan;
  Standard_Boolean myApproxSurfaceFlag;
  Standard_Boolean myApproxCurve3dFlag;
  Standard_Boolean myApproxCurve2dFlag;
  Standard_Boolean myDeg;
  Standard_Boolean myConvert;
  Standard_Boolean myRational;
  Handle(ShapeCustom_RestrictionParameters) myParameters;


};


#include <ShapeCustom_BSplineRestriction.lxx>





#endif // _ShapeCustom_BSplineRestriction_HeaderFile
