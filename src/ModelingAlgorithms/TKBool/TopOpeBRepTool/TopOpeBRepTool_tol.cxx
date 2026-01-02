// Created on: 1997-04-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#include <TopOpeBRepTool_tol.hxx>

#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <Bnd_Box.hxx>
#include <TopOpeBRepTool_box.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TCollection_AsciiString.hxx>

Standard_EXPORT void FTOL_FaceTolerances(const Bnd_Box&             B1,
                                         const Bnd_Box&             B2,
                                         const TopoDS_Face&         myFace1,
                                         const TopoDS_Face&         myFace2,
                                         const BRepAdaptor_Surface& mySurface1,
                                         const BRepAdaptor_Surface& mySurface2,
                                         double&                    myTol1,
                                         double&                    myTol2,
                                         double&                    Deflection,
                                         double&                    MaxUV)
{
  double aTolF1 = BRep_Tool::Tolerance(myFace1);
  double aTolF2 = BRep_Tool::Tolerance(myFace2);
  //
  myTol1 = aTolF1 + aTolF2;
  myTol2 = myTol1;

  double x0, y0, z0, x1, y1, z1, dx, dy, dz;
  bool   Box1OK, Box2OK;

  Box1OK = !B1.IsOpenXmin() && !B1.IsOpenXmax() && !B1.IsOpenYmin() && !B1.IsOpenYmax()
           && !B1.IsOpenZmin() && !B1.IsOpenZmax() && !B1.IsVoid();

  Box2OK = !B2.IsOpenXmin() && !B2.IsOpenXmax() && !B2.IsOpenYmin() && !B2.IsOpenYmax()
           && !B2.IsOpenZmin() && !B2.IsOpenZmax() && !B2.IsVoid();

  if (Box1OK)
  {
    B1.Get(x0, y0, z0, x1, y1, z1);
    dx = x1 - x0;
    dy = y1 - y0;
    dz = z1 - z0;

    if (Box2OK)
    {
      B2.Get(x0, y0, z0, x1, y1, z1);
      double _dx = x1 - x0;
      double _dy = y1 - y0;
      double _dz = z1 - z0;
      if (_dx > dx)
        dx = _dx;
      if (_dy > dy)
        dy = _dy;
      if (_dz > dz)
        dz = _dz;
    }
  }
  else
  {
    if (Box2OK)
    {
      B2.Get(x0, y0, z0, x1, y1, z1);
      dx = x1 - x0;
      dy = y1 - y0;
      dz = z1 - z0;
    }
    else
    {
      dx = dy = dz = 1.0;
    }
  }
  if (dx < dy)
    dx = dy;
  if (dx < dz)
    dx = dz;
  if (dx > 1000000.0)
    dx = 1000000.0; //-- if(dx>10000.0) dx=10000.0;

  TopExp_Explorer ex;
  double          tolef1 = Precision::Confusion();
  for (ex.Init(myFace1, TopAbs_EDGE); ex.More(); ex.Next())
  {
    double tole = BRep_Tool::Tolerance(TopoDS::Edge(ex.Current()));
    if (tole > tolef1)
      tolef1 = tole;
  }
  double tolef2 = Precision::Confusion();
  for (ex.Init(myFace2, TopAbs_EDGE); ex.More(); ex.Next())
  {
    double tole = BRep_Tool::Tolerance(TopoDS::Edge(ex.Current()));
    if (tole > tolef2)
      tolef2 = tole;
  }
  double tolef = tolef1;
  if (tolef2 > tolef)
    tolef = tolef2;
  // jmb le 30 juillet 99. on ne multiplie pas la tolerance par la dimension de la piece

  Deflection = 0.01;
  MaxUV      = 0.01;
  Deflection *= dx;

  double MDEFLECTION = Deflection;
  double MMAXUV      = 0.01;

  double MU0, MU1, MV0, MV1, DP1, DP2, DP;
  MU0 = mySurface1.FirstUParameter();
  MU1 = mySurface1.LastUParameter();
  MV0 = mySurface1.FirstVParameter();
  MV1 = mySurface1.LastVParameter();
  DP1 = MU1 - MU0;
  DP2 = MV1 - MV0;
  if (DP2 < DP1)
    DP = DP2;
  else
    DP = DP1; //-- DP + petit

  MU0 = mySurface2.FirstUParameter();
  MU1 = mySurface2.LastUParameter();
  MV0 = mySurface2.FirstVParameter();
  MV1 = mySurface2.LastVParameter();
  DP1 = MU1 - MU0;
  DP2 = MV1 - MV0;
  if (DP2 > DP1)
    DP1 = DP2; //-- DP1 + petit
  if (DP1 < DP)
    DP = DP1; //-- DP + petit

  DP *= 0.01;
  // jmb le 30 juillet 99
  //  non ! laisser MMAXUV a 0.01
  //   MMAXUV=DP;

#ifdef OCCT_DEBUG
//  printf("\n FaceTolerances3d : TOL1 = %5.5eg TOL2=%5.5eg DEFL=%5.5eg
//  MAXUV=%5.5eg\n",MTOL1,MTOL2,MDEFLECTION,0.01);
#endif

  if (MMAXUV < 1e-3)
    MMAXUV = 1e-3;
  if (MDEFLECTION < 1e-3)
    MDEFLECTION = 1e-3;

  if (MMAXUV > 0.01)
    MMAXUV = 0.01;
  if (MDEFLECTION > 0.1)
    MDEFLECTION = 0.1;

#ifdef OCCT_DEBUG
//  printf("TOL1 = %5.5eg TOL2=%5.5eg DEFL=%5.5eg MAXUV=%5.5eg\n",MTOL1,MTOL2,MDEFLECTION,MMAXUV);
#endif

  Deflection = MDEFLECTION;
  MaxUV      = MMAXUV;
} // FTOL_FaceTolerances

Standard_EXPORT void FTOL_FaceTolerances3d(const TopoDS_Face& myFace1,
                                           const TopoDS_Face& myFace2,
                                           double&            Tol)
{
  const occ::handle<TopOpeBRepTool_HBoxTool>& hbt = FBOX_GetHBoxTool();
  Bnd_Box                                     B1, B2;
  if (hbt->HasBox(myFace1))
  {
    B1 = hbt->Box(myFace1);
  }
  else
  {
    B1.Update(0., 0., 0., 1., 1., 1.);
  }
  if (hbt->HasBox(myFace2))
  {
    B2 = hbt->Box(myFace2);
  }
  else
  {
    B2.Update(0., 0., 0., 1., 1., 1.);
  }
  BRepAdaptor_Surface mySurface1;
  BRepAdaptor_Surface mySurface2;
  mySurface1.Initialize(myFace1);
  mySurface2.Initialize(myFace2);
  double Deflection = 0.01, MaxUV = 0.01;
  double myTol1, myTol2;
  FTOL_FaceTolerances(B1,
                      B2,
                      myFace1,
                      myFace2,
                      mySurface1,
                      mySurface2,
                      myTol1,
                      myTol2,
                      Deflection,
                      MaxUV);
  myTol1 = (myTol1 > 1.e-4) ? 1.e-4 : myTol1;
  myTol2 = (myTol2 > 1.e-4) ? 1.e-4 : myTol2;
  Tol    = std::max(myTol1, myTol2);
}

Standard_EXPORT void FTOL_FaceTolerances3d(const Bnd_Box&             B1,
                                           const Bnd_Box&             B2,
                                           const TopoDS_Face&         myFace1,
                                           const TopoDS_Face&         myFace2,
                                           const BRepAdaptor_Surface& mySurface1,
                                           const BRepAdaptor_Surface& mySurface2,
                                           double&                    myTol1,
                                           double&                    myTol2,
                                           double&                    Deflection,
                                           double&                    MaxUV)
{
  FTOL_FaceTolerances(B1,
                      B2,
                      myFace1,
                      myFace2,
                      mySurface1,
                      mySurface2,
                      myTol1,
                      myTol2,
                      Deflection,
                      MaxUV);
}

Standard_EXPORT void FTOL_FaceTolerances2d(const Bnd_Box&             B1,
                                           const Bnd_Box&             B2,
                                           const TopoDS_Face&         myFace1,
                                           const TopoDS_Face&         myFace2,
                                           const BRepAdaptor_Surface& mySurface1,
                                           const BRepAdaptor_Surface& mySurface2,
                                           double&                    myTol1,
                                           double&                    myTol2)
{
  double BIDDeflection, BIDMaxUV;
  FTOL_FaceTolerances(B1,
                      B2,
                      myFace1,
                      myFace2,
                      mySurface1,
                      mySurface2,
                      myTol1,
                      myTol2,
                      BIDDeflection,
                      BIDMaxUV);
}
