// Created on: 1997-03-26
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepVisual_CameraImage2dWithScale_HeaderFile
#define _StepVisual_CameraImage2dWithScale_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_CameraImage.hxx>

class StepVisual_CameraImage2dWithScale;
DEFINE_STANDARD_HANDLE(StepVisual_CameraImage2dWithScale, StepVisual_CameraImage)

class StepVisual_CameraImage2dWithScale : public StepVisual_CameraImage
{

public:
  Standard_EXPORT StepVisual_CameraImage2dWithScale();

  DEFINE_STANDARD_RTTIEXT(StepVisual_CameraImage2dWithScale, StepVisual_CameraImage)

protected:
private:
};

#endif // _StepVisual_CameraImage2dWithScale_HeaderFile
