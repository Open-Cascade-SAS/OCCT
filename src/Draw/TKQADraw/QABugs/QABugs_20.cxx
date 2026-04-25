// Created on: 2015-10-26
// Created by: Nikolai BUKHALOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <QABugs.hxx>

#include <Extrema_GenLocateExtPS.hxx>
#include <Geom_Circle.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Array1.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <Geom2d_Line.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TopoDS.hxx>
#include <DBRep.hxx>

#include <BRepGProp.hxx>
#include <DDocStd.hxx>
#include <GProp_GProps.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <HLRBRep_PolyHLRToShape.hxx>
#include <HLRBRep_PolyAlgo.hxx>

#include <Standard_Failure.hxx>

#include <Bnd_OBB.hxx>
#include <BRepBndLib.hxx>
#include <OSD_Timer.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_Name.hxx>
#include <AppCont_Function.hxx>
#include <math_ComputeKronrodPointsAndWeights.hxx>

#include <limits>

//=======================================================================
// function : OCC27021
// purpose  : Tests performance of obtaining geometry (points) via topological
//           exploring or fetching the geometry.
//=======================================================================

// Fetch via topology
static std::pair<gp_Pnt, gp_Pnt> getVerticesA(const TopoDS_Edge& theEdge)
{
  std::pair<gp_Pnt, gp_Pnt> result;

  static TopoDS_Vertex aFirst, aLast;
  TopExp::Vertices(theEdge, aFirst, aLast, true);

  result.first  = BRep_Tool::Pnt(aFirst);
  result.second = BRep_Tool::Pnt(aLast);

  return result;
}

// Geometrical way
static std::pair<gp_Pnt, gp_Pnt> getVerticesB(const TopoDS_Edge& theEdge)
{
  double first;
  double last;

  occ::handle<Geom_Curve> curve = BRep_Tool::Curve(theEdge, first, last);

  std::pair<gp_Pnt, gp_Pnt> result;

  if (theEdge.Orientation() == TopAbs_REVERSED)
  {
    curve->D0(first, result.second);
    curve->D0(last, result.first);
  }
  else
  {
    curve->D0(first, result.first);
    curve->D0(last, result.second);
  }
  return result;
}

static int OCC27021(Draw_Interpretor& theDI, int theNArg, const char** theArgVal)
{
  if (theNArg != 2)
  {
    std::cout << "Use: " << theArgVal[0] << " shape" << std::endl;
    return 1;
  }

  TopoDS_Shape shape(DBRep::Get(theArgVal[1]));

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> shape_faces;
  TopExp::MapShapes(shape, TopAbs_FACE, shape_faces);

  // Pick a single face which shows the problem.
  TopoDS_Face face = TopoDS::Face(shape_faces(10));
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> face_edges;
  TopExp::MapShapes(face, TopAbs_EDGE, face_edges);
  TopoDS_Edge edge = TopoDS::Edge(face_edges(2));

  int iterations = 100000000;

  std::pair<gp_Pnt, gp_Pnt> vertices;
  clock_t                   t = clock();

  theDI << "\nRetrieving " << iterations << " vertices using approach A)...\n";
  for (int i = 0; i < iterations; ++i)
  {
    vertices = getVerticesA(edge);
  }
  theDI << "COUNTER RetrievingVertA" << ": " << (clock() - t) / (double)CLOCKS_PER_SEC << "\n";
  t = clock();

  theDI << "\nRetrieving " << iterations << " vertices using approach B)...\n";
  for (int i = 0; i < iterations; ++i)
  {
    vertices = getVerticesB(edge);
  }
  theDI << "COUNTER RetrievingVertB" << ": " << (clock() - t) / (double)CLOCKS_PER_SEC << "\n";

  return 0;
}

//=======================================================================
// function : OCC27235
// purpose : check presentation in GDT document
//=======================================================================
static int OCC27235(Draw_Interpretor& theDI, int n, const char** a)
{
  if (n < 2)
  {
    theDI << "Use: OCC27235 Doc";
    return 1;
  }

  occ::handle<TDocStd_Document> Doc;
  DDocStd::GetDocument(a[1], Doc);
  if (Doc.IsNull())
  {
    theDI << a[1] << " is not a document\n";
    return 1;
  }
  occ::handle<XCAFDoc_DimTolTool> aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(Doc->Main());
  occ::handle<XCAFDoc_ShapeTool>  aShapeTool  = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TopoDS_Compound                 aPresentations;
  BRep_Builder                    B;
  B.MakeCompound(aPresentations);

  NCollection_Sequence<TDF_Label> aLabels;
  aShapeTool->GetShapes(aLabels);
  for (int i = 1; i <= aLabels.Length(); i++)
  {
    aShapeTool->GetSubShapes(aLabels.Value(i), aLabels);
  }

  NCollection_Sequence<TDF_Label> aGDTs;
  aDimTolTool->GetDimensionLabels(aGDTs);
  for (int i = 1; i <= aGDTs.Length(); i++)
  {
    occ::handle<XCAFDoc_Dimension> aDimAttr;
    if (!aGDTs.Value(i).FindAttribute(XCAFDoc_Dimension::GetID(), aDimAttr))
      continue;
    occ::handle<XCAFDimTolObjects_DimensionObject> anObject = aDimAttr->GetObject();
    if (anObject.IsNull())
      continue;
    TopoDS_Shape aShape = anObject->GetPresentation();
    if (!aShape.IsNull())
      B.Add(aPresentations, aShape);
  }

  aGDTs.Clear();
  aDimTolTool->GetGeomToleranceLabels(aGDTs);
  for (int i = 1; i <= aGDTs.Length(); i++)
  {
    occ::handle<XCAFDoc_GeomTolerance> aGTAttr;
    if (!aGDTs.Value(i).FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGTAttr))
      continue;
    occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject = aGTAttr->GetObject();
    if (anObject.IsNull())
      continue;
    TopoDS_Shape aShape = anObject->GetPresentation();
    if (!aShape.IsNull())
      B.Add(aPresentations, aShape);
  }

  for (int i = 1; i <= aLabels.Length(); i++)
  {
    NCollection_Sequence<TDF_Label> aDatL;
    if (aDimTolTool->GetRefDatumLabel(aLabels.Value(i), aDatL))
    {
      for (int j = NCollection_Sequence<TDF_Label>::Lower(); j <= aDatL.Upper(); j++)
      {
        occ::handle<XCAFDoc_Datum> aDat;
        if (!aDatL.Value(j).FindAttribute(XCAFDoc_Datum::GetID(), aDat))
          continue;
        occ::handle<XCAFDimTolObjects_DatumObject> anObject = aDat->GetObject();
        if (anObject.IsNull())
          continue;
        TopoDS_Shape aShape = anObject->GetPresentation();
        if (!aShape.IsNull())
          B.Add(aPresentations, aShape);
      }
    }
  }

  GProp_GProps aG;
  BRepGProp::LinearProperties(aPresentations, aG);
  gp_Pnt aPnt = aG.CentreOfMass();
  theDI << "Centre of mass: " << aPnt.X() << " " << aPnt.Y() << " " << aPnt.Z() << "\n";
  theDI << "Mass: " << aG.Mass() << "\n";

  return 0;
}

//=================================================================================================

static int OCC26930(Draw_Interpretor& theDI, int theNArg, const char** theArgVal)
{
  if (theNArg != 5)
  {
    std::cout << "Use: " << theArgVal[0] << " surface curve start end" << std::endl;
    return 1;
  }

  occ::handle<Geom_Surface> aSurface = DrawTrSurf::GetSurface(theArgVal[1]);
  occ::handle<Geom_Curve>   aCurve   = DrawTrSurf::GetCurve(theArgVal[2]);
  double                    aStart   = Draw::Atof(theArgVal[3]);
  double                    anEnd    = Draw::Atof(theArgVal[4]);

  // project
  occ::handle<Geom2d_Curve> aPCurve;

  ShapeConstruct_ProjectCurveOnSurface aProj;
  aProj.Init(aSurface, Precision::Confusion());
  {
    try
    {
      occ::handle<Geom_Curve> aTmpCurve = aCurve; // to use reference in Perform()
      aProj.Perform(aTmpCurve, aStart, anEnd, aPCurve);
    }
    catch (const Standard_Failure&)
    {
    }
  }

  // check results
  if (aPCurve.IsNull())
  {
    theDI << "Error: pcurve is null\n";
  }
  else
  {
    if (aPCurve->IsKind(STANDARD_TYPE(Geom2d_Line)))
    {
      theDI << "Pcurve is line: OK\n";
    }
    else
    {
      theDI << "Error: PCurve is not line\n";
    }
  }

  return 0;
}

//=================================================================================================

static int OCC27466(Draw_Interpretor& theDI, int theNArg, const char** theArgVal)
{
  if (theNArg != 4)
  {
    std::cout << "Use: " << theArgVal[0] << " face point start_pnt2d" << std::endl;
    return 1;
  }

  TopoDS_Face aFace = TopoDS::Face(DBRep::Get(theArgVal[1], TopAbs_FACE, true));
  if (aFace.IsNull())
    return 1;
  gp_Pnt aPnt;
  if (!DrawTrSurf::GetPoint(theArgVal[2], aPnt))
    return 1;
  gp_Pnt2d aUV;
  if (!DrawTrSurf::GetPoint2d(theArgVal[3], aUV))
    return 1;
  BRepAdaptor_Surface aSurf(aFace);

  constexpr double aTolU = Precision::PConfusion();
  constexpr double aTolV = Precision::PConfusion();

  Extrema_GenLocateExtPS anExtrema(aSurf, aTolU, aTolV);
  anExtrema.Perform(aPnt, aUV.X(), aUV.Y(), true);

  if (!anExtrema.IsDone())
  {
    theDI << "Error: Extrema is not done";
  }
  else
  {
    double aSqDist = anExtrema.SquareDistance();
    gp_Pnt aResPnt = anExtrema.Point().Value();
    double u, v;
    anExtrema.Point().Parameter(u, v);
    gp_Pnt2d aResUV(u, v);
    DrawTrSurf::Set((TCollection_AsciiString(theArgVal[2]) + "_res").ToCString(), aResPnt);
    DrawTrSurf::Set((TCollection_AsciiString(theArgVal[3]) + "_res").ToCString(), aResUV);
    theDI << theArgVal[2] << "_res and " << theArgVal[3]
          << "_res are created, dist=" << sqrt(aSqDist);
  }
  return 0;
}

#include "Geom2d_BezierCurve.hxx"
#include "Geom2dGcc_QualifiedCurve.hxx"
#include "Geom2dAdaptor_Curve.hxx"

#include <Standard_ErrorHandler.hxx>
#include <GeomFill_NSections.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

//=================================================================================================

static int OCC26270(Draw_Interpretor& theDI, int theNArg, const char** theArgVal)
{
  if (theNArg != 3)
  {
    theDI << "Usage :" << theArgVal[0] << " shape result\n";
    return 0;
  }
  TopoDS_Shape                                  aShape = DBRep::Get(theArgVal[1]);
  TopExp_Explorer                               anExp(aShape, TopAbs_EDGE);
  NCollection_Sequence<occ::handle<Geom_Curve>> aCurveSeq;
  for (; anExp.More(); anExp.Next())
  {
    double                  f, l;
    occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(TopoDS::Edge(anExp.Current()), f, l);
    if (!aCurve.IsNull())
    {
      aCurve = new Geom_TrimmedCurve(aCurve, f, l);
      aCurveSeq.Append(aCurve);
    }
  }
  if (aCurveSeq.Length() > 1)
  {
    try
    {
      OCC_CATCH_SIGNALS
      GeomFill_NSections               aBSurface(aCurveSeq);
      occ::handle<Geom_BSplineSurface> aRes = aBSurface.BSplineSurface();
      if (!aRes.IsNull())
      {
        BRepBuilderAPI_MakeFace b_face1(aRes, Precision::Confusion());
        const TopoDS_Face&      bsp_face1 = b_face1.Face();
        DBRep::Set(theArgVal[2], bsp_face1);
      }
    }
    catch (Standard_Failure const&)
    {
      theDI << "ERROR: Exception in GeomFill_NSections\n";
    }
  }
  return 0;
}

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepClass_FaceClassifier.hxx>

static int OCC27884(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb != 4)
  {
    return 0;
  }
  double                  aCheck = Draw::Atof(theArgVec[3]);
  occ::handle<Geom_Curve> aCur   = DrawTrSurf::GetCurve(theArgVec[1]);

  const occ::handle<Standard_Type>& aType = aCur->DynamicType();

  double aF = aCur->FirstParameter();
  double aL = aCur->LastParameter();

  double number_points = Draw::Atof(theArgVec[2]);
  double aSig          = (aL - aF) / (number_points - 1);

  NCollection_List<TopoDS_Shape> aLE;

  gp_Pnt aP, aPF, aPL;
  aPF = aCur->Value(aF);
  aP  = aPF;

  for (int i = 1; i < number_points; i++)
  {
    TopoDS_Edge anE;
    aL  = aF + (i * aSig);
    aPL = aCur->Value(aL);
    if (aCheck == 2)
    {
      if (i % 2 == 1)
      {
        anE = BRepBuilderAPI_MakeEdge(aPF, aPL);
      }
      else
      {
        if (aType == STANDARD_TYPE(Geom_BSplineCurve))
        {
          occ::handle<Geom_BSplineCurve> aCurCopy = occ::down_cast<Geom_BSplineCurve>(aCur->Copy());
          aCurCopy->Segment(aL - aSig, aL);
          anE = BRepBuilderAPI_MakeEdge(aCurCopy);
        }
        else
        {
          occ::handle<Geom_TrimmedCurve> aTCur = new Geom_TrimmedCurve(aCur, aL - aSig, aL);
          anE                                  = BRepBuilderAPI_MakeEdge(aTCur);
        }
      }
      aPF = aPL;
    }
    else
    {
      if (aCheck == 0)
      {
        anE = BRepBuilderAPI_MakeEdge(aPF, aPL);
        aPF = aPL;
      }
      if (aCheck == 1)
      {
        if (aType == STANDARD_TYPE(Geom_BSplineCurve))
        {
          occ::handle<Geom_BSplineCurve> aCurCopy = occ::down_cast<Geom_BSplineCurve>(aCur->Copy());
          aCurCopy->Segment(aL - aSig, aL);
          anE = BRepBuilderAPI_MakeEdge(aCurCopy);
        }
        else
        {
          occ::handle<Geom_TrimmedCurve> aTCur = new Geom_TrimmedCurve(aCur, aL - aSig, aL);
          anE                                  = BRepBuilderAPI_MakeEdge(aTCur);
        }
      }
    }
    aLE.Append(anE);
  }
  if (!aCur->IsClosed())
  {
    TopoDS_Edge anE = BRepBuilderAPI_MakeEdge(aPL, aP);
    aLE.Append(anE);
  }

  BRepBuilderAPI_MakeWire aWire;
  aWire.Add(aLE);
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aWire.Wire());

  //

  double anUMin, anUMax, aVMin, aVMax;
  BRepTools::UVBounds(aFace, anUMin, anUMax, aVMin, aVMax);
  gp_Pnt2d aP2d(anUMin - ((anUMax + anUMin) / 2), aVMin - ((aVMax + aVMin) / 2));

  const double aTol = BRep_Tool::Tolerance(aFace);

  BRepClass_FaceClassifier aClassifier;

  OSD_Timer timer;
  timer.Start();
  for (int i = 1; i <= 100; i++)
  {
    aClassifier.Perform(aFace, aP2d, aTol, true);
  }
  timer.Stop();
  double aTimer1 = timer.UserTimeCPU();
  timer.Reset();
  timer.Start();
  for (int i = 1; i <= 100; i++)
  {
    aClassifier.Perform(aFace, aP2d, aTol, false);
  }
  timer.Stop();
  double aTimer2 = timer.UserTimeCPU();
  theDI << "Improving time: " << (aTimer2 - aTimer1) / aTimer2 * 100 << " %\n";

  return 0;
}

#include <TDF_Tool.hxx>
#include <XCAFDoc_View.hxx>
#include <XCAFDoc_ViewTool.hxx>
#include <XCAFView_Object.hxx>
#include <XCAFView_ProjectionType.hxx>

static int OCC28389(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 20)
  {
    di << "Use: OCC28389 Doc label nb_shapes nb_GDT nb_planes name type pp_x pp_y pp_z vd_x vd_y "
          "vd_z ud_x ud_y ud_z zoom width height";
    return 1;
  }
  occ::handle<TDocStd_Document> aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << "Error: Wrong document";
    return 1;
  }
  occ::handle<XCAFDoc_ViewTool> aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "Error: Wrong label";
    return 1;
  }
  occ::handle<XCAFDoc_View> aView;
  if (!aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    di << "Error: Wrong label";
    return 1;
  }
  occ::handle<XCAFView_Object> anObj = aView->GetObject();
  if (anObj.IsNull())
  {
    di << "Error: Wrong label";
    return 1;
  }

  bool isOK = true;
  // check links
  int                             nbShapes = Draw::Atoi(argv[3]);
  int                             nbGDTs   = Draw::Atoi(argv[4]);
  int                             nbPlanes = Draw::Atoi(argv[5]);
  NCollection_Sequence<TDF_Label> aSequence;
  aViewTool->GetRefShapeLabel(aLabel, aSequence);
  if (aSequence.Length() != nbShapes)
    isOK = false;
  aSequence.Clear();
  aViewTool->GetRefGDTLabel(aLabel, aSequence);
  if (aSequence.Length() != nbGDTs)
    isOK = false;
  aSequence.Clear();
  aViewTool->GetRefClippingPlaneLabel(aLabel, aSequence);
  if (aSequence.Length() != nbPlanes)
    isOK = false;
  if (!isOK)
  {
    di << "Error: Wrong references";
    return 1;
  }

  if (anObj->Name()->IsDifferent(new TCollection_HAsciiString(argv[6])))
  {
    di << "Error: Wrong name";
    return 1;
  }

  XCAFView_ProjectionType aType = XCAFView_ProjectionType_NoCamera;
  if (argv[7][0] == 'p')
    aType = XCAFView_ProjectionType_Parallel;
  else if (argv[7][0] == 'c')
    aType = XCAFView_ProjectionType_Central;

  if (anObj->Type() != aType)
  {
    di << "Error: Wrong type";
    return 1;
  }

  gp_Pnt aPP(Draw::Atof(argv[8]), Draw::Atof(argv[9]), Draw::Atof(argv[10]));
  if (aPP.Distance(anObj->ProjectionPoint()) > Precision::Confusion())
  {
    di << "Error: Wrong projection point";
    return 1;
  }

  gp_Dir aVD(Draw::Atof(argv[11]), Draw::Atof(argv[12]), Draw::Atof(argv[13]));
  if (!aVD.IsEqual(anObj->ViewDirection(), Precision::Angular()))
  {
    di << "Error: Wrong view direction";
    return 1;
  }

  gp_Dir aUD(Draw::Atof(argv[14]), Draw::Atof(argv[15]), Draw::Atof(argv[16]));
  if (!aUD.IsEqual(anObj->UpDirection(), Precision::Angular()))
  {
    di << "Error: Wrong up direction";
    return 1;
  }

  if (fabs(anObj->ZoomFactor() - Draw::Atof(argv[17])) > Precision::Confusion())
  {
    di << "Error: Wrong zoom factor";
    return 1;
  }

  if (fabs(anObj->WindowHorizontalSize() - Draw::Atof(argv[18])) > Precision::Confusion())
    isOK = false;
  if (fabs(anObj->WindowVerticalSize() - Draw::Atof(argv[19])) > Precision::Confusion())
    isOK = false;
  if (!isOK)
  {
    di << "Error: Wrong Window size";
    return 1;
  }

  di << argv[2] << " OK";
  return 0;
}

static int OCC28784(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc < 3)
    return 1;

  TopoDS_Shape aShape = DBRep::Get(argv[2]);
  if (aShape.IsNull())
    return 1;

  gp_Ax2            aPlane(gp::Origin(), gp::DX(), -gp::DZ());
  HLRAlgo_Projector aProjector(aPlane);

  occ::handle<HLRBRep_PolyAlgo> aHLR = new HLRBRep_PolyAlgo(aShape);
  aHLR->Projector(aProjector);
  aHLR->Update();

  HLRBRep_PolyHLRToShape aHLRtoShape;
  aHLRtoShape.Update(aHLR);

  TopoDS_Shape aHidden = aHLRtoShape.HCompound();

  DBRep::Set(argv[1], aHidden);

  return 0;
}

static int OCC28829(Draw_Interpretor&, int, const char**)
{
  // do something that causes FPE exception
  std::cout << "sqrt(-1) = " << sqrt(-1.) << std::endl;
  return 0;
}

#include <STEPCAFControl_Reader.hxx>
#include <TDocStd_Application.hxx>

//=================================================================================================

static int OCC29531(Draw_Interpretor&, int, const char** theArgV)
{
  occ::handle<TDocStd_Application> anApp = DDocStd::GetApplication();
  occ::handle<TDocStd_Document>    aDoc;
  anApp->NewDocument("BinOcaf", aDoc);
  aDoc->SetUndoLimit(1);

  STEPCAFControl_Reader Reader;
  Reader.ReadFile(theArgV[1]);
  Reader.Transfer(aDoc);
  TDF_Label aShL, aDL;
  TDF_Tool::Label(aDoc->GetData(), "0:1:1:2:672", aShL);
  TDF_Tool::Label(aDoc->GetData(), "0:1:4:10", aDL);

  aDoc->OpenCommand();

  occ::handle<XCAFDoc_DimTolTool> aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(aDoc->Main());
  aDimTolTool->SetDimension(aShL, aDL);

  aDoc->CommitCommand();

  aDoc->Undo();
  aDoc->Redo();
  return 0;
}

//=================================================================================================

#include <GeomAdaptor_Surface.hxx>
#include <IntPatch_PointLine.hxx>
#include <IntSurf_PntOn2S.hxx>

static int OCC29807(Draw_Interpretor& theDI, int theNArg, const char** theArgV)
{
  if (theNArg != 7)
  {
    theDI << "Use: " << theArgV[0] << "surface1 surface2 u1 v1 u2 v2\n";
    return 1;
  }

  const occ::handle<Geom_Surface> aS1 = DrawTrSurf::GetSurface(theArgV[1]);
  const occ::handle<Geom_Surface> aS2 = DrawTrSurf::GetSurface(theArgV[2]);

  if (aS1.IsNull() || aS2.IsNull())
  {
    theDI << "Error. Null surface is not supported.\n";
    return 1;
  }

  const double aU1 = Draw::Atof(theArgV[3]);
  const double aV1 = Draw::Atof(theArgV[4]);
  const double aU2 = Draw::Atof(theArgV[5]);
  const double aV2 = Draw::Atof(theArgV[6]);

  const occ::handle<GeomAdaptor_Surface> anAS1 = new GeomAdaptor_Surface(aS1);
  const occ::handle<GeomAdaptor_Surface> anAS2 = new GeomAdaptor_Surface(aS2);

  const gp_Pnt aP1 = anAS1->Value(aU1, aV1);
  const gp_Pnt aP2 = anAS2->Value(aU2, aV2);

  if (aP1.SquareDistance(aP2) > Precision::SquareConfusion())
  {
    theDI << "Error. True intersection point must be specified. "
             "Please check parameters: u1 v1 u2 v2.\n";
    return 1;
  }

  IntSurf_PntOn2S aPOn2S;
  aPOn2S.SetValue(0.5 * (aP1.XYZ() + aP2.XYZ()), aU1, aV1, aU2, aV2);

  const double aCurvatureRadius =
    IntPatch_PointLine::CurvatureRadiusOfIntersLine(anAS1, anAS2, aPOn2S);
  theDI << "Radius of curvature is " << aCurvatureRadius << "\n";
  return 0;
}

//=======================================================================
// function : OCC29311
// purpose  : check performance of OBB calculations
//=======================================================================
static int OCC29311(Draw_Interpretor& theDI, int theArgc, const char** theArgv)
{
  if (theArgc < 4)
  {
    std::cerr << "Use: " << theArgv[0] << " shape counter_name nb_iterations" << std::endl;
    return 1;
  }

  TopoDS_Shape aShape  = DBRep::Get(theArgv[1]);
  int          aNbIter = Draw::Atoi(theArgv[3]);

  Bnd_OBB   anOBB;
  OSD_Timer aTimer;
  aTimer.Start();
  for (int aN = aNbIter; aN > 0; --aN)
  {
    anOBB.SetVoid();
    BRepBndLib::AddOBB(aShape, anOBB, false, false, false);
  }
  aTimer.Stop();

  theDI << "COUNTER " << theArgv[2] << ": " << aTimer.ElapsedTime() << "\n";

  return 0;
}

//=================================================================================================

#include <BRepOffset_Tool.hxx>

static int OCC30391(Draw_Interpretor& theDI, int theNArg, const char** theArgV)
{
  if (theNArg < 7)
  {
    theDI << "Use: " << theArgV[0]
          << "result face LenBeforeUfirst LenAfterUlast LenBeforeVfirst LenAfterVlast\n";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(theArgV[2], TopAbs_FACE);
  if (aShape.IsNull())
    return 1;

  const TopoDS_Face& aFace = TopoDS::Face(aShape);

  double aLenBeforeUfirst = atof(theArgV[3]);
  double aLenAfterUlast   = atof(theArgV[4]);
  double aLenBeforeVfirst = atof(theArgV[5]);
  double aLenAfterVlast   = atof(theArgV[6]);

  TopoDS_Face Result;
  BRepOffset_Tool::EnLargeFace(aFace,
                               Result,
                               true,
                               true,
                               true,
                               true,
                               true,
                               1,
                               aLenBeforeUfirst,
                               aLenAfterUlast,
                               aLenBeforeVfirst,
                               aLenAfterVlast);

  DBRep::Set(theArgV[1], Result);
  return 0;
}

//=================================================================================================

static int OCC29745(Draw_Interpretor& theDI, int theArgc, const char** theArgv)
{
  if (theArgc != 5)
  {
    theDI << "Usage : OCC29745 curve2d/3d continuity t1 t2";
    return 1;
  }

  occ::handle<Geom_Curve>   aC3d;
  occ::handle<Geom2d_Curve> aC2d;

  aC3d = DrawTrSurf::GetCurve(theArgv[1]);
  if (aC3d.IsNull())
  {
    aC2d = DrawTrSurf::GetCurve2d(theArgv[1]);
    if (aC2d.IsNull())
    {
      theDI << "Null curve" << "\n";
      return 1;
    }
  }

  int           i     = Draw::Atoi(theArgv[2]);
  GeomAbs_Shape aCont = GeomAbs_C0;
  if (i <= 0)
    aCont = GeomAbs_C0;
  else if (i == 1)
    aCont = GeomAbs_C1;
  else if (i == 2)
    aCont = GeomAbs_C2;
  else if (i == 3)
    aCont = GeomAbs_C3;
  else if (i >= 4)
    aCont = GeomAbs_CN;

  double t1 = Draw::Atof(theArgv[3]);
  double t2 = Draw::Atof(theArgv[4]);

  GeomAdaptor_Curve   aGAC3d;
  Geom2dAdaptor_Curve aGAC2d;
  int                 aNbInts;
  if (aC2d.IsNull())
  {
    aGAC3d.Load(aC3d, t1, t2);
    aNbInts = aGAC3d.NbIntervals(aCont);
  }
  else
  {
    aGAC2d.Load(aC2d, t1, t2);
    aNbInts = aGAC2d.NbIntervals(aCont);
  }

  NCollection_HArray1<double> anInters(1, aNbInts + 1);
  if (aC2d.IsNull())
  {
    aGAC3d.Intervals(anInters, aCont);
  }
  else
  {
    aGAC2d.Intervals(anInters, aCont);
  }

  theDI << "NbIntervals: " << aNbInts << "; ";
  for (i = anInters.Lower(); i <= anInters.Upper(); ++i)
  {
    theDI << anInters(i) << " ";
  }
  return 0;
}

#include <NCollection_Sequence.hxx>
#include <BinLDrivers.hxx>
#include <BinDrivers.hxx>
#include <XmlLDrivers.hxx>
#include <XmlDrivers.hxx>
#include <StdLDrivers.hxx>
#include <StdDrivers.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDocStd_PathParser.hxx>
#include <OSD.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_Thread.hxx>
#include <OSD_Environment.hxx>

typedef struct
{
  Standard_ThreadId       ID;
  int                     iThread;
  TCollection_AsciiString inFile[3];
  TCollection_AsciiString outFile[2];
  bool                    finished;
  int*                    res;
} Args;

static void printMsg(const char* msg)
{
  printf("%s\n", msg);
}

static int nbREP(50);

void* threadFunction(void* theArgs)
{
  Args* args = (Args*)theArgs;
  try
  {
    if (args->inFile[0].IsEmpty())
    {
      *(args->res) = -1;
      return args->res;
    }

    occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
    OCC_CATCH_SIGNALS;
    BinLDrivers::DefineFormat(anApp);
    BinDrivers::DefineFormat(anApp);
    XmlLDrivers::DefineFormat(anApp);
    XmlDrivers::DefineFormat(anApp);
    StdLDrivers::DefineFormat(anApp);
    StdDrivers::DefineFormat(anApp);

    for (int aFileIndex = 0; aFileIndex < 3; aFileIndex++)
    {
      TCollection_AsciiString       aDocName = args->inFile[aFileIndex];
      occ::handle<TDocStd_Document> aDoc;
      for (int i = 1; i <= nbREP; i++)
      {

        PCDM_ReaderStatus aStatus = anApp->Open(aDocName, aDoc);
        if (aStatus != PCDM_RS_OK)
        {
          args->finished = true;
          *(args->res)   = -1;
          return args->res;
        }
        else
        {
          TDF_Label         aLabel = aDoc->Main();
          TDF_ChildIterator anIt(aLabel, true);
          for (; anIt.More(); anIt.Next())
          {
            const TDF_Label&                  aLab = anIt.Value();
            occ::handle<TDataStd_AsciiString> anAtt;
            aLab.FindAttribute(TDataStd_AsciiString::GetID(), anAtt);
            if (!anAtt.IsNull())
            {
              TCollection_AsciiString aStr = anAtt->Get();
              if (aStr.IsEqual(aDocName))
              {
                *(args->res) = (int)aLab.Tag();
                break;
              }
            }
          }

          if (aFileIndex != 2)
          {
            TCollection_AsciiString anOutDocName = args->outFile[aFileIndex];
            anApp->SaveAs(aDoc, anOutDocName);
          }
          anApp->Close(aDoc);
        }
      }
    }
    args->finished = true;
  }
  catch (...)
  {
    args->finished = true;
    *(args->res)   = -1;
    return args->res;
  }
  args->finished = true;
  return args->res;
}

//=================================================================================================

static int OCC29195(Draw_Interpretor&, int theArgC, const char** theArgV)
{
  if (theArgC < 2)
  {
    std::cout << "\nOCC29195 [nbRep] doc1.cbf doc1.xml doc1.std outDoc1.cbf outDoc1.xml doc2.cbf "
                 "doc2.xml doc2.std outDoc2.cbf outDoc2.xml ...], where:";
    std::cout << "\nnbRep - number repetitions of a thread function (by default - 50)";
    std::cout
      << "\ndocN - names (5 in each group) of OCAF documents names (3 input files, 2 output)\n"
      << std::endl;
    return 1;
  }
  int iThread(0), off(0);
  if (TCollection_AsciiString(theArgV[1]).IsIntegerValue())
  {
    nbREP = TCollection_AsciiString(theArgV[1]).IntegerValue();
    off   = 1;
  }
  if (theArgC - off - 1 < 5 || (theArgC - off - 1) % 5 != 0)
  {
    printMsg("TEST is FAILED: number of arguments is invalid\n");
    return 0;
  }
  int aNbFiles  = (theArgC - off - 1) / 5;
  int nbThreads = OSD_Parallel::NbLogicalProcessors();
  if (aNbFiles < nbThreads)
  {
    nbThreads = aNbFiles;
  }
  // Allocate data
  Args*       args    = new Args[nbThreads];
  OSD_Thread* threads = new OSD_Thread[nbThreads];
  while (iThread < nbThreads)
  {
    if (iThread < aNbFiles)
    {
      args[iThread].inFile[0]  = theArgV[iThread * 5 + off + 1];
      args[iThread].inFile[1]  = theArgV[iThread * 5 + off + 2];
      args[iThread].inFile[2]  = theArgV[iThread * 5 + off + 3];
      args[iThread].outFile[0] = theArgV[iThread * 5 + off + 4];
      args[iThread].outFile[1] = theArgV[iThread * 5 + off + 5];
    }
    args[iThread].iThread  = iThread;
    args[iThread].ID       = threads[iThread].GetId();
    args[iThread].finished = false;
    args[iThread].res      = new int;
    threads[iThread].SetFunction(&threadFunction);
    iThread++;
  }
  for (iThread = 0; iThread < nbThreads; iThread++)
  {
    args[iThread].finished = false;
    threads[iThread].Run((void*)&(args[iThread]));
  }
  // Sleep while the threads are run.
  bool finished = false;
  while (!finished)
  {
    OSD::MilliSecSleep(100);
    finished = true;
    for (iThread = 0; iThread < nbThreads && finished; iThread++)
    {
      finished = args[iThread].finished;
    }
  }
  OSD_Environment anEnv("Result29195");
  for (iThread = 0; iThread < nbThreads; iThread++)
  {
    if (*(args[iThread].res) == -1)
    {
      printMsg("OCC29195 is FAILED\n");
      anEnv.SetValue("FAILED_ERR");
      break;
    }
  }
  if (iThread == nbThreads)
  {
    printMsg("OCC29195 is finished OK\n");
    anEnv.SetValue("OK");
  }
  anEnv.Build();
  return 0;
}

// Class is used in OCC30435
#include <Adaptor3d_Curve.hxx>

class CurveEvaluator : public AppCont_Function

{

public:
  occ::handle<Adaptor3d_Curve> myCurve;

  CurveEvaluator(const occ::handle<Adaptor3d_Curve>& C)
      : myCurve(C)
  {
    myNbPnt   = 1;
    myNbPnt2d = 0;
  }

  double FirstParameter() const override { return myCurve->FirstParameter(); }

  double LastParameter() const override { return myCurve->LastParameter(); }

  bool Value(const double theT,
             NCollection_Array1<gp_Pnt2d>& /*thePnt2d*/,
             NCollection_Array1<gp_Pnt>& thePnt) const override
  {
    thePnt(1) = myCurve->Value(theT);
    return true;
  }

  bool D1(const double theT,
          NCollection_Array1<gp_Vec2d>& /*theVec2d*/,
          NCollection_Array1<gp_Vec>& theVec) const override
  {
    gp_Pnt aDummyPnt;
    myCurve->D1(theT, aDummyPnt, theVec(1));
    return true;
  }
};

#include <GeomAdaptor_Curve.hxx>
#include <Approx_FitAndDivide.hxx>
#include <Convert_CompBezierCurvesToBSplineCurve.hxx>

static int OCC30435(Draw_Interpretor& di, int, const char** a)
{

  occ::handle<Geom_Curve> GC;
  GC = DrawTrSurf::GetCurve(a[2]);
  if (GC.IsNull())
    return 1;

  int    Dmin    = 3;
  int    Dmax    = 12;
  double Tol3d   = 1.e-7;
  bool   inverse = true;

  int inv = atoi(a[3]);
  inverse = inv > 0;

  int maxit = atoi(a[4]);

  occ::handle<GeomAdaptor_Curve> aGAC = new GeomAdaptor_Curve(GC);

  CurveEvaluator aCE(aGAC);

  Approx_FitAndDivide anAppro(Dmin, Dmax, Tol3d, 0., true);
  anAppro.SetInvOrder(inverse);
  int i;
  for (i = 1; i <= maxit; ++i)
    anAppro.Perform(aCE);

  if (!anAppro.IsAllApproximated())
  {
    di << "Approximation failed \n";
    return 1;
  }
  int NbCurves = anAppro.NbMultiCurves();

  Convert_CompBezierCurvesToBSplineCurve Conv;

  double tol3d, tol2d, tolreached = 0.;
  for (i = 1; i <= NbCurves; i++)
  {
    anAppro.Error(i, tol3d, tol2d);
    tolreached                    = std::max(tolreached, tol3d);
    AppParCurves_MultiCurve    MC = anAppro.Value(i);
    NCollection_Array1<gp_Pnt> Poles(1, MC.Degree() + 1);
    MC.Curve(1, Poles);
    Conv.AddCurve(Poles);
  }
  Conv.Perform();
  int NbPoles = Conv.NbPoles();
  int NbKnots = Conv.NbKnots();

  NCollection_Array1<gp_Pnt> NewPoles(1, NbPoles);
  NCollection_Array1<double> NewKnots(1, NbKnots);
  NCollection_Array1<int>    NewMults(1, NbKnots);

  Conv.KnotsAndMults(NewKnots, NewMults);
  Conv.Poles(NewPoles);

  BSplCLib::Reparametrize(GC->FirstParameter(), GC->LastParameter(), NewKnots);
  occ::handle<Geom_BSplineCurve> TheCurve =
    new Geom_BSplineCurve(NewPoles, NewKnots, NewMults, Conv.Degree());

  DrawTrSurf::Set(a[1], TheCurve);
  di << a[1] << ": tolreached = " << tolreached << "\n";

  return 0;
}

#include <BRepExtrema_ExtCF.hxx>

//=================================================================================================

static int OCC30880(Draw_Interpretor& theDI, int theArgc, const char** theArgv)
{
  if (theArgc != 3)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  TopoDS_Shape anEdge = DBRep::Get(theArgv[1]);
  if (anEdge.IsNull() || anEdge.ShapeType() != TopAbs_EDGE)
  {
    theDI << theArgv[1] << " is not an edge.\n";
    return 1;
  }

  TopoDS_Shape aFace = DBRep::Get(theArgv[2]);
  if (aFace.IsNull() || aFace.ShapeType() != TopAbs_FACE)
  {
    theDI << theArgv[2] << " is not a face.\n";
    return 1;
  }

  BRepExtrema_ExtCF anExtCF(TopoDS::Edge(anEdge), TopoDS::Face(aFace));
  if (!anExtCF.IsDone())
  {
    theDI << "Not done\n";
    return 0;
  }

  if (!anExtCF.NbExt())
  {
    theDI << "No solutions\n";
    return 0;
  }

  if (anExtCF.IsParallel())
  {
    theDI << "Infinite number of solutions, distance - " << std::sqrt(anExtCF.SquareDistance(1))
          << "\n";
    return 0;
  }

  double aDistMin = RealLast();
  int    aSolMin  = -1;
  // Look for the minimal solution
  for (int i = 1; i <= anExtCF.NbExt(); ++i)
  {
    double aDist = anExtCF.SquareDistance(i);
    if (aDist < aDistMin)
    {
      aDistMin = aDist;
      aSolMin  = i;
    }
  }

  if (aSolMin < 0)
  {
    theDI << "Failed\n";
    return 0;
  }

  theDI << "Minimal distance - " << std::sqrt(aDistMin) << "\n";
  return 0;
}

#include <BinXCAFDrivers.hxx>
#include <Message.hxx>

namespace
{
class QABugs_XdeLoader : public OSD_Thread
{
public:
  QABugs_XdeLoader(const occ::handle<TDocStd_Application>& theXdeApp,
                   const occ::handle<TDocStd_Document>&    theXdeDoc,
                   const TCollection_AsciiString&          theFilePath)
      : OSD_Thread(performThread),
        myXdeApp(theXdeApp),
        myXdeDoc(theXdeDoc),
        myFilePath(theFilePath)
  {
  }

private:
  void perform()
  {
    occ::handle<TDocStd_Document> aNewDoc;
    const PCDM_ReaderStatus       aReaderStatus = myXdeApp->Open(myFilePath, aNewDoc);
    if (aReaderStatus != PCDM_RS_OK)
    {
      Message::SendFail("Error occurred while reading the file");
      return;
    }
    myXdeDoc = aNewDoc;
    Message::SendInfo() << "Info: document has been opened";
  }

  static void* performThread(void* theData)
  {
    QABugs_XdeLoader* aLoader = (QABugs_XdeLoader*)theData;
    OSD::SetThreadLocalSignal(OSD_SignalMode_Set, false);
    try
    {
      OCC_CATCH_SIGNALS
      aLoader->perform();
    }
    catch (Standard_Failure const& theExcep)
    {
      Message::SendFail() << "Error: unexpected exception " << theExcep;
      return nullptr;
    }
    return nullptr;
  }

private:
  occ::handle<TDocStd_Application> myXdeApp;
  occ::handle<TDocStd_Document>    myXdeDoc;
  TCollection_AsciiString          myFilePath;
};
} // namespace

//=======================================================================
// function : OCC31785
// purpose  : Try reading XBF file in background thread
//=======================================================================
static int OCC31785(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 2)
  {
    theDI << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  TCollection_AsciiString aFileName(theArgVec[1]);

  occ::handle<TDocStd_Application> anXdeApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anXdeApp);

  occ::handle<TDocStd_Document> anXdeDoc;
  anXdeApp->NewDocument(TCollection_ExtendedString("BinXCAF"), anXdeDoc);
  QABugs_XdeLoader aLoader(anXdeApp, anXdeDoc, aFileName);
  aLoader.Run(&aLoader);
  aLoader.Wait();
  return 0;
}

static int QANullifyShape(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Wrong usage.\n";
    di << "Usage: QANullifyShape shape\n";
    return 1;
  }
  TopoDS_Shape aShape = DBRep::Get(a[1]);
  aShape.Nullify();
  DBRep::Set(a[1], aShape);
  return 0;
}

#include <BRepCheck_Analyzer.hxx>
#include <GCPnts_UniformDeflection.hxx>

static int OCC32744(Draw_Interpretor& theDi, int theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 2)
  {
    theDi << "Syntax error: wrong number of arguments!\n";
    return 1;
  }

  const TopoDS_Shape& aShape = DBRep::Get(theArgVec[1]);
  if (aShape.IsNull())
  {
    theDi << " Null Shape is not allowed here\n";
    return 1;
  }
  else if (aShape.ShapeType() != TopAbs_EDGE)
  {
    theDi << " Shape type must be EDGE\n";
    return 1;
  }

  const TopoDS_Edge& anEdge = TopoDS::Edge(aShape);
  BRepCheck_Analyzer analyzer(anEdge);
  if (analyzer.IsValid())
  {
    double                   firstParam = 0., lastParam = 0.;
    occ::handle<Geom_Curve>  pCurve = BRep_Tool::Curve(anEdge, firstParam, lastParam);
    GeomAdaptor_Curve        curveAdaptor(pCurve, firstParam, lastParam);
    GCPnts_UniformDeflection uniformAbs(curveAdaptor, 0.001, firstParam, lastParam);
  }

  return 0;
}

//=======================================================================
// function : QACheckBends
// purpose :
// Checks whether the Curve has a loop/bend
// Use: QACheckBends curve [CosMaxAngle [NbPoints]]
// NbPoints sets the interval of discretization;
// CosMaxAngle sets the maximal rotation angle between two adjacent segments.
// This value must be equal to the cosine of this angle.
//=======================================================================
static int QACheckBends(Draw_Interpretor& theDI, int theNArg, const char** theArgVal)
{
  // Checks whether theCurve has a loop / bend

  if (theNArg < 2)
  {
    theDI << "Use: " << theArgVal[0] << " QACheckBends curve [CosMaxAngle [theNbPoints]]" << "\n";
    return 1;
  }

  occ::handle<Geom_Curve> aCurve = DrawTrSurf::GetCurve(theArgVal[1]);

  if (aCurve.IsNull())
  {
    theDI << " " << theArgVal[1] << " : NULL curve" << "\n";
    return 0;
  }

  double aCosMaxAngle = .8;
  int    aNbPoints    = 1000;

  if (theNArg > 2)
  {
    aCosMaxAngle = Draw::Atof(theArgVal[2]);
  }

  if (theNArg > 3)
  {
    aNbPoints = Draw::Atoi(theArgVal[3]);
  }

  double U1 = aCurve->FirstParameter(), U2 = aCurve->LastParameter();
  if (Precision::IsInfinite(U1) || Precision::IsInfinite(U2))
  {
    theDI << "Infinite interval  : " << U1 << "  " << U2 << "\n";
    return 0;
  }

  double delta = (U2 - U1) / aNbPoints;
  gp_Pnt aP;
  gp_Vec aDC1, aDC2;
  aCurve->D1(U1, aP, aDC1);
  gp_Dir aD1(aDC1);
  double p;
  for (p = U1; p <= U2; p += delta)
  {
    aCurve->D1(p, aP, aDC2);
    gp_Dir aD2(aDC2);
    double aCos = aD1 * aD2;

    if (aCos < aCosMaxAngle)
    {
      theDI << "Error: The curve " << theArgVal[1] << " is possible to have a bend at parameter "
            << p << ". Please check carefully \n";
    }

    aD1 = aD2;
  }

  return 0;
}

static int OCC26441(Draw_Interpretor& theDi, int theNbArgs, const char** theArgVec)
{
  if (theNbArgs < 3)
  {
    theDi << "Syntax error: wrong number of arguments!\n";
    return 1;
  }

  const TopoDS_Shape& aShape = DBRep::Get(theArgVec[1]);
  if (aShape.IsNull())
  {
    theDi << " Null Shape is not allowed here\n";
    return 1;
  }
  const TopoDS_Shape& aRefShape = DBRep::Get(theArgVec[2]);
  if (aRefShape.IsNull())
  {
    theDi << " Null Shape is not allowed here\n";
    return 1;
  }
  if (aShape.ShapeType() != aRefShape.ShapeType())
  {
    theDi << " Shape types are not the same\n";
    return 1;
  }

  double anEps = Precision::Confusion();
  if (theNbArgs > 3)
  {
    anEps = Draw::Atof(theArgVec[3]);
  }

  bool isAllDiff = false;
  if (theNbArgs > 4)
  {
    int Inc = Draw::Atoi(theArgVec[4]);
    if (Inc > 0)
      isAllDiff = true;
  }

  BRep_Builder                                           aBB;
  TopExp_Explorer                                        anExp, anExpRef;
  double                                                 aMaxE = 0., aMaxV = 0.;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aChecked;
  TopoDS_Vertex                                          aV[2], aRefV[2];

  // Checking edge and vertex tolerances
  TopoDS_Compound aBadEdges;
  aBB.MakeCompound(aBadEdges);
  TopoDS_Compound aBadVerts;
  aBB.MakeCompound(aBadVerts);
  anExp.Init(aShape, TopAbs_EDGE);
  anExpRef.Init(aRefShape, TopAbs_EDGE);
  for (; anExpRef.More(); anExpRef.Next())
  {
    const TopoDS_Edge& aRefE = TopoDS::Edge(anExpRef.Current());
    if (!anExp.More())
    {
      theDi << " Different number of edges\n";
      return 1;
    }
    const TopoDS_Edge& anE = TopoDS::Edge(anExp.Current());
    if (!aChecked.Add(anE))
      continue;
    double aTolRef = BRep_Tool::Tolerance(aRefE);
    double aTol    = BRep_Tool::Tolerance(anE);
    double aDiff   = aTol - aTolRef;
    if (isAllDiff && aDiff < 0)
      aDiff = -aDiff;
    if (aDiff > anEps)
    {
      if (aDiff > aMaxE)
        aMaxE = aDiff;

      aBB.Add(aBadEdges, anE);
    }
    TopExp::Vertices(aRefE, aRefV[0], aRefV[1]);
    TopExp::Vertices(anE, aV[0], aV[1]);

    for (int i = 0; i < 2; ++i)
    {
      if (aRefV[i].IsNull())
        continue;
      if (!aChecked.Add(aV[i]))
        continue;
      aTolRef = BRep_Tool::Tolerance(aRefV[i]);
      aTol    = BRep_Tool::Tolerance(aV[i]);
      aDiff   = aTol - aTolRef;
      if (aDiff > anEps)
      {
        if (aDiff > aMaxV)
          aMaxV = aDiff;

        aBB.Add(aBadVerts, aV[i]);
      }
    }

    anExp.Next();
  }

  if (aMaxE > anEps)
  {
    theDi << " Maximal difference for edges : " << aMaxE << "\n";
    DBRep::Set("BadEdges", aBadEdges);
  }
  if (aMaxV > anEps)
  {
    theDi << " Maximal difference for vertices : " << aMaxV << "\n";
    DBRep::Set("BadVerts", aBadVerts);
  }

  return 0;
}

void QABugs::Commands_20(Draw_Interpretor& theCommands)
{
  const char* group = "QABugs";

  theCommands.Add("OCC27021", "OCC27021", __FILE__, OCC27021, group);
  theCommands.Add("OCC27235", "OCC27235", __FILE__, OCC27235, group);
  theCommands.Add("OCC26930", "OCC26930", __FILE__, OCC26930, group);
  theCommands.Add("OCC27466", "OCC27466", __FILE__, OCC27466, group);
  theCommands.Add("OCC26270", "OCC26270 shape result", __FILE__, OCC26270, group);
  theCommands.Add("OCC27884",
                  "OCC27884: Possible improvement for 2d classifier",
                  __FILE__,
                  OCC27884,
                  group);
  theCommands.Add("OCC28389", "OCC28389", __FILE__, OCC28389, group);
  theCommands.Add("OCC28784", "OCC28784 result shape", __FILE__, OCC28784, group);
  theCommands.Add("OCC28829", "OCC28829: perform invalid FPE operation", __FILE__, OCC28829, group);
  theCommands.Add("OCC29531", "OCC29531 <step file name>", __FILE__, OCC29531, group);

  theCommands.Add("OCC29807", "OCC29807 surface1 surface2 u1 v1 u2 v2", __FILE__, OCC29807, group);
  theCommands.Add("OCC29311",
                  "OCC29311 shape counter nbiter: check performance of OBB calculation",
                  __FILE__,
                  OCC29311,
                  group);
  theCommands.Add(
    "OCC30391",
    "OCC30391 result face LenBeforeUfirst LenAfterUlast LenBeforeVfirst LenAfterVlast",
    __FILE__,
    OCC30391,
    group);
  theCommands.Add("OCC29745",
                  "OCC29745 spreading of intervals of continuity on periodic curves",
                  __FILE__,
                  OCC29745,
                  group);
  theCommands.Add("OCC29195",
                  "OCC29195 [nbRep] doc1 [doc2 [doc3 [doc4]]]",
                  __FILE__,
                  OCC29195,
                  group);
  theCommands.Add("OCC30435", "OCC30435 result curve inverse nbit", __FILE__, OCC30435, group);

  theCommands.Add("OCC30880",
                  "Looks for extrema between edge and face.\n"
                  "Usage: OCC30880 edge face",
                  __FILE__,
                  OCC30880,
                  group);

  theCommands.Add("OCC31785",
                  "OCC31785 file.xbf : test reading XBF file in another thread",
                  __FILE__,
                  OCC31785,
                  group);

  theCommands.Add("QANullifyShape",
                  "Nullify shape. Usage: QANullifyShape shape",
                  __FILE__,
                  QANullifyShape,
                  group);

  theCommands.Add("OCC32744",
                  "Tests avoid Endless loop in GCPnts_UniformDeflection",
                  __FILE__,
                  OCC32744,
                  group);

  theCommands.Add("QACheckBends",
                  "QACheckBends curve [CosMaxAngle [theNbPoints]]",
                  __FILE__,
                  QACheckBends,
                  group);
  theCommands.Add("OCC26441",
                  "OCC26441 shape ref_shape [tol [all_diff 0/1]] \nif all_diff = 0, only "
                  "increasing tolerances is considered",
                  __FILE__,
                  OCC26441,
                  group);

  return;
}
