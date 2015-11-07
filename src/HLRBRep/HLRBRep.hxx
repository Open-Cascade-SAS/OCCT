// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRBRep_HeaderFile
#define _HLRBRep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class TopoDS_Edge;
class HLRBRep_Curve;
class HLRBRep_CurveTool;
class HLRBRep_BCurveTool;
class HLRBRep_Curve;
class HLRBRep_SurfaceTool;
class HLRBRep_BSurfaceTool;
class HLRBRep_Surface;
class HLRBRep_CLPropsATool;
class HLRBRep_CLProps;
class HLRBRep_SLPropsATool;
class HLRBRep_SLProps;
class HLRBRep_CInter;
class HLRBRep_LineTool;
class HLRBRep_InterCSurf;
class HLRBRep_EdgeFaceTool;
class HLRBRep_Intersector;
class HLRBRep_EdgeData;
class HLRBRep_FaceData;
class HLRBRep_FaceIterator;
class HLRBRep_Data;
class HLRBRep_ShapeToHLR;
class HLRBRep_HLRToShape;
class HLRBRep_ShapeBounds;
class HLRBRep_EdgeInterferenceTool;
class HLRBRep_VertexList;
class HLRBRep_EdgeIList;
class HLRBRep_AreaLimit;
class HLRBRep_EdgeBuilder;
class HLRBRep_Hider;
class HLRBRep_InternalAlgo;
class HLRBRep_Algo;
class HLRBRep_PolyAlgo;
class HLRBRep_BiPoint;
class HLRBRep_BiPnt2D;
class HLRBRep_PolyHLRToShape;
class HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter;
class HLRBRep_PCLocFOfTheLocateExtPCOfTheProjPCurOfCInter;
class HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter;
class HLRBRep_TheProjPCurOfCInter;
class HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter;
class HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter;
class HLRBRep_TheIntConicCurveOfCInter;
class HLRBRep_IntConicCurveOfCInter;
class HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter;
class HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter;
class HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter;
class HLRBRep_TheIntPCurvePCurveOfCInter;
class HLRBRep_ThePolygonOfInterCSurf;
class HLRBRep_ThePolygonToolOfInterCSurf;
class HLRBRep_ThePolyhedronOfInterCSurf;
class HLRBRep_ThePolyhedronToolOfInterCSurf;
class HLRBRep_TheInterferenceOfInterCSurf;
class HLRBRep_TheCSFunctionOfInterCSurf;
class HLRBRep_TheExactInterCSurf;
class HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf;
class HLRBRep_TheQuadCurvExactInterCSurf;


//! Hidden Lines Removal
//! algorithms on the BRep DataStructure.
//!
//! The class PolyAlgo  is used to remove Hidden lines
//! on Shapes with Triangulations.
class HLRBRep 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static TopoDS_Edge MakeEdge (const HLRBRep_Curve& ec, const Standard_Real U1, const Standard_Real U2);
  
  Standard_EXPORT static TopoDS_Edge MakeEdge3d (const HLRBRep_Curve& ec, const Standard_Real U1, const Standard_Real U2);
  
  Standard_EXPORT static void PolyHLRAngleAndDeflection (const Standard_Real InAngl, Standard_Real& OutAngl, Standard_Real& OutDefl);




protected:





private:




friend class HLRBRep_CurveTool;
friend class HLRBRep_BCurveTool;
friend class HLRBRep_Curve;
friend class HLRBRep_SurfaceTool;
friend class HLRBRep_BSurfaceTool;
friend class HLRBRep_Surface;
friend class HLRBRep_CLPropsATool;
friend class HLRBRep_CLProps;
friend class HLRBRep_SLPropsATool;
friend class HLRBRep_SLProps;
friend class HLRBRep_CInter;
friend class HLRBRep_LineTool;
friend class HLRBRep_InterCSurf;
friend class HLRBRep_EdgeFaceTool;
friend class HLRBRep_Intersector;
friend class HLRBRep_EdgeData;
friend class HLRBRep_FaceData;
friend class HLRBRep_FaceIterator;
friend class HLRBRep_Data;
friend class HLRBRep_ShapeToHLR;
friend class HLRBRep_HLRToShape;
friend class HLRBRep_ShapeBounds;
friend class HLRBRep_EdgeInterferenceTool;
friend class HLRBRep_VertexList;
friend class HLRBRep_EdgeIList;
friend class HLRBRep_AreaLimit;
friend class HLRBRep_EdgeBuilder;
friend class HLRBRep_Hider;
friend class HLRBRep_InternalAlgo;
friend class HLRBRep_Algo;
friend class HLRBRep_PolyAlgo;
friend class HLRBRep_BiPoint;
friend class HLRBRep_BiPnt2D;
friend class HLRBRep_PolyHLRToShape;
friend class HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter;
friend class HLRBRep_PCLocFOfTheLocateExtPCOfTheProjPCurOfCInter;
friend class HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter;
friend class HLRBRep_TheProjPCurOfCInter;
friend class HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter;
friend class HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter;
friend class HLRBRep_TheIntConicCurveOfCInter;
friend class HLRBRep_IntConicCurveOfCInter;
friend class HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter;
friend class HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter;
friend class HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter;
friend class HLRBRep_TheIntPCurvePCurveOfCInter;
friend class HLRBRep_ThePolygonOfInterCSurf;
friend class HLRBRep_ThePolygonToolOfInterCSurf;
friend class HLRBRep_ThePolyhedronOfInterCSurf;
friend class HLRBRep_ThePolyhedronToolOfInterCSurf;
friend class HLRBRep_TheInterferenceOfInterCSurf;
friend class HLRBRep_TheCSFunctionOfInterCSurf;
friend class HLRBRep_TheExactInterCSurf;
friend class HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf;
friend class HLRBRep_TheQuadCurvExactInterCSurf;

};







#endif // _HLRBRep_HeaderFile
