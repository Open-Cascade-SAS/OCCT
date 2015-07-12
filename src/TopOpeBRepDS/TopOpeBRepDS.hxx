// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_HeaderFile
#define _TopOpeBRepDS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_State.hxx>
#include <Standard_OStream.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <Standard_Boolean.hxx>
class TCollection_AsciiString;
class TopOpeBRepDS_Interference;
class TopOpeBRepDS_InterferenceIterator;
class TopOpeBRepDS_TKI;
class TopOpeBRepDS_Transition;
class TopOpeBRepDS_CurvePointInterference;
class TopOpeBRepDS_SurfaceCurveInterference;
class TopOpeBRepDS_SolidSurfaceInterference;
class TopOpeBRepDS_ShapeShapeInterference;
class TopOpeBRepDS_EdgeVertexInterference;
class TopOpeBRepDS_FaceEdgeInterference;
class TopOpeBRepDS_Surface;
class TopOpeBRepDS_Curve;
class TopOpeBRepDS_Point;
class TopOpeBRepDS_GeometryData;
class TopOpeBRepDS_SurfaceData;
class TopOpeBRepDS_CurveData;
class TopOpeBRepDS_PointData;
class TopOpeBRepDS_ShapeData;
class TopOpeBRepDS_DSS;
class TopOpeBRepDS_DataStructure;
class TopOpeBRepDS_SurfaceIterator;
class TopOpeBRepDS_CurveIterator;
class TopOpeBRepDS_PointIterator;
class TopOpeBRepDS_SurfaceExplorer;
class TopOpeBRepDS_CurveExplorer;
class TopOpeBRepDS_PointExplorer;
class TopOpeBRepDS_InterferenceTool;
class TopOpeBRepDS_BuildTool;
class TopOpeBRepDS_Dumper;
class TopOpeBRepDS_Marker;
class TopOpeBRepDS_HDataStructure;
class TopOpeBRepDS_EdgeInterferenceTool;
class TopOpeBRepDS_Edge3dInterferenceTool;
class TopOpeBRepDS_FaceInterferenceTool;
class TopOpeBRepDS_Filter;
class TopOpeBRepDS_Reducer;
class TopOpeBRepDS_TOOL;
class TopOpeBRepDS_FIR;
class TopOpeBRepDS_EIR;
class TopOpeBRepDS_Check;
class TopOpeBRepDS_GapFiller;
class TopOpeBRepDS_GapTool;
class TopOpeBRepDS_Association;
class TopOpeBRepDS_ListOfShapeOn1State;
class TopOpeBRepDS_Explorer;
class TopOpeBRepDS_ShapeWithState;


//! This package provides services used by the TopOpeBRepBuild
//! package performing topological operations on the BRep
//! data structure.
class TopOpeBRepDS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! IN OU ON UN
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopAbs_State S);
  
  Standard_EXPORT static Standard_OStream& Print (const TopAbs_State S, Standard_OStream& OS);
  
  //! <K>
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopOpeBRepDS_Kind K);
  
  //! S1(<K>,<I>)S2
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopOpeBRepDS_Kind K, const Standard_Integer I, const TCollection_AsciiString& B = "", const TCollection_AsciiString& A = "");
  
  Standard_EXPORT static Standard_OStream& Print (const TopOpeBRepDS_Kind K, Standard_OStream& S);
  
  Standard_EXPORT static Standard_OStream& Print (const TopOpeBRepDS_Kind K, const Standard_Integer I, Standard_OStream& S, const TCollection_AsciiString& B = "", const TCollection_AsciiString& A = "");
  
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopAbs_ShapeEnum T);
  
  //! (<T>,<I>)
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopAbs_ShapeEnum T, const Standard_Integer I);
  
  Standard_EXPORT static Standard_OStream& Print (const TopAbs_ShapeEnum T, const Standard_Integer I, Standard_OStream& S);
  
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopAbs_Orientation O);
  
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopOpeBRepDS_Config C);
  
  Standard_EXPORT static Standard_OStream& Print (const TopOpeBRepDS_Config C, Standard_OStream& S);
  
  Standard_EXPORT static Standard_Boolean IsGeometry (const TopOpeBRepDS_Kind K);
  
  Standard_EXPORT static Standard_Boolean IsTopology (const TopOpeBRepDS_Kind K);
  
  Standard_EXPORT static TopAbs_ShapeEnum KindToShape (const TopOpeBRepDS_Kind K);
  
  Standard_EXPORT static TopOpeBRepDS_Kind ShapeToKind (const TopAbs_ShapeEnum S);




protected:





private:




friend class TopOpeBRepDS_Interference;
friend class TopOpeBRepDS_InterferenceIterator;
friend class TopOpeBRepDS_TKI;
friend class TopOpeBRepDS_Transition;
friend class TopOpeBRepDS_CurvePointInterference;
friend class TopOpeBRepDS_SurfaceCurveInterference;
friend class TopOpeBRepDS_SolidSurfaceInterference;
friend class TopOpeBRepDS_ShapeShapeInterference;
friend class TopOpeBRepDS_EdgeVertexInterference;
friend class TopOpeBRepDS_FaceEdgeInterference;
friend class TopOpeBRepDS_Surface;
friend class TopOpeBRepDS_Curve;
friend class TopOpeBRepDS_Point;
friend class TopOpeBRepDS_GeometryData;
friend class TopOpeBRepDS_SurfaceData;
friend class TopOpeBRepDS_CurveData;
friend class TopOpeBRepDS_PointData;
friend class TopOpeBRepDS_ShapeData;
friend class TopOpeBRepDS_DSS;
friend class TopOpeBRepDS_DataStructure;
friend class TopOpeBRepDS_SurfaceIterator;
friend class TopOpeBRepDS_CurveIterator;
friend class TopOpeBRepDS_PointIterator;
friend class TopOpeBRepDS_SurfaceExplorer;
friend class TopOpeBRepDS_CurveExplorer;
friend class TopOpeBRepDS_PointExplorer;
friend class TopOpeBRepDS_InterferenceTool;
friend class TopOpeBRepDS_BuildTool;
friend class TopOpeBRepDS_Dumper;
friend class TopOpeBRepDS_Marker;
friend class TopOpeBRepDS_HDataStructure;
friend class TopOpeBRepDS_EdgeInterferenceTool;
friend class TopOpeBRepDS_Edge3dInterferenceTool;
friend class TopOpeBRepDS_FaceInterferenceTool;
friend class TopOpeBRepDS_Filter;
friend class TopOpeBRepDS_Reducer;
friend class TopOpeBRepDS_TOOL;
friend class TopOpeBRepDS_FIR;
friend class TopOpeBRepDS_EIR;
friend class TopOpeBRepDS_Check;
friend class TopOpeBRepDS_GapFiller;
friend class TopOpeBRepDS_GapTool;
friend class TopOpeBRepDS_Association;
friend class TopOpeBRepDS_ListOfShapeOn1State;
friend class TopOpeBRepDS_Explorer;
friend class TopOpeBRepDS_ShapeWithState;

};







#endif // _TopOpeBRepDS_HeaderFile
