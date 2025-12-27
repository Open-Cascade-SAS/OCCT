// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_PrimitiveArray_HeaderFile
#define OpenGl_PrimitiveArray_HeaderFile

#include <Graphic3d_TypeOfPrimitiveArray.hxx>
#include <Graphic3d_IndexBuffer.hxx>
#include <Graphic3d_BoundBuffer.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_IndexBuffer;
class OpenGl_VertexBuffer;
class OpenGl_GraphicDriver;

//! Class for rendering of arbitrary primitive array.
class OpenGl_PrimitiveArray : public OpenGl_Element
{
public:
  //! OpenGL does not provide a constant for "none" draw mode.
  //! So we define our own one that does not conflict with GL constants and utilizes common GL
  //! invalid value.
  enum
  {
    DRAW_MODE_NONE = -1
  };

  //! Empty constructor
  Standard_EXPORT OpenGl_PrimitiveArray(const OpenGl_GraphicDriver* theDriver);

  //! Default constructor
  Standard_EXPORT OpenGl_PrimitiveArray(const OpenGl_GraphicDriver*          theDriver,
                                        const Graphic3d_TypeOfPrimitiveArray theType,
                                        const occ::handle<Graphic3d_IndexBuffer>& theIndices,
                                        const occ::handle<Graphic3d_Buffer>&      theAttribs,
                                        const occ::handle<Graphic3d_BoundBuffer>& theBounds);

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_PrimitiveArray();

  //! Render primitives to the window
  Standard_EXPORT virtual void Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const
    override;

  //! Release OpenGL resources (VBOs)
  Standard_EXPORT virtual void Release(OpenGl_Context* theContext) override;

  //! Returns estimated GPU memory usage for holding data without considering overheads and
  //! allocation alignment rules.
  Standard_EXPORT virtual size_t EstimatedDataSize() const override;

  //! Increment draw calls statistics.
  Standard_EXPORT virtual void UpdateDrawStats(Graphic3d_FrameStatsDataTmp& theStats,
                                               bool theIsDetailed) const override;

  //! Return true if VBOs initialization has been performed.
  //! VBO initialization is performed during first Render() call.
  //! Notice that this flag does not indicate VBOs validity.
  bool IsInitialized() const { return myIsVboInit; }

  //! Invalidate VBO content without destruction.
  void Invalidate() const { myIsVboInit = false; }

  //! @return primitive type (GL_LINES, GL_TRIANGLES and others)
  int DrawMode() const { return myDrawMode; }

  //! Return TRUE if primitive type generates shaded triangulation.
  virtual bool IsFillDrawMode() const override { return myIsFillType; }

  //! @return indices array
  const occ::handle<Graphic3d_IndexBuffer>& Indices() const { return myIndices; }

  //! @return attributes array
  const occ::handle<Graphic3d_Buffer>& Attributes() const { return myAttribs; }

  //! @return bounds array
  const occ::handle<Graphic3d_BoundBuffer>& Bounds() const { return myBounds; }

  //! Returns unique ID of primitive array.
  size_t GetUID() const { return myUID; }

  //! Initialize indices, attributes and bounds with new data.
  Standard_EXPORT void InitBuffers(const occ::handle<OpenGl_Context>&        theContext,
                                   const Graphic3d_TypeOfPrimitiveArray theType,
                                   const occ::handle<Graphic3d_IndexBuffer>& theIndices,
                                   const occ::handle<Graphic3d_Buffer>&      theAttribs,
                                   const occ::handle<Graphic3d_BoundBuffer>& theBounds);

public:
  //! Returns index VBO.
  const occ::handle<OpenGl_IndexBuffer>& IndexVbo() const { return myVboIndices; }

  //! Returns attributes VBO.
  const occ::handle<OpenGl_VertexBuffer>& AttributesVbo() const { return myVboAttribs; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int  theDepth = -1) const override;

protected:
  //! VBO initialization procedures
  //! @param theCtx        bound GL context
  //! @param theToKeepData when true, myAttribs will not be nullified after VBO creation
  Standard_EXPORT bool buildVBO(const occ::handle<OpenGl_Context>& theCtx,
                                            const bool        theToKeepData) const;

  //! Patch VBO sub-date within invalidated range.
  Standard_EXPORT void updateVBO(const occ::handle<OpenGl_Context>& theCtx) const;

  //! Release GL memory.
  Standard_EXPORT void clearMemoryGL(const occ::handle<OpenGl_Context>& theGlCtx) const;

private:
  //! Initialize normal (OpenGL-provided) VBO
  bool initNormalVbo(const occ::handle<OpenGl_Context>& theCtx) const;

  //! Main procedure to draw array
  void drawArray(const occ::handle<OpenGl_Workspace>& theWorkspace,
                 const NCollection_Vec4<float>*           theFaceColors,
                 const bool          theHasVertColor) const;

  //! Auxiliary procedures
  void drawEdges(const occ::handle<OpenGl_Workspace>& theWorkspace) const;

  void drawMarkers(const occ::handle<OpenGl_Workspace>& theWorkspace) const;

  //! Sets OpenGL draw mode according to the input type of primitive array.
  //! If buffer of attributes is empty, draw mode is set to NONE to avoid invalid array rendering.
  //! @param theType type of primitive array.
  void setDrawMode(const Graphic3d_TypeOfPrimitiveArray theType);

  //! Rebuilds the array of vertex attributes so that it can be drawn without indices.
  bool processIndices(const occ::handle<OpenGl_Context>& theContext) const;

protected:
  mutable occ::handle<OpenGl_IndexBuffer>  myVboIndices;
  mutable occ::handle<OpenGl_VertexBuffer> myVboAttribs;

  mutable occ::handle<Graphic3d_IndexBuffer> myIndices;
  mutable occ::handle<Graphic3d_Buffer>      myAttribs;
  mutable occ::handle<Graphic3d_BoundBuffer> myBounds;
  short                                 myDrawMode;
  mutable bool              myIsFillType;
  mutable bool              myIsVboInit;

  size_t myUID; //!< Unique ID of primitive array.

public:
  DEFINE_STANDARD_ALLOC
};

#endif // OpenGl_PrimitiveArray_Header
