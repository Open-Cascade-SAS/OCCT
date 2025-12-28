// Created on: 2002-03-19
// Created by: QA Admin
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

#include <stdio.h>

#include <QABugs.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <Geom2d_Line.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2d_Circle.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Lin.hxx>
#include <BRepFeat_SplitShape.hxx>
#include <DBRep_DrawableShape.hxx>
#include <BRep_Builder.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <Draw.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Precision.hxx>
#include <Geom_BSplineCurve.hxx>
#include <OSD_Path.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <TopoDS_Wire.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <BRep_Tool.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <V3d_View.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Expression.hxx>

static int BUC60897(Draw_Interpretor& di,
                                 int /*argc*/,
                                 const char** /*argv*/)
{
  char abuf[16];

  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(gp_Pnt2d(100, 0), gp_Dir2d(gp_Dir2d::D::NX));
  Sprintf(abuf, "line");
  const char* st = abuf;
  DrawTrSurf::Set(st, aLine);

  NCollection_Array1<gp_Pnt2d> aPoints(1, 3);
  aPoints.SetValue(1, gp_Pnt2d(0, 0));
  aPoints.SetValue(2, gp_Pnt2d(50, 50));
  aPoints.SetValue(3, gp_Pnt2d(0, 100));
  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoints);
  Sprintf(abuf, "curve");
  DrawTrSurf::Set(st, aCurve);

  Geom2dAdaptor_Curve      aCLine(aLine);
  Geom2dAdaptor_Curve      aCCurve(aCurve);
  Geom2dGcc_QualifiedCurve aQualifCurve1(aCLine, GccEnt_outside);
  Geom2dGcc_QualifiedCurve aQualifCurve2(aCCurve, GccEnt_outside);
  Geom2dGcc_Circ2d2TanRad  aGccCirc2d(aQualifCurve1, aQualifCurve2, 10, 1e-7);
  if (!aGccCirc2d.IsDone())
  {
    di << "Faulty: can not create a circle.\n";
    return 1;
  }
  for (int i = 1; i <= aGccCirc2d.NbSolutions(); i++)
  {
    gp_Circ2d aCirc2d = aGccCirc2d.ThisSolution(i);
    di << "circle : X " << aCirc2d.Location().X() << " Y " << aCirc2d.Location().Y() << " R "
       << aCirc2d.Radius();
    double aTmpR1, aTmpR2;
    gp_Pnt2d      aPnt2d1, aPnt2d2;
    aGccCirc2d.Tangency1(i, aTmpR1, aTmpR2, aPnt2d1);
    aGccCirc2d.Tangency2(i, aTmpR1, aTmpR2, aPnt2d2);
    di << "\ntangency1 : X " << aPnt2d1.X() << " Y " << aPnt2d1.Y();
    di << "\ntangency2 : X " << aPnt2d2.X() << " Y " << aPnt2d2.Y() << "\n";

    Sprintf(abuf, "circle_%d", i);
    occ::handle<Geom2d_Curve> circ_res = new Geom2d_Circle(aCirc2d);
    DrawTrSurf::Set(st, circ_res);
  }

  di << "done\n";
  return 0;
}

static int BUC60889(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 10)
  {
    di << "Usage : " << argv[0]
       << " point_1 point_2 name_of_edge bndbox_X1 bndbox_Y1 bndbox_Z1 bndbox_X2 bndbox_Y2 "
          "bndbox_Z2\n";
    return 1;
  }
  else
  {
    gp_Pnt p1, p2;
    if (!(DrawTrSurf::GetPoint(argv[1], p1)) || !(DrawTrSurf::GetPoint(argv[2], p2)))
    {
      di << "Need two points to define a band\n";
      return 1;
    }
    TopoDS_Edge ed = TopoDS::Edge(DBRep::Get(argv[3]));
    if (ed.IsNull())
    {
      di << "Need an edge to define the band direction\n";
      return 1;
    }
    BRepAdaptor_Curve curve(ed);
    gp_Dir            d = curve.Line().Direction();
    Bnd_Box           bnd_box;
    bnd_box.Update(Draw::Atof(argv[4]),
                   Draw::Atof(argv[5]),
                   Draw::Atof(argv[6]),
                   Draw::Atof(argv[7]),
                   Draw::Atof(argv[8]),
                   Draw::Atof(argv[9]));
    if (bnd_box.IsOut(p1, p2, d))
      di << "The band lies out of the box\n";
    else
      di << "The band intersects the box\n";

    return 0;
  }
}

static int BUC60852(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 8)
    di << "Usage : " << argv[0]
       << " name_of_edge bndbox_X1 bndbox_Y1 bndbox_Z1 bndbox_X2 bndbox_Y2 bndbox_Z2\n";
  else
  {
    TopoDS_Edge shape = TopoDS::Edge(DBRep::Get(argv[1]));
    if (shape.ShapeType() != TopAbs_EDGE)
      di << "shape must be an edge\n";
    else
    {
      BRepAdaptor_Curve curve(shape);
      gp_Lin            lin = curve.Line();
      Bnd_Box           bnd_box;
      bnd_box.Update(Draw::Atof(argv[2]),
                     Draw::Atof(argv[3]),
                     Draw::Atof(argv[4]),
                     Draw::Atof(argv[5]),
                     Draw::Atof(argv[6]),
                     Draw::Atof(argv[7]));
      if (bnd_box.IsOut(lin))
        di << "Line that lies on edge does not intersect the box\n";
      else
        di << "Line that lies on edge intersects the box\n";
    }
  }
  return 0;
}

static int BUC60854(Draw_Interpretor& /*di*/, int argc, const char** argv)
{
  int newnarg;
  if (argc < 3)
    return 1;
  TopoDS_Shape        S = DBRep::Get(argv[2]);
  BRepFeat_SplitShape Spls(S);
  bool    pick = false;
  TopoDS_Shape        EF;
  double       u, v;
  int    i = 3;
  for (newnarg = 3; newnarg < argc; newnarg++)
  {
    if (argv[newnarg][0] == '@')
    {
      break;
    }
  }
  if (newnarg == 3 || (newnarg != argc && ((argc - newnarg) <= 2 || (argc - newnarg) % 2 != 1)))
  {
    return 1;
  }
  if (i < newnarg)
  {
    pick = (argv[i][0] == '.');
    EF   = DBRep::Get(argv[i], TopAbs_FACE);
    if (EF.IsNull())
      return 1;
  }
  while (i < newnarg)
  {
    if (pick)
    {
      DBRep_DrawableShape::LastPick(EF, u, v);
    }
    if (EF.ShapeType() == TopAbs_FACE)
    {
      i++;
      while (i < newnarg)
      {
        TopoDS_Shape     W;
        bool rever = false;
        if (argv[i][0] == '-')
        {
          if (argv[i][1] == '\0')
            return 1;
          pick             = (argv[i][1] == '.');
          const char* Temp = argv[i] + 1;
          W                = DBRep::Get(Temp, TopAbs_SHAPE, false);
          rever            = true;
        }
        else
        {
          pick = (argv[i][0] == '.');
          W    = DBRep::Get(argv[i], TopAbs_SHAPE, false);
        }
        if (W.IsNull())
        {
          return 1;
        }
        TopAbs_ShapeEnum wtyp = W.ShapeType();
        if (wtyp != TopAbs_WIRE && wtyp != TopAbs_EDGE && pick)
        {
          double aTempU, aTempV;
          DBRep_DrawableShape::LastPick(W, aTempU, aTempV);
          wtyp = W.ShapeType();
        }
        if (wtyp != TopAbs_WIRE && wtyp != TopAbs_EDGE)
        {
          EF = DBRep::Get(argv[i]);
          break;
        }
        else
        {
          if (rever)
          {
            W.Reverse();
          }
          if (wtyp == TopAbs_WIRE)
          {
            Spls.Add(TopoDS::Wire(W), TopoDS::Face(EF));
          }
          else
          {
            Spls.Add(TopoDS::Edge(W), TopoDS::Face(EF));
          }
        }
        i++;
      }
    }
    else
      return 1;
  }
  i++;
  while (argv[i][0] != '#')
  {
    TopoDS_Shape Ew, Es;
    TopoDS_Shape aLocalShape(DBRep::Get(argv[i], TopAbs_EDGE));
    Es = TopoDS::Edge(aLocalShape);
    if (Es.IsNull())
    {
      return 1;
    }
    aLocalShape = DBRep::Get(argv[i + 1], TopAbs_EDGE);
    Ew          = TopoDS::Edge(aLocalShape);
    if (Ew.IsNull())
    {
      return 1;
    }
    Spls.Add(TopoDS::Edge(Ew), TopoDS::Edge(Es));
    i += 2;
  }
  Spls.Build();
  const NCollection_List<TopoDS_Shape>& aLeftPart  = Spls.Left();
  const NCollection_List<TopoDS_Shape>& aRightPart = Spls.Right();
  BRep_Builder                BB;
  TopoDS_Shape                aShell;
  BB.MakeShell(TopoDS::Shell(aShell));
  NCollection_List<TopoDS_Shape>::Iterator anIter;
  if (argv[argc - 1][0] == 'L')
  {
    anIter.Initialize(aLeftPart);
  }
  else if (argv[argc - 1][0] == 'R')
  {
    anIter.Initialize(aRightPart);
  }
  else
  {
    return 1;
  }
  for (; anIter.More(); anIter.Next())
    BB.Add(aShell, anIter.Value());
  aShell.Closed(BRep_Tool::IsClosed(aShell));
  DBRep::Set(argv[1], aShell);
  return 0;
}

static int BUC60870(Draw_Interpretor& di, int argc, const char** argv)
{
  int i1;
  if (argc != 5)
  {
    di << "Usage : " << argv[0] << " result name_of_shape_1 name_of_shape_2 dev\n";
    return 1;
  }
  const char *               ns1 = (argv[2]), *ns2 = (argv[3]), *ns0 = (argv[1]);
  TopoDS_Shape               S1(DBRep::Get(ns1)), S2(DBRep::Get(ns2));
  double              dev = Draw::Atof(argv[4]);
  BRepExtrema_DistShapeShape dst(S1, S2, dev);
  if (dst.IsDone())
  {
    char named[100];
    Sprintf(named, "%s%s", ns0, "_val");
    char* tempd = named;
    Draw::Set(tempd, dst.Value());
    di << named << " ";
    for (i1 = 1; i1 <= dst.NbSolution(); i1++)
    {
      gp_Pnt P1, P2;
      P1 = (dst.PointOnShape1(i1));
      P2 = (dst.PointOnShape2(i1));
      if (dst.Value() <= 1.e-9)
      {
        TopoDS_Vertex V = BRepLib_MakeVertex(P1);
        char          namev[100];
        if (i1 == 1)
        {
          Sprintf(namev, "%s", ns0);
        }
        else
        {
          Sprintf(namev, "%s%d", ns0, i1);
        }
        char* tempv = namev;
        DBRep::Set(tempv, V);
        di << namev << " ";
      }
      else
      {
        char        name[100];
        TopoDS_Edge E = BRepLib_MakeEdge(P1, P2);
        if (i1 == 1)
        {
          Sprintf(name, "%s", ns0);
        }
        else
        {
          Sprintf(name, "%s%d", ns0, i1);
        }
        char* temp = name;
        DBRep::Set(temp, E);
        di << name << " ";
      }
    }
  }
  else
  {
    di << "Faulty : found a problem\n";
  }
  return 0;
}

static int BUC60944(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " path\n";
  }

  TCollection_AsciiString in(argv[1]);
  OSD_Path*               aPath = new OSD_Path(in);
  TCollection_AsciiString out;
  aPath->SystemName(out);
  if (in == out)
    di << "The conversion is right.\n";
  else
    di << "Faulty : The conversion is incorrect : " << out.ToCString() << "\n";
  di << out.ToCString() << "\n";
  //  std::cout << aPath->Trek() << " !" << std::endl;
  return 0;
}

bool BuildWiresWithReshape(const occ::handle<ShapeBuild_ReShape>& theReshape,
                                       const NCollection_List<TopoDS_Shape>&       theListOfEdges,
                                       NCollection_List<TopoDS_Shape>&             theListOfWires,
                                       const bool            isFixConnectedMode,
                                       const bool            isKeepLoopsMode,
                                       const double               theTolerance)
{
  NCollection_List<TopoDS_Shape>::Iterator anEdgeIter;
  bool                   isDone;
  TopoDS_Wire                        aWire;

  theListOfWires.Clear();
  occ::handle<ShapeExtend_WireData> aWireData  = new ShapeExtend_WireData;
  occ::handle<ShapeFix_Wire>        aShFixWire = new ShapeFix_Wire;
  aShFixWire->SetContext(theReshape);

  occ::handle<ShapeAnalysis_Wire> aWireAnalyzer;
  ShapeAnalysis_WireOrder    aWireOrder;

  aShFixWire->Load(aWireData);
  aShFixWire->SetPrecision(theTolerance);

  for (anEdgeIter.Initialize(theListOfEdges); anEdgeIter.More(); anEdgeIter.Next())
    aWireData->Add(TopoDS::Edge(anEdgeIter.Value()));

  aWireOrder.KeepLoopsMode() = isKeepLoopsMode;
  aWireAnalyzer              = aShFixWire->Analyzer();
  aWireAnalyzer->CheckOrder(aWireOrder, true);

  aShFixWire->FixReorder(aWireOrder);
  isDone = !aShFixWire->StatusReorder(ShapeExtend_FAIL);
  if (!isDone)
    return false;

  if (isFixConnectedMode)
  {
    aShFixWire->ModifyTopologyMode() = true;
    aShFixWire->FixConnected(theTolerance);
  }

  aWire = aWireData->Wire();

  //   if (aWire.Closed())
  //   {
  //     theListOfWires.Append(aWire);
  //     return true;
  //   }

  int i;
  BRep_Builder     aBuilder;
  TopoDS_Wire      aCurWire;
  TopoDS_Vertex    aVf;
  TopoDS_Vertex    aVl;
  TopoDS_Vertex    aVlast;
  int aNbEdges = aWireData->NbEdges();

  aBuilder.MakeWire(aCurWire);
  if (aNbEdges >= 1)
  {
    TopoDS_Edge anE = aWireData->Edge(1);
    TopExp::Vertices(anE, aVf, aVlast, true);
    aBuilder.Add(aCurWire, anE);
  }

  for (i = 2; i <= aNbEdges; i++)
  {
    TopoDS_Edge anE = aWireData->Edge(i);
    TopExp::Vertices(anE, aVf, aVl, true);
    if (aVf.IsSame(aVlast))
    {
      aBuilder.Add(aCurWire, anE);
      aVlast = aVl;
    }
    else
    {
      aVlast = aVl;
      TopExp::Vertices(aCurWire, aVf, aVl);
      if (aVf.IsSame(aVl))
        aCurWire.Closed(true);
      theListOfWires.Append(aCurWire);
      aBuilder.MakeWire(aCurWire);
      aBuilder.Add(aCurWire, anE);
    }
  }

  TopExp::Vertices(aCurWire, aVf, aVl);
  if (aVf.IsSame(aVl))
    aCurWire.Closed(true);
  theListOfWires.Append(aCurWire);

  return true;
}

bool BuildWires(const NCollection_List<TopoDS_Shape>& theListOfEdges,
                            NCollection_List<TopoDS_Shape>&       theListOfWires,
                            const bool      isFixConnectedMode = false,
                            const bool      isKeepLoopsMode    = true,
                            const double         theTolerance       = Precision::Confusion())
{
  occ::handle<ShapeBuild_ReShape> aReshape = new ShapeBuild_ReShape;
  return BuildWiresWithReshape(aReshape,
                               theListOfEdges,
                               theListOfWires,
                               isFixConnectedMode,
                               isKeepLoopsMode,
                               theTolerance);
}

bool BuildBoundWires(const TopoDS_Shape& theShell, NCollection_List<TopoDS_Shape>& theListOfWires)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> anEdgeFaceMap;
  int                          i;
  bool                          isBound;
  NCollection_List<TopoDS_Shape>                      aBoundaryEdges;

  TopExp::MapShapesAndAncestors(theShell, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMap);

  isBound = false;
  for (i = 1; i <= anEdgeFaceMap.Extent(); i++)
  {
    const NCollection_List<TopoDS_Shape>& anAncestFaces = anEdgeFaceMap.FindFromIndex(i);
    if (anAncestFaces.Extent() == 1)
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeFaceMap.FindKey(i));
      if (!BRep_Tool::Degenerated(anEdge))
      {
        aBoundaryEdges.Append(anEdge);
        isBound = true;
      }
    }
  }

  if (!isBound)
    return true;

  return BuildWires(aBoundaryEdges, theListOfWires);
}

static int BUC60868(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " Result Shell\n";
    return 1;
  }

  TopoDS_Shape aShell = DBRep::Get(argv[2]);

  if (aShell.IsNull())
  {
    di << "Faulty : The shape is NULL\n";
    return 1;
  }

  NCollection_List<TopoDS_Shape> aListOfWires;
  BuildBoundWires(aShell, aListOfWires);

  TopoDS_Shape aRes;
  if (aListOfWires.IsEmpty())
    di << "no bound\n";
  else if (aListOfWires.Extent() == 1)
    aRes = aListOfWires.First();
  else
  {
    BRep_Builder aBld;
    aBld.MakeCompound(TopoDS::Compound(aRes));
    NCollection_List<TopoDS_Shape>::Iterator aWireIter(aListOfWires);
    for (; aWireIter.More(); aWireIter.Next())
      aBld.Add(aRes, aWireIter.Value());
  }

  DBRep::Set(argv[1], aRes);
  return 0;
}

static int BUC60924(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 5)
  {
    di << "Usage : " << argv[0] << " curve X Y Z\n";
    return 1;
  }

  occ::handle<Geom_Curve> aCurve = DrawTrSurf::GetCurve(argv[1]);

  if (aCurve.IsNull())
  {
    di << "Faulty : the curve is NULL.\n";
    return 1;
  }

  gp_XYZ aVec(Draw::Atof(argv[2]), Draw::Atof(argv[3]), Draw::Atof(argv[4]));

  bool isPlanar = false;
  isPlanar                  = ShapeAnalysis_Curve::IsPlanar(aCurve, aVec, 1e-7);

  if (isPlanar)
    di << "The curve is planar !\n";
  else
    di << "Faulty : the curve is not planar!\n";

  return 0;
}

static int BUC60920(Draw_Interpretor& di, int /*argc*/, const char** argv)
{

  occ::handle<AIS_InteractiveContext> myAISContext = ViewerTest::GetAISContext();
  if (myAISContext.IsNull())
  {
    std::cerr << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  di.Eval("box b 10 10 10");
  di.Eval("box w 20 20 20");
  di.Eval("explode w e");

  di.Eval(" vdisplay b");
  di.Eval("vsetdispmode 1");

  const char*  Shname   = "w_11";
  TopoDS_Shape theShape = DBRep::Get(Shname);

  occ::handle<AIS_Shape> anAISShape = new AIS_Shape(theShape);
  myAISContext->Display(anAISShape, true);

  occ::handle<V3d_View> myV3dView = ViewerTest::CurrentView();

  double Xv, Yv;
  myV3dView->Project(20, 20, 0, Xv, Yv);
  //  std::cout<<Xv<<"\t"<<Yv<<std::endl;

  int Xp, Yp;
  myV3dView->Convert(Xv, Yv, Xp, Yp);
  //  std::cout<<Xp<<"\t"<<Yp<<std::endl;

  myAISContext->MoveTo(Xp, Yp, myV3dView, true);

  //   if (myAISContext->IsHilighted(anAISShape))
  //              std::cout << "has hilighted shape : OK"   << std::endl;
  //   else       std::cout << "has hilighted shape : bugged - Faulty "   << std::endl;

  return 0;
}

#include <LDOMParser.hxx>

static int OCC983(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " file\n";
    return 1;
  }

  LDOMParser    aParser;
  LDOM_Document myDOM;

  const char* File = (argv[1]);

  if (!aParser.parse(File))
  {
    myDOM = aParser.getDocument();
    di << "Document parsed\n";
  }
  else
  {
    di << "Document not parsed\n";
    return 0;
  }

  LDOM_Element root = myDOM.getDocumentElement();

  if (root.isNull())
  {
    di << "Root element is null\n";
    return 0;
  }

  TCollection_AsciiString RootName = root.getTagName();
  di << "   RootName = " << RootName.ToCString() << "\n";
  LDOM_NodeList aChildList = root.GetAttributesList();
  for (int i = 0, n = aChildList.getLength(); i < n; i++)
  {
    LDOM_Node               item      = aChildList.item(i);
    TCollection_AsciiString itemName  = item.getNodeName();
    TCollection_AsciiString itemValue = item.getNodeValue();
    di << "       AttributeName = " << itemName.ToCString() << "\n";
    di << "       AttributeValue = " << itemValue.ToCString() << "\n";
  }

  LDOM_Element element;
  LDOM_Node    node;
  for (node = root.getFirstChild(), element = (const LDOM_Element&)node; !element.isNull();
       node = element.getNextSibling(), element = (const LDOM_Element&)node)
  {
    TCollection_AsciiString ElementName = element.getTagName();
    di << "   ElementName = " << ElementName.ToCString() << "\n";
    LDOM_NodeList aChildList2 = element.GetAttributesList();
    for (int i2 = 0, n2 = aChildList2.getLength(); i2 < n2; i2++)
    {
      LDOM_Node               item2      = aChildList2.item(i2);
      TCollection_AsciiString itemName2  = item2.getNodeName();
      TCollection_AsciiString itemValue2 = item2.getNodeValue();
      di << "       AttributeName = " << itemName2.ToCString() << "\n";
      di << "       AttributeValue = " << itemValue2.ToCString() << "\n";
    }
  }
  if (aParser.GetBOM() != LDOM_OSStream::BOM_UNDEFINED)
  {
    di << "BOM is ";
    switch (aParser.GetBOM())
    {
      case LDOM_OSStream::BOM_UTF8:
        di << "UTF-8";
        break;
      case LDOM_OSStream::BOM_UTF16BE:
        di << "UTF-16 (BE)";
        break;
      case LDOM_OSStream::BOM_UTF16LE:
        di << "UTF-16 (LE)";
        break;
      case LDOM_OSStream::BOM_UTF32BE:
        di << "UTF-32 (BE)";
        break;
      case LDOM_OSStream::BOM_UTF32LE:
        di << "UTF-32 (LE)";
        break;
      case LDOM_OSStream::BOM_UTF7:
        di << "UTF-7";
        break;
      case LDOM_OSStream::BOM_UTF1:
        di << "UTF-1";
        break;
      case LDOM_OSStream::BOM_UTFEBCDIC:
        di << "UTF-EBCDIC";
        break;
      case LDOM_OSStream::BOM_SCSU:
        di << "SCSU";
        break;
      case LDOM_OSStream::BOM_BOCU1:
        di << "BOCU-1";
        break;
      case LDOM_OSStream::BOM_GB18030:
        di << "GB-18030";
        break;
      default:
        di << "unexpected";
    }
    di << "\n";
  }

  return 0;
}

static int OCC984(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " file\n";
    return 1;
  }

  LDOMParser    aParser;
  LDOM_Document myDOM;

  // char  *File = new char [100];
  // Sprintf(File,"%s",argv[1]);
  const char* File = (argv[1]);

  if (!aParser.parse(File))
  {
    myDOM = aParser.getDocument();
    di << "Document parsed\n";
  }
  else
  {
    di << "Document not parsed\n";
  }

  return 0;
}

// #include <math.h>
//  See QAOCC.cxx OCC6143
// static int OCC1723 (Draw_Interpretor& /*di*/, int argc, const char **
// argv)
//{
//   if( argc != 1)
//   {
//     std::cout << "Usage : " << argv[0] << std::endl;
//     return 1;
//   }
//
//   bool isBad = false, isCaught;
//
//   // Case 1
//   isCaught = false;
//   {
//     try {
//       OCC_CATCH_SIGNALS
//       int a = 1;
//       int b = 0;
//       int c = a / b;
//     }
//     catch ( Standard_Failure ) {
//       isCaught = true;
//       std::cout << "OCC1723 Case 1 : OK" << std::endl;
//     }
//   }
//   isBad = isBad || !isCaught;
//
//   // Case 2
//   isCaught = false;
//   {
//     try {
//       OCC_CATCH_SIGNALS
//       double d = -1.0;
//       double e = sqrt(d);
//     }
//     catch ( Standard_Failure ) {
//       isCaught = true;
//       std::cout << "OCC1723 Case 2 : OK" << std::endl;
//     }
//   }
//   isBad = isBad || !isCaught;
//
//   // Case 3
//   isCaught = false;
//   {
//     try {
//       OCC_CATCH_SIGNALS
//       double f = 1.0e-200;
//       double g = 1.0e-200;
//       double h = f * g;
//     }
//     catch ( Standard_Failure ) {
//       isCaught = true;
//       std::cout << "OCC1723 Case 3 : OK" << std::endl;
//     }
//   }
//   // MSV: underflow is not caught
//   //isBad = isBad || !isCaught;
//
//   // Case 4
//   isCaught = false;
//   {
//     try {
//       OCC_CATCH_SIGNALS
//       double i = 1.0e+200;
//       double j = 1.0e+200;
//       double k = i * j;
//     }
//     catch ( Standard_Failure ) {
//       isCaught = true;
//       std::cout << "OCC1723 Case 4 : OK" << std::endl;
//     }
//   }
//   isBad = isBad || !isCaught;
//
//   if (isBad) {
//     std::cout << "OCC1723 : Error" << std::endl;
//   } else {
//     std::cout << "OCC1723 : OK" << std::endl;
//   }
//
//   return 0;
// }

#include <locale.h>

static int OCC1919_get(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  const TCollection_AsciiString anOldNumLocale = (const char*)setlocale(LC_NUMERIC, NULL);
  di << "LC_NUMERIC = " << anOldNumLocale.ToCString() << "\n";
  return 0;
}

static int OCC1919_set(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " LC_NUMERIC\n";
    return 1;
  }
  TCollection_AsciiString aNumLocale(argv[1]);
  setlocale(LC_ALL, "");
  setlocale(LC_NUMERIC, aNumLocale.ToCString());
  return 0;
}

#include <DDF.hxx>
#include <TDataStd_Real.hxx>

static int OCC1919_real(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc == 4)
  {
    occ::handle<TDF_Data> DF;
    if (!DDF::GetDF(argv[1], DF))
      return 1;
    TDF_Label L;
    DDF::AddLabel(DF, argv[2], L);

    // TDataStd_Real::Set(L,Draw::Atof(arg[3]));
    TCollection_AsciiString AsciiStringReal(argv[3]);
    if (!AsciiStringReal.IsRealValue())
      return 1;
    double aReal = AsciiStringReal.RealValue();
    di << "aReal = " << aReal << "\n";

    TDataStd_Real::Set(L, aReal);
    return 0;
  }
  return 1;
}

#include <TDataStd_UAttribute.hxx>

static int OCC2932_SetIDUAttribute(Draw_Interpretor& di,
                                                int  argc,
                                                const char**      argv)
{
  if (argc != 5)
  {
    di << "Usage : " << argv[0] << " (DF, entry, oldLocalID, newLocalID)\n";
    return 1;
  }
  occ::handle<TDF_Data> DF;
  if (!DDF::GetDF(argv[1], DF))
    return 1;
  TDF_Label label;
  if (!DDF::FindLabel(DF, argv[2], label))
  {
    di << "No label for entry " << argv[2] << "\n";
    return 0;
  }
  Standard_GUID old_guid(argv[3]); //"00000000-0000-0000-2222-000000000000");
  Standard_GUID new_guid(argv[4]); //"00000000-0000-0000-2222-000000000001");

  occ::handle<TDataStd_UAttribute> UA;
  if (!label.FindAttribute(old_guid, UA))
  {
    di << "No UAttribute Attribute on label " << argv[2] << "\n";
    return 0;
  }
  occ::handle<TDataStd_UAttribute> anotherUA;
  if (label.FindAttribute(new_guid, anotherUA))
  {
    di << "There is this UAttribute Attribute on label " << argv[2] << "\n";
    return 0;
  }
  UA->SetID(new_guid);
  return 0;
}

static int OCC2932_SetTag(Draw_Interpretor& di,
                                       int  argc,
                                       const char**      argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " (DF, entry, Tag)\n";
    return 1;
  }
  occ::handle<TDF_Data> DF;
  if (!DDF::GetDF(argv[1], DF))
    return 1;
  TDF_Label L;
  DDF::AddLabel(DF, argv[2], L);
  int      Tag = Draw::Atoi(argv[3]);
  occ::handle<TDF_TagSource> A   = TDF_TagSource::Set(L);
  A->Set(Tag);
  return 0;
}

#include <TDataStd_Current.hxx>

static int OCC2932_SetCurrent(Draw_Interpretor& di,
                                           int  argc,
                                           const char**      argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " (DF, entry)\n";
    return 1;
  }
  occ::handle<TDF_Data> DF;
  if (!DDF::GetDF(argv[1], DF))
    return 1;
  TDF_Label L;
  DDF::AddLabel(DF, argv[2], L);
  TDataStd_Current::Set(L);
  return 0;
}

static int OCC2932_SetExpression(Draw_Interpretor& di,
                                              int  argc,
                                              const char**      argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " (DF, entry, Expression)\n";
    return 1;
  }
  occ::handle<TDF_Data> DF;
  if (!DDF::GetDF(argv[1], DF))
    return 1;
  TDF_Label L;
  DDF::AddLabel(DF, argv[2], L);
  TCollection_ExtendedString  Expression(argv[3]);
  occ::handle<TDataStd_Expression> A = TDataStd_Expression::Set(L);
  A->SetExpression(Expression);
  return 0;
}

#include <TDataStd_Relation.hxx>

static int OCC2932_SetRelation(Draw_Interpretor& di,
                                            int  argc,
                                            const char**      argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " (DF, entry, Relation)\n";
    return 1;
  }
  occ::handle<TDF_Data> DF;
  if (!DDF::GetDF(argv[1], DF))
    return 1;
  TDF_Label L;
  DDF::AddLabel(DF, argv[2], L);
  TCollection_ExtendedString Relation(argv[3]);
  occ::handle<TDataStd_Relation>  A = TDataStd_Relation::Set(L);
  A->SetRelation(Relation);
  return 0;
}

void QABugs::Commands_14(Draw_Interpretor& theCommands)
{
  const char* group = "QABugs";

  theCommands.Add("BUC60897", "BUC60897", __FILE__, BUC60897, group);
  theCommands.Add("BUC60889",
                  "BUC60889 point_1 point_2 name_of_edge bndbox_X1 bndbox_Y1 bndbox_Z1 bndbox_X2 "
                  "bndbox_Y2 bndbox_Z2",
                  __FILE__,
                  BUC60889,
                  group);
  theCommands.Add(
    "BUC60852",
    "BUC60852 name_of_edge bndbox_X1 bndbox_Y1 bndbox_Z1 bndbox_X2 bndbox_Y2 bndbox_Z2",
    __FILE__,
    BUC60852,
    group);
  theCommands.Add("BUC60854",
                  "BUC60854 result_shape name_of_shape name_of_face name_of_wire/name_of_edge [ "
                  "name_of_wire/name_of_edge ... ] [ name_of_face name_of_wire/name_of_edge [ "
                  "name_of_wire/name_of_edge ... ] ... ] [ @ edge_on_shape edge_on_wire [ "
                  "edge_on_shape edge_on_wire ... ] ] [ # L/R ]",
                  __FILE__,
                  BUC60854,
                  group);
  theCommands.Add("BUC60870",
                  "BUC60870 result name_of_shape_1 name_of_shape_2 dev",
                  __FILE__,
                  BUC60870,
                  group);
  theCommands.Add("BUC60944", "BUC60944 path", __FILE__, BUC60944, group);
  theCommands.Add("BUC60868", "BUC60868 Result Shell", __FILE__, BUC60868, group);
  theCommands.Add("BUC60924", "BUC60924 curve X Y Z", __FILE__, BUC60924, group);
  theCommands.Add("BUC60920", "BUC60920", __FILE__, BUC60920, group);
  theCommands.Add("OCC983", "OCC983 file", __FILE__, OCC983, group);
  theCommands.Add("OCC984", "OCC984 file", __FILE__, OCC984, group);

  //  theCommands.Add ("OCC1723", "OCC1723", __FILE__, OCC1723, group);

  theCommands.Add("OCC1919_get", "OCC1919_get", __FILE__, OCC1919_get, group);

  theCommands.Add("OCC1919_set", "OCC1919_set LC_NUMERIC", __FILE__, OCC1919_set, group);

  theCommands.Add("OCC1919_real", "OCC1919_real (DF, entry, value)", __FILE__, OCC1919_real, group);

  theCommands.Add("OCC2932_SetIDUAttribute",
                  "OCC2932_SetIDUAttribute (DF, entry, oldLocalID, newLocalID)",
                  __FILE__,
                  OCC2932_SetIDUAttribute,
                  group);

  theCommands.Add("OCC2932_SetTag",
                  "OCC2932_SetTag (DF, entry, Tag)",
                  __FILE__,
                  OCC2932_SetTag,
                  group);

  theCommands.Add("OCC2932_SetCurrent",
                  "OCC2932_SetCurrent (DF, entry)",
                  __FILE__,
                  OCC2932_SetCurrent,
                  group);

  theCommands.Add("OCC2932_SetExpression",
                  "OCC2932_SetExpression (DF, entry, Expression)",
                  __FILE__,
                  OCC2932_SetExpression,
                  group);

  theCommands.Add("OCC2932_SetRelation",
                  "OCC2932_SetRelation (DF, entry, Relation)",
                  __FILE__,
                  OCC2932_SetRelation,
                  group);

  return;
}
