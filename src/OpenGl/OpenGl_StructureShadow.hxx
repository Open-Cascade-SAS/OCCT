// Created on: 2014-09-01
// Created by: Ivan SAZONOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef OpenGl_StructureShadow_Header
#define OpenGl_StructureShadow_Header

#include <OpenGl_Structure.hxx>

//! Dummy structure which just redirects to groups of another structure.
class OpenGl_StructureShadow : public OpenGl_Structure
{

public:

  //! Create empty structure
  Standard_EXPORT OpenGl_StructureShadow (const Handle(Graphic3d_StructureManager)& theManager,
                                          const Handle(OpenGl_Structure)&           theStructure);

  virtual const Graphic3d_SequenceOfGroup& DrawGroups() const { return myParent->DrawGroups(); }

private:

  Handle(OpenGl_Structure) myParent;

public:

  DEFINE_STANDARD_RTTI(OpenGl_StructureShadow) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_StructureShadow, OpenGl_Structure)

#endif // OpenGl_StructureShadow_Header
