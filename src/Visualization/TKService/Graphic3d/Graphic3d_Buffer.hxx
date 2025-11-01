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

#ifndef _Graphic3d_Buffer_HeaderFile
#define _Graphic3d_Buffer_HeaderFile

#include <Graphic3d_BufferRange.hxx>
#include <Graphic3d_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Buffer.hxx>
#include <Standard_NotImplemented.hxx>

//! Type of attribute in Vertex Buffer
enum Graphic3d_TypeOfAttribute
{
  Graphic3d_TOA_POS = 0, //!< vertex position
  Graphic3d_TOA_NORM,    //!< normal
  Graphic3d_TOA_UV,      //!< texture coordinates
  Graphic3d_TOA_COLOR,   //!< per-vertex color
  Graphic3d_TOA_CUSTOM,  //!< custom attributes
};

//! Type of the element in Vertex or Index Buffer
enum Graphic3d_TypeOfData
{
  Graphic3d_TOD_USHORT, //!< unsigned 16-bit integer
  Graphic3d_TOD_UINT,   //!< unsigned 32-bit integer
  Graphic3d_TOD_VEC2,   //!< 2-components float vector
  Graphic3d_TOD_VEC3,   //!< 3-components float vector
  Graphic3d_TOD_VEC4,   //!< 4-components float vector
  Graphic3d_TOD_VEC4UB, //!< 4-components unsigned byte vector
  Graphic3d_TOD_FLOAT,  //!< float value
};

//! Vertex attribute definition.
struct Graphic3d_Attribute
{
  // clang-format off
  Graphic3d_TypeOfAttribute Id;       //!< attribute identifier in vertex shader, 0 is reserved for vertex position
  // clang-format on
  Graphic3d_TypeOfData DataType; //!< vec2,vec3,vec4,vec4ub

  Standard_Integer Stride() const { return Stride(DataType); }

  //! @return size of attribute of specified data type
  static Standard_Integer Stride(const Graphic3d_TypeOfData theType)
  {
    switch (theType)
    {
      case Graphic3d_TOD_USHORT:
        return sizeof(unsigned short);
      case Graphic3d_TOD_UINT:
        return sizeof(unsigned int);
      case Graphic3d_TOD_VEC2:
        return sizeof(Graphic3d_Vec2);
      case Graphic3d_TOD_VEC3:
        return sizeof(Graphic3d_Vec3);
      case Graphic3d_TOD_VEC4:
        return sizeof(Graphic3d_Vec4);
      case Graphic3d_TOD_VEC4UB:
        return sizeof(Graphic3d_Vec4ub);
      case Graphic3d_TOD_FLOAT:
        return sizeof(float);
    }
    return 0;
  }
};

typedef NCollection_Array1<Graphic3d_Attribute> Graphic3d_Array1OfAttribute;

//! Buffer of vertex attributes.
class Graphic3d_Buffer : public NCollection_Buffer
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_Buffer, NCollection_Buffer)
public:
  //! Return default vertex data allocator.
  Standard_EXPORT static const Handle(NCollection_BaseAllocator)& DefaultAllocator();

public:
  //! Empty constructor.
  Graphic3d_Buffer(const Handle(NCollection_BaseAllocator)& theAlloc)
      : NCollection_Buffer(theAlloc),
        Stride(0),
        NbElements(0),
        NbAttributes(0)
  {
    //
  }

  //! Return number of initially allocated elements which can fit into this buffer,
  //! while NbElements can be overwritten to smaller value.
  Standard_Integer NbMaxElements() const
  {
    return Stride != 0 ? Standard_Integer(mySize / size_t(Stride)) : 0;
  }

  //! @return array of attributes definitions
  const Graphic3d_Attribute* AttributesArray() const
  {
    return (Graphic3d_Attribute*)(myData + mySize);
  }

  //! @return attribute definition
  const Graphic3d_Attribute& Attribute(const Standard_Integer theAttribIndex) const
  {
    return AttributesArray()[theAttribIndex];
  }

  //! @return attribute definition
  Graphic3d_Attribute& ChangeAttribute(const Standard_Integer theAttribIndex)
  {
    return *((Graphic3d_Attribute*)(myData + mySize) + theAttribIndex);
  }

  //! Find attribute index.
  //! @param theAttrib attribute to find
  //! @return attribute index or -1 if not found
  Standard_Integer FindAttribute(Graphic3d_TypeOfAttribute theAttrib) const
  {
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribute(anAttribIter);
      if (anAttrib.Id == theAttrib)
      {
        return anAttribIter;
      }
    }
    return -1;
  }

  //! @name data accessors for interleaved array
public:
  //! @return data offset to specified attribute
  Standard_Integer AttributeOffset(const Standard_Integer theAttribIndex) const
  {
    Standard_Integer anOffset = 0;
    for (Standard_Integer anAttribIter = 0; anAttribIter < theAttribIndex; ++anAttribIter)
    {
      anOffset += Graphic3d_Attribute::Stride(Attribute(anAttribIter).DataType);
    }
    return anOffset;
  }

  //! @return data for specified attribute
  const Standard_Byte* Data(const Standard_Integer theAttribIndex) const
  {
    return myData + AttributeOffset(theAttribIndex);
  }

  //! @return data for specified attribute
  Standard_Byte* ChangeData(const Standard_Integer theAttribIndex)
  {
    return myData + AttributeOffset(theAttribIndex);
  }

  //! Access specified element.
  inline const Standard_Byte* value(const Standard_Integer theElem) const
  {
    return myData + Stride * size_t(theElem);
  }

  //! Access specified element.
  inline Standard_Byte* changeValue(const Standard_Integer theElem)
  {
    return myData + Stride * size_t(theElem);
  }

  //! Access element with specified position and type.
  template <typename Type_t>
  inline const Type_t& Value(const Standard_Integer theElem) const
  {
    return *reinterpret_cast<const Type_t*>(value(theElem));
  }

  //! Access element with specified position and type.
  template <typename Type_t>
  inline Type_t& ChangeValue(const Standard_Integer theElem)
  {
    return *reinterpret_cast<Type_t*>(changeValue(theElem));
  }

  //! @name general accessors
public:
  using NCollection_Buffer::ChangeData;
  using NCollection_Buffer::Data;

  //! Return the attribute data with stride size specific to this attribute.
  //! @param theAttrib       attribute to find
  //! @param theAttribIndex  index of found attribute
  //! @param theAttribStride stride in bytes between values of this attribute within returned data
  //! pointer
  Standard_Byte* ChangeAttributeData(Graphic3d_TypeOfAttribute theAttrib,
                                     Standard_Integer&         theAttribIndex,
                                     Standard_Size&            theAttribStride)
  {
    return (Standard_Byte*)AttributeData(theAttrib, theAttribIndex, theAttribStride);
  }

  //! Return the attribute data with stride size specific to this attribute.
  //! @param theAttrib       attribute to find
  //! @param theAttribIndex  index of found attribute
  //! @param theAttribStride stride in bytes between values of this attribute within returned data
  //! pointer
  const Standard_Byte* AttributeData(Graphic3d_TypeOfAttribute theAttrib,
                                     Standard_Integer&         theAttribIndex,
                                     Standard_Size&            theAttribStride) const
  {
    const Standard_Byte* aDataPtr = Data();
    if (IsInterleaved())
    {
      for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
      {
        const Graphic3d_Attribute& anAttrib       = Attribute(anAttribIter);
        const Standard_Size        anAttribStride = Graphic3d_Attribute::Stride(anAttrib.DataType);
        if (anAttrib.Id == theAttrib)
        {
          theAttribIndex  = anAttribIter;
          theAttribStride = Stride;
          return aDataPtr;
        }

        aDataPtr += anAttribStride;
      }
    }
    else
    {
      const Standard_Integer aNbMaxVerts = NbMaxElements();
      for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
      {
        const Graphic3d_Attribute& anAttrib       = Attribute(anAttribIter);
        const Standard_Size        anAttribStride = Graphic3d_Attribute::Stride(anAttrib.DataType);
        if (anAttrib.Id == theAttrib)
        {
          theAttribIndex  = anAttribIter;
          theAttribStride = anAttribStride;
          return aDataPtr;
        }

        aDataPtr += anAttribStride * aNbMaxVerts;
      }
    }
    return NULL;
  }

public:
  //! Release buffer.
  void release()
  {
    Free();
    Stride       = 0;
    NbElements   = 0;
    NbAttributes = 0;
  }

  //! Allocates new empty array
  bool Init(const Standard_Integer     theNbElems,
            const Graphic3d_Attribute* theAttribs,
            const Standard_Integer     theNbAttribs)
  {
    release();
    Standard_Integer aStride = 0;
    for (Standard_Integer anAttribIter = 0; anAttribIter < theNbAttribs; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = theAttribs[anAttribIter];
      aStride += anAttrib.Stride();
    }
    if (aStride == 0)
    {
      return false;
    }

    Stride       = aStride;
    NbElements   = theNbElems;
    NbAttributes = theNbAttribs;
    if (NbElements != 0)
    {
      const size_t aDataSize = size_t(Stride) * size_t(NbElements);
      if (!Allocate(aDataSize + sizeof(Graphic3d_Attribute) * NbAttributes))
      {
        release();
        return false;
      }

      mySize = aDataSize;
      for (Standard_Integer anAttribIter = 0; anAttribIter < theNbAttribs; ++anAttribIter)
      {
        ChangeAttribute(anAttribIter) = theAttribs[anAttribIter];
      }
    }
    return true;
  }

  //! Allocates new empty array
  bool Init(const Standard_Integer theNbElems, const Graphic3d_Array1OfAttribute& theAttribs)
  {
    return Init(theNbElems, &theAttribs.First(), theAttribs.Size());
  }

public:
  //! Flag indicating that attributes in the buffer are interleaved; TRUE by default.
  //! Requires sub-classing for creating a non-interleaved buffer (advanced usage).
  virtual Standard_Boolean IsInterleaved() const { return Standard_True; }

  //! Return TRUE if data can be invalidated; FALSE by default.
  //! Requires sub-classing for creating a mutable buffer (advanced usage).
  virtual Standard_Boolean IsMutable() const { return Standard_False; }

  //! Return invalidated range; EMPTY by default.
  //! Requires sub-classing for creating a mutable buffer (advanced usage).
  virtual Graphic3d_BufferRange InvalidatedRange() const { return Graphic3d_BufferRange(); }

  //! Reset invalidated range.
  //! Requires sub-classing for creating a mutable buffer (advanced usage).
  virtual void Validate() {}

  //! Invalidate entire buffer.
  virtual void Invalidate() {}

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

public:
  // clang-format off
  Standard_Integer Stride;       //!< the distance to the attributes of the next vertex within interleaved array
  Standard_Integer NbElements;   //!< number of the elements (@sa NbMaxElements() specifying the number of initially allocated number of elements)
  // clang-format on
  Standard_Integer NbAttributes; //!< number of vertex attributes
};

DEFINE_STANDARD_HANDLE(Graphic3d_Buffer, NCollection_Buffer)

#endif // _Graphic3d_Buffer_HeaderFile
