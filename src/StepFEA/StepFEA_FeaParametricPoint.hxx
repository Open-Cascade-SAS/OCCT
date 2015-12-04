// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_FeaParametricPoint_HeaderFile
#define _StepFEA_FeaParametricPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <StepGeom_Point.hxx>
class TCollection_HAsciiString;


class StepFEA_FeaParametricPoint;
DEFINE_STANDARD_HANDLE(StepFEA_FeaParametricPoint, StepGeom_Point)

//! Representation of STEP entity FeaParametricPoint
class StepFEA_FeaParametricPoint : public StepGeom_Point
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_FeaParametricPoint();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aRepresentationItem_Name, const Handle(TColStd_HArray1OfReal)& aCoordinates);
  
  //! Returns field Coordinates
  Standard_EXPORT Handle(TColStd_HArray1OfReal) Coordinates() const;
  
  //! Set field Coordinates
  Standard_EXPORT void SetCoordinates (const Handle(TColStd_HArray1OfReal)& Coordinates);




  DEFINE_STANDARD_RTTIEXT(StepFEA_FeaParametricPoint,StepGeom_Point)

protected:




private:


  Handle(TColStd_HArray1OfReal) theCoordinates;


};







#endif // _StepFEA_FeaParametricPoint_HeaderFile
