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

#ifndef _Graphic3d_TextureParams_HeaderFile
#define _Graphic3d_TextureParams_HeaderFile

#include <Graphic3d_LevelOfTextureAnisotropy.hxx>
#include <NCollection_Vec2.hxx>
#include <Standard_TypeDef.hxx>
#include <NCollection_Vec4.hxx>
#include <Graphic3d_TextureUnit.hxx>
#include <Graphic3d_TypeOfTextureFilter.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Standard.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

//! This class describes texture parameters.
class Graphic3d_TextureParams : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_TextureParams, Standard_Transient)
public:
  //! Default constructor.
  Standard_EXPORT Graphic3d_TextureParams();

  //! Destructor.
  Standard_EXPORT ~Graphic3d_TextureParams() override;

  //! Default texture unit to be used, default is Graphic3d_TextureUnit_BaseColor.
  Graphic3d_TextureUnit TextureUnit() const { return myTextureUnit; }

  //! Setup default texture unit.
  void SetTextureUnit(Graphic3d_TextureUnit theUnit) { myTextureUnit = theUnit; }

  //! @return TRUE if the texture is modulate.
  //! Default value is FALSE.
  bool IsModulate() const { return myToModulate; }

  //! @param theToModulate turn modulation on/off.
  Standard_EXPORT void SetModulate(const bool theToModulate);

  //! @return TRUE if the texture repeat is enabled.
  //! Default value is FALSE.
  bool IsRepeat() const { return myToRepeat; }

  //! @param theToRepeat turn texture repeat mode ON or OFF (clamping).
  Standard_EXPORT void SetRepeat(const bool theToRepeat);

  //! @return texture interpolation filter.
  //! Default value is Graphic3d_TOTF_NEAREST.
  Graphic3d_TypeOfTextureFilter Filter() const { return myFilter; }

  //! @param theFilter texture interpolation filter.
  Standard_EXPORT void SetFilter(const Graphic3d_TypeOfTextureFilter theFilter);

  //! @return level of anisontropy texture filter.
  //! Default value is Graphic3d_LOTA_OFF.
  Graphic3d_LevelOfTextureAnisotropy AnisoFilter() const { return myAnisoLevel; }

  //! @param theLevel level of anisontropy texture filter.
  Standard_EXPORT void SetAnisoFilter(const Graphic3d_LevelOfTextureAnisotropy theLevel);

  //! Return rotation angle in degrees; 0 by default.
  //! Complete transformation matrix: Rotation -> Translation -> Scale.
  float Rotation() const { return myRotAngle; }

  //! @param theAngleDegrees rotation angle.
  Standard_EXPORT void SetRotation(const float theAngleDegrees);

  //! Return scale factor; (1.0; 1.0) by default, which means no scaling.
  //! Complete transformation matrix: Rotation -> Translation -> Scale.
  const NCollection_Vec2<float>& Scale() const { return myScale; }

  //! @param theScale scale factor.
  Standard_EXPORT void SetScale(const NCollection_Vec2<float> theScale);

  //! Return translation vector; (0.0; 0.0), which means no translation.
  //! Complete transformation matrix: Rotation -> Translation -> Scale.
  const NCollection_Vec2<float>& Translation() const { return myTranslation; }

  //! @param theVec translation vector.
  Standard_EXPORT void SetTranslation(const NCollection_Vec2<float> theVec);

  //! @return texture coordinates generation mode.
  //! Default value is Graphic3d_TOTM_MANUAL.
  Graphic3d_TypeOfTextureMode GenMode() const { return myGenMode; }

  //! @return texture coordinates generation plane S.
  const NCollection_Vec4<float>& GenPlaneS() const { return myGenPlaneS; }

  //! @return texture coordinates generation plane T.
  const NCollection_Vec4<float>& GenPlaneT() const { return myGenPlaneT; }

  //! Setup texture coordinates generation mode.
  Standard_EXPORT void SetGenMode(const Graphic3d_TypeOfTextureMode theMode,
                                  const NCollection_Vec4<float>     thePlaneS,
                                  const NCollection_Vec4<float>     thePlaneT);

  //! @return base texture mipmap level; 0 by default.
  int BaseLevel() const { return myBaseLevel; }

  //! Return maximum texture mipmap array level; 1000 by default.
  //! Real rendering limit will take into account mipmap generation flags and presence of mipmaps in
  //! loaded image.
  int MaxLevel() const { return myMaxLevel; }

  //! Setups texture mipmap array levels range.
  //! The lowest value will be the base level.
  //! The remaining one will be the maximum level.
  void SetLevelsRange(int theFirstLevel, int theSecondLevel = 0)
  {
    myMaxLevel  = theFirstLevel > theSecondLevel ? theFirstLevel : theSecondLevel;
    myBaseLevel = theFirstLevel > theSecondLevel ? theSecondLevel : theFirstLevel;
  }

  //! Return modification counter of parameters related to sampler state.
  unsigned int SamplerRevision() const { return mySamplerRevision; }

private:
  //! Increment revision.
  void updateSamplerRevision() { ++mySamplerRevision; }

private:
  // clang-format off
  NCollection_Vec4<float>                     myGenPlaneS;       //!< texture coordinates generation plane S
  NCollection_Vec4<float>                     myGenPlaneT;       //!< texture coordinates generation plane T
  NCollection_Vec2<float>                     myScale;           //!< texture coordinates scale factor vector; (1,1) by default
  NCollection_Vec2<float>                     myTranslation;     //!< texture coordinates translation vector;  (0,0) by default
  unsigned int                       mySamplerRevision; //!< modification counter of parameters related to sampler state
  Graphic3d_TextureUnit              myTextureUnit;     //!< default texture unit to bind texture; Graphic3d_TextureUnit_BaseColor by default
  Graphic3d_TypeOfTextureFilter      myFilter;          //!< texture filter, Graphic3d_TOTF_NEAREST by default
  Graphic3d_LevelOfTextureAnisotropy myAnisoLevel;      //!< level of anisotropy filter, Graphic3d_LOTA_OFF by default
  Graphic3d_TypeOfTextureMode        myGenMode;         //!< texture coordinates generation mode, Graphic3d_TOTM_MANUAL by default
  int                   myBaseLevel;       //!< base texture mipmap level (0 by default)
  int                   myMaxLevel;        //!< maximum texture mipmap array level (1000 by default)
  float                 myRotAngle;        //!< texture coordinates rotation angle in degrees, 0 by default
  bool                   myToModulate;      //!< flag to modulate texture with material color, FALSE by default
  bool                   myToRepeat;        //!< flag to repeat (true) or wrap (false) texture coordinates out of [0,1] range
  // clang-format on
};

#endif // _Graphic3d_TextureParams_HeaderFile
