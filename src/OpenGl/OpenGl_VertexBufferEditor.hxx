// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _OpenGl_VertexBufferEditor_H__
#define _OpenGl_VertexBufferEditor_H__

#include <OpenGl_VertexBuffer.hxx>
#include <OpenGl_Context.hxx>

#include <NCollection_Array1.hxx>

//! Auxiliary class to iteratively modify data of existing VBO.
//! It provides iteration interface with delayed CPU->GPU memory transfer to avoid slow per-element data transfer.
//! User should explicitly call Flush() method to ensure that all data is transferred to VBO.
//! Temporary buffer on CPU side can be initialized with lesser capacity than  VBO
//! to allow re-usage of shared buffer with fixed size between VBOs.
//!
//! You should use NCollection_Vec2/NCollection_Vec3/NCollection_Vec4 with appropriate length
//! to instantiate this template and access elements in VBO.
//!
//! Notice that this technique designed for VBO streaming scenarios (when VBO is modified from time to time).
//! Also this class doesn't retrieve existing data from VBO - data transferred only in one direction!
//! In case of static data this is preferred to upload it within one call during VBO initialization.
template<typename theVec_t>
class OpenGl_VertexBufferEditor
{

public:

  //! Creates empty editor
  //! theTmpBufferLength - temporary buffer length
  OpenGl_VertexBufferEditor (const Standard_Integer theTmpBufferLength = 0)
  : myElemFrom (0),
    myElemsNb (0),
    myTmpBuffer (0, theTmpBufferLength > 0 ? (theTmpBufferLength - 1) : 2047) {}

  //! Creates empty editor
  //! theTmpBuffer       - pointer to temporary buffer
  //! theTmpBufferLength - temporary buffer length
  OpenGl_VertexBufferEditor (theVec_t*              theTmpBuffer,
                             const Standard_Integer theTmpBufferLength)
  : myElemFrom (0),
    myElemsNb (0),
    myTmpBuffer (theTmpBuffer[0], 0, theTmpBufferLength - 1) {}

  //! Initialize editor for specified VBO.
  //! theGlCtx - bound OpenGL context to edit VBO
  //! theVbo   - VBO to edit
  Standard_Boolean Init (const Handle(OpenGl_Context)&      theGlCtx,
                         const Handle(OpenGl_VertexBuffer)& theVbo)
  {
    myGlCtx = theGlCtx;
    myVbo   = theVbo;
    if (myGlCtx.IsNull() || myVbo.IsNull() || !myVbo->IsValid() || myVbo->GetComponentsNb() != GLuint (theVec_t::Length()))
    {
      return Standard_False;
    }

    myElemFrom = myElemsNb = 0;
    return Standard_True;
  }

  //! Modify current element in VBO.
  theVec_t& Value()
  {
      return myTmpBuffer.ChangeValue (myElemsNb);
  }

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

      if (!myVbo->SubData (myGlCtx, myElemFrom, myElemsNb, &myTmpBuffer.Value (0)[0]))
      {
          // should never happens
          return Standard_False;
      }
      myElemFrom += myElemsNb;
      myElemsNb = 0;

      return Standard_True;
  }

private:

  Handle(OpenGl_Context)       myGlCtx;     //!< handle to current OpenGL context
  Handle(OpenGl_VertexBuffer)  myVbo;       //!< edited VBO
  Standard_Integer             myElemFrom;  //!< element in VBO to upload from
  Standard_Integer             myElemsNb;   //!< current element in temporary buffer
  NCollection_Array1<theVec_t> myTmpBuffer; //!< temporary array

};

#endif // _OpenGl_VertexBufferEditor_H__
