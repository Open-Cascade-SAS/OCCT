// Created on : Tue May 12 14:11:46 2020
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

#ifndef _StepVisual_SurfaceStyleTransparent_HeaderFile_
#define _StepVisual_SurfaceStyleTransparent_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

class StepVisual_SurfaceStyleTransparent;
DEFINE_STANDARD_HANDLE(StepVisual_SurfaceStyleTransparent, Standard_Transient)

//! Representation of STEP entity SurfaceStyleTransparent
class StepVisual_SurfaceStyleTransparent : public Standard_Transient
{
public :

  //! default constructor
  Standard_EXPORT StepVisual_SurfaceStyleTransparent();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Standard_Real theTransparency);

  //! Returns field Transparency
  Standard_EXPORT Standard_Real Transparency() const;
  //! Sets field Transparency
  Standard_EXPORT void SetTransparency (const Standard_Real theTransparency);

DEFINE_STANDARD_RTTIEXT(StepVisual_SurfaceStyleTransparent, Standard_Transient)

private:
  Standard_Real myTransparency;

};
#endif // _StepVisual_SurfaceStyleTransparent_HeaderFile_
