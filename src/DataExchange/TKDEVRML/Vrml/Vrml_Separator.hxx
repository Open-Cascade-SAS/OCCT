// Created on: 1997-03-27
// Created by: Alexander BRIVIN and Dmitry TARASOV
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

#ifndef _Vrml_Separator_HeaderFile
#define _Vrml_Separator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Vrml_SeparatorRenderCulling.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>

//! defines a Separator node of VRML specifying group properties.
//! This group node performs a push (save) of the traversal state before traversing its children
//! and a pop (restore) after traversing them. This isolates the separator's children from the
//! rest of the scene graph. A separator can include lights, cameras, coordinates, normals,
//! bindings, and all other properties.
//! Separators can also perform render culling. Render culling skips over traversal of the
//! separator's children if they are not going to be rendered, based on the comparison of the
//! separator's bounding box with the current view volume. Culling is controlled by the
//! renderCulling field. These are set to AUTO by default, allowing the implementation to
//! decide whether or not to cull.
class Vrml_Separator
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Vrml_Separator(const Vrml_SeparatorRenderCulling aRenderCulling);

  Standard_EXPORT Vrml_Separator();

  Standard_EXPORT void SetRenderCulling(const Vrml_SeparatorRenderCulling aRenderCulling);

  Standard_EXPORT Vrml_SeparatorRenderCulling RenderCulling() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream);

protected:
private:
  Vrml_SeparatorRenderCulling myRenderCulling;
  Standard_Boolean            myFlagPrint;
};

#endif // _Vrml_Separator_HeaderFile
