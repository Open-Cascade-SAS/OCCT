// File:	BRepLib_MakeWire_1.cxx
// Created:	Wed May 20 07:30:03 1998
// Author:	Didier PIFFAULT
//		<dpf@motox.paris1.matra-dtv.fr>


#include <BRepLib_MakeWire.ixx>
#include <BRepLib.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfOrientedShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BRep_Builder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>


//=======================================================================
//function : Add
//purpose  : Add the list of edges to the current wire
//=======================================================================

void  BRepLib_MakeWire::Add(const TopTools_ListOfShape& L)
{
  myError = BRepLib_WireDone;
  if (!myShape.IsNull()) myShape.Closed(Standard_False);

  if (!L.IsEmpty()) {
    NotDone();
    TopTools_MapOfShape mapLocale;
    mapLocale.Assign(myVertices);
    TopTools_DataMapOfShapeShape toCopy;
    TopTools_ListOfShape toAdd, nlist, rlist;
    BRep_Builder BB;

    TopExp_Explorer exv;
    TopTools_MapIteratorOfMapOfShape itMS;
    TopTools_ListIteratorOfListOfShape itList(L);
    for (;itList.More(); itList.Next()) {
      const TopoDS_Edge& curEd=TopoDS::Edge(itList.Value());
      if (!curEd.IsNull()) {
	rlist.Clear();
	nlist.Clear();
	Standard_Boolean copEd=Standard_False;
	if (myEdge.IsNull()) {
	  Add(curEd);
	  if (!VF.IsNull()) mapLocale.Add(VF);
	  if (!VL.IsNull()) mapLocale.Add(VL);
	  NotDone();
	  continue;
	}
	for (exv.Init(curEd, TopAbs_VERTEX); exv.More(); exv.Next()) {
	  const TopoDS_Vertex& edVer=TopoDS::Vertex(exv.Current());
	  rlist.Prepend(edVer);
	  nlist.Prepend(edVer);
	  if (!mapLocale.Contains(edVer)) {
#ifndef DEB
	    Standard_Boolean notYetFound = Standard_True;
#else
	    Standard_Boolean notYetFound;
#endif
	    Standard_Real gap=BRep_Tool::Tolerance(edVer);
	    gp_Pnt pVer=BRep_Tool::Pnt(edVer);
	    for (itMS.Initialize(mapLocale); itMS.More(); itMS.Next()) {
	      notYetFound=Standard_True;
	      const TopoDS_Vertex& refVer=TopoDS::Vertex(itMS.Key());
	      gap +=BRep_Tool::Tolerance(refVer);
	      if (pVer.Distance(BRep_Tool::Pnt(TopoDS::Vertex(refVer))) <= gap) {
		nlist.RemoveFirst();
		nlist.Prepend(refVer.Oriented(edVer.Orientation()));
		copEd=Standard_True;
		notYetFound=Standard_False;
		break;
	      }
	    }
	    if (notYetFound) mapLocale.Add(edVer);
	  }
	}
	if (copEd) {
	  TopoDS_Shape aLocalShape = curEd.EmptyCopied();
	  TopoDS_Edge newEd=TopoDS::Edge(aLocalShape);
//	  TopoDS_Edge newEd=TopoDS::Edge(curEd.EmptyCopied());
	  BB.Transfert(curEd, newEd);
	  newEd.Closed(curEd.Closed());
	  TopTools_ListIteratorOfListOfShape itV(nlist);
	  for (; itV.More(); itV.Next()) {
	    BB.Add(newEd, itV.Value());
	    BB.Transfert(curEd, newEd, TopoDS::Vertex(rlist.First()), TopoDS::Vertex(itV.Value()));
	    rlist.RemoveFirst();
	  }
	  toAdd.Append(newEd);
	}
	else {
	  toAdd.Append(curEd);
	}
      }
    }
    if (!toAdd.IsEmpty()) {
      TopoDS_Compound comp;
      BB.MakeCompound(comp);
      TopTools_MapIteratorOfMapOfOrientedShape itMOS;
      TopTools_MapOfOrientedShape theEdges;
      for (itList.Initialize(toAdd); itList.More(); itList.Next()) {
	BB.Add(comp, itList.Value());
	theEdges.Add(itList.Value());
      }
      TopTools_IndexedDataMapOfShapeListOfShape lesMeres;
      TopExp::MapShapesAndAncestors(comp, TopAbs_VERTEX, TopAbs_EDGE, lesMeres);
      TopoDS_Vertex vf, vl;
      TopoDS_Shape theKey;
      Standard_Boolean usedVertex;
#ifndef DEB
      Standard_Boolean closedEdge = Standard_False;
#else
      Standard_Boolean closedEdge;
#endif
      Standard_Integer vvInd, lastInd;
      do {
	if (!VL.IsNull() && lesMeres.Contains(VL)) {
	  if (!VF.IsNull()) closedEdge=VF.IsSame(VL);
	  usedVertex=Standard_True;
	  for (itList.Initialize(lesMeres.FindFromKey(VL)); itList.More(); itList.Next()) {
	    if (theEdges.Contains(itList.Value())) {
	      usedVertex=Standard_False;
	      theEdges.Remove(itList.Value());
	      TopExp::Vertices(TopoDS::Edge(itList.Value()), vf,vl);
	      if (vf.IsSame(VL)) {
		BB.Add(myShape, itList.Value());
		myVertices.Add(vl);
		VL=vl;
	      }
	      else {
		if (closedEdge) {
		  BB.Add(myShape, itList.Value());
		  VF=vf;
		}
		else {
		  BB.Add(myShape, itList.Value().Reversed());
		  vf.Reverse();
		  VL=vf;
		}
		myVertices.Add(vf);
	      }
	    }
	  }
	  if (usedVertex) {
	    lastInd=lesMeres.Extent();
	    vvInd=lesMeres.FindIndex(VL);
	    if (vvInd != lastInd) {
	      theKey=lesMeres.FindKey(lastInd);
	      nlist=lesMeres.FindFromIndex(lastInd);
	    }
	    lesMeres.RemoveLast();
	    if (vvInd != lastInd) {
	      lesMeres.Substitute(vvInd, theKey, nlist);
	    }
	  }
	}
	else if (!VF.IsNull() && lesMeres.Contains(VF)) {
	  usedVertex=Standard_True;
	  for (itList.Initialize(lesMeres.FindFromKey(VF)); itList.More(); itList.Next()) {
	    if (theEdges.Contains(itList.Value())) {
	      usedVertex=Standard_False;
	      theEdges.Remove(itList.Value());
	      TopExp::Vertices(TopoDS::Edge(itList.Value()), vf,vl);
	      if (vl.IsSame(VF)) {
		BB.Add(myShape, itList.Value());
		myVertices.Add(vf);
		VF=vf;
	      }
	      else {
		BB.Add(myShape, itList.Value().Reversed());
		vl.Reverse();
		myVertices.Add(vl);
		VF=vl;
	      }
	    }
	  }
	  if (usedVertex) {
	    lastInd=lesMeres.Extent();
	    vvInd=lesMeres.FindIndex(VF);
	    if (vvInd != lastInd) {
	      theKey=lesMeres.FindKey(lastInd);
	      nlist=lesMeres.FindFromIndex(lastInd);
	    }
	    lesMeres.RemoveLast();
	    if (vvInd != lastInd) {
	      lesMeres.Substitute(vvInd, theKey, nlist);
	    }
	  }
	}
	else {
	  if (theEdges.Extent()>0) {
	    Standard_Boolean noCandidat=Standard_True;
	    for (itMOS.Initialize(theEdges); itMOS.More(); itMOS.Next()) {
	      TopExp::Vertices(TopoDS::Edge(itMOS.Key()), vf,vl);
	      if (myVertices.Contains(vl)) {
		if (myError==BRepLib_WireDone)  myError = BRepLib_NonManifoldWire;
		BB.Add(myShape, itMOS.Key());
		myVertices.Add(vf);
		VF=vf;
		noCandidat=Standard_False;
		break;
	      }
	      else if (myVertices.Contains(vf)) {
		if (myError==BRepLib_WireDone)  myError = BRepLib_NonManifoldWire;
		BB.Add(myShape, itMOS.Key());
		myVertices.Add(vl);
		VL=vl;
		noCandidat=Standard_False;
		break;
	      }
	    }
	    if (noCandidat) {
	      theEdges.Clear();
// Some Edges are not connected to first edge and the diagnosis is as follows
// but the le "Maker" is Done() because otherwise it is not possible to return the constructed connected part...
	      myError=BRepLib_DisconnectedWire;
	    }
	    else theEdges.Remove(itMOS.Key());
	  }
	}
      } while (theEdges.Extent()>0);
    }
  }

  if (!VF.IsNull() && !VL.IsNull() && VF.IsSame(VL))
    myShape.Closed(Standard_True);  
  Done();
}
