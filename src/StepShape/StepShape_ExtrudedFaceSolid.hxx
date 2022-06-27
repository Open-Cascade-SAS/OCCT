// Created on: 1999-03-11
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepShape_ExtrudedFaceSolid_HeaderFile
#define _StepShape_ExtrudedFaceSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_SweptFaceSolid.hxx>
class StepGeom_Direction;
class TCollection_HAsciiString;
class StepShape_FaceSurface;


class StepShape_ExtrudedFaceSolid;
DEFINE_STANDARD_HANDLE(StepShape_ExtrudedFaceSolid, StepShape_SweptFaceSolid)


class StepShape_ExtrudedFaceSolid : public StepShape_SweptFaceSolid
{

public:

  
  //! Returns a ExtrudedFaceSolid
  Standard_EXPORT StepShape_ExtrudedFaceSolid();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepShape_FaceSurface)& aSweptArea, const Handle(StepGeom_Direction)& aExtrudedDirection, const Standard_Real aDepth);
  
  Standard_EXPORT void SetExtrudedDirection (const Handle(StepGeom_Direction)& aExtrudedDirection);
  
  Standard_EXPORT Handle(StepGeom_Direction) ExtrudedDirection() const;
  
  Standard_EXPORT void SetDepth (const Standard_Real aDepth);
  
  Standard_EXPORT Standard_Real Depth() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_ExtrudedFaceSolid,StepShape_SweptFaceSolid)

protected:




private:


  Handle(StepGeom_Direction) extrudedDirection;
  Standard_Real depth;


};







#endif // _StepShape_ExtrudedFaceSolid_HeaderFile
