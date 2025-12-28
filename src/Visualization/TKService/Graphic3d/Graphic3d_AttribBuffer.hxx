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

#ifndef _Graphic3d_AttribBuffer_HeaderFile
#define _Graphic3d_AttribBuffer_HeaderFile

#include <Graphic3d_Buffer.hxx>

//! Buffer of vertex attributes.
//! This class is intended for advanced usage allowing invalidation of entire buffer content or its
//! sub-part.
class Graphic3d_AttribBuffer : public Graphic3d_Buffer
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_AttribBuffer, Graphic3d_Buffer)
public:
  //! Empty constructor.
  Standard_EXPORT Graphic3d_AttribBuffer(const occ::handle<NCollection_BaseAllocator>& theAlloc);

  //! Allocates new empty array
  Standard_EXPORT bool Init(const int                  theNbElems,
                            const Graphic3d_Attribute* theAttribs,
                            const int                  theNbAttribs);

  //! Allocates new empty array
  bool Init(const int theNbElems, const NCollection_Array1<Graphic3d_Attribute>& theAttribs)
  {
    return Init(theNbElems, &theAttribs.First(), theAttribs.Size());
  }

  //! Return TRUE if data can be invalidated; FALSE by default.
  bool IsMutable() const override { return myIsMutable; }

  //! Set if data can be invalidated.
  Standard_EXPORT void SetMutable(bool theMutable);

  //! Return TRUE for interleaved array; TRUE by default.
  bool IsInterleaved() const override { return myIsInterleaved; }

  //! Setup interleaved/non-interleaved array.
  //! WARNING! Filling non-interleaved buffer should be implemented on user side
  //!          without Graphic3d_Buffer auxiliary methods designed for interleaved data.
  Standard_EXPORT void SetInterleaved(bool theIsInterleaved);

  //! Return invalidated range.
  Graphic3d_BufferRange InvalidatedRange() const override { return myInvalidatedRange; }

  //! Reset invalidated range.
  void Validate() override { myInvalidatedRange.Clear(); }

  //! Invalidate the entire buffer data.
  Standard_EXPORT void Invalidate() override;

  //! Invalidate the entire attribute data.
  Standard_EXPORT void Invalidate(int theAttributeIndex);

  //! Invalidate attribute data within specified sub-range (starting from 0).
  Standard_EXPORT void Invalidate(int theAttributeIndex, int theVertexLower, int theVertexUpper);

  //! Invalidate all attribute data within specified vertex sub-range (starting from 0).
  Standard_EXPORT void Invalidate(int theVertexLower, int theVertexUpper);

  //! Invalidate specified sub-range of data (as byte offsets).
  Standard_EXPORT void invalidate(const Graphic3d_BufferRange& theRange);

protected:
  Graphic3d_BufferRange myInvalidatedRange; //!< invalidated buffer data range (as byte offsets)
                                            // clang-format off
  bool      myIsInterleaved;    //!< flag indicating the vertex attributes being interleaved
                                            // clang-format on
  bool myIsMutable;                         //!< flag indicating that data can be invalidated
};

#endif // _Graphic3d_AttribBuffer_HeaderFile
