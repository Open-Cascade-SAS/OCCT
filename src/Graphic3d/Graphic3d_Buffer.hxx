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

#include <Graphic3d_Vec.hxx>
#include <NCollection_Buffer.hxx>

//! Type of attribute in Vertex Buffer
enum Graphic3d_TypeOfAttribute
{
  Graphic3d_TOA_POS   =  0,   //!< vertex position
  Graphic3d_TOA_NORM,         //!< normal
  Graphic3d_TOA_UV,           //!< texture coordinates
  Graphic3d_TOA_COLOR,        //!< per-vertex color
  Graphic3d_TOA_CUSTOM = 10,  //!< custom attributes
};

//! Type of the element in Vertex or Index Buffer
enum Graphic3d_TypeOfData
{
  Graphic3d_TOD_USHORT,  //!< unsigned 16-bit integer
  Graphic3d_TOD_UINT,    //!< unsigned 32-bit integer
  Graphic3d_TOD_VEC2,    //!< 2-components float vector
  Graphic3d_TOD_VEC3,    //!< 3-components float vector
  Graphic3d_TOD_VEC4,    //!< 4-components float vector
  Graphic3d_TOD_VEC4UB,  //!< 4-components unsigned byte vector
};

//! Vertex attribute definition.
struct Graphic3d_Attribute
{
  Graphic3d_TypeOfAttribute Id;       //!< attribute identifier in vertex shader, 0 is reserved for vertex position
  Graphic3d_TypeOfData      DataType; //!< vec2,vec3,vec4,vec4ub

  Standard_Integer Stride() const { return Stride (DataType); }

  //! @return size of attribute of specified data type
  static Standard_Integer Stride (const Graphic3d_TypeOfData theType)
  {
    switch (theType)
    {
      case Graphic3d_TOD_USHORT: return sizeof(unsigned short);
      case Graphic3d_TOD_UINT:   return sizeof(unsigned int);
      case Graphic3d_TOD_VEC2:   return sizeof(Graphic3d_Vec2);
      case Graphic3d_TOD_VEC3:   return sizeof(Graphic3d_Vec3);
      case Graphic3d_TOD_VEC4:   return sizeof(Graphic3d_Vec4);
      case Graphic3d_TOD_VEC4UB: return sizeof(Graphic3d_Vec4ub);
    }
    return 0;
  }

};

//! Buffer of vertex attributes.
class Graphic3d_Buffer : public NCollection_Buffer
{
public:

  //! Empty constructor.
  Graphic3d_Buffer (const Handle(NCollection_BaseAllocator)& theAlloc)
  : NCollection_Buffer (theAlloc),
    Stride       (0),
    NbElements   (0),
    NbAttributes (0)
  {
    //
  }

  //! @return array of attributes definitions
  const Graphic3d_Attribute* AttributesArray() const
  {
    return (Graphic3d_Attribute* )(myData + mySize);
  }

  //! @return attribute definition
  const Graphic3d_Attribute& Attribute (const Standard_Integer theAttribIndex) const
  {
    return AttributesArray()[theAttribIndex];
  }

  //! @return attribute definition
  Graphic3d_Attribute& ChangeAttribute (const Standard_Integer theAttribIndex)
  {
    return *((Graphic3d_Attribute* )(myData + mySize) + theAttribIndex);
  }

  //! @return data offset to specified attribute
  Standard_Integer AttributeOffset (const Standard_Integer theAttribIndex) const
  {
    Standard_Integer anOffset = 0;
    for (Standard_Integer anAttribIter = 0; anAttribIter < theAttribIndex; ++anAttribIter)
    {
      anOffset += Graphic3d_Attribute::Stride (Attribute (anAttribIter).DataType);
    }
    return anOffset;
  }

  using NCollection_Buffer::Data;
  using NCollection_Buffer::ChangeData;

  //! @return data for specified attribute
  const Standard_Byte* Data (const Standard_Integer theAttribIndex) const
  {
    return myData + AttributeOffset (theAttribIndex);
  }

  //! @return data for specified attribute
  Standard_Byte* ChangeData (const Standard_Integer theAttribIndex)
  {
    return myData + AttributeOffset (theAttribIndex);
  }

  //! Access specified element.
  inline const Standard_Byte* value (const Standard_Integer theElem) const
  {
    return myData + Stride * size_t(theElem);
  }

  //! Access specified element.
  inline Standard_Byte* changeValue (const Standard_Integer theElem)
  {
    return myData + Stride * size_t(theElem);
  }

  //! Access element with specified position and type.
  template <typename Type_t>
  inline const Type_t& Value (const Standard_Integer theElem) const
  {
    return *reinterpret_cast<const Type_t*>(value (theElem));
  }

  //! Access element with specified position and type.
  template <typename Type_t>
  inline Type_t& ChangeValue (const Standard_Integer theElem)
  {
    return *reinterpret_cast<Type_t* >(changeValue (theElem));
  }

  //! Release buffer.
  void release()
  {
    Free();
    Stride       = 0;
    NbElements   = 0;
    NbAttributes = 0;
  }

  //! Allocates new empty array
  bool Init (const Standard_Integer     theNbElems,
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
      if (!Allocate (aDataSize + sizeof(Graphic3d_Attribute) * NbAttributes))
      {
        release();
        return false;
      }

      mySize = aDataSize;
      for (Standard_Integer anAttribIter = 0; anAttribIter < theNbAttribs; ++anAttribIter)
      {
        ChangeAttribute (anAttribIter) = theAttribs[anAttribIter];
      }
    }
    return true;
  }

public:

  Standard_Integer Stride;       //!< the distance to the attributes of the next vertex
  Standard_Integer NbElements;   //!< number of the elements
  Standard_Integer NbAttributes; //!< number of vertex attributes

public:

  DEFINE_STANDARD_RTTI(Graphic3d_Buffer) // Type definition

};

DEFINE_STANDARD_HANDLE(Graphic3d_Buffer, NCollection_Buffer)

#endif // _Graphic3d_Buffer_HeaderFile
