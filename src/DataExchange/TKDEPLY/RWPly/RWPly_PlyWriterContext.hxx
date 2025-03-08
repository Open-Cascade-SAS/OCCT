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

#include <Graphic3d_Vec.hxx>
#include <gp_Pnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_IndexedDataMapOfStringString.hxx>

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
  Standard_EXPORT bool WriteHeader(const Standard_Integer                      theNbNodes,
                                   const Standard_Integer                      theNbElems,
                                   const TColStd_IndexedDataMapOfStringString& theFileInfo);

  //! Write single point with all attributes.
  //! @param[in] thePoint 3D point coordinates
  //! @param[in] theNorm  surface normal direction at the point
  //! @param[in] theUV    surface/texture UV coordinates
  //! @param[in] theColor RGB color values
  Standard_EXPORT bool WriteVertex(const gp_Pnt&           thePoint,
                                   const Graphic3d_Vec3&   theNorm,
                                   const Graphic3d_Vec2&   theUV,
                                   const Graphic3d_Vec4ub& theColor);

  //! Return number of written vertices.
  Standard_Integer NbWrittenVertices() const { return myNbVerts; }

  //! Return vertex offset to be applied to element indices; 0 by default.
  Standard_Integer VertexOffset() const { return myVertOffset; }

  //! Set vertex offset to be applied to element indices.
  void SetVertexOffset(Standard_Integer theOffset) { myVertOffset = theOffset; }

  //! Return surface id to write with element; 0 by default.
  Standard_Integer SurfaceId() const { return mySurfId; }

  //! Set surface id to write with element.
  void SetSurfaceId(Standard_Integer theSurfId) { mySurfId = theSurfId; }

  //! Writing a triangle.
  Standard_EXPORT bool WriteTriangle(const Graphic3d_Vec3i& theTri);

  //! Writing a quad.
  Standard_EXPORT bool WriteQuad(const Graphic3d_Vec4i& theQuad);

  //! Return number of written elements.
  Standard_Integer NbWrittenElements() const { return myNbElems; }

  //! Correctly close the file.
  //! @return FALSE in case of writing error
  Standard_EXPORT bool Close(bool theIsAborted = false);

private:
  std::shared_ptr<std::ostream> myStream;
  TCollection_AsciiString       myName;
  Standard_Integer              myNbHeaderVerts;
  Standard_Integer              myNbHeaderElems;
  Standard_Integer              myNbVerts;
  Standard_Integer              myNbElems;
  Standard_Integer              mySurfId;
  Standard_Integer              myVertOffset;
  bool                          myIsDoublePrec;
  bool                          myHasNormals;
  bool                          myHasColors;
  bool                          myHasTexCoords;
  bool                          myHasSurfId;
};

#endif // _RWPly_PlyWriterContext_HeaderFiler
