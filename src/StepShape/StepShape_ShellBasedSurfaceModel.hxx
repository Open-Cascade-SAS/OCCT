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

#ifndef _StepShape_ShellBasedSurfaceModel_HeaderFile
#define _StepShape_ShellBasedSurfaceModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_HArray1OfShell.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepShape_Shell;


class StepShape_ShellBasedSurfaceModel;
DEFINE_STANDARD_HANDLE(StepShape_ShellBasedSurfaceModel, StepGeom_GeometricRepresentationItem)


class StepShape_ShellBasedSurfaceModel : public StepGeom_GeometricRepresentationItem
{

public:

  
  //! Returns a ShellBasedSurfaceModel
  Standard_EXPORT StepShape_ShellBasedSurfaceModel();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepShape_HArray1OfShell)& aSbsmBoundary);
  
  Standard_EXPORT void SetSbsmBoundary (const Handle(StepShape_HArray1OfShell)& aSbsmBoundary);
  
  Standard_EXPORT Handle(StepShape_HArray1OfShell) SbsmBoundary() const;
  
  Standard_EXPORT StepShape_Shell SbsmBoundaryValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbSbsmBoundary() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_ShellBasedSurfaceModel,StepGeom_GeometricRepresentationItem)

protected:




private:


  Handle(StepShape_HArray1OfShell) sbsmBoundary;


};







#endif // _StepShape_ShellBasedSurfaceModel_HeaderFile
