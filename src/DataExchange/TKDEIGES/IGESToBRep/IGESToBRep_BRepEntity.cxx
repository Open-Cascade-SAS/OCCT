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

//=======================================================================
// purpose  : Members to transfer any BRepEntity into TopoDS_Shape
//=======================================================================
// 21.12.98 rln, gka S4054
// #62 rln 10.01.99 PRO17015
//: q5 abv 19.03.99 unnecessary includes removed
// pdn 12.03.99 S4135 Constructing vertex with minimal tolerance
//: r1 abv 25.03.99 CTS21655.igs, CTS18545.igs: apply FixOrientation to whole face
// S4181 pdn 20.04.99 implementing of reading IGES elementary surfaces.
// pdn 20.04.99 CTS22655 avoid of exceptions in case of empty loops

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf2d.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_VertexList.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_AlgoContainer.hxx>
#include <IGESToBRep_BRepEntity.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <IGESToBRep_IGESBoundary.hxx>
#include <IGESToBRep_ToolContainer.hxx>
#include <IGESToBRep_TopoCurve.hxx>
#include <IGESToBRep_TopoSurface.hxx>
#include <MoniTool_Macros.hxx>
#include <Message_Msg.hxx>
#include <Message_ProgressScope.hxx>
#include <Precision.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeExtend_WireData.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <Transfer_TransientProcess.hxx>

#include <stdio.h>

// rln
// #include <ShapeFix_Face.hxx>
//=================================================================================================

IGESToBRep_BRepEntity::IGESToBRep_BRepEntity()
    : IGESToBRep_CurveAndSurface()
{
  SetModeTransfer(false);
  SetContinuity(0);
}

//=================================================================================================

IGESToBRep_BRepEntity::IGESToBRep_BRepEntity(const IGESToBRep_CurveAndSurface& CS)
    : IGESToBRep_CurveAndSurface(CS)
{
  SetContinuity(0);
}

//=================================================================================================

IGESToBRep_BRepEntity::IGESToBRep_BRepEntity(const double    eps,
                                             const double    epsCoeff,
                                             const double    epsGeom,
                                             const bool mode,
                                             const bool modeapprox,
                                             const bool optimized)
    : IGESToBRep_CurveAndSurface(eps, epsCoeff, epsGeom, mode, modeapprox, optimized)
{
  SetContinuity(0);
}

//=================================================================================================

TopoDS_Shape IGESToBRep_BRepEntity::TransferBRepEntity(const occ::handle<IGESData_IGESEntity>& start,
                                                       const Message_ProgressRange& theProgress)
{
  TopoDS_Shape res;

  if (start->IsKind(STANDARD_TYPE(IGESSolid_Face)))
  {
    DeclareAndCast(IGESSolid_Face, st510, start);
    res = TransferFace(st510);
  }
  else if (start->IsKind(STANDARD_TYPE(IGESSolid_Shell)))
  {
    DeclareAndCast(IGESSolid_Shell, st514, start);
    res = TransferShell(st514, theProgress);
  }
  else if (start->IsKind(STANDARD_TYPE(IGESSolid_ManifoldSolid)))
  {
    DeclareAndCast(IGESSolid_ManifoldSolid, st186, start);
    res = TransferManifoldSolid(st186, theProgress);
  }
  else
  {
    Message_Msg Msg1005("IGES_1005");
    SendFail(start, Msg1005);
  }
  return res;
}

//=================================================================================================

TopoDS_Vertex IGESToBRep_BRepEntity::TransferVertex(const occ::handle<IGESSolid_VertexList>& start,
                                                    const int              index)
{
  TopoDS_Vertex res;

  int nbshapes = NbShapeResult(start);
  if (nbshapes == 0)
  {
    BRep_Builder B;
    for (int inum = 1; inum <= start->NbVertices(); inum++)
    {
      gp_Pnt point = start->Vertex(inum);
      point.Scale(gp_Pnt(0, 0, 0), GetUnitFactor());
      TopoDS_Vertex V;
      // pdn 12.03.99 S4135 Constructing vertex with minimal tolerance
      B.MakeVertex(V, point, Precision::Confusion());
      AddShapeResult(start, V);
    }
  }

  TopoDS_Shape Sh = GetShapeResult(start, index);
  if (Sh.IsNull())
  {
    Message_Msg Msg1156("IGES_1156"); //"the Vertex number %d is a null object." FAIL!!!
    occ::handle<TCollection_HAsciiString> label = GetModel()->StringLabel(start);
    Msg1156.Arg("vertex");
    Msg1156.Arg(label);
    SendWarning(start, Msg1156);
  }
  res = TopoDS::Vertex(Sh);
  return res;
}

//=================================================================================================

TopoDS_Shape IGESToBRep_BRepEntity::TransferEdge(const occ::handle<IGESSolid_EdgeList>& start,
                                                 const int            index)
{
  TopoDS_Shape res;
  BRep_Builder B;

  int nbshapes = NbShapeResult(start);
  if (nbshapes == 0)
  {
    IGESToBRep_TopoCurve TC(*this);
    for (int inum = 1; inum <= start->NbEdges(); inum++)
    {

      // Vertices
      // --------
      occ::handle<IGESSolid_VertexList> thestartlist  = start->StartVertexList(inum);
      int             thestartindex = start->StartVertexIndex(inum);
      TopoDS_Vertex                V1            = TransferVertex(thestartlist, thestartindex);

      occ::handle<IGESSolid_VertexList> theendlist  = start->EndVertexList(inum);
      int             theendindex = start->EndVertexIndex(inum);
      TopoDS_Vertex                V2          = TransferVertex(theendlist, theendindex);

      // Curve
      // -----

      occ::handle<IGESData_IGESEntity> thecurve = start->Curve(inum);
      if (thecurve.IsNull() || !IGESToBRep::IsTopoCurve(thecurve)
          || thecurve->IsKind(STANDARD_TYPE(IGESGeom_CurveOnSurface))
          || thecurve->IsKind(STANDARD_TYPE(IGESGeom_Boundary)))
      {
        Message_Msg Msg1306("IGES_1306"); // one underlying curve is a Null object.
        Msg1306.Arg(inum);
        SendWarning(start, Msg1306);
        TopoDS_Edge Sh;
        AddShapeResult(start, Sh); // add null shape to avoid shift of indexing
      }
      else
      {
        TopoDS_Shape Sh = TC.TransferTopoCurve(thecurve);
        if (!Sh.IsNull())
        {
          if (Sh.ShapeType() == TopAbs_EDGE)
          {
            TopoDS_Edge   edge = TopoDS::Edge(Sh);
            TopoDS_Vertex Vf, Vl;
            TopExp::Vertices(edge, Vf, Vl);
            TopoDS_Edge E;
            B.MakeEdge(E);
            TopLoc_Location    loc;
            double      first, last;
            occ::handle<Geom_Curve> Crv = BRep_Tool::Curve(edge, loc, first, last);
            occ::handle<Geom_Curve> newC3d;
            // in the case of a conic, it is necessary to reverse
            // IGES direction of travel inverse to CASCADE direction of travel.
            if (Crv->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
            {
              DeclareAndCast(Geom_TrimmedCurve, acurve, Crv);
              newC3d = acurve->BasisCurve();
            }
            else
            {
              newC3d = Crv;
            }
            B.UpdateEdge(E, newC3d, loc, 0.); // S4054:GetEpsGeom()*GetUnitFactor()
            gp_Pnt        p1     = BRep_Tool::Pnt(V1);
            gp_Pnt        p2     = BRep_Tool::Pnt(V2);
            gp_Pnt        pf     = BRep_Tool::Pnt(Vf);
            gp_Pnt        pl     = BRep_Tool::Pnt(Vl);
            double dist1f = p1.Distance(pf);
            double dist2f = p2.Distance(pf);
            double dist1l = p1.Distance(pl);
            double dist2l = p2.Distance(pl);
            if (V1.IsSame(V2) || dist1f + dist2l <= dist1l + dist2f + Precision::Confusion())
            {
              //: 77 if (BRepTools::Compare(V1, Vf)) //the part 'else' only if, in fact, edge should
              //: be reversed
              V1.Orientation(TopAbs_FORWARD);
              B.Add(E, V1);
              V2.Orientation(TopAbs_REVERSED);
              B.Add(E, V2);
              // clang-format off
	      B.UpdateVertex(V1, first, E, 0.);//S4054 1.001 * dist1f //:77 GetEpsGeom()*GetUnitFactor();
	      B.UpdateVertex(V2, last,  E, 0.);//S4054 1.001 * dist2l //:77 GetEpsGeom()*GetUnitFactor();
              // clang-format on
              B.Range(E, first, last);
            }
            // modification mjm of 13/10/97 : Reverse the edge?
            else
            {
              E.Reverse();
              V1.Orientation(TopAbs_FORWARD);
              B.Add(E, V1);
              V2.Orientation(TopAbs_REVERSED);
              B.Add(E, V2);
              // clang-format off
	      B.UpdateVertex(V1, last,  E, 0.);//S4054 1.001 * dist1l //:77 GetEpsGeom()*GetUnitFactor();
	      B.UpdateVertex(V2, first, E, 0.);//S4054 1.001 * dist2f //:77 GetEpsGeom()*GetUnitFactor();
	      B.Range (E, first, last);
	    }
	    AddShapeResult(start,E);
	  }
	  else if (Sh.ShapeType() == TopAbs_WIRE) {
	    // pas traite 
	    Message_Msg Msg1325("IGES_1325"); //"Item %d of EdgeList cannot be represented by single edge (non-continuous or composite curve)."
            // clang-format on
            Msg1325.Arg(inum);
            SendWarning(start, Msg1325);
            AddShapeResult(start, Sh);
          }
        }
        else
        {
          Message_Msg                      Msg1156("IGES_1156");
          occ::handle<TCollection_HAsciiString> label = GetModel()->StringLabel(thecurve);
          Msg1156.Arg("underlying curve");
          Msg1156.Arg(label);
          SendWarning(start, Msg1156);
          AddShapeResult(start, Sh); // add null shape to avoid shift of indexing
        }
      }
    }
  }

  TopoDS_Shape Sh = GetShapeResult(start, index);
  if (Sh.IsNull())
  {
    Message_Msg                      Msg1156("IGES_1156");
    occ::handle<TCollection_HAsciiString> label = GetModel()->StringLabel(start);
    Msg1156.Arg("edge");
    Msg1156.Arg(label);
    SendWarning(start, Msg1156);
  }
  return Sh;
}

//=================================================================================================

TopoDS_Shape IGESToBRep_BRepEntity::TransferLoop(const occ::handle<IGESSolid_Loop>& start,
                                                 const TopoDS_Face&            face,
                                                 const gp_Trsf2d&              trans,
                                                 const double           uFact)
{
  TopoDS_Shape res;

  if (!HasShapeResult(start))
  {
    TopoDS_Wire      mywire;
    bool okCurve = true, okCurve3d = true, okCurve2d = true;
    occ::handle<ShapeExtend_WireData> sewd;
    int             filepreference = 3; // 3D is preferred by default
    bool             Result         = true;

    occ::handle<IGESToBRep_IGESBoundary> IB =
      IGESToBRep::AlgoContainer()->ToolContainer()->IGESBoundary();
    IB->Init(*this, start, face, trans, uFact, filepreference);
    BRep_Builder    B;
    ShapeBuild_Edge sbe;

    for (int iedge = 1; iedge <= start->NbEdges(); iedge++)
    {
      int            itype       = start->EdgeType(iedge);
      occ::handle<IGESData_IGESEntity> theedge     = start->Edge(iedge);
      int            indexlist   = start->ListIndex(iedge);
      bool            orientation = start->Orientation(iedge);
      int            nbparam     = start->NbParameterCurves(iedge);
      if (theedge.IsNull())
      {
        Message_Msg Msg1365("IGES_1365"); //"Loop : one edge is null"
        Msg1365.Arg(iedge);
        SendWarning(start, Msg1365);
        //	AddWarning (start,"Loop : one edge is null");
      }
      else
      {
        //  edge
        //  ----
        occ::handle<ShapeExtend_WireData> curve3d = new ShapeExtend_WireData;

        if ((itype == 1) && (theedge->IsKind(STANDARD_TYPE(IGESSolid_VertexList))))
        {
          DeclareAndCast(IGESSolid_VertexList, thelist, theedge);
          TopoDS_Vertex V1 = TransferVertex(thelist, indexlist);
          TopoDS_Edge   E;
          B.MakeEdge(E);
          // szv#4:S4163:12Mar99 SGI warns
          TopoDS_Shape sh = V1.Oriented(TopAbs_FORWARD);
          B.Add(E, TopoDS::Vertex(sh));
          sh = V1.Oriented(TopAbs_REVERSED);
          B.Add(E, TopoDS::Vertex(sh));
          B.Degenerated(E, true);
          curve3d->Add(E);
        }
        else if ((itype == 0) && (theedge->IsKind(STANDARD_TYPE(IGESSolid_EdgeList))))
        {
          DeclareAndCast(IGESSolid_EdgeList, thelist, theedge);
          TopoDS_Shape Sh = TransferEdge(thelist, indexlist);
          if (Sh.IsNull())
          {
            // clang-format off
            continue; // skip non-translated edge hoping for the best; warning is already generated by TransferEdge()
            // clang-format on
          }
          curve3d->Add(Sh);
        }
        else
        {
          Message_Msg                      Msg1365("IGES_1365"); //"Improper type for the edge"
          occ::handle<TCollection_HAsciiString> label = GetModel()->StringLabel(start);
          Msg1365.Arg(iedge);
          SendWarning(start, Msg1365);
          continue;
        }
        if (!orientation)
          curve3d->Reverse();

        //  processing of 2d curves.
        //  -------------------------
        occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> Curves2d;

        // Current limitation:
        // 2D representation is not translated if:
        //- 3D curve was translated into wire (i.e. if it is 102)
        //- more than 1 2D curve,
        //- 2D curve is 102
        occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> seq2d;
        if (curve3d->NbEdges() == 1 && nbparam == 1
            && IGESToBRep::IGESCurveToSequenceOfIGESCurve(start->ParametricCurve(iedge, 1), seq2d)
                 == 1)
        {
          Curves2d = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbparam);
          for (int i = 1; i <= nbparam; i++)
            Curves2d->SetValue(i, start->ParametricCurve(iedge, i));
        }
        occ::handle<ShapeExtend_WireData> lsewd; // result of translation of current edge
        Result = Result
                 & IB->Transfer(okCurve,
                                okCurve3d,
                                okCurve2d,
                                curve3d,
                                Curves2d,
                                !orientation,
                                iedge,
                                lsewd);
        if (iedge == 1)
          sewd = IB->WireData(); // initialization
        if (curve3d->NbEdges() == 1 && lsewd->NbEdges() == 1)
        { // the condition corresponds to limitation above
          // to keep sharing of edges all geometric representations should be put
          // into the edge from EdgeList
          TopoDS_Edge fromedge = lsewd->Edge(1), toedge = curve3d->Edge(1);
          if (!fromedge.IsSame(toedge))
          {
            sbe.RemoveCurve3d(toedge);
            IGESToBRep::TransferPCurve(fromedge, toedge, face);
            sewd->Set(toedge, sewd->Index(fromedge));
          }
        }
      }
    }
    // IB->Check(Result, true);
    // pdn 20.04.99 CTS22655 avoid of exceptions in case of empty loops
    if (!sewd.IsNull())
    {
      // IB.Fix (sewd, false, true, true, true, true);
      mywire = sewd->Wire();
    }
    SetShapeResult(start, mywire);
  }

  TopoDS_Shape Sh = GetShapeResult(start);
  if (Sh.IsNull())
  {
    Message_Msg                      Msg1156("IGES_1156"); // The Loop is null
    occ::handle<TCollection_HAsciiString> label = GetModel()->StringLabel(start);
    Msg1156.Arg("loop");
    Msg1156.Arg(label);
    SendWarning(start, Msg1156);
  }
  return Sh;
}

//=================================================================================================

TopoDS_Shape IGESToBRep_BRepEntity::TransferFace(const occ::handle<IGESSolid_Face>& start)
{
  TopoDS_Shape res;

  if (!HasShapeResult(start))
  {
    BRep_Builder                B;
    TopoDS_Face                 F;
    occ::handle<IGESData_IGESEntity> surf    = start->Surface();
    int            nbloops = start->NbLoops();
    IGESToBRep_TopoSurface      TS(*this);

    // surface
    // -------
    if (surf.IsNull() || !IGESToBRep::IsTopoSurface(surf)
        || surf->IsKind(STANDARD_TYPE(IGESGeom_Plane))
        || surf->IsKind(STANDARD_TYPE(IGESGeom_BoundedSurface))
        || surf->IsKind(STANDARD_TYPE(IGESGeom_TrimmedSurface))
        || surf->IsKind(STANDARD_TYPE(IGESBasic_SingleParent)))
    {
      Message_Msg Msg196("XSTEP_196"); //"no base surface to create the face"
      SendWarning(start, Msg196);
      // AddWarning(start, "no base surface to create the face");
      TopoDS_Shape Sh;
      SetShapeResult(start, Sh);
    }
    else
    {
      // if the IGES surface is a surface of revolution, it will be necessary
      // to invert the 2d curves (u,v) to be in agreement with the parametrization
      // BRep.
      gp_Trsf2d     trans;
      double uFact;
      TopoDS_Shape  myshape = TS.ParamSurface(surf, trans, uFact);

      if (!myshape.IsNull())
      {
        if (myshape.ShapeType() == TopAbs_FACE)
        {
          // #62 rln 10.01.99 PRO17015 (reading back IGES written in 'BRep' mode) face #65
          F = TopoDS::Face(myshape);
          F.EmptyCopy();
          if (nbloops == 0)
            B.NaturalRestriction(F, true);

          // Loops
          // -----
          for (int iloop = 1; iloop <= nbloops; iloop++)
          {
            occ::handle<IGESSolid_Loop> loop  = start->Loop(iloop);
            TopoDS_Shape           Shape = TransferLoop(loop, F, trans, uFact);
            // pdn 20.04.99 CTS22655 avoid of exceptions in case of empty loops
            if (!Shape.IsNull())
              B.Add(F, Shape);
          }

          // update the face
          BRepTools::Update(F);
          F.Orientable(true);
          SetShapeResult(start, F);
        }
      }
      else
      {
        Message_Msg Msg1156("IGES_1156"); // Face : result of TransferTopoSurface is Null
        occ::handle<TCollection_HAsciiString> label = GetModel()->StringLabel(surf);
        Msg1156.Arg("surface");
        Msg1156.Arg(label);
        SendFail(start, Msg1156);
        //  AddWarning(start,"Face : result of TransferTopoSurface is Null");
        TopoDS_Shape Sh;
        SetShapeResult(start, Sh);
      }
    }
  }

  TopoDS_Shape Sh = GetShapeResult(start);
  if (Sh.IsNull())
  {
    Message_Msg                      Msg1156("IGES_1156"); // the Face is a Null object.
    occ::handle<TCollection_HAsciiString> label = GetModel()->StringLabel(start);
    Msg1156.Arg("face");
    Msg1156.Arg(label);
    SendFail(start, Msg1156);
    //    AddWarning (start, "the Face is a Null object.");
  }
  return Sh;
}

//=================================================================================================

TopoDS_Shape IGESToBRep_BRepEntity::TransferShell(const occ::handle<IGESSolid_Shell>& start,
                                                  const Message_ProgressRange&   theProgress)
{
  TopoDS_Shape res;

  if (!HasShapeResult(start))
  {
    TopoDS_Shell S;
    BRep_Builder B;
    B.MakeShell(S);
    int nbfaces = start->NbFaces();
    if (nbfaces != 0)
    {
      bool      closed = true; //: 39
      Message_ProgressScope aPS(theProgress, "Face", nbfaces);
      for (int iface = 1; iface <= nbfaces && aPS.More(); iface++, aPS.Next())
      {
        occ::handle<IGESSolid_Face> face        = start->Face(iface);
        bool       orientation = start->Orientation(iface);
        TopoDS_Shape           Sh          = TransferFace(face);
        if (Sh.IsNull())
        { //: 39 by abv 15.12.97

          closed = false;
          continue;
        }
        if (!orientation)
          Sh.Reverse();
        B.Add(S, Sh);
      }
      if (!closed)
      {
        Message_Msg Msg1360("IGES_1360");
        SendFail(start, Msg1360);
      }
      // AddWarning ( start, "Shell is not closed" ); //:39
      S.Closed(closed); //: 39
      S.Orientable(true);
      SetShapeResult(start, S);
    }
    else
    {
      Message_Msg Msg200("XSTEP_200"); // Number of Faces = 0
      SendFail(start, Msg200);
    }
  }

  TopoDS_Shape Sh = GetShapeResult(start);
  if (Sh.IsNull())
  {
    Message_Msg                      Msg1156("IGES_1156"); // the Shell is a null object
    occ::handle<TCollection_HAsciiString> label = GetModel()->StringLabel(start);
    Msg1156.Arg("shell");
    Msg1156.Arg(label);
    SendFail(start, Msg1156);
  }
  // AddWarning (start, "the Shell is a null object.");
  res = Sh;
  return res;
}

//=================================================================================================

TopoDS_Shape IGESToBRep_BRepEntity::TransferManifoldSolid(
  const occ::handle<IGESSolid_ManifoldSolid>& start,
  const Message_ProgressRange&           theProgress)
{
  TopoDS_Shape res;

  if (!HasShapeResult(start))
  {
    TopoDS_Solid S;
    BRep_Builder B;
    B.MakeSolid(S);
    occ::handle<IGESSolid_Shell> shell      = start->Shell();
    bool        isoriented = start->OrientationFlag();
    int        nbshell    = start->NbVoidShells();
    TopoDS_Shape            Sh         = TransferShell(shell, theProgress);
    if (!Sh.IsNull())
    {
      if (Sh.ShapeType() == TopAbs_SHELL)
      {
        TopoDS_Shell Shell = TopoDS::Shell(Sh);
        if (!isoriented)
          Shell.Reverse();
        B.Add(S, Shell);
      }

      if (nbshell != 0)
      {
        // progress scope without name, since usually we have single shell in solid
        Message_ProgressScope aPS(theProgress, NULL, nbshell);
        for (int ishell = 1; ishell <= nbshell && aPS.More(); ishell++)
        {
          occ::handle<IGESSolid_Shell> voidshell = start->VoidShell(ishell);
          //	  bool orientation = start->VoidOrientationFlag(ishell);
          TopoDS_Shape aSh = TransferShell(voidshell, aPS.Next());
          if (!aSh.IsNull())
          {
            if (aSh.ShapeType() == TopAbs_SHELL)
            {
              TopoDS_Shell Shell = TopoDS::Shell(aSh);
              if (!isoriented)
                Shell.Reverse();
              B.Add(S, Shell);
            }
          }
          else
          {
            //  AddWarning(start,"ManifoldSolid : one VoidShell is Null");
            TopoDS_Shell Shell;
            B.Add(S, Shell);
          }
        }
      }
    }
    SetShapeResult(start, S);
  }

  TopoDS_Shape Sh = GetShapeResult(start);
  if (Sh.IsNull())
  {
    Message_Msg                      Msg1156("IGES_1156"); // the ManifoldSolid is a null object.
    occ::handle<TCollection_HAsciiString> label = GetModel()->StringLabel(start);
    Msg1156.Arg("solid");
    Msg1156.Arg(label);
    SendFail(start, Msg1156);
  }
  //   AddWarning (start, "the ManifoldSolid is a null object.");
  res = Sh;
  return res;
}
