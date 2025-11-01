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

#ifndef _OpenGl_Buffer_H__
#define _OpenGl_Buffer_H__

#include <OpenGl_Resource.hxx>
#include <TCollection_AsciiString.hxx>

//! Buffer Object - is a general storage object for arbitrary data (see sub-classes).
class OpenGl_Buffer : public OpenGl_Resource
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_Buffer, OpenGl_Resource)
public:
  //! Helpful constants
  static const unsigned int NO_BUFFER = 0;

  //! Format VBO target enumeration value.
  Standard_EXPORT static TCollection_AsciiString FormatTarget(unsigned int theTarget);

public:
  //! Create uninitialized buffer.
  Standard_EXPORT OpenGl_Buffer();

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_Buffer();

  //! Return buffer target.
  virtual unsigned int GetTarget() const = 0;

  //! Return TRUE if this is a virtual (for backward compatibility) VBO object.
  virtual bool IsVirtual() const { return false; }

  //! @return true if current object was initialized
  bool IsValid() const { return myBufferId != NO_BUFFER; }

  //! @return the number of components per generic vertex attribute.
  unsigned int GetComponentsNb() const { return myComponentsNb; }

  //! @return number of vertex attributes / number of vertices specified within ::Init()
  Standard_Integer GetElemsNb() const { return myElemsNb; }

  //! Overrides the number of vertex attributes / number of vertexes.
  //! It is up to user specifying this number correct (e.g. below initial value)!
  void SetElemsNb(Standard_Integer theNbElems) { myElemsNb = theNbElems; }

  //! @return data type of each component in the array.
  unsigned int GetDataType() const { return myDataType; }

  //! @return offset to data, NULL by default
  Standard_Byte* GetDataOffset() const { return myOffset; }

  //! Creates buffer object name (id) if not yet generated.
  //! Data should be initialized by another method.
  Standard_EXPORT virtual bool Create(const Handle(OpenGl_Context)& theGlCtx);

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release(OpenGl_Context* theGlCtx) Standard_OVERRIDE;

  //! Bind this buffer object.
  Standard_EXPORT virtual void Bind(const Handle(OpenGl_Context)& theGlCtx) const;

  //! Unbind this buffer object.
  Standard_EXPORT virtual void Unbind(const Handle(OpenGl_Context)& theGlCtx) const;

  //! Notice that buffer object will be unbound after this call.
  //! @param[in] theComponentsNb  specifies the number of components per generic vertex attribute;
  //! must be 1, 2, 3, or 4;
  //! @param[in] theElemsNb       elements count;
  //! @param[in] theData          pointer to float data (vertices/normals etc.).
  Standard_EXPORT bool Init(const Handle(OpenGl_Context)& theGlCtx,
                            const unsigned int            theComponentsNb,
                            const Standard_Integer        theElemsNb,
                            const float*                  theData);

  //! Notice that buffer object will be unbound after this call.
  //! @param[in] theComponentsNb  specifies the number of components per generic vertex attribute;
  //! must be 1, 2, 3, or 4;
  //! @param[in] theElemsNb       elements count;
  //! @param[in] theData          pointer to unsigned int data (indices etc.).
  Standard_EXPORT bool Init(const Handle(OpenGl_Context)& theGlCtx,
                            const unsigned int            theComponentsNb,
                            const Standard_Integer        theElemsNb,
                            const unsigned int*           theData);

  //! Notice that buffer object will be unbound after this call.
  //! @param[in] theComponentsNb  specifies the number of components per generic vertex attribute;
  //! must be 1, 2, 3, or 4;
  //! @param[in] theElemsNb       elements count;
  //! @param[in] theData          pointer to unsigned short data (indices etc.).
  Standard_EXPORT bool Init(const Handle(OpenGl_Context)& theGlCtx,
                            const unsigned int            theComponentsNb,
                            const Standard_Integer        theElemsNb,
                            const unsigned short*         theData);

  //! Notice that buffer object will be unbound after this call.
  //! @param[in] theComponentsNb  specifies the number of components per generic vertex attribute;
  //! must be 1, 2, 3, or 4;
  //! @param[in] theElemsNb       elements count;
  //! @param[in] theData          pointer to Standard_Byte data (indices/colors etc.).
  Standard_EXPORT bool Init(const Handle(OpenGl_Context)& theGlCtx,
                            const unsigned int            theComponentsNb,
                            const Standard_Integer        theElemsNb,
                            const Standard_Byte*          theData);

  //! Notice that buffer object will be unbound after this call.
  //! Function replaces portion of data within this buffer object using glBufferSubData().
  //! The buffer object should be initialized before call.
  //! @param[in] theElemFrom  element id from which replace buffer data (>=0);
  //! @param[in] theElemsNb   elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param[in] theData      pointer to float data.
  Standard_EXPORT bool SubData(const Handle(OpenGl_Context)& theGlCtx,
                               const Standard_Integer        theElemFrom,
                               const Standard_Integer        theElemsNb,
                               const float*                  theData);

  //! Read back buffer sub-range.
  //! Notice that buffer object will be unbound after this call.
  //! Function reads portion of data from this buffer object using glGetBufferSubData().
  //! @param[in] theElemFrom  element id from which replace buffer data (>=0);
  //! @param[in] theElemsNb   elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param[out] theData     destination pointer to float data.
  Standard_EXPORT bool GetSubData(const Handle(OpenGl_Context)& theGlCtx,
                                  const Standard_Integer        theElemFrom,
                                  const Standard_Integer        theElemsNb,
                                  float*                        theData);

  //! Notice that buffer object will be unbound after this call.
  //! Function replaces portion of data within this buffer object using glBufferSubData().
  //! The buffer object should be initialized before call.
  //! @param[in] theElemFrom  element id from which replace buffer data (>=0);
  //! @param[in] theElemsNb   elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param[in] theData      pointer to unsigned int data.
  Standard_EXPORT bool SubData(const Handle(OpenGl_Context)& theGlCtx,
                               const Standard_Integer        theElemFrom,
                               const Standard_Integer        theElemsNb,
                               const unsigned int*           theData);

  //! Read back buffer sub-range.
  //! Notice that buffer object will be unbound after this call.
  //! Function reads portion of data from this buffer object using glGetBufferSubData().
  //! @param[in] theElemFrom  element id from which replace buffer data (>=0);
  //! @param[in] theElemsNb   elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param[out] theData     destination pointer to unsigned int data.
  Standard_EXPORT bool GetSubData(const Handle(OpenGl_Context)& theGlCtx,
                                  const Standard_Integer        theElemFrom,
                                  const Standard_Integer        theElemsNb,
                                  unsigned int*                 theData);

  //! Notice that buffer object will be unbound after this call.
  //! Function replaces portion of data within this buffer object using glBufferSubData().
  //! The buffer object should be initialized before call.
  //! @param[in] theElemFrom  element id from which replace buffer data (>=0);
  //! @param[in] theElemsNb   elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param[in] theData      pointer to unsigned short data.
  Standard_EXPORT bool SubData(const Handle(OpenGl_Context)& theGlCtx,
                               const Standard_Integer        theElemFrom,
                               const Standard_Integer        theElemsNb,
                               const unsigned short*         theData);

  //! Read back buffer sub-range.
  //! Notice that buffer object will be unbound after this call.
  //! Function reads portion of data from this buffer object using glGetBufferSubData().
  //! @param[in] theElemFrom  element id from which replace buffer data (>=0);
  //! @param[in] theElemsNb   elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param[out] theData     destination pointer to unsigned short data.
  Standard_EXPORT bool GetSubData(const Handle(OpenGl_Context)& theGlCtx,
                                  const Standard_Integer        theElemFrom,
                                  const Standard_Integer        theElemsNb,
                                  unsigned short*               theData);

  //! Notice that buffer object will be unbound after this call.
  //! Function replaces portion of data within this buffer object using glBufferSubData().
  //! The buffer object should be initialized before call.
  //! @param[in] theElemFrom  element id from which replace buffer data (>=0);
  //! @param[in] theElemsNb   elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param[in] theData      pointer to Standard_Byte data.
  Standard_EXPORT bool SubData(const Handle(OpenGl_Context)& theGlCtx,
                               const Standard_Integer        theElemFrom,
                               const Standard_Integer        theElemsNb,
                               const Standard_Byte*          theData);

  //! Read back buffer sub-range.
  //! Notice that buffer object will be unbound after this call.
  //! Function reads portion of data from this buffer object using glGetBufferSubData().
  //! @param[in] theElemFrom  element id from which replace buffer data (>=0);
  //! @param[in] theElemsNb   elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param[out] theData     destination pointer to Standard_Byte data.
  Standard_EXPORT bool GetSubData(const Handle(OpenGl_Context)& theGlCtx,
                                  const Standard_Integer        theElemFrom,
                                  const Standard_Integer        theElemsNb,
                                  Standard_Byte*                theData);

public: //! @name advanced methods
  //! Returns estimated GPU memory usage for holding data without considering overheads and
  //! allocation alignment rules.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE
  {
    return IsValid() ? sizeOfGlType(myDataType) * myComponentsNb * myElemsNb : 0;
  }

  //! @return size of specified GL type
  Standard_EXPORT static size_t sizeOfGlType(unsigned int theType);

  //! Initialize buffer with new data.
  Standard_EXPORT virtual bool init(const Handle(OpenGl_Context)& theGlCtx,
                                    const unsigned int            theComponentsNb,
                                    const Standard_Integer        theElemsNb,
                                    const void*                   theData,
                                    const unsigned int            theDataType,
                                    const Standard_Integer        theStride);

  //! Initialize buffer with new data.
  bool init(const Handle(OpenGl_Context)& theGlCtx,
            const unsigned int            theComponentsNb,
            const Standard_Integer        theElemsNb,
            const void*                   theData,
            const unsigned int            theDataType)
  {
    return init(theGlCtx,
                theComponentsNb,
                theElemsNb,
                theData,
                theDataType,
                Standard_Integer(theComponentsNb) * Standard_Integer(sizeOfGlType(theDataType)));
  }

  //! Update part of the buffer with new data.
  Standard_EXPORT virtual bool subData(const Handle(OpenGl_Context)& theGlCtx,
                                       const Standard_Integer        theElemFrom,
                                       const Standard_Integer        theElemsNb,
                                       const void*                   theData,
                                       const unsigned int            theDataType);

  //! Read back buffer sub-range.
  Standard_EXPORT virtual bool getSubData(const Handle(OpenGl_Context)& theGlCtx,
                                          const Standard_Integer        theElemFrom,
                                          const Standard_Integer        theElemsNb,
                                          void*                         theData,
                                          const unsigned int            theDataType);

public:
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  //! Binds a buffer object to an indexed buffer target.
  //! Wrapper for glBindBufferBase().
  //! @param[in] theGlCtx  active OpenGL context
  //! @param[in] theIndex  index to bind
  Standard_EXPORT void BindBufferBase(const Handle(OpenGl_Context)& theGlCtx,
                                      unsigned int                  theIndex);

  //! Unbinds a buffer object from an indexed buffer target.
  //! Wrapper for glBindBufferBase().
  //! @param[in] theGlCtx  active OpenGL context
  //! @param[in] theIndex  index to bind
  Standard_EXPORT void UnbindBufferBase(const Handle(OpenGl_Context)& theGlCtx,
                                        unsigned int                  theIndex);

  //! Binds a buffer object to an indexed buffer target with specified offset and size.
  //! Wrapper for glBindBufferRange().
  //! @param[in] theGlCtx   active OpenGL context
  //! @param[in] theIndex   index to bind (@sa GL_MAX_UNIFORM_BUFFER_BINDINGS in case of uniform
  //! buffer)
  //! @param[in] theOffset  offset within the buffer (@sa GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT in case
  //! of uniform buffer)
  //! @param[in] theSize    sub-section length starting from offset
  Standard_EXPORT void BindBufferRange(const Handle(OpenGl_Context)& theGlCtx,
                                       unsigned int                  theIndex,
                                       const intptr_t                theOffset,
                                       const size_t                  theSize);

protected:
  Standard_Byte* myOffset;   //!< offset to data
  unsigned int   myBufferId; //!< VBO name (index)
  // clang-format off
  unsigned int     myComponentsNb; //!< Number of components per generic vertex attribute, must be 1, 2, 3, or 4
  Standard_Integer myElemsNb;      //!< Number of vertex attributes / number of vertices
  unsigned int     myDataType;     //!< Data type (GL_FLOAT, GL_UNSIGNED_INT, GL_UNSIGNED_BYTE etc.)
  // clang-format on
};

DEFINE_STANDARD_HANDLE(OpenGl_Buffer, OpenGl_Resource)

#endif // _OpenGl_Buffer_H__
