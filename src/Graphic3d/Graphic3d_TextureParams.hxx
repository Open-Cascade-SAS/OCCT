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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Graphic3d_TypeOfTextureFilter.hxx>
#include <Graphic3d_LevelOfTextureAnisotropy.hxx>
#include <Standard_ShortReal.hxx>
#include <Graphic3d_Vec2.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Graphic3d_Vec4.hxx>
#include <Standard_Transient.hxx>


class Graphic3d_TextureParams;
DEFINE_STANDARD_HANDLE(Graphic3d_TextureParams, Standard_Transient)

//! This class describes texture parameters.
class Graphic3d_TextureParams : public Standard_Transient
{

public:

  
  //! Default constructor.
  Standard_EXPORT Graphic3d_TextureParams();
  
  Standard_EXPORT void Destroy() const;
~Graphic3d_TextureParams()
{
  Destroy();
}
  
  //! @return TRUE if the texture is modulate.
  //! Default value is FALSE.
  Standard_EXPORT Standard_Boolean IsModulate() const;
  
  //! @param theToModulate turn modulation on/off.
  Standard_EXPORT void SetModulate (const Standard_Boolean theToModulate);
  
  //! @return TRUE if the texture repeat is enabled.
  //! Default value is FALSE.
  Standard_EXPORT Standard_Boolean IsRepeat() const;
  
  //! @param theToRepeat turn texture repeat mode ON or OFF (clamping).
  Standard_EXPORT void SetRepeat (const Standard_Boolean theToRepeat);
  
  //! @return texture interpolation filter.
  //! Default value is Graphic3d_TOTF_NEAREST.
  Standard_EXPORT Graphic3d_TypeOfTextureFilter Filter() const;
  
  //! @param theFilter texture interpolation filter.
  Standard_EXPORT void SetFilter (const Graphic3d_TypeOfTextureFilter theFilter);
  
  //! @return level of anisontropy texture filter.
  //! Default value is Graphic3d_LOTA_OFF.
  Standard_EXPORT Graphic3d_LevelOfTextureAnisotropy AnisoFilter() const;
  
  //! @param theLevel level of anisontropy texture filter.
  Standard_EXPORT void SetAnisoFilter (const Graphic3d_LevelOfTextureAnisotropy theLevel);
  
  //! @return rotation angle in degrees
  //! Default value is 0.
  Standard_EXPORT Standard_ShortReal Rotation() const;
  
  //! @param theAngleDegrees rotation angle.
  Standard_EXPORT void SetRotation (const Standard_ShortReal theAngleDegrees);
  
  //! @return scale factor
  //! Default value is no scaling (1.0; 1.0).
  Standard_EXPORT const Graphic3d_Vec2& Scale() const;
  
  //! @param theScale scale factor.
  Standard_EXPORT void SetScale (const Graphic3d_Vec2 theScale);
  
  //! @return translation vector
  //! Default value is no translation (0.0; 0.0).
  Standard_EXPORT const Graphic3d_Vec2& Translation() const;
  
  //! @param theVec translation vector.
  Standard_EXPORT void SetTranslation (const Graphic3d_Vec2 theVec);
  
  //! @return texture coordinates generation mode.
  //! Default value is Graphic3d_TOTM_MANUAL.
  Standard_EXPORT Graphic3d_TypeOfTextureMode GenMode() const;
  
  //! @return texture coordinates generation plane S.
  Standard_EXPORT const Graphic3d_Vec4& GenPlaneS() const;
  
  //! @return texture coordinates generation plane T.
  Standard_EXPORT const Graphic3d_Vec4& GenPlaneT() const;
  
  //! Setup texture coordinates generation mode.
  Standard_EXPORT void SetGenMode (const Graphic3d_TypeOfTextureMode theMode, const Graphic3d_Vec4 thePlaneS, const Graphic3d_Vec4 thePlaneT);



  DEFINE_STANDARD_RTTI(Graphic3d_TextureParams,Standard_Transient)

protected:




private:


  Standard_Boolean myToModulate;
  Standard_Boolean myToRepeat;
  Graphic3d_TypeOfTextureFilter myFilter;
  Graphic3d_LevelOfTextureAnisotropy myAnisoLevel;
  Standard_ShortReal myRotAngle;
  Graphic3d_Vec2 myScale;
  Graphic3d_Vec2 myTranslation;
  Graphic3d_TypeOfTextureMode myGenMode;
  Graphic3d_Vec4 myGenPlaneS;
  Graphic3d_Vec4 myGenPlaneT;


};







#endif // _Graphic3d_TextureParams_HeaderFile
