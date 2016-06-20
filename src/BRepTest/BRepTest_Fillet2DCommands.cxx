// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified:	Tue Oct 15 10:12:02 1996
//              Add ChFi2d_TangencyError (PRO3529)
// Modified:	Fri Sep 25 09:38:04 1998
//              status = ChFi2d_NotAuthorized if edges are not
//              lines or circles  (BUC60288) + partial_result


#include <TColgp_Array1OfPnt2d.hxx>
#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <BRepFilletAPI_MakeFillet2d.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopTools_MapOfShape.hxx>

#include <ChFi2d_FilletAPI.hxx>
#include <ChFi2d_ChamferAPI.hxx>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRep_Builder.hxx>

//=======================================================================
//function : chfi2d
//purpose  : 2d fillets and chamfers
//=======================================================================

static Standard_Integer chfi2d(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) {
    di << "chfi2d : not enough args";
    return 1;
  }

  // set up the algorithm
  TopoDS_Shape F = DBRep::Get(a[2],TopAbs_FACE);
  if (F.IsNull()) {
    di << "chfi2d : "<< a[2] << " not a face";
    return 1;
  }

  BRepFilletAPI_MakeFillet2d MF(TopoDS::Face(F));
  if (MF.Status() == ChFi2d_NotPlanar) {
    di << "chfi2d : not a planar face";
    return 1;
  }

  TopoDS_Shape res;
  Standard_Boolean partial_result = Standard_False;
  Standard_Integer i = 3;
  while (i+1 < n) {
    
    TopoDS_Shape aLocalEdge(DBRep::Get(a[i],TopAbs_EDGE));
    TopoDS_Edge E1 = TopoDS::Edge(aLocalEdge);
    aLocalEdge = DBRep::Get(a[i+1],TopAbs_EDGE);
    TopoDS_Edge E2 = TopoDS::Edge(aLocalEdge);
//    TopoDS_Edge E1 = TopoDS::Edge(DBRep::Get(a[i],TopAbs_EDGE));
//    TopoDS_Edge E2 = TopoDS::Edge(DBRep::Get(a[i+1],TopAbs_EDGE));

    if (E1.IsNull() || E2.IsNull()) {
      di << "chfi2d : " << a[i] << " or " << a[i+1] << " not an edge";
      if (partial_result) {
	di <<" WARNING : this is a partial result ";
	DBRep::Set(a[1],res);
      }
      return 1;
    }

    TopoDS_Vertex V;
    if (!TopExp::CommonVertex(E1,E2,V)) {
      di << "chfi2d " <<  a[i] << " and " << a[i+1] << " does not share a vertex";
      if (partial_result) {
	di <<" WARNING : this is a partial result ";
	DBRep::Set(a[1],res);
      }
      return 1;
    }

    i += 2;
    if (i+1 >= n) {
      di << "chfi2d : not enough args";
      if (partial_result) {
	di <<" WARNING : this is a partial result ";
	DBRep::Set(a[1],res);
      }
      return 1;
    }

    Standard_Real p1 = Draw::Atof(a[i+1]);
    if (*a[i] == 'F') {
      MF.AddFillet(V,p1);
    }
    else {
      if (i+2 >= n) {
	di << "chfi2d : not enough args";
	if (partial_result) {
	  di <<" WARNING : this is a partial result ";
	  DBRep::Set(a[1],res);
	}
	return 1;
      }
      Standard_Real p2 = Draw::Atof(a[i+2]);
      if (a[i][2] == 'D') {
	MF.AddChamfer(E1,E2,p1,p2);
      }
      else {
	MF.AddChamfer(E1,V,p1,p2 * (M_PI / 180.0));
      }
    }

    if (MF.Status() == ChFi2d_TangencyError) {
      di << "chfi2d : " <<  a[i-2] << " and " << a[i-1] << " are tangent ";
      if (partial_result) {
	di <<" WARNING : this is a partial result ";
	DBRep::Set(a[1],res);
      }
      return 1;
    }

    if (MF.Status() == ChFi2d_NotAuthorized) {
      di << "chfi2d : " <<  a[i-2] << " or " << a[i-1] << " is not a line or a circle ";
      if (partial_result) {
	di <<" WARNING : this is a partial result ";
	DBRep::Set(a[1],res);
      }
      return 1;
    }

    if (MF.Status() != ChFi2d_IsDone) {
      di << "chfi2d : operation failed on " << a[i-2];
      if (partial_result) {
	di <<" WARNING : this is a partial result ";
	DBRep::Set(a[1],res);
      }
      return 1;
    }
    else {
      partial_result = Standard_True;
      MF.Build();
      res = MF.Shape();
    }
    
    if (*a[i] == 'F') {
      i +=2;
    }
    else {
      i +=3;
    }
  }
  
  MF.Build();
  DBRep::Set(a[1],MF);

  return 0;
}

//=======================================================================
//function : fillet2d
//purpose  : A method to find a plane for 2 edges.
//         : It may return a NULL object of the plane is not found
//         : (the edge are located not in a plane).
//=======================================================================

static Handle(Geom_Plane) findPlane(const TopoDS_Shape& S)
{
  Handle(Geom_Plane) plane;
  BRepBuilderAPI_FindPlane planeFinder(S);
  if (planeFinder.Found())
    plane = planeFinder.Plane();
  return plane;
}

static Handle(Geom_Plane) findPlane(const TopoDS_Shape& E1, const TopoDS_Shape& E2)
{
  BRep_Builder B;
  TopoDS_Compound C;
  B.MakeCompound(C);
  B.Add(C, E1);
  B.Add(C, E2);
  return findPlane(C);
}

//=======================================================================
//function : findCommonPoint
//purpose  : Find a common (or the most close) point of two edges.
//=======================================================================

static gp_Pnt findCommonPoint(const TopoDS_Shape& E1, const TopoDS_Shape& E2)
{
  TopoDS_Vertex v11, v12, v21, v22;
  TopExp::Vertices(TopoDS::Edge(E1), v11, v12);
  TopExp::Vertices(TopoDS::Edge(E2), v21, v22);

  gp_Pnt p11 = BRep_Tool::Pnt(v11);
  gp_Pnt p12 = BRep_Tool::Pnt(v12);
  gp_Pnt p21 = BRep_Tool::Pnt(v21);
  gp_Pnt p22 = BRep_Tool::Pnt(v22);

  gp_Pnt common;
  const double d1121 = p11.SquareDistance(p21);
  const double d1122 = p11.SquareDistance(p22);
  const double d1221 = p12.SquareDistance(p21);
  const double d1222 = p12.SquareDistance(p22);
  if (d1121 < d1122 && d1121 < d1221 && d1121 < d1222)
    common = p11;
  else if (d1122 < d1121 && d1122 < d1221 && d1122 < d1222)
    common = p11;
  else if (d1221 < d1121 && d1221 < d1122 && d1221 < d1222)
    common = p12;
  else if (d1222 < d1121 && d1222 < d1122 && d1222 < d1221)
    common = p12;
  
  return common;
}

static gp_Pnt findCommonPoint(const TopoDS_Shape& W)
{
  // The common point for two edges inside a wire
  // is a sharing vertex of two edges.
  TopTools_MapOfShape vertices;
  TopExp_Explorer expl(W, TopAbs_VERTEX);
  for (; expl.More(); expl.Next())
  {
    if (!vertices.Add(expl.Current()))
    {
      return BRep_Tool::Pnt(TopoDS::Vertex(expl.Current()));
    }
  }
  return gp::Origin(); // not found
}

//=======================================================================
//function : fillet2d
//purpose  : Fillet 2d based on Newton method (recursive, iteration)
//usage    : fillet2d result wire (or edge1 edge2) radius
//=======================================================================

static Standard_Integer fillet2d(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 4 && n != 5) 
  {
    di << "Usage : fillet2d result wire (or edge1 edge2) radius";
    return 1;
  }

  TopoDS_Shape E1, E2, W;
  if (n == 5)
  {
    // Get the edges.
    E1 = DBRep::Get(a[2], TopAbs_EDGE, Standard_True);
    E2 = DBRep::Get(a[3], TopAbs_EDGE, Standard_True);
  }
  else
  {
    // Get the wire.
    W = DBRep::Get(a[2], TopAbs_WIRE, Standard_True);
  }

  // Get the radius value.
  const Standard_Real radius = Atof(n == 5 ? a[4] : a[3]);

  // Find plane of the edges.
  Handle(Geom_Plane) hPlane = n == 5 ? findPlane(E1, E2) : findPlane(W);
  if (hPlane.IsNull())
  {
    di << "Error: the edges are located not in a plane.";
    return 1;
  }

  // Algo.
  ChFi2d_FilletAPI algo;
  gp_Pln plane = hPlane->Pln();
  if (n == 5)
  {
    const TopoDS_Edge& e1 = TopoDS::Edge(E1);
    const TopoDS_Edge& e2 = TopoDS::Edge(E2);
    algo.Init(e1, e2, plane);
  }
  else
  {
    const TopoDS_Wire& w = TopoDS::Wire(W);
    algo.Init(w, plane);
  }
  Standard_Boolean status = algo.Perform(radius);
  if (!status)
  {
    di << "Error: the algrithm failed.";
    return 1;
  }

  // Find a common point of the edges.
  gp_Pnt common = n == 5 ? findCommonPoint(E1, E2) : findCommonPoint(W);

  // Get the number of solutions (usually it is equal to 1).
  Standard_Integer nbSolutions = algo.NbResults(common);
  if (!nbSolutions)
  {
    di << "Error: no solutions.";
    return 1;
  }

  // Get the result for the "nearest" solution (near the common point).
  TopoDS_Edge M1, M2; // modified E1 and E2
  TopoDS_Edge fillet = algo.Result(common, M1, M2);
  if (fillet.IsNull())
  {
    di << "Error: the algrithm produced no result.";
    return 1;
  }

  // Set result for DRAW.
  DBRep::Set(a[1], fillet);

  // Update neighbour edges in DRAW.
  if (n == 5)
  {
    DBRep::Set(a[2], M1);
    DBRep::Set(a[3], M2);
  }
  else // recreate the wire using the fillet
  {
    BRepBuilderAPI_MakeWire mkWire(M1, fillet, M2);
    if (mkWire.IsDone())
      DBRep::Set(a[1], mkWire.Wire());
    else
      DBRep::Set(a[1], fillet);
  }
  return 0;
}

//=======================================================================
//function : chamfer2d
//purpose  : Chamfer 2d.
//usage    : chamfer2d result wire (or edge1 edge2) length1 length2
//=======================================================================

static Standard_Integer chamfer2d(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 5 && n != 6) 
  {
    di << "Usage : chamfer2d result wire (or edge1 edge2) length1 length2";
    return 1;
  }

  TopoDS_Shape W;
  TopoDS_Shape E1, E2;
  if (n == 6)
  {
    // Get the edges.
    E1 = DBRep::Get(a[2], TopAbs_EDGE, Standard_True);
    E2 = DBRep::Get(a[3], TopAbs_EDGE, Standard_True);
  }
  else 
  {
    W = DBRep::Get(a[2], TopAbs_WIRE, Standard_True);
  }

  // Get the lengths.
  const Standard_Real length1 = (n == 6) ? Atof(a[4]) : Atof(a[3]);
  const Standard_Real length2 = (n == 6) ? Atof(a[5]) : Atof(a[4]);

  // Algo.
  ChFi2d_ChamferAPI algo;
  if (n == 6)
  {
    const TopoDS_Edge& e1 = TopoDS::Edge(E1);
    const TopoDS_Edge& e2 = TopoDS::Edge(E2);
    algo.Init(e1, e2);
  }
  else
  {
    const TopoDS_Wire& w = TopoDS::Wire(W);
    algo.Init(w);
  }

  // Prepare the chamfer.
  algo.Perform();

  // Get the result.
  TopoDS_Edge M1, M2; // modified E1 and E2
  TopoDS_Edge chamfer = algo.Result(M1, M2, length1, length2);
  if (chamfer.IsNull())
  {
    di << "Error: the algrithm produced no result.";
    return 1;
  }

  if (n == 6)
  {
    // Set result for DRAW.
    DBRep::Set(a[1], chamfer);
    
    // Update neighbour edges in DRAW.
    DBRep::Set(a[2], M1);
    DBRep::Set(a[3], M2);
  }
  else // recreate the wire using the chamfer
  {
    BRepBuilderAPI_MakeWire mkWire(M1, chamfer, M2);
    if (mkWire.IsDone())
      DBRep::Set(a[1], mkWire.Wire());
    else
      DBRep::Set(a[1], chamfer);
  }

  return 0;
}

//=======================================================================
//function : Fillet2DCommands
//purpose  : 
//=======================================================================

void  BRepTest::Fillet2DCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "TOPOLOGY Fillet2D construction commands";
   
  theCommands.Add("chfi2d","chfi2d result face [edge1 edge2 (F radius/CDD d1 d2/CDA d ang) ....]",__FILE__,chfi2d,g);
  theCommands.Add("fillet2d","fillet2d result wire (or edge1 edge2) radius",__FILE__,fillet2d,g);
  theCommands.Add("chamfer2d","chamfer2d result wire (or edge1 edge2) length1 length2",__FILE__,chamfer2d,g);
}
