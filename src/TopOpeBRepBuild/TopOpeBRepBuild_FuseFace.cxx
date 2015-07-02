// Created on: 1998-07-28
// Created by: LECLERE Florence
// Copyright (c) 1998-1999 Matra Datavision
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

#include <TopOpeBRepBuild_FuseFace.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeInteger.hxx>

#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>

#include <BRepLib_MakeWire.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeEdge.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepCheck_Analyzer.hxx>

#include <Geom_Surface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <ElCLib.hxx>
#include <Precision.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceFUFA();
#endif

static void GroupShape(TopTools_ListOfShape&,
		       Standard_Boolean,
		       TopTools_DataMapOfShapeListOfShape&);

static void GroupEdge(TopTools_DataMapOfShapeListOfShape&,
		      TopTools_DataMapOfShapeListOfShape&);

static void MakeEdge(TopTools_DataMapOfShapeListOfShape&);

static Standard_Boolean SameSupport(const TopoDS_Edge&,
				    const TopoDS_Edge&);

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_FuseFace::Init(const TopTools_ListOfShape& LIF,
				    const TopTools_ListOfShape& LRF,
				    const Standard_Integer CXM)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc) cout << "TopOpeBRepBuild_FuseFace::Init" << endl;
#endif
  myLIF = LIF;
  myLRF = LRF;
  if(CXM == 1) {
    myInternal = Standard_False;
  }
  else if(CXM == 2) {
    myInternal = Standard_True;
  } // CXM
#ifdef OCCT_DEBUG
  if (trc) {
    if (myInternal) {
      cout << " TopOpeBRepBuild_FuseFace::Init : Keep internal connections" << endl;
    } else {
      cout << " TopOpeBRepBuild_FuseFace::Init : Suppress internal connections" << endl;
    }
  }
#endif

  myLFF.Clear();

  myLIE.Clear();
  myLEE.Clear();
  myLME.Clear();

  myLIV.Clear();
  myLEV.Clear();
  myLMV.Clear();

  myModified = Standard_False;
  myDone = Standard_False;

}

//=======================================================================
//function : PerformFace
//purpose  : fusion des faces cosurfaciques, connexes par une ou 
//plusieurs aretes       
//=======================================================================

void TopOpeBRepBuild_FuseFace::PerformFace()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc) cout << "TopOpeBRepBuild_FuseFace::PerformFace()" << endl;
#endif

  myModified = Standard_False;
  myLFF.Clear();
  if (myLRF.IsEmpty()) {
#ifdef OCCT_DEBUG
    if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Empty list of reconstructed faces"<<endl;
#endif
    myModified = Standard_False;
    myDone = Standard_True;
    myLFF = myLRF;
    return;
  }

  Standard_Integer number = myLRF.Extent();
  if (number == 1) {
#ifdef OCCT_DEBUG
    if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : only 1 reconstructed face"<<endl;
#endif
    myModified = Standard_False;
    myDone = Standard_True;
    myLFF = myLRF;
    return;
  }
    
  TopTools_ListIteratorOfListOfShape it2,it3,it4;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itt1,itt2,itt3;
  TopAbs_Orientation ori1;

  Standard_Boolean Ori3dReversed = Standard_False;
  Standard_Boolean Ori3dForward = Standard_False;
  TopTools_ListOfShape mylist;
  for(it2.Initialize(myLRF); it2.More(); it2.Next()) {
    TopoDS_Shape fac = it2.Value();
    ori1 = fac.Orientation();
    if (ori1 == TopAbs_FORWARD) {
      Ori3dForward = Standard_True;
    }
    if (ori1 == TopAbs_REVERSED) {
      Ori3dReversed = Standard_True;
    }
    BRepCheck_Analyzer ana(fac);
    if (!ana.IsValid(fac)) {
//    if (!BRepCheck_Analyzer::IsValid(fac)) {
#ifdef OCCT_DEBUG
      if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Invalid reconstructed face"<<endl;
#endif
      myModified = Standard_False;
      myDone = Standard_True;
      myLFF = myLRF;
      return;
    }
    fac.Orientation(TopAbs_FORWARD);
    mylist.Append(fac);
  }

// Orientation 3d de l'espace limite par la face
  if (Ori3dForward && Ori3dReversed) {
#ifdef OCCT_DEBUG
    if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Bad faces 3d orientation"<<endl;
#endif
    myModified = Standard_False;
    myDone = Standard_True;
    myLFF = myLRF;
    return;
  }
  
// listes de faces avec edges communes.
  Standard_Boolean Keep_Edge;
  Keep_Edge = Standard_False;
  TopTools_DataMapOfShapeListOfShape mapFacLFac;
  GroupShape(mylist,Keep_Edge,mapFacLFac);
  if (mapFacLFac.IsEmpty()) {
#ifdef OCCT_DEBUG
    if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Empty list of faces"<<endl;
#endif
    myModified = Standard_False;
    myDone = Standard_True;
    myLFF = myLRF;
    return;
  }
  Standard_Integer n1 = myLRF.Extent();
  Standard_Integer n2 = mapFacLFac.Extent();
  if (n1 == n2) {
#ifdef OCCT_DEBUG
    if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : No connection"<<endl;
#endif
    myModified = Standard_False;
    myDone = Standard_True;
    myLFF = myLRF;
    return;
  }

  
//boucle sur les listes des faces de 1 face de LRF
    
  for (itt1.Initialize(mapFacLFac); itt1.More(); itt1.Next()) {
    const TopoDS_Shape& fac = itt1.Key();
    TopoDS_Face facref = TopoDS::Face(fac);
    const TopTools_ListOfShape& LFac = mapFacLFac.Find(fac);
    
    Standard_Integer n11 = LFac.Extent();
    if (n11 != 1) {
      TopTools_ListOfShape LWir;
      for(it2.Initialize(LFac); it2.More(); it2.Next()) {
	const TopoDS_Shape& fac1 = it2.Value();
	
	TopExp_Explorer exp;
	for (exp.Init(fac1,TopAbs_WIRE); exp.More(); exp.Next()) {
	  const TopoDS_Shape& wir = exp.Current();
	  LWir.Append(wir);
	}
      } // LFac
      //  listes des wires avec edges communes.
      Keep_Edge = Standard_False;
      TopTools_DataMapOfShapeListOfShape mapWirLWir;
      GroupShape(LWir,Keep_Edge,mapWirLWir);
      if (mapWirLWir.IsEmpty()) {
#ifdef OCCT_DEBUG
	if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Empty list of wires"<<endl;
#endif
	myModified = Standard_False;
	myDone = Standard_True;
	myLFF = myLRF;
	return;
      } 
      
//  boucle sur les listes des wires de 1 face de LRF
      TopTools_ListOfShape myFaceLIE,myFaceLEE,myFaceLME,myFaceLW;
      for (itt2.Initialize(mapWirLWir); itt2.More(); itt2.Next()) {
	const TopoDS_Shape& wir = itt2.Key();
	const TopTools_ListOfShape& LWir1 = mapWirLWir.Find(wir);
	
	Standard_Integer n22 = LWir1.Extent();
	if (n22 != 1) {	
//    boucle sur 1 liste des wires avec edges communes.
	  TopTools_ListOfShape LEdg;
	  for(it3.Initialize(LWir1); it3.More(); it3.Next()) {
	    const TopoDS_Shape& wir1 = it3.Value();
	    
	    TopExp_Explorer exp;
	    for (exp.Init(wir1,TopAbs_EDGE); exp.More(); exp.Next()) {
	      const TopoDS_Shape& edg = exp.Current();
	      LEdg.Append(edg);
	    }
	  } // LWir1
//    listes des edges avec edges communes.
	  Keep_Edge = Standard_True;
	  TopTools_DataMapOfShapeListOfShape mapEdgLEdg;
	  GroupShape(LEdg,Keep_Edge,mapEdgLEdg);
	  if (mapEdgLEdg.IsEmpty()) {
#ifdef OCCT_DEBUG
	    if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Empty list of edges"<<endl;
#endif
	    myModified = Standard_False;
	    myDone = Standard_True;
	    myLFF = myLRF;
	    return;
	  } 
	
//    Elimination selon logique pure
//    boucle sur les listes des egdes de 1 wire de 1 face de LRF
	  TopTools_ListOfShape myWireLE;
	  for (itt3.Initialize(mapEdgLEdg); itt3.More(); itt3.Next()) {
	    const TopoDS_Shape& edg = itt3.Key();
	    const TopTools_ListOfShape& LEdg1 = mapEdgLEdg.Find(edg);
	    Standard_Boolean OriReversed = Standard_False;
	    Standard_Boolean OriForward = Standard_False;
	    Standard_Boolean OriInternal = Standard_False;
	    Standard_Boolean OriExternal = Standard_False;
	    for(it4.Initialize(LEdg1); it4.More(); it4.Next()) {
	      const TopoDS_Shape& edg1 = it4.Value();
	      ori1 = edg1.Orientation();
	      if (ori1 == TopAbs_REVERSED) {
		if (OriReversed) {
#ifdef OCCT_DEBUG
		  if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Bad faces orientation"<<endl;
#endif
		  myModified = Standard_False;
		  myDone = Standard_True;
		  myLFF = myLRF;
		  return;
		}
		OriReversed = Standard_True;
	      }
	      else if (ori1 == TopAbs_FORWARD) {
		if (OriForward) {
#ifdef OCCT_DEBUG
		  if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Bad faces orientation"<<endl;
#endif
		  myModified = Standard_False;
		  myDone = Standard_True;
		  myLFF = myLRF;
		  return;
		}
		OriForward = Standard_True;
	      }
	      else if (ori1 == TopAbs_INTERNAL) {
		OriInternal = Standard_True;
	      }
	      else if (ori1 == TopAbs_EXTERNAL) {
		OriExternal = Standard_True;
	      }
	    } // LEdg1
	  
//      - Traitement edge selon orientation
//      On privilegie orientation selon 1) reversed ou forward
//                                      2) internal
//                                      3) external
//      pour traiter cas ou l'on a au moins 2 orientations differentes parmi
//           forward et reversed - interne - externe 
	
	    if (OriReversed || OriForward) {
	      if (OriReversed && OriForward) {
//		TopoDS_Shape& edg1 = edg.Oriented(TopAbs_INTERNAL);
		const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_INTERNAL);
		myLME.Append(edg1);
		myFaceLME.Append(edg1);
	      } else if (OriReversed) {
//		TopoDS_Shape& edg1 = edg.Oriented(TopAbs_REVERSED);
		const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_REVERSED);
		myWireLE.Append(edg1);
	      } else {
//		TopoDS_Shape& edg1 = edg.Oriented(TopAbs_FORWARD);
		const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_FORWARD);
		myWireLE.Append(edg1);
	      }
	    } 
	    else if (OriInternal) {
//	      TopoDS_Shape& edg1 = edg.Oriented(TopAbs_INTERNAL);
	      const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_INTERNAL);
	      myLIE.Append(edg1);
	      myFaceLIE.Append(edg1);
	    }
	    else if (OriExternal) {
//	      TopoDS_Shape& edg1 = edg.Oriented(TopAbs_EXTERNAL);
	      const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_EXTERNAL);
	      myLEE.Append(edg1);
	      myFaceLEE.Append(edg1);
	    } // Ori
	  } // mapEdgLEdg
	
//    Reconstrution de 1 wire de 1 face de LRF
//    Attention cas ou une liste de wire connectes conduit a plusieurs Wires
	  Standard_Integer number1 = myWireLE.Extent();
	  while (number1 > 0) {
	    BRepLib_MakeWire MW;
	    MW.Add(myWireLE);
	    if (!MW.IsDone()) {
#ifdef OCCT_DEBUG
	      if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Failure in making wire"<<endl;
#endif
	      myModified = Standard_False;
	      myDone = Standard_True;
	      myLFF = myLRF;
	      return;
	    }
	    
//    Astuce pour contourner Wire Not Closed
	    TopoDS_Wire W = MW.Wire();
	    BRepLib_MakeWire MW1(W);
	    W = MW1.Wire();
	    
	    myFaceLW.Append(W);	  
	    
	    TopExp_Explorer exp;
	    TopTools_MapOfShape M;
	    Standard_Integer nb = 0;
	    for (exp.Init(W,TopAbs_EDGE); exp.More(); exp.Next()) {
	      const TopoDS_Shape& edg3 = exp.Current();
	      M.Add(edg3);
	      nb++;
	    }
	    
	    if (nb == number1) {
	      number1 = 0 ;
	    }
	    else {
	      TopTools_ListOfShape ListEdge;
	      for(it3.Initialize(myWireLE); it3.More(); it3.Next()) {
		const TopoDS_Shape& edg2 = it3.Value();
		if (M.Add(edg2)) {
		  ListEdge.Append(edg2);
		}
	      }
	      myWireLE.Assign(ListEdge);
	      number1 = myWireLE.Extent();
	    } // nb
	  } // number
	}
	else {
	  myFaceLW.Append(wir);
	} // n2 =1
      } // mapWirLWir
      
//  Reconstrution de 1 face de LRF
      Handle(Geom_Surface) S = BRep_Tool::Surface(facref);
      if (S->DynamicType() == 
	  STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
	S = Handle(Geom_RectangularTrimmedSurface)::
	  DownCast(S)->BasisSurface();
      }
      BRepLib_MakeFace MF(S, Precision::Confusion());  

      for(it2.Initialize(myFaceLW); it2.More(); it2.Next()) {
	const TopoDS_Wire& wir1 = TopoDS::Wire(it2.Value());
	MF.Add(wir1);
      }

// Ajout des Edges Internes
//                 Externes
//                 Modifiees
      for (it2.Initialize(myFaceLIE); it2.More(); it2.Next()) {
	const TopoDS_Edge& edg1 = TopoDS::Edge(it2.Value());
	BRepLib_MakeWire MW(edg1);
//      MW.Add(edg1);
	const TopoDS_Wire& W = MW.Wire();
	MF.Add(W);
      }
      for (it2.Initialize(myFaceLEE); it2.More(); it2.Next()) {
	const TopoDS_Edge& edg1 = TopoDS::Edge(it2.Value());
	BRepLib_MakeWire MW(edg1);
//      MW.Add(edg1);
	const TopoDS_Wire& W = MW.Wire();
	MF.Add(W);
      }
      if (myInternal) {
	for (it2.Initialize(myFaceLME); it2.More(); it2.Next()) {
	  const TopoDS_Edge& edg1 = TopoDS::Edge(it2.Value());
	  BRepLib_MakeWire MW(edg1);
//	  MW.Add(edg1);
	  const TopoDS_Wire& W = MW.Wire();
	  MF.Add(W);
	}
      }

      if (!MF.IsDone()) {
#ifdef OCCT_DEBUG
	if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Failure in making face"<<endl;
#endif
	myModified = Standard_False;
	myDone = Standard_True;
	myLFF = myLRF;
	return;
      }
      TopoDS_Face F = MF.Face();
      if (Ori3dReversed) {
	F.Reverse();
      }
      myLFF.Append(F);
    }
    else {
      myLFF.Append(facref);
    } // n1 = 1
  } // mapFacLFac
 
  if (myLFF.IsEmpty()) {
#ifdef OCCT_DEBUG
    if (trc) cout<<" TopOpeBRepBuild_FuseFace::PerformFace : Empty list of fusionned faces"<<endl;
#endif
    myModified = Standard_False;
    myDone = Standard_True;
    myLFF = myLRF;
    return;
  }
  
  myModified = Standard_True;
  myDone = Standard_True;

#ifdef OCCT_DEBUG
  if (trc) cout << " TopOpeBRepBuild_FuseFace::PerformFace() : Done" << endl;
#endif
}

//=======================================================================
//function : PerformEdge
//purpose  : fusion des edges cosurfaciques, connexes par une ou 
//plusieurs aretes       
//=======================================================================

void TopOpeBRepBuild_FuseFace::PerformEdge()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc) cout << "TopOpeBRepBuild_FuseFace::PerformEdge()" << endl;
#endif
  TopTools_DataMapOfShapeListOfShape mapVerLEdg,mapTampon;

  TopTools_ListIteratorOfListOfShape it1;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itt1;
//  TopAbs_Orientation ori,ori1;
  
//Niveau 1
//boucle sur les listes des faces de 1 face de LRF
    
  for (it1.Initialize(myLFF); it1.More(); it1.Next()) {
    const TopoDS_Shape& fac = it1.Value();
      
    TopExp_Explorer expw;
    for (expw.Init(fac,TopAbs_WIRE); expw.More(); expw.Next()) {
      const TopoDS_Shape& wir = expw.Current();
   
      TopExp_Explorer expe;
      for (expe.Init(wir,TopAbs_EDGE); expe.More(); expe.Next()) {
	const TopoDS_Shape& edg = expe.Current();
      
	TopExp_Explorer expv;
	for (expv.Init(edg,TopAbs_VERTEX); expv.More(); expv.Next()) {
	  const TopoDS_Shape& ver = expv.Current();
	  if (!mapVerLEdg.IsBound(ver)) {
	    TopTools_ListOfShape LmapEdg;
	    LmapEdg.Append(edg);
	    mapVerLEdg.Bind(ver,LmapEdg);
	  }
	  else {
	    TopTools_ListOfShape& LmapEdg = mapVerLEdg.ChangeFind(ver);
	    LmapEdg.Append(edg);
	  }
	}
      }
    }
  }

//nettoyage du tableau mapVerLSh : shap1 : shap1 shap2 shap3
//On ne garde que les vertex qui appartiennent a - exactement 2 edges
//                                               - de meme support geometrique
  mapTampon = mapVerLEdg;
  mapVerLEdg.Clear();
 
  for (itt1.Initialize(mapTampon); itt1.More(); itt1.Next()) {
    const TopoDS_Shape& ver = itt1.Key();
    const TopTools_ListOfShape& LmapEdg = mapTampon.Find(ver);
    Standard_Integer number = LmapEdg.Extent();
    if (number == 2){
      it1.Initialize(LmapEdg);
      const TopoDS_Edge& edg1 = TopoDS::Edge(it1.Value());  
      it1.Next();
      const TopoDS_Edge& edg2 = TopoDS::Edge(it1.Value());      
      if (SameSupport(edg1,edg2)) {
	mapVerLEdg.Bind(ver,LmapEdg);
      }
    }
  }

//On regroupe ensemble tous les edges consecutifs et SameSupport
  TopTools_DataMapOfShapeListOfShape mapEdgLEdg;
  GroupEdge(mapVerLEdg,mapEdgLEdg);

//On construit les edges somme des edges consecutifs et SameSupport
  MakeEdge(mapEdgLEdg);

  myModified = Standard_True;
  myDone = Standard_True;

#ifdef OCCT_DEBUG
  if (trc) cout << " TopOpeBRepBuild_FuseFace::PerformEdge() : Done" << endl;
#endif
}

//=======================================================================
//function : ClearEdge
//purpose  : Nettoyage des Faces : Suppression des edges internes et externes     
//=======================================================================

void TopOpeBRepBuild_FuseFace::ClearEdge()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc) cout << "TopOpeBRepBuild_FuseFace::ClearEdge()" << endl;
#endif

  TopTools_ListIteratorOfListOfShape it1,it2;
  TopAbs_Orientation ori;
  TopTools_ListOfShape myLFFnew;
  
//Niveau 1
//boucle sur les listes des faces de 1 face de LRF
    
  for (it1.Initialize(myLFF); it1.More(); it1.Next()) {
    const TopoDS_Shape& fac = it1.Value();

    TopTools_ListOfShape myFaceLW;
    TopExp_Explorer expw;
    for (expw.Init(fac,TopAbs_WIRE); expw.More(); expw.Next()) {
      const TopoDS_Shape& wir = expw.Current();

      TopTools_ListOfShape myWireLE;	
      TopExp_Explorer expe;
      for (expe.Init(wir,TopAbs_EDGE); expe.More(); expe.Next()) {
	const TopoDS_Shape& edg = expe.Current();
	
//    Elimination selon des edges interne et externe

	ori = edg.Orientation();
	if (ori == TopAbs_INTERNAL) {
	  myLIE.Append(edg);
	}
	else if (ori == TopAbs_EXTERNAL) {
	  myLEE.Append(edg);
	}
	else {
	  myWireLE.Append(edg);
	}
      }
//    Fin Niveau 3 
//    Reconstrution de 1 wire de 1 face de LRF
      if (!myWireLE.IsEmpty()) {
	BRepLib_MakeWire MW;
	MW.Add(myWireLE);
	if (!MW.IsDone()) {
#ifdef OCCT_DEBUG
	  if (trc) cout<<" TopOpeBRepBuild_FuseFace::ClearEdge : Failure in making wire"<<endl;
#endif
	  myModified = Standard_False;
	  myDone = Standard_True;
	  myLFF = myLRF;
	  return;
	}
	
	//    Astuce pour contourner Wire Not Closed
	TopoDS_Wire W = MW.Wire();
	BRepLib_MakeWire MW1(W);
	W = MW1.Wire();
	
	myFaceLW.Append(W);
      }
    }
//  Fin Niveau 2
//  Reconstrution de 1 face de LRF
    if (myFaceLW.IsEmpty()) {
#ifdef OCCT_DEBUG
      if (trc) cout<<" TopOpeBRepBuild_FuseFace::ClearEdge : Empty list of wires"<<endl;
#endif
      myModified = Standard_False;
      myDone = Standard_True;
      myLFF = myLRF;
      return;
    }
    it2.Initialize(myFaceLW);
    const TopoDS_Wire& wir = TopoDS::Wire(it2.Value());
    const Standard_Boolean OnlyPlane = Standard_False;
    BRepLib_MakeFace MF(wir,OnlyPlane);

    it2.Next();    
    for( ; it2.More(); it2.Next()) {
      const TopoDS_Wire& wir1 = TopoDS::Wire(it2.Value());
      MF.Add(wir1);
    }
    if (!MF.IsDone()) {
#ifdef OCCT_DEBUG
      if (trc) cout<<" TopOpeBRepBuild_FuseFace::ClearEdge : Failure in making face"<<endl;
#endif
      myModified = Standard_False;
      myDone = Standard_True;
      myLFF = myLRF;
      return;
    }
    const TopoDS_Face& F = MF.Face();
    myLFFnew.Append(F);
  }
//Fin Niveau 1 
  if (myLFFnew.IsEmpty()) {
#ifdef OCCT_DEBUG
    if (trc) cout<<" TopOpeBRepBuild_FuseFace::ClearEdge : Empty list of fusionned faces"<<endl;
#endif
    myModified = Standard_False;
    myDone = Standard_True;
    myLFF = myLRF;
    return;
  }
  myLFF = myLFFnew;

  myModified = Standard_True;
  myDone = Standard_True;

#ifdef OCCT_DEBUG
  if (trc) cout << " TopOpeBRepBuild_FuseFace::ClearEdge() : Done" << endl;
#endif
}

//=======================================================================
//function : ClearVertex
//purpose  : Nettoyage des Faces : Suppression des vertex internes et externes     
//=======================================================================

void TopOpeBRepBuild_FuseFace::ClearVertex()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc) cout << "TopOpeBRepBuild_FuseFace::ClearVertex()" << endl;
#endif

#ifdef OCCT_DEBUG
  if (trc) cout << " TopOpeBRepBuild_FuseFace::ClearVertex() : Done" << endl;
#endif
}

//=======================================================================
//function : GroupShape
//purpose  : 
//=======================================================================

static void GroupShape(TopTools_ListOfShape& mylist,Standard_Boolean Keep_Edge, TopTools_DataMapOfShapeListOfShape& mymapShLSh)
{
  TopTools_ListIteratorOfListOfShape it,it1,it2;
  TopTools_DataMapOfShapeListOfShape mapEdgLSh,mapShLSh;
  TopTools_ListOfShape LmapSh4;
  TopAbs_Orientation ori;

// construction du tableau C=locmapEdgLSh : egde1 - shap1 shap2 shap3
// construction du tableau   locmapShLSh  : shap1 - shap1 shap2 shap3
  LmapSh4.Clear();
  for(it.Initialize(mylist); it.More(); it.Next()) {
    const TopoDS_Shape& shap1 = it.Value();
    TopTools_ListOfShape LmapSh;
    LmapSh.Append(shap1);

    mapShLSh.Bind(shap1,LmapSh);
    
    TopExp_Explorer expe;
    for (expe.Init(shap1,TopAbs_EDGE); expe.More(); expe.Next()) {
      const TopoDS_Shape& edg1 = expe.Current();
//    verification si Edge a prendre en compte
      ori = edg1.Orientation();
      Standard_Boolean Edge_OK = Standard_True;
      if (ori == TopAbs_INTERNAL || ori == TopAbs_EXTERNAL) {
	Edge_OK = Standard_False;
      }
      if (Edge_OK || Keep_Edge) {
	if (!mapEdgLSh.IsBound(edg1)) {
	  TopTools_ListOfShape LmapEdg;
	  LmapEdg.Append(shap1);
	  mapEdgLSh.Bind(edg1,LmapEdg);
	}
	else {
	  TopTools_ListOfShape& LmapEdg = mapEdgLSh.ChangeFind(edg1);
	  LmapEdg.Append(shap1);
	  
	  if (!Keep_Edge) {
	    
//          Recuperation premier shape de liste liee a edg1
	    it1.Initialize(LmapEdg);
	    const TopoDS_Shape& shap2 = it1.Value();
	    
//          Controle si premier shape et shape courant sont deja lies
	    TopTools_ListOfShape LmapSh1;
	    LmapSh1 = mapShLSh.Find(shap2);
	    for(it1.Initialize(LmapSh1); it1.More(); it1.Next()) {
	      const TopoDS_Shape& shap = it1.Value();
	      if (shap.IsSame(shap1)) {
		break;
	      }
	    }
//          Premier shape et Shape courant ne sont pas deja lies
	    if (!it1.More()){
	      const TopTools_ListOfShape& LmapSh11 = mapShLSh.Find(shap1);
	      const TopTools_ListOfShape& LmapSh2 = mapShLSh.Find(shap2);
	      TopTools_ListOfShape Lmap1;
	      TopTools_ListOfShape Lmap2;
	      Lmap1.Assign(LmapSh11);
	      Lmap2.Assign(LmapSh2);
	     
	      for(it2.Initialize(Lmap1); it2.More(); it2.Next()) {
		const TopoDS_Shape& shap = it2.Value();
		TopTools_ListOfShape& Lmap = mapShLSh.ChangeFind(shap);
		TopTools_ListOfShape Lmap3;
		Lmap3.Assign(Lmap2);
		Lmap.Append(Lmap3);
	      }
	      for(it2.Initialize(Lmap2); it2.More(); it2.Next()) {
		const TopoDS_Shape& shap = it2.Value();
		TopTools_ListOfShape& Lmap = mapShLSh.ChangeFind(shap);
		TopTools_ListOfShape Lmap3;
		Lmap3.Assign(Lmap1);
		Lmap.Append(Lmap3);
	      }
	    }
	  }
	}
      }
    }
  }
  
// nettoyage du tableau mapShLSh : shap1 : shap1 shap2 shap3
  mymapShLSh.Clear();

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itt;
  if (!Keep_Edge) {
    TopTools_MapOfShape M;
    for (itt.Initialize(mapShLSh); itt.More(); itt.Next()) {
      const TopoDS_Shape& shap1 = itt.Key();
      if (M.Add(shap1)) {
	const TopTools_ListOfShape& LmapSh = mapShLSh.Find(shap1);
	mymapShLSh.Bind(shap1,LmapSh);
	
	for(it1.Initialize(LmapSh); it1.More(); it1.Next()) {
	  const TopoDS_Shape& shap2 = it1.Value();
	  M.Add(shap2);
	}
      }
    }
  }
  else {
     mymapShLSh = mapEdgLSh;
  }
}

//=======================================================================
//function : GroupEdge
//purpose  : 
//=======================================================================

static void GroupEdge(TopTools_DataMapOfShapeListOfShape& mymapVerLEdg, TopTools_DataMapOfShapeListOfShape& mymapEdgLEdg)
{
  TopTools_ListIteratorOfListOfShape it1,it2;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itt;
  TopTools_DataMapOfShapeListOfShape mapEdgLEdg;

// construction du tableau C=locmapEdgLSh : egde1 - shap1 shap2 shap3
// construction du tableau   locmapShLSh  : shap1 - shap1 shap2 shap3
  for(itt.Initialize(mymapVerLEdg); itt.More(); itt.Next()) {
    const TopoDS_Shape& ver1 = itt.Key();
    TopTools_ListOfShape LmapEdg;
    LmapEdg = mymapVerLEdg.Find(ver1);

    it1.Initialize(LmapEdg);
    const TopoDS_Edge& edg1 = TopoDS::Edge(it1.Value());  
    it1.Next();
    const TopoDS_Edge& edg2 = TopoDS::Edge(it1.Value());

    Standard_Boolean Edge1Add,Edge2Add;
    TopoDS_Edge edgold,edgnew;
    if (mapEdgLEdg.IsBound(edg1)) {
      Edge1Add = Standard_False;
      edgold = edg1;
    } else {
      Edge1Add = Standard_True;
      edgnew = edg1;
    }
    if (mapEdgLEdg.IsBound(edg2)) {
      Edge2Add = Standard_False;
      edgold = edg2;
    } else {
      Edge2Add = Standard_True;
      edgnew = edg2;
    }

    if (!(Edge1Add || Edge2Add)) {
      continue;
    }
    else if (Edge1Add && Edge2Add) {
      mapEdgLEdg.Bind(edg1,LmapEdg);
      mapEdgLEdg.Bind(edg2,LmapEdg);
    }
    else {
 

//    Recuperation premier shape de liste liee a edg1 et mise a jour  

      TopTools_ListOfShape LmapEdg11;
      LmapEdg11.Append(edgnew);
      mapEdgLEdg.Bind(edgnew,LmapEdg11);

      TopTools_ListOfShape LmapEdg1;
      LmapEdg1 = mapEdgLEdg.Find(edgold);
   
      for(it2.Initialize(LmapEdg1); it2.More(); it2.Next()) {
	const TopoDS_Shape& edg22 = it2.Value();
	TopTools_ListOfShape& LmapEdg2 = mapEdgLEdg.ChangeFind(edgnew);
	LmapEdg2.Append(edg22);
	TopTools_ListOfShape& LmapEdg3 = mapEdgLEdg.ChangeFind(edg22);
	LmapEdg3.Append(edgnew);
      }
    }
  }

// nettoyage du tableau mapEdgLedg : edg1 : edg1 edg2 edg3
  
  TopTools_MapOfShape M;

  for (itt.Initialize(mapEdgLEdg); itt.More(); itt.Next()) {
    const TopoDS_Shape& edg1 = itt.Key();
    if (M.Add(edg1)) {
      const TopTools_ListOfShape& LmapEdg = mapEdgLEdg.Find(edg1);
      mymapEdgLEdg.Bind(edg1,LmapEdg);
      
      for(it1.Initialize(LmapEdg); it1.More(); it1.Next()) {
	const TopoDS_Shape& edg2 = it1.Value();
	M.Add(edg2);
      }
    }
  }
}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

static void MakeEdge(TopTools_DataMapOfShapeListOfShape& mymapEdgLEdg)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepBuild_GettraceFUFA();
#endif

  TopTools_ListIteratorOfListOfShape it;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itt1;
  TopTools_DataMapIteratorOfDataMapOfShapeInteger itt2;
  TopTools_DataMapOfShapeListOfShape mapEdgLEdg;

// construction du tableau C=locmapEdgLSh : egde1 - shap1 shap2 shap3
// construction du tableau   locmapShLSh  : shap1 - shap1 shap2 shap3
  for(itt1.Initialize(mymapEdgLEdg); itt1.More(); itt1.Next()) {
    const TopoDS_Shape& edg1 = itt1.Key();
    TopTools_ListOfShape LmapEdg;
    LmapEdg = mymapEdgLEdg.Find(edg1);
    TopTools_DataMapOfShapeInteger mapVerInt;

    Standard_Integer VertexExtrem;
    TopoDS_Vertex V1,V2;
    for(it.Initialize(LmapEdg); it.More(); it.Next()) {
      const TopoDS_Edge& edg2 = TopoDS::Edge(it.Value());

      TopExp_Explorer expv;
      for (expv.Init(edg2,TopAbs_VERTEX); expv.More(); expv.Next()) {
	const TopoDS_Shape& ver = expv.Current();

	VertexExtrem = 1;
	if (mapVerInt.IsBound(ver)) {
	  VertexExtrem = 0;
	}
	mapVerInt.Bind(ver,VertexExtrem);
	
      }
    }

    TopTools_ListOfShape myEdgeLV,myEdgeLMV;
    for(itt2.Initialize(mapVerInt); itt2.More(); itt2.Next()) {
      const TopoDS_Shape& ver = itt2.Key();
      VertexExtrem =  mapVerInt.Find(ver);
      if (VertexExtrem == 1) {
	myEdgeLV.Append(ver);
      }
      else {
//	TopoDS_Shape& ver1 = ver.Oriented(TopAbs_INTERNAL);
	const TopoDS_Shape& ver1 = ver.Oriented(TopAbs_INTERNAL);
	myEdgeLMV.Append(ver1);
      }
    }
    Standard_Integer number = myEdgeLV.Extent();
    if (!(number == 2)){
#ifdef OCCT_DEBUG
      if (trc) cout<<" TopOpeBRepBuild_FuseFace::MakeEdge : Failure in reconstructing new edge"<<endl;
#endif
      return;
    }
    it.Initialize(myEdgeLV);
    const TopoDS_Vertex& ver1 = TopoDS::Vertex(it.Value()); 
//    TopoDS_Shape& verf = ver1.Oriented(TopAbs_FORWARD); 
    const TopoDS_Shape& verf = ver1.Oriented(TopAbs_FORWARD); 
    it.Next();
    const TopoDS_Vertex& ver2 = TopoDS::Vertex(it.Value());
//    TopoDS_Shape& verl = ver2.Oriented(TopAbs_FORWARD);
    const TopoDS_Shape& verl = ver2.Oriented(TopAbs_FORWARD);

    Handle(Geom_Curve) curv;
    const TopoDS_Edge& edg = TopoDS::Edge(edg1);
    TopLoc_Location loc;
    Standard_Real first,last;
    curv = BRep_Tool::Curve(edg,loc,first,last);

    BRepLib_MakeEdge ME(curv,TopoDS::Vertex(verf),TopoDS::Vertex(verl));
    const TopoDS_Edge& edgnew = ME.Edge();

//    if (myInternal) {
//      for (it.Initialize(myEdgeLMV); it.More(); it.Next()) {
//	const TopoDS_Vertex& ver1 = TopoDS::Vertex(it.Value());
//	BRep_Builder B;
//	B.MakeEdge(edgnew);
//	B.Add(edgnew,ver1);
//      }
//    }
    mapEdgLEdg.Bind(edgnew,LmapEdg);
    
  }
  mymapEdgLEdg = mapEdgLEdg;
}

//=======================================================================
//function : SameSupport
//purpose  : Edges SameSupport ou pas
//=======================================================================

Standard_Boolean SameSupport(const TopoDS_Edge& E1,
			     const TopoDS_Edge& E2)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepBuild_GettraceFUFA();
#endif

  if (E1.IsNull() || E2.IsNull()) {
    return Standard_False;
  }


  Handle(Geom_Curve) C1,C2;
  TopLoc_Location loc;
  Standard_Real f1,l1,f2,l2;
  Handle(Standard_Type) typC1,typC2;
  
  C1 = BRep_Tool::Curve(E1,loc,f1,l1);
  if (!loc.IsIdentity()) {
    Handle(Geom_Geometry) GG1 = C1->Transformed(loc.Transformation());
    C1 = Handle(Geom_Curve)::DownCast (GG1);
  }
  C2 = BRep_Tool::Curve(E2,loc,f2,l2);
  if (!loc.IsIdentity()) {
    Handle(Geom_Geometry) GG2 = C2->Transformed(loc.Transformation());
    C2 = Handle(Geom_Curve)::DownCast (GG2);
  }
  
  typC1 = C1->DynamicType();
  typC2 = C2->DynamicType();
  
  if (typC1 == STANDARD_TYPE(Geom_TrimmedCurve)) {
    C1 =  Handle(Geom_TrimmedCurve)::DownCast (C1)->BasisCurve();
    typC1 = C1->DynamicType();
  }
  
  if (typC2 == STANDARD_TYPE(Geom_TrimmedCurve)) {
    C2 =  Handle(Geom_TrimmedCurve)::DownCast (C2)->BasisCurve();
    typC2 = C2->DynamicType();
  }
  
  if (typC1 != typC2) {
    return Standard_False;
  }
  
  if (typC1 != STANDARD_TYPE(Geom_Line) &&
      typC1 != STANDARD_TYPE(Geom_Circle) &&
      typC1 != STANDARD_TYPE(Geom_Ellipse) &&
      typC1 != STANDARD_TYPE(Geom_BSplineCurve) && 
      typC1 != STANDARD_TYPE(Geom_BezierCurve)) {
#ifdef OCCT_DEBUG
    if (trc) cout << " TopOpeBRepBuild_FuseFace : Type de Support non traite" << endl;
#endif
    return Standard_False;
  }

  // On a presomption de confusion
  const Standard_Real tollin = Precision::Confusion();
  const Standard_Real tolang = Precision::Angular();
  if (typC1 == STANDARD_TYPE(Geom_Line)) {
    gp_Lin li1( Handle(Geom_Line)::DownCast (C1)->Lin());
    gp_Lin li2( Handle(Geom_Line)::DownCast (C2)->Lin());
    
    if (Abs(li1.Angle(li2)) <= tolang &&
	li1.Location().SquareDistance(li2.Location()) <= tollin*tollin) {
      return Standard_True;
    }
    return Standard_False;
  } 
  else if (typC1 == STANDARD_TYPE(Geom_Circle)) {
    gp_Circ ci1 = Handle(Geom_Circle)::DownCast (C1)->Circ();
    gp_Circ ci2 = Handle(Geom_Circle)::DownCast (C2)->Circ();
    if (Abs(ci1.Radius()-ci2.Radius()) <= tollin &&
	ci1.Location().SquareDistance(ci2.Location()) <= tollin*tollin) {
      // Point debut, calage dans periode, et detection meme sens
      return Standard_True;
    }
    return Standard_False;
  }
  else if (typC1 == STANDARD_TYPE(Geom_Ellipse)) {
    gp_Elips ci1 = Handle(Geom_Ellipse)::DownCast (C1)->Elips();
    gp_Elips ci2 = Handle(Geom_Ellipse)::DownCast (C2)->Elips();
    
    if (Abs(ci1.MajorRadius()-ci2.MajorRadius()) <= tollin &&
	Abs(ci1.MinorRadius()-ci2.MinorRadius()) <= tollin &&
	ci1.Location().SquareDistance(ci2.Location()) <= tollin*tollin) {
      // Point debut, calage dans periode, et detection meme sens
      return Standard_True;
    }
    return Standard_False;
  }
  else if (typC1 == STANDARD_TYPE(Geom_BSplineCurve)) {
    Handle(Geom_BSplineCurve) B1 = Handle(Geom_BSplineCurve)::DownCast (C1);
    Handle(Geom_BSplineCurve) B2 = Handle(Geom_BSplineCurve)::DownCast (C2);
   
    Standard_Integer nbpoles = B1->NbPoles();
    if (nbpoles != B2->NbPoles()) {
      return Standard_False;
    }
   
    Standard_Integer nbknots = B1->NbKnots();
    if (nbknots != B2->NbKnots()) {
      return Standard_False;
    }
   
    TColgp_Array1OfPnt P1(1, nbpoles), P2(1, nbpoles);
    B1->Poles(P1);
    B2->Poles(P2);
   
    Standard_Real tol3d = BRep_Tool::Tolerance(E1);
    for (Standard_Integer p = 1; p <= nbpoles; p++) {
      if ( (P1(p)).Distance(P2(p)) > tol3d) {
	return Standard_False;
      }
    }
   
    TColStd_Array1OfReal K1(1, nbknots), K2(1, nbknots);
    B1->Knots(K1);
    B2->Knots(K2);
   
    TColStd_Array1OfInteger M1(1, nbknots), M2(1, nbknots);
    B1->Multiplicities(M1);
    B2->Multiplicities(M2);
   
    for (Standard_Integer k = 1; k <= nbknots; k++) {
      if ((K1(k)-K2(k)) > tollin) {
	return Standard_False;
      }
      if (Abs(M1(k)-M2(k)) > tollin) {
	return Standard_False;
      }
    }
   
    if (!B1->IsRational()) {
      if (B2->IsRational()) {
	return Standard_False;
      }
    }
    else {
      if (!B2->IsRational()) {
	return Standard_False;
      }
    }
    
    if (B1->IsRational()) {
      TColStd_Array1OfReal W1(1, nbpoles), W2(1, nbpoles);
      B1->Weights(W1);
      B2->Weights(W2);
   
      for (Standard_Integer w = 1; w <= nbpoles; w++) {
	if (Abs(W1(w)-W2(w)) > tollin) {
	  return Standard_False;
	}
      }
    }
    return Standard_True;
  }
  else if (typC1 == STANDARD_TYPE(Geom_BezierCurve)) {
    Handle(Geom_BezierCurve) B1 = Handle(Geom_BezierCurve)::DownCast (C1);
    Handle(Geom_BezierCurve) B2 = Handle(Geom_BezierCurve)::DownCast (C2);
    
    Standard_Integer nbpoles = B1->NbPoles();
    if (nbpoles != B2->NbPoles()) {
      return Standard_False;
    }
    
    TColgp_Array1OfPnt P1(1, nbpoles), P2(1, nbpoles);
    B1->Poles(P1);
    B2->Poles(P2);
    
    for (Standard_Integer p = 1; p <= nbpoles; p++) {
      if ( (P1(p)).Distance(P2(p)) > tollin) {
	return Standard_False;
      }
    }
    
    if (!B1->IsRational()) {
      if (B2->IsRational()) {
	return Standard_False;
      }
    }
    else {
      if (!B2->IsRational()) {
	return Standard_False;
      }
    }
    
    if (B1->IsRational()) {
      TColStd_Array1OfReal W1(1, nbpoles), W2(1, nbpoles);
      B1->Weights(W1);
      B2->Weights(W2);
      
      for (Standard_Integer w = 1; w <= nbpoles; w++) {
	if (Abs(W1(w)-W2(w)) > tollin) {
	  return Standard_False;
	}
      }
    }
    return Standard_True;
  }
  return Standard_False;
}
