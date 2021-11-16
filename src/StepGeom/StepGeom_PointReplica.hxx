// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepGeom_PointReplica_HeaderFile
#define _StepGeom_PointReplica_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_Point.hxx>
class StepGeom_CartesianTransformationOperator;
class TCollection_HAsciiString;


class StepGeom_PointReplica;
DEFINE_STANDARD_HANDLE(StepGeom_PointReplica, StepGeom_Point)


class StepGeom_PointReplica : public StepGeom_Point
{

public:

  
  //! Returns a PointReplica
  Standard_EXPORT StepGeom_PointReplica();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Point)& aParentPt, const Handle(StepGeom_CartesianTransformationOperator)& aTransformation);
  
  Standard_EXPORT void SetParentPt (const Handle(StepGeom_Point)& aParentPt);
  
  Standard_EXPORT Handle(StepGeom_Point) ParentPt() const;
  
  Standard_EXPORT void SetTransformation (const Handle(StepGeom_CartesianTransformationOperator)& aTransformation);
  
  Standard_EXPORT Handle(StepGeom_CartesianTransformationOperator) Transformation() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_PointReplica,StepGeom_Point)

protected:




private:


  Handle(StepGeom_Point) parentPt;
  Handle(StepGeom_CartesianTransformationOperator) transformation;


};







#endif // _StepGeom_PointReplica_HeaderFile
