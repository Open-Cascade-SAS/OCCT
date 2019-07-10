// Created on: 1993-09-22
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
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

#include <MeshTest.hxx>

#include <stdio.h>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTest.hxx>
#include <BRepTools.hxx>
#include <CSLib.hxx>
#include <DBRep.hxx>
#include <Draw_Appli.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Draw_Segment2D.hxx>
#include <DrawTrSurf.hxx>
#include <GeometryTest.hxx>
#include <IMeshData_Status.hxx>
#include <Message.hxx>
#include <Message_ProgressRange.hxx>
#include <Poly_Connect.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>

#include <BRepMesh_Context.hxx>
#include <BRepMesh_FaceDiscret.hxx>
#include <BRepMesh_MeshAlgoFactory.hxx>
#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>

//epa Memory leaks test
//OAN: for triepoints
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )

#define ONETHIRD 0.333333333333333333333333333333333333333333333333333333333333
#define TWOTHIRD 0.666666666666666666666666666666666666666666666666666666666666

#ifdef OCCT_DEBUG_MESH_CHRONO
#include <OSD_Chronometer.hxx>
Standard_Integer D0Control, D0Internal, D0Unif, D0Edges, NbControls;
OSD_Chronometer chTotal, chInternal, chControl, chUnif, chAddPoint;
OSD_Chronometer chEdges, chMaillEdges, chEtuInter, chLastControl, chStock;
OSD_Chronometer chAdd11, chAdd12, chAdd2, chUpdate, chPointValid;
OSD_Chronometer chIsos, chPointsOnIsos;
#endif

//=======================================================================
//function : incrementalmesh
//purpose  : 
//=======================================================================
static Standard_Integer incrementalmesh(Draw_Interpretor& di, Standard_Integer nbarg, const char** argv)
{
  if (nbarg < 3)
  {
    di << "\
Builds triangular mesh for the shape\n\
usage: incmesh Shape LinearDeflection [options]\n\
options:\n\
        -a val          angular deflection for edges in deg\n\
                        (default ~28.64 deg = 0.5 rad)\n\n\
        -ai val         angular deflection inside of faces in deg\n\
                        (default ~57.29 deg = 1 rad)\n\n\
        -di val         Linear deflection used to tessellate the face interior.\n\
        -min            minimum size parameter limiting size of triangle's\n\
                        edges to prevent sinking into amplification in case\n\
                        of distorted curves and surfaces\n\n\
        -relative       notifies that relative deflection is used\n\
                        (switched off by default)\n\n\
        -int_vert_off   disables insertion of internal vertices into mesh\n\
                        (enabled by default)\n\
        -surf_def_off   disables control of deflection of mesh from real\n\
                        surface (enabled by default)\n\
        -parallel       enables parallel execution (switched off by default)\n\
        -adjust_min     enables local adjustment of min size depending on edge size (switched off by default)\n\
        -force_face_def disables usage of shape tolerances for computing face deflection (switched off by default)\n\
        -decrease       enforces the meshing of the shape even if current mesh satisfies the new criteria\
                        (switched off by default).\n\
        -algo {watson|delabella} changes core triangulation algorithm to one with specified id (watson is used by default)\n";
    return 0;
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);
  if (aShape.IsNull())
  {
    di << " Null shapes are not allowed here\n";
    return 0;
  }

  IMeshTools_Parameters aMeshParams;
  aMeshParams.Deflection = aMeshParams.DeflectionInterior = 
    Max(Draw::Atof(argv[2]), Precision::Confusion());

  Handle (IMeshTools_Context) aContext = new BRepMesh_Context;
  if (nbarg > 3)
  {
    Standard_Integer i = 3;
    while (i < nbarg)
    {
      TCollection_AsciiString aOpt(argv[i++]);
      aOpt.LowerCase();

      if (aOpt == "")
        continue;
      else if (aOpt == "-relative")
        aMeshParams.Relative = Standard_True;
      else if (aOpt == "-parallel")
        aMeshParams.InParallel = Standard_True;
      else if (aOpt == "-int_vert_off")
        aMeshParams.InternalVerticesMode = Standard_False;
      else if (aOpt == "-surf_def_off")
        aMeshParams.ControlSurfaceDeflection = Standard_False;
      else if (aOpt == "-adjust_min")
        aMeshParams.AdjustMinSize = Standard_True;
      else if (aOpt == "-force_face_def")
        aMeshParams.ForceFaceDeflection = Standard_True;
      else if (aOpt == "-decrease")
        aMeshParams.AllowQualityDecrease = Standard_True;
      else if (i < nbarg)
      {
        if (aOpt == "-algo")
        {
          TCollection_AsciiString anAlgoStr (argv[i++]);
          anAlgoStr.LowerCase();
          if (anAlgoStr == "watson"
           || anAlgoStr == "0")
          {
            aMeshParams.MeshAlgo = IMeshTools_MeshAlgoType_Watson;
            aContext->SetFaceDiscret (new BRepMesh_FaceDiscret (new BRepMesh_MeshAlgoFactory));
          }
          else if (anAlgoStr == "delabella"
                || anAlgoStr == "1")
          {
            aMeshParams.MeshAlgo = IMeshTools_MeshAlgoType_Delabella;
            aContext->SetFaceDiscret (new BRepMesh_FaceDiscret (new BRepMesh_DelabellaMeshAlgoFactory));
          }
          else if (anAlgoStr == "-1"
                || anAlgoStr == "default")
          {
            // already handled by BRepMesh_Context constructor
            //aMeshParams.MeshAlgo = IMeshTools_MeshAlgoType_DEFAULT;
          }
          else
          {
            di << "Syntax error at " << anAlgoStr;
            return 1;
          }
        }
        else
        {
          Standard_Real aVal = Draw::Atof(argv[i++]);
          if (aOpt == "-a")
          {
            aMeshParams.Angle = aVal * M_PI / 180.;
          }
          else if (aOpt == "-ai")
          {
            aMeshParams.AngleInterior = aVal * M_PI / 180.;
          }
          else if (aOpt == "-min")
            aMeshParams.MinSize = aVal;
          else if (aOpt == "-di")
          {
            aMeshParams.DeflectionInterior = aVal;
          }
          else
            --i;
        }
      }
    }
  }

  di << "Incremental Mesh, multi-threading "
     << (aMeshParams.InParallel ? "ON" : "OFF") << "\n";

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di, 1);
  BRepMesh_IncrementalMesh aMesher;
  aMesher.SetShape (aShape);
  aMesher.ChangeParameters() = aMeshParams;

  aMesher.Perform (aContext, aProgress->Start());

  di << "Meshing statuses: ";
  const Standard_Integer aStatus = aMesher.GetStatusFlags();
  if (!aStatus)
  {
    di << "NoError";
  }
  else
  {
    Standard_Integer i;
    for (i = 0; i < 9; i++)
    {
      Standard_Integer aFlag = aStatus & (1 << i);
      if (aFlag)
      {
        switch ((IMeshData_Status) aFlag)
        {
        case IMeshData_OpenWire:
          di << "OpenWire ";
          break;
        case IMeshData_SelfIntersectingWire:
          di << "SelfIntersectingWire ";
          break;
        case IMeshData_Failure:
          di << "Failure ";
          break;
        case IMeshData_ReMesh:
          di << "ReMesh ";
          break;
        case IMeshData_UnorientedWire:
          di << "UnorientedWire ";
          break;
        case IMeshData_TooFewPoints:
          di << "TooFewPoints ";
          break;
        case IMeshData_Outdated:
          di << "Outdated ";
          break;
        case IMeshData_Reused:
          di << "Reused ";
          break;
        case IMeshData_UserBreak:
          di << "User break";
          break;
        case IMeshData_NoError:
        default:
          break;
        }
      }
    }
  }

  return 0;
}

//=======================================================================
//function : tessellate
//purpose  : 
//=======================================================================
static Standard_Integer tessellate (Draw_Interpretor& /*di*/, Standard_Integer nbarg, const char** argv)
{
  if (nbarg != 5)
  {
    Message::SendFail() << "Builds regular triangulation with specified number of triangles\n"
                 "    Usage: tessellate result {surface|face} nbu nbv\n"
                 "    Triangulation is put into the face with natural bounds (result);\n"
                 "    it will have 2*nbu*nbv triangles and (nbu+1)*(nbv+1) nodes";
    return 1;
  }

  const char *aResName = argv[1];
  const char *aSrcName = argv[2];
  int aNbU = Draw::Atoi (argv[3]);
  int aNbV = Draw::Atoi (argv[4]);

  if (aNbU <= 0 || aNbV <= 0)
  {
    Message::SendFail() << "Error: Arguments nbu and nbv must be both greater than 0";
    return 1;
  }

  Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(aSrcName);
  double aUMin, aUMax, aVMin, aVMax;
  if (! aSurf.IsNull())
  {
    aSurf->Bounds (aUMin, aUMax, aVMin, aVMax);
  }
  else
  {
    TopoDS_Shape aShape = DBRep::Get(aSrcName);
    if (aShape.IsNull() || aShape.ShapeType() != TopAbs_FACE)
    {
      Message::SendFail() << "Error: " << aSrcName << " is not a face";
      return 1;
    }
    TopoDS_Face aFace = TopoDS::Face (aShape);
    aSurf = BRep_Tool::Surface (aFace);
    if (aSurf.IsNull())
    {
      Message::SendFail() << "Error: Face " << aSrcName << " has no surface";
      return 1;
    }

    BRepTools::UVBounds (aFace, aUMin, aUMax, aVMin, aVMax);
  }
  if (Precision::IsInfinite (aUMin) || Precision::IsInfinite (aUMax) || 
      Precision::IsInfinite (aVMin) || Precision::IsInfinite (aVMax))
  {
    Message::SendFail() << "Error: surface has infinite parametric range, aborting";
    return 1;
  }

  BRepBuilderAPI_MakeFace aFaceMaker (aSurf, aUMin, aUMax, aVMin, aVMax, Precision::Confusion());
  if (! aFaceMaker.IsDone())
  {
    Message::SendFail() << "Error: cannot build face with natural bounds, aborting";
    return 1;
  }
  TopoDS_Face aFace = aFaceMaker;

  // create triangulation
  int aNbNodes = (aNbU + 1) * (aNbV + 1);
  int aNbTriangles = 2 * aNbU * aNbV;
  Handle(Poly_Triangulation) aTriangulation =
    new Poly_Triangulation (aNbNodes, aNbTriangles, Standard_False);

  // fill nodes
  TColgp_Array1OfPnt &aNodes = aTriangulation->ChangeNodes();
  GeomAdaptor_Surface anAdSurf (aSurf);
  double aDU = (aUMax - aUMin) / aNbU;
  double aDV = (aVMax - aVMin) / aNbV;
  for (int iU = 0, iShift = 1; iU <= aNbU; iU++, iShift += aNbV + 1)
  {
    double aU = aUMin + iU * aDU;
    for (int iV = 0; iV <= aNbV; iV++)
    {
      double aV = aVMin + iV * aDV;
      gp_Pnt aP = anAdSurf.Value (aU, aV);
      aNodes.SetValue (iShift + iV, aP);
    }
  }

  // fill triangles
  Poly_Array1OfTriangle &aTriangles = aTriangulation->ChangeTriangles();
  for (int iU = 0, iShift = 1, iTri = 0; iU < aNbU; iU++, iShift += aNbV + 1)
  {
    for (int iV = 0; iV < aNbV; iV++)
    {
      int iBase = iShift + iV;
      Poly_Triangle aTri1 (iBase, iBase + aNbV + 2, iBase + 1);
      Poly_Triangle aTri2 (iBase, iBase + aNbV + 1, iBase + aNbV + 2);
      aTriangles.SetValue (++iTri, aTri1);
      aTriangles.SetValue (++iTri, aTri2);
    }
  }

  // put triangulation to face
  BRep_Builder B;
  B.UpdateFace (aFace, aTriangulation);

  // fill edge polygons
  TColStd_Array1OfInteger aUMinIso (1, aNbV + 1), aUMaxIso (1, aNbV + 1);
  for (int iV = 0; iV <= aNbV; iV++)
  {
    aUMinIso.SetValue (1 + iV, 1 + iV);
    aUMaxIso.SetValue (1 + iV, 1 + iV + aNbU * (1 + aNbV));
  }
  TColStd_Array1OfInteger aVMinIso (1, aNbU + 1), aVMaxIso (1, aNbU + 1);
  for (int iU = 0; iU <= aNbU; iU++)
  {
    aVMinIso.SetValue (1 + iU,  1 + iU  * (1 + aNbV));
    aVMaxIso.SetValue (1 + iU, (1 + iU) * (1 + aNbV));
  }
  Handle(Poly_PolygonOnTriangulation) aUMinPoly = new Poly_PolygonOnTriangulation (aUMinIso);
  Handle(Poly_PolygonOnTriangulation) aUMaxPoly = new Poly_PolygonOnTriangulation (aUMaxIso);
  Handle(Poly_PolygonOnTriangulation) aVMinPoly = new Poly_PolygonOnTriangulation (aVMinIso);
  Handle(Poly_PolygonOnTriangulation) aVMaxPoly = new Poly_PolygonOnTriangulation (aVMaxIso);
  for (TopExp_Explorer exp (aFace, TopAbs_EDGE); exp.More(); exp.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge (exp.Current());
    Standard_Real aFirst, aLast;
    Handle(Geom2d_Curve) aC = BRep_Tool::CurveOnSurface (anEdge, aFace, aFirst, aLast);
    gp_Pnt2d aPFirst = aC->Value (aFirst);
    gp_Pnt2d aPLast  = aC->Value (aLast);
    if (Abs (aPFirst.X() - aPLast.X()) < 0.1 * (aUMax - aUMin)) // U=const
    {
      if (BRep_Tool::IsClosed (anEdge, aFace))
        B.UpdateEdge (anEdge, aUMinPoly, aUMaxPoly, aTriangulation);
      else
        B.UpdateEdge (anEdge, (aPFirst.X() < 0.5 * (aUMin + aUMax) ? aUMinPoly : aUMaxPoly), aTriangulation);
    }
    else // V=const
    {
      if (BRep_Tool::IsClosed (anEdge, aFace))
        B.UpdateEdge (anEdge, aVMinPoly, aVMaxPoly, aTriangulation);
      else
        B.UpdateEdge (anEdge, (aPFirst.Y() < 0.5 * (aVMin + aVMax) ? aVMinPoly : aVMaxPoly), aTriangulation);
    }
  }

  DBRep::Set (aResName, aFace);
  return 0;
}

//=======================================================================
//function : MemLeakTest
//purpose  : 
//=======================================================================
static Standard_Integer MemLeakTest(Draw_Interpretor&, Standard_Integer /*nbarg*/, const char** /*argv*/)
{
  for(int i=0;i<10000;i++)
  {
    BRepBuilderAPI_MakePolygon w(gp_Pnt(0,0,0),gp_Pnt(0,100,0),gp_Pnt(20,100,0),gp_Pnt(20,0,0));
    w.Close();     
    TopoDS_Wire wireShape( w.Wire());
    BRepBuilderAPI_MakeFace faceBuilder(wireShape);          
    TopoDS_Face f( faceBuilder.Face());
    BRepMesh_IncrementalMesh im(f,1);
    BRepTools::Clean(f);      
  }
  return 0;
}

//=======================================================================
//function : trianglesinfo
//purpose  : 
//=======================================================================
static Standard_Integer trianglesinfo(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 1;
  TopExp_Explorer ex;
  Handle(Poly_Triangulation) T;
  TopLoc_Location L;

  Standard_Real MaxDeflection = 0.0;
  Standard_Integer nbtriangles = 0, nbnodes = 0;
  for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Face F = TopoDS::Face(ex.Current());
    T = BRep_Tool::Triangulation(F, L);
    if (!T.IsNull()) {
      nbtriangles += T->NbTriangles();
      nbnodes += T->NbNodes();
      if (T->Deflection() > MaxDeflection)
        MaxDeflection = T->Deflection();
    }
  }

  di<<"\n";
  di<<"This shape contains " <<nbtriangles<<" triangles.\n";
  di<<"                    " <<nbnodes    <<" nodes.\n";
  di<<"Maximal deflection " <<MaxDeflection<<"\n";
  di<<"\n";
#ifdef OCCT_DEBUG_MESH_CHRONO
  Standard_Real tot, addp, unif, contr, inter;
  Standard_Real edges, mailledges, etuinter, lastcontrol, stock;
  Standard_Real add11, add12, add2, upda, pointvalid;
  Standard_Real isos, pointsisos;
  chTotal.Show(tot); chAddPoint.Show(addp); chUnif.Show(unif); 
  chControl.Show(contr); chInternal.Show(inter);
  chEdges.Show(edges); chMaillEdges.Show(mailledges);
  chEtuInter.Show(etuinter); chLastControl.Show(lastcontrol); 
  chStock.Show(stock);
  chAdd11.Show(add11); chAdd12.Show(add12); chAdd2.Show(add2); chUpdate.Show(upda);
  chPointValid.Show(pointvalid); chIsos.Show(isos); chPointsOnIsos.Show(pointsisos);

  if (tot > 0.00001) {
    di <<"temps total de maillage:     "<<tot        <<" seconds\n";
    di <<"dont: \n";
    di <<"discretisation des edges:    "<<edges      <<" seconds---> "<< 100*edges/tot      <<" %\n";
    di <<"maillage des edges:          "<<mailledges <<" seconds---> "<< 100*mailledges/tot <<" %\n";
    di <<"controle et points internes: "<<etuinter   <<" seconds---> "<< 100*etuinter/tot   <<" %\n";
    di <<"derniers controles:          "<<lastcontrol<<" seconds---> "<< 100*lastcontrol/tot<<" %\n";
    di <<"stockage dans la S.D.        "<<stock      <<" seconds---> "<< 100*stock/tot      <<" %\n";
    di << "\n";
    di <<"et plus precisement: \n";
    di <<"Add 11ere partie :           "<<add11     <<" seconds---> "<<100*add11/tot      <<" %\n";
    di <<"Add 12ere partie :           "<<add12     <<" seconds---> "<<100*add12/tot      <<" %\n";
    di <<"Add 2eme partie :            "<<add2      <<" seconds---> "<<100*add2/tot       <<" %\n";
    di <<"Update :                     "<<upda      <<" seconds---> "<<100*upda/tot       <<" %\n";
    di <<"AddPoint :                   "<<addp      <<" seconds---> "<<100*addp/tot       <<" %\n";
    di <<"UniformDeflection            "<<unif      <<" seconds---> "<<100*unif/tot       <<" %\n";
    di <<"Controle :                   "<<contr     <<" seconds---> "<<100*contr/tot      <<" %\n";
    di <<"Points Internes:             "<<inter     <<" seconds---> "<<100*inter/tot      <<" %\n";
    di <<"calcul des isos et du, dv:   "<<isos      <<" seconds---> "<<100*isos/tot       <<" %\n";
    di <<"calcul des points sur isos:  "<<pointsisos<<" seconds---> "<<100*pointsisos/tot <<" %\n";
    di <<"IsPointValid:                "<<pointvalid<<" seconds---> "<<100*pointvalid/tot <<" %\n";
    di << "\n";


    di <<"nombre d'appels de controle apres points internes          : "<< NbControls << "\n";
    di <<"nombre de points sur restrictions                          : "<< D0Edges    << "\n";
    di <<"nombre de points calcules par UniformDeflection            : "<< D0Unif     << "\n";
    di <<"nombre de points calcules dans InternalVertices            : "<< D0Internal << "\n";
    di <<"nombre de points calcules dans Control                     : "<< D0Control  << "\n";
    if (nbnodes-D0Edges != 0) { 
      Standard_Real ratio = (Standard_Real)(D0Internal+D0Control)/ (Standard_Real)(nbnodes-D0Edges);
      di <<"---> Ratio: (D0Internal+D0Control) / (nbNodes-nbOnEdges)   : "<< ratio      << "\n";
    }

    di << "\n";

    chTotal.Reset(); chAddPoint.Reset(); chUnif.Reset(); 
    chControl.Reset(); chInternal.Reset();
    chEdges.Reset(); chMaillEdges.Reset();
    chEtuInter.Reset(); chLastControl.Reset(); 
    chStock.Reset();
    chAdd11.Reset(); chAdd12.Reset(); chAdd2.Reset(); chUpdate.Reset();
    chPointValid.Reset(); chIsos.Reset(); chPointsOnIsos.Reset();

  }
#endif
  return 0;
}

//=======================================================================
//function : veriftriangles
//purpose  : 
//=======================================================================
static Standard_Integer veriftriangles(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  Standard_Boolean quiet = 1;
  if (n == 3) quiet = 0;
  TopoDS_Shape Sh = DBRep::Get(a[1]);
  if (Sh.IsNull()) return 1;
  TopExp_Explorer ex;
  Handle(Poly_Triangulation) T;
  TopLoc_Location L;
  Standard_Integer i, n1, n2, n3;
  gp_Pnt2d mitri, v1, v2, v3, mi2d1, mi2d2, mi2d3;
  gp_XYZ vecEd1, vecEd2, vecEd3;
  //  Standard_Real dipo, dm, dv, d1, d2, d3, defle;
  Standard_Real dipo, dv, d1, d2, d3, defle;
  Handle(Geom_Surface) S;
  Standard_Integer nbface = 0;
  gp_Pnt PP;

  for (ex.Init(Sh, TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Face F = TopoDS::Face(ex.Current());
    nbface++;
    T = BRep_Tool::Triangulation(F, L);
    Standard_Real deflemax = 0, deflemin = 1.e100;
    if (!T.IsNull()) {
      Standard_Real defstock = T->Deflection();
      const Poly_Array1OfTriangle& triangles  = T->Triangles();
      const TColgp_Array1OfPnt2d&  Nodes2d    = T->UVNodes();
      const TColgp_Array1OfPnt&    Nodes      = T->Nodes();

      S = BRep_Tool::Surface(F, L);

      for(i = 1; i <= triangles.Length(); i++) {
        if (F.Orientation() == TopAbs_REVERSED) 
          triangles(i).Get(n1,n3,n2);
        else 
          triangles(i).Get(n1,n2,n3);

        const gp_XY& xy1 = Nodes2d(n1).XY();
        const gp_XY& xy2 = Nodes2d(n2).XY();
        const gp_XY& xy3 = Nodes2d(n3).XY();

        mi2d1.SetCoord((xy2.X()+xy3.X())*0.5, 
          (xy2.Y()+xy3.Y())*0.5);
        mi2d2.SetCoord((xy1.X()+xy3.X())*0.5, 
          (xy1.Y()+xy3.Y())*0.5);
        mi2d3.SetCoord((xy1.X()+xy2.X())*0.5, 
          (xy1.Y()+xy2.Y())*0.5);

        gp_XYZ p1 = Nodes(n1).Transformed(L.Transformation()).XYZ();
        gp_XYZ p2 = Nodes(n2).Transformed(L.Transformation()).XYZ();
        gp_XYZ p3 = Nodes(n3).Transformed(L.Transformation()).XYZ();

        vecEd1=p2-p1;
        vecEd2=p3-p2;
        vecEd3=p1-p3;
        d1=vecEd1.SquareModulus();
        d2=vecEd2.SquareModulus();
        d3=vecEd3.SquareModulus();

        if (d1!=0. && d2!=0. && d3!=0.) {
          gp_XYZ equa(vecEd1^vecEd2);
          dv=equa.Modulus();
          if (dv>0.) {
            equa.SetCoord(equa.X()/dv, equa.Y()/dv, equa.Z()/dv);
            dipo=equa*p1;


            mitri.SetCoord(ONETHIRD*(xy1.X()+xy2.X()+xy3.X()),
              ONETHIRD*(xy1.Y()+xy2.Y()+xy3.Y()));
            v1.SetCoord(ONETHIRD*mi2d1.X()+TWOTHIRD*xy1.X(), 
              ONETHIRD*mi2d1.Y()+TWOTHIRD*xy1.Y());
            v2.SetCoord(ONETHIRD*mi2d2.X()+TWOTHIRD*xy2.X(), 
              ONETHIRD*mi2d2.Y()+TWOTHIRD*xy2.Y());
            v3.SetCoord(ONETHIRD*mi2d3.X()+TWOTHIRD*xy3.X(), 
              ONETHIRD*mi2d3.Y()+TWOTHIRD*xy3.Y());

            S->D0(mi2d1.X(), mi2d1.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(mi2d2.X(), mi2d2.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(mi2d3.X(), mi2d3.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(v1.X(), v1.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(v2.X(), v2.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(v3.X(), v3.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(mitri.X(), mitri.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            if (defle > defstock) {
              di <<"face "<< nbface <<" deflection = " << defle <<" pour "<<defstock <<" stockee.\n";
            }
          }
        }
      }
      if (!quiet) {
        di <<"face "<< nbface<<", deflemin = "<< deflemin<<", deflemax = "<<deflemax<<"\n";
      }

    }
  }


  return 0;
}

//=======================================================================
//function : tri2d
//purpose  : 
//=======================================================================
static Standard_Integer tri2d(Draw_Interpretor&, Standard_Integer n, const char** a)
{

  if (n != 2) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Face F = TopoDS::Face(aLocalShape);
  //  TopoDS_Face F = TopoDS::Face(DBRep::Get(a[1]));
  if (F.IsNull()) return 1;
  Handle(Poly_Triangulation) T;
  TopLoc_Location L;

  T = BRep_Tool::Triangulation(F, L);
  if (!T.IsNull()) {
    // Build the connect tool
    Poly_Connect pc(T);

    Standard_Integer i,j, nFree, nInternal, nbTriangles = T->NbTriangles();
    Standard_Integer t[3];

    // count the free edges
    nFree = 0;
    for (i = 1; i <= nbTriangles; i++) {
      pc.Triangles(i,t[0],t[1],t[2]);
      for (j = 0; j < 3; j++)
        if (t[j] == 0) nFree++;
    }

    // allocate the arrays
    TColStd_Array1OfInteger Free(1,2*nFree);
    nInternal = (3*nbTriangles - nFree) / 2;
    TColStd_Array1OfInteger Internal(0,2*nInternal);

    Standard_Integer fr = 1, in = 1;
    const Poly_Array1OfTriangle& triangles = T->Triangles();
    Standard_Integer nodes[3];
    for (i = 1; i <= nbTriangles; i++) {
      pc.Triangles(i,t[0],t[1],t[2]);
      triangles(i).Get(nodes[0],nodes[1],nodes[2]);
      for (j = 0; j < 3; j++) {
        Standard_Integer k = (j+1) % 3;
        if (t[j] == 0) {
          Free(fr)   = nodes[j];
          Free(fr+1) = nodes[k];
          fr += 2;
        }
        // internal edge if this triangle has a lower index than the adjacent
        else if (i < t[j]) {
          Internal(in)   = nodes[j];
          Internal(in+1) = nodes[k];
          in += 2;
        }
      }
    }

    // Display the edges
    if (T->HasUVNodes()) {
      const TColgp_Array1OfPnt2d& Nodes2d = T->UVNodes();

      Handle(Draw_Segment2D) Seg;

      // free edges
      Standard_Integer nn;
      nn = Free.Length() / 2;
      for (i = 1; i <= nn; i++) {
        Seg = new Draw_Segment2D(Nodes2d(Free(2*i-1)),
          Nodes2d(Free(2*i)),
          Draw_rouge);
        dout << Seg;
      }

      // internal edges

      nn = nInternal;
      for (i = 1; i <= nn; i++) {
        Seg = new Draw_Segment2D(Nodes2d(Internal(2*i-1)),
          Nodes2d(Internal(2*i)),
          Draw_bleu);
        dout << Seg;
      }
    }
    dout.Flush();
  }

  return 0;
}

//=======================================================================
//function : wavefront
//purpose  : 
//=======================================================================
static Standard_Integer wavefront(Draw_Interpretor&, Standard_Integer nbarg, const char** argv)
{
  if (nbarg < 2) return 1;

  TopoDS_Shape S = DBRep::Get(argv[1]);
  if (S.IsNull()) return 1;

  // creation du maillage s'il n'existe pas.

  Bnd_Box B;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  BRepBndLib::Add(S, B);
  B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  Standard_Real aDeflection = 
    MAX3( aXmax-aXmin , aYmax-aYmin , aZmax-aZmin) * 0.004;

  BRepMesh_IncrementalMesh aMesh (S, aDeflection);


  TopLoc_Location L;
  TopExp_Explorer ex;

  Standard_Integer i, nbface = 0;
  Standard_Boolean OK = Standard_True;
  gp_Vec D1U,D1V;
  gp_Vec D2U,D2V,D2UV;
  gp_Dir Nor;
  gp_Pnt P;
  Standard_Real U, V;
  CSLib_DerivativeStatus aStatus;
  CSLib_NormalStatus NStat;
  Standard_Real x, y, z;
  Standard_Integer n1, n2, n3;
  Standard_Integer k1, k2, k3;

  char ffile[100];

  if (nbarg == 3) {
    strcpy(ffile, argv[2]);
    strcat(ffile, ".obj");
  }
  else  strcpy(ffile, "wave.obj");
  FILE* outfile = fopen(ffile, "w");


  fprintf(outfile, "%s  %s\n%s %s\n\n", "# CASCADE   ","MATRA DATAVISION", "#", ffile);

  Standard_Integer nbNodes, totalnodes = 0, nbpolygons = 0;
  for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next()) {
    nbface++;
    TopoDS_Face F = TopoDS::Face(ex.Current());
    Handle(Poly_Triangulation) Tr = BRep_Tool::Triangulation(F, L);

    if (!Tr.IsNull()) {
      nbNodes = Tr->NbNodes();
      const TColgp_Array1OfPnt& Nodes = Tr->Nodes();

      // les noeuds.
      for (i = 1; i <= nbNodes; i++) {
        gp_Pnt Pnt = Nodes(i).Transformed(L.Transformation());
        x = Pnt.X();
        y = Pnt.Y();
        z = Pnt.Z();
        fprintf(outfile, "%s      %f  %f  %f\n", "v", x, y, z);
      }

      fprintf(outfile, "\n%s    %d\n\n", "# number of vertex", nbNodes);


      // les normales.

      if (Tr->HasUVNodes()) {
        const TColgp_Array1OfPnt2d& UVNodes = Tr->UVNodes();
        BRepAdaptor_Surface BS(F, Standard_False);

        for (i = 1; i <= nbNodes; i++) {
          U = UVNodes(i).X();
          V = UVNodes(i).Y();

          BS.D1(U,V,P,D1U,D1V);
          CSLib::Normal (D1U, D1V, Precision::Angular(), aStatus, Nor);
          if (aStatus != CSLib_Done) {
            BS.D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
            CSLib::Normal(D1U,D1V,D2U,D2V,D2UV,Precision::Angular(),OK,NStat,Nor);
          }
          if (F.Orientation() == TopAbs_REVERSED) Nor.Reverse();

          fprintf(outfile, "%s      %f  %f  %f\n", "vn", Nor.X(), Nor.Y(), Nor.Z());
        }

        fprintf(outfile, "\n%s    %d\n\n", "# number of vertex normals", nbNodes);
      }

      fprintf(outfile, "%s    %d\n", "s", nbface);

      // les triangles.
      Standard_Integer nbTriangles = Tr->NbTriangles();
      const Poly_Array1OfTriangle& triangles = Tr->Triangles();


      for (i = 1; i <= nbTriangles; i++) {
        if (F.Orientation()  == TopAbs_REVERSED)
          triangles(i).Get(n1, n3, n2);
        else 
          triangles(i).Get(n1, n2, n3);
        k1 = n1+totalnodes;
        k2 = n2+totalnodes;
        k3 = n3+totalnodes;
        fprintf(outfile, "f %d%s%d %d%s%d %d%s%d\n", k1,"//", k1, k2,"//", k2, k3,"//", k3);
      }
      nbpolygons += nbTriangles;
      totalnodes += nbNodes;

      fprintf(outfile, "\n%s    %d\n", "# number of smooth groups", nbface);
      fprintf(outfile, "\n%s    %d\n", "# number of polygons", nbpolygons);

    }
  }

  fclose(outfile);

  return 0;
}

//=======================================================================
//function : triedgepoints
//purpose  : 
//=======================================================================
static Standard_Integer triedgepoints(Draw_Interpretor& di, Standard_Integer nbarg, const char** argv)
{
  if( nbarg < 2 )
    return 1;

  for( Standard_Integer i = 1; i < nbarg; i++ )
  {
    TopoDS_Shape aShape = DBRep::Get(argv[i]);
    if ( aShape.IsNull() )
      continue;

    Handle(Poly_PolygonOnTriangulation) aPoly;
    Handle(Poly_Triangulation)          aT;
    TopLoc_Location                     aLoc;
    TopTools_MapOfShape                 anEdgeMap;
    TopTools_MapIteratorOfMapOfShape    it;
    
    if( aShape.ShapeType() == TopAbs_EDGE )
    {
      anEdgeMap.Add( aShape );
    }
    else
    {
      TopExp_Explorer ex(aShape, TopAbs_EDGE);
      for(; ex.More(); ex.Next() )
        anEdgeMap.Add( ex.Current() );
    }

    if ( anEdgeMap.Extent() == 0 )
      continue;

    char newname[1024];
    strcpy(newname,argv[i]);
    char* p = newname;
    while (*p != '\0') p++;
    *p = '_';
    p++;

    Standard_Integer nbEdge = 1;
    for(it.Initialize(anEdgeMap); it.More(); it.Next())
    {
      BRep_Tool::PolygonOnTriangulation(TopoDS::Edge(it.Key()), aPoly, aT, aLoc);
      if ( aT.IsNull() || aPoly.IsNull() )
        continue;
      
      const TColgp_Array1OfPnt&      Nodes   = aT->Nodes();
      const TColStd_Array1OfInteger& Indices = aPoly->Nodes();
      const Standard_Integer         nbnodes = Indices.Length();

      for( Standard_Integer j = 1; j <= nbnodes; j++ )
      {
        gp_Pnt P3d = Nodes(Indices(j));
        if( !aLoc.IsIdentity() )
          P3d.Transform(aLoc.Transformation());

        if( anEdgeMap.Extent() > 1 )
          Sprintf(p,"%d_%d",nbEdge,j);
        else
          Sprintf(p,"%d",j);
	      DBRep::Set( newname, BRepBuilderAPI_MakeVertex(P3d) );
	      di.AppendElement(newname);
      }
      nbEdge++;
    }
  }
  return 0;
}

//=======================================================================
//function : correctnormals
//purpose  : Corrects normals in shape triangulation nodes (...)
//=======================================================================
static Standard_Integer correctnormals(Draw_Interpretor& theDI,
                                       Standard_Integer /*theNArg*/,
                                       const char** theArgVal)
{
  TopoDS_Shape S = DBRep::Get(theArgVal[1]);

  //Use "correctnormals shape"

  
  if(!BRepLib::EnsureNormalConsistency(S))
  {
    theDI << "Normals have not been changed!\n";
  }
  else
  {
    theDI << "Some corrections in source shape have been made!\n";
  }

  return 0;
}

//=======================================================================
void  MeshTest::Commands(Draw_Interpretor& theCommands)
//=======================================================================
{
  Draw::Commands(theCommands);
  BRepTest::AllCommands(theCommands);
  GeometryTest::AllCommands(theCommands);
  MeshTest::PluginCommands(theCommands);
  const char* g;

  g = "Mesh Commands";

  theCommands.Add("incmesh","Builds triangular mesh for the shape, run w/o args for help",__FILE__, incrementalmesh, g);
  theCommands.Add("tessellate","Builds triangular mesh for the surface, run w/o args for help",__FILE__, tessellate, g);
  theCommands.Add("MemLeakTest","MemLeakTest",__FILE__, MemLeakTest, g);

  theCommands.Add("tri2d", "tri2d facename",__FILE__, tri2d, g);
  theCommands.Add("trinfo","trinfo name, print triangles information on objects",__FILE__,trianglesinfo,g);
  theCommands.Add("veriftriangles","veriftriangles name, verif triangles",__FILE__,veriftriangles,g);
  theCommands.Add("wavefront","wavefront name",__FILE__, wavefront, g);
  theCommands.Add("triepoints", "triepoints shape1 [shape2 ...]",__FILE__, triedgepoints, g);

  theCommands.Add("correctnormals", "correctnormals shape",__FILE__, correctnormals, g);
}
