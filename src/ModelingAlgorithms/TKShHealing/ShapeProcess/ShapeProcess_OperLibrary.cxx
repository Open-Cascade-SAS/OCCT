// Created on: 2000-08-31
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRepTools_Modifier.hxx>
#include <Message_ProgressScope.hxx>
#include <Precision.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeCustom_BSplineRestriction.hxx>
#include <ShapeCustom_ConvertToBSpline.hxx>
#include <ShapeCustom_ConvertToRevolution.hxx>
#include <ShapeCustom_DirectModification.hxx>
#include <ShapeCustom_RestrictionParameters.hxx>
#include <ShapeCustom_SweptToElementary.hxx>
#include <ShapeExtend_MsgRegistrator.hxx>
#include <ShapeFix.hxx>
#include <ShapeFix_FixSmallFace.hxx>
#include <ShapeFix_FixSmallSolid.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_SplitCommonVertex.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Wireframe.hxx>
#include <ShapeProcess.hxx>
#include <ShapeProcess_OperLibrary.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <ShapeProcess_UOperator.hxx>
#include <ShapeUpgrade_ShapeConvertToBezier.hxx>
#include <ShapeUpgrade_ShapeDivideAngle.hxx>
#include <ShapeUpgrade_ShapeDivideClosed.hxx>
#include <ShapeUpgrade_ShapeDivideClosedEdges.hxx>
#include <ShapeUpgrade_ShapeDivideContinuity.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
// function : ApplyModifier
// purpose  : Applies BRepTools_Modification to a shape,
//           taking into account sharing of components of compounds
//=======================================================================
TopoDS_Shape ShapeProcess_OperLibrary::ApplyModifier(
  const TopoDS_Shape&                                                       S,
  const occ::handle<ShapeProcess_ShapeContext>&                             context,
  const occ::handle<BRepTools_Modification>&                                M,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& map,
  const occ::handle<ShapeExtend_MsgRegistrator>&                            msg,
  bool                                                                      theMutableInput)
{
  // protect against INTERNAL/EXTERNAL shapes
  TopoDS_Shape SF = S.Oriented(TopAbs_FORWARD);

  // Process COMPOUNDs separately in order to handle sharing in assemblies
  if (SF.ShapeType() == TopAbs_COMPOUND)
  {
    bool            locModified = false;
    TopoDS_Compound C;
    BRep_Builder    B;
    B.MakeCompound(C);
    for (TopoDS_Iterator it(SF); it.More(); it.Next())
    {
      TopoDS_Shape    shape = it.Value();
      TopLoc_Location L     = shape.Location(), nullLoc;
      shape.Location(nullLoc);
      TopoDS_Shape res;
      if (map.IsBound(shape))
        res = map.Find(shape).Oriented(shape.Orientation());

      else
      {
        res = ApplyModifier(shape, context, M, map, 0, theMutableInput);
        map.Bind(shape, res);
      }
      if (!res.IsSame(shape))
        locModified = true;
      res.Location(L, false);
      B.Add(C, res);
    }
    if (!locModified)
      return S;

    map.Bind(SF, C);
    return C.Oriented(S.Orientation());
  }

  // Modify the shape
  BRepTools_Modifier MD(SF);
  MD.SetMutableInput(theMutableInput);
  MD.Perform(M);
  context->RecordModification(SF, MD, msg);
  return MD.ModifiedShape(SF).Oriented(S.Orientation());
}

//=================================================================================================

static bool directfaces(const occ::handle<ShapeProcess_Context>& context,
                        const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  occ::handle<ShapeCustom_DirectModification> DM = new ShapeCustom_DirectModification;
  DM->SetMsgRegistrator(msg);
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> map;
  TopoDS_Shape                                                             res =
    ShapeProcess_OperLibrary::ApplyModifier(ctx->Result(), ctx, DM, map, msg, true);
  ctx->RecordModification(map, msg);
  ctx->SetResult(res);
  return true;
}

//=================================================================================================

static bool sameparam(const occ::handle<ShapeProcess_Context>& context,
                      const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  ShapeFix::SameParameter(ctx->Result(),
                          ctx->BooleanVal("Force", false),
                          ctx->RealVal("Tolerance3d", Precision::Confusion() /* -1 */),
                          Message_ProgressRange(),
                          msg);

  if (!msg.IsNull())
  {
    // WARNING: not FULL update of context yet!
    occ::handle<ShapeBuild_ReShape> reshape = new ShapeBuild_ReShape;
    ctx->RecordModification(reshape, msg);
  }
  return true;
}

//=================================================================================================

static bool settol(const occ::handle<ShapeProcess_Context>& context, const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  double val;
  if (ctx->IntegerVal("Mode", 0) > 0 && ctx->GetReal("Value", val))
  {
    double rat = ctx->RealVal("Ratio", 1.);
    if (rat >= 1)
    {
      ShapeFix_ShapeTolerance SFST;
      SFST.LimitTolerance(ctx->Result(), val / rat, val * rat);
    }
  }

  BRepLib::UpdateTolerances(ctx->Result(), true);

  double reg;
  if (ctx->GetReal("Regularity", reg))
    BRepLib::EncodeRegularity(ctx->Result(), reg);

  // WARNING: no update of context yet!
  return true;
}

//=================================================================================================

static bool splitangle(const occ::handle<ShapeProcess_Context>& context,
                       const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  ShapeUpgrade_ShapeDivideAngle SDA(ctx->RealVal("Angle", 2 * M_PI), ctx->Result());
  SDA.SetMaxTolerance(ctx->RealVal("MaxTolerance", 1.));
  SDA.SetMsgRegistrator(msg);

  if (!SDA.Perform() && SDA.Status(ShapeExtend_FAIL))
  {
#ifdef OCCT_DEBUG
    std::cout << "ShapeDivideAngle failed" << std::endl;
#endif
    return false;
  }

  ctx->RecordModification(SDA.GetContext(), msg);
  ctx->SetResult(SDA.Result());
  return true;
}

//=================================================================================================

static bool bsplinerestriction(const occ::handle<ShapeProcess_Context>& context,
                               const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  bool ModeSurf = ctx->BooleanVal("SurfaceMode", true);
  bool ModeC3d  = ctx->BooleanVal("Curve3dMode", true);
  bool ModeC2d  = ctx->BooleanVal("Curve2dMode", true);

  double aTol3d = ctx->RealVal("Tolerance3d", 0.01);
  double aTol2d = ctx->RealVal("Tolerance2d", 1e-06);

  GeomAbs_Shape aCont3d = ctx->ContinuityVal("Continuity3d", GeomAbs_C1);
  GeomAbs_Shape aCont2d = ctx->ContinuityVal("Continuity2d", GeomAbs_C2);

  int aMaxDeg = ctx->IntegerVal("RequiredDegree", 9);
  int aMaxSeg = ctx->IntegerVal("RequiredNbSegments", 10000);

  bool ModeDeg  = ctx->BooleanVal("PreferDegree", true);
  bool Rational = ctx->BooleanVal("RationalToPolynomial", false);

  occ::handle<ShapeCustom_RestrictionParameters> aParameters =
    new ShapeCustom_RestrictionParameters;
  ctx->GetInteger("MaxDegree", aParameters->GMaxDegree());
  ctx->GetInteger("MaxNbSegments", aParameters->GMaxSeg());
  ctx->GetBoolean("OffsetSurfaceMode", aParameters->ConvertOffsetSurf());
  ctx->GetBoolean("OffsetCurve3dMode", aParameters->ConvertOffsetCurv3d());
  ctx->GetBoolean("OffsetCurve2dMode", aParameters->ConvertOffsetCurv2d());
  ctx->GetBoolean("LinearExtrusionMode", aParameters->ConvertExtrusionSurf());
  ctx->GetBoolean("RevolutionMode", aParameters->ConvertRevolutionSurf());
  ctx->GetBoolean("SegmentSurfaceMode", aParameters->SegmentSurfaceMode());
  ctx->GetBoolean("ConvCurve3dMode", aParameters->ConvertCurve3d());
  ctx->GetBoolean("ConvCurve2dMode", aParameters->ConvertCurve2d());
  ctx->GetBoolean("BezierMode", aParameters->ConvertBezierSurf());
  // modes to convert elementary surfaces
  ctx->GetBoolean("PlaneMode", aParameters->ConvertPlane());
  // ctx->GetBoolean ("ElementarySurfMode", aParameters->ConvertElementarySurf());
  ctx->GetBoolean("ConicalSurfMode", aParameters->ConvertConicalSurf());
  ctx->GetBoolean("CylindricalSurfMode", aParameters->ConvertCylindricalSurf());
  ctx->GetBoolean("ToroidalSurfMode", aParameters->ConvertToroidalSurf());
  ctx->GetBoolean("SphericalSurfMode", aParameters->ConvertSphericalSurf());

  occ::handle<ShapeCustom_BSplineRestriction> LD = new ShapeCustom_BSplineRestriction(ModeSurf,
                                                                                      ModeC3d,
                                                                                      ModeC2d,
                                                                                      aTol3d,
                                                                                      aTol2d,
                                                                                      aCont3d,
                                                                                      aCont2d,
                                                                                      aMaxDeg,
                                                                                      aMaxSeg,
                                                                                      ModeDeg,
                                                                                      Rational,
                                                                                      aParameters);
  LD->SetMsgRegistrator(msg);
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> map;
  TopoDS_Shape                                                             res =
    ShapeProcess_OperLibrary::ApplyModifier(ctx->Result(), ctx, LD, map, msg, true);
  ctx->RecordModification(map, msg);
  ctx->SetResult(res);
  return true;
}

//=================================================================================================

static bool torevol(const occ::handle<ShapeProcess_Context>& context, const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  occ::handle<ShapeCustom_ConvertToRevolution> CR = new ShapeCustom_ConvertToRevolution();
  CR->SetMsgRegistrator(msg);
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> map;
  TopoDS_Shape                                                             res =
    ShapeProcess_OperLibrary::ApplyModifier(ctx->Result(), ctx, CR, map, msg, true);
  ctx->RecordModification(map, msg);
  ctx->SetResult(res);
  return true;
}

//=================================================================================================

static bool swepttoelem(const occ::handle<ShapeProcess_Context>& context,
                        const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  occ::handle<ShapeCustom_SweptToElementary> SE = new ShapeCustom_SweptToElementary();
  SE->SetMsgRegistrator(msg);
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> map;
  TopoDS_Shape                                                             res =
    ShapeProcess_OperLibrary::ApplyModifier(ctx->Result(), ctx, SE, map, msg, true);
  ctx->RecordModification(map, msg);
  ctx->SetResult(res);
  return true;
}

//=================================================================================================

static bool shapetobezier(const occ::handle<ShapeProcess_Context>& context,
                          const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  bool ModeC3d        = ctx->BooleanVal("Curve3dMode", false);
  bool ModeC2d        = ctx->BooleanVal("Curve2dMode", false);
  bool ModeSurf       = ctx->BooleanVal("SurfaceMode", false);
  bool ModeLine3d     = ctx->BooleanVal("Line3dMode", true);
  bool ModeCircle3d   = ctx->BooleanVal("Circle3dMode", true);
  bool ModeConic3d    = ctx->BooleanVal("Conic3dMode", true);
  bool SegmentMode    = ctx->BooleanVal("SegmentSurfaceMode", true);
  bool PlaneMode      = ctx->BooleanVal("PlaneMode", true);
  bool RevolutionMode = ctx->BooleanVal("RevolutionMode", true);
  bool ExtrusionMode  = ctx->BooleanVal("ExtrusionMode", true);
  bool BSplineMode    = ctx->BooleanVal("BSplineMode", true);

  ShapeUpgrade_ShapeConvertToBezier SCB(ctx->Result());
  SCB.SetMsgRegistrator(msg);
  SCB.SetSurfaceSegmentMode(SegmentMode);
  SCB.SetSurfaceConversion(ModeSurf);
  SCB.Set2dConversion(ModeC2d);
  SCB.Set3dConversion(ModeC3d);
  if (ModeC3d)
  {
    SCB.Set3dLineConversion(ModeLine3d);
    SCB.Set3dCircleConversion(ModeCircle3d);
    SCB.Set3dConicConversion(ModeConic3d);
  }
  if (ModeSurf)
  {
    SCB.SetPlaneMode(PlaneMode);
    SCB.SetRevolutionMode(RevolutionMode);
    SCB.SetExtrusionMode(ExtrusionMode);
    SCB.SetBSplineMode(BSplineMode);
  }

  double maxTol, minTol;
  if (ctx->GetReal("MaxTolerance", maxTol))
    SCB.SetMaxTolerance(maxTol);
  if (ctx->GetReal("MinCurveLength", minTol))
    SCB.SetMinTolerance(minTol);

  bool EdgeMode;
  if (ctx->GetBoolean("EdgeMode", EdgeMode))
    SCB.SetEdgeMode(EdgeMode);

  if (!SCB.Perform() && SCB.Status(ShapeExtend_FAIL))
  {
#ifdef OCCT_DEBUG
    std::cout << "Shape::ShapeConvertToBezier failed" << std::endl; // !!!!
#endif
    return false;
  }

  ctx->RecordModification(SCB.GetContext(), msg);
  ctx->SetResult(SCB.Result());
  return true;
}

//=================================================================================================

static bool converttobspline(const occ::handle<ShapeProcess_Context>& context,
                             const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  bool extrMode   = ctx->BooleanVal("LinearExtrusionMode", true);
  bool revolMode  = ctx->BooleanVal("RevolutionMode", true);
  bool offsetMode = ctx->BooleanVal("OffsetMode", true);

  occ::handle<ShapeCustom_ConvertToBSpline> CBspl = new ShapeCustom_ConvertToBSpline();
  CBspl->SetExtrusionMode(extrMode);
  CBspl->SetRevolutionMode(revolMode);
  CBspl->SetOffsetMode(offsetMode);
  CBspl->SetMsgRegistrator(msg);

  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> map;
  TopoDS_Shape                                                             res =
    ShapeProcess_OperLibrary::ApplyModifier(ctx->Result(), ctx, CBspl, map, msg, true);
  ctx->RecordModification(map, msg);
  ctx->SetResult(res);
  return true;
}

//=================================================================================================

static bool splitcontinuity(const occ::handle<ShapeProcess_Context>& context,
                            const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  double        aTol       = ctx->RealVal("Tolerance3d", 1.e-7);
  double        aTol2D     = ctx->RealVal("Tolerance2d", 1.e-9);
  GeomAbs_Shape aCrvCont   = ctx->ContinuityVal("CurveContinuity", GeomAbs_C1);
  GeomAbs_Shape aSrfCont   = ctx->ContinuityVal("SurfaceContinuity", GeomAbs_C1);
  GeomAbs_Shape aCrv2dCont = ctx->ContinuityVal("Curve2dContinuity", GeomAbs_C1);
  ShapeUpgrade_ShapeDivideContinuity tool(ctx->Result());
  tool.SetBoundaryCriterion(aCrvCont);
  tool.SetSurfaceCriterion(aSrfCont);
  tool.SetPCurveCriterion(aCrv2dCont);
  tool.SetTolerance(aTol);
  tool.SetTolerance2d(aTol2D);

  tool.SetMsgRegistrator(msg);

  double maxTol;
  if (ctx->GetReal("MaxTolerance", maxTol))
    tool.SetMaxTolerance(maxTol);

  if (!tool.Perform() && tool.Status(ShapeExtend_FAIL))
  {
#ifdef OCCT_DEBUG
    std::cout << "SplitContinuity failed" << std::endl;
#endif
    return false;
  }

  ctx->RecordModification(tool.GetContext(), msg);
  ctx->SetResult(tool.Result());
  return true;
}

//=================================================================================================

static bool splitclosedfaces(const occ::handle<ShapeProcess_Context>& context,
                             const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  ShapeUpgrade_ShapeDivideClosed tool(ctx->Result());
  tool.SetMsgRegistrator(msg);

  double closeTol;
  if (ctx->GetReal("CloseTolerance", closeTol))
    tool.SetPrecision(closeTol);

  double maxTol;
  if (ctx->GetReal("MaxTolerance", maxTol))
    tool.SetMaxTolerance(maxTol);

  int  num    = ctx->IntegerVal("NbSplitPoints", 1);
  bool hasSeg = true;
  ctx->GetBoolean("SegmentSurfaceMode", hasSeg);

  tool.SetNbSplitPoints(num);
  tool.SetSurfaceSegmentMode(hasSeg);
  if (!tool.Perform() && tool.Status(ShapeExtend_FAIL))
  {
#ifdef OCCT_DEBUG
    std::cout << "Splitting of closed faces failed" << std::endl;
#endif
    return false;
  }

  ctx->RecordModification(tool.GetContext(), msg);
  ctx->SetResult(tool.Result());
  return true;
}

//=================================================================================================

static bool fixfacesize(const occ::handle<ShapeProcess_Context>& context,
                        const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  occ::handle<ShapeBuild_ReShape> reshape = new ShapeBuild_ReShape;
  ShapeFix_FixSmallFace           FSC;
  FSC.SetContext(reshape);
  FSC.Init(ctx->Result());
  FSC.SetMsgRegistrator(msg);

  double aTol;
  if (ctx->GetReal("Tolerance", aTol))
    FSC.SetPrecision(aTol);

  FSC.Perform();
  TopoDS_Shape newsh = FSC.Shape();

  if (newsh != ctx->Result())
  {
    ctx->RecordModification(reshape, msg);
    ctx->SetResult(newsh);
  }

  return true;
}

//=================================================================================================

static bool fixwgaps(const occ::handle<ShapeProcess_Context>& context, const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  double aTol3d = ctx->RealVal("Tolerance3d", Precision::Confusion());

  occ::handle<ShapeBuild_ReShape> reshape = new ShapeBuild_ReShape;
  occ::handle<ShapeFix_Wireframe> sfwf    = new ShapeFix_Wireframe(ctx->Result());
  sfwf->SetMsgRegistrator(msg);
  sfwf->SetContext(reshape);
  sfwf->SetPrecision(aTol3d);
  sfwf->FixWireGaps();
  TopoDS_Shape result = sfwf->Shape();

  if (result != ctx->Result())
  {
    ctx->RecordModification(reshape, msg);
    ctx->SetResult(result);
  }
  return true;
}

//=================================================================================================

static bool dropsmallsolids(const occ::handle<ShapeProcess_Context>& context,
                            const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  ShapeFix_FixSmallSolid FSS;
  FSS.SetMsgRegistrator(msg);

  double aThreshold;
  int    aMode;
  if (ctx->GetInteger("FixMode", aMode))
    FSS.SetFixMode(aMode);
  if (ctx->GetReal("VolumeThreshold", aThreshold))
    FSS.SetVolumeThreshold(aThreshold);
  if (ctx->GetReal("WidthFactorThreshold", aThreshold))
    FSS.SetWidthFactorThreshold(aThreshold);

  bool aMerge = false;
  ctx->GetBoolean("MergeSolids", aMerge);

  occ::handle<ShapeBuild_ReShape> aReShape = new ShapeBuild_ReShape;

  TopoDS_Shape aResult;
  if (aMerge)
    aResult = FSS.Merge(ctx->Result(), aReShape);
  else
    aResult = FSS.Remove(ctx->Result(), aReShape);

  if (aResult != ctx->Result())
  {
    ctx->RecordModification(aReShape, msg);
    ctx->SetResult(aResult);
  }

  return true;
}

/*
//=================================================================================================

static bool dropsmalledges (const occ::handle<ShapeProcess_Context>& context)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = Handle(ShapeProcess_ShapeContext)::DownCast ( context
); if ( ctx.IsNull() ) return false;

  //occ::handle<ShapeBuild_ReShape> ctx = new ShapeBuild_ReShape;
  occ::handle<MoniFrame_Element> elem = astep->Operand();
  TopoDS_Shape Shape = MoniShape::Shape(elem);
  double aTol3d = Precision::Confusion();
  occ::handle<MoniFrame_TypedValue> ptol3d   = aproc->StackParam("Tolerance3d",true);
  if (ptol3d->IsSetValue()) aTol3d = ptol3d->RealValue();
  occ::handle<ShapeBuild_ReShape> context;
  TopoDS_Shape result = ShapeFix::RemoveSmallEdges(Shape,aTol3d,context);
  if (result == Shape) astep->AddTouched (aproc->Infos(),MoniShape::Element(Shape));
  else
    MoniShapeSW::UpdateFromReShape (aproc->Infos(), astep, Shape, context, TopAbs_FACE);
  return 0;
}
*/

//=================================================================================================

static bool mergesmalledges(const occ::handle<ShapeProcess_Context>& context,
                            const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  double aTol3d = ctx->RealVal("Tolerance3d", Precision::Confusion());

  occ::handle<ShapeBuild_ReShape> reshape = new ShapeBuild_ReShape;
  ShapeFix_Wireframe              ShapeFixWireframe(ctx->Result());
  ShapeFixWireframe.SetContext(reshape);
  ShapeFixWireframe.SetPrecision(aTol3d);
  ShapeFixWireframe.SetMsgRegistrator(msg);

  if (ShapeFixWireframe.FixSmallEdges())
  {
    ctx->RecordModification(reshape, msg);
  }
  return true;
}

//=================================================================================================

static bool fixshape(const occ::handle<ShapeProcess_Context>& context,
                     const Message_ProgressRange&             theProgress)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  occ::handle<ShapeFix_Shape> sfs = new ShapeFix_Shape;
  occ::handle<ShapeFix_Face>  sff = sfs->FixFaceTool();
  occ::handle<ShapeFix_Wire>  sfw = sfs->FixWireTool();
  sfs->SetMsgRegistrator(msg);

  sfs->SetPrecision(ctx->RealVal("Tolerance3d", Precision::Confusion()));
  sfs->SetMinTolerance(ctx->RealVal("MinTolerance3d", Precision::Confusion()));
  sfs->SetMaxTolerance(ctx->RealVal("MaxTolerance3d", Precision::Confusion()));

  sfs->FixFreeShellMode()      = ctx->IntegerVal("FixFreeShellMode", -1);
  sfs->FixFreeFaceMode()       = ctx->IntegerVal("FixFreeFaceMode", -1);
  sfs->FixFreeWireMode()       = ctx->IntegerVal("FixFreeWireMode", -1);
  sfs->FixSameParameterMode()  = ctx->IntegerVal("FixSameParameterMode", -1);
  sfs->FixSolidMode()          = ctx->IntegerVal("FixSolidMode", -1);
  sfs->FixVertexPositionMode() = ctx->IntegerVal("FixVertexPositionMode", 0);
  sfs->FixVertexTolMode()      = ctx->IntegerVal("FixVertexToleranceMode", -1);

  sfs->FixSolidTool()->FixShellMode()            = ctx->IntegerVal("FixShellMode", -1);
  sfs->FixSolidTool()->FixShellOrientationMode() = ctx->IntegerVal("FixShellOrientationMode", -1);
  sfs->FixSolidTool()->CreateOpenSolidMode()     = ctx->BooleanVal("CreateOpenSolidMode", true);

  sfs->FixShellTool()->FixFaceMode() = ctx->IntegerVal("FixFaceMode", -1);
  sfs->FixShellTool()->SetNonManifoldFlag(ctx->IsNonManifold());
  sfs->FixShellTool()->FixOrientationMode() = ctx->IntegerVal("FixFaceOrientationMode", -1);

  // parameters for ShapeFix_Face
  sff->FixWireMode()              = ctx->IntegerVal("FixWireMode", -1);
  sff->FixOrientationMode()       = ctx->IntegerVal("FixOrientationMode", -1);
  sff->FixAddNaturalBoundMode()   = ctx->IntegerVal("FixAddNaturalBoundMode", -1);
  sff->FixMissingSeamMode()       = ctx->IntegerVal("FixMissingSeamMode", -1);
  sff->FixSmallAreaWireMode()     = ctx->IntegerVal("FixSmallAreaWireMode", -1);
  sff->RemoveSmallAreaFaceMode()  = ctx->IntegerVal("RemoveSmallAreaFaceMode", -1);
  sff->FixIntersectingWiresMode() = ctx->IntegerVal("FixIntersectingWiresMode", -1);
  sff->FixLoopWiresMode()         = ctx->IntegerVal("FixLoopWiresMode", -1);
  sff->FixSplitFaceMode()         = ctx->IntegerVal("FixSplitFaceMode", -1);

  // parameters for ShapeFix_Wire
  sfw->ModifyTopologyMode()      = ctx->BooleanVal("ModifyTopologyMode", false);
  sfw->ModifyGeometryMode()      = ctx->BooleanVal("ModifyGeometryMode", true);
  sfw->ClosedWireMode()          = ctx->BooleanVal("ClosedWireMode", true);
  sfw->PreferencePCurveMode()    = ctx->BooleanVal("PreferencePCurveMode", true);
  sfw->FixReorderMode()          = ctx->IntegerVal("FixReorderMode", -1);
  sfw->FixSmallMode()            = ctx->IntegerVal("FixSmallMode", -1);
  sfw->FixConnectedMode()        = ctx->IntegerVal("FixConnectedMode", -1);
  sfw->FixEdgeCurvesMode()       = ctx->IntegerVal("FixEdgeCurvesMode", -1);
  sfw->FixDegeneratedMode()      = ctx->IntegerVal("FixDegeneratedMode", -1);
  sfw->FixLackingMode()          = ctx->IntegerVal("FixLackingMode", -1);
  sfw->FixSelfIntersectionMode() = ctx->IntegerVal("FixSelfIntersectionMode", -1);
  sfw->ModifyRemoveLoopMode()    = ctx->IntegerVal("RemoveLoopMode", -1);
  sfw->FixReversed2dMode()       = ctx->IntegerVal("FixReversed2dMode", -1);
  sfw->FixRemovePCurveMode()     = ctx->IntegerVal("FixRemovePCurveMode", -1);
  sfw->FixRemoveCurve3dMode()    = ctx->IntegerVal("FixRemoveCurve3dMode", -1);
  sfw->FixAddPCurveMode()        = ctx->IntegerVal("FixAddPCurveMode", -1);
  sfw->FixAddCurve3dMode()       = ctx->IntegerVal("FixAddCurve3dMode", -1);
  sfw->FixShiftedMode()          = ctx->IntegerVal("FixShiftedMode", -1);
  sfw->FixSeamMode()             = ctx->IntegerVal("FixSeamMode", -1);
  sfw->FixSameParameterMode()    = ctx->IntegerVal("FixEdgeSameParameterMode", -1);
  sfw->FixNotchedEdgesMode()     = ctx->IntegerVal("FixNotchedEdgesMode", -1);
  sfw->FixTailMode()             = ctx->IntegerVal("FixTailMode", 0);
  sfw->SetMaxTailAngle(ctx->RealVal("MaxTailAngle", 0) * (M_PI / 180));
  sfw->SetMaxTailWidth(ctx->RealVal("MaxTailWidth", -1));
  sfw->FixSelfIntersectingEdgeMode() = ctx->IntegerVal("FixSelfIntersectingEdgeMode", -1);
  sfw->FixIntersectingEdgesMode()    = ctx->IntegerVal("FixIntersectingEdgesMode", -1);
  sfw->FixNonAdjacentIntersectingEdgesMode() =
    ctx->IntegerVal("FixNonAdjacentIntersectingEdgesMode", -1);
  Message_ProgressScope aPS(theProgress, NULL, 2);
  if (sfw->FixTailMode() == 1)
  {
    sfw->FixTailMode() = 0;
    sfs->Init(ctx->Result());
    sfs->Perform(aPS.Next());
    sfw->FixTailMode() = 1;
    if (aPS.UserBreak())
    {
      return false;
    }

    TopoDS_Shape result = sfs->Shape();
    if (result != ctx->Result() || (!msg.IsNull() && !msg->MapShape().IsEmpty()))
    {
      ctx->RecordModification(sfs->Context(), msg);
      ctx->SetResult(result);
    }
  }

  sfs->Init(ctx->Result());
  sfs->Perform(aPS.Next());
  if (aPS.UserBreak())
  {
    return false;
  }

  TopoDS_Shape result = sfs->Shape();
  if ((result != ctx->Result()) || (!msg.IsNull() && !msg->MapShape().IsEmpty()))
  {
    ctx->RecordModification(sfs->Context(), msg);
    ctx->SetResult(result);
  }
  return true;
}

//=================================================================================================

static bool spltclosededges(const occ::handle<ShapeProcess_Context>& context,
                            const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  int nbSplits = ctx->IntegerVal("NbSplitPoints", 1);

  ShapeUpgrade_ShapeDivideClosedEdges tool(ctx->Result());
  tool.SetNbSplitPoints(nbSplits);
  tool.SetMsgRegistrator(msg);

  if (!tool.Perform() && tool.Status(ShapeExtend_FAIL))
  {
#ifdef OCCT_DEBUG
    std::cout << "Splitting of closed edges failed" << std::endl;
#endif
    return false;
  }

  ctx->RecordModification(tool.GetContext(), msg);
  ctx->SetResult(tool.Result());
  return true;
}

//=======================================================================
// function : splitcommonvertex
// purpose  : Two wires have common vertex - this case is valid in BRep model
//           and isn't valid in STEP => before writing into STEP it is necessary
//           to split this vertex (each wire must has one vertex)
//=======================================================================
static bool splitcommonvertex(const occ::handle<ShapeProcess_Context>& context,
                              const Message_ProgressRange&)
{
  occ::handle<ShapeProcess_ShapeContext> ctx = occ::down_cast<ShapeProcess_ShapeContext>(context);
  if (ctx.IsNull())
    return false;

  // activate message mechanism if it is supported by context
  occ::handle<ShapeExtend_MsgRegistrator> msg;
  if (!ctx->Messages().IsNull())
    msg = new ShapeExtend_MsgRegistrator;

  occ::handle<ShapeBuild_ReShape> reshape = new ShapeBuild_ReShape;
  ShapeFix_SplitCommonVertex      SCV;
  SCV.SetContext(reshape);
  SCV.Init(ctx->Result());

  SCV.SetMsgRegistrator(msg);

  SCV.Perform();
  TopoDS_Shape newsh = SCV.Shape();

  if (newsh != ctx->Result())
  {
    ctx->RecordModification(reshape, msg);
    ctx->SetResult(newsh);
  }

  return true;
}

//=======================================================================
// function : Init
// purpose  : Register standard operators
//=======================================================================

void ShapeProcess_OperLibrary::Init()
{
  static bool done = false;
  if (done)
    return;
  done = true;

  ShapeExtend::Init();

  ShapeProcess::RegisterOperator("DirectFaces", new ShapeProcess_UOperator(directfaces));
  ShapeProcess::RegisterOperator("SameParameter", new ShapeProcess_UOperator(sameparam));
  ShapeProcess::RegisterOperator("SetTolerance", new ShapeProcess_UOperator(settol));
  ShapeProcess::RegisterOperator("SplitAngle", new ShapeProcess_UOperator(splitangle));
  ShapeProcess::RegisterOperator("BSplineRestriction",
                                 new ShapeProcess_UOperator(bsplinerestriction));
  ShapeProcess::RegisterOperator("ElementaryToRevolution", new ShapeProcess_UOperator(torevol));
  ShapeProcess::RegisterOperator("SweptToElementary", new ShapeProcess_UOperator(swepttoelem));
  ShapeProcess::RegisterOperator("SurfaceToBSpline", new ShapeProcess_UOperator(converttobspline));
  ShapeProcess::RegisterOperator("ToBezier", new ShapeProcess_UOperator(shapetobezier));
  ShapeProcess::RegisterOperator("SplitContinuity", new ShapeProcess_UOperator(splitcontinuity));
  ShapeProcess::RegisterOperator("SplitClosedFaces", new ShapeProcess_UOperator(splitclosedfaces));
  ShapeProcess::RegisterOperator("FixWireGaps", new ShapeProcess_UOperator(fixwgaps));
  ShapeProcess::RegisterOperator("FixFaceSize", new ShapeProcess_UOperator(fixfacesize));
  ShapeProcess::RegisterOperator("DropSmallSolids", new ShapeProcess_UOperator(dropsmallsolids));
  ShapeProcess::RegisterOperator("DropSmallEdges", new ShapeProcess_UOperator(mergesmalledges));
  ShapeProcess::RegisterOperator("FixShape", new ShapeProcess_UOperator(fixshape));
  ShapeProcess::RegisterOperator("SplitClosedEdges", new ShapeProcess_UOperator(spltclosededges));
  ShapeProcess::RegisterOperator("SplitCommonVertex",
                                 new ShapeProcess_UOperator(splitcommonvertex));
}
