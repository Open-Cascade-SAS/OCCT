// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <Graphic3d_Buffer.hxx>

#include <Graphic3d_BoundBuffer.hxx>
#include <Graphic3d_MutableIndexBuffer.hxx>
#include <NCollection_AlignedAllocator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Buffer, NCollection_Buffer)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_IndexBuffer, Graphic3d_Buffer)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_BoundBuffer, NCollection_Buffer)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_MutableIndexBuffer, Graphic3d_IndexBuffer)

// =======================================================================
// function : DefaultAllocator
// purpose  :
// =======================================================================
const Handle(NCollection_BaseAllocator)& Graphic3d_Buffer::DefaultAllocator()
{
  static const Handle(NCollection_BaseAllocator) THE_ALLOC = new NCollection_AlignedAllocator(16);
  return THE_ALLOC;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_Buffer::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, NCollection_Buffer)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Stride)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, NbElements)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, NbAttributes)
}
