// Created on : Thu May 14 15:13:19 2020
// Created by: Igor KHOZHANOV
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2020
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

#ifndef _StepVisual_SurfaceStyleReflectanceAmbient_HeaderFile_
#define _StepVisual_SurfaceStyleReflectanceAmbient_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

class StepVisual_SurfaceStyleReflectanceAmbient;
DEFINE_STANDARD_HANDLE(StepVisual_SurfaceStyleReflectanceAmbient, Standard_Transient)

//! Representation of STEP entity SurfaceStyleReflectanceAmbient
class StepVisual_SurfaceStyleReflectanceAmbient : public Standard_Transient
{
public :

  //! default constructor
  Standard_EXPORT StepVisual_SurfaceStyleReflectanceAmbient();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Standard_Real theAmbientReflectance);

  //! Returns field AmbientReflectance
  Standard_EXPORT Standard_Real AmbientReflectance() const;
  //! Sets field AmbientReflectance
  Standard_EXPORT void SetAmbientReflectance (const Standard_Real theAmbientReflectance);

DEFINE_STANDARD_RTTIEXT(StepVisual_SurfaceStyleReflectanceAmbient, Standard_Transient)

private:
  Standard_Real myAmbientReflectance;

};
#endif // _StepVisual_SurfaceStyleReflectanceAmbient_HeaderFile_
