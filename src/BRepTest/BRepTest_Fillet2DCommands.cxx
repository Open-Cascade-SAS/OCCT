// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
#include <TopoDS.hxx>
#include <TopExp.hxx>

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
}
