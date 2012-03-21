// Created on: 1995-12-12
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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



#include <BRepCheck_Shell.ixx>

#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <TopExp_Explorer.hxx>

#include <BRepCheck.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp.hxx>

#include <TopTools_DataMapIteratorOfDataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>


Standard_EXPORT Standard_Integer BRepCheck_Trace(const Standard_Integer phase) {
  static int BRC_Trace = 0;
  if (phase < 0) BRC_Trace =0;
  else if (phase > 0) BRC_Trace=phase;
  return BRC_Trace;
}

void PrintShape(const TopoDS_Shape& theShape, const Standard_Integer upper) {
  if (!theShape.IsNull()) {
    Standard_Integer code = theShape.HashCode(upper);
    
    switch (theShape.ShapeType()) {
    case TopAbs_COMPOUND :
      cout << "COMPOUND";
      break;
    case TopAbs_COMPSOLID :
      cout << "COMPSOLID";
      break;
    case TopAbs_SOLID :
      cout << "SOLID";
      break;
    case TopAbs_SHELL :
      cout << "SHELL";
      break;
    case TopAbs_FACE :
      cout << "FACE";
      break;
    case TopAbs_WIRE :
      cout << "WIRE";
      break;
    case TopAbs_EDGE :
      cout << "EDGE";
      break;
    case TopAbs_VERTEX :
      cout << "VERTEX";
      break;
    case TopAbs_SHAPE :
      cout << "SHAPE";
      break;
    }
    cout << " : " << code << " ";
    switch (theShape.Orientation()) {
    case TopAbs_FORWARD :
      cout << "FORWARD";
      break;
    case TopAbs_REVERSED :
      cout << "REVERSED";
      break;
    case TopAbs_INTERNAL :
      cout << "INTERNAL";
      break;
    case TopAbs_EXTERNAL :
      cout << "EXTERNAL";
      break;
    }
    cout << endl;
  }
}
    
static void Propagate(const TopTools_IndexedDataMapOfShapeListOfShape&,
		      const TopoDS_Shape&,   // Face
		      TopTools_MapOfShape&);  // mapofface


#ifdef DEB
static TopAbs_Orientation GetOrientation(const TopoDS_Face&,
					 const TopoDS_Shape&);
#endif


inline Standard_Boolean IsOriented(const TopoDS_Shape& S)
{
  return (S.Orientation() == TopAbs_FORWARD ||
	  S.Orientation() == TopAbs_REVERSED);
}


//=======================================================================
//function : BRepCheck_Shell
//purpose  : 
//=======================================================================

BRepCheck_Shell::BRepCheck_Shell(const TopoDS_Shell& S)
{
  Init(S);
}


//=======================================================================
//function : Minimum
//purpose  : 
//=======================================================================

void BRepCheck_Shell::Minimum()
{
  myCdone = Standard_False;
  myOdone = Standard_False;

  if (!myMin) {
    BRepCheck_ListOfStatus thelist;
    myMap.Bind(myShape, thelist);
    BRepCheck_ListOfStatus& lst = myMap(myShape);

    // it is checked if the shell is "connected"
    TopExp_Explorer exp(myShape,TopAbs_FACE);
    Standard_Integer nbface = 0;
    myMapEF.Clear();
    for (; exp.More(); exp.Next()) {
      nbface++;
      TopExp_Explorer expe;
      for (expe.Init(exp.Current(),TopAbs_EDGE); 
	   expe.More(); expe.Next()) {
	const TopoDS_Shape& edg = expe.Current();
	Standard_Integer index = myMapEF.FindIndex(edg);
	if (index == 0) {
	  TopTools_ListOfShape thelist1;
	  index = myMapEF.Add(edg, thelist1);
	}
	myMapEF(index).Append(exp.Current());
      }
    }

    if (nbface == 0) {
      BRepCheck::Add(lst,BRepCheck_EmptyShell);
    }
    else if (nbface >= 2) {
      TopTools_MapOfShape mapF;
      exp.ReInit();
      Propagate(myMapEF,exp.Current(),mapF);
      if (mapF.Extent() != nbface)  {
	BRepCheck::Add(lst,BRepCheck_NotConnected);
      }
    }
    if (lst.IsEmpty()) {
      lst.Append(BRepCheck_NoError);
    }
    myMapEF.Clear();
    myMin = Standard_True;
  }
}



//=======================================================================
//function : InContext
//purpose  : 
//=======================================================================

void BRepCheck_Shell::InContext(const TopoDS_Shape& S)
{

  if (myMap.IsBound(S)) {
    return;
  }
  BRepCheck_ListOfStatus thelist;
  myMap.Bind(S, thelist);

  BRepCheck_ListOfStatus& lst = myMap(S);

//  for (TopExp_Explorer exp(S,TopAbs_SHELL); exp.More(); exp.Next()) {
  TopExp_Explorer exp(S,TopAbs_SHELL) ;
  for ( ; exp.More(); exp.Next()) {
    if (exp.Current().IsSame(myShape)) {
      break;
    }
  }
  if (!exp.More()) {
    BRepCheck::Add(lst,BRepCheck_SubshapeNotInShape);
    return;
  }

  TopAbs_ShapeEnum styp = S.ShapeType();
  switch (styp) {

  case TopAbs_SOLID:
    {
      BRepCheck_Status fst = Closed();
      if ((fst == BRepCheck_NotClosed && S.Closed()) ||
	  (fst != BRepCheck_NoError)) {
	BRepCheck::Add(lst,fst);
      }
      else if (!IsUnorientable()) {
	fst = Orientation();
	BRepCheck::Add(lst,fst);
      }
    }
    break;

  default:
    break;
  }


  if (lst.IsEmpty()) {
    lst.Append(BRepCheck_NoError);
  }
}


//=======================================================================
//function : Blind
//purpose  : 
//=======================================================================

void BRepCheck_Shell::Blind()
{
  if (!myBlind) {
    // nothing more than in the minimum
    myBlind = Standard_True;
  }
}


//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

BRepCheck_Status BRepCheck_Shell::Closed(const Standard_Boolean Update)
{

  if (myCdone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape), myCstat);
    }
    return myCstat;
  }

  myCdone = Standard_True; // it will be done...

  BRepCheck_ListIteratorOfListOfStatus itl(myMap(myShape));
  if (itl.Value() != BRepCheck_NoError) {
    myCstat = itl.Value();
    return myCstat; // already saved
  }

  myCstat = BRepCheck_NoError;
  //
  Standard_Integer index, aNbF;
  TopExp_Explorer exp, ede;
  TopTools_MapOfShape mapS, aMEToAvoid;
  myMapEF.Clear();
  

  // Checks if the oriented faces of the shell give a "closed" shell,
  // i-e if each oriented edge on oriented faces is found 2 times.
  //
  //modified by NIZNHY-PKV Mon Jun  4 13:59:21 2007f
  exp.Init(myShape,TopAbs_FACE);
  for (; exp.More(); exp.Next()) {
    const TopoDS_Shape& aF=exp.Current();
    if (IsOriented(aF)) {
      ede.Init(exp.Current(),TopAbs_EDGE);
      for (; ede.More(); ede.Next()) {
	const TopoDS_Shape& aE=ede.Current();
	if (!IsOriented(aE)) {
	  aMEToAvoid.Add(aE);
	}
      }
    }
  }
  //modified by NIZNHY-PKV Mon Jun  4 13:59:23 2007t
  //
  exp.Init(myShape,TopAbs_FACE);
  for (; exp.More(); exp.Next()) {
    const TopoDS_Shape& aF=exp.Current();
    if (IsOriented(aF)) {
      if (!mapS.Add(aF)) {
	myCstat = BRepCheck_RedundantFace;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myCstat);
	}
	return myCstat;
      }
      //
      ede.Init(exp.Current(),TopAbs_EDGE);
      for (; ede.More(); ede.Next()) {
	const TopoDS_Shape& aE=ede.Current();
	//modified by NIZNHY-PKV Mon Jun  4 14:07:57 2007f
	//if (IsOriented(aE)) {
	if (!aMEToAvoid.Contains(aE)) {
	  //modified by NIZNHY-PKV Mon Jun  4 14:08:01 2007
	  index = myMapEF.FindIndex(aE);
	  if (!index) {
	    TopTools_ListOfShape thelist;
	    index = myMapEF.Add(aE, thelist);
	  }
	  myMapEF(index).Append(aF);
	}
      }
    }
  }
  //
  myNbori = mapS.Extent();
  if (myNbori >= 2) {
    mapS.Clear();
    // Search for the first oriented face
    TopoDS_Shape aF;
    exp.Init(myShape, TopAbs_FACE);
    for (;exp.More(); exp.Next()) {
      aF=exp.Current();
      if (IsOriented(aF)) {
	break;
      }
    }
    //
    Propagate(myMapEF, aF, mapS);
  }
  //
  //
  aNbF=mapS.Extent();
  if (myNbori != aNbF) {
    myCstat = BRepCheck_NotConnected;
    if (Update) {
      BRepCheck::Add(myMap(myShape),myCstat);
    }
    return myCstat;
  }
  //
  //
  Standard_Integer i, Nbedges, nboc, nbSet;
  //
  Nbedges = myMapEF.Extent();
  for (i = 1; i<=Nbedges; ++i) {
    nboc = myMapEF(i).Extent();
    if (nboc == 0 || nboc >= 3) {
      TopTools_ListOfShape theSet;
      nbSet=NbConnectedSet(theSet);
      // If there is more than one closed cavity the shell is considered invalid
      // this corresponds to the criteria of a solid (not those of a shell)
      if (nbSet>1) {
	myCstat = BRepCheck_InvalidMultiConnexity;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myCstat);
	}
	return myCstat;
      }
    }
    else if (nboc == 1) {
      if (!BRep_Tool::Degenerated(TopoDS::Edge(myMapEF.FindKey(i)))) {
	myCstat=BRepCheck_NotClosed;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myCstat);
	}
	return myCstat;
      }
    }
  }
  
  if (Update) {
    BRepCheck::Add(myMap(myShape),myCstat);
  }
  return myCstat;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

BRepCheck_Status BRepCheck_Shell::Orientation(const Standard_Boolean Update)
{
  if (myOdone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape), myOstat);
    }
    return myOstat;
  }
  myOdone = Standard_True;

  myOstat = Closed();
  if (myOstat != BRepCheck_NotClosed && myOstat != BRepCheck_NoError) {
    if (Update) {
      BRepCheck::Add(myMap(myShape), myOstat);
    }
    return myOstat;
  }

  myOstat = BRepCheck_NoError;


// First the orientation of each face in relation to the shell is found.
// It is used to check BRepCheck_RedundantFace

  TopTools_DataMapOfShapeInteger MapOfShapeOrientation;
  TopExp_Explorer exp,ede;

  for (exp.Init(myShape,TopAbs_FACE); exp.More(); exp.Next()) {
    if (!MapOfShapeOrientation.Bind(exp.Current(), (Standard_Integer)(exp.Current().Orientation()))) {
      myOstat = BRepCheck_RedundantFace;
      if (Update) {
	BRepCheck::Add(myMap(myShape), myOstat);
      }
      else {
	return myOstat;
      }
    }
  }

#ifdef DEB
  if (BRepCheck_Trace(0) > 1) {
    TopTools_DataMapIteratorOfDataMapOfShapeInteger itt(MapOfShapeOrientation);
    Standard_Integer upper = MapOfShapeOrientation.NbBuckets();
    cout << "La map shape Orientation :" << endl;
    for (; itt.More(); itt.Next()) {
      PrintShape(itt.Key(), upper);
    }
    cout << endl;
  }
#endif


// Then the orientation of faces by their connectivity is checked
// BRepCheck_BadOrientationOfSubshape and 
//         BRepCheck_SubshapeNotInShape are checked;

  Standard_Integer Nbedges = myMapEF.Extent();
  TopoDS_Face Fref;
  TopAbs_Orientation orf;

  for (Standard_Integer i = 1; i<= Nbedges; i++) {

    const TopoDS_Edge& edg = TopoDS::Edge(myMapEF.FindKey(i));
    if (BRep_Tool::Degenerated(edg)) continue;
    TopTools_ListOfShape& lface = myMapEF(i);
    TopTools_ListIteratorOfListOfShape lite(lface);

    if (lface.Extent() <= 2)
      {
	lite.Initialize(lface);
	Fref = TopoDS::Face(lite.Value());
	
	if (!MapOfShapeOrientation.IsBound(Fref)) {
	  myOstat = BRepCheck_SubshapeNotInShape;
	  if (Update) {
	    BRepCheck::Add(myMap(myShape), myOstat);
	    }
	  // quit because no workaround for the incoherence is possible
	  return myOstat;
	}
	lite.Next();
	
	if (lite.More()) { // Edge of connectivity
	  //JR/Hp :
	  Standard_Integer iorf = MapOfShapeOrientation.Find(Fref);
	  orf = (TopAbs_Orientation) iorf;
	  //orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fref);
	  Fref.Orientation(orf);
	  
	  // edge is examined
	  if (!lite.Value().IsSame(Fref)) { // edge non "closed"
	    for (ede.Init(Fref,TopAbs_EDGE); ede.More(); ede.Next()) {
	      if (ede.Current().IsSame(edg)) {
		break;
	      }
	    }
	    TopAbs_Orientation orient = ede.Current().Orientation();
	    TopoDS_Face Fcur= TopoDS::Face(lite.Value());
	    
	    if (!MapOfShapeOrientation.IsBound(Fcur)) {
	      myOstat = BRepCheck_SubshapeNotInShape;
	      if (Update) {
		BRepCheck::Add(myMap(myShape), myOstat);
		}
	      // quit because no workaround for the incoherence is possible
	      return myOstat;
	    }
	    
	    //JR/Hp :
	    Standard_Integer iorf = MapOfShapeOrientation.Find(Fcur) ;
	    orf = (TopAbs_Orientation) iorf ;
	    //	orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
	    Fcur.Orientation(orf);
	    
	    for (ede.Init(Fcur, TopAbs_EDGE); ede.More(); ede.Next()) {
	      if (ede.Current().IsSame(edg)) {
		break;
	      }
	    }
	    if (ede.Current().Orientation() == orient) {
	      // The loop is continued on the edges as many times 
	      // as the same edge is present in the wire

	      // modified by NIZHNY-MKK  Tue Sep 30 11:11:42 2003
	      Standard_Boolean bfound = Standard_False;
	      ede.Next();
	      for (; ede.More(); ede.Next()) {
		if (ede.Current().IsSame(edg)) {
		  // modified by NIZHNY-MKK  Tue Sep 30 11:12:03 2003
		  bfound = Standard_True;
		  break;
		}
	      }
	      // 	      if (ede.Current().Orientation() == orient) {
	      // modified by NIZHNY-MKK  Thu Oct  2 17:56:47 2003
	      if (!bfound || (ede.Current().Orientation() == orient)) {
		myOstat = BRepCheck_BadOrientationOfSubshape;
		if (Update) {
		  BRepCheck::Add(myMap(myShape), myOstat);
		    break;
		  }
		return myOstat;
	      }
	    }
	  }
	}
      }
    else //more than two faces
      {
	Standard_Integer numF = 0, numR = 0;
	TopTools_MapOfShape Fmap;

	for (lite.Initialize(lface); lite.More(); lite.Next())
	  {
	    TopoDS_Face Fcur= TopoDS::Face(lite.Value());
	    if (!MapOfShapeOrientation.IsBound(Fcur))
	      {
		myOstat = BRepCheck_SubshapeNotInShape;
		if (Update)
		  BRepCheck::Add(myMap(myShape), myOstat);
	      // quit because no workaround for the incoherence is possible
		return myOstat;
	      }

	    Standard_Integer iorf = MapOfShapeOrientation.Find(Fcur);
	    orf = (TopAbs_Orientation) iorf;
	    //orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
	    Fcur.Orientation(orf);

	    for (ede.Init(Fcur,TopAbs_EDGE); ede.More(); ede.Next())
	      if (ede.Current().IsSame(edg))
		break;
	    if (Fmap.Contains(Fcur)) //edge is "closed" on Fcur, we meet Fcur twice
	      {
		ede.Next();
		for (; ede.More(); ede.Next())
		  if (ede.Current().IsSame(edg))
		    break;
	      }
	    TopAbs_Orientation orient = ede.Current().Orientation();
	    if (orient == TopAbs_FORWARD)
	      numF++;
	    else
	      numR++;

	    Fmap.Add(Fcur);
	  }

	if (numF != numR)
	  {
	    myOstat = BRepCheck_BadOrientationOfSubshape;
	    if (Update)
	      {
		BRepCheck::Add(myMap(myShape), myOstat);
		break;
	      }
	    return myOstat;
	  }
      }
  }

// If at least one incorrectly oriented face has been found, it is checked if the shell can be oriented. 
//          i.e. : if by modification of the orientation of a face it is possible to find 
//          a coherent orientation. (it is not possible on a Moebius band)
//          BRepCheck_UnorientableShape is checked

  if (myOstat == BRepCheck_BadOrientationOfSubshape) {
    if (!Fref.IsNull()) {
      if (Nbedges > 0) {
	TopTools_MapOfShape alre;
	TopTools_ListOfShape voisin;
	voisin.Append(Fref);
	alre.Clear();
	while (!voisin.IsEmpty()) {
	  Fref=TopoDS::Face(voisin.First());
	  voisin.RemoveFirst();
	  if (!MapOfShapeOrientation.IsBound(Fref)) {
	    myOstat = BRepCheck_SubshapeNotInShape;
	    if (Update) {
	      BRepCheck::Add(myMap(myShape), myOstat);
	    }
	    // quit because no workaround for the incoherence is possible
	    return myOstat;
	  }
//JR/Hp :
          Standard_Integer iorf = MapOfShapeOrientation.Find(Fref) ;
	  orf = (TopAbs_Orientation) iorf ;
//	  orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fref);
	  Fref.Orientation(orf);

#ifdef DEB
  if (BRepCheck_Trace(0) > 3) {
    cout << "Fref : " ;
    PrintShape(Fref, MapOfShapeOrientation.NbBuckets());
  }
#endif

	  TopExp_Explorer edFcur;
	  alre.Add(Fref);

	  for (ede.Init(Fref,TopAbs_EDGE); ede.More(); ede.Next()) {
	    const TopoDS_Edge& edg = TopoDS::Edge(ede.Current());
	    TopAbs_Orientation orient = edg.Orientation();
	    TopTools_ListOfShape& lface = myMapEF.ChangeFromKey(edg);
	    TopTools_ListIteratorOfListOfShape lite(lface);
	  
	    TopoDS_Face Fcur= TopoDS::Face(lite.Value());
	    if (Fcur.IsSame(Fref)) {
	      lite.Next();
	      if (lite.More()) {
		Fcur=TopoDS::Face(lite.Value());
	      }
	      else {
		// from the free border one goes to the next edge
		continue;
	      }
	    }

	    if (!MapOfShapeOrientation.IsBound(Fcur)) {
	      myOstat = BRepCheck_SubshapeNotInShape;
	      if (Update) {
		BRepCheck::Add(myMap(myShape), myOstat);
	      }
	      // quit because no workaround for the incoherence is possible
	      return myOstat;
	    }

//JR/Hp :
            Standard_Integer iorf = MapOfShapeOrientation.Find(Fcur) ;
	    orf = (TopAbs_Orientation) iorf ;
//	    orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
	    Fcur.Orientation(orf);

#ifdef DEB
  if (BRepCheck_Trace(0) > 3) {
    cout << "    Fcur : " ;
    PrintShape(Fcur, MapOfShapeOrientation.NbBuckets());
  }
#endif
	    for (edFcur.Init(Fcur, TopAbs_EDGE); edFcur.More(); edFcur.Next()) {
	      if (edFcur.Current().IsSame(edg)) {
		break;
	      }
	    }
	    if (edFcur.Current().Orientation() == orient) {
	      if (alre.Contains(Fcur)) {
		// It is necessary to return a face that has been already examined or returned
		// if one gets nowhere, the shell cannot be oriented.
		myOstat = BRepCheck_UnorientableShape;
		if (Update) {
		  BRepCheck::Add(myMap(myShape), myOstat);
		}
		// quit, otherwise there is a risk of taking too much time.
#ifdef DEB
  if (BRepCheck_Trace(0) > 3) {
    orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
    Fcur.Orientation(orf);
    cout << "    Error : this face has been already examined " << endl;
    cout << "    Imposible to return it ";
    PrintShape(Fcur, MapOfShapeOrientation.NbBuckets());
  }
#endif
		return myOstat;
	      }
	      orf = TopAbs::Reverse(orf);
	      MapOfShapeOrientation(Fcur)=orf;


#ifdef DEB
  if (BRepCheck_Trace(0) > 3) {
    orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
    Fcur.Orientation(orf);
    cout << "    Resulting Fcur is returned : " ;
    PrintShape(Fcur, MapOfShapeOrientation.NbBuckets());
  }
#endif

	    }
	    if (alre.Add(Fcur)) {
	      voisin.Append(Fcur);
	    }
	  }
	}
      }
    }
  }

  if (Update) {
    BRepCheck::Add(myMap(myShape), myOstat);
  }
  return myOstat;
}

//=======================================================================
//function : SetUnorientable
//purpose  : 
//=======================================================================

void BRepCheck_Shell::SetUnorientable()
{
  BRepCheck::Add(myMap(myShape),BRepCheck_UnorientableShape);
}


//=======================================================================
//function : IsUnorientable
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Shell::IsUnorientable() const
{
  if (myOdone) {
    return (myOstat != BRepCheck_NoError);
  }
  for (BRepCheck_ListIteratorOfListOfStatus itl(myMap(myShape));
       itl.More();
       itl.Next()) {
    if (itl.Value() == BRepCheck_UnorientableShape) {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : NbConnectedSet
//purpose  : 
//=======================================================================

Standard_Integer BRepCheck_Shell::NbConnectedSet(TopTools_ListOfShape& theSets)
{
  // The connections are found 
  TopTools_IndexedDataMapOfShapeListOfShape parents;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, parents);
  // All faces are taken
  TopTools_MapOfShape theFaces;
  TopExp_Explorer exsh(myShape, TopAbs_FACE);
  for (; exsh.More(); exsh.Next()) theFaces.Add(exsh.Current());
  // The edges that are not oriented or have more than 2 connections are missing
  Standard_Integer iCur;
  TopTools_MapOfShape theMultiEd;
  TopTools_MapOfShape theUnOriEd;
  for (iCur=1; iCur<=parents.Extent(); iCur++) {
    const TopoDS_Edge& Ed = TopoDS::Edge(parents.FindKey(iCur));
    if (parents(iCur).Extent()> 2) theMultiEd.Add(Ed);
    if (Ed.Orientation()!=TopAbs_REVERSED &&
	Ed.Orientation()!=TopAbs_FORWARD) theUnOriEd.Add(Ed);
  }
  // Starting from multiconnected edges propagation by simple connections
  TopTools_ListIteratorOfListOfShape lconx1, lconx2;
  TopTools_MapIteratorOfMapOfShape itmsh(theMultiEd);
  TopoDS_Shell CurShell;
  TopoDS_Shape adFac;
  TopTools_ListOfShape lesCur;
  BRep_Builder BRB;
  Standard_Boolean newCur=Standard_True;
  BRB.MakeShell(CurShell);
  for (; itmsh.More(); itmsh.Next()) {
    const TopoDS_Shape& Ed = itmsh.Key();
    if (!theUnOriEd.Contains(Ed)) {
      for (lconx1.Initialize(parents.FindFromKey(Ed)); lconx1.More(); lconx1.Next()) {
	if (theFaces.Contains(lconx1.Value())) {
	  adFac=lconx1.Value();
	  BRB.Add(CurShell, adFac);
	  theFaces.Remove(adFac);
	  newCur=Standard_False;
	  if (theFaces.IsEmpty()) break;
	  lesCur.Append(adFac);
	  while (!lesCur.IsEmpty()) {
	    adFac=lesCur.First();
	    lesCur.RemoveFirst();
	    for (exsh.Init(adFac, TopAbs_EDGE); exsh.More(); exsh.Next()) {
	      const TopoDS_Shape& ced = exsh.Current();
	      if (!theMultiEd.Contains(ced)) {
		for (lconx2.Initialize(parents.FindFromKey(ced)); lconx2.More(); lconx2.Next()) {
		  if (theFaces.Contains(lconx2.Value())) {
		    adFac=lconx2.Value();
		    BRB.Add(CurShell, adFac);
		    theFaces.Remove(adFac);
		    newCur=Standard_False;
		    if (theFaces.IsEmpty()) break;
		    lesCur.Append(adFac);
		  }
		}
	      }
	      if (theFaces.IsEmpty()) break;
	    }
	  }
	  if (!newCur) {
	   theSets.Append(CurShell);
	   CurShell.Nullify();
	   newCur=Standard_True;
	   BRB.MakeShell(CurShell);
	  }
	}
	if (theFaces.IsEmpty()) break;
      }
    }
    if (theFaces.IsEmpty()) break;
  }
  return theSets.Extent();
}

//=======================================================================
//function : GetOrientation
//purpose  : 
//=======================================================================

#ifdef DEB
static TopAbs_Orientation GetOrientation(const TopoDS_Face& F,
					 const TopoDS_Shape& S)

{
  TopExp_Explorer exp;
  for (exp.Init(S,TopAbs_FACE); exp.More(); exp.Next()) {
    if (exp.Current().IsSame(F)) {
      return exp.Current().Orientation();
    }
  }
  return TopAbs_FORWARD; // for compilation
}
#endif


//=======================================================================
//function : Propagate
//purpose  : 
//=======================================================================

static void Propagate(const TopTools_IndexedDataMapOfShapeListOfShape& mapEF,
		      const TopoDS_Shape& fac,
		      TopTools_MapOfShape& mapF)
{
  if (mapF.Contains(fac)) {
    return;
  }
  mapF.Add(fac); // attention, if oriented == Standard_True, fac should
                 // be FORWARD or REVERSED. It is not checked.

  TopExp_Explorer ex;
  for (ex.Init(fac,TopAbs_EDGE); ex.More(); ex.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(ex.Current());
// test if the edge is in the map (only orienteed edges are present)
    if (mapEF.Contains(edg)) {
      for (TopTools_ListIteratorOfListOfShape itl(mapEF.FindFromKey(edg));
	   itl.More(); itl.Next()) {
	if (!itl.Value().IsSame(fac) &&
	    !mapF.Contains(itl.Value())) {
	  Propagate(mapEF,itl.Value(),mapF);
	}
      }
    }
  }
}
