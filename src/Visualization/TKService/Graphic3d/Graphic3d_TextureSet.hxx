// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _Graphic3d_TextureSet_HeaderFile
#define _Graphic3d_TextureSet_HeaderFile

#include <Graphic3d_TextureMap.hxx>
#include <NCollection_Array1.hxx>

//! Class holding array of textures to be mapped as a set.
//! Textures should be defined in ascending order of texture units within the set.
class Graphic3d_TextureSet : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_TextureSet, Standard_Transient)
public:
  //! Class for iterating texture set.
  class Iterator : public NCollection_Array1<occ::handle<Graphic3d_TextureMap>>::Iterator
  {
  public:
    //! Empty constructor.
    Iterator() = default;

    //! Constructor.
    Iterator(const occ::handle<Graphic3d_TextureSet>& theSet)
    {
      if (!theSet.IsNull())
      {
        NCollection_Array1<occ::handle<Graphic3d_TextureMap>>::Iterator::Init(theSet->myTextures);
      }
    }
  };

public:
  //! Empty constructor.
  Graphic3d_TextureSet() = default;

  //! Constructor.
  Graphic3d_TextureSet(int theNbTextures)
      : myTextures(0, theNbTextures - 1)
  {
  }

  //! Constructor for a single texture.
  Graphic3d_TextureSet(const occ::handle<Graphic3d_TextureMap>& theTexture)
      : myTextures(0, 0)
  {
    myTextures.ChangeFirst() = theTexture;
  }

  //! Return TRUE if texture array is empty.
  bool IsEmpty() const { return myTextures.IsEmpty(); }

  //! Return number of textures.
  int Size() const { return myTextures.Size(); }

  //! Return the lower index in texture set.
  int Lower() const { return myTextures.Lower(); }

  //! Return the upper index in texture set.
  int Upper() const { return myTextures.Upper(); }

  //! Return the first texture.
  const occ::handle<Graphic3d_TextureMap>& First() const { return myTextures.First(); }

  //! Return the first texture.
  void SetFirst(const occ::handle<Graphic3d_TextureMap>& theTexture)
  {
    myTextures.ChangeFirst() = theTexture;
  }

  //! Return the texture at specified position within [0, Size()) range.
  const occ::handle<Graphic3d_TextureMap>& Value(int theIndex) const
  {
    return myTextures.Value(theIndex);
  }

  //! Return the texture at specified position within [0, Size()) range.
  void SetValue(int theIndex, const occ::handle<Graphic3d_TextureMap>& theTexture)
  {
    myTextures.SetValue(theIndex, theTexture);
  }

protected:
  NCollection_Array1<occ::handle<Graphic3d_TextureMap>> myTextures;
};

#endif // _Graphic3d_TextureSet_HeaderFile
