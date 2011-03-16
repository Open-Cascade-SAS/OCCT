// File:	BRepFeat_MakeLinearForm.cxx
// Created:	Mon Apr 14 13:34:46 1997
// Author:	Olga KOULECHOVA
//		<opt@langdox.paris1.matra-dtv.fr>


#include <BRepFeat_MakeLinearForm.ixx>

#include <BRepFeat.hxx>

#include <LocOpe.hxx>
#include <LocOpe_Builder.hxx>
#include <LocOpe_LinearForm.hxx>
#include <LocOpe_Gluer.hxx>
#include <LocOpe_FindEdges.hxx>

#include <gp_Vec.hxx>
#include <gp_Pln.hxx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gp_Vec2d.hxx>
#include <gp_Pnt2d.hxx>

#include <Geom_Curve.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

#include <Geom_TrimmedCurve.hxx>
#include <GeomProjLib.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_Curve.hxx>

#include <TColgp_SequenceOfPnt.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <IntRes2d_IntersectionPoint.hxx>

#include <BRepTools_WireExplorer.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>

#include <TColgp_Array1OfPnt.hxx>

#include <TColGeom_Array1OfCurve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>

#include <BRepTools.hxx>

#include <BRepBuilderAPI.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

//modified by NIZNHY-PKV Fri Mar 22 16:51:33 2002 f
//#include <BRepAlgo_Section.hxx>
//#include <BRepAlgo_Common.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAlgoAPI_Common.hxx>
//modified by NIZNHY-PKV Fri Mar 22 16:51:35 2002 t

#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepExtrema_ExtCF.hxx>

#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>

#include <Standard_ConstructionError.hxx>

#include <Precision.hxx>

#include <GeomLProp_CLProps.hxx>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepLib_MakeWire.hxx>
//#include <DbgTools.hxx>

#ifdef DEB
Standard_IMPORT Standard_Boolean BRepFeat_GettraceFEAT();
Standard_IMPORT Standard_Boolean BRepFeat_GettraceFEATRIB();
#endif

static void MajMap(const TopoDS_Shape&, // base
		   const LocOpe_LinearForm&,
		   TopTools_DataMapOfShapeListOfShape&, // myMap
		   TopoDS_Shape&,  // myFShape
		   TopoDS_Shape&); // myLShape

static void SetGluedFaces(const TopTools_DataMapOfShapeListOfShape& theSlmap,
			  LocOpe_LinearForm&,
			  TopTools_DataMapOfShapeShape&);

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepFeat_MakeLinearForm::Init(const TopoDS_Shape& Sbase,
				   const TopoDS_Wire& W,
				   const Handle(Geom_Plane)& Plane,
				   const gp_Vec& Direc,
				   const gp_Vec& Direc1,
				   const Standard_Integer Mode,
				   const Standard_Boolean Modify)
{
#ifdef DEB
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeLinearForm::Init" << endl;
#endif
  Standard_Boolean RevolRib = Standard_False;
  Done();
  myGenerated.Clear();
  
// modify = 0 si on ne veut pas faire de glissement
//        = 1 si on veut essayer de faire un glissement
  Standard_Boolean Sliding = Modify;
  myLFMap.Clear();

  myShape.Nullify();
  myMap.Clear();
  myFShape.Nullify();
  myLShape.Nullify();
  mySbase  = Sbase;
  mySkface.Nullify();
  myPbase.Nullify();

  myGShape.Nullify();
  mySUntil.Nullify();
  myListOfEdges.Clear();
  mySlface.Clear();

  TopoDS_Shape aLocalShape = W.Oriented(TopAbs_FORWARD);
  myWire = TopoDS::Wire(aLocalShape);
//  myWire = TopoDS::Wire(W.Oriented(TopAbs_FORWARD));
  myDir  = Direc;
  myDir1 = Direc1;
  myPln  = Plane;

  if(Mode == 0) 
    myFuse   = Standard_False;
  else // if(Mode == 1) 
    myFuse   = Standard_True;
#ifdef DEB
  if (trc) {
    if (myFuse)  cout << " Fuse" << endl;
    if (!myFuse)  cout << " Cut" << endl;
  }
#endif

  
// ---Determination Tolerance : tolerance max sur les parametres
  myTol = Precision::Confusion();

  TopExp_Explorer exx;  
  exx.Init(myWire, TopAbs_VERTEX);
  for(; exx.More(); exx.Next()) {
    const Standard_Real& tol = BRep_Tool::
      Tolerance(TopoDS::Vertex(exx.Current()));
    if(tol > myTol) myTol = tol;
  }

  exx.Init(Sbase, TopAbs_VERTEX);
  for(; exx.More(); exx.Next()) {
    const Standard_Real& tol = BRep_Tool::
      Tolerance(TopoDS::Vertex(exx.Current()));
    if(tol > myTol) myTol = tol;
  }

// ---Controle des directions
//    le wire doit etre dans la nervure
  gp_Vec nulldir(0, 0, 0);
  if(!myDir1.IsEqual(nulldir, myTol, myTol)) {
    Standard_Real ang = myDir1.Angle(myDir);
    if(ang != PI) {
#ifdef DEB
      if (trc) cout << " Directions must be opposite" << endl;
#endif
      myStatusError = BRepFeat_BadDirect;
      NotDone();
      return;
    }
  }
  else {

// Rib is centre in the middle of translation
#ifdef DEB
    if (trc)  cout << " Rib is centre" << endl;
#endif
    const gp_Vec& DirTranslation = (Direc + Direc1) * 0.5;
    gp_Trsf T;
    T.SetTranslation(DirTranslation);
    BRepBuilderAPI_Transform trf(T);
    trf.Perform(myWire);
    myWire = TopoDS::Wire(trf.Shape());
    myDir  = Direc  - DirTranslation;
    myDir1 = Direc1 - DirTranslation;
    myPln->Transform(T);
  }

// ---Calcul boite englobante
  BRep_Builder BB;

  TopTools_ListOfShape theList;  
  
  TopoDS_Shape U;
  U.Nullify();
  gp_Pnt FirstCorner, LastCorner;
  Standard_Real bnd = HeightMax(mySbase, U, FirstCorner, LastCorner);
  myBnd = bnd;

  BRepPrimAPI_MakeBox Bndbox(FirstCorner, LastCorner);
  TopoDS_Solid BndBox = Bndbox.Solid();


// ---Construction de la face plan de travail (section boite englobante)
  BRepLib_MakeFace PlaneF(myPln->Pln(), -6.*myBnd, 
			  6.*myBnd, -6.*myBnd, 6.*myBnd);
  TopoDS_Face PlaneFace = TopoDS::Face(PlaneF.Shape());

  //modified by NIZNHY-PKV Fri Mar 22 16:49:28 2002 f
  //BRepAlgo_Common PlaneS(BndBox, PlaneFace);
  BRepAlgoAPI_Common PlaneS(BndBox, PlaneFace);
  //modified by NIZNHY-PKV Fri Mar 22 16:49:39 2002 t  
  TopExp_Explorer EXP;
  TopoDS_Shape PlaneSect = PlaneS.Shape();
  EXP.Init(PlaneSect, TopAbs_WIRE);
  TopoDS_Wire www = TopoDS::Wire(EXP.Current());
  BRepLib_MakeFace Bndface(myPln->Pln(), www, Standard_True);
  TopoDS_Face BndFace = TopoDS::Face(Bndface.Shape());


// ---Recherche des faces d'appui de la nervure
  TopoDS_Edge FirstEdge, LastEdge;
  TopoDS_Face FirstFace, LastFace;
  TopoDS_Vertex FirstVertex, LastVertex;

  Standard_Boolean OnFirstFace = Standard_False;
  Standard_Boolean OnLastFace = Standard_False;
  Standard_Boolean PtOnFirstEdge = Standard_False;
  Standard_Boolean PtOnLastEdge = Standard_False;
  TopoDS_Edge OnFirstEdge, OnLastEdge;
  OnFirstEdge.Nullify();
  OnLastEdge.Nullify();

  Standard_Boolean Data = ExtremeFaces(RevolRib, myBnd, myPln, FirstEdge, LastEdge, 
				       FirstFace, LastFace, FirstVertex, 
				       LastVertex, OnFirstFace, OnLastFace,
				       PtOnFirstEdge, PtOnLastEdge,
				       OnFirstEdge, OnLastEdge);
 
  if(!Data) {
#ifdef DEB
    if (trc) cout << " No Extreme faces" << endl;
#endif
    myStatusError = BRepFeat_NoExtFace;
    NotDone();
    return;
  }


// ---Point detrompeur pour le cote du wire a remplir - cote matiere
  gp_Pnt CheckPnt = CheckPoint(FirstEdge, bnd/10., myPln);

//  Standard_Real f, l;

// ---Controle glissement valable
// Plein de cas ou on sort du glissement
  Standard_Integer Concavite = 3;  // a priori le profile n'est pas concave

  myFirstPnt = BRep_Tool::Pnt(FirstVertex);
  myLastPnt  = BRep_Tool::Pnt(LastVertex);

// SliList : liste des faces concernees par la nervure
  TopTools_ListOfShape SliList;
  SliList.Append(FirstFace);

  if(Sliding) {    // glissement
#ifdef DEB
    if (trc) cout << " Sliding" << endl;
#endif
    Standard_Boolean Sliding = Standard_False;
    Handle(Geom_Surface) s = BRep_Tool::Surface(FirstFace);
    if (s->DynamicType() == 
	STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      s = Handle(Geom_RectangularTrimmedSurface)::
	DownCast(s)->BasisSurface();
    }
    if(s->DynamicType() == STANDARD_TYPE(Geom_Plane) ||
       s->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface)) {
// si plan ou cylindre : glissement possible
      Sliding = Standard_True;
    }
  }

// Controle uniquement points de depart et d'arrivee
// -> pas de controle au milieu - a ameliorer
// Si on faisait un controle entre Surface et sgement entre les 2 points limite
// -> tres cher - a ameliorer
  if(Sliding) {
    gp_Pnt p1(myFirstPnt.X()+myDir.X(),myFirstPnt.Y()+myDir.Y(),
	      myFirstPnt.Z()+myDir.Z());
    BRepLib_MakeEdge ee1(myFirstPnt, p1);
    BRepExtrema_ExtCF ext1(ee1, FirstFace);
    if(ext1.NbExt() == 1 && ext1.SquareDistance(1)<=BRep_Tool::Tolerance(FirstFace) * BRep_Tool::Tolerance(FirstFace)) {
      gp_Pnt p2(myLastPnt.X()+myDir.X(),myLastPnt.Y()+myDir.Y(),
		myLastPnt.Z()+myDir.Z());
      BRepLib_MakeEdge ee2(myLastPnt, p2);
      BRepExtrema_ExtCF ext2(ee2, LastFace); // ExtCF : courbes et surfaces
      if(ext2.NbExt() == 1 && ext2.SquareDistance(1)<=BRep_Tool::Tolerance(LastFace) * BRep_Tool::Tolerance(LastFace)) {
	Sliding = Standard_True;
      }
      else {
	Sliding = Standard_False;
      }      
    }
    else {
      Sliding = Standard_False;
    }   
  }

  if(!myDir1.IsEqual(nulldir, Precision::Confusion(), Precision::Confusion())) {
    if(Sliding) {
      gp_Pnt p1(myFirstPnt.X()+myDir1.X(),myFirstPnt.Y()+myDir1.Y(),
		myFirstPnt.Z()+myDir1.Z());
      BRepLib_MakeEdge ee1(myFirstPnt, p1);
      BRepExtrema_ExtCF ext1(ee1, FirstFace);
      if(ext1.NbExt() == 1 && ext1.SquareDistance(1)<=BRep_Tool::Tolerance(FirstFace) * BRep_Tool::Tolerance(FirstFace)) {
	gp_Pnt p2(myLastPnt.X()+myDir1.X(),myLastPnt.Y()+myDir1.Y(),
		  myLastPnt.Z()+myDir1.Z());
	BRepLib_MakeEdge ee2(myLastPnt, p2);
	BRepExtrema_ExtCF ext2(ee2, LastFace);
	if(ext2.NbExt() == 1 && ext2.SquareDistance(1)<=BRep_Tool::Tolerance(LastFace) * BRep_Tool::Tolerance(LastFace)) {
	  Sliding = Standard_True;
	}
	else {
	  Sliding = Standard_False;
	}      
      }
      else {
	Sliding = Standard_False;
      }   
    }
  }


// On construit un gros profil qui va jusqu`a la boite englobante
// -> par tangence avec premier et dernier edge du Wire
// -> par normales aux faces d'appui : statistiquement meilleur
// On intersecte le tout pour trouver le profil final


// ---cas de glissement : construction de la face profil
  if(Sliding) {
#ifdef DEB
    if (trc) cout << " still Sliding" << endl;
#endif
    TopoDS_Face Prof;
    Standard_Boolean ProfileOK;
    ProfileOK = SlidingProfile(Prof,RevolRib,myTol,Concavite,myPln,BndFace,CheckPnt,
			       FirstFace,LastFace,FirstVertex,LastVertex,
			       FirstEdge,LastEdge);
    
    if (!ProfileOK) {
#ifdef DEB
      cout << "Not computable" << endl;
      if (trc) cout << "Face profile not computable" << endl;
#endif
      myStatusError = BRepFeat_NoFaceProf;
      NotDone();
      return;
    }


// ---Propagation sur les faces du shape initial
// pour trouver les faces concernees par la nervure
    Standard_Boolean falseside = Standard_True;
    Sliding = Propagate(SliList, Prof, myFirstPnt, myLastPnt, falseside);
// Controle si on a ce qu`il faut pour avoir la matiere du bon cote
    if(falseside == Standard_False) {
#ifdef DEB
      cout << "Verify plane and wire orientation" << endl;
#endif
      myStatusError = BRepFeat_FalseSide;
      NotDone();
      return;
    }
  }


// ---Generation du profile de base de la nervure

  TopoDS_Wire w;
  BB.MakeWire(w);
  TopoDS_Edge thePreviousEdge;
  TopoDS_Vertex theFV;
  thePreviousEdge.Nullify();

// compteur du nombre d`edges pour remplir la map
  Standard_Integer counter = 1;

// ---cas de glissement
  if(Sliding && !myListOfEdges.IsEmpty()) {
    BRepTools_WireExplorer EX1(myWire);
    for(; EX1.More(); EX1.Next()) {
      const TopoDS_Edge& E = EX1.Current();
      if(!myLFMap.IsBound(E)) {
        TopTools_ListOfShape theTmpList;
	myLFMap.Bind(E, theTmpList );
      }      
      if(E.IsSame(FirstEdge)) {
	Standard_Real f, l;
	Handle(Geom_Curve) cc = BRep_Tool::Curve(E, f, l);
	cc = new Geom_TrimmedCurve(cc, f, l);
	gp_Pnt pt;
	if(!FirstEdge.IsSame(LastEdge)) {
	  pt = BRep_Tool::Pnt(TopExp::LastVertex(E,Standard_True)); 
	}
	else {
	  pt = myLastPnt;
	  Standard_Real fpar = IntPar(cc, myFirstPnt);
	  Standard_Real lpar = IntPar(cc, pt);
	  Handle(Geom_Curve) ccc;
	  if(fpar > lpar) {
	    ccc = Handle(Geom_Curve)::DownCast(cc->Reversed());
	    cc = ccc;
	  }
	}
	TopoDS_Edge ee1;
	if(thePreviousEdge.IsNull()) {
	  BRepLib_MakeVertex v1(myFirstPnt);
	  BRepLib_MakeVertex v2(pt);	  
	  BRepLib_MakeEdge e(cc, v1, v2);
	  ee1 = TopoDS::Edge(e.Shape());
	} 
	else {
	  const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	  BRepLib_MakeVertex v2(pt);
	  
	  BRepLib_MakeEdge e(cc, v1, v2);
	  ee1 = TopoDS::Edge(e.Shape());
	}
	TopoDS_Shape aLocalShape = ee1.Oriented(E.Orientation());
	ee1 = TopoDS::Edge(aLocalShape);
//	ee1 = TopoDS::Edge(ee1.Oriented(E.Orientation()));
	if(counter == 1) theFV = TopExp::FirstVertex(ee1,Standard_True);
	myLFMap(E).Append(ee1);
	BB.Add(w, ee1);
	thePreviousEdge = ee1;
	counter++;
	EX1.Next();
	break;
      }
    }
 
// Cas plusieurs edges
    if(!FirstEdge.IsSame(LastEdge)) {
      for(; EX1.More(); EX1.Next()) {
	const TopoDS_Edge& E = EX1.Current();
	if(!myLFMap.IsBound(E)) {
          TopTools_ListOfShape thelist1;
	  myLFMap.Bind(E, thelist1);
	}      
	theList.Append(E);
	Standard_Real f, l;
	if(!E.IsSame(LastEdge)) {
	  Handle(Geom_Curve) ccc = BRep_Tool::Curve(E, f, l);
	  TopoDS_Vertex v1, v2;
	  if(!thePreviousEdge.IsNull()) {
	    v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	    v2 = TopExp::LastVertex(E,Standard_True);
	  }
	  else {
//	    v1 = TopExp::LastVertex(E,Standard_True);
	    v1 = TopExp::FirstVertex(E,Standard_True);
	    v2 = TopExp::LastVertex(E,Standard_True);
	  }
	  BRepLib_MakeEdge E1(ccc, v1, v2);
	  TopoDS_Edge E11 = TopoDS::Edge(E1.Shape());
	  TopoDS_Shape aLocalShape = E11.Oriented(E.Orientation());
	  E11 = TopoDS::Edge(aLocalShape);
//	  E11 = TopoDS::Edge(E11.Oriented(E.Orientation()));
	  thePreviousEdge = E11;
	  myLFMap(E).Append(E11);
	  BB.Add(w, E11);
	  if(counter == 1) theFV = TopExp::FirstVertex(E11,Standard_True);
	  counter++;
	}
	else {
	  Standard_Real f, l;
	  Handle(Geom_Curve) cc = BRep_Tool::Curve(E, f, l);
	  gp_Pnt pf = BRep_Tool::Pnt(TopExp::FirstVertex(E,Standard_True));
	  gp_Pnt pl = myLastPnt;
	  TopoDS_Edge ee;
	  if(thePreviousEdge.IsNull()) {
	    BRepLib_MakeEdge e(cc, pf , pl); 
	    ee = TopoDS::Edge(e.Shape());
	  }
	  else {
	    const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	    BRepLib_MakeVertex v2(pl);
	    BRepLib_MakeEdge e(cc, v1, v2);
	    ee = TopoDS::Edge(e.Shape());
	  }
	  TopoDS_Shape aLocalShape = ee.Oriented(E.Orientation());
	  ee = TopoDS::Edge(aLocalShape);
//	  ee = TopoDS::Edge(ee.Oriented(E.Orientation()));
	  BB.Add(w, ee);
	  myLFMap(E).Append(ee);
	  if(counter == 1) theFV = TopExp::FirstVertex(ee,Standard_True);
	  thePreviousEdge = ee;
	  counter++;
	  break;
	}
      }
    }
    
    TopTools_ListIteratorOfListOfShape it(myListOfEdges);
    Standard_Boolean FirstOK = Standard_False;
    Standard_Boolean LastOK = Standard_False;
    
    gp_Pnt theLastPnt = myLastPnt;
    Standard_Integer sens = 0;
    TopoDS_Edge theEdge, theLEdge, theFEdge;
    Standard_Integer counter1 = counter;
    TopTools_ListOfShape NewListOfEdges;
    NewListOfEdges.Clear();
    while (!FirstOK) {
      const TopoDS_Edge& edg = TopoDS::Edge(it.Value());
      gp_Pnt fp, lp;
      Standard_Real f, l;
      Handle(Geom_Curve) ccc = BRep_Tool::Curve(edg, f, l);
      Handle(Geom_TrimmedCurve) cc = new Geom_TrimmedCurve(ccc, f, l);
      if ( edg.Orientation() == TopAbs_REVERSED) cc->Reverse();

      fp = cc->Value(cc->FirstParameter());
      lp = cc->Value(cc->LastParameter());
      Standard_Real dist = fp.Distance(theLastPnt);
      if(dist <= myTol) {
	sens = 1;
	LastOK = Standard_True;
      }
      else {
	dist = lp.Distance(theLastPnt);
	if(dist <= myTol) {
	  sens = 2;
	  LastOK = Standard_True;
	  cc->Reverse();
	}
      }
      Standard_Integer FirstFlag = 0;
      if(sens==1 && lp.Distance(myFirstPnt) <= myTol) {
	FirstOK = Standard_True;
	FirstFlag = 1;
      }
      else if(sens==2 && fp.Distance(myFirstPnt) <= myTol) {
	FirstOK = Standard_True;
	FirstFlag = 2;
      }
      
      if (LastOK) {
	TopoDS_Edge eeee;
	Standard_Real fpar = cc->FirstParameter();
	Standard_Real lpar = cc->LastParameter();
	if(!FirstOK) {
	  if(thePreviousEdge.IsNull()) {
	    BRepLib_MakeEdge e(cc, fpar, lpar);
	    eeee = TopoDS::Edge(e.Shape());
	  }
	  else {
	    const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	    BB.UpdateVertex(v1, dist);
	    BRepLib_MakeVertex v2(cc->Value(lpar));
	    TopoDS_Vertex nv=v2.Vertex();
	    BRepLib_MakeEdge e(cc, v1, nv);
	    eeee = TopoDS::Edge(e.Shape());
	  }
	}
	else {
	  if(thePreviousEdge.IsNull()) {
	    BRepLib_MakeVertex v1(cc->Value(fpar)); 
	    BRepLib_MakeEdge e(cc, v1, theFV);
	    eeee = TopoDS::Edge(e.Shape());
	  }
	  else {
	    const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	    BRepLib_MakeEdge e(cc, v1, theFV);
	    eeee = TopoDS::Edge(e.Shape());
	  }
	}

	thePreviousEdge = eeee;
	BB.Add(w, eeee);
	if(counter == 1) theFV = TopExp::FirstVertex(eeee,Standard_True);
	counter1++;
	NewListOfEdges.Append(edg);
	theEdge = eeee;
#ifdef DEB
	Standard_Real dist1 = 
#endif
	  theLastPnt.Distance(myLastPnt);

	if(dist <= myTol) 
	  theFEdge = edg;
	theLastPnt = BRep_Tool::Pnt(TopExp::LastVertex(theEdge,Standard_True));
      }

      if(FirstFlag == 1) {
	theLEdge = edg;
      }
      else if(FirstFlag == 2) {
	theLEdge = theEdge;
      }

      if(LastOK) {
	myListOfEdges.Remove(it);
	it.Initialize(myListOfEdges);
	LastOK = Standard_False;
      }
      else if(it.More()) it.Next();
      else {
	Sliding = Standard_False;
	break;
      }	
      sens = 0;
    }


    TopTools_DataMapOfShapeListOfShape SlidMap;
    SlidMap.Clear();
    
    if(Sliding && counter1 > counter) {
      TopTools_ListIteratorOfListOfShape it;
      TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm;
      TopExp_Explorer EX2(w, TopAbs_EDGE);
      Standard_Integer ii = 0;
      for(; EX2.More(); EX2.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge(EX2.Current());
	ii++;	
	if(ii >= counter && ii <= counter1) {
	  it.Initialize(NewListOfEdges);
	  Standard_Integer jj = 0;
	  for(; it.More(); it.Next()) {
	    const TopoDS_Edge& e2 = TopoDS::Edge(it.Value());
	    jj++;
	    if(jj== (ii - counter +1)) {	  
	      itm.Initialize(mySlface);
	      for(; itm.More(); itm.Next()) {
		const TopoDS_Face& fac = TopoDS::Face(itm.Key());
		const TopTools_ListOfShape& ledg = itm.Value();
		TopTools_ListIteratorOfListOfShape itedg(ledg);
		//Standard_Integer iiii = 0;
		for(; itedg.More(); itedg.Next()) {
		  const TopoDS_Edge& e1 = TopoDS::Edge(itedg.Value());
		  if(e1.IsSame(e2)){
		    if(!SlidMap.IsBound(fac)) {
                      TopTools_ListOfShape thelist2;
		      SlidMap.Bind(fac, thelist2);
		    }
		    SlidMap(fac).Append(E);
		  }
		}		
	      }
	    }
	  } 
	}
      }
    }

    mySlface.Clear();
    mySlface = SlidMap;
  }

// ---Arguments de LocOpe_LinearForm : arguments du prism
// glissement
  if(Sliding) {
    TopoDS_Face F;
    BB.MakeFace(F, myPln, myTol);
    BB.Add(F, w);
//    BRepLib_MakeFace F(myPln->Pln(),w, Standard_True);
    mySkface = F;
    myPbase  = mySkface;
    mySUntil.Nullify();
  }
  

// ---Cas sans glissement : construction de la face profil  
  if(!Sliding) {
#ifdef DEB
    if (trc) {
      if (Modify) cout << " Sliding failure" << endl;
      cout << " no Sliding" << endl;
    }
#endif
    TopoDS_Face Prof;
    Standard_Boolean ProfileOK;
    ProfileOK = NoSlidingProfile(Prof,RevolRib,myTol,Concavite,myPln,
				 bnd,BndFace,CheckPnt,
				 FirstFace,LastFace,FirstVertex,LastVertex,
				 FirstEdge,LastEdge,OnFirstFace,OnLastFace);
    
    if (!ProfileOK) {
#ifdef DEB
      cout << "Not computable" << endl;
      if (trc) cout << " Face profile not computable" << endl;
#endif
      myStatusError = BRepFeat_NoFaceProf;
      NotDone();
      return;
    }


// ---Propagation sur les faces du shape initial
// pour trouver les faces concernees par la nervure
    Standard_Boolean falseside = Standard_True;
    Propagate(SliList, Prof, myFirstPnt, myLastPnt, falseside);
// Controle si on a ce qu`il faut pour avoir la matiere du bon cote
    if(falseside == Standard_False) {
#ifdef DEB
      cout << "Verify plane and wire orientation" << endl;
#endif
      myStatusError = BRepFeat_FalseSide;
      NotDone();
      return;
    }

    mySlface.Clear();

    TopTools_ListIteratorOfListOfShape it;
    it.Initialize(SliList);
    
    TopoDS_Shape comp;
    
    BRep_Builder BB;
    BB.MakeShell(TopoDS::Shell(comp));
    
    for(; it.More(); it.Next()) {
      BB.Add(comp, it.Value());
    }
    
    mySUntil = comp;

    mySkface = Prof;
    myPbase  = Prof;
  }

  mySliding = Sliding;

  TopExp_Explorer exp;
  for (exp.Init(mySbase,TopAbs_FACE);exp.More();exp.Next()) {
    TopTools_ListOfShape thelist3;
    myMap.Bind(exp.Current(), thelist3);
    myMap(exp.Current()).Append(exp.Current());
  }
}


//=======================================================================
//function : Add
//purpose  : add des element de collage
//=======================================================================

void BRepFeat_MakeLinearForm::Add(const TopoDS_Edge& E,
				  const TopoDS_Face& F)
{
#ifdef DEB
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeLinearForm::Add" << endl;
#endif
  if(mySlface.IsEmpty()) {
    TopExp_Explorer exp;
    for (exp.Init(mySbase,TopAbs_FACE);exp.More();exp.Next()) {
      if (exp.Current().IsSame(F)) {
	break;
      }
    }
    if (!exp.More()) {
      Standard_ConstructionError::Raise();
    }
  
    if (!mySlface.IsBound(F)) {
      TopTools_ListOfShape thelist;
      mySlface.Bind(F, thelist);
    }
    TopTools_ListIteratorOfListOfShape itl(mySlface(F));
    for (; itl.More();itl.Next()) {
      if (itl.Value().IsSame(E)) {
	break;
      }
    }
    if (!itl.More()) {
      mySlface(F).Append(E);
    }
  } 
}

  

//=======================================================================
//function : Perform
//purpose  : construction de nervure a partir d'un profile et du shape init
//=======================================================================

void BRepFeat_MakeLinearForm::Perform()
{
#ifdef DEB
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeLinearForm::Perform()" << endl;
#endif
  if(mySbase.IsNull() || mySkface.IsNull() || myPbase.IsNull()) {
#ifdef DEB
    if (trc) cout << " Fields not initialized" << endl;
#endif
    myStatusError = BRepFeat_NotInitialized;
    NotDone();
    return;
  }

  gp_Vec nulldir(0, 0, 0);

  Standard_Real Length = myDir.Magnitude() +  myDir1.Magnitude();

  myGluedF.Clear();
 
  if(!mySUntil.IsNull()) 
    myPerfSelection = BRepFeat_SelectionU;
  else 
    myPerfSelection = BRepFeat_NoSelection;

  gp_Dir dir(myDir);
  gp_Vec V = Length*dir;

  LocOpe_LinearForm theForm;

  if(myDir1.IsEqual(nulldir, Precision::Confusion(), Precision::Confusion())) 
    theForm.Perform(myPbase, V, myFirstPnt, myLastPnt);
  else 
    theForm.Perform(myPbase, V, myDir1, myFirstPnt, myLastPnt);

  TopoDS_Shape VraiForm = theForm.Shape();   // primitive de la nervure

  myFacesForDraft.Append(theForm.FirstShape());
  myFacesForDraft.Append(theForm.LastShape());
  MajMap(myPbase,theForm,myMap,myFShape,myLShape);   // gestion de descendants

  TopExp_Explorer exx(myPbase, TopAbs_EDGE);
  for(; exx.More(); exx.Next()) {
    const TopoDS_Edge& e = TopoDS::Edge(exx.Current());
    if(!myMap.IsBound(e)) {
#ifdef DEB
      if (trc) cout << " Sliding face not in Base shape" << endl;
#endif
      myStatusError = BRepFeat_IncSlidFace;
      NotDone();
      return;
    }
  }

  myGShape = VraiForm;
  SetGluedFaces(mySlface, theForm, myGluedF);  // gestion des faces de glissement 

  if(!myGluedF.IsEmpty() && !mySUntil.IsNull()) {
#ifdef DEB
    cout << "The case is not computable" << endl;
    if (trc) cout << " Glued faces not empty and Until shape not null" << endl;
#endif
    myStatusError = BRepFeat_InvShape;
    NotDone();
    return;
  }

  LFPerform();

/*

  TopExp_Explorer expr(mySbase, TopAbs_FACE);
  char nom1[20], nom2[20];
  Standard_Integer ii = 0;
  for(; expr.More(); expr.Next()) {
    ii++;
    sprintf(nom1, "faceinitial_%d", ii);
    DBRep::Set(nom1, expr.Current());
    Standard_Integer jj = 0;
    const TopTools_ListOfShape& list = Modified(expr.Current());
    TopTools_ListIteratorOfListOfShape ite(list);
    for(; ite.More(); ite.Next()) {
      jj++;
      sprintf(nom2, "facemodifie_%d_%d", ii, jj);
      DBRep::Set(nom2, ite.Value());
    }
  }

  expr.Init(myWire, TopAbs_EDGE);
  ii=0;
  for(; expr.More(); expr.Next()) {
    ii++;
    sprintf(nom1, "edgeinitial_%d", ii);
    DBRep::Set(nom1, expr.Current());
    Standard_Integer jj = 0;
    const TopTools_ListOfShape& genf = Generated(expr.Current());
    TopTools_ListIteratorOfListOfShape ite(genf);
    for(; ite.More(); ite.Next()) {
      jj++;
      sprintf(nom2, "egdegeneree_%d_%d", ii, jj);
      DBRep::Set(nom2, ite.Value());
    }
  }
*/
}

//=======================================================================
//function : Propagate
//purpose  : propagation sur les faces du shape initial, recherche 
// des faces concernees par la nervure
//=======================================================================
  Standard_Boolean BRepFeat_MakeLinearForm::Propagate(TopTools_ListOfShape& SliList,
						      const TopoDS_Face& fac,
						      const gp_Pnt& Firstpnt, 
						      const gp_Pnt& Lastpnt, 
						      Standard_Boolean& falseside)
{
#ifdef DEB
  Standard_Boolean trc = BRepFeat_GettraceFEATRIB();
  if (trc) cout << "BRepFeat_MakeLinearForm::Propagate" << endl;
#endif
  gp_Pnt Firstpoint = Firstpnt;
  gp_Pnt Lastpoint = Lastpnt;

  Standard_Boolean result = Standard_True;
  TopoDS_Face CurrentFace, saveFace;
  CurrentFace = TopoDS::Face(SliList.First());
  saveFace = CurrentFace;

  Standard_Boolean LastOK = Standard_False, FirstOK= Standard_False;
  Standard_Boolean v1OK = Standard_False, v2OK= Standard_False;
  TopoDS_Vertex v1, v2, v3, v4, ve1, ve2;

  //modified by NIZNHY-PKV Fri Mar 22 16:50:24 2002 f
  //BRepAlgo_Section sect (fac, CurrentFace, Standard_False);
  BRepAlgoAPI_Section sect (fac, CurrentFace, Standard_False);
  //modified by NIZNHY-PKV Fri Mar 22 16:50:32 2002 t

  sect.Approximation(Standard_True);
  sect.Build();

  TopExp_Explorer Ex;
  TopoDS_Edge eb, ec;
  gp_Pnt p1, p2;
#ifndef DEB
  Standard_Real t1 = 0., t2 = 0.;
#else
  Standard_Real t1, t2;
#endif
  Standard_Boolean c1f, c2f, c1l, c2l;

  for (Ex.Init(sect.Shape(), TopAbs_EDGE); Ex.More(); Ex.Next()) {
    ec = TopoDS::Edge(Ex.Current());
    v1 = TopExp::FirstVertex(ec,Standard_True); 
    v2 = TopExp::LastVertex(ec,Standard_True); 
    p1 = BRep_Tool::Pnt(v1);
    p2 = BRep_Tool::Pnt(v2);
    t1 = BRep_Tool::Tolerance(v1);
    t2 = BRep_Tool::Tolerance(v2);
    c1f = p1.Distance(Firstpoint)<=t1;
    c2f = p2.Distance(Firstpoint)<=t2;
    c1l = p1.Distance(Lastpoint)<=t1;
    c2l = p2.Distance(Lastpoint)<=t2;
    if (c1f || c2f || c1l|| c2l) {
      eb = ec;
      if (c1f || c1l) v1OK=Standard_True;
      if (c2f || c2l) v2OK=Standard_True;
      if (c1f || c2f) FirstOK=Standard_True;
      if (c1l || c2l) LastOK=Standard_True;
      break;
    }
  }

  if(eb.IsNull()) {
    falseside = Standard_False;
    return Standard_False;
  }
  TopTools_ListOfShape thelist;
  mySlface.Bind(CurrentFace, thelist);
  mySlface(CurrentFace).Append(eb);
    
  myListOfEdges.Clear();
  myListOfEdges.Append(eb);
    
  // Les deux points sont sur la meme face.
  if(LastOK && FirstOK) {
    return result;
  }
  
  TopTools_IndexedDataMapOfShapeListOfShape mapedges;
  TopExp::MapShapesAndAncestors(mySbase, TopAbs_EDGE, TopAbs_FACE, mapedges);
  TopExp_Explorer ex;
  TopoDS_Edge FirstEdge;
  BRep_Builder BB;

  TopoDS_Vertex Vprevious;
  gp_Pnt ptprev;
  Standard_Real tvp, dp;

  while (!(LastOK && FirstOK)) {
    if (v1OK) {
      Vprevious=v2;
      ptprev=p2;
      tvp=t2;
    }
    else {
      Vprevious=v1;
      ptprev=p1;
      tvp=t1;
    }
    
    // retrouver l'edge connexe a v1 ou v2:
    for (ex.Init(CurrentFace, TopAbs_EDGE); ex.More(); ex.Next()) {
      const TopoDS_Edge& rfe = TopoDS::Edge(ex.Current());

      BRepExtrema_ExtPC projF(Vprevious, rfe);

      if(projF.IsDone() && projF.NbExt() >=1) {
	Standard_Real dist2min = RealLast();
	Standard_Integer index = 0;
	for (Standard_Integer sol =1 ; sol <= projF.NbExt(); sol++) {
	  if (projF.SquareDistance(sol) <= dist2min) {
	    index = sol;
	    dist2min = projF.SquareDistance(sol);
	  }
	}
	if (index != 0) {
	  if (dist2min <= BRep_Tool::Tolerance(rfe) * BRep_Tool::Tolerance(rfe)) {
	    FirstEdge = rfe;
	    // Si l'edge n'est pas perpendiculaire au plan de la nervure il
	    // faut mettre Sliding(result) a faux.
	    if (result) {
	      result=Standard_False;
	      ve1 = TopExp::FirstVertex(rfe,Standard_True);
	      ve2 = TopExp::LastVertex(rfe,Standard_True);
	      BRepExtrema_ExtPF perp(ve1, fac);
	      if (perp.IsDone()) {
		gp_Pnt pe1=perp.Point(1);
		perp.Perform(ve2, fac);
		if (perp.IsDone()) {
		  gp_Pnt pe2=perp.Point(1);
		  if (pe1.Distance(pe2)<=BRep_Tool::Tolerance(rfe)) 
		    result=Standard_True;
		}
	      }
	    }
	    break;
	  }
	}
      }
    }
    
    const TopTools_ListOfShape& L = mapedges.FindFromKey(FirstEdge);
    TopTools_ListIteratorOfListOfShape It(L);

    for (; It.More(); It.Next()) {
      const TopoDS_Face& FF = TopoDS::Face(It.Value());
      if (!FF.IsSame(CurrentFace)) {
	CurrentFace = FF;
	break;
      }
    }

    //modified by NIZNHY-PKV Fri Mar 22 16:50:53 2002 f
    //BRepAlgo_Section sectf (fac, CurrentFace, Standard_False);
    BRepAlgoAPI_Section sectf (fac, CurrentFace, Standard_False);
    //modified by NIZNHY-PKV Fri Mar 22 16:51:03 2002 t
    sectf.Approximation(Standard_True);
    sectf.Build();

    TopoDS_Edge edg1;
    for (Ex.Init(sectf.Shape(), TopAbs_EDGE); Ex.More(); Ex.Next()) {
      edg1 = TopoDS::Edge(Ex.Current());
      v1=TopExp::FirstVertex(edg1,Standard_True);
      v2=TopExp::LastVertex(edg1,Standard_True);
      t1 = BRep_Tool::Tolerance(v1);
      t2 = BRep_Tool::Tolerance(v2);
      p1 = BRep_Tool::Pnt(v1);
      p2 = BRep_Tool::Pnt(v2);
      v1OK = p1.Distance(ptprev)<=t1;
      v2OK = p2.Distance(ptprev)<=t2;
      if (v1OK || v2OK) break;
    }    
    
    if (v1OK) {
      if (!FirstOK) {
	dp = p2.Distance(Firstpoint);
	if(dp <= 2*t2) {
	  FirstOK = Standard_True;
	  BB.UpdateVertex(v2, dp);
	}
      }
      if (!LastOK) {
	dp = p2.Distance(Lastpoint);
	if(dp <= 2*t2) {
	  LastOK = Standard_True;
	  BB.UpdateVertex(v2, dp);
	}
      }
    }
    else if (v2OK) {
      if (!FirstOK) {
	dp = p1.Distance(Firstpoint);
	if(dp <= 2*t1) {
	  FirstOK = Standard_True;
	  BB.UpdateVertex(v1, dp);
	}
      }
      if (!LastOK) {
	dp = p1.Distance(Lastpoint);
	if(dp <= 2*t1) {
	  LastOK = Standard_True;
	  BB.UpdateVertex(v1, dp);
	}
      }
    }
    else {
      // on arrive pas a chainer la section
      return Standard_False;
    }
// #ifdef DEB
//     Standard_Boolean isnb=
// #endif
    TopTools_ListOfShape thelist1;
#ifdef DEB
     Standard_Boolean isnb=
#endif
    mySlface.Bind(CurrentFace, thelist1);
    mySlface(CurrentFace).Append(edg1);
    myListOfEdges.Append(edg1);
  }

  return result;
  
}

//=======================================================================
//function : MajMap
//purpose  : gestion de descendants
//=======================================================================

static void MajMap(const TopoDS_Shape& theB,
		   const LocOpe_LinearForm& theP,
		   TopTools_DataMapOfShapeListOfShape& theMap, // myMap
		   TopoDS_Shape& theFShape,  // myFShape
		   TopoDS_Shape& theLShape) // myLShape
{
  TopExp_Explorer exp(theP.FirstShape(),TopAbs_WIRE);
  if (exp.More()) {
    theFShape = exp.Current();
    TopTools_ListOfShape thelist;
    theMap.Bind(theFShape, thelist);
    for (exp.Init(theP.FirstShape(),TopAbs_FACE);exp.More();exp.Next()) {
      theMap(theFShape).Append(exp.Current());
    }
  }
  
  exp.Init(theP.LastShape(),TopAbs_WIRE);
  if (exp.More()) {
    theLShape = exp.Current();
    TopTools_ListOfShape thelist1;
    theMap.Bind(theLShape, thelist1);
    for (exp.Init(theP.LastShape(),TopAbs_FACE);exp.More();exp.Next()) {
      theMap(theLShape).Append(exp.Current());
    }
  }
  
  for (exp.Init(theB,TopAbs_EDGE); exp.More(); exp.Next()) {
    if (!theMap.IsBound(exp.Current())) {
#ifdef DEB
      const TopoDS_Edge& e = 
#endif
      TopoDS::Edge(exp.Current());
      TopTools_ListOfShape thelist2; 
      theMap.Bind(exp.Current(), thelist2);
      theMap(exp.Current()) = theP.Shapes(exp.Current());
    }
  }
}

//=======================================================================
//function : SetGluedFaces
//purpose  : gestion des faces de collage
//=======================================================================

static void SetGluedFaces(const TopTools_DataMapOfShapeListOfShape& theSlmap,
			  LocOpe_LinearForm& thePrism,
			  TopTools_DataMapOfShapeShape& theMap)
{
  // Glissements
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(theSlmap);
  if(!theSlmap.IsEmpty()) {
    for (; itm.More(); itm.Next()) {
      const TopoDS_Face& fac = TopoDS::Face(itm.Key());
      const TopTools_ListOfShape& ledg = itm.Value();
      TopTools_ListIteratorOfListOfShape it;
      for (it.Initialize(ledg); it.More(); it.Next()) {
	const TopTools_ListOfShape& gfac = thePrism.Shapes(it.Value());
	if (gfac.Extent() != 1) {
#ifdef DEB
	  cout << "Pb SetGluedFace" << endl;
#endif
	}
	theMap.Bind(gfac.First(),fac);
      }
    }
  }
}

