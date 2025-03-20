// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepVisual_ExternallyDefinedTextFont_HeaderFile
#define _StepVisual_ExternallyDefinedTextFont_HeaderFile

#include <Standard.hxx>

#include <StepBasic_ExternallyDefinedItem.hxx>

class StepVisual_ExternallyDefinedTextFont;
DEFINE_STANDARD_HANDLE(StepVisual_ExternallyDefinedTextFont, StepBasic_ExternallyDefinedItem)

//! Representation of STEP entity ExternallyDefinedTextFont
class StepVisual_ExternallyDefinedTextFont : public StepBasic_ExternallyDefinedItem
{

public:
  //! Empty constructor
  Standard_EXPORT StepVisual_ExternallyDefinedTextFont();

  DEFINE_STANDARD_RTTIEXT(StepVisual_ExternallyDefinedTextFont, StepBasic_ExternallyDefinedItem)

protected:
private:
};

#endif // _StepVisual_ExternallyDefinedTextFont_HeaderFile
