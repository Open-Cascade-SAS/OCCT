// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _RWPly_PlyWriterContext_HeaderFiler
#define _RWPly_PlyWriterContext_HeaderFiler

#include <NCollection_Vec2.hxx>

#include <Standard_TypeDef.hxx>

#include <NCollection_Vec3.hxx>

#include <NCollection_Vec4.hxx>

#include <NCollection_Mat4.hxx>
#include <gp_Pnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_IndexedDataMap.hxx>

#include <memory>

//! Auxiliary low-level tool writing PLY file.
class RWPly_PlyWriterContext
{
public:
  //! Empty constructor.
  Standard_EXPORT RWPly_PlyWriterContext();

  //! Destructor, will emit error message if file was not closed.
  Standard_EXPORT ~RWPly_PlyWriterContext();

public: //! @name vertex attributes parameters
  //! Return TRUE if vertex position should be stored with double floating point precision; FALSE by
  //! default.
  bool IsDoublePrecision() const { return myIsDoublePrec; }

  //! Set if vertex position should be stored with double floating point precision.
  void SetDoublePrecision(bool theDoublePrec) { myIsDoublePrec = theDoublePrec; }

  //! Return TRUE if normals should be written as vertex attribute; FALSE by default.
  bool HasNormals() const { return myHasNormals; }

  //! Set if normals should be written.
  void SetNormals(const bool theHasNormals) { myHasNormals = theHasNormals; }

  //! Return TRUE if UV / texture coordinates should be written as vertex attribute; FALSE by
  //! default.
  bool HasTexCoords() const { return myHasTexCoords; }

  //! Set if UV / texture coordinates should be written.
  void SetTexCoords(const bool theHasTexCoords) { myHasTexCoords = theHasTexCoords; }

  //! Return TRUE if point colors should be written as vertex attribute; FALSE by default.
  bool HasColors() const { return myHasColors; }

  //! Set if point colors should be written.
  void SetColors(bool theToWrite) { myHasColors = theToWrite; }

public: //! @name element attributes parameters
  //! Return TRUE if surface Id should be written as element attribute; FALSE by default.
  bool HasSurfaceId() const { return myHasSurfId; }

  //! Set if surface Id should be written as element attribute; FALSE by default.
  void SetSurfaceId(bool theSurfId) { myHasSurfId = theSurfId; }

public: //! @name writing into file
  //! Return TRUE if file has been opened.
  bool IsOpened() const { return myStream.get() != nullptr; }

  //! Open file for writing.
  Standard_EXPORT bool Open(
    const TCollection_AsciiString&       theName,
    const std::shared_ptr<std::ostream>& theStream = std::shared_ptr<std::ostream>());

  //! Write the header.
  //! @param[in] theNbNodes number of vertex nodes
  //! @param[in] theNbElems number of mesh elements
  //! @param[in] theFileInfo optional comments
  Standard_EXPORT bool WriteHeader(
    const int theNbNodes,
    const int theNbElems,
    const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>&
      theFileInfo);

  //! Write single point with all attributes.
  //! @param[in] thePoint 3D point coordinates
  //! @param[in] theNorm  surface normal direction at the point
  //! @param[in] theUV    surface/texture UV coordinates
  //! @param[in] theColor RGB color values
  Standard_EXPORT bool WriteVertex(const gp_Pnt&                    thePoint,
                                   const NCollection_Vec3<float>&   theNorm,
                                   const NCollection_Vec2<float>&   theUV,
                                   const NCollection_Vec4<uint8_t>& theColor);

  //! Return number of written vertices.
  int NbWrittenVertices() const { return myNbVerts; }

  //! Return vertex offset to be applied to element indices; 0 by default.
  int VertexOffset() const { return myVertOffset; }

  //! Set vertex offset to be applied to element indices.
  void SetVertexOffset(int theOffset) { myVertOffset = theOffset; }

  //! Return surface id to write with element; 0 by default.
  int SurfaceId() const { return mySurfId; }

  //! Set surface id to write with element.
  void SetSurfaceId(int theSurfId) { mySurfId = theSurfId; }

  //! Writing a triangle.
  Standard_EXPORT bool WriteTriangle(const NCollection_Vec3<int>& theTri);

  //! Writing a quad.
  Standard_EXPORT bool WriteQuad(const NCollection_Vec4<int>& theQuad);

  //! Return number of written elements.
  int NbWrittenElements() const { return myNbElems; }

  //! Correctly close the file.
  //! @return FALSE in case of writing error
  Standard_EXPORT bool Close(bool theIsAborted = false);

private:
  std::shared_ptr<std::ostream> myStream;
  TCollection_AsciiString       myName;
  int                           myNbHeaderVerts;
  int                           myNbHeaderElems;
  int                           myNbVerts;
  int                           myNbElems;
  int                           mySurfId;
  int                           myVertOffset;
  bool                          myIsDoublePrec;
  bool                          myHasNormals;
  bool                          myHasColors;
  bool                          myHasTexCoords;
  bool                          myHasSurfId;
};

#endif // _RWPly_PlyWriterContext_HeaderFiler
