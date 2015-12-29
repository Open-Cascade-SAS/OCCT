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

#ifndef _StepShape_OrientedClosedShell_HeaderFile
#define _StepShape_OrientedClosedShell_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_HArray1OfFace.hxx>
#include <Standard_Integer.hxx>
class StepShape_ClosedShell;
class TCollection_HAsciiString;
class StepShape_Face;


class StepShape_OrientedClosedShell;
DEFINE_STANDARD_HANDLE(StepShape_OrientedClosedShell, StepShape_ClosedShell)


class StepShape_OrientedClosedShell : public StepShape_ClosedShell
{

public:

  
  //! Returns a OrientedClosedShell
  Standard_EXPORT StepShape_OrientedClosedShell();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepShape_ClosedShell)& aClosedShellElement, const Standard_Boolean aOrientation);
  
  Standard_EXPORT void SetClosedShellElement (const Handle(StepShape_ClosedShell)& aClosedShellElement);
  
  Standard_EXPORT Handle(StepShape_ClosedShell) ClosedShellElement() const;
  
  Standard_EXPORT void SetOrientation (const Standard_Boolean aOrientation);
  
  Standard_EXPORT Standard_Boolean Orientation() const;
  
  Standard_EXPORT virtual void SetCfsFaces (const Handle(StepShape_HArray1OfFace)& aCfsFaces) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(StepShape_HArray1OfFace) CfsFaces() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(StepShape_Face) CfsFacesValue (const Standard_Integer num) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer NbCfsFaces() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepShape_OrientedClosedShell,StepShape_ClosedShell)

protected:




private:


  Handle(StepShape_ClosedShell) closedShellElement;
  Standard_Boolean orientation;


};







#endif // _StepShape_OrientedClosedShell_HeaderFile
