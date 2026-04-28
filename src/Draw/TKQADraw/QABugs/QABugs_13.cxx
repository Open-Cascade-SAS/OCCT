// Created on: 2002-05-16
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

#include <QABugs.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_Plane.hxx>
#include <Law_Linear.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <GProp_GProps.hxx>
#include <Standard_ErrorHandler.hxx>

#ifndef M_SQRT2
  #define M_SQRT2 1.41421356237309504880168872420969808
#endif

#include <gce_MakePln.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <BRepAlgoAPI_Fuse.hxx>

//=================================================================================================

static int OCC544(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc > 6)
  {
    di << "Usage : " << argv[0] << " [[[[[wT [[[[d1 [[[d2 [[R [length]]]]]\n";
    return 1;
  }

  // Used to Display Geometry or Topolgy
  char name[255];
  bool check = true;

  // Set default arguments
  double radius_l = 20.0;
  double radius_r = 80.0;
  // mkv 15.07.03 double bend_angle = M_PI/2.0;
  double bend_angle = M_PI / 2.0;

  double major_rad      = 280.0;
  double wall_thickness = 10.0;

  // Convert arguments
  if (argc > 1)
    radius_l = Draw::Atof(argv[1]);
  if (argc > 2)
    radius_r = Draw::Atof(argv[2]);
  if (argc > 3)
    bend_angle = Draw::Atof(argv[3]);
  if (argc > 4)
    major_rad = Draw::Atof(argv[4]);
  if (argc > 5)
    wall_thickness = Draw::Atof(argv[5]);

  // mkv 15.07.03 if ((bend_angle >= 2.0*M_PI)) {
  if ((bend_angle >= 2.0 * M_PI))
  {
    di << "The arguments are invalid.\n";
    return 1;
  }
  di << "creating the shape for a bent tube\n";

  gp_Ax2 origin(gp_Pnt(500.0, -300.0, 100.0), gp_Dir(0.0, -1.0 / M_SQRT2, -1.0 / M_SQRT2));

  TopoDS_Face  firstFace, lastFace;
  TopoDS_Solid wallSolid, myShape;
  // Construct a circle for the first face, on the xy-plane at the origin
  gp_Pln  circ1Plane(origin.Location(), origin.Direction());
  gp_Circ faceCircle(origin, radius_l);
  gp_Circ outFaceCircle(origin, radius_l + wall_thickness);

  // Construct center for a circle to be the spine of
  // revolution, on the xz-plane at x=major_rad
  gp_Pnt circ_center = origin.Location().Translated(major_rad * origin.XDirection());

  // This point will be the center of the second face.
  gp_Pnt endPoint = origin.Location();
  endPoint.Translate(major_rad * (1.0 - cos(bend_angle)) * origin.XDirection());
  endPoint.Translate((-major_rad * sin(bend_angle)) * origin.Direction());

  // Construct the plane for the second face to sit on.
  gp_Pln circ2Plane =
    gce_MakePln(circ_center, endPoint, endPoint.Translated(major_rad * origin.YDirection()))
      .Value();

  // The circle used for the spine.
  gp_Ax2  spineAxis(circ_center, origin.YDirection(), origin.XDirection());
  gp_Circ circle(spineAxis, major_rad);

  gp_Ax2  circ2axis(endPoint, circ2Plane.Axis().Direction(), origin.YDirection());
  gp_Circ faceCircle2(circ2axis, radius_r);
  gp_Circ outFaceCircle2(circ2axis, radius_r + wall_thickness);

  TopoDS_Edge E1_1   = BRepBuilderAPI_MakeEdge(faceCircle, 0, M_PI);
  TopoDS_Edge E1_2   = BRepBuilderAPI_MakeEdge(faceCircle, M_PI, 2. * M_PI);
  TopoDS_Wire Wire1_ = BRepBuilderAPI_MakeWire(E1_1, E1_2);

  // Create the face at the near end for the wall solid, an annular ring.
  TopoDS_Edge Eout1_1     = BRepBuilderAPI_MakeEdge(outFaceCircle, 0, M_PI);
  TopoDS_Edge Eout1_2     = BRepBuilderAPI_MakeEdge(outFaceCircle, M_PI, 2. * M_PI);
  TopoDS_Wire outerWire1_ = BRepBuilderAPI_MakeWire(Eout1_1, Eout1_2);

  TopoDS_Edge E2_1   = BRepBuilderAPI_MakeEdge(faceCircle2, 0, M_PI);
  TopoDS_Edge E2_2   = BRepBuilderAPI_MakeEdge(faceCircle2, M_PI, 2. * M_PI);
  TopoDS_Wire Wire2_ = BRepBuilderAPI_MakeWire(E2_1, E2_2);

  // Create the face at the far end for the wall solid, an annular ring.
  TopoDS_Edge Eout2_1     = BRepBuilderAPI_MakeEdge(outFaceCircle2, 0, M_PI);
  TopoDS_Edge Eout2_2     = BRepBuilderAPI_MakeEdge(outFaceCircle2, M_PI, 2. * M_PI);
  TopoDS_Wire outerWire2_ = BRepBuilderAPI_MakeWire(Eout2_1, Eout2_2);

  BRepBuilderAPI_MakeFace mkFace;

  occ::handle<Geom_Curve> SpineCurve(
    GC_MakeArcOfCircle(circle, endPoint, origin.Location(), true).Value());
  occ::handle<Law_Linear> myLaw  = new Law_Linear();
  occ::handle<Law_Linear> myLaw2 = new Law_Linear();

  myLaw->Set(SpineCurve->FirstParameter(), radius_r / radius_l, SpineCurve->LastParameter(), 1.0);

  myLaw2->Set(SpineCurve->FirstParameter(),
              (radius_r + wall_thickness) / (radius_l + wall_thickness),
              SpineCurve->LastParameter(),
              1.0);

  di << "SpineCurve->FirstParameter() is " << SpineCurve->FirstParameter() << "\n";
  di << "SpineCurve->LastParameter() is " << SpineCurve->LastParameter() << "\n";
  di << "Law1 Value at FirstParameter() is " << myLaw->Value(SpineCurve->FirstParameter()) << "\n";
  di << "Law1 Value at LastParameter() is " << myLaw->Value(SpineCurve->LastParameter()) << "\n";
  di << "radius_r / radius_l is " << radius_r / radius_l << "\n";

  BRepBuilderAPI_MakeEdge mkEdge;

  mkEdge.Init(SpineCurve);
  if (!mkEdge.IsDone())
    return 1;
  TopoDS_Wire SpineWire = BRepBuilderAPI_MakeWire(mkEdge.Edge()).Wire();

  Sprintf(name, "SpineWire");
  DBRep::Set(name, SpineWire);

  Sprintf(name, "Wire1_");
  DBRep::Set(name, Wire1_);

  Sprintf(name, "outerWire1_");
  DBRep::Set(name, outerWire1_);

  Sprintf(name, "Wire2_");
  DBRep::Set(name, Wire2_);

  Sprintf(name, "outerWire2_");
  DBRep::Set(name, outerWire2_);

  di.Eval("fit");

  TopoDS_Vertex Location1, Location2;

  TopExp::Vertices(SpineWire, Location2, Location1);

  Sprintf(name, "Location1");
  DBRep::Set(name, Location1);

  Sprintf(name, "Location2");
  DBRep::Set(name, Location2);

  // Make inner pipe shell
  BRepOffsetAPI_MakePipeShell mkPipe1(SpineWire);
  mkPipe1.SetTolerance(1.0e-8, 1.0e-8, 1.0e-6);
  mkPipe1.SetTransitionMode(BRepBuilderAPI_Transformed);
  mkPipe1.SetMode(false);
  mkPipe1.SetLaw(Wire1_, myLaw, Location1, false, false);
  mkPipe1.Build();
  if (!mkPipe1.IsDone())
    return 1;

  // Make outer pipe shell
  BRepOffsetAPI_MakePipeShell mkPipe2(SpineWire);
  mkPipe2.SetTolerance(1.0e-8, 1.0e-8, 1.0e-6);
  mkPipe2.SetTransitionMode(BRepBuilderAPI_Transformed);
  mkPipe2.SetMode(false);
  mkPipe2.SetLaw(outerWire1_, myLaw2, Location1, false, false);
  mkPipe2.Build();
  if (!mkPipe2.IsDone())
    return 1;

  //    Sprintf(name,"w1-first");
  //    DBRep::Set(name,mkPipe1.FirstShape());

  //    Sprintf(name,"w1-last");
  //    DBRep::Set(name,mkPipe1.LastShape());

  //    Sprintf(name,"w2-first");
  //    DBRep::Set(name,mkPipe2.FirstShape());

  //    Sprintf(name,"w2-last");
  //    DBRep::Set(name,mkPipe2.LastShape());

  BRepOffsetAPI_Sewing SewIt(1.0e-4);

  // Make tube
  TopExp_Explorer getFaces;
  TopoDS_Face     test_face;
  getFaces.Init(mkPipe1.Shape(), TopAbs_FACE);
  while (getFaces.More())
  {
    SewIt.Add(getFaces.Current().Reversed());
    getFaces.Next();
  }

  // Make face for first opening
  occ::handle<Geom_Plane> Plane1 = new Geom_Plane(circ1Plane);
  mkFace.Init(Plane1, false, Precision::Confusion());
  mkFace.Add(TopoDS::Wire(outerWire1_));
  mkFace.Add(TopoDS::Wire(Wire1_.Reversed()));
  if (!mkFace.IsDone())
    return 1;
  TopoDS_Face Face1 = mkFace.Face();

  // Make face for second opening
  occ::handle<Geom_Plane> Plane2 = new Geom_Plane(circ2Plane);
  mkFace.Init(Plane2, false, Precision::Confusion());
  mkFace.Add(TopoDS::Wire(outerWire2_));
  mkFace.Add(TopoDS::Wire(Wire2_.Reversed()));
  if (!mkFace.IsDone())
    return 1;
  TopoDS_Face Face2 = mkFace.Face();

  // Grab the gas solid now that we've extracted the faces.
  mkPipe1.MakeSolid();
  myShape = TopoDS::Solid(mkPipe1.Shape());

  getFaces.Clear();
  getFaces.Init(mkPipe2.Shape(), TopAbs_FACE);
  while (getFaces.More())
  {
    SewIt.Add(getFaces.Current());
    getFaces.Next();
  }

  SewIt.Add(Face1.Reversed());
  SewIt.Add(Face2);

  SewIt.Perform();

  di << "The result of the Sewing operation is a ";
  // Check to see if we have a solid
  switch (SewIt.SewedShape().ShapeType())
  {
    case (TopAbs_COMPOUND):
      di << "TopAbs_COMPOUND\n";
      break;
    case (TopAbs_COMPSOLID):
      di << "TopAbs_COMPSOLID\n";
      break;
    case (TopAbs_SOLID):
      di << "TopAbs_SOLID\n";
      break;
    case (TopAbs_SHELL):
      di << "TopAbs_SHELL\n";
      break;
    case (TopAbs_FACE):
      di << "TopAbs_FACE\n";
      break;
    case (TopAbs_WIRE):
      di << "TopAbs_WIRE\n";
      break;
    case (TopAbs_EDGE):
      di << "TopAbs_EDGE\n";
      break;
    case (TopAbs_VERTEX):
      di << "TopAbs_VERTEX\n";
      break;
    case (TopAbs_SHAPE):
      di << "TopAbs_SHAPE\n";
  }

  BRep_Builder B;

  TopoDS_Shell TubeShell;
  di << "Can we turn it into a shell? ";
  try
  {
    OCC_CATCH_SIGNALS
    TubeShell = TopoDS::Shell(SewIt.SewedShape());
    B.MakeSolid(wallSolid);
    B.Add(wallSolid, TubeShell);
    di << " yes\n";
  }
  catch (Standard_TypeMismatch const&)
  {
    di << "Can't convert to shell...\n";
    TopExp_Explorer getSol;
    getSol.Init(SewIt.SewedShape(), TopAbs_SOLID);
    if (getSol.More())
    {
      di << "First solid found in compound\n";
      wallSolid = TopoDS::Solid(getSol.Current());
      TopoDS_Solid test_solid;
      while (getSol.More())
      {
        di << "Next solid found in compound\n";
        getSol.Next();
        test_solid = TopoDS::Solid(getSol.Current());

        di << "BRepAlgoAPI_Fuse fuser(test_solid, wallSolid)\n";
        BRepAlgoAPI_Fuse fuser(test_solid, wallSolid);
        TopExp_Explorer  aExpS(fuser.Shape(), TopAbs_SOLID);
        if (aExpS.More())
        {
          wallSolid = TopoDS::Solid(aExpS.Current());
        }
      }
    }
    else
    {
      // Let's see if we can extract shells instead of solids.
      TopExp_Explorer getShel;
      getShel.Init(SewIt.SewedShape(), TopAbs_SHELL);
      if (getShel.More())
      {
        di << "First shell found in compound\n";
        B.MakeSolid(wallSolid);
        di << "B.Add(wallSolid,TopoDS::Shell(getShel.Current()));\n";
        int i = 1;
        while (getShel.More())
        {
          di << "Next shell found in compound\n";
          di << "B.Add(wallSolid,TopoDS::Shell(getShel.Current()));\n";
          Sprintf(name, "shell%d", i++);
          DBRep::Set(name, getShel.Current());
          B.Add(wallSolid, TopoDS::Shell(getShel.Current()));
          getShel.Next();
        }
      }
    }
  }

  Sprintf(name, "result");
  DBRep::Set(name, wallSolid);

  // Now calculated the volume of the outside tube.
  GProp_GProps gprops;
  BRepGProp::VolumeProperties(wallSolid, gprops);
  di << "The wallSolid's volume is: " << gprops.Mass() << "\n";

  if (check)
  {
    if (!(BRepCheck_Analyzer(wallSolid).IsValid()))
      di << "The TopoDS_Solid was checked, and it was invalid!\n";
    else
      di << "The TopoDS_Solid was checked, and it was valid.\n";
    if (!wallSolid.Closed())
      di << "The TopoDS_Solid is not closed!\n";
    else
      di << "The TopoDS_Solid is closed.\n";
    if (!wallSolid.Checked())
      di << "The TopoDS_Solid is not checked!\n";
    else
      di << "The TopoDS_Solid has been checked.\n";
    if (wallSolid.Infinite())
      di << "The TopoDS_Solid is infinite!\n";
    else
      di << "The TopoDS_Solid is finite.\n";
  }

  di << "The result is a ";
  // Check to see if we have a solid
  switch (wallSolid.ShapeType())
  {
    case (TopAbs_COMPOUND):
      di << "TopAbs_COMPOUND\n";
      break;
    case (TopAbs_COMPSOLID):
      di << "TopAbs_COMPSOLID\n";
      break;
    case (TopAbs_SOLID):
      di << "TopAbs_SOLID\n";
      break;
    case (TopAbs_SHELL):
      di << "TopAbs_SHELL\n";
      break;
    case (TopAbs_FACE):
      di << "TopAbs_FACE\n";
      break;
    case (TopAbs_WIRE):
      di << "TopAbs_WIRE\n";
      break;
    case (TopAbs_EDGE):
      di << "TopAbs_EDGE\n";
      break;
    case (TopAbs_VERTEX):
      di << "TopAbs_VERTEX\n";
      break;
    case (TopAbs_SHAPE):
      di << "TopAbs_SHAPE\n";
  }

  return 0;
}

void QABugs::Commands_13(Draw_Interpretor& theCommands)
{
  const char* group = "QABugs";

  theCommands.Add("OCC544",
                  "OCC544 [[[[[wT [[[[d1 [[[d2 [[R [length ]]]]]",
                  __FILE__,
                  OCC544,
                  group);

  return;
}
