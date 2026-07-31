// Copyright (c) 2015-2021 OPEN CASCADE SAS
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

#ifndef _RWObj_ObjWriterContext_HeaderFiler
#define _RWObj_ObjWriterContext_HeaderFiler

#include <NCollection_Vec2.hxx>

#include <Standard_TypeDef.hxx>

#include <NCollection_Vec3.hxx>

#include <NCollection_Vec4.hxx>

#include <NCollection_Mat4.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_IndexedDataMap.hxx>

//! Auxiliary low-level tool writing OBJ file.
class RWObj_ObjWriterContext
{
public:
  //! Main constructor.
  Standard_EXPORT RWObj_ObjWriterContext(const TCollection_AsciiString& theName);

  //! Destructor, will emit error message if file was not closed.
  Standard_EXPORT ~RWObj_ObjWriterContext();

  //! Return true if file has been opened.
  bool IsOpened() const { return myFile != nullptr; }

  //! Correctly close the file.
  Standard_EXPORT bool Close();

  //! Return true if normals are defined.
  bool HasNormals() const { return myHasNormals; }

  //! Set if normals are defined.
  void SetNormals(const bool theHasNormals) { myHasNormals = theHasNormals; }

  //! Return true if normals are defined.
  bool HasTexCoords() const { return myHasTexCoords; }

  //! Set if normals are defined.
  void SetTexCoords(const bool theHasTexCoords) { myHasTexCoords = theHasTexCoords; }

  //! Write the header.
  Standard_EXPORT bool WriteHeader(
    const int                      theNbNodes,
    const int                      theNbElems,
    const TCollection_AsciiString& theMatLib,
    const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>&
      theFileInfo);

  //! Return active material or empty string if not set.
  const TCollection_AsciiString& ActiveMaterial() const { return myActiveMaterial; }

  //! Set active material.
  Standard_EXPORT bool WriteActiveMaterial(const TCollection_AsciiString& theMaterial);

  //! Writing a triangle
  Standard_EXPORT bool WriteTriangle(const NCollection_Vec3<int>& theTri);

  //! Writing a quad
  Standard_EXPORT bool WriteQuad(const NCollection_Vec4<int>& theQuad);

  //! Writing a vector
  Standard_EXPORT bool WriteVertex(const NCollection_Vec3<float>& theValue);

  //! Writing a vector
  Standard_EXPORT bool WriteNormal(const NCollection_Vec3<float>& theValue);

  //! Writing a vector
  Standard_EXPORT bool WriteTexCoord(const NCollection_Vec2<float>& theValue);

  //! Writing a group name
  Standard_EXPORT bool WriteGroup(const TCollection_AsciiString& theValue);

  //! Increment indices shift.
  Standard_EXPORT void FlushFace(int theNbNodes);

public:
  int NbFaces;

private:
  FILE*                   myFile;
  TCollection_AsciiString myName;
  TCollection_AsciiString myActiveMaterial;
  NCollection_Vec4<int>   myElemPosFirst;
  NCollection_Vec4<int>   myElemNormFirst;
  NCollection_Vec4<int>   myElemUVFirst;
  bool                    myHasNormals;
  bool                    myHasTexCoords;
};

#endif // _RWObj_ObjWriterContext_HeaderFiler
