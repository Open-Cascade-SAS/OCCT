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

#ifndef _StepShape_Block_HeaderFile
#define _StepShape_Block_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_GeometricRepresentationItem.hxx>
class StepGeom_Axis2Placement3d;
class TCollection_HAsciiString;

class StepShape_Block;
DEFINE_STANDARD_HANDLE(StepShape_Block, StepGeom_GeometricRepresentationItem)

class StepShape_Block : public StepGeom_GeometricRepresentationItem
{

public:
  //! Returns a Block
  Standard_EXPORT StepShape_Block();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&  aName,
                            const Handle(StepGeom_Axis2Placement3d)& aPosition,
                            const Standard_Real                      aX,
                            const Standard_Real                      aY,
                            const Standard_Real                      aZ);

  Standard_EXPORT void SetPosition(const Handle(StepGeom_Axis2Placement3d)& aPosition);

  Standard_EXPORT Handle(StepGeom_Axis2Placement3d) Position() const;

  Standard_EXPORT void SetX(const Standard_Real aX);

  Standard_EXPORT Standard_Real X() const;

  Standard_EXPORT void SetY(const Standard_Real aY);

  Standard_EXPORT Standard_Real Y() const;

  Standard_EXPORT void SetZ(const Standard_Real aZ);

  Standard_EXPORT Standard_Real Z() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_Block, StepGeom_GeometricRepresentationItem)

protected:
private:
  Handle(StepGeom_Axis2Placement3d) position;
  Standard_Real                     x;
  Standard_Real                     y;
  Standard_Real                     z;
};

#endif // _StepShape_Block_HeaderFile
