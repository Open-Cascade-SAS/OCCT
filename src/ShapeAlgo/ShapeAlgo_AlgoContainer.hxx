// Created on: 2000-02-07
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _ShapeAlgo_AlgoContainer_HeaderFile
#define _ShapeAlgo_AlgoContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <TColGeom_HSequenceOfBoundedCurve.hxx>
#include <TColGeom2d_HSequenceOfBoundedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class ShapeAlgo_ToolContainer;
class ShapeAnalysis_Wire;
class ShapeExtend_WireData;
class Geom_BSplineCurve;
class Geom2d_BSplineCurve;
class TopoDS_Shape;
class Geom_BSplineSurface;
class Geom_Surface;
class TopoDS_Wire;
class TopoDS_Face;
class Geom_Curve;


class ShapeAlgo_AlgoContainer;
DEFINE_STANDARD_HANDLE(ShapeAlgo_AlgoContainer, Standard_Transient)


class ShapeAlgo_AlgoContainer : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT ShapeAlgo_AlgoContainer();
  
  //! Sets ToolContainer
    void SetToolContainer (const Handle(ShapeAlgo_ToolContainer)& TC);
  
  //! Returns ToolContainer
    Handle(ShapeAlgo_ToolContainer) ToolContainer() const;
  
  //! Finds the best way to connect and connects <nextsewd> to already
  //! built <sewd> (in <saw>).
  //! Returns False if <nextsewd> cannot be connected, otherwise - True.
  //! <maxtol> specifies the maximum tolerance with which <nextsewd> can
  //! be added.
  //! <distmin> is used to receive the minimum distance between <nextsewd>
  //! and <sewd>.
  //! <revsewd>   is True if <sewd>     has been reversed before connecting.
  //! <revnextwd> is True if <nextsewd> has been reversed before connecting.
  //! Uses functionality of ShapeAnalysis_Wire.
  Standard_EXPORT virtual Standard_Boolean ConnectNextWire (const Handle(ShapeAnalysis_Wire)& saw, const Handle(ShapeExtend_WireData)& nextsewd, const Standard_Real maxtol, Standard_Real& distmin, Standard_Boolean& revsewd, Standard_Boolean& revnextsewd) const;
  
  Standard_EXPORT virtual void ApproxBSplineCurve (const Handle(Geom_BSplineCurve)& bspline, TColGeom_SequenceOfCurve& seq) const;
  
  Standard_EXPORT virtual void ApproxBSplineCurve (const Handle(Geom2d_BSplineCurve)& bspline, TColGeom2d_SequenceOfCurve& seq) const;
  
  Standard_EXPORT virtual Standard_Boolean C0BSplineToSequenceOfC1BSplineCurve (const Handle(Geom_BSplineCurve)& BS, Handle(TColGeom_HSequenceOfBoundedCurve)& seqBS) const;
  
  //! Converts C0 B-Spline curve into sequence of C1 B-Spline curves.
  //! Calls ShapeUpgrade::C0BSplineToSequenceOfC1BSplineCurve.
  Standard_EXPORT virtual Standard_Boolean C0BSplineToSequenceOfC1BSplineCurve (const Handle(Geom2d_BSplineCurve)& BS, Handle(TColGeom2d_HSequenceOfBoundedCurve)& seqBS) const;
  
  //! Converts a shape on C0 geometry into the shape on C1 geometry.
  Standard_EXPORT virtual TopoDS_Shape C0ShapeToC1Shape (const TopoDS_Shape& shape, const Standard_Real tol) const;
  
  //! Converts a surface to B-Spline.
  //! Uses ShapeConstruct.
  Standard_EXPORT virtual Handle(Geom_BSplineSurface) ConvertSurfaceToBSpline (const Handle(Geom_Surface)& surf, const Standard_Real UF, const Standard_Real UL, const Standard_Real VF, const Standard_Real VL) const;
  
  //! Return 2 wires with the same number of edges. The both Edges
  //! number i of these wires have got the same ratio between
  //! theirs parameter lengths and their wire parameter lengths.
  Standard_EXPORT virtual Standard_Boolean HomoWires (const TopoDS_Wire& wireIn1, const TopoDS_Wire& wireIn2, TopoDS_Wire& wireOut1, TopoDS_Wire& wireOut2, const Standard_Boolean byParam) const;
  
  //! Returns the outer wire on the face <Face>.
  Standard_EXPORT virtual TopoDS_Wire OuterWire (const TopoDS_Face& face) const;
  
  //! Converts surface to periodic form.
  //! Calls ShapeCustom_Surface.
  Standard_EXPORT virtual Handle(Geom_Surface) ConvertToPeriodic (const Handle(Geom_Surface)& surf) const;
  
  //! Computes exact UV bounds of all wires on the face
  Standard_EXPORT virtual void GetFaceUVBounds (const TopoDS_Face& F, Standard_Real& Umin, Standard_Real& Umax, Standard_Real& Vmin, Standard_Real& Vmax) const;
  
  //! Convert Geom_Curve to Geom_BSplineCurve
  Standard_EXPORT virtual Handle(Geom_BSplineCurve) ConvertCurveToBSpline (const Handle(Geom_Curve)& C3D, const Standard_Real First, const Standard_Real Last, const Standard_Real Tol3d, const GeomAbs_Shape Continuity, const Standard_Integer MaxSegments, const Standard_Integer MaxDegree) const;




  DEFINE_STANDARD_RTTIEXT(ShapeAlgo_AlgoContainer,Standard_Transient)

protected:




private:


  Handle(ShapeAlgo_ToolContainer) myTC;


};


#include <ShapeAlgo_AlgoContainer.lxx>





#endif // _ShapeAlgo_AlgoContainer_HeaderFile
