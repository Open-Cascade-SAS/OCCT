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

#include <StepVisual_DirectionCountSelect.hxx>

StepVisual_DirectionCountSelect::StepVisual_DirectionCountSelect() :
    theUDirectionCount(0),
    theVDirectionCount(0),
    theTypeOfContent(0)
{}

void StepVisual_DirectionCountSelect::SetTypeOfContent(const Standard_Integer aType)
{
  theTypeOfContent = aType;
}

Standard_Integer StepVisual_DirectionCountSelect::TypeOfContent() const 
{
  return theTypeOfContent;
}

Standard_Integer StepVisual_DirectionCountSelect::UDirectionCount() const
{
  return theUDirectionCount;
}

void StepVisual_DirectionCountSelect::SetUDirectionCount(const Standard_Integer aUDirectionCount)
{
  theUDirectionCount = aUDirectionCount;
  theTypeOfContent   = 1;
}


Standard_Integer StepVisual_DirectionCountSelect::VDirectionCount() const
{
  return theUDirectionCount;
}

void StepVisual_DirectionCountSelect::SetVDirectionCount(const Standard_Integer aVDirectionCount)
{
  theVDirectionCount = aVDirectionCount;
  theTypeOfContent   = 2;
}


