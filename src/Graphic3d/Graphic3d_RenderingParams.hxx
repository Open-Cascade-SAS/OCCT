// Created on: 2014-05-14
// Created by: Denis BOGOLEPOV
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

#ifndef _Graphic3d_RenderingParams_HeaderFile
#define _Graphic3d_RenderingParams_HeaderFile

#include <Graphic3d_RenderingMode.hxx>
#include <Graphic3d_StereoMode.hxx>
#include <Graphic3d_Mat4.hxx>
#include <Graphic3d_Vec4.hxx>

//! Helper class to store rendering parameters.
class Graphic3d_RenderingParams
{
public:
  //! Default pixels density.
  static const unsigned int THE_DEFAULT_RESOLUTION = 72u;

  //! Default number of samples per pixel.
  static const Standard_Integer THE_DEFAULT_SPP = 1;

  //! Default ray-tracing depth.
  static const Standard_Integer THE_DEFAULT_DEPTH = 3;

  //! Anaglyph filter presets.
  enum Anaglyph
  {
    Anaglyph_RedCyan_Simple,       //!< simple    filter for Red-Cyan      glasses (R+GB)
    Anaglyph_RedCyan_Optimized,    //!< optimized filter for Red-Cyan      glasses (R+GB)
    Anaglyph_YellowBlue_Simple,    //!< simple    filter for Yellow-Blue   glasses (RG+B)
    Anaglyph_YellowBlue_Optimized, //!< optimized filter for Yellow-Blue   glasses (RG+B)
    Anaglyph_GreenMagenta_Simple,  //!< simple    filter for Green-Magenta glasses (G+RB)
    Anaglyph_UserDefined           //!< use externally specified matrices
  };

public:

  //! Creates default rendering parameters.
  Graphic3d_RenderingParams()
  : Method                      (Graphic3d_RM_RASTERIZATION),
    IsGlobalIlluminationEnabled (Standard_False),
    SamplesPerPixel             (THE_DEFAULT_SPP),
    RaytracingDepth             (THE_DEFAULT_DEPTH),
    IsShadowEnabled             (Standard_True),
    IsReflectionEnabled         (Standard_False),
    IsAntialiasingEnabled       (Standard_False),
    IsTransparentShadowEnabled  (Standard_False),
    UseEnvironmentMapBackground (Standard_False),
    CoherentPathTracingMode     (Standard_False),

    StereoMode (Graphic3d_StereoMode_QuadBuffer),
    AnaglyphFilter (Anaglyph_RedCyan_Optimized),
    ToReverseStereo (Standard_False),

    Resolution (THE_DEFAULT_RESOLUTION)
  {
    const Graphic3d_Vec4 aZero (0.0f, 0.0f, 0.0f, 0.0f);
    AnaglyphLeft .SetRow (0, Graphic3d_Vec4 (1.0f,  0.0f,  0.0f, 0.0f));
    AnaglyphLeft .SetRow (1, aZero);
    AnaglyphLeft .SetRow (2, aZero);
    AnaglyphLeft .SetRow (3, aZero);
    AnaglyphRight.SetRow (0, aZero);
    AnaglyphRight.SetRow (1, Graphic3d_Vec4 (0.0f,  1.0f,  0.0f, 0.0f));
    AnaglyphRight.SetRow (2, Graphic3d_Vec4 (0.0f,  0.0f,  1.0f, 0.0f));
    AnaglyphRight.SetRow (3, aZero);
  }

public:

  Graphic3d_RenderingMode Method;                      //!< specifies rendering mode, Graphic3d_RM_RASTERIZATION by default

  Standard_Boolean        IsGlobalIlluminationEnabled; //!< enables/disables global illumination effects (path tracing)
  Standard_Integer        SamplesPerPixel;             //!< number of samples per pixel (SPP)
  Standard_Integer        RaytracingDepth;             //!< maximum ray-tracing depth, 3 by default
  Standard_Boolean        IsShadowEnabled;             //!< enables/disables shadows rendering, True by default
  Standard_Boolean        IsReflectionEnabled;         //!< enables/disables specular reflections, False by default
  Standard_Boolean        IsAntialiasingEnabled;       //!< enables/disables adaptive anti-aliasing, False by default
  Standard_Boolean        IsTransparentShadowEnabled;  //!< enables/disables light propagation through transparent media, False by default
  Standard_Boolean        UseEnvironmentMapBackground; //!< enables/disables environment map background
  Standard_Boolean        CoherentPathTracingMode;     //!< enables/disables 'coherent' tracing mode (single RNG seed within 16x16 image blocks)

  Graphic3d_StereoMode    StereoMode;                  //!< stereoscopic output mode, Graphic3d_StereoMode_QuadBuffer by default
  Anaglyph                AnaglyphFilter;              //!< filter for anaglyph output, Anaglyph_RedCyan_Optimized by default
  Graphic3d_Mat4          AnaglyphLeft;                //!< left  anaglyph filter (in normalized colorspace), Color = AnaglyphRight * theColorRight + AnaglyphLeft * theColorLeft;
  Graphic3d_Mat4          AnaglyphRight;               //!< right anaglyph filter (in normalized colorspace), Color = AnaglyphRight * theColorRight + AnaglyphLeft * theColorLeft;
  Standard_Boolean        ToReverseStereo;             //!< flag to reverse stereo pair, FALSE by default

  unsigned int            Resolution;                  //!< Pixels density (PPI), defines scaling factor for parameters like text size
                                                       //!< (when defined in screen-space units rather than in 3D) to be properly displayed
                                                       //!< on device (screen / printer). 72 is default value.
                                                       //!< Note that using difference resolution in different Views in same Viewer
                                                       //!< will lead to performance regression (for example, text will be recreated every time).

};

#endif // _Graphic3d_RenderingParams_HeaderFile
