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

  = default;

//=================================================================================================

DE_ShapeFixConfigurationNode::DE_ShapeFixConfigurationNode(
  const occ::handle<DE_ShapeFixConfigurationNode>& theNode)
    : DE_ConfigurationNode(theNode),
      ShapeFixParameters(theNode->ShapeFixParameters)
{
}

//=================================================================================================

bool DE_ShapeFixConfigurationNode::Load(const occ::handle<DE_ConfigurationContext>& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".healing";

  ShapeFixParameters.Tolerance3d =
    theResource->RealVal("tolerance3d", ShapeFixParameters.Tolerance3d, aScope);
  ShapeFixParameters.MaxTolerance3d =
    theResource->RealVal("max.tolerance3d", ShapeFixParameters.MaxTolerance3d, aScope);
  ShapeFixParameters.MinTolerance3d =
    theResource->RealVal("min.tolerance3d", ShapeFixParameters.MinTolerance3d, aScope);
  ShapeFixParameters.FixFreeShellMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("free.shell",
                            static_cast<int>(ShapeFixParameters.FixFreeShellMode),
                            aScope));
  ShapeFixParameters.FixFreeFaceMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("free.face",
                            static_cast<int>(ShapeFixParameters.FixFreeFaceMode),
                            aScope));
  ShapeFixParameters.FixFreeWireMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("free.wire",
                            static_cast<int>(ShapeFixParameters.FixFreeWireMode),
                            aScope));
  ShapeFixParameters.FixSameParameterMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("same.parameter",
                            static_cast<int>(ShapeFixParameters.FixSameParameterMode),
                            aScope));
  ShapeFixParameters.FixSolidMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("solid", static_cast<int>(ShapeFixParameters.FixSolidMode), aScope));
  ShapeFixParameters.FixShellOrientationMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("shell.orientation",
                            static_cast<int>(ShapeFixParameters.FixShellOrientationMode),
                            aScope));
  ShapeFixParameters.CreateOpenSolidMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("create.open.solid",
                            static_cast<int>(ShapeFixParameters.CreateOpenSolidMode),
                            aScope));
  ShapeFixParameters.FixShellMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("shell", static_cast<int>(ShapeFixParameters.FixShellMode), aScope));
  ShapeFixParameters.FixFaceOrientationMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("face.orientation",
                            static_cast<int>(ShapeFixParameters.FixFaceOrientationMode),
                            aScope));
  ShapeFixParameters.FixFaceMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("face", static_cast<int>(ShapeFixParameters.FixFaceMode), aScope));
  ShapeFixParameters.FixWireMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("wire", static_cast<int>(ShapeFixParameters.FixWireMode), aScope));
  ShapeFixParameters.FixOrientationMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("orientation",
                            static_cast<int>(ShapeFixParameters.FixOrientationMode),
                            aScope));
  ShapeFixParameters.FixAddNaturalBoundMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("add.natural.bound",
                            static_cast<int>(ShapeFixParameters.FixAddNaturalBoundMode),
                            aScope));
  ShapeFixParameters.FixMissingSeamMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("missing.seam",
                            static_cast<int>(ShapeFixParameters.FixMissingSeamMode),
                            aScope));
  ShapeFixParameters.FixSmallAreaWireMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("small.area.wire",
                            static_cast<int>(ShapeFixParameters.FixSmallAreaWireMode),
                            aScope));
  ShapeFixParameters.RemoveSmallAreaFaceMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("remove.small.area.face",
                            static_cast<int>(ShapeFixParameters.RemoveSmallAreaFaceMode),
                            aScope));
  ShapeFixParameters.FixIntersectingWiresMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("intersecting.wires",
                            static_cast<int>(ShapeFixParameters.FixIntersectingWiresMode),
                            aScope));
  ShapeFixParameters.FixLoopWiresMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("loop.wires",
                            static_cast<int>(ShapeFixParameters.FixLoopWiresMode),
                            aScope));
  ShapeFixParameters.FixSplitFaceMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("split.face",
                            static_cast<int>(ShapeFixParameters.FixSplitFaceMode),
                            aScope));
  ShapeFixParameters.AutoCorrectPrecisionMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("auto.correct.precision",
                            static_cast<int>(ShapeFixParameters.AutoCorrectPrecisionMode),
                            aScope));
  ShapeFixParameters.ModifyTopologyMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("modify.topology",
                            static_cast<int>(ShapeFixParameters.ModifyTopologyMode),
                            aScope));
  ShapeFixParameters.ModifyGeometryMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("modify.geometry",
                            static_cast<int>(ShapeFixParameters.ModifyGeometryMode),
                            aScope));
  ShapeFixParameters.ClosedWireMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("closed.wire",
                            static_cast<int>(ShapeFixParameters.ClosedWireMode),
                            aScope));
  ShapeFixParameters.PreferencePCurveMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("preference.pcurve",
                            static_cast<int>(ShapeFixParameters.PreferencePCurveMode),
                            aScope));
  ShapeFixParameters.FixReorderMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("reorder.edges",
                            static_cast<int>(ShapeFixParameters.FixReorderMode),
                            aScope));
  ShapeFixParameters.FixSmallMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("remove.small.edges",
                            static_cast<int>(ShapeFixParameters.FixSmallMode),
                            aScope));
  ShapeFixParameters.FixConnectedMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("connected.edges",
                            static_cast<int>(ShapeFixParameters.FixConnectedMode),
                            aScope));
  ShapeFixParameters.FixEdgeCurvesMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("edge.curves",
                            static_cast<int>(ShapeFixParameters.FixEdgeCurvesMode),
                            aScope));
  ShapeFixParameters.FixDegeneratedMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("add.degenerated.edges",
                            static_cast<int>(ShapeFixParameters.FixDegeneratedMode),
                            aScope));
  ShapeFixParameters.FixLackingMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("add.lacking.edges",
                            static_cast<int>(ShapeFixParameters.FixLackingMode),
                            aScope));
  ShapeFixParameters.FixSelfIntersectionMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("selfintersection",
                            static_cast<int>(ShapeFixParameters.FixSelfIntersectionMode),
                            aScope));
  ShapeFixParameters.RemoveLoopMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("remove.loop",
                            static_cast<int>(ShapeFixParameters.RemoveLoopMode),
                            aScope));
  ShapeFixParameters.FixReversed2dMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("reversed2d",
                            static_cast<int>(ShapeFixParameters.FixReversed2dMode),
                            aScope));
  ShapeFixParameters.FixRemovePCurveMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("remove.pcurve",
                            static_cast<int>(ShapeFixParameters.FixRemovePCurveMode),
                            aScope));
  ShapeFixParameters.FixRemoveCurve3dMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("remove.curve3d",
                            static_cast<int>(ShapeFixParameters.FixRemoveCurve3dMode),
                            aScope));
  ShapeFixParameters.FixAddPCurveMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("add.pcurve",
                            static_cast<int>(ShapeFixParameters.FixAddPCurveMode),
                            aScope));
  ShapeFixParameters.FixAddCurve3dMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("add.curve3d",
                            static_cast<int>(ShapeFixParameters.FixAddCurve3dMode),
                            aScope));
  ShapeFixParameters.FixSeamMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("correct.order.in.seam",
                            static_cast<int>(ShapeFixParameters.FixSeamMode),
                            aScope));
  ShapeFixParameters.FixShiftedMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("shifted",
                            static_cast<int>(ShapeFixParameters.FixShiftedMode),
                            aScope));
  ShapeFixParameters.FixEdgeSameParameterMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("edge.same.parameter",
                            static_cast<int>(ShapeFixParameters.FixEdgeSameParameterMode),
                            aScope));
  ShapeFixParameters.FixNotchedEdgesMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("notched.edges",
                            static_cast<int>(ShapeFixParameters.FixNotchedEdgesMode),
                            aScope));
  ShapeFixParameters.FixTailMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("tail", static_cast<int>(ShapeFixParameters.FixTailMode), aScope));
  ShapeFixParameters.MaxTailAngle = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("max.tail.angle",
                            static_cast<int>(ShapeFixParameters.MaxTailAngle),
                            aScope));
  ShapeFixParameters.MaxTailWidth = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("max.tail.width",
                            static_cast<int>(ShapeFixParameters.MaxTailWidth),
                            aScope));
  ShapeFixParameters.FixSelfIntersectingEdgeMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("selfintersecting.edge",
                            static_cast<int>(ShapeFixParameters.FixSelfIntersectingEdgeMode),
                            aScope));
  ShapeFixParameters.FixIntersectingEdgesMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("intersecting.edges",
                            static_cast<int>(ShapeFixParameters.FixIntersectingEdgesMode),
                            aScope));
  ShapeFixParameters.FixNonAdjacentIntersectingEdgesMode =
    static_cast<DE_ShapeFixParameters::FixMode>(theResource->IntegerVal(
      "nonadjacent.intersecting.edges",
      static_cast<int>(ShapeFixParameters.FixNonAdjacentIntersectingEdgesMode),
      aScope));
  ShapeFixParameters.FixVertexPositionMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("vertex.position",
                            static_cast<int>(ShapeFixParameters.FixVertexPositionMode),
                            aScope));
  ShapeFixParameters.FixVertexToleranceMode = static_cast<DE_ShapeFixParameters::FixMode>(
    theResource->IntegerVal("vertex.tolerance",
                            static_cast<int>(ShapeFixParameters.FixVertexToleranceMode),
                            aScope));

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
  aResult +=
    aScope + "free.shell :\t " + static_cast<int>(ShapeFixParameters.FixFreeShellMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Face for ShapeFix_Shape\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "free.face :\t " + static_cast<int>(ShapeFixParameters.FixFreeFaceMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Wire for ShapeFix_Shape\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "free.wire :\t " + static_cast<int>(ShapeFixParameters.FixFreeWireMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying ShapeFix::SameParameter after all fixes\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "same.parameter :\t "
             + static_cast<int>(ShapeFixParameters.FixSameParameterMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Solid\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "solid :\t " + static_cast<int>(ShapeFixParameters.FixSolidMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines the mode for applying analysis and fixes of orientation of shells in the solid\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "shell.orientation :\t "
             + static_cast<int>(ShapeFixParameters.FixShellOrientationMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for creation of solids. If operation is executed then solids are "
             "created from open shells ";
  aResult += "else solids are created from closed shells only\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "create.open.solid :\t "
             + static_cast<int>(ShapeFixParameters.CreateOpenSolidMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Shell for ShapeFix_Solid\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "shell :\t " + static_cast<int>(ShapeFixParameters.FixShellMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines the mode for applying analysis and fixes of orientation of faces in the shell\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "face.orientation :\t "
             + static_cast<int>(ShapeFixParameters.FixFaceOrientationMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "face :\t " + static_cast<int>(ShapeFixParameters.FixFaceMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Wire\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "wire :\t " + static_cast<int>(ShapeFixParameters.FixWireMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying a fix for the orientation of faces in the shell\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "orientation :\t " + static_cast<int>(ShapeFixParameters.FixOrientationMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the add natural bound mode. If operation is executed then natural boundary "
             "is added on faces that miss them\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "add.natural.bound :\t "
             + static_cast<int>(ShapeFixParameters.FixAddNaturalBoundMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix missing seam mode (tries to insert seam is missed)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "missing.seam :\t " + static_cast<int>(ShapeFixParameters.FixMissingSeamMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix small area wire mode (drops small wires)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "small.area.wire :\t "
             + static_cast<int>(ShapeFixParameters.FixSmallAreaWireMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the remove face with small area (drops faces with small outer wires)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "remove.small.area.face :\t "
             + static_cast<int>(ShapeFixParameters.RemoveSmallAreaFaceMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix intersecting wires mode in ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "intersecting.wires :\t "
             + static_cast<int>(ShapeFixParameters.FixIntersectingWiresMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix loop wires mode in ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "loop.wires :\t " + static_cast<int>(ShapeFixParameters.FixLoopWiresMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix split face mode in ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "split.face :\t " + static_cast<int>(ShapeFixParameters.FixSplitFaceMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the auto-correct precision mode in ShapeFix_Face\n";
  aResult +=
    "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "auto.correct.precision :\t "
             + static_cast<int>(ShapeFixParameters.AutoCorrectPrecisionMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to modify topology of the wire during fixing "
             "(adding/removing edges etc.)\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "modify.topology :\t "
             + static_cast<int>(ShapeFixParameters.ModifyTopologyMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to modify geometry of the edges and vertices\n";
  aResult +=
    "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "modify.geometry :\t "
             + static_cast<int>(ShapeFixParameters.ModifyGeometryMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to defines whether the wire is to be closed (by calling "
             "methods like FixDegenerated() ";
  aResult += "!and FixConnected() for lastand first edges\n";
  aResult +=
    "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult +=
    aScope + "closed.wire :\t " + static_cast<int>(ShapeFixParameters.ClosedWireMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to defines the 2d representation of the wire is preferable "
             "over 3d one ";
  aResult += "(in the case of ambiguity in FixEdgeCurves)\n";
  aResult += "!and FixConnected() for lastand first edges\n";
  aResult +=
    "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "preference.pcurve :\t "
             + static_cast<int>(ShapeFixParameters.PreferencePCurveMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to reorder edges in the wire\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "reorder.edges :\t " + static_cast<int>(ShapeFixParameters.FixReorderMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove small edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "remove.small.edges :\t " + static_cast<int>(ShapeFixParameters.FixSmallMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix connecting edges in the wire\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "connected.edges :\t " + static_cast<int>(ShapeFixParameters.FixConnectedMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix edges (3Dcurves and 2D curves)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "edge.curves :\t " + static_cast<int>(ShapeFixParameters.FixEdgeCurvesMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for add degenerated edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "add.degenerated.edges :\t "
             + static_cast<int>(ShapeFixParameters.FixDegeneratedMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for add lacking edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "add.lacking.edges :\t " + static_cast<int>(ShapeFixParameters.FixLackingMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix selfintersection edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "selfintersection :\t "
             + static_cast<int>(ShapeFixParameters.FixSelfIntersectionMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove loop\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "remove.loop :\t " + static_cast<int>(ShapeFixParameters.RemoveLoopMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to fix edge if pcurve is directed opposite to 3d curve\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "reversed2d :\t " + static_cast<int>(ShapeFixParameters.FixReversed2dMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove the pcurve(s) of the edge if it does not match "
             "the vertices\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "remove.pcurve :\t " + static_cast<int>(ShapeFixParameters.FixRemovePCurveMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines the mode allowed to remove 3d curve of the edge if it does not match the vertices\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "remove.curve3d :\t "
             + static_cast<int>(ShapeFixParameters.FixRemoveCurve3dMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines the mode allowed to add pcurve(s) of the edge if missing (by projecting 3d curve)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "add.pcurve :\t " + static_cast<int>(ShapeFixParameters.FixAddPCurveMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to build 3d curve of the edge if missing\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "add.curve3d :\t " + static_cast<int>(ShapeFixParameters.FixAddCurve3dMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to correct order of pcurves in the seam edge depends on "
             "its orientation\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "correct.order.in.seam :\t " + static_cast<int>(ShapeFixParameters.FixSeamMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to shifts wrong 2D curves back, ensuring that the 2D "
             "curves of the edges in the wire are connected\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "shifted :\t " + static_cast<int>(ShapeFixParameters.FixShiftedMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying EdgeSameParameter\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "edge.same.parameter :\t "
             + static_cast<int>(ShapeFixParameters.FixEdgeSameParameterMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix notched edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "notched.edges :\t " + static_cast<int>(ShapeFixParameters.FixNotchedEdgesMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix tail in wire\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "tail :\t " + static_cast<int>(ShapeFixParameters.FixTailMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for max angle of the tails\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "max.tail.angle :\t " + static_cast<int>(ShapeFixParameters.MaxTailAngle) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for max tail width\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult +=
    aScope + "max.tail.width :\t " + static_cast<int>(ShapeFixParameters.MaxTailWidth) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix selfintersecting of edge\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "selfintersecting.edge :\t "
             + static_cast<int>(ShapeFixParameters.FixSelfIntersectingEdgeMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix intersecting edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "intersecting.edges :\t "
             + static_cast<int>(ShapeFixParameters.FixIntersectingEdgesMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix non adjacent intersecting edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "nonadjacent.intersecting.edges :\t "
             + static_cast<int>(ShapeFixParameters.FixNonAdjacentIntersectingEdgesMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying ShapeFix::FixVertexPosition before all fixes\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "vertex.position :\t "
             + static_cast<int>(ShapeFixParameters.FixVertexPositionMode) + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for increases the tolerances of the edge vertices to comprise ";
  aResult += "!the ends of 3d curve and pcurve on the given face (first method) or all pcurves "
             "stored in an edge (second one)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), "
             "\"Fix\"(1)\n";
  aResult += aScope + "vertex.tolerance :\t "
             + static_cast<int>(ShapeFixParameters.FixVertexToleranceMode) + "\n";
  aResult += "!\n";

  return aResult;
}
