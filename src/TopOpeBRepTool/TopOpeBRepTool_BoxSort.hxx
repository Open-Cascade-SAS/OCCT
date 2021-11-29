// Created on: 1993-07-08
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

#ifndef _TopOpeBRepTool_BoxSort_HeaderFile
#define _TopOpeBRepTool_BoxSort_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRepTool_HBoxTool;



class TopOpeBRepTool_BoxSort 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepTool_BoxSort();
  
  Standard_EXPORT TopOpeBRepTool_BoxSort(const Handle(TopOpeBRepTool_HBoxTool)& T);
  
  Standard_EXPORT void SetHBoxTool (const Handle(TopOpeBRepTool_HBoxTool)& T);
  
  Standard_EXPORT const Handle(TopOpeBRepTool_HBoxTool)& HBoxTool() const;
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT void AddBoxes (const TopoDS_Shape& S, const TopAbs_ShapeEnum TS, const TopAbs_ShapeEnum TA = TopAbs_SHAPE);
  
  Standard_EXPORT void MakeHAB (const TopoDS_Shape& S, const TopAbs_ShapeEnum TS, const TopAbs_ShapeEnum TA = TopAbs_SHAPE);
  
  Standard_EXPORT const Handle(Bnd_HArray1OfBox)& HAB() const;
  
  Standard_EXPORT static void MakeHABCOB (const Handle(Bnd_HArray1OfBox)& HAB, Bnd_Box& COB);
  
  Standard_EXPORT const TopoDS_Shape& HABShape (const Standard_Integer I) const;
  
  Standard_EXPORT void MakeCOB (const TopoDS_Shape& S, const TopAbs_ShapeEnum TS, const TopAbs_ShapeEnum TA = TopAbs_SHAPE);
  
  Standard_EXPORT void AddBoxesMakeCOB (const TopoDS_Shape& S, const TopAbs_ShapeEnum TS, const TopAbs_ShapeEnum TA = TopAbs_SHAPE);
  
  Standard_EXPORT const TColStd_ListIteratorOfListOfInteger& Compare (const TopoDS_Shape& S);
  
  Standard_EXPORT const TopoDS_Shape& TouchedShape (const TColStd_ListIteratorOfListOfInteger& I) const;
  
  Standard_EXPORT const Bnd_Box& Box (const TopoDS_Shape& S) const;
  
  Standard_EXPORT ~TopOpeBRepTool_BoxSort();

private:



  Bnd_Box myCOB;
  Bnd_BoundSortBox myBSB;
  TColStd_ListIteratorOfListOfInteger myIterator;
  TopoDS_Shape myLastCompareShape;
  Bnd_Box myLastCompareShapeBox;
  Handle(TopOpeBRepTool_HBoxTool) myHBT;
  Handle(Bnd_HArray1OfBox) myHAB;
  Handle(TColStd_HArray1OfInteger) myHAI;


};







#endif // _TopOpeBRepTool_BoxSort_HeaderFile
