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

#ifndef _StepShape_EdgeCurve_HeaderFile
#define _StepShape_EdgeCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepShape_Edge.hxx>
class StepGeom_Curve;
class TCollection_HAsciiString;
class StepShape_Vertex;


class StepShape_EdgeCurve;
DEFINE_STANDARD_HANDLE(StepShape_EdgeCurve, StepShape_Edge)


class StepShape_EdgeCurve : public StepShape_Edge
{

public:

  
  //! Returns a EdgeCurve
  Standard_EXPORT StepShape_EdgeCurve();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepShape_Vertex)& aEdgeStart, const Handle(StepShape_Vertex)& aEdgeEnd, const Handle(StepGeom_Curve)& aEdgeGeometry, const Standard_Boolean aSameSense);
  
  Standard_EXPORT void SetEdgeGeometry (const Handle(StepGeom_Curve)& aEdgeGeometry);
  
  Standard_EXPORT Handle(StepGeom_Curve) EdgeGeometry() const;
  
  Standard_EXPORT void SetSameSense (const Standard_Boolean aSameSense);
  
  Standard_EXPORT Standard_Boolean SameSense() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_EdgeCurve,StepShape_Edge)

protected:




private:


  Handle(StepGeom_Curve) edgeGeometry;
  Standard_Boolean sameSense;


};







#endif // _StepShape_EdgeCurve_HeaderFile
