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

#include <Graphic3d_Mat4.hxx>
#include <Graphic3d_RenderTransparentMethod.hxx>
#include <Graphic3d_RenderingMode.hxx>
#include <Graphic3d_StereoMode.hxx>
#include <Graphic3d_ToneMappingMethod.hxx>
#include <Graphic3d_Vec4.hxx>

//! Helper class to store rendering parameters.
class Graphic3d_RenderingParams
{
public:
  //! Default pixels density.
  static const unsigned int THE_DEFAULT_RESOLUTION = 72u;

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
    TransparencyMethod          (Graphic3d_RTM_BLEND_UNORDERED),
    OitDepthFactor              (0.0f),
    NbMsaaSamples               (0),
    RenderResolutionScale       (1.0f),
    // ray tracing parameters
    IsGlobalIlluminationEnabled (Standard_False),
    RaytracingDepth             (THE_DEFAULT_DEPTH),
    IsShadowEnabled             (Standard_True),
    IsReflectionEnabled         (Standard_False),
    IsAntialiasingEnabled       (Standard_False),
    IsTransparentShadowEnabled  (Standard_False),
    UseEnvironmentMapBackground (Standard_False),
    CoherentPathTracingMode     (Standard_False),
    AdaptiveScreenSampling      (Standard_False),
    ShowSamplingTiles           (Standard_False),
    TwoSidedBsdfModels          (Standard_False),
    RadianceClampingValue       (30.0),
    RebuildRayTracingShaders    (Standard_False),
    NbRayTracingTiles           (16 * 16),
    CameraApertureRadius        (0.0f),
    CameraFocalPlaneDist        (1.0f),
    ToneMappingMethod           (Graphic3d_ToneMappingMethod_Disabled),
    Exposure                    (0.f),
    WhitePoint                  (1.f),
    // stereoscopic parameters
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

  //! Returns resolution ratio.
  Standard_ShortReal ResolutionRatio() const
  {
    return Resolution / static_cast<Standard_ShortReal> (THE_DEFAULT_RESOLUTION);
  }

public:

  Graphic3d_RenderingMode           Method;                      //!< specifies rendering mode, Graphic3d_RM_RASTERIZATION by default
  Graphic3d_RenderTransparentMethod TransparencyMethod;          //!< specifies rendering method for transparent graphics
  Standard_ShortReal                OitDepthFactor;              //!< scalar factor [0-1] controlling influence of depth of a fragment to its final coverage
  Standard_Integer                  NbMsaaSamples;               //!< number of MSAA samples (should be within 0..GL_MAX_SAMPLES, power-of-two number), 0 by default
  Standard_ShortReal                RenderResolutionScale;       //!< rendering resolution scale factor, 1 by default;
                                                                 //!  incompatible with MSAA (e.g. NbMsaaSamples should be set to 0)

  Standard_Boolean                  IsGlobalIlluminationEnabled; //!< enables/disables global illumination effects (path tracing)
  Standard_Integer                  SamplesPerPixel;             //!< number of samples per pixel (SPP)
  Standard_Integer                  RaytracingDepth;             //!< maximum ray-tracing depth, 3 by default
  Standard_Boolean                  IsShadowEnabled;             //!< enables/disables shadows rendering, True by default
  Standard_Boolean                  IsReflectionEnabled;         //!< enables/disables specular reflections, False by default
  Standard_Boolean                  IsAntialiasingEnabled;       //!< enables/disables adaptive anti-aliasing, False by default
  Standard_Boolean                  IsTransparentShadowEnabled;  //!< enables/disables light propagation through transparent media, False by default
  Standard_Boolean                  UseEnvironmentMapBackground; //!< enables/disables environment map background
  Standard_Boolean                  CoherentPathTracingMode;     //!< enables/disables 'coherent' tracing mode (single RNG seed within 16x16 image blocks)
  Standard_Boolean                  AdaptiveScreenSampling;      //!< enables/disables adaptive screen sampling mode for path tracing, FALSE by default
  Standard_Boolean                  ShowSamplingTiles;           //!< enables/disables debug mode for adaptive screen sampling, FALSE by default
  Standard_Boolean                  TwoSidedBsdfModels;          //!< forces path tracing to use two-sided versions of original one-sided scattering models
  Standard_ShortReal                RadianceClampingValue;       //!< maximum radiance value used for clamping radiance estimation.
  Standard_Boolean                  RebuildRayTracingShaders;    //!< forces rebuilding ray tracing shaders at the next frame
  Standard_Integer                  NbRayTracingTiles;           //!< total number of screen tiles used in adaptive sampling mode (PT only)
  Standard_ShortReal                CameraApertureRadius;        //!< aperture radius of perspective camera used for depth-of-field, 0.0 by default (no DOF) (path tracing only)
  Standard_ShortReal                CameraFocalPlaneDist;        //!< focal  distance of perspective camera used for depth-of field, 1.0 by default (path tracing only)

  Graphic3d_ToneMappingMethod       ToneMappingMethod;           //!< specifies tone mapping method for path tracing, Graphic3d_ToneMappingMethod_Disabled by default
  Standard_ShortReal                Exposure;                    //!< exposure value used for tone mapping (path tracing), 0.0 by default
  Standard_ShortReal                WhitePoint;                  //!< white point value used in filmic tone mapping (path tracing), 1.0 by default

  Graphic3d_StereoMode              StereoMode;                  //!< stereoscopic output mode, Graphic3d_StereoMode_QuadBuffer by default
  Anaglyph                          AnaglyphFilter;              //!< filter for anaglyph output, Anaglyph_RedCyan_Optimized by default
  Graphic3d_Mat4                    AnaglyphLeft;                //!< left  anaglyph filter (in normalized colorspace), Color = AnaglyphRight * theColorRight + AnaglyphLeft * theColorLeft;
  Graphic3d_Mat4                    AnaglyphRight;               //!< right anaglyph filter (in normalized colorspace), Color = AnaglyphRight * theColorRight + AnaglyphLeft * theColorLeft;
  Standard_Boolean                  ToReverseStereo;             //!< flag to reverse stereo pair, FALSE by default

  unsigned int                      Resolution;                  //!< Pixels density (PPI), defines scaling factor for parameters like text size
                                                                 //!  (when defined in screen-space units rather than in 3D) to be properly displayed
                                                                 //!  on device (screen / printer). 72 is default value.
                                                                 //!  Note that using difference resolution in different Views in same Viewer
                                                                 //!  will lead to performance regression (for example, text will be recreated every time).
};

#endif // _Graphic3d_RenderingParams_HeaderFile
