// Created on: 2020-02-06
// Created by: Svetlana SHUTINA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Graphic3d_RenderingParams.hxx>

#include <Standard_Dump.hxx>

//=================================================================================================

void Graphic3d_RenderingParams::DumpJson(Standard_OStream& theOStream,
                                         Standard_Integer  theDepth) const
{
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Method)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ShadingModel)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, TransparencyMethod)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, LineFeather)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, PbrEnvPow2Size)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, PbrEnvSpecMapNbLevels)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, PbrEnvBakingDiffNbSamples)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, PbrEnvBakingSpecNbSamples)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, PbrEnvBakingProbability)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, OitDepthFactor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, NbMsaaSamples)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, RenderResolutionScale)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ToEnableDepthPrepass)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ToEnableAlphaToCoverage)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsGlobalIlluminationEnabled)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, SamplesPerPixel)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, RaytracingDepth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsShadowEnabled)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsReflectionEnabled)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsAntialiasingEnabled)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsTransparentShadowEnabled)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, UseEnvironmentMapBackground)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ToIgnoreNormalMapInRayTracing)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, CoherentPathTracingMode)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, AdaptiveScreenSampling)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, AdaptiveScreenSamplingAtomic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ShowSamplingTiles)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, TwoSidedBsdfModels)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, RadianceClampingValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, RebuildRayTracingShaders)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, RayTracingTileSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, NbRayTracingTiles)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, CameraApertureRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, CameraFocalPlaneDist)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, FrustumCullingState)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ToneMappingMethod)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Exposure)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, WhitePoint)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, StereoMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, AnaglyphFilter)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &AnaglyphLeft)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &AnaglyphRight)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ToReverseStereo)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, StatsPosition.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, ChartPosition.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &ChartSize)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, StatsTextAspect.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, StatsUpdateInterval)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, StatsTextHeight)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, StatsNbFrames)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, StatsMaxChartTime)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, CollectedStats)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ToShowStats)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Resolution)
}
