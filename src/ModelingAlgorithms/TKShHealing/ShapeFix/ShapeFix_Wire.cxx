// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//: j6 abv 7 Dec 98: ProSTEP TR10 r0601_id.stp #57676 & #58586:
//    in FixIntersectingEdges, do not cut edges because of influence on adjacent faces
// pdn 17.12.98: shifting whole wire in FixShifted
//: k3 abv 24 Dec 98: BUC50070 #26682 and #30087: removing self-int loops on pcurves
// pdn 30.12.98: PRO10366 #210: shifting pcurve between two singularities
// pdn 05.01.98: renaming method FixLittle to FixSmall
//: l0 abv 10.01.99: remove unused code
//: n2 abv 22.01.99: ma-test5.igs: IGES read (pref3d): remove degen edge with no pcurve
//: o4 abv 17.02.99: r0301_db.stp #53082: using parameter isClosed in CheckOrder
//: p4 abv 23.02.99: PRO9234 #15720: update UV points of edges after shifting pcurves
// #67 rln 01.03.99 S4135: ims010.igs treatment of Geom_SphericalSurface together with V-closed
// surfaces
//: p7 abv 10.03.99 PRO18206: using method IsDegenerated() to detect singularity in FixLacking
//: r0 abv 19.03.99 PRO7226.stp #489490: remove degenerated edges if several
// #78 rln 12.03.99 S4135: checking spatial closure with Precision
// #79 rln 15.03.99 S4135: bmarkmdl.igs: check for gap before shifting on singularities
//     pdn 17.03.99 S4135: implemented fixing not adjacent intersection
// #86 rln 22.03.99 S4135: repeat of fix self-intersection if it was fixed just before
//%15 pdn 06.04.99 CTS18546-2: fix of self-intersection is repeated while fixed
// #3  smh 01.04.99 S4163: Overflow
// #4  szv          S4163: optimization
//: r7 abv 12.04.99 S4136: FixLk: extend cases of adding degenerated edge
//: r8 abv 12.04.99 PRO10107.stp #2241: try increasing tolerance of edge for fix IE
//: s2 abv 21.04.99 S4136, PRO7978.igs: FixLacking extended to be able to bend pcurves
// szv#9:S4244:19Aug99 Methods FixGaps3d and FixGaps2d were introduced
// #15 smh 03.04.2000 PRO19800. Checking degenerated point on a surface of revolution.
//  sln 25.09.2001  checking order of 3d and 2d representation curves
//  van 19.10.2001  fix of non-adjacent self-intersection corrected to agree with BRepCheck
//  skl 07.03.2002 fix for bug OCC180
//  skl 15.05.2002 for OCC208 (if few edges have reference to
//                 one pcurve we make replace pcurve)
//  PTV 26.06.2002  Remove regressions after fix OCC450

#include <ShapeFix_Wire.hxx>

#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <NCollection_Sequence.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis_TransferParametersProj.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeBuild_Vertex.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_IntersectionTool.hxx>
#include <ShapeFix_SplitTool.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HSequence.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_Wire, ShapeFix_Root)

// S4135
// #######################################################################
//   Constructors, initializations, modes, querying
// #######################################################################
//=================================================================================================

ShapeFix_Wire::ShapeFix_Wire()
    : myMaxTailAngleSine(0),
      myMaxTailWidth(-1)
{
  myFixEdge  = new ShapeFix_Edge;
  myAnalyzer = new ShapeAnalysis_Wire;
  ClearModes();
  ClearStatuses();
  myStatusRemovedSegment = false;
}

//=================================================================================================

ShapeFix_Wire::ShapeFix_Wire(const TopoDS_Wire& wire, const TopoDS_Face& face, const double prec)
    : myMaxTailAngleSine(0),
      myMaxTailWidth(-1)
{
  myFixEdge  = new ShapeFix_Edge;
  myAnalyzer = new ShapeAnalysis_Wire;
  ClearModes();
  SetMaxTolerance(prec);
  myStatusRemovedSegment = false;
  Init(wire, face, prec);
}

//=================================================================================================

void ShapeFix_Wire::SetPrecision(const double prec)
{
  ShapeFix_Root::SetPrecision(prec);
  myAnalyzer->SetPrecision(prec);
}

//=================================================================================================

void ShapeFix_Wire::SetMaxTailAngle(const double theMaxTailAngle)
{
  myMaxTailAngleSine = std::sin(theMaxTailAngle);
  myMaxTailAngleSine = (myMaxTailAngleSine >= 0) ? myMaxTailAngleSine : 0;
}

//=================================================================================================

void ShapeFix_Wire::SetMaxTailWidth(const double theMaxTailWidth)
{
  myMaxTailWidth = theMaxTailWidth;
}

//=================================================================================================

void ShapeFix_Wire::ClearModes()
{
  myTopoMode        = false;
  myGeomMode        = true;
  myClosedMode      = true;
  myPreference2d    = true;
  myFixGapsByRanges = false;

  myRemoveLoopMode = -1;

  myFixReversed2dMode      = -1;
  myFixRemovePCurveMode    = -1;
  myFixRemoveCurve3dMode   = -1;
  myFixAddPCurveMode       = -1;
  myFixAddCurve3dMode      = -1;
  myFixSeamMode            = -1;
  myFixShiftedMode         = -1;
  myFixSameParameterMode   = -1;
  myFixVertexToleranceMode = -1;

  myFixNotchedEdgesMode                 = -1;
  myFixSelfIntersectingEdgeMode         = -1;
  myFixIntersectingEdgesMode            = -1;
  myFixNonAdjacentIntersectingEdgesMode = -1;
  myFixTailMode                         = 0;

  myFixReorderMode          = -1;
  myFixSmallMode            = -1;
  myFixConnectedMode        = -1;
  myFixEdgeCurvesMode       = -1;
  myFixDegeneratedMode      = -1;
  myFixSelfIntersectionMode = -1;
  myFixLackingMode          = -1;
  myFixGaps3dMode           = -1;
  myFixGaps2dMode           = -1;
}

//=================================================================================================

void ShapeFix_Wire::ClearStatuses()
{
  int emptyStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

  myLastFixStatus = emptyStatus;

  myStatusReorder          = emptyStatus;
  myStatusSmall            = emptyStatus;
  myStatusConnected        = emptyStatus;
  myStatusEdgeCurves       = emptyStatus;
  myStatusDegenerated      = emptyStatus;
  myStatusSelfIntersection = emptyStatus;
  myStatusLacking          = emptyStatus;
  myStatusGaps3d           = emptyStatus; // szv#9:S4244:19Aug99 new method introduced
  myStatusGaps2d           = emptyStatus; // szv#9:S4244:19Aug99 new method introduced
  myStatusClosed           = emptyStatus;
  myStatusNotches          = emptyStatus;
  myStatusFixTails         = emptyStatus;
}

//=================================================================================================

void ShapeFix_Wire::Init(const TopoDS_Wire& wire, const TopoDS_Face& face, const double prec)
{
  Load(wire);
  SetFace(face);
  SetPrecision(prec);
}

//=================================================================================================

void ShapeFix_Wire::Init(const occ::handle<ShapeAnalysis_Wire>& saw)
{
  ClearStatuses();
  myAnalyzer = saw;
  myShape.Nullify();
  //  SetPrecision ( saw.Precision() );
}

//=================================================================================================

void ShapeFix_Wire::Load(const TopoDS_Wire& wire)
{
  ClearStatuses();

  TopoDS_Wire W = wire;
  if (!Context().IsNull())
  {
    TopoDS_Shape S = Context()->Apply(wire);
    W              = TopoDS::Wire(S);
  }

  myAnalyzer->Load(W);
  myShape = wire;
}

//=================================================================================================

void ShapeFix_Wire::Load(const occ::handle<ShapeExtend_WireData>& sbwd)
{
  ClearStatuses();
  myAnalyzer->Load(sbwd);
  if (!Context().IsNull())
    UpdateWire();
  myShape.Nullify();
}

//=================================================================================================

int ShapeFix_Wire::NbEdges() const
{
  occ::handle<ShapeExtend_WireData> sbwd = myAnalyzer->WireData();
  return sbwd.IsNull() ? 0 : sbwd->NbEdges();
}

// #######################################################################
//   Fixing methods of API level
// #######################################################################

//=======================================================================
// function : Perform
// purpose  : This method performs all the available fixes.
//           If some fix is turned on or off explicitly by the flag,
//           it is called or not depending on that flag
//           Else (i.e. if flag is default) fix is called depending on the
//           situation: default is True, but some fixes are not called or
//           are limited if order of edges in the wire is not OK
//=======================================================================

bool ShapeFix_Wire::Perform(const Message_ProgressRange& theProgress)
{
  ClearStatuses();
  if (!IsLoaded())
    return false;

  if (!Context().IsNull())
    myFixEdge->SetContext(Context());

  bool Fixed = false;

  // FixReorder is first, because as a rule wire is required to be ordered
  // We shall analyze the order of edges in the wire and set appropriate
  // status even if FixReorder should not be called (if it is forbidden)

  ShapeAnalysis_WireOrder sawo;
  bool                    ReorderOK = (myAnalyzer->CheckOrder(sawo, myClosedMode) == 0);
  if (NeedFix(myFixReorderMode, !ReorderOK))
  {
    if (FixReorder())
      Fixed = true;
    ReorderOK = !StatusReorder(ShapeExtend_FAIL);
  }

  if (theProgress.UserBreak())
    return false;

  // FixSmall is allowed to change topology only if mode is set and FixReorder
  // did not failed
  if (NeedFix(myFixSmallMode, myTopoMode))
  {
    if (FixSmall(!myTopoMode || !ReorderOK, MinTolerance()))
    {
      Fixed = true;
      // retry reorder if necessary (after FixSmall it can work better)
      if (NeedFix(myFixReorderMode, !ReorderOK))
      {
        FixReorder();
        ReorderOK = !StatusReorder(ShapeExtend_FAIL);
      }
    }
  }

  if (theProgress.UserBreak())
    return false;

  if (NeedFix(myFixConnectedMode, ReorderOK))
  {
    if (FixConnected())
      Fixed = true;
  }

  if (theProgress.UserBreak())
    return false;

  if (NeedFix(myFixEdgeCurvesMode))
  {
    int savFixShiftedMode = myFixShiftedMode;
    // turn out FixShifted if reorder not done
    if (myFixShiftedMode == -1 && !ReorderOK)
      myFixShiftedMode = 0;
    if (FixEdgeCurves())
      Fixed = true;
    myFixShiftedMode = savFixShiftedMode;
  }

  if (theProgress.UserBreak())
    return false;

  if (NeedFix(myFixDegeneratedMode))
  {
    if (FixDegenerated())
      Fixed = true; // ?? if ! ReorderOK ??
  }

  if (theProgress.UserBreak())
    return false;

  // pdn - temporary to test
  if (myFixTailMode <= 0 && NeedFix(myFixNotchedEdgesMode, ReorderOK))
  {
    Fixed |= FixNotchedEdges();
    if (Fixed)
      FixShifted(); // skl 07.03.2002 for OCC180
  }

  if (theProgress.UserBreak())
    return false;

  if (myFixTailMode != 0)
  {
    if (FixTails())
    {
      Fixed = true;
      FixShifted();
    }
  }

  if (theProgress.UserBreak())
    return false;

  if (NeedFix(myFixSelfIntersectionMode, myClosedMode))
  {
    int savFixIntersectingEdgesMode = myFixIntersectingEdgesMode;
    // switch off FixIntEdges if reorder not done
    if (myFixIntersectingEdgesMode == -1 && !ReorderOK)
      myFixIntersectingEdgesMode = 0;
    if (FixSelfIntersection())
      Fixed = true;
    FixReorder();
    myFixIntersectingEdgesMode = savFixIntersectingEdgesMode;
  }

  if (theProgress.UserBreak())
    return false;

  if (NeedFix(myFixLackingMode, ReorderOK))
  {
    if (FixLacking())
      Fixed = true;
  }

  if (theProgress.UserBreak())
    return false;

  // TEMPORARILY without special mode !!!
  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  for (int iedge = 1; iedge <= sbwd->NbEdges(); iedge++)
  {
    if (myFixEdge->FixVertexTolerance(sbwd->Edge(iedge), Face()))
    {
      Fixed = true;
    }
  }

  if (theProgress.UserBreak())
    return false;

  if (!Context().IsNull())
    UpdateWire();

  return Fixed;
}

//=================================================================================================

bool ShapeFix_Wire::FixReorder(bool theModeBoth)
{
  myStatusReorder = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded())
  {
    return false;
  }

  // fix in Both mode for bi-periodic surface
  ShapeAnalysis_WireOrder sawo;
  if (!myAnalyzer->Surface().IsNull() && myAnalyzer->Surface()->Surface()->IsUPeriodic()
      && myAnalyzer->Surface()->Surface()->IsVPeriodic() && theModeBoth)
  {
    myAnalyzer->CheckOrder(sawo, myClosedMode, true, true);
  }
  else
  {
    myAnalyzer->CheckOrder(sawo, myClosedMode, true, false);
  }

  FixReorder(sawo);

  if (LastFixStatus(ShapeExtend_FAIL))
  {
    myStatusReorder |= ShapeExtend::EncodeStatus(
      LastFixStatus(ShapeExtend_FAIL1) ? ShapeExtend_FAIL1 : ShapeExtend_FAIL2);
  }
  if (!LastFixStatus(ShapeExtend_DONE))
  {
    return false;
  }

  myStatusReorder |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  if (sawo.Status() == 2 || sawo.Status() == -2)
  {
    myStatusReorder |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
  }
  if (sawo.Status() < 0)
  {
    myStatusReorder |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
  }
  if (sawo.Status() == 3)
  {
    // only shifted
    myStatusReorder |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
  }
  return true;
}

//=================================================================================================

int ShapeFix_Wire::FixSmall(const bool lockvtx, const double precsmall)
{
  myStatusSmall = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded())
    return false;

  for (int i = NbEdges(); i > 0; i--)
  {
    FixSmall(i, lockvtx, precsmall);
    myStatusSmall |= myLastFixStatus;
  }

  return StatusSmall(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeFix_Wire::FixConnected(const double prec)
{
  myStatusConnected = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded())
    return false;

  int stop = (myClosedMode ? 0 : 1);
  for (int i = NbEdges(); i > stop; i--)
  {
    // Call without UpdateWire to avoid O(n^2) behavior in the loop
    FixConnected(i, prec, false);
    myStatusConnected |= myLastFixStatus;
  }

  // Update wire once after all connections are fixed
  // Using Value() in UpdateWire() prevents edge explosion from replacement chains
  if (!Context().IsNull())
    UpdateWire();

  return StatusConnected(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeFix_Wire::FixEdgeCurves()
{
  myStatusEdgeCurves = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded())
    return false;
  bool isReady = IsReady();

  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  int                               i, nb = sbwd->NbEdges();
  TopoDS_Face                       face          = Face();
  occ::handle<ShapeFix_Edge>        theAdvFixEdge = myFixEdge;
  if (theAdvFixEdge.IsNull())
    myFixReversed2dMode = false;

  // fix revesred 2d / 3d curves
  if (isReady && NeedFix(myFixReversed2dMode))
  {
    for (i = 1; i <= nb; i++)
    {
      theAdvFixEdge->FixReversed2d(sbwd->Edge(i), face);
      if (theAdvFixEdge->Status(ShapeExtend_DONE))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      if (theAdvFixEdge->Status(ShapeExtend_FAIL))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    }
  }

  // add / remove pcurve
  if (isReady && NeedFix(myFixRemovePCurveMode, false))
  {
    for (i = 1; i <= nb; i++)
    {
      myFixEdge->FixRemovePCurve(sbwd->Edge(i), face);
      if (myFixEdge->Status(ShapeExtend_DONE))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
      if (myFixEdge->Status(ShapeExtend_FAIL))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    }
  }

  if (isReady && NeedFix(myFixAddPCurveMode))
  {
    int overdegen = 0; //: c0
    for (i = 1; i <= nb; i++)
    {
      myFixEdge->FixAddPCurve(sbwd->Edge(i),
                              face,
                              sbwd->IsSeam(i),
                              myAnalyzer->Surface(),
                              Precision());
      if (myFixEdge->Status(ShapeExtend_DONE))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
      if (myFixEdge->Status(ShapeExtend_FAIL))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);

      // if ( !sbwd->IsSeam(i) && myFixEdge->Status ( ShapeExtend_DONE2 )
      //      && BRep_Tool::SameParameter(sbwd->Edge(i)) ) {
      if (!sbwd->IsSeam(i) && myFixEdge->Status(ShapeExtend_DONE2))
      {
        // abv 24 Feb 00: trj3_s1-ac-214.stp #1631 etc.: try to split the edge in singularity
        if (!Context().IsNull())
        {
          ShapeBuild_Edge                    sbe;
          TopoDS_Edge                        E = sbwd->Edge(i);
          ShapeAnalysis_Curve                SAC;
          double                             a, b;
          occ::handle<Geom_Curve>            C   = BRep_Tool::Curve(E, a, b);
          occ::handle<ShapeAnalysis_Surface> S   = myAnalyzer->Surface();
          int                                nbs = S->NbSingularities(MinTolerance());
          GeomAdaptor_Curve                  GAC(C, a, b);
          NCollection_Sequence<double>       seq;
          for (int j = 1; j <= nbs; j++)
          {
            double   Preci;
            gp_Pnt2d pd1, pd2;
            gp_Pnt   P3d, pr;
            double   par1, par2, split;
            bool     tmpUIsoDeg;
            S->Singularity(j, Preci, P3d, pd1, pd2, par1, par2, tmpUIsoDeg);
            if (SAC.Project(GAC, P3d, MinTolerance(), pr, split, true)
                < std::max(Preci, MinTolerance()))
            {
              if (split - a > ::Precision::PConfusion() && b - split > ::Precision::PConfusion())
              {
                int k;
                for (k = 1; k <= seq.Length(); k++)
                {
                  if (split < seq(k) - ::Precision::PConfusion())
                  {
                    seq.InsertBefore(k, split);
                    break;
                  }
                  else if (split < seq(k) + ::Precision::PConfusion())
                    break;
                }
                if (k > seq.Length())
                  seq.Append(split);
              }
            }
          }
          if (seq.Length() > 0)
          { // supposed that edge is SP
#ifdef OCCT_DEBUG
            std::cout << "Edge going over singularity detected; split" << std::endl;
#endif
            bool isFwd = (E.Orientation() == TopAbs_FORWARD);
            E.Orientation(TopAbs_FORWARD);

            // if( BRep_Tool::SameParameter(sbwd->Edge(i)) )
            //   sbe.RemovePCurve ( E, face );

            // 10.04.2003 skl for using trimmed lines as pcurves
            ShapeAnalysis_Edge sae;
            if (BRep_Tool::SameParameter(sbwd->Edge(i)))
              sbe.RemovePCurve(E, face);
            else
            {
              if (sae.HasPCurve(E, face))
              {
                occ::handle<Geom2d_Curve> C2d;
                double                    fp2d, lp2d;
                if (sae.PCurve(E, face, C2d, fp2d, lp2d))
                {
                  if (!C2d->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
                    sbe.RemovePCurve(E, face);
                }
              }
            }

            //    myFixEdge->FixSameParameter ( E ); // to ensure SameRange & SP
            BRep_Builder  B;
            TopoDS_Vertex V1, V2, V;
            // ShapeAnalysis_Edge sae;
            V1 = sae.FirstVertex(E);
            V2 = sae.LastVertex(E);

            occ::handle<ShapeExtend_WireData> sw = new ShapeExtend_WireData;
            for (int k = 0; k <= seq.Length(); k++)
            {
              double split = (k < seq.Length() ? seq(k + 1) : b);
              if (k < seq.Length())
              {
                B.MakeVertex(V, C->Value(split), BRep_Tool::Tolerance(E));
                // try increase tolerance before splitting
                double aDist = BRep_Tool::Pnt(V1).Distance(BRep_Tool::Pnt(V));
                if (aDist < BRep_Tool::Tolerance(V1) * 1.01)
                {
                  B.UpdateVertex(V1, std::max(aDist, BRep_Tool::Tolerance(V1)));
                  a  = split;
                  V1 = V;
                  continue;
                }
                else
                {
                  aDist = BRep_Tool::Pnt(V2).Distance(BRep_Tool::Pnt(V));
                  if (aDist < BRep_Tool::Tolerance(V2) * 1.01)
                  {
                    B.UpdateVertex(V, std::max(aDist, BRep_Tool::Tolerance(V2)));
                    b  = split;
                    V2 = V;
                    continue;
                  }
                }
              }
              else
              {
                V = V2;
              }

              TopoDS_Edge edge = sbe.CopyReplaceVertices(E, V1, V);
              if (BRep_Tool::SameParameter(sbwd->Edge(i)))
              {
                // TopoDS_Edge edge = sbe.CopyReplaceVertices ( E, V1, V );
                B.Range(edge, a, split);
                sw->Add(edge);
              }
              else
              {
                // TopoDS_Edge edge = sbe.CopyReplaceVertices(sbwd->Edge(i),V1,V);
                occ::handle<ShapeAnalysis_TransferParameters> sftp =
                  new ShapeAnalysis_TransferParameters(E, face);
                sftp->TransferRange(edge, a, split, false);
                sw->Add(edge);
              }
              // sw->Add(edge);
              a  = split;
              V1 = V;
            }
            if (!isFwd)
            {
              sw->Reverse();
              E.Orientation(TopAbs_REVERSED);
            }
            Context()->Replace(E, sw->Wire());
            UpdateWire();
            nb = sbwd->NbEdges();
            i--;
            continue;
          }
        }

        overdegen = i;
      }
    }

    //: c0 abv 20 Feb 98: treat case of curve going over degenerated pole and seam
    if (overdegen && myAnalyzer->Surface()->IsUClosed(Precision()))
    {
      ShapeBuild_Edge sbe;
      double          URange, SUF, SUL, SVF, SVL;
      myAnalyzer->Surface()->Bounds(SUF, SUL, SVF, SVL);
      URange = (std::abs(SUL - SUF));
      gp_XY              vec(0, 0);
      ShapeAnalysis_Edge sae;
      int                k;
      for (k = 1; k <= nb; k++)
      {
        double                    cf, cl;
        occ::handle<Geom2d_Curve> c2d;
        if (!sae.PCurve(sbwd->Edge(k), face, c2d, cf, cl, true))
          break;
        vec += c2d->Value(cl).XY() - c2d->Value(cf).XY();
      }
      if (k > nb && std::abs(std::abs(vec.X()) - URange) < 0.1 * URange)
      {
        sbe.RemovePCurve(sbwd->Edge(overdegen), face);
        myFixEdge->Projector()->AdjustOverDegenMode() = false;
        myFixEdge->FixAddPCurve(sbwd->Edge(overdegen),
                                face,
                                sbwd->IsSeam(overdegen),
                                myAnalyzer->Surface(),
                                Precision());
      }
#ifdef OCCT_DEBUG
      std::cout << "Edge going over singularity detected; pcurve adjusted" << std::endl;
#endif
    }
  }

  // add / remove pcurve
  if (isReady && NeedFix(myFixRemoveCurve3dMode, false))
  {
    for (i = 1; i <= nb; i++)
    {
      myFixEdge->FixRemoveCurve3d(sbwd->Edge(i));
      if (myFixEdge->Status(ShapeExtend_DONE))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
      if (myFixEdge->Status(ShapeExtend_FAIL))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL4);
    }
  }
  if (NeedFix(myFixAddCurve3dMode))
  {
    for (i = 1; i <= nb; i++)
    {
      myFixEdge->FixAddCurve3d(sbwd->Edge(i));
      if (myFixEdge->Status(ShapeExtend_DONE))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
      if (myFixEdge->Status(ShapeExtend_FAIL))
      {
        //: abv 29.08.01: Spatial_firex_lofting.sat: if 3d curve cannot
        // be built because edge has no pcurves either, remove that edge
        occ::handle<Geom2d_Curve> C;
        occ::handle<Geom_Surface> S;
        TopLoc_Location           L;
        double                    first = 0., last = 0.;
        BRep_Tool::CurveOnSurface(sbwd->Edge(i), C, S, L, first, last);
        if (C.IsNull() || std::abs(last - first) < Precision::PConfusion())
        {
          // clang-format off
          SendWarning ( sbwd->Edge ( i ), Message_Msg ( "FixWire.FixCurve3d.Removed" ) );// Incomplete edge (with no pcurves or 3d curve) removed
          // clang-format on
          sbwd->Remove(i--);
          nb--;
          myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
          if (i == nb)
          {
            FixClosed(Precision());
          }
          else
          {
            FixConnected(i + 1, Precision());
          }
        }
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL5);
      }
    }
  }

  // fix seam
  if (isReady && NeedFix(myFixSeamMode, false))
  {
    for (i = 1; i <= nb; i++)
    {
      FixSeam(i);
      if (LastFixStatus(ShapeExtend_DONE))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE6);
      if (LastFixStatus(ShapeExtend_FAIL))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL6);
    }
  }

  // fix shifted
  if (isReady && NeedFix(myFixShiftedMode))
  {
    FixShifted();
    if (LastFixStatus(ShapeExtend_DONE))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE7);
    if (LastFixStatus(ShapeExtend_FAIL))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL7);
  }

  // fix same parameter
  if (isReady && NeedFix(myFixSameParameterMode))
  {
    for (i = 1; i <= nb; i++)
    {
      // skl 28.10.2004 for OCC6366 - check SameRange
      ShapeAnalysis_Edge      sae;
      double                  First, Last;
      occ::handle<Geom_Curve> tmpc3d = BRep_Tool::Curve(sbwd->Edge(i), First, Last);
      if (sae.HasPCurve(sbwd->Edge(i), face))
      {
        occ::handle<Geom2d_Curve> C2d;
        double                    fp2d, lp2d;
        if (sae.PCurve(sbwd->Edge(i), face, C2d, fp2d, lp2d, false))
        {
          if (fabs(First - fp2d) > Precision::PConfusion()
              || fabs(Last - lp2d) > Precision::PConfusion())
          {
            BRep_Builder B;
            B.SameRange(sbwd->Edge(i), false);
          }
          else if (!sae.CheckPCurveRange(First, Last, C2d))
          {
            // Replace pcurve
            TopLoc_Location                  L;
            const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(face, L);
            ShapeBuild_Edge().RemovePCurve(sbwd->Edge(i), S, L);
            myFixEdge->FixAddPCurve(sbwd->Edge(i),
                                    face,
                                    sbwd->IsSeam(i),
                                    myAnalyzer->Surface(),
                                    Precision());
            if (myFixEdge->Status(ShapeExtend_DONE))
              myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
            if (myFixEdge->Status(ShapeExtend_FAIL))
              myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
          }
        }
      }
      myFixEdge->FixSameParameter(sbwd->Edge(i), Face());
      if (myFixEdge->Status(ShapeExtend_DONE))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE8);
      if (myFixEdge->Status(ShapeExtend_FAIL))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL8);
    }
  }

  //: abv 10.06.02: porting C40 -> dev (CC670-12608.stp): moved from Perform()
  // Update with face is needed for plane surfaces (w/o stored pcurves)
  if (NeedFix(myFixVertexToleranceMode))
  {
    for (i = 1; i <= nb; i++)
    {
      myFixEdge->FixVertexTolerance(sbwd->Edge(i), Face());
      if (myFixEdge->Status(ShapeExtend_DONE))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE8);
      if (myFixEdge->Status(ShapeExtend_FAIL))
        myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL8);
    }
    if (!Context().IsNull())
      UpdateWire();
  }

  return StatusEdgeCurves(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeFix_Wire::FixDegenerated()
{
  myStatusDegenerated = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  //  if ( ! myAnalyzer->Surface()->HasSingularities ( Precision() ) ) return;

  int lastcoded = -1, prevcoded = 0;
  int stop = (myClosedMode ? 0 : 1);
  for (int i = NbEdges(); i > stop; i--)
  {
    FixDegenerated(i);
    myStatusDegenerated |= myLastFixStatus;
    //: r0 abv 19 Mar 99: PRO7226.stp #489490: remove duplicated degenerated edges
    int coded = (LastFixStatus(ShapeExtend_DONE2) ? 1 : 0);
    if (lastcoded == -1)
      lastcoded = coded;
    if (coded && (prevcoded || (i == 1 && lastcoded)) && NbEdges() > 1)
    {
      occ::handle<ShapeExtend_WireData> sbwd = WireData();
      BRep_Builder                      B;
      sbwd->Remove(i);
      if (!prevcoded)
        i = NbEdges();
      B.Degenerated(sbwd->Edge(i++), false);
      prevcoded = 0;
    }
    else
      prevcoded = coded;
  }

  return StatusDegenerated(ShapeExtend_DONE);
}

//=======================================================================
// function : FixSelfIntersection
// purpose  : Applies FixSelfIntersectingEdge and FixIntersectingEdges
//           and removes wrong edges
//=======================================================================

bool ShapeFix_Wire::FixSelfIntersection()
{
  myStatusSelfIntersection = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  int                               nb   = sbwd->NbEdges();

  if (NeedFix(myFixSelfIntersectingEdgeMode))
  {
    if (myRemoveLoopMode < 1)
      for (int num = 1; num <= nb; num++)
      {
        FixSelfIntersectingEdge(num);
        myStatusSelfIntersection |= myLastFixStatus;
      }
    else if (myRemoveLoopMode == 1)
    {
      for (int num = 1; num <= nb; num++)
      {
        FixSelfIntersectingEdge(num);
        myStatusSelfIntersection |= myLastFixStatus;
        if (nb < sbwd->NbEdges())
          num--;
        nb = sbwd->NbEdges();
      }
      FixClosed(Precision());
    }
  }

  if (NeedFix(myFixIntersectingEdgesMode))
  {
    int num = (myClosedMode ? 1 : 2);
    for (; nb > 1 && num <= nb; num++)
    {
      FixIntersectingEdges(num);
      if (LastFixStatus(ShapeExtend_FAIL1))
        myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
      if (LastFixStatus(ShapeExtend_FAIL2))
        myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      if (!LastFixStatus(ShapeExtend_DONE))
        continue;

      if (LastFixStatus(ShapeExtend_DONE1))
        myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      if (LastFixStatus(ShapeExtend_DONE2))
        myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
      if (LastFixStatus(ShapeExtend_DONE6))
        myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_DONE6);

      if (/*! myTopoMode ||*/ nb < 3)
      {
        // #86 rln 22.03.99 sim2.igs, entity 4292: After fixing of self-intersecting
        // BRepCheck finds one more self-intersection not found by ShapeAnalysis
        //%15 pdn 06.04.99 repeat until fixed CTS18546-2 entity 777

        // if the tolerance was modified we should recheck the result, if it was enough
        if (LastFixStatus(ShapeExtend_DONE7)) // num--;
          FixIntersectingEdges(num);
        continue;
      }

      if (LastFixStatus(ShapeExtend_DONE4))
        sbwd->Remove(num);
      if (LastFixStatus(ShapeExtend_DONE3))
        sbwd->Remove(num > 1 ? num - 1 : nb + num - 1);
      if (LastFixStatus(ShapeExtend_DONE4) || LastFixStatus(ShapeExtend_DONE3))
      {
        myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
        num = (myClosedMode ? 1 : 2);
        nb  = sbwd->NbEdges();
#ifdef OCCT_DEBUG
        std::cout << "Warning: ShapeFix_Wire::FixSelfIntersection: Edge removed" << std::endl;
#endif
      }
      else
      {
        // #86 rln 22.03.99
        //%15 pdn 06.04.99 repeat until fixed CTS18546-2 entity 777
        FixIntersectingEdges(num);
        /*if ( LastFixStatus ( ShapeExtend_DONE7 ) )*/
        // Always revisit the fixed edge
        // num--;
      }
    }
    if (!Context().IsNull())
      UpdateWire();
  }

  // pdn 17.03.99 S4135 to avoid regression fixing not adjacent intersection
  if (NeedFix(myFixNonAdjacentIntersectingEdgesMode))
  {

    ShapeFix_IntersectionTool ITool(Context(), Precision());
    int                       NbSplit = 0, NbCut = 0, NbRemoved = 0;
    if (ITool.FixSelfIntersectWire(sbwd, myAnalyzer->Face(), NbSplit, NbCut, NbRemoved))
    {
      myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5); // gka 06.09.04
    }
    if (NbSplit > 0 || NbRemoved > 0)
    {
      if (NbRemoved > 0)
        myStatusRemovedSegment = true;
      // Load(sbwd); commented by skl 29.12.2004 for OCC7624, instead this
      //             string inserted following three strings:
      myAnalyzer->Load(sbwd);
      if (!Context().IsNull())
        UpdateWire();
      myShape.Nullify();
    }
#ifdef OCCT_DEBUG
    if (StatusSelfIntersection(ShapeExtend_DONE5))
      std::cout
        << "Warning: ShapeFix_Wire::FixIntersection: Non-adjacent intersection fixed (split-"
        << NbSplit << ", cut-" << NbCut << ", removed-" << NbRemoved << ")" << std::endl;
#endif

    /*
        NCollection_Array1<Bnd_Box2d> boxes(1,nb);
        TopLoc_Location L;
        const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(Face(), L);
        occ::handle<Geom2d_Curve> c2d;
        double cf,cl;
        ShapeAnalysis_Edge sae;
        for(int i = 1; i <= nb; i++){
          TopoDS_Edge E = sbwd->Edge (i);
          if(sae.PCurve (E,S,L,c2d,cf,cl,false)) {
        Bnd_Box2d box;
            Geom2dAdaptor_Curve gac;
            double aFirst = c2d->FirstParameter();
            double aLast = c2d->LastParameter();
            if(c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))
               && (cf < aFirst || cl > aLast)) {
              //pdn avoiding problems with segment in Bnd_Box
              gac.Load(c2d);
            }
            else
              gac.Load(c2d,cf,cl);
        BndLib_Add2dCurve::Add(gac,::Precision::Confusion(),box);
        boxes(i) = box;
          }
        }

        bool isFail = false, isDone = false;
        for(int num1 = 1; num1 < nb-1; num1++) {
          int fin = (num1 == 1 ? nb-1 : nb);
          for(int num2 = num1+2; num2 <= fin; num2++)
        if(!boxes(num1).IsOut(boxes(num2))){
          FixIntersectingEdges (num1, num2);
          isFail |= LastFixStatus ( ShapeExtend_FAIL1 );
          isDone |= LastFixStatus ( ShapeExtend_DONE1 );
        }
        }

        if(isFail)
          myStatusSelfIntersection |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL3 );
        if(isDone)
          myStatusSelfIntersection |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE5 );
    #ifdef OCCT_DEBUG
        if (StatusSelfIntersection (ShapeExtend_DONE5))
          std::cout << "Warning: ShapeFix_Wire::FixSelfIntersection: Non adjacent intersection
    fixed" << std::endl; #endif
    */
  }

  return StatusSelfIntersection(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeFix_Wire::FixLacking(const bool force)
{
  myStatusLacking = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  int start = (myClosedMode ? 1 : 2);
  for (int i = start; i <= NbEdges(); i++)
  {
    FixLacking(i, force);
    myStatusLacking |= myLastFixStatus;
  }

  return StatusLacking(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeFix_Wire::FixClosed(const double prec)
{
  myStatusClosed = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded() || NbEdges() < 1)
    return false;

  FixConnected(1, prec);
  if (LastFixStatus(ShapeExtend_DONE))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  if (LastFixStatus(ShapeExtend_FAIL))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);

  FixDegenerated(1);
  if (LastFixStatus(ShapeExtend_DONE))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
  if (LastFixStatus(ShapeExtend_FAIL))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);

  FixLacking(1);
  if (LastFixStatus(ShapeExtend_DONE))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
  if (LastFixStatus(ShapeExtend_FAIL))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);

  return StatusClosed(ShapeExtend_DONE);
}

// #######################################################################
//   Fixing methods of advanced level
// #######################################################################

//=================================================================================================

bool ShapeFix_Wire::FixReorder(const ShapeAnalysis_WireOrder& wi)
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded())
    return false;

  int status = wi.Status();
  if (status == 0)
    return false;
  if (status <= -10)
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }

  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  int                               i, nb = sbwd->NbEdges();
  if (nb != wi.NbEdges())
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }
  // D abord on protege
  for (i = 1; i <= nb; i++)
  {
    if (wi.Ordered(i) == 0)
    {
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
      return false;
    }
  }

  occ::handle<NCollection_HSequence<TopoDS_Shape>> newedges =
    new NCollection_HSequence<TopoDS_Shape>();
  for (i = 1; i <= nb; i++)
    newedges->Append(sbwd->Edge(wi.Ordered(i)));
  for (i = 1; i <= nb; i++)
    sbwd->Set(TopoDS::Edge(newedges->Value(i)), i);

  myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  return true;
}

//=================================================================================================

bool ShapeFix_Wire::FixSmall(const int num, const bool lockvtx, const double precsmall)
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded() || NbEdges() <= 1)
    return false;

  // analysis:
  occ::handle<ShapeAnalysis_Wire> theAdvAnalyzer = myAnalyzer;
  if (theAdvAnalyzer.IsNull())
    return false;
  int n = (num > 0 ? num : NbEdges());
  theAdvAnalyzer->CheckSmall(n, precsmall);
  if (theAdvAnalyzer->LastCheckStatus(ShapeExtend_FAIL))
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    //: n2    return false;
  }

  if (!theAdvAnalyzer->LastCheckStatus(ShapeExtend_DONE))
    return false;

  // OUI cette edge est NULLE

  if (theAdvAnalyzer->LastCheckStatus(ShapeExtend_DONE2))
  {
    // edge is small, but vertices are not the same..
    if (lockvtx || !myTopoMode)
    {
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      return false;
    }
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
  }
  else
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);

  // action: remove edge
  if (!Context().IsNull())
    Context()->Remove(WireData()->Edge(n));
  // clang-format off
  SendWarning ( WireData()->Edge ( n ), Message_Msg ( "FixAdvWire.FixSmall.MSG0" ) ); //Small edge(s) removed
  // clang-format on
  WireData()->Remove(n);

  // call FixConnected in the case if vertices of the small edge were not the same
  if (LastFixStatus(ShapeExtend_DONE2))
  {
    int savLastFixStatus = myLastFixStatus;
    // #43 rln 20.11.98 S4054 CTS18544 entity 21734 removing last edge
    FixConnected(n <= NbEdges() ? n : 1, precsmall);
    if (LastFixStatus(ShapeExtend_FAIL))
      savLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    myLastFixStatus = savLastFixStatus;
  }

  return true;
}

//=================================================================================================

bool ShapeFix_Wire::FixConnected(const int num, const double prec, const bool theUpdateWire)
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded() || NbEdges() <= 0)
    return false;

  // analysis
  myAnalyzer->CheckConnected(num, prec < 0 ? MaxTolerance() : prec);
  if (myAnalyzer->LastCheckStatus(ShapeExtend_FAIL))
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  }
  if (!myAnalyzer->LastCheckStatus(ShapeExtend_DONE))
    return false;

  // action: replacing vertex
  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  int                               n2   = (num > 0 ? num : sbwd->NbEdges());
  int                               n1   = (n2 > 1 ? n2 - 1 : sbwd->NbEdges());
  TopoDS_Edge                       E1   = sbwd->Edge(n1);
  TopoDS_Edge                       E2   = sbwd->Edge(n2);

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      V1 = sae.LastVertex(E1);
  TopoDS_Vertex      V2 = sae.FirstVertex(E2);
  TopoDS_Vertex      V;

  if (myAnalyzer->LastCheckStatus(ShapeExtend_DONE1))
  { // absolutely confused
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    // #40 rln 18.11.98 S4054 BUC60035 entity 2393 (2-nd sub-curve is edge with the same vertex)
    if (V2.IsSame(sae.LastVertex(E2)))
    {
      V = V2;
      if (!Context().IsNull())
        Context()->Replace(V1, V.Oriented(V1.Orientation()));
    }
    else
    {
      V = V1;
      if (!Context().IsNull())
        Context()->Replace(V2, V.Oriented(V2.Orientation()));
    }
  }
  else
  { // on moyenne ...
    if (myAnalyzer->LastCheckStatus(ShapeExtend_DONE2))
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    else
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
    ShapeBuild_Vertex sbv;
    V = sbv.CombineVertex(V1, V2, 1.0001);
    if (!Context().IsNull())
    {
      Context()->Replace(V1, V.Oriented(V1.Orientation()));
      Context()->Replace(V2, V.Oriented(V2.Orientation()));
    }
  }

  // replace vertices to a new one
  ShapeBuild_Edge sbe;
  if (sbwd->NbEdges() < 2)
  {
    if (E2.Free() && myTopoMode)
    {
      BRep_Builder B;
      B.Remove(E2, sae.FirstVertex(E2));
      B.Remove(E2, sae.LastVertex(E2));
      B.Add(E2, V.Oriented(TopAbs_FORWARD));
      B.Add(E2, V.Oriented(TopAbs_REVERSED));
    }
    else
    {
      TopoDS_Edge tmpE = sbe.CopyReplaceVertices(E2, V, V);
      sbwd->Set(tmpE, n2);
      if (!Context().IsNull())
        Context()->Replace(E2, tmpE);
    }
  }
  else
  {
    if (E2.Free() && E1.Free() && myTopoMode)
    {
      BRep_Builder B;
      B.Remove(E2, sae.FirstVertex(E2));
      B.Add(E2, V.Oriented(TopAbs_FORWARD));
      if (!myAnalyzer->LastCheckStatus(ShapeExtend_DONE1)
          || sae.FirstVertex(E2).IsSame(sae.LastVertex(E2)))
      {
        B.Remove(E1, sae.LastVertex(E1));
        B.Add(E1, V.Oriented(TopAbs_REVERSED));
      }
    }
    else
    {
      TopoDS_Edge tmpE2 = sbe.CopyReplaceVertices(E2, V, TopoDS_Vertex());
      sbwd->Set(tmpE2, n2);
      if (!Context().IsNull())
        Context()->Replace(E2, tmpE2);
      if (!myAnalyzer->LastCheckStatus(ShapeExtend_DONE1)
          || sae.FirstVertex(E2).IsSame(sae.LastVertex(E2)))
      {
        TopoDS_Edge tmpE1 = sbe.CopyReplaceVertices(E1, TopoDS_Vertex(), V);
        sbwd->Set(tmpE1, n1);
        if (!Context().IsNull())
          Context()->Replace(E1, tmpE1);
      }
    }
  }

  // Optionally update wire data with context replacements
  if (theUpdateWire && !Context().IsNull())
    UpdateWire();

  return true;
}

//=================================================================================================

bool ShapeFix_Wire::FixSeam(const int num)
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  occ::handle<Geom2d_Curve> C1, C2;
  double                    cf, cl;
  if (!myAnalyzer->CheckSeam(num, C1, C2, cf, cl))
    return false;

  BRep_Builder B;
  TopoDS_Edge  E = WireData()->Edge(num > 0 ? num : NbEdges());
  B.UpdateEdge(E, C2, C1, Face(), 0.); //: S4136: BRep_Tool::Tolerance(E)
  B.Range(E, Face(), cf, cl);
  myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);

  return true;
}

//=======================================================================
// function : FixShifted
// purpose  : fix parametric curves which may be shifted
// to whole parametric range of closed surface as result of recomputing
// from 3d representation.
// This can be a curve on a seam or near it.
// This function is to be called before FixDegenerated.
// LIMITATION: this function cannot fix cases when, e.g., closed wire is
// composed of two meridians of the sphere and one of them is seam.
// NOTE: wire is supposed to be closed and sorted !
//=======================================================================

//: p4 abv 23.02.99: PRO9234 #15720: update UV points of edge
static void UpdateEdgeUVPoints(TopoDS_Edge& E, const TopoDS_Face& F)
{
  double first, last;
  BRep_Tool::Range(E, F, first, last);
  BRep_Builder().Range(E, F, first, last);
}

//=================================================================================================

bool ShapeFix_Wire::FixShifted()
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  occ::handle<ShapeAnalysis_Surface> surf = myAnalyzer->Surface();
  // #78 rln 12.03.99 S4135: checking spatial closure with Precision
  bool uclosed = surf->IsUClosed(Precision());
  bool vclosed =
    surf->IsVClosed(Precision()) || surf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface));
  // #67 rln 01.03.99 S4135: ims010.igs entity D11900 (2D contour is 2*PI higher than V range
  // [-pi/2,p/2])

  // PTV 26.06.2002 begin
  // CTS18546-2.igs entity 2222: base curve is periodic and 2dcurve is shifted
  bool   IsVCrvClosed = false;
  double VRange       = 1.;
  if (surf->Surface()->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    occ::handle<Geom_SurfaceOfRevolution> aSurOfRev =
      occ::down_cast<Geom_SurfaceOfRevolution>(surf->Surface());
    occ::handle<Geom_Curve> aBaseCrv = aSurOfRev->BasisCurve();
    while ((aBaseCrv->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
           || (aBaseCrv->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))))
    {
      if (aBaseCrv->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
        aBaseCrv = occ::down_cast<Geom_OffsetCurve>(aBaseCrv)->BasisCurve();
      if (aBaseCrv->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
        aBaseCrv = occ::down_cast<Geom_TrimmedCurve>(aBaseCrv)->BasisCurve();
    }
    if (aBaseCrv->IsPeriodic())
    {
      vclosed      = true;
      VRange       = aBaseCrv->Period();
      IsVCrvClosed = true;
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeFix_Wire::FixShifted set vclosed True for Surface of Revolution"
                << std::endl;
#endif
    }
  }
  // PTV 26.06.2002 end
  if (!uclosed && !vclosed)
    return false;

  double URange, /*VRange,*/ SUF, SUL, SVF, SVL;
  surf->Surface()->Bounds(SUF, SUL, SVF, SVL);
  double SUMid, SVMid;
  SUMid = 0.5 * (SUF + SUL);
  SVMid = 0.5 * (SVF + SVL);
  if (uclosed)
    URange = std::abs(SUL - SUF);
  else
    URange = RealLast();
  if (!IsVCrvClosed)
  {
    if (vclosed)
      VRange = std::abs(SVL - SVF);
    else
      VRange = RealLast();
  }
  double UTol = 0.2 * URange, VTol = 0.2 * VRange;

  occ::handle<ShapeExtend_WireData> sbwdOring = WireData();
  ShapeAnalysis_Edge                sae;
  occ::handle<ShapeExtend_WireData> sbwd = new ShapeExtend_WireData;
  for (int i = 1; i <= sbwdOring->NbEdges(); i++)
  {
    TopoDS_Edge E1 = sbwdOring->Edge(i);
    if (BRep_Tool::Degenerated(E1) && !sae.HasPCurve(E1, Face()))
      continue;

    sbwd->Add(E1);
  }

  ShapeBuild_Edge sbe;
  int             nb  = sbwd->NbEdges();
  bool            end = (nb == 0), degstop = false;
  int             stop  = nb;
  int             degn2 = 0;
  gp_Pnt          pdeg;
  // pdn 17.12.98 r0901_ec 38237 to shift wire at 0

  // GeomAdaptor_Surface& GAS = myAnalyzer->Surface()->Adaptor3d()->ChangeSurface(); //SK
  Bnd_Box2d box;
  for (int n2 = 1, n1 = nb; !end; n1 = n2++)
  {
    if (n2 > nb)
      n2 = 1;
    if (n2 == stop)
      end = true;

    TopoDS_Edge E1 = sbwd->Edge(n1);
    TopoDS_Edge E2 = sbwd->Edge(n2);

    if (BRep_Tool::Degenerated(E1) || BRep_Tool::Degenerated(E2))
    {
      if (!degstop)
      {
        stop    = n2;
        degstop = true;
      }
      continue;
    }

    TopoDS_Vertex V = sae.FirstVertex(E2);
    if (V.IsNull())
      continue;

    gp_Pnt p = BRep_Tool::Pnt(V);

    double                    a1 = 0., b1 = 0., a2 = 0., b2 = 0.;
    occ::handle<Geom2d_Curve> c2d1, c2d2;

    //: abv 29.08.01: torCuts.sat: distinguish degeneration by U and by V;
    // only corresponding move is prohibited
    //    bool isDeg = surf->IsDegenerated ( p, Max ( Precision(),
    //    BRep_Tool::Tolerance(V) ) );
    int      isDeg = 0;
    gp_Pnt2d degP1, degP2;
    double   degT1, degT2;
    if (surf->DegeneratedValues(p,
                                std::max(Precision(), BRep_Tool::Tolerance(V)),
                                degP1,
                                degP2,
                                degT1,
                                degT2))
      isDeg = (std::abs(degP1.X() - degP2.X()) > std::abs(degP1.Y() - degP2.Y()) ? 1 : 2);

    // abv 23 Feb 00: UKI60107-6 210: additional check for near-degenerated case
    // smh#15 PRO19800. Check if the surface is surface of revolution.
    if (surf->Surface()->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      if (!isDeg && !vclosed)
      {
        if (c2d1.IsNull() && !sae.PCurve(E1, Face(), c2d1, a1, b1, true))
          continue;
        gp_Pnt2d p1(SUF, c2d1->Value(b1).Y());
        gp_Pnt2d p2(SUL, c2d1->Value(b1).Y());
        if (surf->IsDegenerated(p1, p2, MaxTolerance(), 10)
            && !surf->IsDegenerated(c2d1->Value(a1),
                                    c2d1->Value(b1),
                                    MaxTolerance(),
                                    10)) // abv 31.07.00: trj4_pm1-ec-214.stp #31274: still allow
                                         // work if edge already exists
          isDeg = 1;
      }
      if (!isDeg && !uclosed)
      {
        if (c2d1.IsNull() && !sae.PCurve(E1, Face(), c2d1, a1, b1, true))
          continue;
        gp_Pnt2d p1(c2d1->Value(b1).X(), SVF);
        gp_Pnt2d p2(c2d1->Value(b1).X(), SVL);
        if (surf->IsDegenerated(p1, p2, MaxTolerance(), 10)
            && !surf->IsDegenerated(c2d1->Value(a1),
                                    c2d1->Value(b1),
                                    MaxTolerance(),
                                    10)) // abv 31.07.00: trj4_pm1-ec-214.stp #31274: still allow
                                         // work if edge already exists
          isDeg = 2;
      }
    }

    if (isDeg)
    {
      if (!degstop)
      {
        stop    = n2;
        degstop = true;
      }

      if (!degn2)
      {
        degn2 = n2;
        pdeg  = p;
      }
      else
      {
        if (pdeg.SquareDistance(p) < Precision() * Precision())
        {
          degn2 = n2;
          // if ( stop < n2 ) { stop = n2; degstop = true; }
        }
        else
        {
          double                    ax1 = 0., bx1 = 0., ax2 = 0., bx2 = 0.;
          occ::handle<Geom2d_Curve> cx1, cx2;
          if ((c2d1.IsNull() && !sae.PCurve(E1, Face(), c2d1, a1, b1, true))
              || (c2d2.IsNull() && !sae.PCurve(E2, Face(), c2d2, a2, b2, true))
              || !sae.PCurve(sbwd->Edge(degn2 > 1 ? degn2 - 1 : nb), Face(), cx1, ax1, bx1, true)
              || !sae.PCurve(sbwd->Edge(degn2), Face(), cx2, ax2, bx2, true))
          {
            myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
            continue;
          }
          gp_Pnt2d pd1 = cx1->Value(bx1);
          gp_Pnt2d pd2 = cx2->Value(ax2);
          gp_Pnt2d pn1 = c2d1->Value(b1);
          gp_Pnt2d pn2 = c2d2->Value(a2);
          gp_Vec2d x(0., 0.); // shift vector
          double   period;
          if (uclosed)
          {
            x.SetX(1.);
            period = URange;
          }
          else
          {
            x.SetY(1.);
            period = VRange;
          }
          double rot1 = (pn1.XY() - pd2.XY()) ^ x.XY();
          double rot2 = (pd1.XY() - pn2.XY()) ^ x.XY();
          double scld = (pd2.XY() - pd1.XY()) * x.XY();
          double scln = (pn2.XY() - pn1.XY()) * x.XY();
          if (rot1 * rot2 < -::Precision::PConfusion() && scld * scln < -::Precision::PConfusion()
              && std::abs(scln) > 0.1 * period && std::abs(scld) > 0.1 * period
              && rot1 * scld > ::Precision::PConfusion() && rot2 * scln > ::Precision::PConfusion())
          {
            // abv 02 Mar 00: trying more sophisticated analysis (ie_exhaust-A.stp #37520)
            double sign  = (rot2 > 0 ? 1. : -1.);
            double deep1 = std::min(
              sign * (pn2.XY() * x.XY()),
              std::min(
                sign * (pd1.XY() * x.XY()),
                std::min(
                  sign * (c2d2->Value(b2).XY() * x.XY()),
                  std::min(sign * (cx1->Value(ax1).XY() * x.XY()),
                           std::min(sign * (c2d2->Value(0.5 * (a2 + b2)).XY() * x.XY()),
                                    sign * (cx1->Value(0.5 * (ax1 + bx1)).XY() * x.XY()))))));
            double deep2 = std::max(
              sign * (pn1.XY() * x.XY()),
              std::max(
                sign * (pd2.XY() * x.XY()),
                std::max(
                  sign * (c2d1->Value(a1).XY() * x.XY()),
                  std::max(sign * (cx2->Value(bx2).XY() * x.XY()),
                           std::max(sign * (c2d1->Value(0.5 * (a1 + b1)).XY() * x.XY()),
                                    sign * (cx2->Value(0.5 * (ax2 + bx2)).XY() * x.XY()))))));
            double deep = deep2 - deep1; // estimated current size of wire by x
            // pdn 30 Oct 00: trying correct period [0,period] (trj5_k1-tc-203.stp #4698)
            double dx = ShapeAnalysis::AdjustToPeriod(deep,
                                                      ::Precision::PConfusion(),
                                                      period + ::Precision::PConfusion());
            x *= (scld > 0 ? -dx : dx);
            // x *= ( std::abs(scld-scln) > 1.5 * period ? 2. : 1. ) *
            //      ( scld >0 ? -period : period );
            gp_Trsf2d Shift;
            Shift.SetTranslation(x);
            for (int k = degn2;; k++)
            {
              if (k > nb)
                k = 1;
              if (k == n2)
                break;
              TopoDS_Edge edge = sbwd->Edge(k);
              if (!sae.PCurve(edge, Face(), cx1, ax1, bx1, true))
                continue;
              // cx1->Transform ( Shift );
              //  skl 15.05.2002 for OCC208 (if few edges have reference to one pcurve)
              occ::handle<Geom2d_Curve> cx1new =
                occ::down_cast<Geom2d_Curve>(cx1->Transformed(Shift));
              sbe.ReplacePCurve(edge, cx1new, Face());
              UpdateEdgeUVPoints(edge, Face());
            }
            myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
#ifdef OCCT_DEBUG
            std::cout << "Info: ShapeFix_Wire::FixShifted(): bi - meridian case fixed" << std::endl;
#endif
            continue;
          }
          // degn2 = n2; pdeg = p; // ie_exhaust-A.stp #37520
        }
      }
      /*
            // pdn to fix half sphere
            TopoDS_Vertex VE = sae.LastVertex ( E2 );
            gp_Pnt pe = BRep_Tool::Pnt ( VE );
            //pdn is second vertex on singular point ?
            if ( surf->IsDegenerated ( pe, Max ( Precision(), BRep_Tool::Tolerance(V) ) ) ) {
          if ( ( c2d1.IsNull() && ! sae.PCurve ( E1, Face(), c2d1, a1, b1, true ) ) ||
               ( c2d2.IsNull() && ! sae.PCurve ( E2, Face(), c2d2, a2, b2, true ) ) ) {
            myLastFixStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL1 );
            continue;
          }
          gp_Pnt2d p2d1 = c2d1->Value ( b1 ), p2f = c2d2->Value ( a2 ), p2l = c2d2->Value ( b2 );
          double pres2 = ::Precision::PConfusion();
          double du = 0.,dv = 0.;
          //#79 rln 15.03.99 S4135: bmarkmdl.igs entity 633 (incorrectly oriented contour) check for
         gap
          if(uclosed&&(std::abs(p2f.X()-p2l.X())<pres2)&&std::abs(p2d1.X()-p2f.X())>GAS.UResolution(Precision()))
         { if((std::abs(p2f.X()-SUF)<pres2)&&(p2f.Y()<p2l.Y())) du = URange;
            if((std::abs(p2f.X()-SUL)<pres2)&&(p2f.Y()>p2l.Y()))
              du = -URange;
          }
          if(vclosed&&(std::abs(p2f.Y()-p2l.Y())<pres2)&&std::abs(p2d1.Y()-p2f.Y())>GAS.VResolution(Precision()))
         { if((std::abs(p2f.Y()-SVF)<pres2)&&(p2f.X()>p2l.X())) dv = VRange;
            if((std::abs(p2f.Y()-SVL)<pres2)&&(p2f.X()<p2l.X()))
              dv = -VRange;
          }
          if ( du ==0. && dv == 0. ) continue;
          myLastFixStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
          gp_Trsf2d Shift;
          Shift.SetTranslation ( gp_Vec2d ( du, dv ) );
          c2d2->Transform ( Shift );
          UpdateEdgeUVPoints ( E2, Face() );//rln 15.03.99 syntax correction :E1
            }
      */
      //: abv 29.08.01: torCuts.sat:      continue;
    }

    if ((c2d1.IsNull() && !sae.PCurve(E1, Face(), c2d1, a1, b1, true))
        || (c2d2.IsNull() && !sae.PCurve(E2, Face(), c2d2, a2, b2, true)))
    {
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
      continue;
    }
    gp_Pnt2d p2d1 = c2d1->Value(b1);
    gp_Pnt2d p2d2 = c2d2->Value(a2);
    box.Add(p2d1);

    double du = 0., dv = 0.;
    if (uclosed && isDeg != 1)
    {
      double dx = std::abs(p2d2.X() - p2d1.X());
      if (dx > URange - UTol)
        du = ShapeAnalysis::AdjustByPeriod(p2d2.X(), p2d1.X(), URange);
      else if (dx > UTol && stop == nb)
        stop = n2; //: abv 29.08.01: torCuts2.stp
    }
    if (vclosed && isDeg != 2)
    {
      double dy = std::abs(p2d2.Y() - p2d1.Y());
      if (dy > VRange - VTol)
        dv = ShapeAnalysis::AdjustByPeriod(p2d2.Y(), p2d1.Y(), VRange);
      else if (dy > VTol && stop == nb)
        stop = n2;
    }
    if (du == 0. && dv == 0.)
      continue;

    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    gp_Trsf2d Shift;
    Shift.SetTranslation(gp_Vec2d(du, dv));
    // c2d2->Transform ( Shift );
    //  skl 15.05.2002 for OCC208 (if few edges have reference to one pcurve)
    occ::handle<Geom2d_Curve> c2d2new = occ::down_cast<Geom2d_Curve>(c2d2->Transformed(Shift));
    sbe.ReplacePCurve(E2, c2d2new, Face());
    UpdateEdgeUVPoints(E2, Face());
  }
  if (box.IsVoid())
    return false; // #3 smh 01.04.99. S4163: Overflow, when box is void.

  double umin, vmin, umax, vmax;
  box.Get(umin, vmin, umax, vmax);
  if (std::abs(umin + umax - SUF - SUL) < URange && std::abs(vmin + vmax - SVF - SVL) < VRange
      && !LastFixStatus(ShapeExtend_DONE))
    return false;

  box.SetVoid();
  int n; // svv Jan11 2000 : porting on DEC
  for (n = 1; n <= nb; n++)
  {
    double                    a, b;
    occ::handle<Geom2d_Curve> c2d;
    if (!sae.PCurve(sbwd->Edge(n), Face(), c2d, a, b, true))
      continue;
    box.Add(c2d->Value(a));
    box.Add(c2d->Value(0.5 * (a + b)));
  }
  box.Get(umin, vmin, umax, vmax);

  double du = 0., dv = 0.;

  if (uclosed)
  {
    double umid = 0.5 * (umin + umax);
    //     du = ShapeAnalysis::AdjustToPeriod(umid, SUF, SUL);
    // PTV 26.06.2002 xloop torus-apple iges face mode
    du = ShapeAnalysis::AdjustByPeriod(umid, SUMid, URange);
  }
  if (vclosed)
  {
    double vmid = 0.5 * (vmin + vmax);
    //     dv = ShapeAnalysis::AdjustToPeriod(vmid, SVF, SVL);
    // PTV 26.06.2002 xloop torus-apple iges face mode
    dv = ShapeAnalysis::AdjustByPeriod(vmid, SVMid, VRange);
  }

  if (du == 0. && dv == 0.)
    return true;

  myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);

  gp_Trsf2d Shift;
  Shift.SetTranslation(gp_Vec2d(du, dv));

  for (n = 1; n <= sbwdOring->NbEdges(); n++)
  {
    double                    a, b;
    occ::handle<Geom2d_Curve> c2d;
    TopoDS_Edge               ed = sbwdOring->Edge(n);
    if (!sae.PCurve(ed, Face(), c2d, a, b, true))
      continue;
    // skl 15.05.2002 for OCC208 (if few edges have reference to one pcurve)
    occ::handle<Geom2d_Curve> c2d2 = occ::down_cast<Geom2d_Curve>(c2d->Transformed(Shift));
    sbe.ReplacePCurve(ed, c2d2, Face());
    UpdateEdgeUVPoints(ed, Face());
  }
  return true;
}

//=================================================================================================

bool ShapeFix_Wire::FixDegenerated(const int num)
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  // analysis
  gp_Pnt2d p2d1, p2d2;
  myAnalyzer->CheckDegenerated(num, p2d1, p2d2);
  if (myAnalyzer->LastCheckStatus(ShapeExtend_FAIL1))
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  }
  //: abv 29.08.01: torHalf2.sat: if edge was encoded as degenerated but
  //  has no pcurve and no singularity is found at that point, remove it
  if (myAnalyzer->LastCheckStatus(ShapeExtend_FAIL2))
  {
    WireData()->Remove(num);
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
    return true;
  }
  if (!myAnalyzer->LastCheckStatus(ShapeExtend_DONE))
    return false;

  // action: create degenerated edge and insert it (or replace)

  gp_Vec2d                 vect2d(p2d1, p2d2);
  gp_Dir2d                 dir2d(vect2d);
  occ::handle<Geom2d_Line> line2d = new Geom2d_Line(p2d1, dir2d);

  TopoDS_Edge  degEdge;
  BRep_Builder B;
  B.MakeEdge(degEdge);
  B.Degenerated(degEdge, true);
  B.UpdateEdge(degEdge, line2d, Face(), ::Precision::Confusion());
  B.Range(degEdge, Face(), 0., vect2d.Magnitude());

  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  int                               n2   = (num > 0 ? num : sbwd->NbEdges());
  int                               n1   = (n2 > 1 ? n2 - 1 : sbwd->NbEdges());

  bool lack = myAnalyzer->LastCheckStatus(ShapeExtend_DONE1);
  int  n3   = (lack ? n2 : (n2 < sbwd->NbEdges() ? n2 + 1 : 1));

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      V1 = sae.LastVertex(sbwd->Edge(n1));
  TopoDS_Vertex      V2 = sae.FirstVertex(sbwd->Edge(n3));

  V1.Orientation(TopAbs_FORWARD);
  V2.Orientation(TopAbs_REVERSED);
  B.Add(degEdge, V1);
  B.Add(degEdge, V2);
  degEdge.Orientation(TopAbs_FORWARD);

  if (lack)
  {
    sbwd->Add(degEdge, n2);
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  }
  else
  {
    sbwd->Set(degEdge, n2);
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
  }

  //  commented to avoid extra messages
  //  SendWarning ( degEdge, Message_Msg ( "FixWire.FixDegenerated.MSG0" ) );// Degenerated edge(s)
  //  detected

  return true;
}

//=======================================================================
// function : FixSelfIntersectingEdge
// purpose  : Tests edge for self-intersection and updates tolerance of vertex
//           if intersection is found
//           Returns True if tolerance was increased
//=======================================================================

// Create edge on basis of E with new pcurve and call FixSP
// Return resulting tolerance and modified pcurve
static bool TryNewPCurve(const TopoDS_Edge&         E,
                         const TopoDS_Face&         face,
                         occ::handle<Geom2d_Curve>& c2d,
                         double&                    first,
                         double&                    last,
                         double&                    tol)
{
  double                  f, l;
  occ::handle<Geom_Curve> crv = BRep_Tool::Curve(E, f, l);
  if (crv.IsNull())
    return false;

  // make temp edge and compute tolerance
  BRepBuilderAPI_MakeEdge mkedge(crv, f, l);

  ShapeBuild_Edge SBE;          // skl 17.07.2001
  SBE.SetRange3d(mkedge, f, l); // skl 17.07.2001

  if (!mkedge.IsDone())
    return false;

  TopoDS_Edge  edge = mkedge;
  BRep_Builder B;
  B.UpdateEdge(edge, c2d, face, 0.);
  B.Range(edge, face, first, last);
  B.SameRange(edge, false);
  // no call to BRepLib:  B.SameParameter ( edge, false );

  occ::handle<ShapeFix_Edge> sfe = new ShapeFix_Edge;
  sfe->FixSameParameter(edge, face);
  c2d = BRep_Tool::CurveOnSurface(edge, face, first, last);
  tol = BRep_Tool::Tolerance(edge);
  return true;
}

//: k3 abv 24 Dec 98: BUC50070 #26682 and #30087:
// Try to cut out the loop on the pcurve and make new pcurve by concatenating
// the parts.
// If result is not SameParameter with prec, does nothing and returns False
// Warning: resulting pcurve will be C0

//=================================================================================================

static int howMuchPCurves(const TopoDS_Edge& E)
{
  int                                                               count = 0;
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(
    (*((occ::handle<BRep_TEdge>*)&E.TShape()))->ChangeCurves());

  while (itcr.More())
  {
    const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
    if (cr->IsCurveOnSurface())
      count++;
    itcr.Next();
  }

  return count;
}

//=================================================================================================

static bool RemoveLoop(TopoDS_Edge&                      E,
                       const TopoDS_Face&                face,
                       const IntRes2d_IntersectionPoint& IP,
                       const double                      tolfact,
                       const double                      prec,
                       const bool                        RemoveLoop3d)
{
  bool loopRemoved3d;
  if (BRep_Tool::IsClosed(E, face))
    return false;

  double                  f, l;
  occ::handle<Geom_Curve> crv = BRep_Tool::Curve(E, f, l);

  double t1 = IP.ParamOnFirst();
  double t2 = IP.ParamOnSecond();
  if (t1 > t2)
  {
    double t = t1;
    t1       = t2;
    t2       = t;
  }

  ShapeAnalysis_Edge        sae;
  double                    a, b;
  occ::handle<Geom2d_Curve> c2d;
  if (!sae.PCurve(E, face, c2d, a, b, false))
    return false;

#ifdef OCCT_DEBUG
  std::cout << "Cut Loop: params (" << t1 << ", " << t2;
#endif
  GeomAdaptor_Curve GAC(crv, f, l);
  double            dt = tolfact * GAC.Resolution(prec);
  t1 -= dt; // 1e-3;//::Precision::PConfusion();
  t2 += dt; // 1e-3;//::Precision::PConfusion();
#ifdef OCCT_DEBUG
  std::cout << ") -> (" << t1 << ", " << t2 << ")" << std::endl;
#endif

  if (t1 <= a || t2 >= b)
  { // should not be so, but to be sure ..
    return false;
  }

  // direct construction causes error on osf system.
  gp_Pnt p(0, 0, 0);
  gp_Dir d(gp_Dir::D::Z);
  gp_Ax3 ax(p, d);
  gp_Pln Pln(ax);

  // PTV OCC884
  occ::handle<Geom_Plane> aPlaneSurf = occ::down_cast<Geom_Plane>(BRep_Tool::Surface(face));
  occ::handle<Geom_Curve> pcurve3d   = crv;
  if (!aPlaneSurf.IsNull())
  {
    Pln = aPlaneSurf->Pln();
  }
  else
    pcurve3d = GeomAPI::To3d(c2d, Pln);

  // first segment
  // occ::handle<Geom_Curve> pcurve3d = GeomAPI::To3d ( c2d, Pln );
  occ::handle<Geom_TrimmedCurve>      trim = new Geom_TrimmedCurve(pcurve3d, a, t1);
  GeomConvert_CompCurveToBSplineCurve connect(trim);

  // null-length segment patch instead of loop
  NCollection_Array1<gp_Pnt> Poles(1, 2);
  NCollection_Array1<double> Knots(1, 2);
  NCollection_Array1<int>    Mults(1, 2);

  Poles.SetValue(1, trim->Value(t1));
  Knots.SetValue(1, t1);
  Mults.SetValue(1, 2);

  trim = new Geom_TrimmedCurve(pcurve3d, t2, b);
  Poles.SetValue(2, trim->Value(t2));
  Knots.SetValue(2, t2);
  Mults.SetValue(2, 2);

  occ::handle<Geom_BSplineCurve> patch = new Geom_BSplineCurve(Poles, Knots, Mults, 1);
  if (!connect.Add(patch, ::Precision::PConfusion(), true, false))
    return false;

  // last segment
  if (!connect.Add(trim, ::Precision::PConfusion(), true, false))
    return false;
  // PTV OCC884
  // keep created 3d curve
  occ::handle<Geom_Curve> aNew3dCrv = connect.BSplineCurve();

  occ::handle<Geom2d_Curve> bs = GeomAPI::To2d(aNew3dCrv, Pln);
  if (bs.IsNull())
    return false;

  // make temp edge and compute tolerance
  BRep_Builder B;

  if (!RemoveLoop3d)
  { // old variant (not remove loop 3d)
    double newtol = 0;
    // OCC901
    int nbC2d = howMuchPCurves(E);
    if (nbC2d <= 1 && !aPlaneSurf.IsNull())
      B.UpdateEdge(E, bs, face, 0);
    else if (!TryNewPCurve(E, face, bs, a, b, newtol))
      return false;

    double tol = BRep_Tool::Tolerance(E);
#ifdef OCCT_DEBUG
    std::cout << "Cut Loop: tol orig " << tol << ", prec " << prec << ", new tol " << newtol
              << std::endl;
#endif
    if (newtol > std::max(prec, tol))
      return false;
    //: s2  bs = BRep_Tool::CurveOnSurface ( edge, face, a, b );
    if (std::abs(a - f) > ::Precision::PConfusion() || // smth strange, cancel
        std::abs(b - l) > ::Precision::PConfusion())
      return false;
    // PTV OCC884
    if (!aPlaneSurf.IsNull())
    {
      B.UpdateEdge(E, aNew3dCrv, std::max(newtol, tol));
      // OCC901
      if (!TryNewPCurve(E, face, bs, a, b, newtol))
        return false;
    }
    B.UpdateEdge(E, bs, face, newtol);
    B.UpdateVertex(sae.FirstVertex(E), newtol);
    B.UpdateVertex(sae.LastVertex(E), newtol);
    return true;
  }

  //: q1
  TopLoc_Location                  L;
  occ::handle<Geom_Surface>        S  = BRep_Tool::Surface(face, L);
  occ::handle<Geom2dAdaptor_Curve> AC = new Geom2dAdaptor_Curve(c2d);
  occ::handle<GeomAdaptor_Surface> AS = new GeomAdaptor_Surface(S);

  Adaptor3d_CurveOnSurface ACS(AC, AS);
  gp_Pnt                   P1(ACS.Value(t1));
  gp_Pnt                   P2(ACS.Value(t2));
  gp_Pnt pcurPnt((P1.X() + P2.X()) / 2, (P1.Y() + P2.Y()) / 2, (P1.Z() + P2.Z()) / 2);

  ShapeAnalysis_TransferParametersProj       SFTP(E, face);
  occ::handle<NCollection_HSequence<double>> Seq2d = new NCollection_HSequence<double>;
  Seq2d->Append(t1);
  Seq2d->Append(t2);
  Seq2d->Append((t1 + t2) / 2);
  occ::handle<NCollection_HSequence<double>> Seq3d = SFTP.Perform(Seq2d, false);

  // clang-format off
  double dist1 = pcurPnt.Distance(crv->Value(Seq3d->Value(1)));// correcting Seq3d already project
  // clang-format on
  double dist2 = pcurPnt.Distance(crv->Value(Seq3d->Value(2)));
  double dist3 = pcurPnt.Distance(crv->Value(Seq3d->Value(3)));
  double ftrim, ltrim;
  loopRemoved3d = dist3 <= std::max(dist1, dist2);

  occ::handle<Geom_Curve> bs1;

  if (!loopRemoved3d)
  {
    // create new 3d curve
    ftrim = Seq3d->Value(1);
    ltrim = Seq3d->Value(2);
    ftrim -= dt;
    ltrim += dt;
    occ::handle<Geom_TrimmedCurve>      trim1 = new Geom_TrimmedCurve(crv, f, ftrim);
    GeomConvert_CompCurveToBSplineCurve connect1(trim1);
    NCollection_Array1<gp_Pnt>          Poles1(1, 2);
    NCollection_Array1<double>          Knots1(1, 2);
    NCollection_Array1<int>             Mults1(1, 2);

    Poles1.SetValue(1, trim1->Value(ftrim));
    Knots1.SetValue(1, ftrim);
    Mults1.SetValue(1, 2);

    trim1 = new Geom_TrimmedCurve(crv, ltrim, l);
    Poles1.SetValue(2, trim1->Value(ltrim));
    Knots1.SetValue(2, ltrim);
    Mults1.SetValue(2, 2);

    // create b-spline curve
    occ::handle<Geom_BSplineCurve> patch1 = new Geom_BSplineCurve(Poles1, Knots1, Mults1, 1);

    if (!connect1.Add(patch1, ::Precision::PConfusion(), true, false))
      return false;

    // last segment
    if (!connect1.Add(trim1, ::Precision::PConfusion(), true, false))
      return false;

    bs1 = connect1.BSplineCurve();

    if (bs1.IsNull())
      return false;
  }
  //  double oldtol = BRep_Tool::Tolerance ( E );

  if (!loopRemoved3d)
    B.UpdateEdge(E, bs1, L, 0.);
  B.UpdateEdge(E, bs, face, 0.);
  B.Range(E, face, f, l);
  B.SameRange(E, false);

  occ::handle<ShapeFix_Edge> sfe = new ShapeFix_Edge;
  sfe->FixSameParameter(E);

  return true;
}

//=================================================================================================

static bool RemoveLoop(TopoDS_Edge&                      E,
                       const TopoDS_Face&                face,
                       const IntRes2d_IntersectionPoint& IP2d,
                       TopoDS_Edge&                      E1,
                       TopoDS_Edge&                      E2)
{
#ifdef OCCT_DEBUG
  std::cout << "Info: ShapeFix_Wire::FixSelfIntersection : Try insert vertex" << std::endl;
#endif

  if (BRep_Tool::IsClosed(E, face))
    return false;

  double                  f, l;
  occ::handle<Geom_Curve> crv = BRep_Tool::Curve(E, f, l);

  double t1 = IP2d.ParamOnFirst();
  double t2 = IP2d.ParamOnSecond();

  if (t1 > t2)
  {
    double t = t1;
    t1       = t2;
    t2       = t;
  }

  ShapeAnalysis_Edge sae;

  // define vertexes Vfirst , Vlast, Vmid
  TopoDS_Vertex Vfirst, Vlast, Vmid;
  // initialize Vfirst and Vlast
  Vfirst = sae.FirstVertex(E);
  Vlast  = sae.LastVertex(E);

  // find a 2d curve and parameters from edge E
  double                    a, b;
  occ::handle<Geom2d_Curve> c2d;
  if (!sae.PCurve(E, face, c2d, a, b, false))
    return false;

  // first segment for 2d curve
  occ::handle<Geom2d_TrimmedCurve> trim1;
  if ((t1 - a) > Precision::PConfusion())
    trim1 = new Geom2d_TrimmedCurve(c2d, a, t1);
  // second segment for 2d curve
  occ::handle<Geom2d_TrimmedCurve> trim2 = new Geom2d_TrimmedCurve(c2d, t2, b);

  //  if ( trim1.IsNull() || trim2.IsNull()  )
  if (trim2.IsNull())
    return false;

  TopLoc_Location                  L;
  occ::handle<Geom_Surface>        S  = BRep_Tool::Surface(face, L);
  occ::handle<Geom2dAdaptor_Curve> AC = new Geom2dAdaptor_Curve(c2d);
  occ::handle<GeomAdaptor_Surface> AS = new GeomAdaptor_Surface(S);

  Adaptor3d_CurveOnSurface ACS(AC, AS);
  gp_Pnt                   P1(ACS.Value(t1));
  gp_Pnt                   P2(ACS.Value(t2));
  gp_Pnt pcurPnt((P1.X() + P2.X()) / 2, (P1.Y() + P2.Y()) / 2, (P1.Z() + P2.Z()) / 2);

  // transfer parameters from pcurve to 3d curve
  ShapeAnalysis_TransferParametersProj       SFTP(E, face);
  occ::handle<NCollection_HSequence<double>> Seq2d = new NCollection_HSequence<double>;
  Seq2d->Append(t1);
  Seq2d->Append(t2);
  Seq2d->Append((t1 + t2) / 2);
  occ::handle<NCollection_HSequence<double>> Seq3d = SFTP.Perform(Seq2d, false);

  // clang-format off
  double dist1 = pcurPnt.Distance(crv->Value(Seq3d->Value(1)));// correcting Seq3d already project
  // clang-format on
  double dist2 = pcurPnt.Distance(crv->Value(Seq3d->Value(2)));
  double dist3 = pcurPnt.Distance(crv->Value(Seq3d->Value(3)));
  double ftrim, ltrim;
  if (dist3 > std::max(dist1, dist2))
  { // is loop in 3d
    ftrim = Seq3d->Value(1);
    ltrim = Seq3d->Value(2);
  }
  else
  { // not loop in 3d
    ftrim = Seq3d->Value(3);
    ltrim = Seq3d->Value(3);
  }

  //  ftrim = Seq3d->Value(1);
  //  ltrim = Seq3d->Value(2);

  // trim for 3d curve 'crv' with parameters from 'f' to 'l'
  occ::handle<Geom_TrimmedCurve> trim3;
  if (!trim1.IsNull())
    trim3 = new Geom_TrimmedCurve(crv, f, ftrim);
  // second segment for 3d curve
  occ::handle<Geom_TrimmedCurve> trim4 = new Geom_TrimmedCurve(crv, ltrim, l);

  //  if ( trim3.IsNull() || trim4.IsNull()  )
  if (trim4.IsNull())
    return false;

  // create a point for middle vertex
  gp_Pnt pnt1 = crv->Value(ftrim);
  gp_Pnt pnt2 = crv->Value(ltrim);
  gp_Pnt Pmid((pnt1.X() + pnt2.X()) / 2, (pnt1.Y() + pnt2.Y()) / 2, (pnt1.Z() + pnt2.Z()) / 2);

  BRep_Builder B;

  // create new copies for E1 and E2
  if (!trim1.IsNull())
    E1 = TopoDS::Edge(E.EmptyCopied());
  E2 = TopoDS::Edge(E.EmptyCopied());

  // initialize middle vertex Vmid
  if (trim1.IsNull())
    B.MakeVertex(Vmid, pnt2, 0.);
  else
    B.MakeVertex(Vmid, Pmid, 0.);

  ShapeBuild_Edge sbe;

  // replace verteces for new edges E1 and E2
  if (E.Orientation() == TopAbs_FORWARD)
  {
    if (!E1.IsNull())
      E1 = sbe.CopyReplaceVertices(E1, Vfirst, Vmid);
    E2 = sbe.CopyReplaceVertices(E2, Vmid, Vlast);
  }
  else
  {
    if (!E1.IsNull())
      E1 = sbe.CopyReplaceVertices(E1, Vmid, Vlast);
    E2 = sbe.CopyReplaceVertices(E2, Vfirst, Vmid);
  }

  // Update edges by 2d and 3d curves
  occ::handle<ShapeFix_Edge> mySfe = new ShapeFix_Edge;
  if (!E1.IsNull())
  {
    B.UpdateEdge(E1, trim1, face, 0.);
    B.UpdateEdge(E1, trim3, 0.);
    B.Range(E1, f, ftrim);
    B.SameRange(E1, false);
    //    B.SameParameter(E1,false);
    mySfe->FixSameParameter(E1);
    mySfe->FixVertexTolerance(E1);
  }
  B.UpdateEdge(E2, trim2, face, 0.);
  B.UpdateEdge(E2, trim4, 0.);
  B.Range(E2, ltrim, l);
  B.SameRange(E2, false);
  //  B.SameParameter(E2, false);
  mySfe->FixSameParameter(E2);
  mySfe->FixVertexTolerance(E2);

  return true;
}

//=================================================================================================

bool ShapeFix_Wire::FixSelfIntersectingEdge(const int num)
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  // analysis
  NCollection_Sequence<IntRes2d_IntersectionPoint> points2d;
  NCollection_Sequence<gp_Pnt>                     points3d;
  occ::handle<ShapeAnalysis_Wire>                  theAdvAnalyzer = myAnalyzer;
  if (theAdvAnalyzer.IsNull())
    return false;
  theAdvAnalyzer->CheckSelfIntersectingEdge(num, points2d, points3d);
  if (theAdvAnalyzer->LastCheckStatus(ShapeExtend_FAIL))
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  }
  if (!theAdvAnalyzer->LastCheckStatus(ShapeExtend_DONE))
    return false;

  // action: increase tolerance of vertex

  TopoDS_Edge E = WireData()->Edge(num > 0 ? num : NbEdges());

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      V1   = sae.FirstVertex(E);
  TopoDS_Vertex      V2   = sae.LastVertex(E);
  double             tol1 = BRep_Tool::Tolerance(V1);
  double             tol2 = BRep_Tool::Tolerance(V2);
  gp_Pnt             pnt1 = BRep_Tool::Pnt(V1);
  gp_Pnt             pnt2 = BRep_Tool::Pnt(V2);

  // cycle is to verify fix in case of RemoveLoop
  double                    tolfact = 0.1; // factor for shifting by parameter in RemoveLoop
  double                    f2d = 0., l2d = 0.;
  occ::handle<Geom2d_Curve> c2d;
  double                    newtol = 0.; // = Precision();

  if (myRemoveLoopMode < 1)
  {
    for (int iter = 0; iter < 30; iter++)
    {
      bool   loopRemoved = false;
      double prevFirst = 0, prevLast = 0;
      for (int i = 1; i <= points2d.Length(); i++)
      {
        gp_Pnt pint   = points3d.Value(i);
        double dist21 = pnt1.SquareDistance(pint);
        double dist22 = pnt2.SquareDistance(pint);
        if (dist21 < tol1 * tol1 || dist22 < tol2 * tol2)
          continue;
        newtol = 1.001 * std::sqrt(std::min(dist21, dist22)); //: f8

        //: k3 abv 24 Dec 98: BUC50070 #26682 and #30087: try to remove loop
        if (myGeomMode)
        {
          if (c2d.IsNull())
            sae.PCurve(E, Face(), c2d, f2d, l2d, false);
          double firstpar = points2d.Value(i).ParamOnFirst();
          double lastpar  = points2d.Value(i).ParamOnSecond();
          if (firstpar > prevFirst && lastpar < prevLast)
            continue;
          if (RemoveLoop(E,
                         Face(),
                         points2d.Value(i),
                         tolfact,
                         std::min(MaxTolerance(), std::max(newtol, Precision())),
                         myRemoveLoopMode == 0))
          {
            myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
            loopRemoved = true;
            prevFirst   = firstpar;
            prevLast    = lastpar;
            continue; // repeat of fix on that edge required (to be done by caller)
          }
        }
        if (newtol < MaxTolerance())
        {
          myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
          BRep_Builder B;
          if (dist21 < dist22)
            B.UpdateVertex(V1, tol1 = newtol);
          else
            B.UpdateVertex(V2, tol2 = newtol);
        }
        else
          myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      }

      // after RemoveLoop, check that self-intersection disappeared
      if (loopRemoved)
      {
        NCollection_Sequence<IntRes2d_IntersectionPoint> pnts2d;
        NCollection_Sequence<gp_Pnt>                     pnts3d;
        theAdvAnalyzer->CheckSelfIntersectingEdge(num, pnts2d, pnts3d);
        if (!theAdvAnalyzer->LastCheckStatus(ShapeExtend_DONE))
          break;
        // points3d.Append(pnts3d);
        // points2d.Append(pnts2d);
        points3d = pnts3d;
        points2d = pnts2d;
        BRep_Builder B;
        B.UpdateEdge(E, c2d, Face(), 0.);
        B.Range(E, Face(), f2d, l2d);
        // newtol+=Precision();
      }
      else
      {
        break;
      }
    }
  }

  //===============================================
  // RemoveLoopMode = 1 , insert vertex
  //===============================================
  if (myRemoveLoopMode == 1)
  {
    // after fixing will be nb+1 edges
    bool loopRemoved;
    // create a sequence of resulting edges
    occ::handle<NCollection_HSequence<TopoDS_Shape>> TTSS = new NCollection_HSequence<TopoDS_Shape>;
    TopoDS_Edge                                      E2;

    loopRemoved = false;
    //: k3 abv 24 Dec 98: BUC50070 #26682 and #30087: try to remove loop
    if (myGeomMode)
    {
      if (c2d.IsNull())
        sae.PCurve(E, Face(), c2d, f2d, l2d, false);
      TopoDS_Edge E1;
      if (RemoveLoop(E, Face(), points2d.Value(1), E1, E2))
      {
        myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
        loopRemoved = true;
        if (!E1.IsNull())
        {
          TTSS->Append(E1);
          newtol = std::max(BRep_Tool::Tolerance(E1), BRep_Tool::Tolerance(E2));
        }
        else
          newtol = BRep_Tool::Tolerance(E2);
      }
    }

    TTSS->Append(E2);

    if (newtol > MaxTolerance())
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);

    ShapeExtend_WireData sewd;
    for (int i = 1; i <= TTSS->Length(); i++)
    {
      sewd.Add(TopoDS::Edge(TTSS->Value(i)));
    }
    if (!Context().IsNull())
    {
      Context()->Replace(E, sewd.Wire());
      UpdateWire();
    }
    else
    {
      WireData()->Remove(num > 0 ? num : NbEdges());
      WireData()->Add(sewd.Wire(), num > 0 ? num : NbEdges());
    }
    if (loopRemoved)
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE8);
  }

  if (LastFixStatus(ShapeExtend_DONE) && !myShape.IsNull())
  {
    // clang-format off
    SendWarning ( E, Message_Msg ( "FixAdvWire.FixIntersection.MSG5" ) );// Edge was self-intersecting, corrected
    // clang-format on
  }

  return LastFixStatus(ShapeExtend_DONE);
}

//=================================================================================================

static double ComputeLocalDeviation(const TopoDS_Edge& edge,
                                    const gp_Pnt&      pint,
                                    const gp_Pnt&      pnt,
                                    double             f,
                                    double             l,
                                    const TopoDS_Face& face)
{
  ShapeAnalysis_Edge      sae;
  occ::handle<Geom_Curve> c3d;
  double                  a, b;
  if (!sae.Curve3d(edge, c3d, a, b, false))
    return RealLast();

  gp_Lin line(pint, gp_Vec(pint, pnt));

  occ::handle<Geom2d_Curve> Crv;
  double                    fp, lp;
  if (sae.PCurve(edge, face, Crv, fp, lp, false))
  {
    if (Crv->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      occ::handle<Geom2d_TrimmedCurve> tc = occ::down_cast<Geom2d_TrimmedCurve>(Crv);
      if (tc->BasisCurve()->IsKind(STANDARD_TYPE(Geom2d_Line)))
      {
        f = a + (f - fp) * (b - a) / (lp - fp);
        l = a + (l - fp) * (b - a) / (lp - fp);
      }
    }
  }

  const int NSEG = 10;
  double    step = (l - f) / NSEG;
  double    dev  = 0.;
  for (int i = 1; i < NSEG; i++)
  {
    gp_Pnt p = c3d->Value(f + i * step);
    double d = line.Distance(p);
    if (dev < d)
      dev = d;
  }
  return dev;
}

//=================================================================================================

bool ShapeFix_Wire::FixIntersectingEdges(const int num)
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady() || NbEdges() < 2)
    return false;

  // analysis
  NCollection_Sequence<IntRes2d_IntersectionPoint> points2d;
  NCollection_Sequence<gp_Pnt>                     points3d;
  NCollection_Sequence<double>                     errors;
  occ::handle<ShapeAnalysis_Wire>                  theAdvAnalyzer = myAnalyzer;
  if (theAdvAnalyzer.IsNull())
    return false;
  theAdvAnalyzer->CheckIntersectingEdges(num, points2d, points3d, errors);
  if (theAdvAnalyzer->LastCheckStatus(ShapeExtend_FAIL))
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  }
  if (!theAdvAnalyzer->LastCheckStatus(ShapeExtend_DONE))
    return false;

  // rln 03/02/98: CSR#BUC50004 entity 56 (to avoid later inserting lacking edge)
  //: l0  bool isLacking = myAnalyzer->CheckLacking ( num );

  // action: increase tolerance of vertex

  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  int                               n2   = (num > 0 ? num : sbwd->NbEdges());
  int                               n1   = (n2 > 1 ? n2 - 1 : sbwd->NbEdges());
  TopoDS_Edge                       E1   = sbwd->Edge(n1);
  TopoDS_Edge                       E2   = sbwd->Edge(n2);
  if (!Context().IsNull())
  {
    E1 = TopoDS::Edge(Context()->Apply(sbwd->Edge(n1)));
    E2 = TopoDS::Edge(Context()->Apply(sbwd->Edge(n2)));
  }

  bool   isForward1 = (E1.Orientation() == TopAbs_FORWARD);
  bool   isForward2 = (E2.Orientation() == TopAbs_FORWARD);
  double a1, b1, a2, b2;
  BRep_Tool::Range(E1, Face(), a1, b1);
  BRep_Tool::Range(E2, Face(), a2, b2);

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      Vp = sae.FirstVertex(E1);
  TopoDS_Vertex      V1 = sae.LastVertex(E1);
  TopoDS_Vertex      V2 = sae.FirstVertex(E2);
  TopoDS_Vertex      Vn = sae.LastVertex(E2);

  double tol = BRep_Tool::Tolerance(V1);
  gp_Pnt pnt = BRep_Tool::Pnt(V1);

  double prevRange1 = RealLast(), prevRange2 = RealLast();
  bool   cutEdge1 = false, cutEdge2 = false;
  bool   IsCutLine     = false;
  bool   isChangedEdge = false;

  BRep_Builder B;

  int nb = points3d.Length();
  for (int i = 1; i <= nb; i++)
  {
    const IntRes2d_IntersectionPoint& IP     = points2d.Value(i);
    double                            param1 = (num == 1 ? IP.ParamOnSecond() : IP.ParamOnFirst());
    double                            param2 = (num == 1 ? IP.ParamOnFirst() : IP.ParamOnSecond());

    double newRange1 = std::abs((isForward1 ? a1 : b1) - param1);
    double newRange2 = std::abs((isForward2 ? b2 : a2) - param2);
    if (newRange1 > prevRange1 && newRange2 > prevRange2)
      continue;

    gp_Pnt pint   = points3d.Value(i);
    double rad    = errors.Value(i);
    double newtol = 1.0001 * (pnt.Distance(pint) + rad);

    // GeomAdaptor_Surface& Ads = myAnalyzer->Surface()->Adaptor3d()->ChangeSurface();

    //: r8 abv 12 Apr 99: try increasing tolerance of edge

    bool locMayEdit = myTopoMode;
    // Always try to modify the tolerance firstly as a better solution
    if (/*! myTopoMode &&*/ newtol > tol)
    {
      double te1 =
        rad + ComputeLocalDeviation(E1, pint, pnt, param1, (isForward1 ? b1 : a1), Face());
      double te2 =
        rad + ComputeLocalDeviation(E2, pint, pnt, (isForward2 ? a2 : b2), param2, Face());
      double maxte = std::max(te1, te2);
      if (maxte < MaxTolerance() && maxte < newtol)
      {
        if (BRep_Tool::Tolerance(E1) < te1 || BRep_Tool::Tolerance(E2) < te2)
        {
#ifdef OCCT_DEBUG
          std::cout << "Warning: ShapeFix_Wire::FixIE: edges tolerance increased: (" << te1 << ", "
                    << te2 << ") / " << newtol << std::endl;
#endif

          // Make copy of edges.
          if (!Context().IsNull())
          {
            isChangedEdge = true; // To avoid double copying of vertexes.

            // Intersection point of two base edges.
            ShapeBuild_Edge aSBE;
            TopoDS_Vertex   VV1 = Context()->CopyVertex(V1);

            TopoDS_Vertex VVp = Vp;
            TopoDS_Vertex VVn = Vn;
            if (Vp.IsSame(Vn))
            {
              // Should modify only one vertex.
              VVp = Context()->CopyVertex(Vp);
              VVn = VVp;
            }
            else
            {
              VVp = Context()->CopyVertex(Vp);
              VVn = Context()->CopyVertex(Vn);
            }

            TopoDS_Edge EE1 = aSBE.CopyReplaceVertices(E1, VVp, VV1);
            TopoDS_Edge EE2 = aSBE.CopyReplaceVertices(E2, VV1, VVn);

            Context()->Replace(E1, EE1);
            Context()->Replace(E2, EE2);

            UpdateWire();
            E1 = sbwd->Edge(n1);
            E2 = sbwd->Edge(n2);
            Vp = sae.FirstVertex(E1);
            V1 = sae.LastVertex(E1);
            V2 = sae.FirstVertex(E2);
            Vn = sae.LastVertex(E2);
          }

          B.UpdateEdge(E1, 1.000001 * te1);
          B.UpdateVertex(sae.FirstVertex(E1), 1.000001 * te1);
          B.UpdateVertex(sae.LastVertex(E1), 1.000001 * te1);
          B.UpdateEdge(E2, 1.000001 * te2);
          B.UpdateVertex(sae.FirstVertex(E2), 1.000001 * te2);
          B.UpdateVertex(sae.LastVertex(E2), 1.000001 * te2);

          myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE6);
          locMayEdit = false;
        }
        newtol = 1.000001 * maxte;
      }
    }

    if (locMayEdit || newtol <= MaxTolerance())
    {
      prevRange1 = newRange1;
      prevRange2 = newRange2;
      if (locMayEdit)
      {
        newtol = 1.0001 * (pnt.Distance(pint) + rad);
        //: j6 abv 7 Dec 98: ProSTEP TR10 r0601_id.stp #57676 & #58586: do not cut edges because of
        //: influence on adjacent faces
        ShapeFix_SplitTool aTool;

        if (!aTool.CutEdge(E1, (isForward1 ? a1 : b1), param1, Face(), IsCutLine))
        {
          if (V1.IsSame(Vp))
            myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
          else
            locMayEdit = false;
        }
        else
          cutEdge1 = true; //: h4

        if (!aTool.CutEdge(E2, (isForward2 ? b2 : a2), param2, Face(), IsCutLine))
        {
          if (V2.IsSame(Vn))
            myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
          else
            locMayEdit = false;
        }
        else
          cutEdge2 = true; //: h4
      }

      if (locMayEdit && newRange1 <= prevRange1 && newRange2 <= prevRange2 && // rln 09/01/98
          BRep_Tool::SameParameter(E1) && BRep_Tool::SameParameter(E2))
      {
        myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
        pnt = pint;
        if (tol <= rad)
        {
          myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
          tol = 1.001 * rad;
        }
      }
      else
      {
        if (IsCutLine)
        {
          myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
          pnt = pint;
          if (tol <= rad)
          {
            myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
            tol = 1.001 * rad;
          }
        }
        else
        { // else increase tolerance
          if (tol < newtol)
          { // rln 07.04.99 CCI60005-brep.igs
            myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
            tol = newtol;
          }
        }
      }
    }
    else
    {
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    }
  }

  if (!LastFixStatus(ShapeExtend_DONE))
    return false;

  if (isChangedEdge)
  {
    B.UpdateVertex(V1, pnt, tol);
    B.UpdateVertex(V2, pnt, tol);
  }
  else
  {
    if (!Context().IsNull())
    {
      if (V1.IsSame(V2))
      {
        Context()->CopyVertex(V1, pnt, tol);
      }
      else
      {
        Context()->CopyVertex(V1, pnt, tol);
        Context()->CopyVertex(V2, pnt, tol);
      }
    }
    else
    {
      B.UpdateVertex(V1, pnt, tol);
      B.UpdateVertex(V2, pnt, tol);
    }
  }

  //: h4: make edges SP (after all cuts: t4mug.stp #3730+#6460)
  if (cutEdge1)
  {
    if (!Context().IsNull())
      E1 = TopoDS::Edge(Context()->Apply(E1));
    myFixEdge->FixSameParameter(E1);
  }
  if (cutEdge2 && !IsCutLine)
  {
    if (!Context().IsNull())
      E2 = TopoDS::Edge(Context()->Apply(E2));
    myFixEdge->FixSameParameter(E2);
  }
  if (cutEdge1 || cutEdge2)
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE7);
  }
  if (!myShape.IsNull())
  {
    // clang-format off
    SendWarning ( Message_Msg ( "FixAdvWire.FixIntersection.MSG10" ) );// Edges were intersecting, corrected
    // clang-format on
  }
  return true;
}

//=================================================================================================

// pdn 17.03.99 fixing non adjacent intersection by increasing tolerance of vertex

bool ShapeFix_Wire::FixIntersectingEdges(const int num1, const int num2)
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;
  NCollection_Sequence<IntRes2d_IntersectionPoint> points2d;
  NCollection_Sequence<gp_Pnt>                     points3d;
  NCollection_Sequence<double>                     errors;
  occ::handle<ShapeAnalysis_Wire>                  theAdvAnalyzer = myAnalyzer;
  if (theAdvAnalyzer.IsNull())
    return false;
  theAdvAnalyzer->CheckIntersectingEdges(num1, num2, points2d, points3d, errors);
  if (theAdvAnalyzer->LastCheckStatus(ShapeExtend_FAIL))
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  }
  if (!theAdvAnalyzer->LastCheckStatus(ShapeExtend_DONE))
    return false;
  NCollection_Array1<gp_Pnt>       vertexPoints(1, 4);
  NCollection_Array1<double>       vertexTolers(1, 4);
  NCollection_Array1<double>       newTolers(1, 4);
  NCollection_Array1<TopoDS_Shape> vertices(1, 4);
  newTolers.Init(0);

  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  int                               n2   = (num1 > 0 ? num1 : sbwd->NbEdges());
  int                               n1   = (num2 > 1 ? num2 : sbwd->NbEdges());
  if (n1 == n2)
    return false;

  TopoDS_Edge edge1 = sbwd->Edge(n1);
  TopoDS_Edge edge2 = sbwd->Edge(n2);

  ShapeAnalysis_Edge sae;
  vertices(1) = sae.FirstVertex(edge1);
  vertices(2) = sae.LastVertex(edge1);
  vertices(3) = sae.FirstVertex(edge2);
  vertices(4) = sae.LastVertex(edge2);

  int i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= 4; i++)
  {
    vertexPoints(i) = BRep_Tool::Pnt(TopoDS::Vertex(vertices(i)));
    vertexTolers(i) = BRep_Tool::Tolerance(TopoDS::Vertex(vertices(i)));
  }

  double aNewTolEdge1 = 0.0, aNewTolEdge2 = 0.0;
  int    nb = points3d.Length();
  for (i = 1; i <= nb; i++)
  {
    gp_Pnt pint = points3d.Value(i);

    // searching for the nearest vertexies to the intersection point
    double aVtx1Param = 0., aVtx2Param = 0.;
    int    aVC1, aVC2;
    double aMinDist = RealLast();
    gp_Pnt aNearestVertex;
    double aNecessaryVtxTole = 0.0;
    for (aVC1 = 1; aVC1 <= 2; aVC1++)
    {
      for (aVC2 = 3; aVC2 <= 4; aVC2++)
      {

        double aVtxIPDist  = pint.Distance(vertexPoints(aVC1));
        double aVtxVtxDist = vertexPoints(aVC1).Distance(vertexPoints(aVC2));
        if (aMinDist > aVtxIPDist && aVtxIPDist > aVtxVtxDist)
        {
          aNecessaryVtxTole = aVtxVtxDist;
          aNearestVertex    = vertexPoints(aVC1);
          aMinDist          = aVtxIPDist;
          aVtx1Param        = BRep_Tool::Parameter(TopoDS::Vertex(vertices(aVC1)), edge1);
          aVtx2Param        = BRep_Tool::Parameter(TopoDS::Vertex(vertices(aVC2)), edge2);
        }
      }
    }

    // calculation of necessary tolerances of edges
    const IntRes2d_IntersectionPoint& IP     = points2d.Value(i);
    double                            param1 = IP.ParamOnFirst();
    double                            param2 = IP.ParamOnSecond();
    occ::handle<Geom_Curve>           aCurve1, aCurve2;
    double                            f, l;
    TopLoc_Location                   L1, L2;
    aCurve1 = BRep_Tool::Curve(edge1, L1, f, l);
    aCurve2 = BRep_Tool::Curve(edge2, L2, f, l);

    // if aMinDist lower than resolution than the intersection point lyes inside the vertex
    if (aMinDist < gp::Resolution())
      continue;

    double aMaxEdgeTol1 = 0.0, aMaxEdgeTol2 = 0.0;
    if (aMinDist < RealLast() && !aCurve1.IsNull() && !aCurve2.IsNull())
    {
      gp_Lin aLig(aNearestVertex, gp_Vec(aNearestVertex, pint));
      int    aPointsC;
      double du1   = 0.05 * (param1 - aVtx1Param);
      double du2   = 0.05 * (param2 - aVtx2Param);
      double tole1 = BRep_Tool::Tolerance(edge1);
      double tole2 = BRep_Tool::Tolerance(edge2);
      for (aPointsC = 2; aPointsC < 19; aPointsC++)
      {
        double u  = aVtx1Param + aPointsC * du1;
        gp_Pnt P1 = aCurve1->Value(u);
        P1.Transform(L1.Transformation());
        double d1 = aLig.Distance(P1) * 2.0000001;
        if (d1 > tole1 && d1 > aMaxEdgeTol1)
          aMaxEdgeTol1 = d1;

        u         = aVtx2Param + aPointsC * du2;
        gp_Pnt P2 = aCurve2->Value(u);
        P2.Transform(L2.Transformation());
        double d2 = aLig.Distance(P2) * 2.0000001;
        if (d2 > tole2 && d2 > aMaxEdgeTol2)
          aMaxEdgeTol2 = d2;
      }
      if (aMaxEdgeTol1 == 0.0 && aMaxEdgeTol2 == 0.0)
        continue;
      // if the vertexies are far than tolerances so
      // we do not need to increase edge tolerance
      if (aNecessaryVtxTole > std::max(aMaxEdgeTol1, tole1)
          || aNecessaryVtxTole > std::max(aMaxEdgeTol2, tole2))
      {
        aMaxEdgeTol1 = 0.0;
        aMaxEdgeTol2 = 0.0;
      }
    }

    double rad    = errors.Value(i);
    double finTol = RealLast();
    int    rank   = 1;
    for (int j = 1; j <= 4; j++)
    {
      double newtol = 1.0001 * (pint.Distance(vertexPoints(j)) + rad);
      if (newtol < finTol)
      {
        rank   = j;
        finTol = newtol;
      }
    }
    if (finTol <= MaxTolerance())
    {
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      if (newTolers(rank) < finTol)
      {
        if (std::max(aMaxEdgeTol1, aMaxEdgeTol2) < finTol && (aMaxEdgeTol1 > 0 || aMaxEdgeTol2 > 0))
        {
          aNewTolEdge1 = std::max(aNewTolEdge1, aMaxEdgeTol1);
          aNewTolEdge2 = std::max(aNewTolEdge2, aMaxEdgeTol2);
        }
        else
        {
          newTolers(rank) = finTol;
        }
      }
    }
    else
    {
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    }
  }

  BRep_Builder B;
  // update of tolerances of edges
  if (aNewTolEdge1 > 0)
  {
    for (i = 1; i <= 2; i++)
      if (aNewTolEdge1 > std::max(vertexTolers(i), newTolers(i)))
        newTolers(i) = aNewTolEdge1;
    B.UpdateEdge(edge1, aNewTolEdge1);
  }
  if (aNewTolEdge2 > 0)
  {
    for (i = 3; i <= 4; i++)
      if (aNewTolEdge2 > std::max(vertexTolers(i), newTolers(i)))
        newTolers(i) = aNewTolEdge2;
    B.UpdateEdge(edge2, aNewTolEdge2);
  }

  // update of tolerances of vertexies
  for (i = 1; i <= 4; i++)
    if (newTolers(i) > 0)
      B.UpdateVertex(TopoDS::Vertex(vertices(i)), newTolers(i));

  if (!myShape.IsNull())
  {
    // clang-format off
    SendWarning ( Message_Msg ( "FixAdvWire.FixIntersection.MSG10" ) );// Edges were intersecting, corrected
    // clang-format on
  }
  return true;
}

//=======================================================================
// function : FixLacking
// purpose  : Test if two adjucent edges are disconnected in 2d (while connected
//           in 3d), and in that case either increase tolerance of the vertex or
//           add a new edge (straight in 2d space), in order to close wire in 2d.
//           Returns True if edge was added or tolerance was increased.
// NOTE     : Is to be run after FixDegenerated
// Algorithm: 1. Compute the 2d gap between edges and calculate a tolerance
//              which should have vertex in order to comprise the gap
//              (using GeomAdaptor_Surface); computed value is inctol
//           2. If inctol < tol of vertex, return False (everything is OK)
//           3. If inctol < Precision, just increase tolerance of vertex to inctol
//           4. Else (if both edges are not degenerated) try to add new edge
//              with straight pcurve (in order to close the gap):
//              a) if flag MayEdit is False
//                 1. if inctol < MaxTolerance, increase tolerance of vertex to inctol
//                 2. else try to add degenerated edge (check that middle point of
//                    that pcurveis inside the vertex)
//              b) if MayEdit is True
//                 1. try to replace big vertex with two new small vertices
//                    connected by new edge. This is made if there is a 3d space
//                    between ends of adjacent edges.
//                 2. if inctol < MaxTolerance, increase tolerance of vertex to inctol
//                 3. else add either degenerated or closed edge (if middle point
//                    of a pcurve of a new edge is inside the vertex, then
//		      degenerated edge is added, else new edge is closed).
//           5. If new edge cannot be added, but inctol < MaxTolerance,
//              when increase tolerance of vertex to a value of inctol
// Short list of some internal variables:
// tol    - tolerance of vertex
// tol2d  - tolerance in parametric space of the surface corresponding to 2*tol
// dist2d - distance between ends of pcurves of edges (2d)
// inctol - tolerance required for vertex to close 2d gap (=tol*dist2d/tol2d)
// tol1, tol2 - tolerances of edges, tol0 = tol1 + tol2
// p3d1, p3d2 - ends of 3d curves of edges
//=======================================================================
//: h2 abv 28 May 98: merged modifications by abv 22 Apr 98, gka 27 May 98
// and pdn 25 May 98 concerning lacking closed or degenerated edges
// Example files: r0501_pe #107813, UKI60107-6 250, UKI60107-3 1577.

//: s2 abv 21 Apr 99: add functionality for bending pcurve
static bool TryBendingPCurve(const TopoDS_Edge&         E,
                             const TopoDS_Face&         face,
                             const gp_Pnt2d             p2d,
                             const bool                 end,
                             occ::handle<Geom2d_Curve>& c2d,
                             double&                    first,
                             double&                    last,
                             double&                    tol)
{
  ShapeAnalysis_Edge sae;
  if (!sae.PCurve(E, face, c2d, first, last, false))
    return false;

  {
    try
    {
      OCC_CATCH_SIGNALS
      occ::handle<Geom2d_BSplineCurve> bs;
      if (c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
        bs = occ::down_cast<Geom2d_BSplineCurve>(c2d->Copy());
      else // if ( c2d->IsKind(STANDARD_TYPE(Geom2d_Line)) )
      {
        occ::handle<Geom2d_TrimmedCurve> trim = new Geom2d_TrimmedCurve(c2d, first, last);
        bs                                    = Geom2dConvert::CurveToBSplineCurve(trim);
      }
      if (bs.IsNull())
        return false;

      double par = (end ? last : first);
      if (fabs(bs->FirstParameter() - par) < ::Precision::PConfusion()
          && bs->Multiplicity(1) > bs->Degree())
        bs->SetPole(1, p2d);
      else if (fabs(bs->LastParameter() - par) < ::Precision::PConfusion()
               && bs->Multiplicity(bs->NbKnots()) > bs->Degree())
        bs->SetPole(bs->NbPoles(), p2d);
      else
      {
        bs->Segment(first, last);
        if (fabs(bs->FirstParameter() - par) < ::Precision::PConfusion()
            && bs->Multiplicity(1) > bs->Degree())
          bs->SetPole(1, p2d);
        else if (fabs(bs->LastParameter() - par) < ::Precision::PConfusion()
                 && bs->Multiplicity(bs->NbKnots()) > bs->Degree())
          bs->SetPole(bs->NbPoles(), p2d);
        else
          return false;
      }
      c2d = bs;

      if (!TryNewPCurve(E, face, c2d, first, last, tol))
        return false;
    }
    catch (Standard_Failure const&)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeFix_Wire::FixLacking: Exception in Geom2d_BSplineCurve::Segment()"
                << std::endl;
#endif
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool ShapeFix_Wire::FixLacking(const int num, const bool force)
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  //=============
  // First phase: analysis whether the problem (gap) exists
  gp_Pnt2d p2d1, p2d2;
  myAnalyzer->CheckLacking(num, (force ? Precision() : 0.), p2d1, p2d2);
  if (myAnalyzer->LastCheckStatus(ShapeExtend_FAIL))
  {
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  }
  if (!myAnalyzer->LastCheckStatus(ShapeExtend_DONE))
    return false;

  //=============
  // Second phase: collection of data necessary for further analysis

  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  int                               n2   = (num > 0 ? num : sbwd->NbEdges());
  int                               n1   = (n2 > 1 ? n2 - 1 : sbwd->NbEdges());
  TopoDS_Edge                       E1   = sbwd->Edge(n1);
  TopoDS_Edge                       E2   = sbwd->Edge(n2);

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      V1  = sae.LastVertex(E1);
  TopoDS_Vertex      V2  = sae.FirstVertex(E2);
  double             tol = std::max(BRep_Tool::Tolerance(V1), BRep_Tool::Tolerance(V2));

  double Prec   = Precision();
  double dist2d = myAnalyzer->MaxDistance2d();
  double inctol = myAnalyzer->MaxDistance3d();

  TopoDS_Face                        face = myAnalyzer->Face();
  occ::handle<ShapeAnalysis_Surface> surf = myAnalyzer->Surface();

  gp_Pnt p3d1, p3d2;
  double tol1 = ::Precision::Confusion(), tol2 = ::Precision::Confusion(); // SK

  //=============
  //: s2 abv 21 Apr 99: Speculation: try bending pcurves
  double                    bendtol1 = 0., bendtol2 = 0.;
  occ::handle<Geom2d_Curve> bendc1, bendc2;
  double                    bendf1 = 0., bendl1 = 0., bendf2 = 0., bendl2 = 0.;
  if (myGeomMode && !BRep_Tool::IsClosed(E1, face) && !BRep_Tool::IsClosed(E2, face))
  {
    gp_Pnt2d p2d = 0.5 * (p2d1.XY() + p2d2.XY());
    bool     ok1 = TryBendingPCurve(E1,
                                face,
                                p2d,
                                E1.Orientation() == TopAbs_FORWARD,
                                bendc1,
                                bendf1,
                                bendl1,
                                bendtol1);
    bool     ok2 = TryBendingPCurve(E2,
                                face,
                                p2d,
                                E2.Orientation() == TopAbs_REVERSED,
                                bendc2,
                                bendf2,
                                bendl2,
                                bendtol2);
    if (ok1 && !ok2)
    {
      bendtol2 = BRep_Tool::Tolerance(E2);
      ok1      = TryBendingPCurve(E1,
                             face,
                             p2d2,
                             E1.Orientation() == TopAbs_FORWARD,
                             bendc1,
                             bendf1,
                             bendl1,
                             bendtol1);
    }
    else if (!ok1 && ok2)
    {
      bendtol1 = BRep_Tool::Tolerance(E1);
      ok2      = TryBendingPCurve(E2,
                             face,
                             p2d1,
                             E2.Orientation() == TopAbs_FORWARD,
                             bendc2,
                             bendf2,
                             bendl2,
                             bendtol2);
    }
    if (!ok1 && !ok2)
      bendc1.Nullify();
  }

  //=============
  // Third phase: analyse how to fix the problem

  // selector of solutions
  bool doIncrease  = false; // increase tolerance
  bool doAddLong   = false; // add long 3d edge in replacement of a vertex
  bool doAddClosed = false; // add closed 3d edge
  bool doAddDegen  = false; // add degenerated edge
  bool doBend      = false; //: s2 bend pcurves

  // if bending is OK with existing tolerances of edges, take it
  if (!bendc1.IsNull() && !bendc2.IsNull()
      && ((bendtol1 < BRep_Tool::Tolerance(E1) && bendtol2 < BRep_Tool::Tolerance(E2))
          || (inctol < Prec && bendtol1 < inctol && bendtol2 < inctol)))
    doBend = true;

  // is it OK just to increase tolerance (to a value less than preci)?
  else if (inctol < Prec)
    doIncrease = true;

  // If increase is not OK or force, try to find other solutions (adding edge)
  else if (!BRep_Tool::Degenerated(E2) && !BRep_Tool::Degenerated(E1))
  {

    // analyze the 3d space btw edges: is it enough to add long 3d edge?
    if (myTopoMode)
    {
      occ::handle<Geom_Curve> c3d;
      double                  a, b;
      if (!sae.Curve3d(E1, c3d, a, b, true))
      { // cannot work
        myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
        return false;
      }
      p3d1             = c3d->Value(b);
      double dist2d3d1 = p3d1.Distance(surf->Value(p2d1));
      if (!sae.Curve3d(E2, c3d, a, b, true))
      { // cannot work
        myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
        return false;
      }
      p3d2             = c3d->Value(a);
      double dist2d3d2 = p3d2.Distance(surf->Value(p2d2));

      tol1 = std::max(BRep_Tool::Tolerance(E1), dist2d3d1);
      tol2 = std::max(BRep_Tool::Tolerance(E2), dist2d3d2);
      //: c5  double tol0 = Max ( tol1 + tol2, thepreci );
      double tol0    = tol1 + tol2; //: c5 abv 26 Feb 98: CTS17806 #44418
      double dist3d2 = p3d1.SquareDistance(p3d2);

      // is it OK to add a long 3d edge?
      if (!myAnalyzer->LastCheckStatus(ShapeExtend_DONE2)
          && //: 81 abv 20 Jan 98: don`t add back-going edges (zigzags)
          dist3d2 > 1.25 * tol0 * tol0
          && (force || dist3d2 > Prec * Prec || inctol > MaxTolerance()))
      {
        doAddLong = true;
      }
    }

    //: h6 abv 25 Jun 98: BUC40132 6361: try to increase tol up to MaxTol if not add
    if (!doAddLong && inctol < MaxTolerance()
        && !myAnalyzer->Surface()->IsDegenerated(p2d1, p2d2, 2. * tol, 10.))
    { //: p7
      if (!bendc1.IsNull() && !bendc2.IsNull() && bendtol1 < inctol && bendtol2 < inctol)
        doBend = true;
      else
        doIncrease = true;
    }
    else

      // else try to add either degenerated or closed edge
      if (!doAddLong)
      {
        gp_Pnt pV = 0.5 * (BRep_Tool::Pnt(V1).XYZ() + BRep_Tool::Pnt(V2).XYZ());
        gp_Pnt pm = myAnalyzer->Surface()->Value(0.5 * (p2d1.XY() + p2d2.XY()));

        double dist = pV.Distance(pm);
        if (dist <= tol)
          doAddDegen = true;
        else if (myTopoMode)
          doAddClosed = true;
        else if (dist <= MaxTolerance())
        { //: r7 abv 12 Apr 99: t3d_opt.stp #14245 after S4136
          doAddDegen = true;
          doIncrease = true;
          inctol     = dist;
        }
      }
  }

  else if (!BRep_Tool::Degenerated(E2) && BRep_Tool::Degenerated(E1))
  {
    // create new degenerated edge and replace E1 to new edge
  }
  else if (BRep_Tool::Degenerated(E2) && !BRep_Tool::Degenerated(E1))
  {
    // create new degenerated edge and replace E2 to new edge
  }

  //=============
  // Third phase - do the fixes
  BRep_Builder B;

  // add edge
  if (doAddLong || doAddDegen || doAddClosed)
  {

    // construct new vertices
    TopoDS_Vertex newV1, newV2;
    if (doAddLong)
    {
      newV1 = BRepBuilderAPI_MakeVertex(p3d1);
      newV1.Reverse();
      newV2 = BRepBuilderAPI_MakeVertex(p3d2);
      B.UpdateVertex(newV1, 1.001 * tol1);
      B.UpdateVertex(newV2, 1.001 * tol2);
    }
    else
    {
      newV1 = V1;
      newV2 = V2;
    }

    // prepare new edge
    TopoDS_Edge edge;
    B.MakeEdge(edge);
    if (doAddDegen)
      B.Degenerated(edge, true); // sln: do it before adding curve
    gp_Vec2d                 v12(p2d1, p2d2);
    occ::handle<Geom2d_Line> theLine2d = new Geom2d_Line(p2d1, gp_Dir2d(v12));
    B.UpdateEdge(edge, theLine2d, face, ::Precision::Confusion());
    B.Range(edge, face, 0, dist2d);
    B.Add(edge, newV1.Oriented(TopAbs_FORWARD));
    B.Add(edge, newV2.Oriented(TopAbs_REVERSED));
    ShapeBuild_Edge sbe;
    if (!doAddDegen && !sbe.BuildCurve3d(edge))
    {
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
      return false;
    }

    // if long edge is added, replace vertices of adjacent edges
    if (doAddLong)
    {

      // replace 1st edge (n1==n2 - special case: wire consists of one edge)
      TopoDS_Edge edge1 = sbe.CopyReplaceVertices(E1, (n1 == n2 ? newV2 : TopoDS_Vertex()), newV1);
      sbwd->Set(edge1, n1);
      if (!Context().IsNull())
      {
        Context()->Replace(E1, edge1);
        // actually, this will occur only in context of single face
        // hence, recording to ReShape is rather for tracking modifications
        // than for keeping sharing
        Context()->Replace(V1, newV1.Oriented(V1.Orientation()));
        if (!V1.IsSame(V2))
        {
          Context()->Replace(V2, newV2.Oriented(V2.Orientation()));
        }
      }
      // replace 2nd edge
      if (n1 != n2)
      {
        TopoDS_Edge edge2 = sbe.CopyReplaceVertices(E2, newV2, TopoDS_Vertex());
        sbwd->Set(edge2, n2);
        if (!Context().IsNull())
          Context()->Replace(E2, edge2);
      }
      if (!Context().IsNull())
        UpdateWire();
    }

    // insert new edge
    if (doAddDegen)
    {
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeFix_Wire::FixLacking: degenerated edge added" << std::endl;
#endif
    }
    else if (!doAddLong)
    {
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
    }
    sbwd->Add(edge, n2);
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
  }

  // else try to increase tol up to MaxTol
  else if (inctol > tol && inctol < MaxTolerance())
  {
    if (!bendc1.IsNull() && !bendc2.IsNull() && bendtol1 < inctol && bendtol2 < inctol)
      doBend = true;
    else
      doIncrease = true;
  }

  // bend pcurves
  if (doBend)
  { //: s2 abv 21 Apr 99
    B.UpdateEdge(E1, bendc1, face, bendtol1);
    B.Range(E1, face, bendf1, bendl1);
    B.UpdateEdge(E2, bendc2, face, bendtol2);
    B.Range(E2, face, bendf2, bendl2);
    B.UpdateVertex(sae.FirstVertex(E1), bendtol1);
    B.UpdateVertex(sae.LastVertex(E1), bendtol1);
    B.UpdateVertex(sae.FirstVertex(E2), bendtol2);
    B.UpdateVertex(sae.LastVertex(E2), bendtol2);
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
    //: s3 abv 22 Apr 99: PRO7187 #11534: self-intersection not detected unitil curve is bent (!)
    FixSelfIntersectingEdge(n1);
    FixSelfIntersectingEdge(n2);
    FixIntersectingEdges(n2); // skl 24.04.2003 for OCC58
#ifdef OCCT_DEBUG
    std::cout << "Info: ShapeFix_Wire::FixLacking: Bending pcurves" << std::endl;
#endif
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
  }

  // increase vertex tolerance
  if (doIncrease)
  {
    B.UpdateVertex(V1, 1.001 * inctol);
    B.UpdateVertex(V2, 1.001 * inctol);
    myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  }

  if (LastFixStatus(ShapeExtend_DONE))
    return true;

  myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
  return false;
}

//=================================================================================================

bool ShapeFix_Wire::FixNotchedEdges()
{
  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  occ::handle<ShapeAnalysis_Wire> theAdvAnalyzer = myAnalyzer;
  TopoDS_Face                     face           = Face();
  if (!Context().IsNull())
    UpdateWire();
  occ::handle<ShapeExtend_WireData> sewd = WireData();

  for (int i = 1; i <= NbEdges() && NbEdges() > 2; i++)
  {
    double param;
    int    toRemove;
    if (theAdvAnalyzer->CheckNotchedEdges(i, toRemove, param, MinTolerance()))
    {
      int                       n2            = (i > 0) ? i : NbEdges();
      int                       n1            = (n2 > 1) ? n2 - 1 : NbEdges();
      bool                      isRemoveFirst = (n1 == toRemove);
      int                       toSplit       = (n2 == toRemove ? n1 : n2);
      TopoDS_Edge               splitE        = sewd->Edge(toSplit);
      ShapeAnalysis_Edge        sae;
      occ::handle<Geom2d_Curve> c2d;
      double                    a, b;
      sae.PCurve(splitE, face, c2d, a, b, true);
      ShapeBuild_Edge    sbe;
      TopAbs_Orientation orient = splitE.Orientation();

      // check whether the whole edges should be removed - this is the case
      // when split point coincides with the end of the edge;
      // for closed edges split point may fall at the other end (see issue #0029780)
      if (std::abs(param - (isRemoveFirst ? b : a)) <= ::Precision::PConfusion()
          || (sae.IsClosed3d(splitE)
              && std::abs(param - (isRemoveFirst ? a : b)) <= ::Precision::PConfusion()))
      {
        FixDummySeam(n1);
        // The seam edge is removed from the list. So, need to step back to avoid missing of edge
        // processing
        i--;
      }
      else // perform splitting of the edge and adding to wire
      {
        // pdn check if it is necessary
        if (std::abs((isRemoveFirst ? a : b) - param) < ::Precision::PConfusion())
        {
          continue;
        }

        occ::handle<ShapeAnalysis_TransferParametersProj> transferParameters =
          new ShapeAnalysis_TransferParametersProj;
        transferParameters->SetMaxTolerance(MaxTolerance());
        transferParameters->Init(splitE, face);
        double first, last;
        if (a < b)
        {
          first = a;
          last  = b;
        }
        else
        {
          first = b;
          last  = a;
        }
        TopoDS_Vertex Vnew;
        BRep_Builder  B;
        B.MakeVertex(Vnew,
                     Analyzer()->Surface()->Value(c2d->Value(param)),
                     ::Precision::Confusion());
        TopoDS_Edge wE = splitE;
        wE.Orientation(TopAbs_FORWARD);
        TopoDS_Shape aTmpShape = Vnew.Oriented(TopAbs_REVERSED); // for porting
        TopoDS_Edge  newE1 =
          sbe.CopyReplaceVertices(wE, sae.FirstVertex(wE), TopoDS::Vertex(aTmpShape));
        sbe.CopyPCurves(newE1, wE);
        transferParameters->TransferRange(newE1, first, param, true);
        B.SameRange(newE1, false);
        B.SameParameter(newE1, false);
        aTmpShape = Vnew.Oriented(TopAbs_FORWARD);
        TopoDS_Edge newE2 =
          sbe.CopyReplaceVertices(wE, TopoDS::Vertex(aTmpShape), sae.LastVertex(wE));
        sbe.CopyPCurves(newE2, wE);
        transferParameters->TransferRange(newE2, param, last, true);
        B.SameRange(newE2, false);
        B.SameParameter(newE2, false);

        if (!Context().IsNull())
        {
          TopoDS_Wire wire;
          B.MakeWire(wire);
          B.Add(wire, newE1);
          B.Add(wire, newE2);
          Context()->Replace(wE, wire);
        }

        newE1.Orientation(orient);
        newE2.Orientation(orient);
        if (orient == TopAbs_REVERSED)
        {
          TopoDS_Edge tmp = newE2;
          newE2           = newE1;
          newE1           = tmp;
        }

        bool isRemoveLast = ((n1 == NbEdges()) && (n2 == 1));
        sewd->Set(newE1, toSplit);
        sewd->Add(newE2, (toSplit == NbEdges() ? 0 : toSplit + 1));

        FixDummySeam(isRemoveLast ? NbEdges() : toRemove);
        myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
      }

      i--;
      if (!Context().IsNull()) // skl 07.03.2002 for OCC180
        UpdateWire();
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    }
  }
  myStatusNotches = myLastFixStatus;
  return LastFixStatus(ShapeExtend_DONE);
}

//=================================================================================================

static void CopyReversePcurves(const TopoDS_Edge& toedge,
                               const TopoDS_Edge& fromedge,
                               const bool         reverse)
{
  const TopLoc_Location& fromLoc = fromedge.Location();
  const TopLoc_Location& toLoc   = toedge.Location();
  for (NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator fromitcr(
         (*((occ::handle<BRep_TEdge>*)&fromedge.TShape()))->ChangeCurves());
       fromitcr.More();
       fromitcr.Next())
  {
    occ::handle<BRep_GCurve> fromGC = occ::down_cast<BRep_GCurve>(fromitcr.Value());
    if (fromGC.IsNull())
      continue;
    if (fromGC->IsCurveOnSurface())
    {
      occ::handle<Geom_Surface>                                surface = fromGC->Surface();
      TopLoc_Location                                          L       = fromGC->Location();
      bool                                                     found   = false;
      NCollection_List<occ::handle<BRep_CurveRepresentation>>& tolist =
        (*((occ::handle<BRep_TEdge>*)&toedge.TShape()))->ChangeCurves();
      occ::handle<BRep_GCurve> toGC;
      for (NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator toitcr(tolist);
           toitcr.More() && !found;
           toitcr.Next())
      {
        toGC = occ::down_cast<BRep_GCurve>(toitcr.Value());
        if (toGC.IsNull() || !toGC->IsCurveOnSurface() || surface != toGC->Surface()
            || L != toGC->Location())
          continue;
        found = true;
        break;
      }
      if (!found)
      {
        double fp = fromGC->First();
        double lp = fromGC->Last();
        toGC      = occ::down_cast<BRep_GCurve>(fromGC->Copy());
        tolist.Append(toGC);
        occ::handle<Geom2d_Curve> pcurve = occ::down_cast<Geom2d_Curve>(fromGC->PCurve()->Copy());
        if (reverse)
        {
          fp = pcurve->ReversedParameter(fp);
          lp = pcurve->ReversedParameter(lp);
          pcurve->Reverse();
          double tmp = fp;
          fp         = lp;
          lp         = tmp;
        }
        // bug OCC209 invalid location of pcurve in the edge after copying
        TopLoc_Location newLoc = (fromLoc * L).Predivided(toLoc);
        toGC->SetRange(fp, lp);
        toGC->PCurve(pcurve);
        toGC->Location(newLoc);
        if (fromGC->IsCurveOnClosedSurface())
        {
          pcurve = fromGC->PCurve2();
          toGC->PCurve2(occ::down_cast<Geom2d_Curve>(pcurve->Copy()));
        }
      }
    }
  }
}

//=================================================================================================

//  Note:    This function temporarily not used, because adress to it in
//           function FixDummySeam() (see below line 2472) not used too
//
// static bool HasNewPCurves(const TopoDS_Edge& toedge,
//				      const TopoDS_Edge& fromedge)
//
//{
//  for (NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator fromitcr
//       ((*((occ::handle<BRep_TEdge>*)&fromedge.TShape()))->ChangeCurves()); fromitcr.More();
//       fromitcr.Next()) {
//    occ::handle<BRep_GCurve> fromGC = occ::down_cast<BRep_GCurve>(fromitcr.Value());
//    if ( fromGC.IsNull() ) continue;
//    if ( fromGC->IsCurveOnSurface() ) {
//      occ::handle<Geom_Surface> surface = fromGC->Surface();
//      TopLoc_Location L = fromGC->Location();
//      bool found = false;
//      NCollection_List<occ::handle<BRep_CurveRepresentation>>& tolist =
//      (*((occ::handle<BRep_TEdge>*)&toedge.TShape()))->ChangeCurves(); occ::handle<BRep_GCurve>
//      toGC; for (NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator toitcr
//      (tolist); toitcr.More() && !found; toitcr.Next()) {
//	toGC = occ::down_cast<BRep_GCurve>(toitcr.Value());
//	if ( toGC.IsNull() || !toGC->IsCurveOnSurface() ||
//	    surface != toGC->Surface() || L != toGC->Location() )  continue;
//	found = true;
//	break;
//      }
//      if (!found)
//	return true;
//    }
//  }
//  return false;
//}

//=================================================================================================

void ShapeFix_Wire::FixDummySeam(const int num)
{
  ShapeAnalysis_Edge                sae;
  ShapeBuild_Edge                   sbe;
  ShapeBuild_Vertex                 sbv;
  int                               num1 = (num == NbEdges()) ? 1 : num + 1;
  occ::handle<ShapeExtend_WireData> sewd = WireData();
  TopoDS_Edge                       E1 = sewd->Edge(num), E2 = sewd->Edge(num1);
  TopoDS_Vertex                     V1 = sae.FirstVertex(E1), V2 = sae.LastVertex(E2);
  TopoDS_Vertex                     Vm = sbv.CombineVertex(V1, V2, 1.0001);

  // pnd defining if new pcurves exists
  // pdn Temporary not removed
  //  bool toRemove = !(HasNewPCurves(E1,E2)||HasNewPCurves(E2,E1));
  bool toRemove = false;

  // creating new edge with pcurves and new vertex
  TopoDS_Vertex Vs = sae.FirstVertex(E2);
  if (Vs.IsSame(V1) || Vs.IsSame(V2))
    Vs = Vm;
  TopoDS_Edge newEdge = sbe.CopyReplaceVertices(E2, Vs, Vm);
  CopyReversePcurves(newEdge, E1, E1.Orientation() == E2.Orientation());
  BRep_Builder B;
  B.SameRange(newEdge, false);
  B.SameParameter(newEdge, false);

  if (!Context().IsNull())
  {
    if (toRemove)
    {
      Context()->Remove(E2);
      Context()->Remove(E1);
    }
    else
    {
      Context()->Replace(E2, newEdge);
      Context()->Replace(E1, newEdge.Reversed());
    }
    Context()->Replace(V1, Vm.Oriented(V1.Orientation()));
    Context()->Replace(V2, Vm.Oriented(V2.Orientation()));
  }

  int         next  = (num1 == NbEdges()) ? 1 : num1 + 1;
  int         prev  = (num > 1) ? num - 1 : NbEdges();
  TopoDS_Edge prevE = sewd->Edge(prev), nextE = sewd->Edge(next);

  TopoDS_Edge tmpE1 = sbe.CopyReplaceVertices(prevE, TopoDS_Vertex(), Vm);
  sewd->Set(tmpE1, prev);
  if (!Context().IsNull())
    Context()->Replace(prevE, tmpE1);

  tmpE1 = sbe.CopyReplaceVertices(nextE, Vm, TopoDS_Vertex());
  sewd->Set(tmpE1, next);
  if (!Context().IsNull())
    Context()->Replace(nextE, tmpE1);

  // removing edges from wire
  int n1, n2;
  if (num < num1)
  {
    n1 = num;
    n2 = num1;
  }
  else
  {
    n1 = num1;
    n2 = num;
  }
  sewd->Remove(n2);
  sewd->Remove(n1);
}

//=================================================================================================

void ShapeFix_Wire::UpdateWire()
{
  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  // Track already-expanded edges to prevent exponential growth from shared context
  // (especially important for non-orientable surfaces like Moebius strips where
  // edges are shared across faces and accumulated replacements can cascade)
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anExpandedEdges;

  for (int i = 1; i <= sbwd->NbEdges(); i++)
  {
    TopoDS_Edge E = sbwd->Edge(i);
    // Use Value() instead of Apply() to get only the direct replacement
    // without recursively following replacement chains. This prevents
    // exponential edge explosion on Moebius strips where shared edges
    // accumulate cascading replacements across multiple faces.
    // The full chain resolution happens at final shell-level Apply().
    // Note: Now again we are using Apply() because using Value() breaks test de step_2 R7
    TopoDS_Shape S = Context()->Apply(E);
    if (S == E)
      continue;
    // Skip if this edge was already expanded in this call to prevent duplicates
    if (!anExpandedEdges.Add(E))
      continue;
    for (TopExp_Explorer exp(S, TopAbs_EDGE); exp.More(); exp.Next())
      sbwd->Add(exp.Current(), i++);
    sbwd->Remove(i--);
  }
}

//=================================================================================================

bool ShapeFix_Wire::FixTails()
{
  if (myMaxTailWidth < 0 || !IsReady())
  {
    return false;
  }

  myLastFixStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!Context().IsNull())
  {
    UpdateWire();
  }
  occ::handle<ShapeExtend_WireData> aSEWD   = WireData();
  int                               aECount = NbEdges(), aENs[] = {aECount, 1};
  bool                              aCheckAngle = true;
  while (aECount >= 2 && aENs[1] <= aECount)
  {
    const TopoDS_Edge aEs[] = {aSEWD->Edge(aENs[0]), aSEWD->Edge(aENs[1])};
    TopoDS_Edge       aEParts[2][2];
    if (!myAnalyzer->CheckTail(aEs[0],
                               aEs[1],
                               aCheckAngle ? myMaxTailAngleSine : -1,
                               myMaxTailWidth,
                               MaxTolerance(),
                               aEParts[0][0],
                               aEParts[0][1],
                               aEParts[1][0],
                               aEParts[1][1]))
    {
      aENs[0]     = aENs[1]++;
      aCheckAngle = true;
      continue;
    }

    // Provide not less than 1 edge in the result wire.
    int       aSplitCounts[] = {aEParts[0][1].IsNull() ? 0 : 1, aEParts[1][1].IsNull() ? 0 : 1};
    const int aRemoveCount   = (aEParts[0][0].IsNull() ? 0 : 1) + (aEParts[1][0].IsNull() ? 0 : 1);
    if (aECount + aSplitCounts[0] + aSplitCounts[1] < 1 + aRemoveCount)
    {
      aENs[0]     = aENs[1]++;
      aCheckAngle = true;
      continue;
    }

    // Split the edges.
    for (int aEI = 0; aEI < 2; ++aEI)
    {
      if (aSplitCounts[aEI] == 0)
      {
        continue;
      }

      // Replace the edge by the wire of its parts in the shape.
      const TopoDS_Edge aE = aEs[aEI];
      if (!Context().IsNull())
      {
        TopoDS_Wire aEWire;
        BRep_Builder().MakeWire(aEWire);
        BRep_Builder().Add(aEWire, aEParts[aEI][0]);
        BRep_Builder().Add(aEWire, aEParts[aEI][1]);
        TopoDS_Edge aFE = TopoDS::Edge(aE.Oriented(TopAbs_FORWARD));
        Context()->Replace(aFE, aEWire);
      }

      // Replace the edge by its parts in the edge wire.
      const TopAbs_Orientation aOrient = aE.Orientation();
      aEParts[aEI][0].Orientation(aOrient);
      aEParts[aEI][1].Orientation(aOrient);
      const int aFirstPI = (aOrient != TopAbs_REVERSED) ? 0 : 1;
      const int aAdd     = (aEI == 0 || aENs[1] < aENs[0]) ? 0 : aSplitCounts[0];
      aSEWD->Set(aEParts[aEI][aFirstPI], aENs[aEI] + aAdd);
      aSEWD->Add(aEParts[aEI][1 - aFirstPI], aENs[aEI] + 1 + aAdd);
    }

    // Remove the tail.
    if (aRemoveCount == 2)
    {
      aCheckAngle = true;
      FixDummySeam(aENs[0] + aSplitCounts[0] + ((aENs[0] < aENs[1]) ? 0 : aSplitCounts[1]));
      if (!Context().IsNull())
      {
        UpdateWire();
      }
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE);

      if (aSplitCounts[0] + aSplitCounts[1] == 2)
      {
        aENs[0] = aENs[1]++;
        continue;
      }

      if (aSplitCounts[0] == aSplitCounts[1])
      {
        aECount -= 2;
        if (aENs[1] >= 3)
        {
          --aENs[0];
          --aENs[1];
        }
        else
        {
          aENs[0] = aECount;
          aENs[1] = 1;
        }
        aCheckAngle = false;
      }
      else
      {
        --aECount;
        if (aSplitCounts[0] != 0)
        {
          aENs[0] = (aENs[0] <= aECount) ? aENs[0] : aECount;
        }
        else
        {
          if (aENs[1] >= 3)
          {
            --aENs[0];
            --aENs[1];
          }
          else
          {
            aENs[0] = aECount;
            aENs[1] = 1;
          }
        }
      }
    }
    else
    {
      aCheckAngle = false;
      --aECount;
      const int aRI = aEParts[0][0].IsNull() ? 1 : 0;
      if (aSplitCounts[aRI] != 0)
      {
        if (aRI == 0)
        {
          if (aENs[1] >= 3)
          {
            --aENs[0];
            --aENs[1];
          }
          else
          {
            aENs[0] = aECount;
            aENs[1] = 1;
          }
        }
        else
        {
          aENs[0] = (aENs[1] > 1) ? aENs[0] : aECount;
        }
      }
      aSEWD->Remove(aENs[aRI] + ((aRI != 0 || aSplitCounts[0] == 0) ? 0 : 1));
      if (!Context().IsNull())
      {
        Context()->Remove(aEs[aRI].Oriented(TopAbs_FORWARD));
        UpdateWire();
      }
      myLastFixStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE);
    }
  }
  myStatusNotches = myLastFixStatus;
  return ShapeExtend::DecodeStatus(myLastFixStatus, ShapeExtend_DONE);
}
