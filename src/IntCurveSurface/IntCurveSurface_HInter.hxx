// Created on: 1993-04-07
// Created by: Laurent BUCHARD
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

#ifndef _IntCurveSurface_HInter_HeaderFile
#define _IntCurveSurface_HInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntCurveSurface_Intersection.hxx>
#include <Standard_Real.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
class Adaptor3d_HCurve;
class IntCurveSurface_TheHCurveTool;
class Adaptor3d_HSurface;
class Adaptor3d_HSurfaceTool;
class IntCurveSurface_ThePolygonOfHInter;
class IntCurveSurface_ThePolygonToolOfHInter;
class IntCurveSurface_ThePolyhedronOfHInter;
class IntCurveSurface_ThePolyhedronToolOfHInter;
class IntCurveSurface_TheInterferenceOfHInter;
class IntCurveSurface_TheCSFunctionOfHInter;
class IntCurveSurface_TheExactHInter;
class IntCurveSurface_TheQuadCurvExactHInter;
class IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter;
class Bnd_BoundSortBox;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Parab;
class gp_Hypr;
class IntAna_IntConicQuad;
class Bnd_Box;



class IntCurveSurface_HInter  : public IntCurveSurface_Intersection
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty Constructor
  Standard_EXPORT IntCurveSurface_HInter();
  
  //! Compute the Intersection between the curve and the
  //! surface
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface);
  
  //! Compute the Intersection  between the curve  and
  //! the surface. The   Curve is already  sampled and
  //! its polygon : <Polygon> is given.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HCurve)& Curve, const IntCurveSurface_ThePolygonOfHInter& Polygon, const Handle(Adaptor3d_HSurface)& Surface);
  
  //! Compute the Intersection  between the curve  and
  //! the surface. The   Curve is already  sampled and
  //! its polygon : <Polygon> is given. The Surface is
  //! also sampled and <Polyhedron> is given.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HCurve)& Curve, const IntCurveSurface_ThePolygonOfHInter& ThePolygon, const Handle(Adaptor3d_HSurface)& Surface, const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron);
  
  //! Compute the Intersection  between the curve  and
  //! the surface. The   Curve is already  sampled and
  //! its polygon : <Polygon> is given. The Surface is
  //! also sampled and <Polyhedron> is given.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HCurve)& Curve, const IntCurveSurface_ThePolygonOfHInter& ThePolygon, const Handle(Adaptor3d_HSurface)& Surface, const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron, Bnd_BoundSortBox& BndBSB);
  
  //! Compute the Intersection  between the curve  and
  //! the surface. The Surface is already  sampled and
  //! its polyhedron : <Polyhedron> is given.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface, const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron);




protected:

  
  //! Compute the Intersection between the curve and the
  //! surface
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real U0, const Standard_Real V0, const Standard_Real U1, const Standard_Real V1);
  
  Standard_EXPORT void InternalPerformCurveQuadric (const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface);
  
  Standard_EXPORT void InternalPerform (const Handle(Adaptor3d_HCurve)& Curve, const IntCurveSurface_ThePolygonOfHInter& Polygon, const Handle(Adaptor3d_HSurface)& Surface, const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  Standard_EXPORT void InternalPerform (const Handle(Adaptor3d_HCurve)& Curve, const IntCurveSurface_ThePolygonOfHInter& Polygon, const Handle(Adaptor3d_HSurface)& Surface, const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, Bnd_BoundSortBox& BSB);
  
  Standard_EXPORT void InternalPerform (const Handle(Adaptor3d_HCurve)& Curve, const IntCurveSurface_ThePolygonOfHInter& Polygon, const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  Standard_EXPORT void PerformConicSurf (const gp_Lin& Line, const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  Standard_EXPORT void PerformConicSurf (const gp_Circ& Circle, const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  Standard_EXPORT void PerformConicSurf (const gp_Elips& Ellipse, const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  Standard_EXPORT void PerformConicSurf (const gp_Parab& Parab, const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  Standard_EXPORT void PerformConicSurf (const gp_Hypr& Hyper, const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  Standard_EXPORT void AppendIntAna (const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& Surface, const IntAna_IntConicQuad& InterAna);
  
  Standard_EXPORT void AppendPoint (const Handle(Adaptor3d_HCurve)& Curve, const Standard_Real w, const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real u, const Standard_Real v);
  
  Standard_EXPORT void AppendSegment (const Handle(Adaptor3d_HCurve)& Curve, const Standard_Real u0, const Standard_Real u1, const Handle(Adaptor3d_HSurface)& Surface);




private:

  
  Standard_EXPORT void DoSurface (const Handle(Adaptor3d_HSurface)& surface, const Standard_Real u0, const Standard_Real u1, const Standard_Real v0, const Standard_Real v1, TColgp_Array2OfPnt& pntsOnSurface, Bnd_Box& boxSurface, Standard_Real& gap);
  
  Standard_EXPORT void DoNewBounds (const Handle(Adaptor3d_HSurface)& surface, const Standard_Real u0, const Standard_Real u1, const Standard_Real v0, const Standard_Real v1, const TColgp_Array2OfPnt& pntsOnSurface, const TColStd_Array1OfReal& X, const TColStd_Array1OfReal& Y, const TColStd_Array1OfReal& Z, TColStd_Array1OfReal& Bounds);




};







#endif // _IntCurveSurface_HInter_HeaderFile
