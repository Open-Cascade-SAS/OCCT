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

#include <Graphic3d_Vec.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_IndexedDataMapOfStringString.hxx>

//! Auxiliary low-level tool writing OBJ file.
class RWObj_ObjWriterContext
{
public:
  //! Main constructor.
  Standard_EXPORT RWObj_ObjWriterContext(const TCollection_AsciiString& theName);

  //! Destructor, will emit error message if file was not closed.
  Standard_EXPORT ~RWObj_ObjWriterContext();

  //! Return true if file has been opened.
  bool IsOpened() const { return myFile != NULL; }

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
  Standard_EXPORT bool WriteHeader(const Standard_Integer                      theNbNodes,
                                   const Standard_Integer                      theNbElems,
                                   const TCollection_AsciiString&              theMatLib,
                                   const TColStd_IndexedDataMapOfStringString& theFileInfo);

  //! Return active material or empty string if not set.
  const TCollection_AsciiString& ActiveMaterial() const { return myActiveMaterial; }

  //! Set active material.
  Standard_EXPORT bool WriteActiveMaterial(const TCollection_AsciiString& theMaterial);

  //! Writing a triangle
  Standard_EXPORT bool WriteTriangle(const Graphic3d_Vec3i& theTri);

  //! Writing a quad
  Standard_EXPORT bool WriteQuad(const Graphic3d_Vec4i& theQuad);

  //! Writing a vector
  Standard_EXPORT bool WriteVertex(const Graphic3d_Vec3& theValue);

  //! Writing a vector
  Standard_EXPORT bool WriteNormal(const Graphic3d_Vec3& theValue);

  //! Writing a vector
  Standard_EXPORT bool WriteTexCoord(const Graphic3d_Vec2& theValue);

  //! Writing a group name
  Standard_EXPORT bool WriteGroup(const TCollection_AsciiString& theValue);

  //! Increment indices shift.
  Standard_EXPORT void FlushFace(Standard_Integer theNbNodes);

public:
  Standard_Integer NbFaces;

private:
  FILE*                   myFile;
  TCollection_AsciiString myName;
  TCollection_AsciiString myActiveMaterial;
  Graphic3d_Vec4i         myElemPosFirst;
  Graphic3d_Vec4i         myElemNormFirst;
  Graphic3d_Vec4i         myElemUVFirst;
  bool                    myHasNormals;
  bool                    myHasTexCoords;
};

#endif // _RWObj_ObjWriterContext_HeaderFiler
