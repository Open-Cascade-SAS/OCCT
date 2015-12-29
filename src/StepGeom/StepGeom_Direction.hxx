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

#ifndef _StepGeom_Direction_HeaderFile
#define _StepGeom_Direction_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;


class StepGeom_Direction;
DEFINE_STANDARD_HANDLE(StepGeom_Direction, StepGeom_GeometricRepresentationItem)


class StepGeom_Direction : public StepGeom_GeometricRepresentationItem
{

public:

  
  //! Returns a Direction
  Standard_EXPORT StepGeom_Direction();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TColStd_HArray1OfReal)& aDirectionRatios);
  
  Standard_EXPORT void SetDirectionRatios (const Handle(TColStd_HArray1OfReal)& aDirectionRatios);
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) DirectionRatios() const;
  
  Standard_EXPORT Standard_Real DirectionRatiosValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbDirectionRatios() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_Direction,StepGeom_GeometricRepresentationItem)

protected:




private:


  Handle(TColStd_HArray1OfReal) directionRatios;


};







#endif // _StepGeom_Direction_HeaderFile
