// Copyright (c) Open CASCADE 2025
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

#ifndef _StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular_HeaderFile_
#define _StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular_HeaderFile_

#include <StepVisual_SurfaceStyleReflectanceAmbientDiffuse.hxx>
#include <StepVisual_Colour.hxx>

//! Representation of STEP entity SurfaceStyleReflectanceAmbientDiffuseSpecular
class StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular
    : public StepVisual_SurfaceStyleReflectanceAmbientDiffuse
{
public:
  //! default constructor
  Standard_EXPORT StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Standard_Real              theAmbientReflectance,
                            const Standard_Real              theDiffuseReflectance,
                            const Standard_Real              theSpecularReflectance,
                            const Standard_Real              theSpecularExponent,
                            const Handle(StepVisual_Colour)& theSpecularColour);

  //! Returns field SpecularReflectance
  Standard_EXPORT Standard_Real SpecularReflectance() const;

  //! Sets field SpecularReflectance
  Standard_EXPORT void SetSpecularReflectance(const Standard_Real theSpecularReflectance);

  //! Returns field SpecularExponent
  Standard_EXPORT Standard_Real SpecularExponent() const;

  //! Sets field SpecularExponent
  Standard_EXPORT void SetSpecularExponent(const Standard_Real theSpecularExponent);

  //! Returns field SpecularColour
  Standard_EXPORT Handle(StepVisual_Colour) SpecularColour() const;

  //! Sets field SpecularColour
  Standard_EXPORT void SetSpecularColour(const Handle(StepVisual_Colour)& theSpecularColour);

  DEFINE_STANDARD_RTTIEXT(StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular,
                          StepVisual_SurfaceStyleReflectanceAmbientDiffuse)

private:
  Standard_Real             mySpecularReflectance;
  Standard_Real             mySpecularExponent;
  Handle(StepVisual_Colour) mySpecularColour;
};
#endif // _StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular_HeaderFile_
