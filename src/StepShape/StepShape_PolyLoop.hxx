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

#ifndef _StepShape_PolyLoop_HeaderFile
#define _StepShape_PolyLoop_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepShape_Loop.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepGeom_CartesianPoint;


class StepShape_PolyLoop;
DEFINE_STANDARD_HANDLE(StepShape_PolyLoop, StepShape_Loop)


class StepShape_PolyLoop : public StepShape_Loop
{

public:

  
  //! Returns a PolyLoop
  Standard_EXPORT StepShape_PolyLoop();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_HArray1OfCartesianPoint)& aPolygon);
  
  Standard_EXPORT void SetPolygon (const Handle(StepGeom_HArray1OfCartesianPoint)& aPolygon);
  
  Standard_EXPORT Handle(StepGeom_HArray1OfCartesianPoint) Polygon() const;
  
  Standard_EXPORT Handle(StepGeom_CartesianPoint) PolygonValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbPolygon() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_PolyLoop,StepShape_Loop)

protected:




private:


  Handle(StepGeom_HArray1OfCartesianPoint) polygon;


};







#endif // _StepShape_PolyLoop_HeaderFile
