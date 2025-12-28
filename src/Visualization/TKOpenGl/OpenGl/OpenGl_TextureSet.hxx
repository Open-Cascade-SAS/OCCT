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

#ifndef _OpenGl_TextureSet_Header
#define _OpenGl_TextureSet_Header

#include <Graphic3d_TextureSet.hxx>
#include <Graphic3d_TextureSetBits.hxx>

class OpenGl_Texture;

//! Class holding array of textures to be mapped as a set.
//! Textures should be defined in ascending order of texture units within the set.
class OpenGl_TextureSet : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_TextureSet, Standard_Transient)
public:
  //! Texture slot - combination of Texture and binding Unit.
  struct TextureSlot
  {
    occ::handle<OpenGl_Texture> Texture;
    Graphic3d_TextureUnit       Unit;

    operator const occ::handle<OpenGl_Texture>&() const { return Texture; }

    operator occ::handle<OpenGl_Texture>&() { return Texture; }

    TextureSlot()
        : Unit(Graphic3d_TextureUnit_0)
    {
    }
  };

  //! Class for iterating texture set.
  class Iterator : public NCollection_Array1<TextureSlot>::Iterator
  {
  public:
    //! Empty constructor.
    Iterator() {}

    //! Constructor.
    Iterator(const occ::handle<OpenGl_TextureSet>& theSet)
    {
      if (!theSet.IsNull())
      {
        NCollection_Array1<TextureSlot>::Iterator::Init(theSet->myTextures);
      }
    }

    //! Access texture.
    const occ::handle<OpenGl_Texture>& Value() const
    {
      return NCollection_Array1<TextureSlot>::Iterator::Value().Texture;
    }

    occ::handle<OpenGl_Texture>& ChangeValue()
    {
      return NCollection_Array1<TextureSlot>::Iterator::ChangeValue().Texture;
    }

    //! Access texture unit.
    Graphic3d_TextureUnit Unit() const
    {
      return NCollection_Array1<TextureSlot>::Iterator::Value().Unit;
    }

    Graphic3d_TextureUnit& ChangeUnit()
    {
      return NCollection_Array1<TextureSlot>::Iterator::ChangeValue().Unit;
    }
  };

public:
  //! Empty constructor.
  OpenGl_TextureSet()
      : myTextureSetBits(Graphic3d_TextureSetBits_NONE)
  {
  }

  //! Constructor.
  OpenGl_TextureSet(int theNbTextures)
      : myTextures(0, theNbTextures - 1),
        myTextureSetBits(Graphic3d_TextureSetBits_NONE)
  {
  }

  //! Constructor for a single texture.
  Standard_EXPORT OpenGl_TextureSet(const occ::handle<OpenGl_Texture>& theTexture);

  //! Return texture units declared within the program, @sa Graphic3d_TextureSetBits.
  int TextureSetBits() const { return myTextureSetBits; }

  //! Return texture units declared within the program, @sa Graphic3d_TextureSetBits.
  int& ChangeTextureSetBits() { return myTextureSetBits; }

  //! Return TRUE if texture array is empty.
  bool IsEmpty() const { return myTextures.IsEmpty(); }

  //! Return number of textures.
  int Size() const { return myTextures.Size(); }

  //! Return the lower index in texture set.
  int Lower() const { return myTextures.Lower(); }

  //! Return the upper index in texture set.
  int Upper() const { return myTextures.Upper(); }

  //! Return the first texture.
  const occ::handle<OpenGl_Texture>& First() const { return myTextures.First().Texture; }

  //! Return the first texture.
  occ::handle<OpenGl_Texture>& ChangeFirst() { return myTextures.ChangeFirst().Texture; }

  //! Return the first texture unit.
  Graphic3d_TextureUnit FirstUnit() const { return myTextures.First().Unit; }

  //! Return the last texture.
  const occ::handle<OpenGl_Texture>& Last() const { return myTextures.Last().Texture; }

  //! Return the last texture.
  occ::handle<OpenGl_Texture>& ChangeLast() { return myTextures.ChangeLast().Texture; }

  //! Return the last texture unit.
  Graphic3d_TextureUnit LastUnit() const { return myTextures.Last().Unit; }

  //! Return the last texture unit.
  Graphic3d_TextureUnit& ChangeLastUnit() { return myTextures.ChangeLast().Unit; }

  //! Return the texture at specified position within [0, Size()) range.
  const occ::handle<OpenGl_Texture>& Value(int theIndex) const
  {
    return myTextures.Value(theIndex).Texture;
  }

  //! Return the texture at specified position within [0, Size()) range.
  occ::handle<OpenGl_Texture>& ChangeValue(int theIndex)
  {
    return myTextures.ChangeValue(theIndex).Texture;
  }

  //! Return TRUE if texture color modulation has been enabled for the first texture
  //! or if texture is not set at all.
  Standard_EXPORT bool IsModulate() const;

  //! Return TRUE if other than point sprite textures are defined within point set.
  Standard_EXPORT bool HasNonPointSprite() const;

  //! Return TRUE if last texture is a point sprite.
  Standard_EXPORT bool HasPointSprite() const;

  //! Nullify all handles.
  void InitZero()
  {
    myTextures.Init(TextureSlot());
    myTextureSetBits = Graphic3d_TextureSetBits_NONE;
  }

protected:
  NCollection_Array1<TextureSlot> myTextures;
  int                             myTextureSetBits;
};

#endif //_OpenGl_TextureSet_Header
