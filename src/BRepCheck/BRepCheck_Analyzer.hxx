// Created on: 1995-12-08
// Created by: Jacques GOUSSARD
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

#ifndef _BRepCheck_Analyzer_HeaderFile
#define _BRepCheck_Analyzer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <BRepCheck_DataMapOfShapeResult.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class Standard_NullObject;
class Standard_NoSuchObject;
class TopoDS_Shape;
class BRepCheck_Result;


//! A framework to check the overall
//! validity of a shape. For a shape to be valid in Open
//! CASCADE, it - or its component subshapes - must respect certain
//! criteria. These criteria are checked by the function IsValid.
//! Once you have determined whether a shape is valid or not, you can
//! diagnose its specific anomalies and correct them using the services of
//! the ShapeAnalysis, ShapeUpgrade, and ShapeFix packages.
class BRepCheck_Analyzer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a shape validation object defined by the shape S.
  //! <S> is the  shape  to control.  <GeomControls>  If
  //! False   only topological informaions  are checked.
  //! The geometricals controls are
  //! For a Vertex :
  //! BRepCheck_InvalidToleranceValue  NYI
  //! For an Edge :
  //! BRepCheck_InvalidCurveOnClosedSurface,
  //! BRepCheck_InvalidCurveOnSurface,
  //! BRepCheck_InvalidSameParameterFlag,
  //! BRepCheck_InvalidToleranceValue  NYI
  //! For a face :
  //! BRepCheck_UnorientableShape,
  //! BRepCheck_IntersectingWires,
  //! BRepCheck_InvalidToleranceValue  NYI
  //! For a wire :
  //! BRepCheck_SelfIntersectingWire
    BRepCheck_Analyzer(const TopoDS_Shape& S, const Standard_Boolean GeomControls = Standard_True);
  
  //! <S> is the  shape  to control.  <GeomControls>  If
  //! False   only topological informaions  are checked.
  //! The geometricals controls are
  //! For a Vertex :
  //! BRepCheck_InvalidTolerance  NYI
  //! For an Edge :
  //! BRepCheck_InvalidCurveOnClosedSurface,
  //! BRepCheck_InvalidCurveOnSurface,
  //! BRepCheck_InvalidSameParameterFlag,
  //! BRepCheck_InvalidTolerance  NYI
  //! For a face :
  //! BRepCheck_UnorientableShape,
  //! BRepCheck_IntersectingWires,
  //! BRepCheck_InvalidTolerance  NYI
  //! For a wire :
  //! BRepCheck_SelfIntersectingWire
  Standard_EXPORT void Init (const TopoDS_Shape& S, const Standard_Boolean GeomControls = Standard_True);
  
  //! <S> is a  subshape of the  original shape. Returns
  //! <STandard_True> if no default has been detected on
  //! <S> and any of its subshape.
  Standard_EXPORT Standard_Boolean IsValid (const TopoDS_Shape& S) const;
  
  //! Returns true if no defect is
  //! detected on the shape S or any of its subshapes.
  //! Returns true if the shape S is valid.
  //! This function checks whether a given shape is valid by checking that:
  //! -      the topology is correct
  //! -      parameterization of edges in particular is correct.
  //! For the topology to be correct, the following conditions must be satisfied:
  //! -      edges should have at least two vertices if they are not
  //! degenerate edges. The vertices should be within the range of
  //! the bounding edges at the tolerance specified in the vertex,
  //! -      edges should share at least one face. The representation of
  //! the edges should be within the tolerance criterion assigned to them.
  //! -      wires defining a face should not self-intersect and should be closed,
  //! - there should be one wire which contains all other wires inside a face,
  //! -      wires should be correctly oriented with respect to each of the edges,
  //! -      faces should be correctly oriented, in particular with
  //! respect to adjacent faces if these faces define a solid,
  //! -      shells defining a solid should be closed. There should
  //! be one enclosing shell if the shape is a solid;
  //! To check parameterization of edge, there are 2 approaches depending on
  //! the edge?s contextual situation.
  //! -      if the edge is either single, or it is in the context
  //! of a wire or a compound, its parameterization is defined by
  //! the parameterization of its 3D curve and is considered as    valid.
  //! -      If the edge is in the context of a face, it should
  //! have SameParameter and SameRange flags set to Standard_True. To
  //! check these flags, you should call the function
  //! BRep_Tool::SameParameter and BRep_Tool::SameRange for an
  //! edge. If at least one of these flags is set to Standard_False,
  //! the edge is considered as invalid without any additional check.
  //! If the edge is contained by a face, and it has SameParameter and
  //! SameRange flags set to Standard_True, IsValid checks
  //! whether representation of the edge on face, in context of which the
  //! edge is considered, has the same parameterization up to the
  //! tolerance value coded on the edge. For a given parameter t on the edge
  //! having C as a 3D curve and one PCurve P on a surface S (base
  //! surface of the reference face), this checks that |C(t) - S(P(t))|
  //! is less than or equal to tolerance, where tolerance is the tolerance
  //! value coded on the edge.
    Standard_Boolean IsValid() const;
  
    const Handle(BRepCheck_Result)& Result (const TopoDS_Shape& SubS) const;




protected:





private:

  
  Standard_EXPORT void Put (const TopoDS_Shape& S, const Standard_Boolean Gctrl);
  
  Standard_EXPORT void Perform (const TopoDS_Shape& S);
  
  Standard_EXPORT Standard_Boolean ValidSub (const TopoDS_Shape& S, const TopAbs_ShapeEnum SubType) const;


  TopoDS_Shape myShape;
  BRepCheck_DataMapOfShapeResult myMap;


};


#include <BRepCheck_Analyzer.lxx>





#endif // _BRepCheck_Analyzer_HeaderFile
