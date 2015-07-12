// Created on: 1993-07-07
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

#ifndef _TopOpeBRep_ShapeScanner_HeaderFile
#define _TopOpeBRep_ShapeScanner_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepTool_BoxSort.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class TopoDS_Shape;
class TopOpeBRepTool_ShapeExplorer;
class TopOpeBRepTool_BoxSort;


//! Find, among the  subshapes SS of a reference shape
//! RS, the ones which 3D box interfers with the box of
//! a shape S (SS and S are of the same type).
class TopOpeBRep_ShapeScanner 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRep_ShapeScanner();
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT void AddBoxesMakeCOB (const TopoDS_Shape& S, const TopAbs_ShapeEnum TS, const TopAbs_ShapeEnum TA = TopAbs_SHAPE);
  
  Standard_EXPORT void Init (const TopoDS_Shape& E);
  
  Standard_EXPORT void Init (TopOpeBRepTool_ShapeExplorer& X);
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const TopoDS_Shape& Current() const;
  
  Standard_EXPORT const TopOpeBRepTool_BoxSort& BoxSort() const;
  
  Standard_EXPORT TopOpeBRepTool_BoxSort& ChangeBoxSort();
  
  Standard_EXPORT Standard_Integer Index() const;
  
  Standard_EXPORT Standard_OStream& DumpCurrent (Standard_OStream& OS) const;




protected:





private:



  TopOpeBRepTool_BoxSort myBoxSort;
  TColStd_ListIteratorOfListOfInteger myListIterator;


};







#endif // _TopOpeBRep_ShapeScanner_HeaderFile
