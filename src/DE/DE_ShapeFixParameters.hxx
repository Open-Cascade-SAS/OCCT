// Copyright (c) 2024 OPEN CASCADE SAS
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

#ifndef _DE_ShapeFixParameters_HeaderFile
#define _DE_ShapeFixParameters_HeaderFile

#include <Standard_Macro.hxx>
#include <TopAbs_ShapeEnum.hxx>

//! Struct for shape healing parameters storage
struct DE_ShapeFixParameters
{
  //! Enum, classifying a type of value for parameters
  enum class FixMode : signed char
  {
    FixOrNot = -1, //!< Procedure will be executed or not (depending on the situation)
    NotFix   = 0,  //!< Procedure will be executed
    Fix      = 1   //!< Procedure will be executed anyway
  };

  double           Tolerance3d                         = 1.e-6;
  double           MaxTolerance3d                      = 1.0;
  double           MinTolerance3d                      = 1.e-7;
  TopAbs_ShapeEnum DetalizationLevel                   = TopAbs_ShapeEnum::TopAbs_VERTEX;
  bool             NonManifold                         = false;
  FixMode          FixFreeShellMode                    = FixMode::FixOrNot;
  FixMode          FixFreeFaceMode                     = FixMode::FixOrNot;
  FixMode          FixFreeWireMode                     = FixMode::FixOrNot;
  FixMode          FixSameParameterMode                = FixMode::FixOrNot;
  FixMode          FixSolidMode                        = FixMode::FixOrNot;
  FixMode          FixShellOrientationMode             = FixMode::FixOrNot;
  FixMode          CreateOpenSolidMode                 = FixMode::NotFix;
  FixMode          FixShellMode                        = FixMode::FixOrNot;
  FixMode          FixFaceOrientationMode              = FixMode::FixOrNot;
  FixMode          FixFaceMode                         = FixMode::FixOrNot;
  FixMode          FixWireMode                         = FixMode::FixOrNot;
  FixMode          FixOrientationMode                  = FixMode::FixOrNot;
  FixMode          FixAddNaturalBoundMode              = FixMode::FixOrNot;
  FixMode          FixMissingSeamMode                  = FixMode::FixOrNot;
  FixMode          FixSmallAreaWireMode                = FixMode::FixOrNot;
  FixMode          RemoveSmallAreaFaceMode             = FixMode::FixOrNot;
  FixMode          FixIntersectingWiresMode            = FixMode::FixOrNot;
  FixMode          FixLoopWiresMode                    = FixMode::FixOrNot;
  FixMode          FixSplitFaceMode                    = FixMode::FixOrNot;
  FixMode          AutoCorrectPrecisionMode            = FixMode::Fix;
  FixMode          ModifyTopologyMode                  = FixMode::NotFix;
  FixMode          ModifyGeometryMode                  = FixMode::Fix;
  FixMode          ClosedWireMode                      = FixMode::Fix;
  FixMode          PreferencePCurveMode                = FixMode::Fix;
  FixMode          FixReorderMode                      = FixMode::FixOrNot;
  FixMode          FixSmallMode                        = FixMode::FixOrNot;
  FixMode          FixConnectedMode                    = FixMode::FixOrNot;
  FixMode          FixEdgeCurvesMode                   = FixMode::FixOrNot;
  FixMode          FixDegeneratedMode                  = FixMode::FixOrNot;
  FixMode          FixLackingMode                      = FixMode::FixOrNot;
  FixMode          FixSelfIntersectionMode             = FixMode::FixOrNot;
  FixMode          RemoveLoopMode                      = FixMode::FixOrNot;
  FixMode          FixReversed2dMode                   = FixMode::FixOrNot;
  FixMode          FixRemovePCurveMode                 = FixMode::FixOrNot;
  FixMode          FixRemoveCurve3dMode                = FixMode::FixOrNot;
  FixMode          FixAddPCurveMode                    = FixMode::FixOrNot;
  FixMode          FixAddCurve3dMode                   = FixMode::FixOrNot;
  FixMode          FixSeamMode                         = FixMode::FixOrNot;
  FixMode          FixShiftedMode                      = FixMode::FixOrNot;
  FixMode          FixEdgeSameParameterMode            = FixMode::NotFix;
  FixMode          FixNotchedEdgesMode                 = FixMode::FixOrNot;
  FixMode          FixTailMode                         = FixMode::NotFix;
  FixMode          MaxTailAngle                        = FixMode::NotFix;
  FixMode          MaxTailWidth                        = FixMode::FixOrNot;
  FixMode          FixSelfIntersectingEdgeMode         = FixMode::FixOrNot;
  FixMode          FixIntersectingEdgesMode            = FixMode::FixOrNot;
  FixMode          FixNonAdjacentIntersectingEdgesMode = FixMode::FixOrNot;
  FixMode          FixVertexPositionMode               = FixMode::NotFix;
  FixMode          FixVertexToleranceMode              = FixMode::FixOrNot;
};

#endif // _DE_ShapeFixParameters_HeaderFile
