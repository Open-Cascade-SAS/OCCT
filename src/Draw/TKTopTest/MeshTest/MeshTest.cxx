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
#include <OSD_OpenFile.hxx>
#include <Poly_MergeNodesTool.hxx>
#include <Poly_TriangulationParameters.hxx>
#include <Prs3d_Drawer.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_TEdge.hxx>
#include <TopExp.hxx>

#include <BRepMesh_Context.hxx>
#include <BRepMesh_FaceDiscret.hxx>
#include <BRepMesh_MeshAlgoFactory.hxx>
#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>

#include <algorithm>

// epa Memory leaks test
// OAN: for triepoints
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

#define MAX2(X, Y) (std::abs(X) > std::abs(Y) ? std::abs(X) : std::abs(Y))
#define MAX3(X, Y, Z) (MAX2(MAX2(X, Y), Z))

#define ONETHIRD 0.333333333333333333333333333333333333333333333333333333333333
#define TWOTHIRD 0.666666666666666666666666666666666666666666666666666666666666

#ifdef OCCT_DEBUG_MESH_CHRONO
  #include <OSD_Chronometer.hxx>
int D0Control, D0Internal, D0Unif, D0Edges, NbControls;
OSD_Chronometer  chTotal, chInternal, chControl, chUnif, chAddPoint;
OSD_Chronometer  chEdges, chMaillEdges, chEtuInter, chLastControl, chStock;
OSD_Chronometer  chAdd11, chAdd12, chAdd2, chUpdate, chPointValid;
OSD_Chronometer  chIsos, chPointsOnIsos;
#endif

//=================================================================================================

static int incrementalmesh(Draw_Interpretor& theDI,
                                        int  theNbArgs,
                                        const char**      theArgVec)
{
  if (theNbArgs < 3)
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  NCollection_List<TopoDS_Shape>    aListOfShapes;
  IMeshTools_Parameters aMeshParams;
  bool                  hasDefl = false, hasAngDefl = false, isPrsDefl = false;

  occ::handle<IMeshTools_Context> aContext = new BRepMesh_Context();
  for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString aNameCase(theArgVec[anArgIter]);
    aNameCase.LowerCase();
    if (aNameCase == "-relative" || aNameCase == "-norelative")
    {
      aMeshParams.Relative = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aNameCase == "-parallel" || aNameCase == "-noparallel")
    {
      aMeshParams.InParallel = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aNameCase == "-int_vert_off")
    {
      aMeshParams.InternalVerticesMode = !Draw::ParseOnOffIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aNameCase == "-surf_def_off")
    {
      aMeshParams.ControlSurfaceDeflection =
        !Draw::ParseOnOffIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aNameCase == "-surf_def_all")
    {
      aMeshParams.EnableControlSurfaceDeflectionAllSurfaces =
        Draw::ParseOnOffIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aNameCase == "-adjust_min")
    {
      aMeshParams.AdjustMinSize = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aNameCase == "-force_face_def")
    {
      aMeshParams.ForceFaceDeflection = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aNameCase == "-decrease")
    {
      aMeshParams.AllowQualityDecrease =
        Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aNameCase == "-algo" && anArgIter + 1 < theNbArgs)
    {
      TCollection_AsciiString anAlgoStr(theArgVec[++anArgIter]);
      anAlgoStr.LowerCase();
      if (anAlgoStr == "watson" || anAlgoStr == "0")
      {
        aMeshParams.MeshAlgo = IMeshTools_MeshAlgoType_Watson;
        aContext->SetFaceDiscret(new BRepMesh_FaceDiscret(new BRepMesh_MeshAlgoFactory()));
      }
      else if (anAlgoStr == "delabella" || anAlgoStr == "1")
      {
        aMeshParams.MeshAlgo = IMeshTools_MeshAlgoType_Delabella;
        aContext->SetFaceDiscret(new BRepMesh_FaceDiscret(new BRepMesh_DelabellaMeshAlgoFactory()));
      }
      else if (anAlgoStr == "-1" || anAlgoStr == "default")
      {
        // already handled by BRepMesh_Context constructor
        // aMeshParams.MeshAlgo = IMeshTools_MeshAlgoType_DEFAULT;
      }
      else
      {
        theDI << "Syntax error at '" << anAlgoStr << "'";
        return 1;
      }
    }
    else if ((aNameCase == "-prs" || aNameCase == "-presentation" || aNameCase == "-vis"
              || aNameCase == "-visualization")
             && !isPrsDefl)
    {
      isPrsDefl = true;
    }
    else if ((aNameCase == "-angular" || aNameCase == "-angdefl"
              || aNameCase == "-angulardeflection" || aNameCase == "-a")
             && anArgIter + 1 < theNbArgs)
    {
      double aVal = Draw::Atof(theArgVec[++anArgIter]) * M_PI / 180.;
      if (aVal <= Precision::Angular())
      {
        theDI << "Syntax error: invalid input parameter '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      aMeshParams.Angle = aVal;
      hasAngDefl        = true;
    }
    else if (aNameCase == "-ai" && anArgIter + 1 < theNbArgs)
    {
      double aVal = Draw::Atof(theArgVec[++anArgIter]) * M_PI / 180.;
      if (aVal <= Precision::Angular())
      {
        theDI << "Syntax error: invalid input parameter '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      aMeshParams.AngleInterior = aVal;
    }
    else if (aNameCase == "-min" && anArgIter + 1 < theNbArgs)
    {
      double aVal = Draw::Atof(theArgVec[++anArgIter]);
      if (aVal <= Precision::Confusion())
      {
        theDI << "Syntax error: invalid input parameter '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      aMeshParams.MinSize = aVal;
    }
    else if (aNameCase == "-di" && anArgIter + 1 < theNbArgs)
    {
      double aVal = Draw::Atof(theArgVec[++anArgIter]);
      if (aVal <= Precision::Confusion())
      {
        theDI << "Syntax error: invalid input parameter '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      aMeshParams.DeflectionInterior = aVal;
    }
    else if (aNameCase.IsRealValue(true) && !hasDefl)
    {
      aMeshParams.Deflection = std::max(Draw::Atof(theArgVec[anArgIter]), Precision::Confusion());
      if (aMeshParams.DeflectionInterior < Precision::Confusion())
      {
        aMeshParams.DeflectionInterior = aMeshParams.Deflection;
      }
      hasDefl = true;
    }
    else
    {
      TopoDS_Shape aShape = DBRep::Get(theArgVec[anArgIter]);
      if (aShape.IsNull())
      {
        theDI << "Syntax error: null shapes are not allowed here '" << theArgVec[anArgIter]
              << "'\n";
        return 1;
      }
      aListOfShapes.Append(aShape);
    }
  }

  if (aListOfShapes.IsEmpty())
  {
    Message::SendFail("Syntax error: wrong number of arguments.");
    return 1;
  }

  theDI << "Incremental Mesh, multi-threading " << (aMeshParams.InParallel ? "ON" : "OFF") << "\n";

  TopoDS_Shape aShape;
  if (aListOfShapes.Size() == 1)
  {
    aShape = aListOfShapes.First();
  }
  else
  {
    TopoDS_Compound aCompound;
    BRep_Builder().MakeCompound(aCompound);
    for (NCollection_List<TopoDS_Shape>::Iterator anIterShape(aListOfShapes); anIterShape.More();
         anIterShape.Next())
    {
      BRep_Builder().Add(aCompound, anIterShape.Value());
    }
    aShape = aCompound;
  }

  if (isPrsDefl)
  {
    occ::handle<Prs3d_Drawer> aDrawer = new Prs3d_Drawer();
    if (hasDefl)
    {
      aDrawer->SetDeviationCoefficient(aMeshParams.Deflection);
    }
    aMeshParams.Deflection = StdPrs_ToolTriangulatedShape::GetDeflection(aShape, aDrawer);
    if (!hasAngDefl)
    {
      aMeshParams.Angle = aDrawer->DeviationAngle();
    }
  }

  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(theDI, 1);
  BRepMesh_IncrementalMesh       aMesher;
  aMesher.SetShape(aShape);
  aMesher.ChangeParameters() = aMeshParams;
  aMesher.Perform(aContext, aProgress->Start());

  theDI << "Meshing statuses: ";
  const int aStatus = aMesher.GetStatusFlags();
  if (aStatus == 0)
  {
    theDI << "NoError";
    return 0;
  }

  for (int i = 0; i < 9; i++)
  {
    int aFlag = aStatus & (1 << i);
    if (aFlag)
    {
      switch ((IMeshData_Status)aFlag)
      {
        case IMeshData_OpenWire:
          theDI << "OpenWire ";
          break;
        case IMeshData_SelfIntersectingWire:
          theDI << "SelfIntersectingWire ";
          break;
        case IMeshData_Failure:
          theDI << "Failure ";
          break;
        case IMeshData_ReMesh:
          theDI << "ReMesh ";
          break;
        case IMeshData_UnorientedWire:
          theDI << "UnorientedWire ";
          break;
        case IMeshData_TooFewPoints:
          theDI << "TooFewPoints ";
          break;
        case IMeshData_Outdated:
          theDI << "Outdated ";
          break;
        case IMeshData_Reused:
          theDI << "Reused ";
          break;
        case IMeshData_UserBreak:
          theDI << "UserBreak ";
          break;
        case IMeshData_NoError:
          break;
      }
    }
  }
  return 0;
}

//=================================================================================================

static int tessellate(Draw_Interpretor& /*di*/,
                                   int nbarg,
                                   const char**     argv)
{
  if (nbarg != 5)
  {
    Message::SendFail() << "Builds regular triangulation with specified number of triangles\n"
                           "    Usage: tessellate result {surface|face} nbu nbv\n"
                           "    Triangulation is put into the face with natural bounds (result);\n"
                           "    it will have 2*nbu*nbv triangles and (nbu+1)*(nbv+1) nodes";
    return 1;
  }

  const char* aResName = argv[1];
  const char* aSrcName = argv[2];
  int         aNbU     = Draw::Atoi(argv[3]);
  int         aNbV     = Draw::Atoi(argv[4]);

  if (aNbU <= 0 || aNbV <= 0)
  {
    Message::SendFail() << "Error: Arguments nbu and nbv must be both greater than 0";
    return 1;
  }

  occ::handle<Geom_Surface> aSurf = DrawTrSurf::GetSurface(aSrcName);
  double               aUMin, aUMax, aVMin, aVMax;
  if (!aSurf.IsNull())
  {
    aSurf->Bounds(aUMin, aUMax, aVMin, aVMax);
  }
  else
  {
    TopoDS_Shape aShape = DBRep::Get(aSrcName);
    if (aShape.IsNull() || aShape.ShapeType() != TopAbs_FACE)
    {
      Message::SendFail() << "Error: " << aSrcName << " is not a face";
      return 1;
    }
    TopoDS_Face aFace = TopoDS::Face(aShape);
    aSurf             = BRep_Tool::Surface(aFace);
    if (aSurf.IsNull())
    {
      Message::SendFail() << "Error: Face " << aSrcName << " has no surface";
      return 1;
    }

    BRepTools::UVBounds(aFace, aUMin, aUMax, aVMin, aVMax);
  }
  if (Precision::IsInfinite(aUMin) || Precision::IsInfinite(aUMax) || Precision::IsInfinite(aVMin)
      || Precision::IsInfinite(aVMax))
  {
    Message::SendFail() << "Error: surface has infinite parametric range, aborting";
    return 1;
  }

  BRepBuilderAPI_MakeFace aFaceMaker(aSurf, aUMin, aUMax, aVMin, aVMax, Precision::Confusion());
  if (!aFaceMaker.IsDone())
  {
    Message::SendFail() << "Error: cannot build face with natural bounds, aborting";
    return 1;
  }
  TopoDS_Face aFace = aFaceMaker;

  // create triangulation
  int                        aNbNodes     = (aNbU + 1) * (aNbV + 1);
  int                        aNbTriangles = 2 * aNbU * aNbV;
  occ::handle<Poly_Triangulation> aTriangulation =
    new Poly_Triangulation(aNbNodes, aNbTriangles, false);

  // fill nodes
  GeomAdaptor_Surface anAdSurf(aSurf);
  double              aDU = (aUMax - aUMin) / aNbU;
  double              aDV = (aVMax - aVMin) / aNbV;
  for (int iU = 0, iShift = 1; iU <= aNbU; iU++, iShift += aNbV + 1)
  {
    double aU = aUMin + iU * aDU;
    for (int iV = 0; iV <= aNbV; iV++)
    {
      double aV = aVMin + iV * aDV;
      gp_Pnt aP = anAdSurf.Value(aU, aV);
      aTriangulation->SetNode(iShift + iV, aP);
    }
  }

  // fill triangles
  for (int iU = 0, iShift = 1, iTri = 0; iU < aNbU; iU++, iShift += aNbV + 1)
  {
    for (int iV = 0; iV < aNbV; iV++)
    {
      int           iBase = iShift + iV;
      Poly_Triangle aTri1(iBase, iBase + aNbV + 2, iBase + 1);
      Poly_Triangle aTri2(iBase, iBase + aNbV + 1, iBase + aNbV + 2);
      aTriangulation->SetTriangle(++iTri, aTri1);
      aTriangulation->SetTriangle(++iTri, aTri2);
    }
  }

  // put triangulation to face
  BRep_Builder B;
  B.UpdateFace(aFace, aTriangulation);

  // fill edge polygons
  NCollection_Array1<int> aUMinIso(1, aNbV + 1), aUMaxIso(1, aNbV + 1);
  for (int iV = 0; iV <= aNbV; iV++)
  {
    aUMinIso.SetValue(1 + iV, 1 + iV);
    aUMaxIso.SetValue(1 + iV, 1 + iV + aNbU * (1 + aNbV));
  }
  NCollection_Array1<int> aVMinIso(1, aNbU + 1), aVMaxIso(1, aNbU + 1);
  for (int iU = 0; iU <= aNbU; iU++)
  {
    aVMinIso.SetValue(1 + iU, 1 + iU * (1 + aNbV));
    aVMaxIso.SetValue(1 + iU, (1 + iU) * (1 + aNbV));
  }
  occ::handle<Poly_PolygonOnTriangulation> aUMinPoly = new Poly_PolygonOnTriangulation(aUMinIso);
  occ::handle<Poly_PolygonOnTriangulation> aUMaxPoly = new Poly_PolygonOnTriangulation(aUMaxIso);
  occ::handle<Poly_PolygonOnTriangulation> aVMinPoly = new Poly_PolygonOnTriangulation(aVMinIso);
  occ::handle<Poly_PolygonOnTriangulation> aVMaxPoly = new Poly_PolygonOnTriangulation(aVMaxIso);
  for (TopExp_Explorer exp(aFace, TopAbs_EDGE); exp.More(); exp.Next())
  {
    TopoDS_Edge          anEdge = TopoDS::Edge(exp.Current());
    double        aFirst, aLast;
    occ::handle<Geom2d_Curve> aC      = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
    gp_Pnt2d             aPFirst = aC->Value(aFirst);
    gp_Pnt2d             aPLast  = aC->Value(aLast);
    if (std::abs(aPFirst.X() - aPLast.X()) < 0.1 * (aUMax - aUMin)) // U=const
    {
      if (BRep_Tool::IsClosed(anEdge, aFace))
        B.UpdateEdge(anEdge, aUMinPoly, aUMaxPoly, aTriangulation);
      else
        B.UpdateEdge(anEdge,
                     (aPFirst.X() < 0.5 * (aUMin + aUMax) ? aUMinPoly : aUMaxPoly),
                     aTriangulation);
    }
    else // V=const
    {
      if (BRep_Tool::IsClosed(anEdge, aFace))
        B.UpdateEdge(anEdge, aVMinPoly, aVMaxPoly, aTriangulation);
      else
        B.UpdateEdge(anEdge,
                     (aPFirst.Y() < 0.5 * (aVMin + aVMax) ? aVMinPoly : aVMaxPoly),
                     aTriangulation);
    }
  }

  DBRep::Set(aResName, aFace);
  return 0;
}

//=================================================================================================

static int MemLeakTest(Draw_Interpretor&,
                                    int /*nbarg*/,
                                    const char** /*argv*/)
{
  for (int i = 0; i < 10000; i++)
  {
    BRepBuilderAPI_MakePolygon w(gp_Pnt(0, 0, 0),
                                 gp_Pnt(0, 100, 0),
                                 gp_Pnt(20, 100, 0),
                                 gp_Pnt(20, 0, 0));
    w.Close();
    TopoDS_Wire              wireShape(w.Wire());
    BRepBuilderAPI_MakeFace  faceBuilder(wireShape);
    const TopoDS_Face&       f = faceBuilder.Face();
    BRepMesh_IncrementalMesh im(f, 1);
    BRepTools::Clean(f);
  }
  return 0;
}

//=================================================================================================

static int TrLateLoad(Draw_Interpretor& theDI,
                                   int  theNbArgs,
                                   const char**      theArgVec)
{
  if (theNbArgs < 3)
  {
    theDI << "Syntax error: not enough arguments\n";
    return 1;
  }
  TopoDS_Shape aShape = DBRep::Get(theArgVec[1]);
  if (aShape.IsNull())
  {
    theDI << "Syntax error: '" << theArgVec[1] << "' is not a shape\n";
    return 1;
  }
  for (int anArgIter = 2; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-load")
    {
      if (anArgIter + 1 < theNbArgs)
      {
        TCollection_AsciiString aLoadArg(theArgVec[anArgIter + 1]);
        aLoadArg.LowerCase();
        if (aLoadArg == "all" || aLoadArg == "*")
        {
          // Load all triangulations
          anArgIter++;
          if (BRepTools::LoadAllTriangulations(aShape))
          {
            theDI << "All triangulations of shape " << theArgVec[1] << " were loaded\n";
          }
          continue;
        }
        if (aLoadArg.IsIntegerValue())
        {
          // Load defined triangulation
          anArgIter++;
          int anIndexToLoad = aLoadArg.IntegerValue();
          if (anIndexToLoad < -1)
          {
            Message::SendWarning("Invalid negative triangulation index to be loaded");
            continue;
          }
          if (BRepTools::LoadTriangulation(aShape, anIndexToLoad))
          {
            theDI << "The " << anIndexToLoad << " triangulation of shape " << theArgVec[1]
                  << " was loaded\n";
          }
          continue;
        }
      }
      // Load active triangulation
      if (BRepTools::LoadTriangulation(aShape))
      {
        theDI << "The active triangulation of shape " << theArgVec[1] << " was loaded\n";
      }
      continue;
    }
    else if (anArgCase == "-unload")
    {
      if (anArgIter + 1 < theNbArgs)
      {
        TCollection_AsciiString anUnloadArg(theArgVec[anArgIter + 1]);
        anUnloadArg.LowerCase();
        if (anUnloadArg == "all" || anUnloadArg == "*")
        {
          // Unload all triangulations
          anArgIter++;
          if (BRepTools::UnloadAllTriangulations(aShape))
          {
            theDI << "All triangulations of shape " << theArgVec[1] << " were unloaded\n";
          }
          continue;
        }
        if (anUnloadArg.IsIntegerValue())
        {
          // Unload defined triangulation
          anArgIter++;
          int anIndexToUnload = anUnloadArg.IntegerValue();
          if (anIndexToUnload < -1)
          {
            Message::SendWarning("Invalid negative triangulation index to be unloaded");
            continue;
          }
          if (BRepTools::UnloadTriangulation(aShape, anIndexToUnload))
          {
            theDI << "The " << anIndexToUnload << " triangulation of shape " << theArgVec[1]
                  << " was unloaded\n";
          }
          continue;
        }
      }
      // Unload active triangulation
      if (BRepTools::UnloadTriangulation(aShape))
      {
        theDI << "The active triangulation of shape " << theArgVec[1] << " was unloaded\n";
      }
      continue;
    }
    else if (anArgIter + 1 < theNbArgs && anArgCase == "-activate"
             && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsIntegerValue())
    {
      int anIndexToActivate =
        TCollection_AsciiString(theArgVec[++anArgIter]).IntegerValue();
      if (anIndexToActivate < 0)
      {
        Message::SendWarning("Invalid negative triangulation index to be activated");
        continue;
      }
      if (BRepTools::ActivateTriangulation(aShape, anIndexToActivate, false))
      {
        theDI << "The " << anIndexToActivate << " triangulation of shape " << theArgVec[1]
              << " was activated\n";
      }
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArgCase == "-activatestrict" || anArgCase == "-activateexact")
             && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsIntegerValue())
    {
      int anIndexToActivate =
        TCollection_AsciiString(theArgVec[++anArgIter]).IntegerValue();
      if (anIndexToActivate < 0)
      {
        Message::SendWarning("Invalid negative triangulation index to be activated");
        continue;
      }
      if (BRepTools::ActivateTriangulation(aShape, anIndexToActivate, true))
      {
        theDI << "The " << anIndexToActivate << " triangulation of shape " << theArgVec[1]
              << " was activated\n";
      }
    }
    else if (anArgCase == "-loadsingle")
    {
      int anIndexToSingleLoad = -1;
      if (anArgIter + 1 < theNbArgs
          && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsIntegerValue())
      {
        anIndexToSingleLoad = TCollection_AsciiString(theArgVec[++anArgIter]).IntegerValue();
      }
      if (anIndexToSingleLoad < -1)
      {
        Message::SendWarning("Invalid negative triangulation index to be single loaded");
        continue;
      }
      // Unload all triangulations
      if (BRepTools::UnloadAllTriangulations(aShape))
      {
        theDI << "All triangulations of shape " << theArgVec[1] << " were unloaded\n";
      }
      // Activate required triangulation
      if (anIndexToSingleLoad > -1 && BRepTools::ActivateTriangulation(aShape, anIndexToSingleLoad))
      {
        theDI << "The " << anIndexToSingleLoad << " triangulation of shape " << theArgVec[1]
              << " was activated\n";
      }
      // Load active triangulation
      if (BRepTools::LoadTriangulation(aShape))
      {
        theDI << "The " << anIndexToSingleLoad << " triangulation of shape " << theArgVec[1]
              << " was loaded\n";
      }

      continue;
    }
    else if (anArgCase == "-loadsingleexact" || anArgCase == "-loadsinglestrict")
    {
      int anIndexToSingleLoad = -1;
      if (anArgIter + 1 < theNbArgs
          && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsIntegerValue())
      {
        anIndexToSingleLoad = TCollection_AsciiString(theArgVec[++anArgIter]).IntegerValue();
      }
      if (anIndexToSingleLoad <= -1)
      {
        Message::SendWarning("Invalid negative triangulation index to be single loaded");
        continue;
      }
      // Unload all triangulations
      if (BRepTools::UnloadAllTriangulations(aShape))
      {
        theDI << "All triangulations of shape " << theArgVec[1] << " were unloaded\n";
      }
      // Load required triangulation
      if (BRepTools::LoadTriangulation(aShape, anIndexToSingleLoad, true))
      {
        theDI << "The " << anIndexToSingleLoad << " triangulation of shape " << theArgVec[1]
              << " was loaded and activated\n";
      }
      continue;
    }
    else
    {
      theDI << "Syntax error: incorrect arguments";
      return 1;
    }
  }
  return 0;
}

//=================================================================================================

static int trianglesinfo(Draw_Interpretor& theDI,
                                      int  theNbArgs,
                                      const char**      theArgVec)
{
  if (theNbArgs < 2)
  {
    Message::SendFail("Syntax error: not enough arguments");
    return 1;
  }
  TopoDS_Shape aShape = DBRep::Get(theArgVec[1]);
  if (aShape.IsNull())
  {
    theDI << theArgVec[1] << " is not a shape\n";
    return 1;
  }

  struct TriangulationStat
  {
    TriangulationStat()
        : NbFaces(0),
          NbEmptyFaces(0),
          NbTriangles(0),
          NbDeferredFaces(0),
          NbUnloadedFaces(0),
          NbUnloadedTriangles(0)
    {
    }

    NCollection_IndexedDataMap<occ::handle<Standard_Type>, int> TypeMap;
    int                                                    NbFaces;
    int                                                    NbEmptyFaces;
    int                                                    NbTriangles;
    int                                                    NbDeferredFaces;
    int                                                    NbUnloadedFaces;
    int                                                    NbUnloadedTriangles;
  };

  bool toPrintLODs = false;
  if (theNbArgs > 2)
  {
    TCollection_AsciiString anArgCase(theArgVec[2]);
    anArgCase.LowerCase();
    if (anArgCase == "-lods")
    {
      toPrintLODs = true;
    }
  }

  TopExp_Explorer            anExp;
  occ::handle<Poly_Triangulation> aTriangulation;
  TopLoc_Location            aLoc;
  double              aMaxDeflection = 0.0, aMeshingDefl = -1.0, aMeshingAngDefl = -1.0,
                aMeshingMinSize = -1.0;
  int aNbFaces = 0, aNbEmptyFaces = 0, aNbTriangles = 0, aNbNodes = 0,
                   aNbRepresentations = 0;
  NCollection_IndexedDataMap<int, TriangulationStat> aLODsStat;
  NCollection_Vector<int>                            aNbLODs;
  for (anExp.Init(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    TopoDS_Face aFace = TopoDS::Face(anExp.Current());
    aNbFaces++;
    aTriangulation = BRep_Tool::Triangulation(aFace, aLoc);
    if (!aTriangulation.IsNull())
    {
      aNbTriangles += aTriangulation->NbTriangles();
      aNbNodes += aTriangulation->NbNodes();
      aMaxDeflection = std::max(aMaxDeflection, aTriangulation->Deflection());
      if (!aTriangulation->Parameters().IsNull())
      {
        aMeshingDefl    = std::max(aMeshingDefl, aTriangulation->Parameters()->Deflection());
        aMeshingAngDefl = std::max(aMeshingAngDefl, aTriangulation->Parameters()->Angle());
        aMeshingMinSize = std::max(aMeshingMinSize, aTriangulation->Parameters()->MinSize());
      }
    }
    else
    {
      aNbEmptyFaces++;
    }
    if (toPrintLODs)
    {
      // Collect LODs information
      const NCollection_List<occ::handle<Poly_Triangulation>>& aLODs = BRep_Tool::Triangulations(aFace, aLoc);
      if (aLODs.Size() != 0)
      {
        aNbLODs.Append(aLODs.Size());
      }
      int aTriangIndex = 0;
      for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(aLODs); anIter.More();
           anIter.Next(), ++aTriangIndex)
      {
        TriangulationStat* aStats = aLODsStat.ChangeSeek(aTriangIndex);
        if (aStats == NULL)
        {
          int aNewIndex = aLODsStat.Add(aTriangIndex, TriangulationStat());
          aStats                     = &aLODsStat.ChangeFromIndex(aNewIndex);
        }
        aStats->NbFaces++;
        const occ::handle<Poly_Triangulation>& aLOD = anIter.Value();
        if (aLOD.IsNull())
        {
          aStats->NbEmptyFaces++;
          continue;
        }
        int* aDynTypeCounter = aStats->TypeMap.ChangeSeek(aLOD->DynamicType());
        if (aDynTypeCounter == NULL)
        {
          int aNewIndex = aStats->TypeMap.Add(aLOD->DynamicType(), 0);
          aDynTypeCounter            = &aStats->TypeMap.ChangeFromIndex(aNewIndex);
        }
        (*aDynTypeCounter)++;
        if (aLOD->HasDeferredData())
        {
          aStats->NbDeferredFaces++;
          if (aLOD->HasGeometry())
          {
            aStats->NbTriangles += aLOD->NbTriangles();
          }
          else
          {
            aStats->NbUnloadedFaces++;
            aStats->NbTriangles += aLOD->NbDeferredTriangles();
            aStats->NbUnloadedTriangles += aLOD->NbDeferredTriangles();
          }
        }
        else
        {
          if (aLOD->HasGeometry())
          {
            aStats->NbTriangles += aLOD->NbTriangles();
          }
          else
          {
            aStats->NbEmptyFaces++;
          }
        }
      }
    }
  }
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  TopExp::MapShapes(aShape, TopAbs_EDGE, anEdges);
  for (int i = 1; i <= anEdges.Extent(); ++i)
  {
    const TopoDS_Edge&                    anEdge = TopoDS::Edge(anEdges(i));
    occ::handle<BRep_CurveRepresentation>      aCR;
    BRep_TEdge*                           aTE  = static_cast<BRep_TEdge*>(anEdge.TShape().get());
    const NCollection_List<occ::handle<BRep_CurveRepresentation>>& aLCR = aTE->Curves();
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator anIterCR(aLCR);

    while (anIterCR.More())
    {
      aCR = anIterCR.Value();
      if (aCR->IsPolygonOnTriangulation())
      {
        aNbRepresentations++;
      }
      anIterCR.Next();
    }
  }

  theDI << "\n";
  theDI << "This shape contains " << aNbFaces << " faces.\n";
  if (aNbEmptyFaces > 0)
  {
    theDI << "                    " << aNbEmptyFaces << " empty faces.\n";
  }
  theDI << "                    " << aNbTriangles << " triangles.\n";
  theDI << "                    " << aNbNodes << " nodes.\n";
  theDI << "                    " << aNbRepresentations << " polygons on triangulation.\n";
  theDI << "Maximal deflection " << aMaxDeflection << "\n";
  if (aMeshingDefl > 0.0)
  {
    theDI << "Meshing deflection " << aMeshingDefl << "\n";
  }
  if (aMeshingAngDefl > 0.0)
  {
    theDI << "Meshing angular deflection " << (aMeshingAngDefl * 180.0 / M_PI) << "\n";
  }
  if (aMeshingMinSize > 0.0)
  {
    theDI << "Meshing min size " << aMeshingMinSize << "\n";
  }

  if (aNbLODs.Size() > 0)
  {
    // Find all different numbers of triangulation LODs and their average value per face
    if (aNbLODs.Size() > 1)
    {
      std::sort(aNbLODs.begin(), aNbLODs.end());
    }
    NCollection_IndexedMap<int> aLODsRange;
    for (NCollection_Vector<int>::Iterator aNbIter(aNbLODs); aNbIter.More();
         aNbIter.Next())
    {
      if (!aLODsRange.Contains(aNbIter.Value()))
      {
        aLODsRange.Add(aNbIter.Value());
      }
    }
    TCollection_AsciiString aLODsRangeStr;
    int        anIndex = 0;
    for (NCollection_IndexedMap<int>::Iterator aRangeIter(aLODsRange);
         aRangeIter.More();
         aRangeIter.Next(), anIndex++)
    {
      aLODsRangeStr += TCollection_AsciiString(aRangeIter.Value());
      if (anIndex < aLODsRange.Size() - 1)
      {
        aLODsRangeStr += " ";
      }
    }
    theDI << TCollection_AsciiString("Number of triangulation LODs [") + aLODsRangeStr + "]\n";
    if (aLODsRange.Size() > 1)
    {
      // Find average number of triangulation LODs per face
      int aMedian = aNbLODs.Value(aNbLODs.Lower() + aNbLODs.Size() / 2);
      if ((aNbLODs.Size() % 2) == 0)
      {
        aMedian += aNbLODs.Value(aNbLODs.Lower() + aNbLODs.Size() / 2 - 1);
        aMedian /= 2;
      }
      theDI << TCollection_AsciiString("                             [average per face: ") + aMedian
                 + "]\n";
    }
  }
  if (!aLODsStat.IsEmpty())
  {
    TCollection_AsciiString aLODsStatStr;
    for (NCollection_IndexedDataMap<int, TriangulationStat>::Iterator anIter(
           aLODsStat);
         anIter.More();
         anIter.Next())
    {
      const TriangulationStat& aLodStat = anIter.Value();
      aLODsStatStr += TCollection_AsciiString("LOD #") + anIter.Key() + ". ";
      // aLODsStatStr += TCollection_AsciiString("NbFaces: ") + aLodStat.NbFaces;
      if (aLodStat.NbEmptyFaces > 0 || aLodStat.NbFaces < aNbFaces)
      {
        const int aNbEmpty = aLodStat.NbEmptyFaces + (aNbFaces - aLodStat.NbFaces);
        aLODsStatStr += TCollection_AsciiString("NbEmpty: ") + aNbEmpty + ", ";
      }
      aLODsStatStr += TCollection_AsciiString("NbTris: ") + aLodStat.NbTriangles;
      if (aLodStat.NbDeferredFaces > 0)
      {
        aLODsStatStr += TCollection_AsciiString(", NbDeferred: ") + aLodStat.NbDeferredFaces;
        if (aLodStat.NbUnloadedFaces > 0)
        {
          aLODsStatStr += TCollection_AsciiString(", NbUnloaded: ") + aLodStat.NbUnloadedFaces
                          + ", NbUnloadedTris: " + aLodStat.NbUnloadedTriangles;
        }
      }
      aLODsStatStr += ".\n";

      // Add types
      aLODsStatStr += TCollection_AsciiString("        Types: ");
      int aCounter = 0;
      for (NCollection_IndexedDataMap<occ::handle<Standard_Type>, int>::Iterator aTypeIter(
             aLodStat.TypeMap);
           aTypeIter.More();
           aTypeIter.Next(), aCounter++)
      {
        aLODsStatStr +=
          TCollection_AsciiString(aTypeIter.Key()->Name()) + " (" + aTypeIter.Value() + ")";
        if (aCounter < aLodStat.TypeMap.Size() - 1)
        {
          aLODsStatStr += TCollection_AsciiString(", ");
        }
      }
      aLODsStatStr += ".\n";
    }

    theDI << aLODsStatStr;
  }
  theDI << "\n";

#ifdef OCCT_DEBUG_MESH_CHRONO
  double tot, addp, unif, contr, inter;
  double edges, mailledges, etuinter, lastcontrol, stock;
  double add11, add12, add2, upda, pointvalid;
  double isos, pointsisos;
  chTotal.Show(tot);
  chAddPoint.Show(addp);
  chUnif.Show(unif);
  chControl.Show(contr);
  chInternal.Show(inter);
  chEdges.Show(edges);
  chMaillEdges.Show(mailledges);
  chEtuInter.Show(etuinter);
  chLastControl.Show(lastcontrol);
  chStock.Show(stock);
  chAdd11.Show(add11);
  chAdd12.Show(add12);
  chAdd2.Show(add2);
  chUpdate.Show(upda);
  chPointValid.Show(pointvalid);
  chIsos.Show(isos);
  chPointsOnIsos.Show(pointsisos);

  if (tot > 0.00001)
  {
    theDI << "temps total de maillage:     " << tot << " seconds\n";
    theDI << "dont: \n";
    theDI << "discretisation des edges:    " << edges << " seconds---> " << 100 * edges / tot
          << " %\n";
    theDI << "maillage des edges:          " << mailledges << " seconds---> "
          << 100 * mailledges / tot << " %\n";
    theDI << "controle et points internes: " << etuinter << " seconds---> " << 100 * etuinter / tot
          << " %\n";
    theDI << "derniers controles:          " << lastcontrol << " seconds---> "
          << 100 * lastcontrol / tot << " %\n";
    theDI << "stockage dans la S.D.        " << stock << " seconds---> " << 100 * stock / tot
          << " %\n";
    theDI << "\n";
    theDI << "et plus precisement: \n";
    theDI << "Add 11ere partie :           " << add11 << " seconds---> " << 100 * add11 / tot
          << " %\n";
    theDI << "Add 12ere partie :           " << add12 << " seconds---> " << 100 * add12 / tot
          << " %\n";
    theDI << "Add 2eme partie :            " << add2 << " seconds---> " << 100 * add2 / tot
          << " %\n";
    theDI << "Update :                     " << upda << " seconds---> " << 100 * upda / tot
          << " %\n";
    theDI << "AddPoint :                   " << addp << " seconds---> " << 100 * addp / tot
          << " %\n";
    theDI << "UniformDeflection            " << unif << " seconds---> " << 100 * unif / tot
          << " %\n";
    theDI << "Controle :                   " << contr << " seconds---> " << 100 * contr / tot
          << " %\n";
    theDI << "Points Internes:             " << inter << " seconds---> " << 100 * inter / tot
          << " %\n";
    theDI << "calcul des isos et du, dv:   " << isos << " seconds---> " << 100 * isos / tot
          << " %\n";
    theDI << "calcul des points sur isos:  " << pointsisos << " seconds---> "
          << 100 * pointsisos / tot << " %\n";
    theDI << "IsPointValid:                " << pointvalid << " seconds---> "
          << 100 * pointvalid / tot << " %\n";
    theDI << "\n";

    theDI << "nombre d'appels de controle apres points internes          : " << NbControls << "\n";
    theDI << "nombre de points sur restrictions                          : " << D0Edges << "\n";
    theDI << "nombre de points calcules par UniformDeflection            : " << D0Unif << "\n";
    theDI << "nombre de points calcules dans InternalVertices            : " << D0Internal << "\n";
    theDI << "nombre de points calcules dans Control                     : " << D0Control << "\n";
    if (nbnodes - D0Edges != 0)
    {
      double ratio =
        (double)(D0Internal + D0Control) / (double)(nbnodes - D0Edges);
      theDI << "---> Ratio: (D0Internal+D0Control) / (nbNodes-nbOnEdges)   : " << ratio << "\n";
    }

    theDI << "\n";

    chTotal.Reset();
    chAddPoint.Reset();
    chUnif.Reset();
    chControl.Reset();
    chInternal.Reset();
    chEdges.Reset();
    chMaillEdges.Reset();
    chEtuInter.Reset();
    chLastControl.Reset();
    chStock.Reset();
    chAdd11.Reset();
    chAdd12.Reset();
    chAdd2.Reset();
    chUpdate.Reset();
    chPointValid.Reset();
    chIsos.Reset();
    chPointsOnIsos.Reset();
  }
#endif
  return 0;
}

//=================================================================================================

static int veriftriangles(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
    return 1;
  bool quiet = 1;
  if (n == 3)
    quiet = 0;
  TopoDS_Shape Sh = DBRep::Get(a[1]);
  if (Sh.IsNull())
    return 1;
  TopExp_Explorer            ex;
  occ::handle<Poly_Triangulation> T;
  TopLoc_Location            L;
  int           i, n1, n2, n3;
  gp_Pnt2d                   mitri, v1, v2, v3, mi2d1, mi2d2, mi2d3;
  gp_XYZ                     vecEd1, vecEd2, vecEd3;
  //  double dipo, dm, dv, d1, d2, d3, defle;
  double        dipo, dv, d1, d2, d3, defle;
  occ::handle<Geom_Surface> S;
  int     nbface = 0;
  gp_Pnt               PP;

  for (ex.Init(Sh, TopAbs_FACE); ex.More(); ex.Next())
  {
    TopoDS_Face F = TopoDS::Face(ex.Current());
    nbface++;
    T                      = BRep_Tool::Triangulation(F, L);
    double deflemax = 0, deflemin = 1.e100;
    if (!T.IsNull())
    {
      double defstock = T->Deflection();

      S = BRep_Tool::Surface(F, L);

      for (i = 1; i <= T->NbTriangles(); i++)
      {
        if (F.Orientation() == TopAbs_REVERSED)
          T->Triangle(i).Get(n1, n3, n2);
        else
          T->Triangle(i).Get(n1, n2, n3);

        const gp_Pnt2d xy1 = T->UVNode(n1);
        const gp_Pnt2d xy2 = T->UVNode(n2);
        const gp_Pnt2d xy3 = T->UVNode(n3);

        mi2d1.SetCoord((xy2.X() + xy3.X()) * 0.5, (xy2.Y() + xy3.Y()) * 0.5);
        mi2d2.SetCoord((xy1.X() + xy3.X()) * 0.5, (xy1.Y() + xy3.Y()) * 0.5);
        mi2d3.SetCoord((xy1.X() + xy2.X()) * 0.5, (xy1.Y() + xy2.Y()) * 0.5);

        gp_XYZ p1 = T->Node(n1).Transformed(L.Transformation()).XYZ();
        gp_XYZ p2 = T->Node(n2).Transformed(L.Transformation()).XYZ();
        gp_XYZ p3 = T->Node(n3).Transformed(L.Transformation()).XYZ();

        vecEd1 = p2 - p1;
        vecEd2 = p3 - p2;
        vecEd3 = p1 - p3;
        d1     = vecEd1.SquareModulus();
        d2     = vecEd2.SquareModulus();
        d3     = vecEd3.SquareModulus();

        if (d1 != 0. && d2 != 0. && d3 != 0.)
        {
          gp_XYZ equa(vecEd1 ^ vecEd2);
          dv = equa.Modulus();
          if (dv > 0.)
          {
            equa.SetCoord(equa.X() / dv, equa.Y() / dv, equa.Z() / dv);
            dipo = equa * p1;

            mitri.SetCoord(ONETHIRD * (xy1.X() + xy2.X() + xy3.X()),
                           ONETHIRD * (xy1.Y() + xy2.Y() + xy3.Y()));
            v1.SetCoord(ONETHIRD * mi2d1.X() + TWOTHIRD * xy1.X(),
                        ONETHIRD * mi2d1.Y() + TWOTHIRD * xy1.Y());
            v2.SetCoord(ONETHIRD * mi2d2.X() + TWOTHIRD * xy2.X(),
                        ONETHIRD * mi2d2.Y() + TWOTHIRD * xy2.Y());
            v3.SetCoord(ONETHIRD * mi2d3.X() + TWOTHIRD * xy3.X(),
                        ONETHIRD * mi2d3.Y() + TWOTHIRD * xy3.Y());

            S->D0(mi2d1.X(), mi2d1.Y(), PP);
            PP       = PP.Transformed(L.Transformation());
            defle    = std::abs((equa * PP.XYZ()) - dipo);
            deflemax = std::max(deflemax, defle);
            deflemin = std::min(deflemin, defle);

            S->D0(mi2d2.X(), mi2d2.Y(), PP);
            PP       = PP.Transformed(L.Transformation());
            defle    = std::abs((equa * PP.XYZ()) - dipo);
            deflemax = std::max(deflemax, defle);
            deflemin = std::min(deflemin, defle);

            S->D0(mi2d3.X(), mi2d3.Y(), PP);
            PP       = PP.Transformed(L.Transformation());
            defle    = std::abs((equa * PP.XYZ()) - dipo);
            deflemax = std::max(deflemax, defle);
            deflemin = std::min(deflemin, defle);

            S->D0(v1.X(), v1.Y(), PP);
            PP       = PP.Transformed(L.Transformation());
            defle    = std::abs((equa * PP.XYZ()) - dipo);
            deflemax = std::max(deflemax, defle);
            deflemin = std::min(deflemin, defle);

            S->D0(v2.X(), v2.Y(), PP);
            PP       = PP.Transformed(L.Transformation());
            defle    = std::abs((equa * PP.XYZ()) - dipo);
            deflemax = std::max(deflemax, defle);
            deflemin = std::min(deflemin, defle);

            S->D0(v3.X(), v3.Y(), PP);
            PP       = PP.Transformed(L.Transformation());
            defle    = std::abs((equa * PP.XYZ()) - dipo);
            deflemax = std::max(deflemax, defle);
            deflemin = std::min(deflemin, defle);

            S->D0(mitri.X(), mitri.Y(), PP);
            PP       = PP.Transformed(L.Transformation());
            defle    = std::abs((equa * PP.XYZ()) - dipo);
            deflemax = std::max(deflemax, defle);
            deflemin = std::min(deflemin, defle);

            if (defle > defstock)
            {
              di << "face " << nbface << " deflection = " << defle << " pour " << defstock
                 << " stockee.\n";
            }
          }
        }
      }
      if (!quiet)
      {
        di << "face " << nbface << ", deflemin = " << deflemin << ", deflemax = " << deflemax
           << "\n";
      }
    }
  }

  return 0;
}

//=================================================================================================

static int tri2d(Draw_Interpretor&, int n, const char** a)
{

  if (n != 2)
    return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Face  F           = TopoDS::Face(aLocalShape);
  //  TopoDS_Face F = TopoDS::Face(DBRep::Get(a[1]));
  if (F.IsNull())
    return 1;
  occ::handle<Poly_Triangulation> T;
  TopLoc_Location            L;

  T = BRep_Tool::Triangulation(F, L);
  if (!T.IsNull())
  {
    // Build the connect tool
    Poly_Connect pc(T);

    int i, j, nFree, nInternal, nbTriangles = T->NbTriangles();
    int t[3];

    // count the free edges
    nFree = 0;
    for (i = 1; i <= nbTriangles; i++)
    {
      pc.Triangles(i, t[0], t[1], t[2]);
      for (j = 0; j < 3; j++)
        if (t[j] == 0)
          nFree++;
    }

    // allocate the arrays
    NCollection_Array1<int> Free(1, 2 * nFree);
    nInternal = (3 * nbTriangles - nFree) / 2;
    NCollection_Array1<int> Internal(0, 2 * nInternal);

    int fr = 1, in = 1;
    int nodes[3];
    for (i = 1; i <= nbTriangles; i++)
    {
      pc.Triangles(i, t[0], t[1], t[2]);
      T->Triangle(i).Get(nodes[0], nodes[1], nodes[2]);
      for (j = 0; j < 3; j++)
      {
        int k = (j + 1) % 3;
        if (t[j] == 0)
        {
          Free(fr)     = nodes[j];
          Free(fr + 1) = nodes[k];
          fr += 2;
        }
        // internal edge if this triangle has a lower index than the adjacent
        else if (i < t[j])
        {
          Internal(in)     = nodes[j];
          Internal(in + 1) = nodes[k];
          in += 2;
        }
      }
    }

    // Display the edges
    if (T->HasUVNodes())
    {
      occ::handle<Draw_Segment2D> Seg;

      // free edges
      int nn;
      nn = Free.Length() / 2;
      for (i = 1; i <= nn; i++)
      {
        Seg = new Draw_Segment2D(T->UVNode(Free[2 * i - 1]), T->UVNode(Free[2 * i]), Draw_rouge);
        dout << Seg;
      }

      // internal edges

      nn = nInternal;
      for (i = 1; i <= nn; i++)
      {
        Seg =
          new Draw_Segment2D(T->UVNode(Internal[2 * i - 1]), T->UVNode(Internal[2 * i]), Draw_bleu);
        dout << Seg;
      }
    }
    dout.Flush();
  }

  return 0;
}

//=================================================================================================

static int wavefront(Draw_Interpretor&, int nbarg, const char** argv)
{
  if (nbarg < 2)
    return 1;

  TopoDS_Shape S = DBRep::Get(argv[1]);
  if (S.IsNull())
    return 1;

  // creation du maillage s'il n'existe pas.

  Bnd_Box       B;
  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  BRepBndLib::Add(S, B);
  B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  double aDeflection = MAX3(aXmax - aXmin, aYmax - aYmin, aZmax - aZmin) * 0.004;

  BRepMesh_IncrementalMesh aMesh(S, aDeflection);

  TopLoc_Location L;
  TopExp_Explorer ex;

  int       i, nbface = 0;
  bool       OK = true;
  gp_Vec                 D1U, D1V;
  gp_Vec                 D2U, D2V, D2UV;
  gp_Dir                 Nor;
  gp_Pnt                 P;
  double          U, V;
  CSLib_DerivativeStatus aStatus;
  CSLib_NormalStatus     NStat;
  double          x, y, z;
  int       n1, n2, n3;
  int       k1, k2, k3;

  TCollection_AsciiString aFile;

  if (nbarg == 3)
  {
    aFile = argv[2];
    aFile += ".obj";
  }
  else
    aFile = "wave.obj";
  FILE* outfile = OSD_OpenFile(aFile.ToCString(), "w");

  fprintf(outfile, "%s  %s\n%s %s\n\n", "# CASCADE   ", "MATRA DATAVISION", "#", aFile.ToCString());

  int nbNodes, totalnodes = 0, nbpolygons = 0;
  for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next())
  {
    nbface++;
    TopoDS_Face                F  = TopoDS::Face(ex.Current());
    occ::handle<Poly_Triangulation> Tr = BRep_Tool::Triangulation(F, L);

    if (!Tr.IsNull())
    {
      nbNodes = Tr->NbNodes();

      // les noeuds.
      for (i = 1; i <= nbNodes; i++)
      {
        gp_Pnt Pnt = Tr->Node(i).Transformed(L.Transformation());
        x          = Pnt.X();
        y          = Pnt.Y();
        z          = Pnt.Z();
        fprintf(outfile, "%s      %f  %f  %f\n", "v", x, y, z);
      }

      fprintf(outfile, "\n%s    %d\n\n", "# number of vertex", nbNodes);

      // les normales.

      if (Tr->HasUVNodes())
      {
        BRepAdaptor_Surface BS(F, false);

        for (i = 1; i <= nbNodes; i++)
        {
          U = Tr->UVNode(i).X();
          V = Tr->UVNode(i).Y();

          BS.D1(U, V, P, D1U, D1V);
          CSLib::Normal(D1U, D1V, Precision::Angular(), aStatus, Nor);
          if (aStatus != CSLib_Done)
          {
            BS.D2(U, V, P, D1U, D1V, D2U, D2V, D2UV);
            CSLib::Normal(D1U, D1V, D2U, D2V, D2UV, Precision::Angular(), OK, NStat, Nor);
          }
          if (F.Orientation() == TopAbs_REVERSED)
            Nor.Reverse();

          fprintf(outfile, "%s      %f  %f  %f\n", "vn", Nor.X(), Nor.Y(), Nor.Z());
        }

        fprintf(outfile, "\n%s    %d\n\n", "# number of vertex normals", nbNodes);
      }

      fprintf(outfile, "%s    %d\n", "s", nbface);

      // les triangles.
      int nbTriangles = Tr->NbTriangles();

      for (i = 1; i <= nbTriangles; i++)
      {
        if (F.Orientation() == TopAbs_REVERSED)
          Tr->Triangle(i).Get(n1, n3, n2);
        else
          Tr->Triangle(i).Get(n1, n2, n3);
        k1 = n1 + totalnodes;
        k2 = n2 + totalnodes;
        k3 = n3 + totalnodes;
        fprintf(outfile, "f %d%s%d %d%s%d %d%s%d\n", k1, "//", k1, k2, "//", k2, k3, "//", k3);
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

//=================================================================================================

static int triedgepoints(Draw_Interpretor& di,
                                      int  nbarg,
                                      const char**      argv)
{
  if (nbarg < 2)
    return 1;

  for (int i = 1; i < nbarg; i++)
  {
    TopoDS_Shape aShape = DBRep::Get(argv[i]);
    if (aShape.IsNull())
      continue;

    occ::handle<Poly_PolygonOnTriangulation> aPoly;
    occ::handle<Poly_Triangulation>          aT;
    TopLoc_Location                     aLoc;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                 anEdgeMap;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator    it;

    if (aShape.ShapeType() == TopAbs_EDGE)
    {
      anEdgeMap.Add(aShape);
    }
    else
    {
      TopExp_Explorer ex(aShape, TopAbs_EDGE);
      for (; ex.More(); ex.Next())
        anEdgeMap.Add(ex.Current());
    }

    if (anEdgeMap.Extent() == 0)
      continue;

    char newname[1024];
    strcpy(newname, argv[i]);
    char* p = newname;
    while (*p != '\0')
      p++;
    *p = '_';
    p++;

    int nbEdge = 1;
    for (it.Initialize(anEdgeMap); it.More(); it.Next())
    {
      BRep_Tool::PolygonOnTriangulation(TopoDS::Edge(it.Key()), aPoly, aT, aLoc);
      if (aT.IsNull() || aPoly.IsNull())
        continue;

      const NCollection_Array1<int>& Indices = aPoly->Nodes();
      const int         nbnodes = Indices.Length();

      for (int j = 1; j <= nbnodes; j++)
      {
        gp_Pnt P3d = aT->Node(Indices[j]);
        if (!aLoc.IsIdentity())
          P3d.Transform(aLoc.Transformation());

        if (anEdgeMap.Extent() > 1)
          Sprintf(p, "%d_%d", nbEdge, j);
        else
          Sprintf(p, "%d", j);
        DBRep::Set(newname, BRepBuilderAPI_MakeVertex(P3d));
        di.AppendElement(newname);
      }
      nbEdge++;
    }
  }
  return 0;
}

//=================================================================================================

static int TrMergeNodes(Draw_Interpretor& theDI,
                                     int  theNbArgs,
                                     const char**      theArgVec)
{
  if (theNbArgs < 2)
  {
    theDI << "Syntax error: not enough arguments";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(theArgVec[1]);
  if (aShape.IsNull())
  {
    theDI << "Syntax error: '" << theArgVec[1] << "' is not a shape";
    return 1;
  }

  double           aMergeAngle = M_PI / 4.0, aMergeToler = 0.0;
  bool                    toForce = false;
  TCollection_AsciiString aResFace;
  for (int anArgIter = 2; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgIter + 1 < theNbArgs
        && (anArgCase == "-angle" || anArgCase == "-smoothangle" || anArgCase == "-mergeangle")
        && Draw::ParseReal(theArgVec[anArgIter + 1], aMergeAngle))
    {
      if (aMergeAngle < 0.0 || aMergeAngle > 90.0)
      {
        theDI << "Syntax error: angle should be within [0,90] range";
        return 1;
      }

      ++anArgIter;
      aMergeAngle = aMergeAngle * M_PI / 180.0;
    }
    else if (anArgIter + 1 < theNbArgs && anArgCase == "-tolerance"
             && Draw::ParseReal(theArgVec[anArgIter + 1], aMergeToler))
    {
      if (aMergeToler < 0.0)
      {
        theDI << "Syntax error: tolerance should be within >=0";
        return 1;
      }

      ++anArgIter;
    }
    else if (anArgCase == "-force")
    {
      toForce = Draw::ParseOnOffIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (anArgIter + 1 < theNbArgs && anArgCase == "-oneface")
    {
      aResFace = theArgVec[++anArgIter];
    }
    else
    {
      theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  int aNbNodesOld = 0, aNbTrisOld = 0;
  int aNbNodesNew = 0, aNbTrisNew = 0;
  if (!aResFace.IsEmpty())
  {
    TopLoc_Location     aFaceLoc;
    Poly_MergeNodesTool aMergeTool(aMergeAngle, aMergeToler);
    for (TopExp_Explorer aFaceIter(aShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
    {
      const TopoDS_Face&         aFace = TopoDS::Face(aFaceIter.Value());
      occ::handle<Poly_Triangulation> aTris = BRep_Tool::Triangulation(aFace, aFaceLoc);
      if (aTris.IsNull() || aTris->NbNodes() < 3 || aTris->NbTriangles() < 1)
      {
        continue;
      }

      aNbNodesOld += aTris->NbNodes();
      aNbTrisOld += aTris->NbTriangles();
      aMergeTool.AddTriangulation(aTris, aFaceLoc, aFace.Orientation() == TopAbs_REVERSED);
    }
    occ::handle<Poly_Triangulation> aNewTris = aMergeTool.Result();
    if (aNewTris.IsNull())
    {
      theDI << "Error: empty result";
      return 0;
    }

    aNbNodesNew += aNewTris->NbNodes();
    aNbTrisNew += aNewTris->NbTriangles();
    TopoDS_Face aFace;
    BRep_Builder().MakeFace(aFace, aNewTris);
    DBRep::Set(aResFace.ToCString(), aFace);
  }
  else
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aProcessedFaces;
    TopLoc_Location     aDummy;
    for (TopExp_Explorer aFaceIter(aShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(aFaceIter.Value());
      if (!aProcessedFaces.Add(aFace.Located(TopLoc_Location())))
      {
        continue;
      }

      occ::handle<Poly_Triangulation> aTris = BRep_Tool::Triangulation(aFace, aDummy);
      if (aTris.IsNull() || aTris->NbNodes() < 3 || aTris->NbTriangles() < 1)
      {
        continue;
      }

      aNbNodesOld += aTris->NbNodes();
      aNbTrisOld += aTris->NbTriangles();
      Poly_MergeNodesTool aMergeTool(aMergeAngle, aMergeToler, aTris->NbTriangles());
      aMergeTool.AddTriangulation(aTris);
      if (toForce || aMergeTool.NbNodes() != aTris->NbNodes()
          || aMergeTool.NbElements() != aTris->NbTriangles())
      {
        BRep_Builder().UpdateFace(aFace, aMergeTool.Result(), false);
      }

      aTris = BRep_Tool::Triangulation(aFace, aDummy);
      aNbNodesNew += aTris->NbNodes();
      aNbTrisNew += aTris->NbTriangles();
    }
  }
  theDI << "Old, Triangles: " << aNbTrisOld << ", Nodes: " << aNbNodesOld << "\n";
  theDI << "New, Triangles: " << aNbTrisNew << ", Nodes: " << aNbNodesNew << "\n";
  return 0;
}

//=======================================================================
// function : correctnormals
// purpose  : Corrects normals in shape triangulation nodes (...)
//=======================================================================
static int correctnormals(Draw_Interpretor& theDI,
                                       int /*theNArg*/,
                                       const char** theArgVal)
{
  TopoDS_Shape S = DBRep::Get(theArgVal[1]);

  // Use "correctnormals shape"

  if (!BRepLib::EnsureNormalConsistency(S))
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
void MeshTest::Commands(Draw_Interpretor& theCommands)
//=======================================================================
{
  Draw::Commands(theCommands);
  BRepTest::AllCommands(theCommands);
  GeometryTest::AllCommands(theCommands);
  MeshTest::PluginCommands(theCommands);
  const char* g;

  g = "Mesh Commands";

  theCommands.Add(
    "incmesh",
    "incmesh Shape LinDefl [-angular Angle]=28.64 [-prs]"
    "\n\t\t:   [-relative {0|1}]=0 [-parallel {0|1}]=0 [-min Size]"
    "\n\t\t:   [-algo {watson|delabella}]=watson"
    "\n\t\t:   [-di Value] [-ai Angle]=57.29"
    "\n\t\t:   [-int_vert_off {0|1}]=0 [-surf_def_off {0|1}]=0 [-adjust_min {0|1}]=0"
    "\n\t\t:   [-force_face_def {0|1}]=0 [-decrease {0|1}]=0"
    "\n\t\t: Builds triangular mesh for the shape."
    "\n\t\t:  LinDefl         linear deflection to control mesh quality;"
    "\n\t\t:  -angular        angular deflection for edges in deg (~28.64 deg = 0.5 rad by "
    "default);"
    "\n\t\t:  -prs            apply default meshing parameters for visualization purposes"
    "\n\t\t:                  (20 deg angular deflection, 0.001 of bounding box linear deflection);"
    "\n\t\t:  -relative       notifies that relative deflection is used (FALSE by default);"
    "\n\t\t:  -parallel       enables parallel execution (FALSE by default);"
    "\n\t\t:  -algo           changes core triangulation algorithm to one with specified id "
    "(watson by default);"
    "\n\t\t:  -min            minimum size parameter limiting size of triangle's edges to prevent "
    "sinking"
    "\n\t\t:                  into amplification in case of distorted curves and surfaces;"
    "\n\t\t:  -di             linear deflection used to tessellate the face interior;"
    "\n\t\t:  -ai             angular deflection inside of faces in deg (~57.29 deg = 1 rad by "
    "default);"
    "\n\t\t:  -int_vert_off   disables insertion of internal vertices into mesh (enabled by "
    "default);"
    "\n\t\t:  -surf_def_off   disables control of deflection of mesh from real surface (enabled by "
    "default);"
    "\n\t\t:  -surf_def_all   enables control of deflection of mesh from real surface for all "
    "types of surfaces"
    "\n\t\t:                  (FALSE by default);"
    "\n\t\t:  -adjust_min     enables local adjustment of min size depending on edge size (FALSE "
    "by default);"
    "\n\t\t:  -force_face_def disables usage of shape tolerances for computing face deflection "
    "(FALSE by default);"
    "\n\t\t:  -decrease       enforces the meshing of the shape even if current mesh satisfies the "
    "new criteria"
    "\n\t\t:                  (FALSE by default).",
    __FILE__,
    incrementalmesh,
    g);
  theCommands.Add("tessellate",
                  "Builds triangular mesh for the surface, run w/o args for help",
                  __FILE__,
                  tessellate,
                  g);
  theCommands.Add("MemLeakTest", "MemLeakTest", __FILE__, MemLeakTest, g);

  theCommands.Add("tri2d", "tri2d facename", __FILE__, tri2d, g);
  theCommands.Add("trinfo",
                  "trinfo shapeName [-lods], print triangles information on objects"
                  "\n\t\t: -lods Print detailed LOD information",
                  __FILE__,
                  trianglesinfo,
                  g);
  theCommands.Add("veriftriangles",
                  "veriftriangles name, verif triangles",
                  __FILE__,
                  veriftriangles,
                  g);
  theCommands.Add("wavefront", "wavefront name", __FILE__, wavefront, g);
  theCommands.Add("triepoints", "triepoints shape1 [shape2 ...]", __FILE__, triedgepoints, g);
  theCommands.Add("trlateload",
                  "trlateload shapeName"
                  "\n\t\t:   [-load {-1|Index|ALL}=-1] [-unload {-1|Index|ALL}=-1]"
                  "\n\t\t:   [-activate Index] [-activateExact Index]"
                  "\n\t\t:   [-loadSingle {-1|Index}=-1] [-loadSingleExact {Index}=-1]"
                  "\n\t\t: Interaction with deferred triangulations."
                  "\n\t\t:   '-load'            - load triangulation (-1 - currently active one, "
                  "Index - with defined index,"
                  "\n\t\t:                      ALL - all available ones)"
                  "\n\t\t:   '-unload'          - unload triangulation (-1 - currently active one, "
                  "Index - with defined index,"
                  "\n\t\t:                      ALL - all available ones)"
                  "\n\t\t:   '-activate'        - activate triangulation with defined index. If it "
                  "doesn't exist -"
                  "\n\t\t:                      activate the last available triangulation."
                  "\n\t\t:   '-activateExact'   - activate exactly triangulation with defined "
                  "index or do nothing."
                  "\n\t\t:   '-loadSingle'      - make loaded and active ONLY specified "
                  "triangulation (-1 - currently active one,"
                  "\n\t\t:                      Index - with defined index or last available if it "
                  "doesn't exist)."
                  "\n\t\t:                      All other triangulations will be unloaded."
                  "\n\t\t:   '-loadSingleExact' - make loaded and active ONLY exactly specified "
                  "triangulation. All other triangulations"
                  "\n\t\t:                      will be unloaded. If triangulation with such Index "
                  "doesn't exist do nothing",
                  __FILE__,
                  TrLateLoad,
                  g);
  theCommands.Add(
    "trmergenodes",
    "trmergenodes shapeName"
    "\n\t\t:   [-angle Angle] [-tolerance Value] [-oneFace Result]"
    "\n\t\t: Merging nodes within triangulation data."
    "\n\t\t:   -angle     merge angle upper limit in degrees; 45 when unspecified"
    "\n\t\t:   -tolerance linear tolerance to merge nodes; 0.0 when unspecified"
    "\n\t\t:   -oneFace   create a new single Face with specified name for the whole triangulation",
    __FILE__,
    TrMergeNodes,
    g);
  theCommands.Add("correctnormals", "correctnormals shape", __FILE__, correctnormals, g);
}
