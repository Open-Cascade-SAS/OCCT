// Created by: Kirill GAVRILOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_BufferCompatT_HeaderFile
#define _OpenGl_BufferCompatT_HeaderFile

#include <NCollection_Buffer.hxx>
#include <OpenGl_Buffer.hxx>

//! Compatibility layer for old OpenGL without VBO.
//! Make sure to pass pointer from GetDataOffset() instead of NULL.
//! Method GetDataOffset() returns pointer to real data in this class
//! (while base class OpenGl_VertexBuffer always return NULL).
//!
//! Methods Bind()/Unbind() do nothing (do not affect OpenGL state)
//! and ::GetTarget() is never used.
//! For this reason there is no analog for OpenGl_IndexBuffer.
//! Just pass GetDataOffset() to glDrawElements() directly as last argument.
//!
//! Class overrides methods init() and subData() to copy data into own memory buffer.
//! Extra method initLink() might be used to pass existing buffer through handle without copying the
//! data.
//!
//! Method Create() creates dummy identifier for this object which should NOT be passed to OpenGL
//! functions.
template <class BaseBufferT>
class OpenGl_BufferCompatT : public BaseBufferT
{

public:
  //! Create uninitialized VBO.
  OpenGl_BufferCompatT()
  {
    //
  }

  //! Destroy object.
  virtual ~OpenGl_BufferCompatT() { Release(NULL); }

  //! Return TRUE.
  virtual bool IsVirtual() const override { return true; }

  //! Creates VBO name (id) if not yet generated.
  //! Data should be initialized by another method.
  inline bool Create(const occ::handle<OpenGl_Context>& theGlCtx) override;

  //! Destroy object - will release memory if any.
  inline virtual void Release(OpenGl_Context* theGlCtx) override;

  //! Bind this VBO.
  virtual void Bind(const occ::handle<OpenGl_Context>&) const override
  {
    //
  }

  //! Unbind this VBO.
  virtual void Unbind(const occ::handle<OpenGl_Context>&) const override
  {
    //
  }

public: //! @name advanced methods
  //! Initialize buffer with existing data.
  //! Data will NOT be copied by this method!
  inline bool initLink(const occ::handle<NCollection_Buffer>& theData,
                       const unsigned int                     theComponentsNb,
                       const int                              theElemsNb,
                       const unsigned int                     theDataType);

  //! Initialize buffer with new data (data will be copied).
  inline virtual bool init(const occ::handle<OpenGl_Context>& theGlCtx,
                           const unsigned int                 theComponentsNb,
                           const int                          theElemsNb,
                           const void*                        theData,
                           const unsigned int                 theDataType,
                           const int                          theStride) override;

  //! Update part of the buffer with new data.
  inline virtual bool subData(const occ::handle<OpenGl_Context>& theGlCtx,
                              const int                          theElemFrom,
                              const int                          theElemsNb,
                              const void*                        theData,
                              const unsigned int                 theDataType) override;

  //! Read back buffer sub-range.
  inline virtual bool getSubData(const occ::handle<OpenGl_Context>& theGlCtx,
                                 const int                          theElemFrom,
                                 const int                          theElemsNb,
                                 void*                              theData,
                                 const unsigned int                 theDataType) override;

protected:
  occ::handle<NCollection_Buffer> myData; //!< buffer data
};

//=================================================================================================

template <class BaseBufferT>
bool OpenGl_BufferCompatT<BaseBufferT>::Create(const occ::handle<OpenGl_Context>&)
{
  if (BaseBufferT::myBufferId == OpenGl_Buffer::NO_BUFFER)
  {
    BaseBufferT::myBufferId = (unsigned int)-1; // dummy identifier...
    myData                  = new NCollection_Buffer(Graphic3d_Buffer::DefaultAllocator());
  }
  return BaseBufferT::myBufferId != OpenGl_Buffer::NO_BUFFER;
}

//=================================================================================================

template <class BaseBufferT>
void OpenGl_BufferCompatT<BaseBufferT>::Release(OpenGl_Context*)
{
  if (BaseBufferT::myBufferId == OpenGl_Buffer::NO_BUFFER)
  {
    return;
  }

  BaseBufferT::myOffset   = NULL;
  BaseBufferT::myBufferId = OpenGl_Buffer::NO_BUFFER;
  myData.Nullify();
}

//=================================================================================================

template <class BaseBufferT>
bool OpenGl_BufferCompatT<BaseBufferT>::initLink(const occ::handle<NCollection_Buffer>& theData,
                                                 const unsigned int theComponentsNb,
                                                 const int          theElemsNb,
                                                 const unsigned int theDataType)
{
  if (theData.IsNull())
  {
    BaseBufferT::myOffset = NULL;
    return false;
  }

  if (BaseBufferT::myBufferId == OpenGl_Buffer::NO_BUFFER)
  {
    BaseBufferT::myBufferId = (unsigned int)-1; // dummy identifier...
  }
  myData                      = theData;
  BaseBufferT::myDataType     = theDataType;
  BaseBufferT::myComponentsNb = theComponentsNb;
  BaseBufferT::myElemsNb      = theElemsNb;
  BaseBufferT::myOffset       = myData->ChangeData();
  return true;
}

//=================================================================================================

template <class BaseBufferT>
bool OpenGl_BufferCompatT<BaseBufferT>::init(const occ::handle<OpenGl_Context>& theCtx,
                                             const unsigned int                 theComponentsNb,
                                             const int                          theElemsNb,
                                             const void*                        theData,
                                             const unsigned int                 theDataType,
                                             const int                          theStride)
{
  if (!Create(theCtx))
  {
    BaseBufferT::myOffset = NULL;
    return false;
  }

  BaseBufferT::myDataType     = theDataType;
  BaseBufferT::myComponentsNb = theComponentsNb;
  BaseBufferT::myElemsNb      = theElemsNb;

  const size_t aNbBytes = size_t(BaseBufferT::myElemsNb) * theStride;
  if (!myData->Allocate(aNbBytes))
  {
    BaseBufferT::myOffset = NULL;
    return false;
  }

  BaseBufferT::myOffset = myData->ChangeData();
  if (theData != NULL)
  {
    memcpy(myData->ChangeData(), theData, aNbBytes);
  }
  return true;
}

//=================================================================================================

template <class BaseBufferT>
bool OpenGl_BufferCompatT<BaseBufferT>::subData(const occ::handle<OpenGl_Context>&,
                                                const int          theElemFrom,
                                                const int          theElemsNb,
                                                const void*        theData,
                                                const unsigned int theDataType)
{
  if (!BaseBufferT::IsValid() || BaseBufferT::myDataType != theDataType || theElemFrom < 0
      || ((theElemFrom + theElemsNb) > BaseBufferT::myElemsNb))
  {
    return false;
  }
  else if (theData == NULL)
  {
    return true;
  }

  const size_t aDataSize = BaseBufferT::sizeOfGlType(theDataType);
  const size_t anOffset  = size_t(theElemFrom) * size_t(BaseBufferT::myComponentsNb) * aDataSize;
  const size_t aNbBytes  = size_t(theElemsNb) * size_t(BaseBufferT::myComponentsNb) * aDataSize;
  memcpy(myData->ChangeData() + anOffset, theData, aNbBytes);
  return true;
}

//=================================================================================================

template <class BaseBufferT>
bool OpenGl_BufferCompatT<BaseBufferT>::getSubData(const occ::handle<OpenGl_Context>&,
                                                   const int          theElemFrom,
                                                   const int          theElemsNb,
                                                   void*              theData,
                                                   const unsigned int theDataType)
{
  if (!BaseBufferT::IsValid() || BaseBufferT::myDataType != theDataType || theElemFrom < 0
      || ((theElemFrom + theElemsNb) > BaseBufferT::myElemsNb) || theData == NULL)
  {
    return false;
  }

  const size_t aDataSize = BaseBufferT::sizeOfGlType(theDataType);
  const size_t anOffset  = size_t(theElemFrom) * size_t(BaseBufferT::myComponentsNb) * aDataSize;
  const size_t aNbBytes  = size_t(theElemsNb) * size_t(BaseBufferT::myComponentsNb) * aDataSize;
  memcpy(theData, myData->Data() + anOffset, aNbBytes);
  return true;
}

#endif // _OpenGl_VertexBufferCompat_HeaderFile
