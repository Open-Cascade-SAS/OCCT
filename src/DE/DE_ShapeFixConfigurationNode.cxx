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
}

//=======================================================================
// function : DE_ShapeFixConfigurationNode
// purpose  :
//=======================================================================
DE_ShapeFixConfigurationNode::DE_ShapeFixConfigurationNode()
  : DE_ConfigurationNode()
{}

//=======================================================================
// function : DE_ShapeFixConfigurationNode
// purpose  :
//=======================================================================
DE_ShapeFixConfigurationNode::DE_ShapeFixConfigurationNode(const Handle(DE_ShapeFixConfigurationNode)& theNode)
  : DE_ConfigurationNode(theNode)
{
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool DE_ShapeFixConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".healing";

  HealingParameters.Tolerance3d = theResource->RealVal("tolerance3d", HealingParameters.Tolerance3d, aScope);
  HealingParameters.MaxTolerance3d = theResource->RealVal("max.tolerance3d", HealingParameters.MaxTolerance3d, aScope);
  HealingParameters.MinTolerance3d = theResource->RealVal("min.tolerance3d", HealingParameters.MinTolerance3d, aScope);
  HealingParameters.FixFreeShellMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("free.shell", (int)HealingParameters.FixFreeShellMode, aScope);
  HealingParameters.FixFreeFaceMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("free.face", (int)HealingParameters.FixFreeFaceMode, aScope);
  HealingParameters.FixFreeWireMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("free.wire", (int)HealingParameters.FixFreeWireMode, aScope);
  HealingParameters.FixSameParameterMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("same.parameter", (int)HealingParameters.FixSameParameterMode, aScope);
  HealingParameters.FixSolidMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("solid", (int)HealingParameters.FixSolidMode, aScope);
  HealingParameters.FixShellOrientationMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("shell.orientation", (int)HealingParameters.FixShellOrientationMode, aScope);
  HealingParameters.CreateOpenSolidMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("create.open.solid", (int)HealingParameters.CreateOpenSolidMode, aScope);
  HealingParameters.FixShellMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("shell", (int)HealingParameters.FixShellMode, aScope);
  HealingParameters.FixFaceOrientationMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("face.orientation", (int)HealingParameters.FixFaceOrientationMode, aScope);
  HealingParameters.FixFaceMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("face", (int)HealingParameters.FixFaceMode, aScope);
  HealingParameters.FixWireMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("wire", (int)HealingParameters.FixWireMode, aScope);
  HealingParameters.FixOrientationMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("orientation", (int)HealingParameters.FixOrientationMode, aScope);
  HealingParameters.FixAddNaturalBoundMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("add.natural.bound", (int)HealingParameters.FixAddNaturalBoundMode, aScope);
  HealingParameters.FixMissingSeamMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("missing.seam", (int)HealingParameters.FixMissingSeamMode, aScope);
  HealingParameters.FixSmallAreaWireMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("small.area.wire", (int)HealingParameters.FixSmallAreaWireMode, aScope);
  HealingParameters.RemoveSmallAreaFaceMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("remove.small.area.face", (int)HealingParameters.RemoveSmallAreaFaceMode, aScope);
  HealingParameters.FixIntersectingWiresMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("intersecting.wires", (int)HealingParameters.FixIntersectingWiresMode, aScope);
  HealingParameters.FixLoopWiresMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("loop.wires", (int)HealingParameters.FixLoopWiresMode, aScope);
  HealingParameters.FixSplitFaceMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("split.face", (int)HealingParameters.FixSplitFaceMode, aScope);
  HealingParameters.AutoCorrectPrecisionMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("auto.correct.precision", (int)HealingParameters.AutoCorrectPrecisionMode, aScope);
  HealingParameters.ModifyTopologyMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("modify.topology", (int)HealingParameters.ModifyTopologyMode, aScope);
  HealingParameters.ModifyGeometryMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("modify.geometry", (int)HealingParameters.ModifyGeometryMode, aScope);
  HealingParameters.ClosedWireMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("closed.wire", (int)HealingParameters.ClosedWireMode, aScope);
  HealingParameters.PreferencePCurveMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("preference.pcurve", (int)HealingParameters.PreferencePCurveMode, aScope);
  HealingParameters.FixReorderMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("reorder.edges", (int)HealingParameters.FixReorderMode, aScope);
  HealingParameters.FixSmallMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("remove.small.edges", (int)HealingParameters.FixSmallMode, aScope);
  HealingParameters.FixConnectedMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("connected.edges", (int)HealingParameters.FixConnectedMode, aScope);
  HealingParameters.FixEdgeCurvesMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("edge.curves", (int)HealingParameters.FixEdgeCurvesMode, aScope);
  HealingParameters.FixDegeneratedMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("add.degenerated.edges", (int)HealingParameters.FixDegeneratedMode, aScope);
  HealingParameters.FixLackingMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("add.lacking.edges", (int)HealingParameters.FixLackingMode, aScope);
  HealingParameters.FixSelfIntersectionMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("selfintersection", (int)HealingParameters.FixSelfIntersectionMode, aScope);
  HealingParameters.RemoveLoopMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("remove.loop", (int)HealingParameters.RemoveLoopMode, aScope);
  HealingParameters.FixReversed2dMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("reversed2d", (int)HealingParameters.FixReversed2dMode, aScope);
  HealingParameters.FixRemovePCurveMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("remove.pcurve", (int)HealingParameters.FixRemovePCurveMode, aScope);
  HealingParameters.FixRemoveCurve3dMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("remove.curve3d", (int)HealingParameters.FixRemoveCurve3dMode, aScope);
  HealingParameters.FixAddPCurveMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("add.pcurve", (int)HealingParameters.FixAddPCurveMode, aScope);
  HealingParameters.FixAddCurve3dMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("add.curve3d", (int)HealingParameters.FixAddCurve3dMode, aScope);
  HealingParameters.FixSeamMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("correct.order.in.seam", (int)HealingParameters.FixSeamMode, aScope);
  HealingParameters.FixShiftedMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("shifted", (int)HealingParameters.FixShiftedMode, aScope);
  HealingParameters.FixEdgeSameParameterMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("edge.same.parameter", (int)HealingParameters.FixEdgeSameParameterMode, aScope);
  HealingParameters.FixNotchedEdgesMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("notched.edges", (int)HealingParameters.FixNotchedEdgesMode, aScope);
  HealingParameters.FixTailMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("tail", (int)HealingParameters.FixTailMode, aScope);
  HealingParameters.MaxTailAngle = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("max.tail.angle", (int)HealingParameters.MaxTailAngle, aScope);
  HealingParameters.MaxTailWidth = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("max.tail.width", (int)HealingParameters.MaxTailWidth, aScope);
  HealingParameters.FixSelfIntersectingEdgeMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("selfintersecting.edge", (int)HealingParameters.FixSelfIntersectingEdgeMode, aScope);
  HealingParameters.FixIntersectingEdgesMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("intersecting.edges", (int)HealingParameters.FixIntersectingEdgesMode, aScope);
  HealingParameters.FixNonAdjacentIntersectingEdgesMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("nonadjacent.intersecting.edges", (int)HealingParameters.FixNonAdjacentIntersectingEdgesMode, aScope);
  HealingParameters.FixVertexPositionMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("vertex.position", (int)HealingParameters.FixVertexPositionMode, aScope);
  HealingParameters.FixVertexToleranceMode = (DE_ShapeFixParameters::FixMode)
    theResource->IntegerVal("vertex.tolerance", (int)HealingParameters.FixVertexToleranceMode, aScope);

  return true;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString DE_ShapeFixConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".healing";

  aResult += "!\n";
  aResult += "!Shape healing parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the maximum allowable tolerance\n";
  aResult += "!Default value: 1.e-6. Available values: any real positive (non null) value\n";
  aResult += aScope + "tolerance3d :\t " + HealingParameters.Tolerance3d + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the maximum allowable tolerance\n";
  aResult += "!Default value: 1.0. Available values: any real positive (non null) value\n";
  aResult += aScope + "max.tolerance3d :\t " + HealingParameters.MaxTolerance3d + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the minimum allowable tolerance\n";
  aResult += "!Default value: 1.e-7. Available values: any real positive (non null) value\n";
  aResult += aScope + "min.tolerance3d :\t " + HealingParameters.MinTolerance3d + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Shell for ShapeFix_Shape\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "free.shell :\t " + (int)HealingParameters.FixFreeShellMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Face for ShapeFix_Shape\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "free.face :\t " + (int)HealingParameters.FixFreeFaceMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Wire for ShapeFix_Shape\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "free.wire :\t " + (int)HealingParameters.FixFreeWireMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying ShapeFix::SameParameter after all fixes\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "same.parameter :\t " + (int)HealingParameters.FixSameParameterMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Solid\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "solid :\t " + (int)HealingParameters.FixSolidMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying analysis and fixes of orientation of shells in the solid\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "shell.orientation :\t " + (int)HealingParameters.FixShellOrientationMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for creation of solids. If operation is executed then solids are created from open shells ";
  aResult += "else solids are created from closed shells only\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "create.open.solid :\t " + (int)HealingParameters.CreateOpenSolidMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Shell for ShapeFix_Solid\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "shell :\t " + (int)HealingParameters.FixShellMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying analysis and fixes of orientation of faces in the shell\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "face.orientation :\t " + (int)HealingParameters.FixFaceOrientationMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "face :\t " + (int)HealingParameters.FixFaceMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying fixes of ShapeFix_Wire\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "wire :\t " + (int)HealingParameters.FixWireMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying a fix for the orientation of faces in the shell\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "orientation :\t " + (int)HealingParameters.FixOrientationMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the add natural bound mode. If operation is executed then natural boundary is added on faces that miss them\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "add.natural.bound :\t " + (int)HealingParameters.FixAddNaturalBoundMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix missing seam mode (tries to insert seam is missed)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "missing.seam :\t " + (int)HealingParameters.FixMissingSeamMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix small area wire mode (drops small wires)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "small.area.wire :\t " + (int)HealingParameters.FixSmallAreaWireMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the remove face with small area (drops faces with small outer wires)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "remove.small.area.face :\t " + (int)HealingParameters.RemoveSmallAreaFaceMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix intersecting wires mode in ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "intersecting.wires :\t " + (int)HealingParameters.FixIntersectingWiresMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix loop wires mode in ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "loop.wires :\t " + (int)HealingParameters.FixLoopWiresMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the fix split face mode in ShapeFix_Face\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "split.face :\t " + (int)HealingParameters.FixSplitFaceMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the auto-correct precision mode in ShapeFix_Face\n";
  aResult += "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "auto.correct.precision :\t " + (int)HealingParameters.AutoCorrectPrecisionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to modify topology of the wire during fixing (adding/removing edges etc.)\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "modify.topology :\t " + (int)HealingParameters.ModifyTopologyMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to modify geometry of the edges and vertices\n";
  aResult += "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "modify.geometry :\t " + (int)HealingParameters.ModifyGeometryMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to defines whether the wire is to be closed (by calling methods like FixDegenerated() ";
  aResult += "!and FixConnected() for lastand first edges\n";
  aResult += "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "closed.wire :\t " + (int)HealingParameters.ClosedWireMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to defines the 2d representation of the wire is preferable over 3d one ";
  aResult += "(in the case of ambiguity in FixEdgeCurves)\n";
  aResult += "!and FixConnected() for lastand first edges\n";
  aResult += "!Default value: \"Fix\"(1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "preference.pcurve :\t " + (int)HealingParameters.PreferencePCurveMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to reorder edges in the wire\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "reorder.edges :\t " + (int)HealingParameters.FixReorderMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove small edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "remove.small.edges :\t " + (int)HealingParameters.FixSmallMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix connecting edges in the wire\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "connected.edges :\t " + (int)HealingParameters.FixConnectedMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix edges (3Dcurves and 2D curves)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "edge.curves :\t " + (int)HealingParameters.FixEdgeCurvesMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for add degenerated edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "add.degenerated.edges :\t " + (int)HealingParameters.FixDegeneratedMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for add lacking edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "add.lacking.edges :\t " + (int)HealingParameters.FixLackingMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix selfintersection edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "selfintersection :\t " + (int)HealingParameters.FixSelfIntersectionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove loop\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "remove.loop :\t " + (int)HealingParameters.RemoveLoopMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to fix edge if pcurve is directed opposite to 3d curve\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "reversed2d :\t " + (int)HealingParameters.FixReversed2dMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove the pcurve(s) of the edge if it does not match the vertices\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "remove.pcurve :\t " + (int)HealingParameters.FixRemovePCurveMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to remove 3d curve of the edge if it does not match the vertices\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "remove.curve3d :\t " + (int)HealingParameters.FixRemoveCurve3dMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to add pcurve(s) of the edge if missing (by projecting 3d curve)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "add.pcurve :\t " + (int)HealingParameters.FixAddPCurveMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to build 3d curve of the edge if missing\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "add.curve3d :\t " + (int)HealingParameters.FixAddCurve3dMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to correct order of pcurves in the seam edge depends on its orientation\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "correct.order.in.seam :\t " + (int)HealingParameters.FixSeamMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode allowed to shifts wrong 2D curves back, ensuring that the 2D curves of the edges in the wire are connected\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "shifted :\t " + (int)HealingParameters.FixShiftedMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying EdgeSameParameter\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "edge.same.parameter :\t " + (int)HealingParameters.FixEdgeSameParameterMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix notched edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "notched.edges :\t " + (int)HealingParameters.FixNotchedEdgesMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix tail in wire\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "tail :\t " + (int)HealingParameters.FixTailMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for max angle of the tails\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "max.tail.angle :\t " + (int)HealingParameters.MaxTailAngle + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for max tail width\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "max.tail.width :\t " + (int)HealingParameters.MaxTailWidth + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix selfintersecting of edge\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "selfintersecting.edge :\t " + (int)HealingParameters.FixSelfIntersectingEdgeMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix intersecting edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "intersecting.edges :\t " + (int)HealingParameters.FixIntersectingEdgesMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for fix non adjacent intersecting edges\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "nonadjacent.intersecting.edges :\t " + (int)HealingParameters.FixNonAdjacentIntersectingEdgesMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for applying ShapeFix::FixVertexPosition before all fixes\n";
  aResult += "!Default value: \"NotFix\"(0). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "vertex.position :\t " + (int)HealingParameters.FixVertexPositionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode for increases the tolerances of the edge vertices to comprise ";
  aResult += "!the ends of 3d curve and pcurve on the given face (first method) or all pcurves stored in an edge (second one)\n";
  aResult += "!Default value: \"FixOrNot\"(-1). Available values: \"FixOrNot\"(-1), \"NotFix\"(0), \"Fix\"(1)\n";
  aResult += aScope + "vertex.tolerance :\t " + (int)HealingParameters.FixVertexToleranceMode + "\n";
  aResult += "!\n";

  return aResult;
}
