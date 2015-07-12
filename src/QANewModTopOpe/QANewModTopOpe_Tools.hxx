// Created on: 2003-05-06
// Created by: Michael KLOKOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _QANewModTopOpe_Tools_HeaderFile
#define _QANewModTopOpe_Tools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <BOPAlgo_PPaveFiller.hxx>
#include <Standard_Boolean.hxx>
#include <BOPAlgo_PBuilder.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
class TopoDS_Shape;
class TopoDS_Edge;


//! to  provide  several  tools  for  porting  to  OCC  5.0  (mkk)
class QANewModTopOpe_Tools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Integer NbPoints (const BOPAlgo_PPaveFiller& theDSFiller);
  
  Standard_EXPORT static TopoDS_Shape NewVertex (const BOPAlgo_PPaveFiller& theDSFiller, const Standard_Integer theIndex);
  
  Standard_EXPORT static Standard_Boolean HasSameDomain (const BOPAlgo_PBuilder& theBuilder, const TopoDS_Shape& theFace);
  
  Standard_EXPORT static void SameDomain (const BOPAlgo_PBuilder& theBuilder, const TopoDS_Shape& theFace, TopTools_ListOfShape& theResultList);
  
  Standard_EXPORT static Standard_Boolean IsSplit (const BOPAlgo_PPaveFiller& theDSFiller, const TopoDS_Shape& theEdge, const TopAbs_State theState);
  
  Standard_EXPORT static void Splits (const BOPAlgo_PPaveFiller& theDSFiller, const TopoDS_Shape& theEdge, const TopAbs_State theState, TopTools_ListOfShape& theResultList);
  
  Standard_EXPORT static Standard_Boolean SplitE (const TopoDS_Edge& theEdge, TopTools_ListOfShape& theSplits);
  
  Standard_EXPORT static Standard_Boolean EdgeCurveAncestors (const BOPAlgo_PPaveFiller& theDSFiller, const TopoDS_Shape& theEdge, TopoDS_Shape& theFace1, TopoDS_Shape& theFace2);
  
  Standard_EXPORT static Standard_Boolean EdgeSectionAncestors (const BOPAlgo_PPaveFiller& theDSFiller, const TopoDS_Shape& theEdge, TopTools_ListOfShape& LF1, TopTools_ListOfShape& LF2, TopTools_ListOfShape& LE1, TopTools_ListOfShape& LE2);
  
  Standard_EXPORT static Standard_Boolean BoolOpe (const TopoDS_Shape& theFace1, const TopoDS_Shape& theFace2, Standard_Boolean& IsCommonFound, TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap);




protected:





private:





};







#endif // _QANewModTopOpe_Tools_HeaderFile
