// Created on: 1992-08-27
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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


#ifndef No_Exception
// #define No_Exception
#endif
#include <HLRBRep_ShapeToHLR.ixx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <HLRBRep.hxx>
#include <BRepTopAdaptor_MapOfShapeTool.hxx>

//=======================================================================
// Function : Load
// Purpose  : 
//=======================================================================

Handle(HLRBRep_Data) 
HLRBRep_ShapeToHLR::Load(const Handle(HLRTopoBRep_OutLiner)& S,
			 const HLRAlgo_Projector& P,
			 BRepTopAdaptor_MapOfShapeTool& MST,
			 const Standard_Integer nbIso)
{ 
  S->Fill(P,MST,nbIso);

  HLRTopoBRep_Data& TopDS = S->DataStructure();
  TopTools_IndexedMapOfShape                FM;
  TopTools_IndexedMapOfShape                EM;
  TopTools_IndexedDataMapOfShapeListOfShape VerticesToEdges;
  TopTools_IndexedDataMapOfShapeListOfShape EdgesToFaces;

  TopExp_Explorer exshell,exface;

  for (exshell.Init(S->OutLinedShape(), TopAbs_SHELL);
       exshell.More(); 
       exshell.Next()) {                             // faces in a shell
    
    for (exface.Init(exshell.Current(), TopAbs_FACE);
	 exface.More(); 
	 exface.Next()) {
      if (!FM.Contains(exface.Current()))
	FM.Add(exface.Current());
    }
  }

  for (exface.Init(S->OutLinedShape(), TopAbs_FACE, TopAbs_SHELL);
       exface.More(); 
       exface.Next()) {                           // faces not in a shell
    if (!FM.Contains(exface.Current()))
      FM.Add(exface.Current());
  }
  
  TopExp::MapShapes(S->OutLinedShape(),TopAbs_EDGE,EM);

  Standard_Integer i;
  Standard_Integer nbEdge = EM.Extent ();

  for (i = 1; i <= nbEdge; i++)                 // vertices back to edges
    TopExp::MapShapesAndAncestors
      (EM(i), TopAbs_VERTEX, TopAbs_EDGE, VerticesToEdges);
  
  Standard_Integer nbVert = VerticesToEdges.Extent();
  Standard_Integer nbFace = FM.Extent();

  TopoDS_Vertex VF, VL;
  TopTools_ListIteratorOfListOfShape itn;
  Standard_Integer   i1, i2;
  Standard_Boolean   o1, o2;
  Standard_Boolean   c1, c2;
  Standard_Real      pf, pl;
  Standard_ShortReal tf, tl;

  // Create the data structure
  Handle(HLRBRep_Data) DS = new HLRBRep_Data (nbVert, nbEdge, nbFace);
  HLRBRep_EdgeData* ed; 
  if(nbEdge != 0) ed = &(DS->EDataArray().ChangeValue(1));
//  ed++;

  for (i = 1; i <= nbFace; i++) { // test of Double edges
    TopExp::MapShapesAndAncestors
      (FM(i),TopAbs_EDGE, TopAbs_FACE, EdgesToFaces);
  }

  for (i = 1; i <= nbEdge; i++) {  // load the Edges
    const TopoDS_Edge& Edg = TopoDS::Edge (EM(i));
    TopExp::Vertices (Edg, VF, VL);
    BRep_Tool::Range (Edg, pf, pl);
    Standard_Boolean reg1 = Standard_False;
    Standard_Boolean regn = Standard_False;
    Standard_Integer inde = EdgesToFaces.FindIndex(Edg);
    if (inde > 0) {
      if (EdgesToFaces(inde).Extent() == 2) {
	itn = EdgesToFaces(inde);
	const TopoDS_Face& F1 = TopoDS::Face(itn.Value());
	itn.Next();
	const TopoDS_Face& F2 = TopoDS::Face(itn.Value());
	GeomAbs_Shape rg = BRep_Tool::Continuity(Edg,F1,F2);
	reg1 = rg >= GeomAbs_G1;
        regn = rg >= GeomAbs_G2; 
      }
    }
      
    if (VF.IsNull()) {
      i1 = 0;
      o1 = Standard_False;
      c1 = Standard_False;
      pf = RealFirst();
      tf = (Standard_ShortReal) Epsilon(pf);
    }
    else {
      i1 = VerticesToEdges.FindIndex(VF);
      o1 = TopDS.IsOutV(VF);
      c1 = TopDS.IsIntV(VF);
      tf = (Standard_ShortReal) BRep_Tool::Tolerance(VF);
    }
    
    if (VL.IsNull()) {
      i2 = 0;
      o2 = Standard_False;
      c2 = Standard_False;
      pl = RealLast();
      tl = (Standard_ShortReal) Epsilon (pl);
    }
    else {
      i2 = VerticesToEdges.FindIndex(VL);
      o2 = TopDS.IsOutV(VL);
      c2 = TopDS.IsIntV(VL);
      tl = (Standard_ShortReal) BRep_Tool::Tolerance(VL);
    }
    
    ed->Set (reg1,regn, Edg, i1, i2, o1, o2, c1, c2, pf, tf, pl, tl);
    DS->EdgeMap().Add(Edg);
    ed++;
  }
  
  ExploreShape(S,DS,FM,EM);
  return DS;
}

//=======================================================================
// Function : ExploreFace
// Purpose  : 
//=======================================================================

void
HLRBRep_ShapeToHLR::ExploreFace(const Handle(HLRTopoBRep_OutLiner)& S,
				const Handle(HLRBRep_Data)& DS,
				const TopTools_IndexedMapOfShape& FM,
				const TopTools_IndexedMapOfShape& EM,
				Standard_Integer& i,
				const TopoDS_Face& F,
				const Standard_Boolean closed)
{
  i++;
  TopExp_Explorer Ex1,Ex2;
  HLRTopoBRep_Data& TopDS = S->DataStructure();
  TopAbs_Orientation orient = FM(i).Orientation();
  TopoDS_Face theFace   = TopoDS::Face(FM(i));
  theFace.Orientation (TopAbs_FORWARD);
  HLRBRep_FaceData& fd = DS->FDataArray().ChangeValue(i);
  
  Standard_Integer nw = 0;
  
  for (Ex1.Init(theFace, TopAbs_WIRE); Ex1.More(); Ex1.Next())
    nw++;

  fd.Set (theFace, orient, closed, nw);
  nw = 0;
     
  for (Ex1.Init(theFace, TopAbs_WIRE); Ex1.More(); Ex1.Next()) {
    nw++;
    Standard_Integer ne = 0;
    
    for (Ex2.Init(Ex1.Current(), TopAbs_EDGE); Ex2.More(); Ex2.Next())
      ne++;
    
    fd.SetWire (nw, ne);
    ne = 0;
    
    for (Ex2.Init(Ex1.Current(), TopAbs_EDGE);
	 Ex2.More();
	 Ex2.Next()) {
      ne++;
      const TopoDS_Edge& E = TopoDS::Edge(Ex2.Current());
      Standard_Integer ie   = EM.FindIndex(E);
      TopAbs_Orientation orient = E.Orientation();
      Standard_Boolean Int  = TopDS.IsIntLFaceEdge(F,E);
      Standard_Boolean Iso  = TopDS.IsIsoLFaceEdge(F,E);
      Standard_Boolean Out  = TopDS.IsOutLFaceEdge(F,E);
      Standard_Boolean Dbl  = BRepTools::IsReallyClosed(TopoDS::Edge(E),theFace);
      fd.SetWEdge(nw, ne, ie, orient, Out, Int, Dbl, Iso);
    }
  }
  DS->FaceMap().Add(theFace);
}

//=======================================================================
//function : ExploreShape
//purpose  : 
//=======================================================================

void
HLRBRep_ShapeToHLR::ExploreShape (const Handle(HLRTopoBRep_OutLiner)& S,
				  const Handle(HLRBRep_Data)& DS,
				  const TopTools_IndexedMapOfShape& FM,
				  const TopTools_IndexedMapOfShape& EM)
{
  TopTools_MapOfShape ShapeMap;
  TopExp_Explorer exshell, exface, exedge;
  Standard_Integer i = 0;

  for (exshell.Init (S->OriginalShape(), TopAbs_SHELL);
       exshell.More (); 
       exshell.Next ()) {             // faces in a shell (open or close)

    Standard_Boolean closed = exshell.Current().Closed();

    if (!closed) {
      Standard_Integer ie;
      Standard_Integer nbEdge = EM.Extent ();
      Standard_Integer *flag = new Standard_Integer[nbEdge + 1];

      for(ie = 1; ie<=nbEdge; ie++)
	flag[ie] = 0;
      
      for (exedge.Init(exshell.Current(), TopAbs_EDGE);
	   exedge.More(); 
	   exedge.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge(exedge.Current());
	ie = EM.FindIndex(E);
	TopAbs_Orientation orient = E.Orientation();
	if (!BRep_Tool::Degenerated(E)) {
	  if      (orient == TopAbs_FORWARD ) flag[ie] += 1;
	  else if (orient == TopAbs_REVERSED) flag[ie] -= 1;
	}
      }
      closed = Standard_True;

      for (ie = 1; ie <= nbEdge && closed; ie++)
	closed = (flag[ie] == 0); 
      delete [] flag;
      flag = NULL;
    }
      
    for (exface.Init(exshell.Current(), TopAbs_FACE);
	 exface.More(); 
	 exface.Next()) {
      if (ShapeMap.Add(exface.Current())) {
	ExploreFace(S,DS,FM,EM,i,
		    TopoDS::Face(exface.Current()),
		    closed);
      }
    }
  }
  
  for (exface.Init(S->OriginalShape(), TopAbs_FACE, TopAbs_SHELL);
       exface.More(); 
       exface.Next()) {                           // faces not in a shell
    if (ShapeMap.Add(exface.Current())) {
      ExploreFace(S,DS,FM,EM,i,
		  TopoDS::Face(exface.Current()),
		  Standard_False);
    }
  }
}
