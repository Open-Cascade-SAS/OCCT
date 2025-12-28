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

#include <Font_Hinting.hxx>
#include <Font_NameOfFont.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_RenderTransparentMethod.hxx>
#include <Graphic3d_RenderingMode.hxx>
#include <Graphic3d_StereoMode.hxx>
#include <Graphic3d_ToneMappingMethod.hxx>
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <NCollection_Vec4.hxx>
#include <Standard_TypeDef.hxx>

//! Helper class to store rendering parameters.
class Graphic3d_RenderingParams
{
public:
  //! Default pixels density.
  static const unsigned int THE_DEFAULT_RESOLUTION = 72u;

  //! Default ray-tracing depth.
  static const int THE_DEFAULT_DEPTH = 3;

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

  //! Statistics display flags.
  //! If not specified otherwise, the counter value is computed for a single rendered frame.
  enum PerfCounters
  {
    PerfCounters_NONE = 0x000, //!< no stats
    PerfCounters_FrameRate =
      0x001, //!< Frame Rate,        frames per second (number of frames within elapsed time)
    PerfCounters_CPU = 0x002, //!< CPU utilization as frames per second (number of frames within CPU
                              //!< utilization time (rendering thread))
    PerfCounters_Layers     = 0x004, //!< count layers (groups of structures)
    PerfCounters_Structures = 0x008, //!< count low-level Structures (normal unhighlighted
                                     //!< Presentable Object is usually represented by 1 Structure)
    //
    PerfCounters_Groups =
      0x010, //!< count primitive Groups (1 Structure holds 1 or more primitive Group)
    PerfCounters_GroupArrays =
      0x020, //!< count Arrays within Primitive Groups (optimal primitive Group holds 1 Array)
    //
    PerfCounters_Triangles = 0x040, //!< count Triangles
    PerfCounters_Points    = 0x080, //!< count Points
    PerfCounters_Lines     = 0x100, //!< count Line segments
    //
    PerfCounters_EstimMem = 0x200, //!< estimated GPU memory usage
    //
    PerfCounters_FrameTime =
      0x400, //!< frame CPU utilization time (rendering thread); @sa Graphic3d_FrameStatsTimer
    PerfCounters_FrameTimeMax = 0x800, //!< maximum frame times
    //
    PerfCounters_SkipImmediate = 0x1000, //!< do not include immediate viewer updates (e.g. lazy
                                         //!< updates without redrawing entire view content)
    //! show basic statistics
    PerfCounters_Basic =
      PerfCounters_FrameRate | PerfCounters_CPU | PerfCounters_Layers | PerfCounters_Structures,
    //! extended (verbose) statistics
    PerfCounters_Extended = PerfCounters_Basic | PerfCounters_Groups | PerfCounters_GroupArrays
                            | PerfCounters_Triangles | PerfCounters_Points | PerfCounters_Lines
                            | PerfCounters_EstimMem,
    //! all counters
    PerfCounters_All = PerfCounters_Extended | PerfCounters_FrameTime | PerfCounters_FrameTimeMax,
  };

  //! State of frustum culling optimization.
  enum FrustumCulling
  {
    FrustumCulling_Off,     //!< culling is disabled
    FrustumCulling_On,      //!< culling is active, and the list of culled entities is automatically
                            //!< updated before redraw
    FrustumCulling_NoUpdate //!< culling is active, but the list of culled entities is not updated
  };

public:
  //! Creates default rendering parameters.
  Graphic3d_RenderingParams()
      : Method(Graphic3d_RM_RASTERIZATION),
        ShadingModel(Graphic3d_TypeOfShadingModel_Phong),
        TransparencyMethod(Graphic3d_RTM_BLEND_UNORDERED),
        Resolution(THE_DEFAULT_RESOLUTION),
        FontHinting(Font_Hinting_Off),
        LineFeather(1.0f),
        // PBR parameters
        PbrEnvPow2Size(9),
        PbrEnvSpecMapNbLevels(6),
        PbrEnvBakingDiffNbSamples(1024),
        PbrEnvBakingSpecNbSamples(256),
        PbrEnvBakingProbability(0.99f),
        //
        OitDepthFactor(0.0f),
        NbOitDepthPeelingLayers(4),
        NbMsaaSamples(0),
        RenderResolutionScale(1.0f),
        ShadowMapResolution(1024),
        ShadowMapBias(0.005f),
        ToEnableDepthPrepass(false),
        ToEnableAlphaToCoverage(true),
        // ray tracing parameters
        IsGlobalIlluminationEnabled(false),
        SamplesPerPixel(0),
        RaytracingDepth(THE_DEFAULT_DEPTH),
        IsShadowEnabled(true),
        IsReflectionEnabled(false),
        IsAntialiasingEnabled(false),
        IsTransparentShadowEnabled(false),
        UseEnvironmentMapBackground(false),
        ToIgnoreNormalMapInRayTracing(false),
        CoherentPathTracingMode(false),
        AdaptiveScreenSampling(false),
        AdaptiveScreenSamplingAtomic(false),
        ShowSamplingTiles(false),
        TwoSidedBsdfModels(false),
        RadianceClampingValue(30.0),
        RebuildRayTracingShaders(false),
        RayTracingTileSize(32),
        NbRayTracingTiles(16 * 16),
        CameraApertureRadius(0.0f),
        CameraFocalPlaneDist(1.0f),
        FrustumCullingState(FrustumCulling_On),
        ToneMappingMethod(Graphic3d_ToneMappingMethod_Disabled),
        Exposure(0.f),
        WhitePoint(1.f),
        // stereoscopic parameters
        StereoMode(Graphic3d_StereoMode_QuadBuffer),
        HmdFov2d(30.0f),
        AnaglyphFilter(Anaglyph_RedCyan_Optimized),
        ToReverseStereo(false),
        ToSmoothInterlacing(true),
        ToMirrorComposer(true),
        //
        StatsPosition(new Graphic3d_TransformPers(Graphic3d_TMF_2d,
                                                  Aspect_TOTP_LEFT_UPPER,
                                                  NCollection_Vec2<int>(20, 20))),
        ChartPosition(new Graphic3d_TransformPers(Graphic3d_TMF_2d,
                                                  Aspect_TOTP_RIGHT_UPPER,
                                                  NCollection_Vec2<int>(20, 20))),
        ChartSize(-1, -1),
        StatsTextAspect(new Graphic3d_AspectText3d()),
        StatsUpdateInterval(1.0),
        StatsTextHeight(16),
        StatsNbFrames(1),
        StatsMaxChartTime(0.1f),
        CollectedStats(PerfCounters_Basic),
        ToShowStats(false)
  {
    const NCollection_Vec4<float> aZero(0.0f, 0.0f, 0.0f, 0.0f);
    AnaglyphLeft.SetRow(0, NCollection_Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f));
    AnaglyphLeft.SetRow(1, aZero);
    AnaglyphLeft.SetRow(2, aZero);
    AnaglyphLeft.SetRow(3, aZero);
    AnaglyphRight.SetRow(0, aZero);
    AnaglyphRight.SetRow(1, NCollection_Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f));
    AnaglyphRight.SetRow(2, NCollection_Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f));
    AnaglyphRight.SetRow(3, aZero);

    StatsTextAspect->SetColor(Quantity_NOC_WHITE);
    StatsTextAspect->SetColorSubTitle(Quantity_NOC_BLACK);
    StatsTextAspect->SetFont(Font_NOF_ASCII_MONO);
    StatsTextAspect->SetDisplayType(Aspect_TODT_SHADOW);
    StatsTextAspect->SetTextZoomable(false);
    StatsTextAspect->SetTextFontAspect(Font_FA_Regular);
  }

  //! Returns resolution ratio.
  float ResolutionRatio() const
  {
    return Resolution / static_cast<float>(THE_DEFAULT_RESOLUTION);
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

public: //! @name general parameters
  // clang-format off
  Graphic3d_RenderingMode           Method;                      //!< specifies rendering mode, Graphic3d_RM_RASTERIZATION by default
  Graphic3d_TypeOfShadingModel      ShadingModel;                //!< specified default shading model, Graphic3d_TypeOfShadingModel_Phong by default
  Graphic3d_RenderTransparentMethod TransparencyMethod;          //!< specifies rendering method for transparent graphics
  unsigned int                      Resolution;                  //!< Pixels density (PPI), defines scaling factor for parameters like text size
                                                                 //!  (when defined in screen-space units rather than in 3D) to be properly displayed
                                                                 //!  on device (screen / printer). 72 is default value.
                                                                 //!  Note that using difference resolution in different Views in same Viewer
                                                                 //!  will lead to performance regression (for example, text will be recreated every time).
  Font_Hinting                      FontHinting;                 //!< enables/disables text hinting within textured fonts, Font_Hinting_Off by default;
                                                                 //!  hinting improves readability of thin text on low-resolution screen,
                                                                 //!  but adds distortions to original font depending on font family and font library version
  float                LineFeather;                 //!< line feather width in pixels (> 0.0), 1.0 by default;
                                                                 //!  high values produce blurred results, small values produce sharp (aliased) edges

public: //! @name rendering resolution parameters

  int                  PbrEnvPow2Size;              //!< size of IBL maps side can be calculated as 2^PbrEnvPow2Size (> 0), 9 by default
  int                  PbrEnvSpecMapNbLevels;       //!< number of levels used in specular IBL map (> 1), 6 by default
  int                  PbrEnvBakingDiffNbSamples;   //!< number of samples used in Monte-Carlo integration during diffuse IBL map's
                                                                 //!  spherical harmonics coefficients generation (> 0), 1024 by default
  int                  PbrEnvBakingSpecNbSamples;   //!< number of samples used in Monte-Carlo integration during specular IBL map's generation (> 0), 256 by default
  float                PbrEnvBakingProbability;     //!< controls strength of samples reducing strategy during specular IBL map's generation
                                                                 //!  (see 'SpecIBLMapSamplesFactor' function for detail explanation) [0.0, 1.0], 0.99 by default

  float                OitDepthFactor;              //!< scalar factor [0-1] controlling influence of depth of a fragment to its final coverage (Graphic3d_RTM_BLEND_OIT), 0.0 by default
  int                  NbOitDepthPeelingLayers;     //!< number of depth peeling (Graphic3d_RTM_DEPTH_PEELING_OIT) layers, 4 by default
  int                  NbMsaaSamples;               //!< number of MSAA samples (should be within 0..GL_MAX_SAMPLES, power-of-two number), 0 by default
  float                RenderResolutionScale;       //!< rendering resolution scale factor, 1 by default;
                                                                 //!  incompatible with MSAA (e.g. NbMsaaSamples should be set to 0)
  int                  ShadowMapResolution;         //!< shadow texture map resolution, 1024 by default
  float                ShadowMapBias;               //!< shadowmap bias, 0.005 by default;
  bool                  ToEnableDepthPrepass;        //!< enables/disables depth pre-pass, False by default
  bool                  ToEnableAlphaToCoverage;     //!< enables/disables alpha to coverage, True by default

public: //! @name Ray-Tracing/Path-Tracing parameters

  bool                  IsGlobalIlluminationEnabled; //!< enables/disables global illumination effects (path tracing)
  int                  SamplesPerPixel;             //!< number of samples per pixel (SPP)
  int                  RaytracingDepth;             //!< maximum ray-tracing depth, 3 by default
  bool                  IsShadowEnabled;             //!< enables/disables shadows rendering, True by default
  bool                  IsReflectionEnabled;         //!< enables/disables specular reflections, False by default
  bool                  IsAntialiasingEnabled;       //!< enables/disables adaptive anti-aliasing, False by default
  bool                  IsTransparentShadowEnabled;  //!< enables/disables light propagation through transparent media, False by default
  bool                  UseEnvironmentMapBackground; //!< enables/disables environment map background
  bool                  ToIgnoreNormalMapInRayTracing; //!< enables/disables normal map ignoring during path tracing; FALSE by default
  bool                  CoherentPathTracingMode;     //!< enables/disables 'coherent' tracing mode (single RNG seed within 16x16 image blocks)
  bool                  AdaptiveScreenSampling;      //!< enables/disables adaptive screen sampling mode for path tracing, FALSE by default
  bool                  AdaptiveScreenSamplingAtomic;//!< enables/disables usage of atomic float operations within adaptive screen sampling, FALSE by default
  bool                  ShowSamplingTiles;           //!< enables/disables debug mode for adaptive screen sampling, FALSE by default
  bool                  TwoSidedBsdfModels;          //!< forces path tracing to use two-sided versions of original one-sided scattering models
  float                RadianceClampingValue;       //!< maximum radiance value used for clamping radiance estimation.
  bool                  RebuildRayTracingShaders;    //!< forces rebuilding ray tracing shaders at the next frame
  int                  RayTracingTileSize;          //!< screen tile size, 32 by default (adaptive sampling mode of path tracing);
  int                  NbRayTracingTiles;           //!< maximum number of screen tiles per frame, 256 by default (adaptive sampling mode of path tracing);
                                                                 //!  this parameter limits the number of tiles to be rendered per redraw, increasing Viewer interactivity,
                                                                 //!  but also increasing the time for achieving a good quality; -1 means no limit
  float                CameraApertureRadius;        //!< aperture radius of perspective camera used for depth-of-field, 0.0 by default (no DOF) (path tracing only)
  float                CameraFocalPlaneDist;        //!< focal  distance of perspective camera used for depth-of field, 1.0 by default (path tracing only)
  FrustumCulling                    FrustumCullingState;         //!< state of frustum culling optimization; FrustumCulling_On by default

  Graphic3d_ToneMappingMethod       ToneMappingMethod;           //!< specifies tone mapping method for path tracing, Graphic3d_ToneMappingMethod_Disabled by default
  float                Exposure;                    //!< exposure value used for tone mapping (path tracing), 0.0 by default
  float                WhitePoint;                  //!< white point value used in filmic tone mapping (path tracing), 1.0 by default

public: //! @name VR / stereoscopic parameters

  Graphic3d_StereoMode              StereoMode;                  //!< stereoscopic output mode, Graphic3d_StereoMode_QuadBuffer by default
  float                HmdFov2d;                    //!< sharp field of view range in degrees for displaying on-screen 2D elements, 30.0 by default;
  Anaglyph                          AnaglyphFilter;              //!< filter for anaglyph output, Anaglyph_RedCyan_Optimized by default
  NCollection_Mat4<float>                    AnaglyphLeft;                //!< left  anaglyph filter (in normalized colorspace), Color = AnaglyphRight * theColorRight + AnaglyphLeft * theColorLeft;
  NCollection_Mat4<float>                    AnaglyphRight;               //!< right anaglyph filter (in normalized colorspace), Color = AnaglyphRight * theColorRight + AnaglyphLeft * theColorLeft;
  bool                  ToReverseStereo;             //!< flag to reverse stereo pair, FALSE by default
  bool                  ToSmoothInterlacing;         //!< flag to smooth output on interlaced displays (improves text readability / reduces line aliasing), TRUE by default
  bool                  ToMirrorComposer;            //!< if output device is an external composer - mirror rendering results in window in addition to sending frame to composer, TRUE by default

public: //! @name on-screen display parameters

  occ::handle<Graphic3d_TransformPers>   StatsPosition;               //!< location of stats, upper-left position by default
  occ::handle<Graphic3d_TransformPers>   ChartPosition;               //!< location of stats chart, upper-right position by default
  NCollection_Vec2<int>                   ChartSize;                   //!< chart size in pixels, (-1, -1) by default which means that chart will occupy a portion of viewport
  occ::handle<Graphic3d_AspectText3d>    StatsTextAspect;             //!< stats text aspect
  float                StatsUpdateInterval;         //!< time interval between stats updates in seconds, 1.0 second by default;
                                                                 //!  too often updates might impact performance and will smear text within widgets
                                                                 //!  (especially framerate, which is better averaging);
                                                                 //!  0.0 interval will force updating on each frame
  int                  StatsTextHeight;             //!< stats text size; 16 by default
  int                  StatsNbFrames;               //!< number of data frames to collect history; 1 by default
  float                StatsMaxChartTime;           //!< upper time limit within frame chart in seconds; 0.1 seconds by default (100 ms or 10 FPS)
  PerfCounters                      CollectedStats;              //!< performance counters to collect, PerfCounters_Basic by default;
                                                                 //!  too verbose options might impact rendering performance,
                                                                 //!  because some counters might lack caching optimization (and will require expensive iteration through all data structures)
  bool                  ToShowStats;                 //!< display performance statistics, FALSE by default;
                                                                 //!  note that counters specified within CollectedStats will be updated nevertheless
                                                                 //!  of visibility of widget managed by ToShowStats flag (e.g. stats can be retrieved by application for displaying using other methods)
  // clang-format on
};

#endif // _Graphic3d_RenderingParams_HeaderFile
