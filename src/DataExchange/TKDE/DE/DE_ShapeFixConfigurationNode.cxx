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

#include <DE_ShapeFixConfigurationNode.hxx>

#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <DE_Wrapper.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DE_ShapeFixConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}
} // namespace

//=================================================================================================

DE_ShapeFixConfigurationNode::DE_ShapeFixConfigurationNode()
    : DE_ConfigurationNode()
{
}

//=================================================================================================

DE_ShapeFixConfigurationNode::DE_ShapeFixConfigurationNode(
  const Handle(DE_ShapeFixConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode),
      ShapeFixParameters(theNode->ShapeFixParameters)
{
}

//=================================================================================================

bool DE_ShapeFixConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".healing";

  ShapeFixParameters.Tolerance3d =
    theResource->RealVal("tolerance3d", ShapeFixParameters.Tolerance3d, aScope);
  ShapeFixParameters.MaxTolerance3d =
    theResource->RealVal("max.tolerance3d", ShapeFixParameters.MaxTolerance3d, aScope);
  ShapeFixParameters.MinTolerance3d =
    theResource->RealVal("min.tolerance3d", ShapeFixParameters.MinTolerance3d, aScope);
  ShapeFixParameters.FixFreeShellMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "free.shell",
    (int)ShapeFixParameters.FixFreeShellMode,
    aScope);
  ShapeFixParameters.FixFreeFaceMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("free.face",
                                                            (int)ShapeFixParameters.FixFreeFaceMode,
                                                            aScope);
  ShapeFixParameters.FixFreeWireMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("free.wire",
                                                            (int)ShapeFixParameters.FixFreeWireMode,
                                                            aScope);
  ShapeFixParameters.FixSameParameterMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "same.parameter",
    (int)ShapeFixParameters.FixSameParameterMode,
    aScope);
  ShapeFixParameters.FixSolidMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("solid",
                                                            (int)ShapeFixParameters.FixSolidMode,
                                                            aScope);
  ShapeFixParameters.FixShellOrientationMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "shell.orientation",
      (int)ShapeFixParameters.FixShellOrientationMode,
      aScope);
  ShapeFixParameters.CreateOpenSolidMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "create.open.solid",
    (int)ShapeFixParameters.CreateOpenSolidMode,
    aScope);
  ShapeFixParameters.FixShellMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("shell",
                                                            (int)ShapeFixParameters.FixShellMode,
                                                            aScope);
  ShapeFixParameters.FixFaceOrientationMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "face.orientation",
      (int)ShapeFixParameters.FixFaceOrientationMode,
      aScope);
  ShapeFixParameters.FixFaceMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("face",
                                                            (int)ShapeFixParameters.FixFaceMode,
                                                            aScope);
  ShapeFixParameters.FixWireMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("wire",
                                                            (int)ShapeFixParameters.FixWireMode,
                                                            aScope);
  ShapeFixParameters.FixOrientationMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "orientation",
    (int)ShapeFixParameters.FixOrientationMode,
    aScope);
  ShapeFixParameters.FixAddNaturalBoundMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "add.natural.bound",
      (int)ShapeFixParameters.FixAddNaturalBoundMode,
      aScope);
  ShapeFixParameters.FixMissingSeamMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "missing.seam",
    (int)ShapeFixParameters.FixMissingSeamMode,
    aScope);
  ShapeFixParameters.FixSmallAreaWireMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "small.area.wire",
    (int)ShapeFixParameters.FixSmallAreaWireMode,
    aScope);
  ShapeFixParameters.RemoveSmallAreaFaceMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "remove.small.area.face",
      (int)ShapeFixParameters.RemoveSmallAreaFaceMode,
      aScope);
  ShapeFixParameters.FixIntersectingWiresMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "intersecting.wires",
      (int)ShapeFixParameters.FixIntersectingWiresMode,
      aScope);
  ShapeFixParameters.FixLoopWiresMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "loop.wires",
    (int)ShapeFixParameters.FixLoopWiresMode,
    aScope);
  ShapeFixParameters.FixSplitFaceMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "split.face",
    (int)ShapeFixParameters.FixSplitFaceMode,
    aScope);
  ShapeFixParameters.AutoCorrectPrecisionMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "auto.correct.precision",
      (int)ShapeFixParameters.AutoCorrectPrecisionMode,
      aScope);
  ShapeFixParameters.ModifyTopologyMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "modify.topology",
    (int)ShapeFixParameters.ModifyTopologyMode,
    aScope);
  ShapeFixParameters.ModifyGeometryMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "modify.geometry",
    (int)ShapeFixParameters.ModifyGeometryMode,
    aScope);
  ShapeFixParameters.ClosedWireMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("closed.wire",
                                                            (int)ShapeFixParameters.ClosedWireMode,
                                                            aScope);
  ShapeFixParameters.PreferencePCurveMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "preference.pcurve",
    (int)ShapeFixParameters.PreferencePCurveMode,
    aScope);
  ShapeFixParameters.FixReorderMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("reorder.edges",
                                                            (int)ShapeFixParameters.FixReorderMode,
                                                            aScope);
  ShapeFixParameters.FixSmallMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("remove.small.edges",
                                                            (int)ShapeFixParameters.FixSmallMode,
                                                            aScope);
  ShapeFixParameters.FixConnectedMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "connected.edges",
    (int)ShapeFixParameters.FixConnectedMode,
    aScope);
  ShapeFixParameters.FixEdgeCurvesMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "edge.curves",
    (int)ShapeFixParameters.FixEdgeCurvesMode,
    aScope);
  ShapeFixParameters.FixDegeneratedMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "add.degenerated.edges",
    (int)ShapeFixParameters.FixDegeneratedMode,
    aScope);
  ShapeFixParameters.FixLackingMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("add.lacking.edges",
                                                            (int)ShapeFixParameters.FixLackingMode,
                                                            aScope);
  ShapeFixParameters.FixSelfIntersectionMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "selfintersection",
      (int)ShapeFixParameters.FixSelfIntersectionMode,
      aScope);
  ShapeFixParameters.RemoveLoopMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("remove.loop",
                                                            (int)ShapeFixParameters.RemoveLoopMode,
                                                            aScope);
  ShapeFixParameters.FixReversed2dMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "reversed2d",
    (int)ShapeFixParameters.FixReversed2dMode,
    aScope);
  ShapeFixParameters.FixRemovePCurveMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "remove.pcurve",
    (int)ShapeFixParameters.FixRemovePCurveMode,
    aScope);
  ShapeFixParameters.FixRemoveCurve3dMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "remove.curve3d",
    (int)ShapeFixParameters.FixRemoveCurve3dMode,
    aScope);
  ShapeFixParameters.FixAddPCurveMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "add.pcurve",
    (int)ShapeFixParameters.FixAddPCurveMode,
    aScope);
  ShapeFixParameters.FixAddCurve3dMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "add.curve3d",
    (int)ShapeFixParameters.FixAddCurve3dMode,
    aScope);
  ShapeFixParameters.FixSeamMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("correct.order.in.seam",
                                                            (int)ShapeFixParameters.FixSeamMode,
                                                            aScope);
  ShapeFixParameters.FixShiftedMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("shifted",
                                                            (int)ShapeFixParameters.FixShiftedMode,
                                                            aScope);
  ShapeFixParameters.FixEdgeSameParameterMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "edge.same.parameter",
      (int)ShapeFixParameters.FixEdgeSameParameterMode,
      aScope);
  ShapeFixParameters.FixNotchedEdgesMode = (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
    "notched.edges",
    (int)ShapeFixParameters.FixNotchedEdgesMode,
    aScope);
  ShapeFixParameters.FixTailMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("tail",
                                                            (int)ShapeFixParameters.FixTailMode,
                                                            aScope);
  ShapeFixParameters.MaxTailAngle =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("max.tail.angle",
                                                            (int)ShapeFixParameters.MaxTailAngle,
                                                            aScope);
  ShapeFixParameters.MaxTailWidth =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal("max.tail.width",
                                                            (int)ShapeFixParameters.MaxTailWidth,
                                                            aScope);
  ShapeFixParameters.FixSelfIntersectingEdgeMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "selfintersecting.edge",
      (int)ShapeFixParameters.FixSelfIntersectingEdgeMode,
      aScope);
  ShapeFixParameters.FixIntersectingEdgesMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "intersecting.edges",
      (int)ShapeFixParameters.FixIntersectingEdgesMode,
      aScope);
  ShapeFixParameters.FixNonAdjacentIntersectingEdgesMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "nonadjacent.intersecting.edges",
      (int)ShapeFixParameters.FixNonAdjacentIntersectingEdgesMode,
      aScope);
  ShapeFixParameters.FixVertexPositionMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "vertex.position",
      (int)ShapeFixParameters.FixVertexPositionMode,
      aScope);
  ShapeFixParameters.FixVertexToleranceMode =
    (DE_ShapeFixParameters::FixMode)theResource->IntegerVal(
      "vertex.tolerance",
      (int)ShapeFixParameters.FixVertexToleranceMode,
      aScope);

  return true;
}

//=================================================================================================

TCollection_AsciiString DE_ShapeFixConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".healing.";

  aResult += "!\n";
  aResult += "!Shape healing parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the maximum allowable tolerance\n";
  aResult += "!Default value: 1.e-6. Available values: any real positive (non null) value\n";
  aResult += aScope + "tolerance3d :\t " + ShapeFixParameters.Tolerance3d + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the maximum allowable tolerance\n";
  aResult += "!Default value: 1.0. Available values: any real positive (non null) value\n";
  aResult += aScope + "max.tolerance3d :\t " + ShapeFixParameters.MaxTolerance3d + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the minimum allowable tolerance\n";
  aResult += "!Default value: 1.e-7. Available values: any real positive (non null) value\n";
  aResult += aScope + "min.tolerance3d :\t " + ShapeFixParameters.MinTolerance3d + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Shell for ShapeFix_Shape\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "free.shell :\t " + (int)ShapeFixParameters.FixFreeShellMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Face for ShapeFix_Shape\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "free.face :\t " + (int)ShapeFixParameters.FixFreeFaceMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Wire for ShapeFix_Shape\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "free.wire :\t " + (int)ShapeFixParameters.FixFreeWireMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying ShapeFix::SameParameter after all fixes\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "same.parameter :\t " + (int)ShapeFixParameters.FixSameParameterMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Solid\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "solid :\t " + (int)ShapeFixParameters.FixSolidMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines the mode for applying analysis and fixes of orientation of shells in the solid\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "shell.orientation :\t " + (int)ShapeFixParameters.FixShellOrientationMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for creation of solids. If operation is executed then solids are "
             "created from open shells ";
  aResult += "else solids are created from closed shells only\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "create.open.solid :\t " + (int)ShapeFixParameters.CreateOpenSolidMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Shell for ShapeFix_Solid\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "shell :\t " + (int)ShapeFixParameters.FixShellMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines the mode for applying analysis and fixes of orientation of faces in the shell\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "face.orientation :\t " + (int)ShapeFixParameters.FixFaceOrientationMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "face :\t " + (int)ShapeFixParameters.FixFaceMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Wire\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "wire :\t " + (int)ShapeFixParameters.FixWireMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying a fix for the orientation of faces in the shell\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "orientation :\t " + (int)ShapeFixParameters.FixOrientationMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the add natural bound mode. If operation is executed then natural boundary "
             "is added on faces that miss them\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "add.natural.bound :\t " + (int)ShapeFixParameters.FixAddNaturalBoundMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix missing seam mode (tries to insert seam is missed)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "missing.seam :\t " + (int)ShapeFixParameters.FixMissingSeamMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix small area wire mode (drops small wires)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "small.area.wire :\t " + (int)ShapeFixParameters.FixSmallAreaWireMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the remove face with small area (drops faces with small outer wires)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "remove.small.area.face :\t " + (int)ShapeFixParameters.RemoveSmallAreaFaceMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix intersecting wires mode in ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "intersecting.wires :\t " + (int)ShapeFixParameters.FixIntersectingWiresMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix loop wires mode in ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "loop.wires :\t " + (int)ShapeFixParameters.FixLoopWiresMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix split face mode in ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "split.face :\t " + (int)ShapeFixParameters.FixSplitFaceMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the auto-correct precision mode in ShapeFix_Face\n";
  aResult +=
    "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "auto.correct.precision :\t "
             + (int)ShapeFixParameters.AutoCorrectPrecisionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to modify topology of the wire during fixing "
             "(adding/removing edges etc.)\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "modify.topology :\t " + (int)ShapeFixParameters.ModifyTopologyMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to modify geometry of the edges and vertices\n";
  aResult +=
    "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "modify.geometry :\t " + (int)ShapeFixParameters.ModifyGeometryMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to defines whether the wire is to be closed (by calling "
             "methods like FixDegenerated() ";
  aResult += "!and FixConnected() for lastand first edges\n";
  aResult +=
    "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "closed.wire :\t " + (int)ShapeFixParameters.ClosedWireMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to defines the 2d representation of the wire is preferable "
             "over 3d one ";
  aResult += "(in the case of ambiguity in FixEdgeCurves)\n";
  aResult += "!and FixConnected() for lastand first edges\n";
  aResult +=
    "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult +=
    aScope + "preference.pcurve :\t " + (int)ShapeFixParameters.PreferencePCurveMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to reorder edges in the wire\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "reorder.edges :\t " + (int)ShapeFixParameters.FixReorderMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove small edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "remove.small.edges :\t " + (int)ShapeFixParameters.FixSmallMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix connecting edges in the wire\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "connected.edges :\t " + (int)ShapeFixParameters.FixConnectedMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix edges (3Dcurves and 2D curves)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "edge.curves :\t " + (int)ShapeFixParameters.FixEdgeCurvesMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for add degenerated edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "add.degenerated.edges :\t " + (int)ShapeFixParameters.FixDegeneratedMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for add lacking edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "add.lacking.edges :\t " + (int)ShapeFixParameters.FixLackingMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix selfintersection edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "selfintersection :\t " + (int)ShapeFixParameters.FixSelfIntersectionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove loop\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "remove.loop :\t " + (int)ShapeFixParameters.RemoveLoopMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to fix edge if pcurve is directed opposite to 3d curve\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "reversed2d :\t " + (int)ShapeFixParameters.FixReversed2dMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove the pcurve(s) of the edge if it does not match "
             "the vertices\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "remove.pcurve :\t " + (int)ShapeFixParameters.FixRemovePCurveMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines the mode allowed to remove 3d curve of the edge if it does not match the vertices\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "remove.curve3d :\t " + (int)ShapeFixParameters.FixRemoveCurve3dMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines the mode allowed to add pcurve(s) of the edge if missing (by projecting 3d curve)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "add.pcurve :\t " + (int)ShapeFixParameters.FixAddPCurveMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to build 3d curve of the edge if missing\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "add.curve3d :\t " + (int)ShapeFixParameters.FixAddCurve3dMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to correct order of pcurves in the seam edge depends on "
             "its orientation\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "correct.order.in.seam :\t " + (int)ShapeFixParameters.FixSeamMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to shifts wrong 2D curves back, ensuring that the 2D "
             "curves of the edges in the wire are connected\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "shifted :\t " + (int)ShapeFixParameters.FixShiftedMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying EdgeSameParameter\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "edge.same.parameter :\t " + (int)ShapeFixParameters.FixEdgeSameParameterMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix notched edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "notched.edges :\t " + (int)ShapeFixParameters.FixNotchedEdgesMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix tail in wire\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "tail :\t " + (int)ShapeFixParameters.FixTailMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for max angle of the tails\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "max.tail.angle :\t " + (int)ShapeFixParameters.MaxTailAngle + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for max tail width\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "max.tail.width :\t " + (int)ShapeFixParameters.MaxTailWidth + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix selfintersecting of edge\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "selfintersecting.edge :\t "
             + (int)ShapeFixParameters.FixSelfIntersectingEdgeMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix intersecting edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "intersecting.edges :\t " + (int)ShapeFixParameters.FixIntersectingEdgesMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix non adjacent intersecting edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "nonadjacent.intersecting.edges :\t "
             + (int)ShapeFixParameters.FixNonAdjacentIntersectingEdgesMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying ShapeFix::FixVertexPosition before all fixes\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "vertex.position :\t " + (int)ShapeFixParameters.FixVertexPositionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for increases the tolerances of the edge vertices to comprise ";
  aResult += "!the ends of 3d curve and pcurve on the given face (first method) or all pcurves "
             "stored in an edge (second one)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "vertex.tolerance :\t " + (int)ShapeFixParameters.FixVertexToleranceMode + "\n";
  aResult += "!\n";

  return aResult;
}
