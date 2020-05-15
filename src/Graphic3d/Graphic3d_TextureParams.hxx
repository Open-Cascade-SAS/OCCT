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
#include <Graphic3d_Vec2.hxx>
#include <Graphic3d_Vec4.hxx>
#include <Graphic3d_TextureUnit.hxx>
#include <Graphic3d_TypeOfTextureFilter.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
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
  Standard_EXPORT virtual ~Graphic3d_TextureParams();

  //! Default texture unit to be used, default is Graphic3d_TextureUnit_BaseColor.
  Graphic3d_TextureUnit TextureUnit() const { return myTextureUnit; }

  //! Setup default texture unit.
  void SetTextureUnit (Graphic3d_TextureUnit theUnit) { myTextureUnit = theUnit; }

  //! @return TRUE if the texture is modulate.
  //! Default value is FALSE.
  Standard_Boolean IsModulate() const { return myToModulate; }
  
  //! @param theToModulate turn modulation on/off.
  Standard_EXPORT void SetModulate (const Standard_Boolean theToModulate);
  
  //! @return TRUE if the texture repeat is enabled.
  //! Default value is FALSE.
  Standard_Boolean IsRepeat() const { return myToRepeat; }
  
  //! @param theToRepeat turn texture repeat mode ON or OFF (clamping).
  Standard_EXPORT void SetRepeat (const Standard_Boolean theToRepeat);
  
  //! @return texture interpolation filter.
  //! Default value is Graphic3d_TOTF_NEAREST.
  Graphic3d_TypeOfTextureFilter Filter() const { return myFilter; }
  
  //! @param theFilter texture interpolation filter.
  Standard_EXPORT void SetFilter (const Graphic3d_TypeOfTextureFilter theFilter);
  
  //! @return level of anisontropy texture filter.
  //! Default value is Graphic3d_LOTA_OFF.
  Graphic3d_LevelOfTextureAnisotropy AnisoFilter() const { return myAnisoLevel; }
  
  //! @param theLevel level of anisontropy texture filter.
  Standard_EXPORT void SetAnisoFilter (const Graphic3d_LevelOfTextureAnisotropy theLevel);
  
  //! Return rotation angle in degrees; 0 by default.
  //! Complete transformation matrix: Rotation -> Translation -> Scale.
  Standard_ShortReal Rotation() const { return myRotAngle; }
  
  //! @param theAngleDegrees rotation angle.
  Standard_EXPORT void SetRotation (const Standard_ShortReal theAngleDegrees);
  
  //! Return scale factor; (1.0; 1.0) by default, which means no scaling.
  //! Complete transformation matrix: Rotation -> Translation -> Scale.
  const Graphic3d_Vec2& Scale() const { return myScale; }
  
  //! @param theScale scale factor.
  Standard_EXPORT void SetScale (const Graphic3d_Vec2 theScale);

  //! Return translation vector; (0.0; 0.0), which means no translation.
  //! Complete transformation matrix: Rotation -> Translation -> Scale.
  const Graphic3d_Vec2& Translation() const { return myTranslation; }
  
  //! @param theVec translation vector.
  Standard_EXPORT void SetTranslation (const Graphic3d_Vec2 theVec);
  
  //! @return texture coordinates generation mode.
  //! Default value is Graphic3d_TOTM_MANUAL.
  Graphic3d_TypeOfTextureMode GenMode() const { return myGenMode; }
  
  //! @return texture coordinates generation plane S.
  const Graphic3d_Vec4& GenPlaneS() const { return myGenPlaneS; }
  
  //! @return texture coordinates generation plane T.
  const Graphic3d_Vec4& GenPlaneT() const { return myGenPlaneT; }
  
  //! Setup texture coordinates generation mode.
  Standard_EXPORT void SetGenMode (const Graphic3d_TypeOfTextureMode theMode, const Graphic3d_Vec4 thePlaneS, const Graphic3d_Vec4 thePlaneT);

  //! @return base texture mipmap level; 0 by default.
  Standard_Integer BaseLevel() const { return myBaseLevel; }

  //! Return maximum texture mipmap array level; 1000 by default.
  //! Real rendering limit will take into account mipmap generation flags and presence of mipmaps in loaded image.
  Standard_Integer MaxLevel() const { return myMaxLevel; }

  //! Setups texture mipmap array levels range.
  //! The lowest value will be the base level.
  //! The remaining one will be the maximum level.
  void SetLevelsRange (Standard_Integer theFirstLevel, Standard_Integer theSecondLevel = 0)
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

  Graphic3d_Vec4                     myGenPlaneS;       //!< texture coordinates generation plane S
  Graphic3d_Vec4                     myGenPlaneT;       //!< texture coordinates generation plane T
  Graphic3d_Vec2                     myScale;           //!< texture coordinates scale factor vector; (1,1) by default
  Graphic3d_Vec2                     myTranslation;     //!< texture coordinates translation vector;  (0,0) by default
  unsigned int                       mySamplerRevision; //!< modification counter of parameters related to sampler state
  Graphic3d_TextureUnit              myTextureUnit;     //!< default texture unit to bind texture; Graphic3d_TextureUnit_BaseColor by default
  Graphic3d_TypeOfTextureFilter      myFilter;          //!< texture filter, Graphic3d_TOTF_NEAREST by default
  Graphic3d_LevelOfTextureAnisotropy myAnisoLevel;      //!< level of anisotropy filter, Graphic3d_LOTA_OFF by default
  Graphic3d_TypeOfTextureMode        myGenMode;         //!< texture coordinates generation mode, Graphic3d_TOTM_MANUAL by default
  Standard_Integer                   myBaseLevel;       //!< base texture mipmap level (0 by default)
  Standard_Integer                   myMaxLevel;        //!< maximum texture mipmap array level (1000 by default)
  Standard_ShortReal                 myRotAngle;        //!< texture coordinates rotation angle in degrees, 0 by default
  Standard_Boolean                   myToModulate;      //!< flag to modulate texture with material color, FALSE by default
  Standard_Boolean                   myToRepeat;        //!< flag to repeat (true) or wrap (false) texture coordinates out of [0,1] range

};

DEFINE_STANDARD_HANDLE(Graphic3d_TextureParams, Standard_Transient)

#endif // _Graphic3d_TextureParams_HeaderFile
