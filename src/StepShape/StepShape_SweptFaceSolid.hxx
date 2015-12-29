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

#ifndef _StepShape_SweptFaceSolid_HeaderFile
#define _StepShape_SweptFaceSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_SolidModel.hxx>
class StepShape_FaceSurface;
class TCollection_HAsciiString;


class StepShape_SweptFaceSolid;
DEFINE_STANDARD_HANDLE(StepShape_SweptFaceSolid, StepShape_SolidModel)


class StepShape_SweptFaceSolid : public StepShape_SolidModel
{

public:

  
  //! Returns a SweptFaceSolid
  Standard_EXPORT StepShape_SweptFaceSolid();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepShape_FaceSurface)& aSweptArea);
  
  Standard_EXPORT virtual void SetSweptFace (const Handle(StepShape_FaceSurface)& aSweptArea);
  
  Standard_EXPORT virtual Handle(StepShape_FaceSurface) SweptFace() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_SweptFaceSolid,StepShape_SolidModel)

protected:




private:


  Handle(StepShape_FaceSurface) sweptArea;


};







#endif // _StepShape_SweptFaceSolid_HeaderFile
