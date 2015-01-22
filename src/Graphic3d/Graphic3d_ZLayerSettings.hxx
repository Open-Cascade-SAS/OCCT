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

#ifndef _Graphic3d_ZLayerSettings_HeaderFile
#define _Graphic3d_ZLayerSettings_HeaderFile

#include <Standard_TypeDef.hxx>

enum Graphic3d_ZLayerSetting
{
  Graphic3d_ZLayerDepthTest = 1,
  Graphic3d_ZLayerDepthWrite = 2,
  Graphic3d_ZLayerDepthClear = 4,
  Graphic3d_ZLayerDepthOffset = 8
};

//! Structure defines list of ZLayer properties.
struct Graphic3d_ZLayerSettings
{

  //! Default settings.
  Graphic3d_ZLayerSettings()
    : DepthOffsetFactor (1.0f),
      DepthOffsetUnits  (1.0f),
      Flags (Graphic3d_ZLayerDepthTest
           | Graphic3d_ZLayerDepthWrite
           | Graphic3d_ZLayerDepthClear),
      IsImmediate (false)
  {}

  //! Returns true if theSetting is enabled.
  const Standard_Boolean IsSettingEnabled (const Graphic3d_ZLayerSetting theSetting) const
  {
    return (Flags & theSetting) == theSetting;
  }

  //! Enables theSetting
  void EnableSetting (const Graphic3d_ZLayerSetting theSetting)
  {
    Flags = Flags | theSetting;
  }

  //! Disables theSetting
  void DisableSetting (const Graphic3d_ZLayerSetting theSetting)
  {
    Flags = Flags & (~theSetting);
  }

  //! Sets minimal possible positive depth offset.
  //! Access DepthOffsetFactor and DepthOffsetUnits values for manual offset control.
  void SetDepthOffsetPositive()
  {
    DepthOffsetFactor = 1.f;
    DepthOffsetUnits  = 1.f;
    EnableSetting (Graphic3d_ZLayerDepthOffset);
  }

  //! Sets minimal possible negative depth offset.
  //! Access DepthOffsetFactor and DepthOffsetUnits values for manual offset control.
  void SetDepthOffsetNegative()
  {
    DepthOffsetFactor =  1.f;
    DepthOffsetUnits  = -1.f;
    EnableSetting (Graphic3d_ZLayerDepthOffset);
  }

public:

  Standard_ShortReal DepthOffsetFactor; //!< factor argument value for OpenGl glPolygonOffset function
  Standard_ShortReal DepthOffsetUnits;  //!< units  argument value for OpenGl glPolygonOffset function
  Standard_Integer   Flags;             //!< storage field for settings
  bool               IsImmediate;       //!< immediate layer will be drawn after all normal layers

};

#endif // _Graphic3d_ZLayerSettings_HeaderFile
