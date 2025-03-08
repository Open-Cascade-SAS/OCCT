// Created by: Kirill GAVRILOV
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

#ifndef OpenGl_VertexBufferEditor_HeaderFile
#define OpenGl_VertexBufferEditor_HeaderFile

#include <OpenGl_Buffer.hxx>
#include <OpenGl_Context.hxx>

#include <NCollection_Array1.hxx>

//! Auxiliary class to iteratively modify data of existing VBO.
//! It provides iteration interface with delayed CPU->GPU memory transfer to avoid slow per-element
//! data transfer. User should explicitly call Flush() method to ensure that all data is transferred
//! to VBO. Temporary buffer on CPU side can be initialized with lesser capacity than  VBO to allow
//! re-usage of shared buffer with fixed size between VBOs.
//!
//! You should use NCollection_Vec2/NCollection_Vec3/NCollection_Vec4 with appropriate length
//! to instantiate this template and access elements in VBO.
//!
//! Notice that this technique designed for VBO streaming scenarios (when VBO is modified from time
//! to time). Also this class doesn't retrieve existing data from VBO - data transferred only in one
//! direction! In case of static data this is preferred to upload it within one call during VBO
//! initialization.
template <typename theVec_t>
class OpenGl_VertexBufferEditor
{

public:
  //! Creates empty editor
  //! theTmpBufferLength[in]  temporary buffer length
  explicit OpenGl_VertexBufferEditor(const Standard_Integer theTmpBufferLength = 0)
      : myElemFrom(0),
        myElemsNb(0),
        myTmpBuffer(0, theTmpBufferLength > 0 ? (theTmpBufferLength - 1) : 2047)
  {
  }

  //! Creates empty editor
  //! theTmpBuffer[in]        pointer to temporary buffer
  //! theTmpBufferLength[in]  temporary buffer length
  OpenGl_VertexBufferEditor(theVec_t* theTmpBuffer, const Standard_Integer theTmpBufferLength)
      : myElemFrom(0),
        myElemsNb(0),
        myTmpBuffer(theTmpBuffer[0], 0, theTmpBufferLength - 1)
  {
  }

  //! Initialize editor for specified buffer object.
  //! theGlCtx[in]  bound OpenGL context to edit buffer object
  //! theVbo[in]    buffer to edit
  Standard_Boolean Init(const Handle(OpenGl_Context)& theGlCtx, const Handle(OpenGl_Buffer)& theVbo)
  {
    myGlCtx = theGlCtx;
    myVbo   = theVbo;
    if (myGlCtx.IsNull() || myVbo.IsNull() || !myVbo->IsValid()
        || myVbo->GetComponentsNb() != GLuint(theVec_t::Length()))
    {
      return Standard_False;
    }

    myElemFrom = myElemsNb = 0;
    return Standard_True;
  }

  //! Modify current element in VBO.
  theVec_t& Value() { return myTmpBuffer.ChangeValue(myElemsNb); }

  //! Move to the next position in VBO.
  Standard_Boolean Next()
  {
    if (++myElemsNb > myTmpBuffer.Upper())
    {
      return Flush();
    }
    return Standard_True;
  }

  //! Push current data from local buffer to VBO.
  Standard_Boolean Flush()
  {
    if (myElemsNb <= 0)
    {
      return Standard_True;
    }

    if (myVbo.IsNull() || !myVbo->SubData(myGlCtx, myElemFrom, myElemsNb, &myTmpBuffer.Value(0)[0]))
    {
      // should never happens
      return Standard_False;
    }
    myElemFrom += myElemsNb;
    myElemsNb = 0;

    return Standard_True;
  }

  //! @return assigned VBO
  const Handle(OpenGl_Buffer)& GetVBO() const { return myVbo; }

private:
  Handle(OpenGl_Context)       myGlCtx;     //!< handle to current OpenGL context
  Handle(OpenGl_Buffer)        myVbo;       //!< edited VBO
  Standard_Integer             myElemFrom;  //!< element in VBO to upload from
  Standard_Integer             myElemsNb;   //!< current element in temporary buffer
  NCollection_Array1<theVec_t> myTmpBuffer; //!< temporary array
};

#endif // _OpenGl_VertexBufferEditor_H__
