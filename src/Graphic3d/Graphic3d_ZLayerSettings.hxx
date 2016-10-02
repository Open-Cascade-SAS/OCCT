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

#include <gp_XYZ.hxx>
#include <Geom_Transformation.hxx>
#include <Graphic3d_PolygonOffset.hxx>
#include <TCollection_AsciiString.hxx>

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
  : myIsImmediate       (Standard_False),
    myUseEnvironmentTexture (Standard_True),
    myToEnableDepthTest (Standard_True),
    myToEnableDepthWrite(Standard_True),
    myToClearDepth      (Standard_True) {}

  //! Return user-provided name.
  const TCollection_AsciiString& Name() const { return myName; }

  //! Set custom name.
  void SetName (const TCollection_AsciiString& theName) { myName = theName; }

  //! Return the origin of all objects within the layer.
  const gp_XYZ& Origin() const { return myOrigin; }

  //! Return the transformation to the origin.
  const Handle(Geom_Transformation)& OriginTransformation() const { return myOriginTrsf; }

  //! Set the origin of all objects within the layer.
  void SetOrigin (const gp_XYZ& theOrigin)
  {
    myOrigin = theOrigin;
    myOriginTrsf.Nullify();
    if (!theOrigin.IsEqual (gp_XYZ(0.0, 0.0, 0.0), gp::Resolution()))
    {
      myOriginTrsf = new Geom_Transformation();
    }
  }

  //! Return true if this layer should be drawn after all normal (non-immediate) layers.
  Standard_Boolean IsImmediate() const { return myIsImmediate; }

  //! Set the flag indicating the immediate layer, which should be drawn after all normal (non-immediate) layers.
  void SetImmediate (const Standard_Boolean theValue) { myIsImmediate = theValue; }

  //! Return flag to allow/prevent environment texture mapping usage for specific layer.
  Standard_Boolean UseEnvironmentTexture() const { return myUseEnvironmentTexture; }

  //! Set the flag to allow/prevent environment texture mapping usage for specific layer.
  void SetEnvironmentTexture (const Standard_Boolean theValue) { myUseEnvironmentTexture = theValue; }

  //! Return true if depth test should be enabled.
  Standard_Boolean ToEnableDepthTest() const { return myToEnableDepthTest; }

  //! Set if depth test should be enabled.
  void SetEnableDepthTest(const Standard_Boolean theValue) { myToEnableDepthTest = theValue; }

  //! Return true depth values should be written during rendering.
  Standard_Boolean ToEnableDepthWrite() const { return myToEnableDepthWrite; }

  //! Set if depth values should be written during rendering.
  void SetEnableDepthWrite (const Standard_Boolean theValue) { myToEnableDepthWrite = theValue; }

  //! Return true if depth values should be cleared before drawing the layer.
  Standard_Boolean ToClearDepth() const { return myToClearDepth; }

  //! Set if depth values should be cleared before drawing the layer.
  void SetClearDepth (const Standard_Boolean theValue) { myToClearDepth = theValue; }

  //! Return glPolygonOffset() arguments.
  const Graphic3d_PolygonOffset& PolygonOffset() const { return myPolygonOffset; }

  //! Setup glPolygonOffset() arguments.
  void SetPolygonOffset (const Graphic3d_PolygonOffset& theParams) { myPolygonOffset = theParams; }

  //! Modify glPolygonOffset() arguments.
  Graphic3d_PolygonOffset& ChangePolygonOffset() { return myPolygonOffset; }

  //! Returns true if theSetting is enabled.
  Standard_DEPRECATED("Deprecated method IsSettingEnabled() should be replaced by individual property getters")
  Standard_Boolean IsSettingEnabled (const Graphic3d_ZLayerSetting theSetting) const
  {
    switch (theSetting)
    {
      case Graphic3d_ZLayerDepthTest:   return myToEnableDepthTest;
      case Graphic3d_ZLayerDepthWrite:  return myToEnableDepthWrite;
      case Graphic3d_ZLayerDepthClear:  return myToClearDepth;
      case Graphic3d_ZLayerDepthOffset: return myPolygonOffset.Mode != Aspect_POM_Off;
    }
    return Standard_False;
  }

  //! Enables theSetting
  Standard_DEPRECATED("Deprecated method EnableSetting() should be replaced by individual property getters")
  void EnableSetting (const Graphic3d_ZLayerSetting theSetting)
  {
    switch (theSetting)
    {
      case Graphic3d_ZLayerDepthTest:   myToEnableDepthTest  = Standard_True; return;
      case Graphic3d_ZLayerDepthWrite:  myToEnableDepthWrite = Standard_True; return;
      case Graphic3d_ZLayerDepthClear:  myToClearDepth       = Standard_True; return;
      case Graphic3d_ZLayerDepthOffset: myPolygonOffset.Mode = Aspect_POM_Fill; return;
    }
  }

  //! Disables theSetting
  Standard_DEPRECATED("Deprecated method DisableSetting() should be replaced by individual property getters")
  void DisableSetting (const Graphic3d_ZLayerSetting theSetting)
  {
    switch (theSetting)
    {
      case Graphic3d_ZLayerDepthTest:   myToEnableDepthTest  = Standard_False; return;
      case Graphic3d_ZLayerDepthWrite:  myToEnableDepthWrite = Standard_False; return;
      case Graphic3d_ZLayerDepthClear:  myToClearDepth       = Standard_False; return;
      case Graphic3d_ZLayerDepthOffset: myPolygonOffset.Mode = Aspect_POM_Off; return;
    }
  }

  //! Sets minimal possible positive depth offset.
  void SetDepthOffsetPositive()
  {
    myPolygonOffset.Mode   = Aspect_POM_Fill;
    myPolygonOffset.Factor = 1.0f;
    myPolygonOffset.Units  = 1.0f;
  }

  //! Sets minimal possible negative depth offset.
  void SetDepthOffsetNegative()
  {
    myPolygonOffset.Mode   = Aspect_POM_Fill;
    myPolygonOffset.Factor = 1.0f;
    myPolygonOffset.Units  =-1.0f;
  }

protected:

  TCollection_AsciiString     myName;                  //!< user-provided name
  Handle(Geom_Transformation) myOriginTrsf;            //!< transformation to the origin
  gp_XYZ                      myOrigin;                //!< the origin of all objects within the layer
  Graphic3d_PolygonOffset     myPolygonOffset;         //!< glPolygonOffset() arguments
  Standard_Boolean            myIsImmediate;           //!< immediate layer will be drawn after all normal layers
  Standard_Boolean            myUseEnvironmentTexture; //!< flag to allow/prevent environment texture mapping usage for specific layer
  Standard_Boolean            myToEnableDepthTest;     //!< option to enable depth test
  Standard_Boolean            myToEnableDepthWrite;    //!< option to enable write depth values
  Standard_Boolean            myToClearDepth;          //!< option to clear depth values before drawing the layer

};

#endif // _Graphic3d_ZLayerSettings_HeaderFile
