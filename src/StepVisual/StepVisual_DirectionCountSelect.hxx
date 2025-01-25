// Created on: 1995-12-06
// Created by: Frederic MAUPAS
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

#ifndef _StepVisual_DirectionCountSelect_HeaderFile
#define _StepVisual_DirectionCountSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepVisual_DirectionCountSelect
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT StepVisual_DirectionCountSelect();

  Standard_EXPORT void SetTypeOfContent(const Standard_Integer aTypeOfContent);

  Standard_EXPORT Standard_Integer TypeOfContent() const;

  Standard_EXPORT Standard_Integer UDirectionCount() const;

  Standard_EXPORT void SetUDirectionCount(const Standard_Integer aUDirectionCount);

  Standard_EXPORT Standard_Integer VDirectionCount() const;

  Standard_EXPORT void SetVDirectionCount(const Standard_Integer aUDirectionCount);

protected:
private:
  Standard_Integer theUDirectionCount;
  Standard_Integer theVDirectionCount;
  Standard_Integer theTypeOfContent;
};

#endif // _StepVisual_DirectionCountSelect_HeaderFile
