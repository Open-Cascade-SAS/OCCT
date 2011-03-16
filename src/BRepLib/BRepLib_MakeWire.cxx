// File:	BRepLib_MakeWire.cxx
// Created:	Fri Jul 23 15:51:57 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepLib_MakeWire.ixx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <gp_Pnt.hxx>
#include <Geom_Curve.hxx>
#include <gp.hxx>


//=======================================================================
//function : BRepLib_MakeWire
//purpose  : 
//=======================================================================

BRepLib_MakeWire::BRepLib_MakeWire() :
     myError(BRepLib_EmptyWire)
{
}


//=======================================================================
//function : BRepLib_MakeWire
//purpose  : 
//=======================================================================

BRepLib_MakeWire::BRepLib_MakeWire(const TopoDS_Edge& E)
{
  Add(E);
}


//=======================================================================
//function : BRepLib_MakeWire
//purpose  : 
//=======================================================================

BRepLib_MakeWire::BRepLib_MakeWire(const TopoDS_Edge& E1, 
				   const TopoDS_Edge& E2)
{
  Add(E1);
  Add(E2);
}


//=======================================================================
//function : BRepLib_MakeWire
//purpose  : 
//=======================================================================

BRepLib_MakeWire::BRepLib_MakeWire(const TopoDS_Edge& E1,
				   const TopoDS_Edge& E2, 
				   const TopoDS_Edge& E3)
{
  Add(E1);
  Add(E2);
  Add(E3);
}


//=======================================================================
//function : BRepLib_MakeWire
//purpose  : 
//=======================================================================

BRepLib_MakeWire::BRepLib_MakeWire(const TopoDS_Edge& E1, 
				   const TopoDS_Edge& E2,
				   const TopoDS_Edge& E3, 
				   const TopoDS_Edge& E4)
{
  Add(E1);
  Add(E2);
  Add(E3);
  Add(E4);
}


//=======================================================================
//function : BRepLib_MakeWire
//purpose  : 
//=======================================================================

BRepLib_MakeWire::BRepLib_MakeWire(const TopoDS_Wire& W)
{
  Add(W);
}


//=======================================================================
//function : BRepLib_MakeWire
//purpose  : 
//=======================================================================

BRepLib_MakeWire::BRepLib_MakeWire(const TopoDS_Wire& W, 
				   const TopoDS_Edge& E)
{
  Add(W);
  Add(E);
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepLib_MakeWire::Add(const TopoDS_Wire& W)
{
  TopExp_Explorer ex(W,TopAbs_EDGE);
  while (ex.More()) {
    Add(TopoDS::Edge(ex.Current()));
    ex.Next();
  }
}

//=======================================================================
//function : Add
//purpose  : 
// PMN  19/03/1998  Pour des Probleme de performances on n'utilise pas
//                  TopExp::Vertices sur des wire
// PMN  10/09/1998  Dans le cas ou le wire est precedament ferme (ou degenere) 
//                  on emploie quand meme TopExp::Vertices ... Afin de lever
//                  les ambiguites.
//=======================================================================

void  BRepLib_MakeWire::Add(const TopoDS_Edge& E)
{

  Standard_Boolean forward = Standard_False; 
     // pour dire si on decider d'ajouter forward
  Standard_Boolean reverse = Standard_False; 
     // pour dire si on decide d'ajouter reversed
  Standard_Boolean init = Standard_False;
     // Pour savoir s'il on doit calculer VL, VF
  BRep_Builder B;
  TopoDS_Iterator it;

  if (myEdge.IsNull()) {
    init = Standard_True;
    // first edge, create the wire
    B.MakeWire(TopoDS::Wire(myShape));

    // set the edge
    myEdge = E;

    // add the vertices
    for (it.Initialize(myEdge); it.More(); it.Next()) 
      myVertices.Add(it.Value());
  }
  
  else {
    init = myShape.Closed(); // Si c'est ferme, on ne controle
    TopoDS_Shape aLocalShape = E.Oriented(TopAbs_FORWARD);
    TopoDS_Edge EE = TopoDS::Edge(aLocalShape);
//    TopoDS_Edge EE = TopoDS::Edge(E.Oriented(TopAbs_FORWARD));

    // test the vertices of the edge
    
    Standard_Boolean connected = Standard_False;
    Standard_Boolean copyedge  = Standard_False;
    
    if (myError != BRepLib_NonManifoldWire) {
      if (VF.IsNull() || VL.IsNull()) 
	myError = BRepLib_NonManifoldWire;
    }

    for (it.Initialize(EE); it.More(); it.Next()) {

      const TopoDS_Vertex& VE = TopoDS::Vertex(it.Value());

      // if the vertex is in the wire, ok for the connection
      if (myVertices.Contains(VE)) {
	connected = Standard_True;
        myVertex = VE;
	if (myError != BRepLib_NonManifoldWire) {
	  // l est on toujours ?
	  if (VF.IsSame(VL)) {
	    // Orientation indetermine (en 3d) : On garde l'init
	    if (!VF.IsSame(VE)) myError = BRepLib_NonManifoldWire;
	  }
	  else {
	    if (VF.IsSame(VE)) {
	      if  (VE.Orientation() == TopAbs_FORWARD) 
		reverse = Standard_True;
	      else 
		forward = Standard_True;
	    }
	    else if (VL.IsSame(VE)) {
	      if (VE.Orientation() == TopAbs_REVERSED)
		reverse = Standard_True;
	      else 
		forward = Standard_True;
	    }
	    else 
	      myError = BRepLib_NonManifoldWire;
	  }
	}
      }
      else {
	  // search if there is a similar vertex in the edge	
	gp_Pnt PE = BRep_Tool::Pnt(VE);
	
//	Standard_Boolean newvertex = Standard_False;
	TopTools_MapIteratorOfMapOfShape itm(myVertices);
	while (itm.More()) {
	  
	  const TopoDS_Vertex& VW = TopoDS::Vertex(itm.Key());
	  gp_Pnt PW = BRep_Tool::Pnt(VW);
	  Standard_Real l = PE.Distance(PW);
	  
	  if ((l < BRep_Tool::Tolerance(VE)) ||
	      (l < BRep_Tool::Tolerance(VW))) {
	    copyedge = Standard_True;
	    if (myError != BRepLib_NonManifoldWire) {
	      // l est on toujours ?
	      if (VF.IsSame(VL)) {
		// Orientation indetermine (en 3d) : On garde l'init
		if (!VF.IsSame(VW)) myError = BRepLib_NonManifoldWire;
	      }
	      else {
		if (VF.IsSame(VW)) {
		 if  (VE.Orientation() == TopAbs_FORWARD) 
		   reverse = Standard_True;
		 else 
		   forward = Standard_True; 
		}
		else if (VL.IsSame(VW)) { 
		  if (VE.Orientation() == TopAbs_REVERSED)
		    reverse = Standard_True;
		  else 
		    forward = Standard_True;
		}
		else 
		  myError = BRepLib_NonManifoldWire;
	      }
	    }      
	    break;
	  }
	  itm.Next();
	}
	if (copyedge) {
	  connected = Standard_True;
	}
      }
    }
    
    if (!connected) {
      myError = BRepLib_DisconnectedWire;
      NotDone();
      return;
    }
    else {
      if (!copyedge) {
	myEdge = EE;
	for (it.Initialize(EE); it.More(); it.Next())
	  myVertices.Add(it.Value());
      }
      else {
	// copy the edge
	TopoDS_Shape Dummy = EE.EmptyCopied();
	myEdge = TopoDS::Edge(Dummy);
	myEdge.Closed(EE.Closed());
	
	for (it.Initialize(EE); it.More(); it.Next()) {

	  const TopoDS_Vertex& VE = TopoDS::Vertex(it.Value());
	  gp_Pnt PE = BRep_Tool::Pnt(VE);
	  
	  Standard_Boolean newvertex = Standard_False;
	  TopTools_MapIteratorOfMapOfShape itm(myVertices);
	  while (itm.More()) {
	  
	    const TopoDS_Vertex& VW = TopoDS::Vertex(itm.Key());
	    gp_Pnt PW = BRep_Tool::Pnt(VW);
	    Standard_Real l = PE.Distance(PW), tolE, tolW;
	    tolW = BRep_Tool::Tolerance(VW);
	    tolE = BRep_Tool::Tolerance(VE);
	    
	    if ((l < tolE) || (l < tolW)) {

	      Standard_Real maxtol = .5*(tolW + tolE + l), cW, cE;
	      if(maxtol > tolW && maxtol > tolE) {
		cW = (maxtol - tolE)/l; 
		cE = 1. - cW;
	      }
	      else if (maxtol > tolW) {maxtol = tolE; cW = 0.; cE = 1.;}
	      else {maxtol = tolW; cW = 1.; cE = 0.;}

	      gp_Pnt PC(cW*PW.X() + cE*PE.X(),cW*PW.Y() + cE*PE.Y(),cW*PW.Z() + cE*PE.Z());

	      B.UpdateVertex(VW, PC, maxtol);
	      
	      newvertex = Standard_True;
	      myVertex = VW;
	      myVertex.Orientation(VE.Orientation());
	      B.Add(myEdge,myVertex);
	      B.Transfert(EE,myEdge,VE,myVertex);
	      break;
	    }
	    
	    itm.Next();
	  }
	  if (!newvertex) {
	    myVertices.Add(VE);
	    B.Add(myEdge,VE);
	    B.Transfert(EE,myEdge,VE,VE);
	  }
	}
      }
    }
    // On decide ici de l'orientation de l'arete
    // S'il y a ambiguite (en 3d) on garde l'orientation donnee en entree
    // Cas d'ambiguite :
    // reverse et forward sont faux car on n'a rien decider : 
    //       wire ferme, vertex interne ... 
    // reverse et forward sont vrai : Edge ferme ou degenere
    if ( ((forward == reverse) && (E.Orientation() == TopAbs_REVERSED)) ||
       ( reverse && !forward) )  myEdge.Reverse();
  }
 
  // add myEdge to myShape
  B.Add(myShape,myEdge);
  myShape.Closed(Standard_False);

  // Initialize VF, VL
  if (init) TopExp::Vertices(TopoDS::Wire(myShape), VF,VL);
  else {
    if (myError == BRepLib_WireDone){ // Update only
      TopoDS_Vertex V1,V2,VRef; 
      TopExp::Vertices(myEdge, V1, V2);
      if (V1.IsSame(myVertex)) VRef = V2;
      else if (V2.IsSame(myVertex)) VRef = V1;
      else {
#if DEB
	cout << "MakeWire : Y A UN PROBLEME !!" << endl;
#endif
	myError = BRepLib_NonManifoldWire;
      }
      
      if (VF.IsSame(VL)) {
	// Cas particulier: il faut controler les orientations
#if DEB
	if (!VF.IsSame(myVertex))
	  cout << "MakeWire : Y A UN PROBLEME !!" << endl;
#endif
	
      }
      else { // Cas general
	if (VF.IsSame(myVertex)) VF = VRef;
	else if (VL.IsSame(myVertex)) VL = VRef;
	else {
#if DEB
	  cout << "MakeWire : Y A UN PROBLEME !!" << endl;
#endif
	  myError = BRepLib_NonManifoldWire;
	}
      }
    }
    if (myError == BRepLib_NonManifoldWire) {
      VF = VL = TopoDS_Vertex(); // nullify
    }
  }
  // Test myShape is closed
  if (!VF.IsNull() && !VL.IsNull() && VF.IsSame(VL))
    myShape.Closed(Standard_True);
  
  myError = BRepLib_WireDone;
  Done();
}


//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================

const TopoDS_Wire&  BRepLib_MakeWire::Wire()const 
{
  return TopoDS::Wire(Shape());
}


//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge&  BRepLib_MakeWire::Edge()const 
{
  return myEdge;
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepLib_MakeWire::Vertex()const 
{
  return myVertex;
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakeWire::operator TopoDS_Wire() const
{
  return Wire();
}



//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepLib_WireError BRepLib_MakeWire::Error() const
{
  return myError;
}
