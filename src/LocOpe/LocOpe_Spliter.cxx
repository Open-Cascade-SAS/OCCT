// Created on: 1996-01-09
// Created by: Jacques GOUSSARD
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


//  Modified by skv - Mon May 31 12:34:09 2004 OCC5865

#include <LocOpe_Spliter.ixx>

#include <LocOpe_ProjectedWires.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>

#include <LocOpe_BuildWires.hxx>

#include <gp_Vec2d.hxx>
#include <gp_Vec.hxx>
#include <Geom_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRep_Tool.hxx>

#include <BRepTools_Substitution.hxx>
#include <LocOpe_SplitShape.hxx>

#include <BRep_Builder.hxx>

#include <TopoDS.hxx>
#include <TopExp.hxx>

#include <Standard_ConstructionError.hxx>


//  Modified by skv - Mon May 31 13:00:30 2004 OCC5865 Begin
// static void RebuildWires(TopTools_ListOfShape&);
static void RebuildWires(TopTools_ListOfShape&,
			 const Handle(LocOpe_ProjectedWires)&);
//  Modified by skv - Mon May 31 13:00:31 2004 OCC5865 End

static void Put(const TopoDS_Shape&,
                TopTools_DataMapOfShapeListOfShape&);

static void Select(const TopoDS_Edge&,
		   TopTools_ListOfShape&);


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void LocOpe_Spliter::Perform(const Handle(LocOpe_ProjectedWires)& PW)
{
  if (myShape.IsNull()) {
    Standard_NullObject::Raise();
  }
  myDone = Standard_False;
  myMap.Clear();
  myRes.Nullify();

  Put(myShape,myMap);

  TopTools_MapOfShape mapV,mapE;
  TopTools_DataMapOfShapeShape EdgOnEdg;
  TopTools_IndexedDataMapOfShapeListOfShape mapFE;
  TopExp_Explorer exp,exp2;
  
  // 1ere etape : substitution des vertex

  TopoDS_Vertex Vb;
  TopTools_ListOfShape lsubs;
  BRepTools_Substitution theSubs;
  BRep_Builder BB;

  for (PW->InitEdgeIterator(); PW->MoreEdge(); PW->NextEdge()) {
    const TopoDS_Edge& edg = PW->Edge();
    mapE.Add(edg);
    for (exp.Init(edg,TopAbs_VERTEX); exp.More(); exp.Next()) {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(exp.Current());
      if (!mapV.Contains(vtx)) {
	if (PW->OnVertex(vtx,Vb)) {
	  mapV.Add(vtx);
	  lsubs.Clear();
	  TopoDS_Vertex vsub = TopoDS::Vertex(vtx.Oriented(TopAbs_FORWARD));
	  gp_Pnt p1 = BRep_Tool::Pnt(vsub), p2 = BRep_Tool::Pnt(Vb);
	  Standard_Real d = p1.Distance(p2);
	  d = d + BRep_Tool::Tolerance(Vb);
	  BB.UpdateVertex(vsub, d);
	  lsubs.Append(vsub);
	  theSubs.Substitute(Vb.Oriented(TopAbs_FORWARD),lsubs);
	}
	
      }
    }
  }

  theSubs.Build(myShape);
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itdesc(myMap);
  if (theSubs.IsCopied(myShape)) {
    // on n`a fait que des substitutions de vertex. Donc chaque element
    // est remplace par lui meme ou par un seul element du meme type.
    for (; itdesc.More(); itdesc.Next()) {
      if (theSubs.IsCopied(itdesc.Key())) {
	const TopTools_ListOfShape& lsub = theSubs.Copy(itdesc.Key());
#ifdef DEB
	if (lsub.Extent() != 1) {
	  Standard_ConstructionError::Raise();
	}
#endif
	myMap(itdesc.Key()).Clear(); 
	myMap(itdesc.Key()).Append(lsub.First()); 
      }
    }
  }

  myRes = myMap(myShape).First();
  LocOpe_SplitShape theCFace(myRes);

  // Adds every vertices lying on an edge of the shape, and prepares 
  // work to rebuild wires on each face
  TopoDS_Edge Ed;
  Standard_Real prm;

  TopTools_MapOfShape theFacesWithSection;
  for (PW->InitEdgeIterator(); PW->MoreEdge(); PW->NextEdge()) {
    const TopoDS_Edge& edg = PW->Edge();
    for (exp.Init(edg,TopAbs_VERTEX); exp.More(); exp.Next()) {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(exp.Current());
      if (!mapV.Contains(vtx)) {
	mapV.Add(vtx);
	if (PW->OnEdge(vtx,Ed,prm)) {
	  // on devrait verifier que le vtx n`existe pas deja sur l`edge
	  if(!myMap.IsBound(Ed)) continue;
	  Ed = TopoDS::Edge(myMap(Ed).First());
	  theCFace.Add(vtx,prm,Ed);
	}
      }
    }
    TopoDS_Edge Ebis;
    if (PW->OnEdge(Ebis)) {
      //	Ebis = TopoDS::Edge(myMap(Ebis).First());
      EdgOnEdg.Bind(edg,Ebis);
    }
    else {
      TopoDS_Face fac = PW->OnFace();
      if(!myMap.IsBound(fac)) continue;
      Standard_Boolean IsFaceWithSec = PW->IsFaceWithSection(fac);
      fac = TopoDS::Face(myMap(fac).First());
      if (IsFaceWithSec)
        theFacesWithSection.Add(fac);
      if (!mapFE.Contains(fac)) {
        TopTools_ListOfShape thelist;
	mapFE.Add(fac, thelist);
      }
      mapFE.ChangeFromKey(fac).Append(edg);
    }
  }

  // Rebuilds wires on each face of the shape

  TopTools_ListIteratorOfListOfShape itl;
  for (Standard_Integer i=1; i<=mapFE.Extent(); i++) {
    const TopoDS_Face& fac = TopoDS::Face(mapFE.FindKey(i));
    TopTools_ListOfShape& ledges = mapFE(i);
//  Modified by skv - Mon May 31 12:32:54 2004 OCC5865 Begin
//     RebuildWires(ledges);
    RebuildWires(ledges, PW);
//  Modified by skv - Mon May 31 12:32:54 2004 OCC5865 End
    if (theFacesWithSection.Contains(fac))
      theCFace.Add(ledges, fac);
    else
      for (itl.Initialize(ledges); itl.More(); itl.Next())
        theCFace.Add(TopoDS::Wire(itl.Value()),fac);
  }


  // Mise a jour des descendants

  for (itdesc.Reset(); itdesc.More(); itdesc.Next()) {
    const TopoDS_Shape& sori = itdesc.Key();
    const TopoDS_Shape& scib = itdesc.Value().First();
    myMap(sori) = theCFace.DescendantShapes(scib);
  }

  const TopTools_ListOfShape& lres = myMap(myShape);

  TopAbs_ShapeEnum typS = myShape.ShapeType();
  if (typS == TopAbs_FACE && lres.Extent() >=2) {
    BRep_Builder B;
    myRes.Nullify();
    B.MakeShell(TopoDS::Shell(myRes));
    myRes.Orientation(TopAbs_FORWARD);
    for (itl.Initialize(lres); itl.More(); itl.Next()) {
      B.Add(myRes,itl.Value().Oriented(myShape.Orientation()));
    }
  }
  else if (typS == TopAbs_EDGE && lres.Extent() >=2) {
    BRep_Builder B;
    myRes.Nullify();
    B.MakeWire(TopoDS::Wire(myRes));
    myRes.Orientation(TopAbs_FORWARD);
    for (itl.Initialize(lres); itl.More(); itl.Next()) {
      B.Add(myRes,itl.Value().Oriented(myShape.Orientation()));
    }
  }
  else {
    if (lres.Extent() != 1) {
      return;
    }
    myRes = lres.First();
  }

  theSubs.Clear();
  for (TopTools_DataMapIteratorOfDataMapOfShapeShape itee(EdgOnEdg);
       itee.More();
       itee.Next()) {
    const TopoDS_Edge& e1 = TopoDS::Edge(itee.Key());
    // on recherche dans les descendants de e2 l`edge qui correspont a e1

    TopoDS_Vertex vf1,vl1,vf2,vl2;
    TopExp::Vertices(e1,vf1,vl1);
    lsubs.Clear();
    for (itl.Initialize(myMap(itee.Value()));
	 itl.More();
	 itl.Next()) {
      const TopoDS_Edge& e2 = TopoDS::Edge(itl.Value());
      TopExp::Vertices(e2,vf2,vl2);

      if (!vl1.IsSame(vf1)) {
	if (vf1.IsSame(vf2) && vl1.IsSame(vl2)) {
	  lsubs.Append(e2.Oriented(TopAbs_FORWARD));
	  //	break;
	}
	else if (vf1.IsSame(vl2) && vl1.IsSame(vf2)) {
	  lsubs.Append(e2.Oriented(TopAbs_REVERSED));
	  //	break;
	}
      }
      else { // discrimination sur les tangentes
	if (vf2.IsSame(vl2) && vl2.IsSame(vl1)) { // tout au meme point
	  TopLoc_Location Loc;
	  Standard_Real f,l;
	  gp_Pnt pbid;
	  gp_Vec v1,v2;
	  Handle(Geom_Curve) C = BRep_Tool::Curve(e1,Loc,f,l);
	  C->D1(f,pbid,v1);
	  v1.Transform(Loc.Transformation());

	  C = BRep_Tool::Curve(e2,Loc,f,l);
	  C->D1(f,pbid,v2);
	  v2.Transform(Loc.Transformation());
	  if (v1.Dot(v2) >0.) {
	    lsubs.Append(e2.Oriented(TopAbs_FORWARD));
	  }
	  else {
	    lsubs.Append(e2.Oriented(TopAbs_REVERSED));
	  }
	}
      }

    }
    if (lsubs.Extent() >= 2) { // il faut faire un choix
      Select(e1,lsubs);
    }
    if (lsubs.Extent() == 1) {
      TopoDS_Shape ebase = lsubs.First();
      lsubs.Clear();
      lsubs.Append(e1.Oriented(ebase.Orientation()));
      theSubs.Substitute(ebase.Oriented(TopAbs_FORWARD),lsubs);
    }
    else {
#ifdef DEB
      cout << "Pb pour substitution" << endl;
#endif
    }
  }

  theSubs.Build(myRes);

  for (itdesc.Reset(); itdesc.More(); itdesc.Next()) {
    TopTools_ListOfShape& ldesc = myMap(itdesc.Key());
    TopTools_ListOfShape newdesc;
    for (itl.Initialize(ldesc); itl.More(); itl.Next()) {
      if (theSubs.IsCopied(itl.Value())) {
	const TopTools_ListOfShape& lsub = theSubs.Copy(itl.Value());
#ifdef DEB
	if (lsub.Extent() != 1) {
	  Standard_ConstructionError::Raise();
	}
#endif
	newdesc.Append(lsub.First());
      }
      else {
	newdesc.Append(itl.Value());
      }
    }
    myMap(itdesc.Key()) = newdesc;
  }
  
  if (theSubs.IsCopied(myRes)) {
    myRes = theSubs.Copy(myRes).First();
  }

  myDLeft.Clear();
  myLeft.Clear();
  mapV.Clear();

  TopTools_MapIteratorOfMapOfShape itms;

  for (exp.Init(myRes, TopAbs_FACE); exp.More(); exp.Next()) {
    const TopoDS_Face& fac = TopoDS::Face(exp.Current());
    for (exp2.Init(fac,TopAbs_EDGE); exp2.More(); exp2.Next()) {
      const TopoDS_Edge& edg = TopoDS::Edge(exp2.Current());
      for (itms.Initialize(mapE); 
	   itms.More(); itms.Next()) {
	if (itms.Key().IsSame(edg) &&
	    edg.Orientation() == itms.Key().Orientation()) {
	  break;
	}
      }
      if (itms.More()) {
	break;
      }
    }
    if (exp2.More()) {
      myDLeft.Append(fac);
      myLeft.Append(fac);
    }
    else {
      mapV.Add(fac);
    }
  }

/* JAG : Ne peut pas marcher

  Standard_Boolean full = mapV.IsEmpty();
  while (!full) {
    full = Standard_True;
    itms.Initialize(mapV);
    const TopoDS_Face& fac = TopoDS::Face(itms.Key());
    for (exp.Init(fac,TopAbs_EDGE); exp.More(); exp.Next()) {
      if (!mapE.Contains(exp.Current())) {
	for (itl.Initialize(myLeft); itl.More(); itl.Next()) {
	  const TopoDS_Face& fac2 = TopoDS::Face(itl.Value());
	  for (exp2.Init(fac2,TopAbs_EDGE); exp2.More(); exp2.Next()) {
	    if (exp2.Current().IsSame(exp.Current())) {
	      myLeft.Append(fac);
	      mapV.Remove(fac);
	      full = mapV.IsEmpty();
	      break;
	    }
	  }
	  if (exp2.More()) {
	    break;
	  }
	}
	if (itl.More()) {
	  break;
	}
      }
    }
  }
*/

  // Map des edges ou les connexions sont possibles
  TopTools_MapOfShape Mapebord;
  for (itl.Initialize(myLeft); itl.More(); itl.Next()) {
    for (exp.Init(itl.Value(),TopAbs_EDGE); exp.More(); exp.Next()) {
      if (!mapE.Contains(exp.Current())) {
	if (!Mapebord.Add(exp.Current())) {
	  Mapebord.Remove(exp.Current());
	}
      }
    }
  }


  while (Mapebord.Extent() != 0) {
    itms.Initialize(Mapebord);
    TopoDS_Shape edg = itms.Key();

    for (itms.Initialize(mapV); itms.More(); itms.Next()) {
      const TopoDS_Shape& fac = itms.Key();
      for (exp.Init(fac,TopAbs_EDGE); exp.More(); exp.Next()) {
	if (exp.Current().IsSame(edg)) {
	  break;
	}
      }
      if (exp.More()) {
	break; // face a gauche
      }
    }
    if (itms.More()) {
      TopoDS_Shape fac = itms.Key();
      for (exp.Init(fac,TopAbs_EDGE); exp.More(); exp.Next()) {
	if (!Mapebord.Add(exp.Current())) {
	  Mapebord.Remove(exp.Current());
	}
      }
      mapV.Remove(fac);
      myLeft.Append(fac);
    }
    else {
      Mapebord.Remove(edg);
    }
  }

  myDone = Standard_True;
}


//=======================================================================
//function : DescendantShapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& LocOpe_Spliter::
   DescendantShapes(const TopoDS_Shape& F)
{
  if (!myDone) {StdFail_NotDone::Raise();}
  if (myMap.IsBound(F))
    return myMap(F);
  else {
    static TopTools_ListOfShape empty;
    return empty;
  }
}


//=======================================================================
//function : DirectLeft
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& LocOpe_Spliter::DirectLeft() const
{
  if (!myDone) {StdFail_NotDone::Raise();}
  return myDLeft;

}


//=======================================================================
//function : Left
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& LocOpe_Spliter::Left() const
{
  if (!myDone) {StdFail_NotDone::Raise();}
  return myLeft;

}


//=======================================================================
//function : RebuildWires
//purpose  : 
//=======================================================================

//  Modified by skv - Mon May 31 12:31:39 2004 OCC5865 Begin
//static void RebuildWires(TopTools_ListOfShape& ledge)
static void RebuildWires(TopTools_ListOfShape& ledge,
			 const Handle(LocOpe_ProjectedWires)& PW)
{
  LocOpe_BuildWires theBuild(ledge, PW);
//  Modified by skv - Mon May 31 12:31:40 2004 OCC5865 End
  if (!theBuild.IsDone()) {
    Standard_ConstructionError::Raise();
  }
  ledge = theBuild.Result();


}



//=======================================================================
//function : Put
//purpose  : 
//=======================================================================

static void Put(const TopoDS_Shape& S,
                TopTools_DataMapOfShapeListOfShape& theMap)
{
  if (theMap.IsBound(S)) {
    return;
  }
  TopTools_ListOfShape thelist;
  theMap.Bind(S, thelist);
  theMap(S).Append(S);
  for (TopoDS_Iterator it(S); it.More(); it.Next()) {
    Put(it.Value(),theMap);
  }
}


//=======================================================================
//function : Select
//purpose  : 
//=======================================================================

static void Select(const TopoDS_Edge& Ebase,
		   TopTools_ListOfShape& lsubs)
{

  // Choix d`un point

  Handle(Geom_Curve) C;
  TopLoc_Location Loc;
  Standard_Real f,l,dmin = RealLast();
  Standard_Integer i=0,imin = 0;

  C = BRep_Tool::Curve(Ebase,Loc,f,l);

  if (!Loc.IsIdentity()) {
    Handle(Geom_Geometry) GG = C->Transformed(Loc.Transformation());
    C = *((Handle(Geom_Curve)*)&GG);
  }
  gp_Pnt Pt(C->Value((f+l)/2.));

  GeomAPI_ProjectPointOnCurve proj;
//  for (TopTools_ListIteratorOfListOfShape itl(lsubs);
  TopTools_ListIteratorOfListOfShape itl(lsubs);
  for ( ;itl.More();itl.Next()) {
    i++;
    const TopoDS_Edge& edg = TopoDS::Edge(itl.Value());
    C = BRep_Tool::Curve(edg,Loc,f,l);
    if (!Loc.IsIdentity()) {
      Handle(Geom_Geometry) GG = C->Transformed(Loc.Transformation());
      C = *((Handle(Geom_Curve)*)&GG);
    }
    proj.Init(Pt,C,f,l);
    if (proj.NbPoints() > 0) {
      if (proj.LowerDistance() < dmin) {
	imin = i;
	dmin = proj.LowerDistance();
      }
    }
  }
  if (imin == 0) {
    lsubs.Clear();
  }
  else {
    itl.Initialize(lsubs);
    i = 1;
    while (i < imin) {
      lsubs.Remove(itl);
      i++;
    }
    itl.Next();
    while (itl.More()) {
      lsubs.Remove(itl);
    }
  }
}
