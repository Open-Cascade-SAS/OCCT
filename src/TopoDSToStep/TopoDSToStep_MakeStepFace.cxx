// Created on: 1994-11-30
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

// Integration to ensure SCCS base integrity
//%pdn 30 Nov 98: TestRally 9 issue on r1001_ec.stp (toruses)
// abv 6 Jan 99: TR10: fix by PDN commented (temporarily) because CATIA do not read DEG_TORUSes
// rln 19.01.99: uncomment %30 pdn for integration into K4L
// szv#4 S4163
// abv 30.11.99: fix %30 pdn changed to produce SurfaceOfRevolution instead of
// DegenerateToroidalSurface

#include <Bnd_Box2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <GeomToStep_MakeSurface.hxx>
#include <Precision.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_GeometricRepresentationContextAndParametricRepresentationContext.hxx>
#include <StepGeom_Pcurve.hxx>
#include <StepGeom_SeamCurve.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepShape_AdvancedFace.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_HArray1OfFaceBound.hxx>
#include <StepShape_Loop.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_MakeStepFace.hxx>
#include <TopoDSToStep_MakeStepWire.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <GeomConvert_Units.hxx>

// Processing of non-manifold topology (ssv; 10.11.2010)
// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------
TopoDSToStep_MakeStepFace::TopoDSToStep_MakeStepFace()
    : myError(TopoDSToStep_FaceOther)
{
  done = Standard_False;
}

TopoDSToStep_MakeStepFace::TopoDSToStep_MakeStepFace(const TopoDS_Face&                    F,
                                                     TopoDSToStep_Tool&                    T,
                                                     const Handle(Transfer_FinderProcess)& FP,
                                                     const StepData_Factors& theLocalFactors)
{
  done = Standard_False;
  Init(F, T, FP, theLocalFactors);
}

// ----------------------------------------------------------------------------
// Method  : Init
// Purpose :
// ----------------------------------------------------------------------------
void TopoDSToStep_MakeStepFace::Init(const TopoDS_Face&                    aFace,
                                     TopoDSToStep_Tool&                    aTool,
                                     const Handle(Transfer_FinderProcess)& FP,
                                     const StepData_Factors&               theLocalFactors)
{
  // --------------------------------------------------------------
  // the face is given with its relative orientation (in the Shell)
  // --------------------------------------------------------------

  // szv#4:S4163:12Mar99 SGI warns
  TopoDS_Shape      sh          = aFace.Oriented(TopAbs_FORWARD);
  const TopoDS_Face ForwardFace = TopoDS::Face(sh);
  aTool.SetCurrentFace(ForwardFace);
  Handle(TransferBRep_ShapeMapper) errShape =
    new TransferBRep_ShapeMapper(aFace); // on ne sait jamais

  // [BEGIN] Processing non-manifold topology (another approach) (ssv; 10.11.2010)
  if (Handle(StepData_StepModel)::DownCast(FP->Model())->InternalParameters.WriteNonmanifold != 0)
  {
    Handle(StepShape_AdvancedFace)   anAF;
    Handle(TransferBRep_ShapeMapper) aSTEPMapper = TransferBRep::ShapeMapper(FP, aFace);
    if (FP->FindTypedTransient(aSTEPMapper, STANDARD_TYPE(StepShape_AdvancedFace), anAF))
    {
      // Non-manifold topology detected
      Handle(StepShape_AdvancedFace) aLinkingAF = new StepShape_AdvancedFace;
      aLinkingAF->Init(anAF->Name(), anAF->Bounds(), anAF->FaceGeometry(), !anAF->SameSense());

      myError  = TopoDSToStep_FaceDone;
      myResult = aLinkingAF;
      done     = Standard_True;
      return;
    }
  }
  // [END] Processing non-manifold topology (ssv; 10.11.2010)

  if (aTool.IsBound(aFace))
  {
    myError  = TopoDSToStep_FaceDone;
    done     = Standard_True;
    myResult = aTool.Find(aFace);
    return;
  }

  if (aFace.Orientation() == TopAbs_INTERNAL || aFace.Orientation() == TopAbs_EXTERNAL)
  {
    FP->AddWarning(errShape, " Face from Non Manifold Topology");
    myError = TopoDSToStep_NonManifoldFace;
    done    = Standard_False;
    return;
  }

  // ------------------
  // Get the Outer Wire
  // ------------------
  const TopoDS_Wire theOuterWire = BRepTools::OuterWire(ForwardFace);
  if (theOuterWire.IsNull())
  {
    FP->AddWarning(errShape, " Face without wire not mapped");
    myError = TopoDSToStep_InfiniteFace;
    done    = Standard_False;
    return;
  }

  try
  {
    // -----------------
    // Translate Surface
    // -----------------
    Handle(Geom_Surface) Su = BRep_Tool::Surface(ForwardFace);
    if (Su.IsNull())
    {
      FP->AddWarning(errShape, " Face without geometry not mapped");
      myError = TopoDSToStep_FaceOther;
      done    = Standard_False;
      return;
    }

    //  CKY  23 SEP 1996 : une FACE de Step n a pas droit a RECTANGULAR_TRIMMED...
    //  Il faut donc d abord "demonter" la RectangularTrimmedSurface pour
    //  passer la Surface de base
    Handle(Geom_RectangularTrimmedSurface) aRTS =
      Handle(Geom_RectangularTrimmedSurface)::DownCast(Su);
    if (!aRTS.IsNull())
      Su = aRTS->BasisSurface();

    // Surfaces with indirect Axes are already reversed
    aTool.SetSurfaceReversed(Standard_False);

    GeomToStep_MakeSurface   MkSurface(Su, theLocalFactors);
    Handle(StepGeom_Surface) Spms = MkSurface.Value();

    //%pdn 30 Nov 98: TestRally 9 issue on r1001_ec.stp:
    // toruses with major_radius < minor are re-coded as degenerate
    // rln 19.01.99: uncomment %30 pdn for integration into K4L
    {
      // If the surface is Offset it is necessary to check the base surface
      Standard_Boolean           aSurfaceIsOffset = Standard_False;
      Handle(Geom_OffsetSurface) anOffsetSu;
      if (Su->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
      {
        aSurfaceIsOffset = Standard_True;
        anOffsetSu       = Handle(Geom_OffsetSurface)::DownCast(Su);
      }
      if ((Spms->IsKind(STANDARD_TYPE(StepGeom_ToroidalSurface)))
          || ((aSurfaceIsOffset)
              && anOffsetSu->BasisSurface()->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))))
      {
        Handle(Geom_ToroidalSurface) TS;
        if (aSurfaceIsOffset)
          TS = Handle(Geom_ToroidalSurface)::DownCast(anOffsetSu->BasisSurface());
        else
          TS = Handle(Geom_ToroidalSurface)::DownCast(Su);
        Standard_Real R = TS->MajorRadius();
        Standard_Real r = TS->MinorRadius();
        if (R < r) // if torus is degenerate or base surface is degenerate, make revolution instead
        {
          gp_Ax3 Ax3 = TS->Position();
          gp_Pnt pos = Ax3.Location();
          gp_Dir dir = Ax3.Direction();
          gp_Dir X   = Ax3.XDirection();
          // create basis curve
          Standard_Real UF, VF, UL, VL;
          ShapeAlgo::AlgoContainer()->GetFaceUVBounds(aFace, UF, UL, VF, VL);
          gp_Ax2             Ax2(pos.XYZ() + X.XYZ() * TS->MajorRadius(), X ^ dir, X);
          Handle(Geom_Curve) BasisCurve = new Geom_Circle(Ax2, TS->MinorRadius());
          // convert basis curve to bspline in order to avoid self-intersecting
          // surface of revolution (necessary e.g. for CATIA)
          if (VL - VF - 2 * M_PI < -Precision::PConfusion())
            BasisCurve =
              ShapeAlgo::AlgoContainer()->ConvertCurveToBSpline(BasisCurve,
                                                                VF,
                                                                VL,
                                                                Precision::Approximation(),
                                                                GeomAbs_C1,
                                                                100,
                                                                9);

          // create surface of revolution
          gp_Ax1 Axis = Ax3.Axis();
          if (!Ax3.Direct())
            Axis.Reverse();
          Handle(Geom_SurfaceOfRevolution) Rev = new Geom_SurfaceOfRevolution(BasisCurve, Axis);

          // and translate it
          if (aSurfaceIsOffset)
          {
            anOffsetSu->SetBasisSurface(Rev);
            GeomToStep_MakeSurface MkRev(anOffsetSu, theLocalFactors);
            Spms = MkRev.Value();
          }
          else
          {
            GeomToStep_MakeSurface MkRev(Rev, theLocalFactors);
            Spms = MkRev.Value();
          }
        }
      }
    }

    // ----------------
    // Translates Wires
    // ----------------
    Handle(StepShape_Loop) Loop;

    TopoDSToStep_MakeStepWire   MkWire;
    TColStd_SequenceOfTransient mySeq;

    // Initialize the Wire Explorer with the forward face
    TopExp_Explorer WireExp;
    for (WireExp.Init(ForwardFace, TopAbs_WIRE); WireExp.More(); WireExp.Next())
    {

      const TopoDS_Wire CurrentWire = TopoDS::Wire(WireExp.Current());
      if (!CurrentWire.IsNull())
      {
        MkWire.Init(CurrentWire, aTool, FP, theLocalFactors);
        if (MkWire.IsDone())
          Loop = Handle(StepShape_Loop)::DownCast(MkWire.Value());
        else
        {
          FP->AddWarning(errShape, " a Wire not mapped");
          continue;
        }
      }

      Handle(StepShape_FaceBound) FaceBound = new StepShape_FaceBound();

      // ----------------------------------------------------
      // When the geometric normal of a Surface is reversed :
      //    - the wire topological orientation is reversed
      // ----------------------------------------------------
      // CAS.CADE face orientation :
      // when a face is reversed in a shell, the orientation of the underlying
      // topology is implicitly reversed. This is not the case in Step.
      // If face orientation is Reversed => the underlying (Step mapped) wire
      // are explicitly reversed

      Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");

      // Ajoute le 30 Juin pour TEST
      // Il convient de reprendre a la base ce probleme d'orientation
      // et notamment la politique d`exploration du Shape (on explore
      // toujours les sous-shapes d'un shape de maniere FORWARD !
      // la modif (on ajoute : si context faceted ... sinon) est a verifier
      // aupres des autres editeurs de CFAO de la Round Table.

      if (!aTool.Faceted() && aFace.Orientation() == TopAbs_REVERSED)
        FaceBound->Init(aName, Loop, (CurrentWire.Orientation() == TopAbs_REVERSED));
      else
        FaceBound->Init(aName, Loop, (CurrentWire.Orientation() == TopAbs_FORWARD));

      mySeq.Append(FaceBound);
    }

    // ----------------------------------------
    // Translate the Edge 2D Geometry (pcurves)
    // ----------------------------------------
    if (!aTool.Faceted() && aTool.PCurveMode() != 0)
    {

      TopExp_Explorer Ex(ForwardFace, TopAbs_EDGE);

      // ------------------------------------------------
      // Exploration of all the Edges in the current face
      // ------------------------------------------------
      for (; Ex.More(); Ex.Next())
      {
        TopoDS_Edge          E = TopoDS::Edge(Ex.Current());
        Standard_Real        cf, cl;
        Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(E, ForwardFace, cf, cl);
        if (BRep_Tool::Degenerated(E) || C2d.IsNull())
        {
          // The edge 2D Geometry degenerates in 3D
          // The edge 2D geometry is not mapped onto any Step entity
          // (ProStep agreement)
          continue;
        }
        // Copy the Curve2d which might be changed
        C2d = Handle(Geom2d_Curve)::DownCast(C2d->Copy());

        // for writing VERTEX_LOOP
        if (!aTool.IsBound(E))
          continue;
        Handle(StepGeom_Curve) Cpms =
          Handle(StepShape_EdgeCurve)::DownCast(aTool.Find(E))->EdgeGeometry();
        if (Cpms.IsNull())
          continue;

        if (aTool.IsBound(E))
        {
          if (C2d->IsKind(STANDARD_TYPE(Geom2d_Hyperbola))
              || C2d->IsKind(STANDARD_TYPE(Geom2d_Parabola)))
          {
            if (Su->IsKind(STANDARD_TYPE(Geom_SphericalSurface))
                || Su->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
                || Su->IsKind(STANDARD_TYPE(Geom_ConicalSurface))
                || Su->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
            {
              C2d = new Geom2d_TrimmedCurve(C2d, cf, cl, Standard_True);
            }
          }

          if ((C2d->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
              || (C2d->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))))
          {
            C2d = Geom2dConvert::CurveToBSplineCurve(C2d);
          }

          // if the Surface is a RectangularTrimmedSurface,
          // use the BasisSurface.
          //   CKY  23 SEP 1996 : on reste en Radian car on code des Radians
          //    sauf que ca ne marche pas bien ...
          Handle(Geom2d_Curve) C2dMapped;
          if (Su->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
          {
            Handle(Geom_RectangularTrimmedSurface) alocalRTS =
              Handle(Geom_RectangularTrimmedSurface)::DownCast(Su);
            C2dMapped = GeomConvert_Units::RadianToDegree(C2d,
                                                          alocalRTS->BasisSurface(),
                                                          theLocalFactors.LengthFactor(),
                                                          theLocalFactors.FactorRadianDegree());
          }
          else
          {
            C2dMapped = GeomConvert_Units::RadianToDegree(C2d,
                                                          Su,
                                                          theLocalFactors.LengthFactor(),
                                                          theLocalFactors.FactorRadianDegree());
          }
          //
          //	C2dMapped = C2d;  // cky : en remplacement de ce qui precede
          GeomToStep_MakeCurve MkCurve(C2dMapped, theLocalFactors);

          // --------------------
          // Translate the Pcurve
          // --------------------
          Handle(StepGeom_Pcurve)                     Pc  = new StepGeom_Pcurve;
          Handle(StepRepr_DefinitionalRepresentation) DRI = new StepRepr_DefinitionalRepresentation;
          Handle(StepRepr_HArray1OfRepresentationItem) aItems =
            new StepRepr_HArray1OfRepresentationItem(1, 1);
          aItems->SetValue(1, MkCurve.Value());
          Handle(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext)
            aContext =
              new StepGeom_GeometricRepresentationContextAndParametricRepresentationContext();
          Handle(TCollection_HAsciiString) aContextIdentifier =
            new TCollection_HAsciiString("2D SPACE");
          Handle(TCollection_HAsciiString) aContextType   = new TCollection_HAsciiString("");
          Standard_Integer                 aCoordSpaceDim = 2;
          aContext->Init(aContextIdentifier, aContextType, aCoordSpaceDim);

          Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");

          DRI->Init(aName, aItems, aContext);
          Pc->Init(aName, Spms, DRI);
          Handle(StepGeom_SurfaceCurve) C1pms = Handle(StepGeom_SurfaceCurve)::DownCast(Cpms);
          Handle(StepGeom_HArray1OfPcurveOrSurface) aGeom = C1pms->AssociatedGeometry();
          if (aGeom.IsNull())
            aGeom = new StepGeom_HArray1OfPcurveOrSurface(1, 2);
          StepGeom_PcurveOrSurface PcOrSur;
          PcOrSur.SetValue(Pc);
          if ((aGeom->Value(1)).IsNull())
          {
            aGeom->SetValue(1, PcOrSur);
            if (C1pms->IsKind(STANDARD_TYPE(StepGeom_SeamCurve)))
            {
              aGeom->SetValue(2, PcOrSur); // c est au moins ca
            }
          }
          else if (aGeom->Value(2).IsNull() || //) {
                   C1pms->IsKind(STANDARD_TYPE(StepGeom_SeamCurve)))
          { //: a8 abv 13 Feb 98: allow seam to have two different pcurves
            aGeom->SetValue(2, PcOrSur);
          }
          C1pms->SetAssociatedGeometry(aGeom);
        }
      }
    }

    // ------------------
    // Translate the Face
    // ------------------
    Standard_Integer nbWires = mySeq.Length();
    if (nbWires)
    {
      Handle(StepShape_HArray1OfFaceBound) aBounds = new StepShape_HArray1OfFaceBound(1, nbWires);
      for (Standard_Integer i = 1; i <= nbWires; i++)
      {
        aBounds->SetValue(i, Handle(StepShape_FaceBound)::DownCast(mySeq.Value(i)));
      }
      Handle(StepShape_AdvancedFace) Fpms = new StepShape_AdvancedFace;
      // The underlying surface has always a direct axis (see above)
      Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");

      Fpms->Init(aName, aBounds, Spms, aFace.Orientation() == TopAbs_FORWARD);

      aTool.Bind(aFace, Fpms);
      myError  = TopoDSToStep_FaceDone;
      myResult = Fpms;
      done     = Standard_True;
    }
    else
    {
      // ----------------------------
      // MakeFace Face Error Handling
      // ----------------------------
      FP->AddWarning(errShape, " No Wires of this Face were mapped");
      myError = TopoDSToStep_NoWireMapped;
      done    = Standard_False;
    }
  }
  catch (Standard_Failure const& theFailure)
  {
    FP->AddFail(errShape, theFailure.GetMessageString());
    myError = TopoDSToStep_FaceOther;
    done    = Standard_False;
  }
}

// ----------------------------------------------------------------------------
// Method  : Value
// Purpose :
// ----------------------------------------------------------------------------

const Handle(StepShape_TopologicalRepresentationItem)& TopoDSToStep_MakeStepFace::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeStepFace::Value() - no result");
  return myResult;
}

// ----------------------------------------------------------------------------
// Method  : Error
// Purpose :
// ----------------------------------------------------------------------------

TopoDSToStep_MakeFaceError TopoDSToStep_MakeStepFace::Error() const
{
  return myError;
}
