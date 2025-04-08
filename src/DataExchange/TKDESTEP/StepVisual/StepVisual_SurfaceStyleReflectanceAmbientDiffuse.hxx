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

#ifndef _StepVisual_SurfaceStyleReflectanceAmbientDiffuse_HeaderFile_
#define _StepVisual_SurfaceStyleReflectanceAmbientDiffuse_HeaderFile_

#include <StepVisual_SurfaceStyleReflectanceAmbient.hxx>

//! Representation of STEP entity SurfaceStyleReflectanceAmbientDiffuse
class StepVisual_SurfaceStyleReflectanceAmbientDiffuse
    : public StepVisual_SurfaceStyleReflectanceAmbient
{
public:
  //! default constructor
  Standard_EXPORT StepVisual_SurfaceStyleReflectanceAmbientDiffuse();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Standard_Real theAmbientReflectance,
                            const Standard_Real theDiffuseReflectance);

  //! Returns field DiffuseReflectance
  Standard_EXPORT Standard_Real DiffuseReflectance() const;

  //! Sets field DiffuseReflectance
  Standard_EXPORT void SetDiffuseReflectance(const Standard_Real theDiffuseReflectance);

  DEFINE_STANDARD_RTTIEXT(StepVisual_SurfaceStyleReflectanceAmbientDiffuse,
                          StepVisual_SurfaceStyleReflectanceAmbient)

private:
  Standard_Real myDiffuseReflectance;
};
#endif // _StepVisual_SurfaceStyleReflectanceAmbientDiffuse_HeaderFile_
