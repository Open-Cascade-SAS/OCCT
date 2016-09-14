// Created on: 2013-07-15
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_SequenceOfHClipPlane_HeaderFile
#define _Graphic3d_SequenceOfHClipPlane_HeaderFile

#include <NCollection_Sequence.hxx>
#include <NCollection_Shared.hxx>
#include <Graphic3d_ClipPlane.hxx>

//! Class defining the sequence of clipping planes.
class Graphic3d_SequenceOfHClipPlane : public Standard_Transient, public NCollection_Sequence<Handle(Graphic3d_ClipPlane)>
{
DEFINE_STANDARD_RTTI_INLINE(Graphic3d_SequenceOfHClipPlane,Standard_Transient)
public:
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

  //! Empty constructor.
  Graphic3d_SequenceOfHClipPlane() : myToOverrideGlobal (Standard_False) {}

  //! Return true if local properties should override global properties.
  Standard_Boolean ToOverrideGlobal() const { return myToOverrideGlobal; }

  //! Setup flag defining if local properties should override global properties.
  void SetOverrideGlobal (const Standard_Boolean theToOverride) { myToOverrideGlobal = theToOverride; }

private:

  Standard_Boolean myToOverrideGlobal;

};

DEFINE_STANDARD_HANDLE(Graphic3d_SequenceOfHClipPlane, Standard_Transient)

#endif // _Graphic3d_SequenceOfHClipPlane_HeaderFile
