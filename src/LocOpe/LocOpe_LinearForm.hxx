// Created on: 1997-04-14
// Created by: Olga PILLOT
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _LocOpe_LinearForm_HeaderFile
#define _LocOpe_LinearForm_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <gp_Vec.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <gp_Pnt.hxx>
#include <TopTools_ListOfShape.hxx>
class Standard_NoSuchObject;
class StdFail_NotDone;
class TopoDS_Shape;
class gp_Vec;
class gp_Pnt;


//! Defines a linear form (using Prism from BRepSweep)
//! with modifications provided for the LinearForm feature.
class LocOpe_LinearForm 
{
public:

  DEFINE_STANDARD_ALLOC

  
    LocOpe_LinearForm();
  
    LocOpe_LinearForm(const TopoDS_Shape& Base, const gp_Vec& V, const gp_Pnt& Pnt1, const gp_Pnt& Pnt2);
  
    LocOpe_LinearForm(const TopoDS_Shape& Base, const gp_Vec& V, const gp_Vec& Vectra, const gp_Pnt& Pnt1, const gp_Pnt& Pnt2);
  
  Standard_EXPORT void Perform (const TopoDS_Shape& Base, const gp_Vec& V, const gp_Pnt& Pnt1, const gp_Pnt& Pnt2);
  
  Standard_EXPORT void Perform (const TopoDS_Shape& Base, const gp_Vec& V, const gp_Vec& Vectra, const gp_Pnt& Pnt1, const gp_Pnt& Pnt2);
  
  Standard_EXPORT const TopoDS_Shape& FirstShape() const;
  
  Standard_EXPORT const TopoDS_Shape& LastShape() const;
  
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  Standard_EXPORT const TopTools_ListOfShape& Shapes (const TopoDS_Shape& S) const;




protected:





private:

  
  Standard_EXPORT void IntPerf();


  TopoDS_Shape myBase;
  gp_Vec myVec;
  gp_Vec myTra;
  Standard_Boolean myDone;
  Standard_Boolean myIsTrans;
  TopoDS_Shape myRes;
  TopoDS_Shape myFirstShape;
  TopoDS_Shape myLastShape;
  TopTools_DataMapOfShapeListOfShape myMap;
  gp_Pnt myPnt1;
  gp_Pnt myPnt2;


};


#include <LocOpe_LinearForm.lxx>





#endif // _LocOpe_LinearForm_HeaderFile
