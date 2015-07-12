// Created on: 1998-06-03
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeCustom_HeaderFile
#define _ShapeCustom_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_DataMapOfShapeShape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Shape;
class BRepTools_Modification;
class BRepTools_Modifier;
class Message_ProgressIndicator;
class ShapeBuild_ReShape;
class ShapeCustom_RestrictionParameters;
class ShapeCustom_Surface;
class ShapeCustom_Curve;
class ShapeCustom_Curve2d;
class ShapeCustom_RestrictionParameters;
class ShapeCustom_Modification;
class ShapeCustom_DirectModification;
class ShapeCustom_TrsfModification;
class ShapeCustom_BSplineRestriction;
class ShapeCustom_ConvertToRevolution;
class ShapeCustom_SweptToElementary;
class ShapeCustom_ConvertToBSpline;


//! This package is intended to
//! convert geometrical objects and topological. The
//! modifications of one geometrical object to another
//! (one) geometrical object are provided. The supported
//! modifications are the following:
//! conversion of BSpline and Bezier surfaces to analytical form,
//! conversion of indirect elementary surfaces (with left-handed
//! coordinate systems) into  direct ones,
//! conversion of elementary surfaces to surfaces of revolution,
//! conversion of surface of linear extrusion, revolution, offset
//! surface to bspline,
//! modification of parameterization, degree, number of segments of bspline
//! surfaces,  scale the shape.
class ShapeCustom 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Applies modifier to shape and checks sharing in the case assemblies.
  Standard_EXPORT static TopoDS_Shape ApplyModifier (const TopoDS_Shape& S, const Handle(BRepTools_Modification)& M, TopTools_DataMapOfShapeShape& context, BRepTools_Modifier& MD, const Handle(Message_ProgressIndicator)& aProgress = NULL, const Handle(ShapeBuild_ReShape)& aReShape = NULL);
  
  //! Returns a new shape without indirect surfaces.
  Standard_EXPORT static TopoDS_Shape DirectFaces (const TopoDS_Shape& S);
  
  //! Returns a new shape which is scaled original
  Standard_EXPORT static TopoDS_Shape ScaleShape (const TopoDS_Shape& S, const Standard_Real scale);
  
  //! Returns a new shape with all surfaces, curves and pcurves
  //! which type is BSpline/Bezier or based on them converted
  //! having Degree less than <MaxDegree> or number of spans less
  //! than <NbMaxSegment> in dependence on parameter priority <Degree>.
  //! <GmaxDegree> and <GMaxSegments> are maximum possible degree
  //! and number of spans correspondingly.
  //! These values will be used in those cases when approximation with
  //! specified parameters is impossible and one of GmaxDegree or
  //! GMaxSegments is selected in dependence on priority.
  //! Note that even if approximation is impossible with <GMaxDegree>
  //! then number of spans can exceed specified <GMaxSegment>
  //! <Rational> specifies if to convert Rational BSpline/Bezier into
  //! polynomial B-Spline.
  //! If flags ConvOffSurf,ConvOffCurve3d,ConvOffCurve2d are Standard_True there are means
  //! that Offset surfaces , Offset curves 3d and Offset curves 2d are converted to BSPline
  //! correspondingly.
  Standard_EXPORT static TopoDS_Shape BSplineRestriction (const TopoDS_Shape& S, const Standard_Real Tol3d, const Standard_Real Tol2d, const Standard_Integer MaxDegree, const Standard_Integer MaxNbSegment, const GeomAbs_Shape Continuity3d, const GeomAbs_Shape Continuity2d, const Standard_Boolean Degree, const Standard_Boolean Rational, const Handle(ShapeCustom_RestrictionParameters)& aParameters);
  
  //! Returns a new shape with all elementary periodic surfaces converted
  //! to Geom_SurfaceOfRevolution
  Standard_EXPORT static TopoDS_Shape ConvertToRevolution (const TopoDS_Shape& S);
  
  //! Returns a new shape with all surfaces of revolution and linear extrusion
  //! convert to elementary periodic surfaces
  Standard_EXPORT static TopoDS_Shape SweptToElementary (const TopoDS_Shape& S);
  
  //! Returns a new shape with all surfaces of linear extrusion, revolution,
  //! offset, and planar surfaces converted according to flags to
  //! Geom_BSplineSurface (with same parameterisation).
  Standard_EXPORT static TopoDS_Shape ConvertToBSpline (const TopoDS_Shape& S, const Standard_Boolean extrMode, const Standard_Boolean revolMode, const Standard_Boolean offsetMode, const Standard_Boolean planeMode = Standard_False);




protected:





private:




friend class ShapeCustom_Surface;
friend class ShapeCustom_Curve;
friend class ShapeCustom_Curve2d;
friend class ShapeCustom_RestrictionParameters;
friend class ShapeCustom_Modification;
friend class ShapeCustom_DirectModification;
friend class ShapeCustom_TrsfModification;
friend class ShapeCustom_BSplineRestriction;
friend class ShapeCustom_ConvertToRevolution;
friend class ShapeCustom_SweptToElementary;
friend class ShapeCustom_ConvertToBSpline;

};







#endif // _ShapeCustom_HeaderFile
