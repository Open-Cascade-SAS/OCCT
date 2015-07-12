// Created on: 1995-03-17
// Created by: Dieter THIEMANN
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

#ifndef _TopoDSToStep_MakeGeometricCurveSet_HeaderFile
#define _TopoDSToStep_MakeGeometricCurveSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDSToStep_Root.hxx>
class StepShape_GeometricCurveSet;
class StdFail_NotDone;
class TopoDS_Shape;
class Transfer_FinderProcess;


//! This class implements the mapping between a Shape
//! from TopoDS and a GeometricCurveSet from StepShape in order
//! to create a GeometricallyBoundedWireframeRepresentation.
class TopoDSToStep_MakeGeometricCurveSet  : public TopoDSToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopoDSToStep_MakeGeometricCurveSet(const TopoDS_Shape& SH, const Handle(Transfer_FinderProcess)& FP);
  
  Standard_EXPORT const Handle(StepShape_GeometricCurveSet)& Value() const;




protected:





private:



  Handle(StepShape_GeometricCurveSet) theGeometricCurveSet;


};







#endif // _TopoDSToStep_MakeGeometricCurveSet_HeaderFile
