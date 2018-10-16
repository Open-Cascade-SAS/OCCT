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

#include <Graphic3d_AttribBuffer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_AttribBuffer, Graphic3d_Buffer)

// =======================================================================
// function : Graphic3d_AttribBuffer
// purpose  :
// =======================================================================
Graphic3d_AttribBuffer::Graphic3d_AttribBuffer(const Handle(NCollection_BaseAllocator)& theAlloc)
: Graphic3d_Buffer (theAlloc),
  myIsInterleaved (Standard_True),
  myIsMutable (Standard_False)
{
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool Graphic3d_AttribBuffer::Init (const Standard_Integer     theNbElems,
                                   const Graphic3d_Attribute* theAttribs,
                                   const Standard_Integer     theNbAttribs)
{
  if (!Graphic3d_Buffer::Init (theNbElems, theAttribs, theNbAttribs))
  {
    return false;
  }

  if (mySize > (Standard_Size )IntegerLast()
   && myIsMutable)
  {
    throw Standard_OutOfRange ("Graphic3d_AttribBuffer::Init(), Mutable flag cannot be used for buffer exceeding 32-bit address space");
  }
  return true;
}

// =======================================================================
// function : SetMutable
// purpose  :
// =======================================================================
void Graphic3d_AttribBuffer::SetMutable (Standard_Boolean theMutable)
{
  if (mySize > (Standard_Size )IntegerLast()
   && theMutable)
  {
    throw Standard_OutOfRange ("Graphic3d_AttribBuffer::SetMutable(), Mutable flag cannot be used for buffer exceeding 32-bit address space");
  }
  myIsMutable = theMutable;
}

// =======================================================================
// function : Invalidate
// purpose  :
// =======================================================================
void Graphic3d_AttribBuffer::SetInterleaved (Standard_Boolean theIsInterleaved)
{
  if (NbMaxElements() != 0)
  {
    throw Standard_ProgramError ("Graphic3d_AttribBuffer::SetInterleaved() should not be called for allocated buffer");
  }
  myIsInterleaved = theIsInterleaved;
}

// =======================================================================
// function : invalidate
// purpose  :
// =======================================================================
void Graphic3d_AttribBuffer::invalidate (const Graphic3d_BufferRange& theRange)
{
  if (mySize > (Standard_Size )IntegerLast())
  {
    throw Standard_OutOfRange ("Graphic3d_Buffer::Invalidate() cannot be used for buffer exceeding 32-bit address space");
  }

  myInvalidatedRange.Unite (theRange);
}

// =======================================================================
// function : Invalidate
// purpose  :
// =======================================================================
void Graphic3d_AttribBuffer::Invalidate()
{
  if (mySize > (Standard_Size )IntegerLast())
  {
    throw Standard_OutOfRange ("Graphic3d_AttribBuffer::Invalidate() cannot be used for buffer exceeding 32-bit address space");
  }

  invalidate (Graphic3d_BufferRange (0, (Standard_Integer )mySize));
}

// =======================================================================
// function : Invalidate
// purpose  :
// =======================================================================
void Graphic3d_AttribBuffer::Invalidate (Standard_Integer theAttributeIndex)
{
  Standard_OutOfRange_Raise_if (theAttributeIndex < 0
                             || theAttributeIndex >= NbAttributes, "Graphic3d_AttribBuffer::Invalidate()");
  if (myIsInterleaved)
  {
    Invalidate();
    return;
  }

  Graphic3d_BufferRange aRange;
  const Standard_Integer aNbMaxVerts = NbMaxElements();
  for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
  {
    const Graphic3d_Attribute& anAttrib = Attribute (anAttribIter);
    const Standard_Integer anAttribStride = Graphic3d_Attribute::Stride (anAttrib.DataType);
    if (anAttribIter == theAttributeIndex)
    {
      aRange.Length = anAttribStride * aNbMaxVerts;
      invalidate (aRange);
      return;
    }

    aRange.Start += anAttribStride * aNbMaxVerts;
  }
}

// =======================================================================
// function : Invalidate
// purpose  :
// =======================================================================
void Graphic3d_AttribBuffer::Invalidate (Standard_Integer theAttributeIndex,
                                         Standard_Integer theVertexLower,
                                         Standard_Integer theVertexUpper)
{
  Standard_OutOfRange_Raise_if (theAttributeIndex < 0
                             || theAttributeIndex >= NbAttributes
                             || theVertexLower < 0
                             || theVertexLower > theVertexUpper
                             || theVertexUpper >= NbMaxElements(), "Graphic3d_AttribBuffer::Invalidate()");
  if (myIsInterleaved)
  {
    Invalidate (theVertexLower, theVertexUpper);
    return;
  }

  Graphic3d_BufferRange aRange;
  const Standard_Integer aNbMaxVerts = NbMaxElements();
  for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
  {
    const Graphic3d_Attribute& anAttrib = Attribute (anAttribIter);
    const Standard_Integer anAttribStride = Graphic3d_Attribute::Stride (anAttrib.DataType);
    if (anAttribIter == theAttributeIndex)
    {
      aRange.Start += anAttribStride * theVertexLower;
      aRange.Length = anAttribStride * (theVertexUpper - theVertexLower + 1);
      invalidate (aRange);
      return;
    }

    aRange.Start += anAttribStride * aNbMaxVerts;
  }
}

// =======================================================================
// function : Invalidate
// purpose  :
// =======================================================================
void Graphic3d_AttribBuffer::Invalidate (Standard_Integer theVertexLower,
                                         Standard_Integer theVertexUpper)
{
  Standard_OutOfRange_Raise_if (theVertexLower < 0
                             || theVertexLower > theVertexUpper
                             || theVertexUpper >= NbMaxElements(), "Graphic3d_AttribBuffer::Invalidate()");
  if (myIsInterleaved)
  {
    invalidate (Graphic3d_BufferRange (Stride * theVertexLower,
                                       Stride * (theVertexUpper - theVertexLower + 1)));
    return;
  }

  for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
  {
    Invalidate (anAttribIter, theVertexLower, theVertexUpper);
  }
}
