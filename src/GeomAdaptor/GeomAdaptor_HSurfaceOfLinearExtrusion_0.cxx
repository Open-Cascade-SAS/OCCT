// Created on: 1992-10-08
// Created by: Isabelle GRIGNON
// Copyright (c) 1992-1999 Matra Datavision
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

#include <GeomAdaptor_HSurfaceOfLinearExtrusion.hxx>

#include <Standard_Type.hxx>

#include <Standard_OutOfRange.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_DomainError.hxx>
#include <GeomAdaptor_SurfaceOfLinearExtrusion.hxx>
#include <Adaptor3d_Surface.hxx>



#define TheSurface GeomAdaptor_SurfaceOfLinearExtrusion
#define TheSurface_hxx <GeomAdaptor_SurfaceOfLinearExtrusion.hxx>
#define Adaptor3d_GenHSurface GeomAdaptor_HSurfaceOfLinearExtrusion
#define Adaptor3d_GenHSurface_hxx <GeomAdaptor_HSurfaceOfLinearExtrusion.hxx>
#define Handle_Adaptor3d_GenHSurface Handle(GeomAdaptor_HSurfaceOfLinearExtrusion)
#include <Adaptor3d_GenHSurface.gxx>

