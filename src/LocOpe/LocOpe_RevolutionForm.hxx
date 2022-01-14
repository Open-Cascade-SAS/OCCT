// Created on: 1997-10-20
// Created by: Olga KOULECHOVA
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

#ifndef _LocOpe_RevolutionForm_HeaderFile
#define _LocOpe_RevolutionForm_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax1.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <gp_Pnt.hxx>
#include <TopTools_ListOfShape.hxx>


//! Defines a revolution form (using Revol from BRepSweep)
//! with modifications provided for the RevolutionForm feature.
class LocOpe_RevolutionForm 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT LocOpe_RevolutionForm();
  
  Standard_EXPORT LocOpe_RevolutionForm(const TopoDS_Shape& Base, const gp_Ax1& Axe, const Standard_Real Angle);
  
  Standard_EXPORT void Perform (const TopoDS_Shape& Base, const gp_Ax1& Axe, const Standard_Real Angle);
  
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
  Standard_Real myAngle;
  gp_Ax1 myAxis;
  Standard_Real myAngTra;
  Standard_Boolean myDone;
  Standard_Boolean myIsTrans;
  TopoDS_Shape myRes;
  TopoDS_Shape myFirstShape;
  TopoDS_Shape myLastShape;
  TopTools_DataMapOfShapeListOfShape myMap;
  gp_Pnt myPnt1;
  gp_Pnt myPnt2;


};







#endif // _LocOpe_RevolutionForm_HeaderFile
