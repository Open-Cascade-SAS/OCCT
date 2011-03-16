// File:        BRepCheck_Wire.cxx
// Created:        Tue Dec 12 17:49:08 1995
// Author:        Jacques GOUSSARD
//                <jag@bravox>
// Modified by dpf, Fri Dec 19 15:31:03 1997 
//   Taitement de la fermeture en 2d.
//
//  modified by eap Tue Dec 18 14:14:25 2001 (bug OCC23)
//   Check self-intersection in case of closed edge
//
//  modified by eap Fri Dec 21 17:36:55 2001 (bug OCC35)
//   Closed2d() added

//  Modified by skv - Wed Jul 23 12:22:20 2003 OCC1764 

#include <BRepCheck_Wire.ixx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopLoc_Location.hxx>
#include <TColGeom2d_Array1OfCurve.hxx>
#include <IntRes2d_Intersection.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntRes2d_Transition.hxx>
#include <IntRes2d_Domain.hxx>
#include <Geom2dInt_GInter.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepCheck.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <TopTools_MapIteratorOfMapOfOrientedShape.hxx>

//Patch
#include <Precision.hxx>
#include <Bnd_Array1OfBox2d.hxx>
#include <BndLib_Add2dCurve.hxx>

//#ifdef WNT
#include <stdio.h>
#include <BRepTools_WireExplorer.hxx>
#include <TopExp.hxx>
//#endif

#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <ElCLib.hxx>


static void Propagate(const TopTools_IndexedDataMapOfShapeListOfShape&,
                      const TopoDS_Shape&,   // edge
                      TopTools_MapOfShape&); // mapofedge


static TopAbs_Orientation GetOrientation(const TopTools_MapOfShape&,
                                         const TopoDS_Edge&);


static 
  void ChoixUV(const TopoDS_Vertex&,
	       const TopoDS_Edge&,
	       const TopoDS_Face&,
	       TopTools_ListOfShape&);

//      20/03/02 akm vvv (OCC234)
// static 
//   Standard_Boolean CheckLoopOrientation( const TopoDS_Vertex&,
// 					const TopoDS_Edge&,
// 					const TopoDS_Edge&,
// 					const TopoDS_Face&,
// 					TopTools_ListOfShape&);
//      20/03/02 akm ^^^

inline Standard_Boolean IsOriented(const TopoDS_Shape& S)
{
  return (S.Orientation() == TopAbs_FORWARD ||
          S.Orientation() == TopAbs_REVERSED);
}

static
  void CurveDirForParameter(const Handle(Geom2d_Curve)& aC2d,
			    const Standard_Real aPrm,
			    gp_Pnt2d& Pnt,
			    gp_Vec2d& aVec2d);

//  Modified by Sergey KHROMOV - Thu Jun 20 11:21:51 2002 OCC325 Begin
static Standard_Boolean IsClosed2dForPeriodicFace
                        (const TopoDS_Face   &theFace,
			 const gp_Pnt2d      &theP1,
			 const gp_Pnt2d      &theP2,
			 const TopoDS_Vertex &theVertex);

static Standard_Boolean GetPnt2d(const TopoDS_Vertex    &theVertex,
				 const TopoDS_Edge      &theEdge,
				 const TopoDS_Face      &theFace,
				       gp_Pnt2d         &aPnt);
//  Modified by Sergey KHROMOV - Wed May 22 10:44:08 2002 End

//=======================================================================
//function : BRepCheck_Wire
//purpose  : 
//=======================================================================
BRepCheck_Wire::BRepCheck_Wire(const TopoDS_Wire& W)
{
  Init(W);
}
//=======================================================================
//function : Minimum
//purpose  : 
//=======================================================================
void BRepCheck_Wire::Minimum()
{
  myCdone = Standard_False;
  myGctrl = Standard_True;
  if (!myMin) {
    BRepCheck_ListOfStatus thelist;
    myMap.Bind(myShape, thelist);
    BRepCheck_ListOfStatus& lst = myMap(myShape);

    // on verifie que le wire est "connexe" == check that the wire is "connex"
    TopExp_Explorer exp(myShape,TopAbs_EDGE);
    Standard_Integer nbedge = 0;
    myMapVE.Clear();
    // fill myMapVE
    for (; exp.More(); exp.Next()) {
      nbedge++;
      TopExp_Explorer expv;
      for (expv.Init(exp.Current(),TopAbs_VERTEX);
           expv.More(); expv.Next()) {
        const TopoDS_Shape& vtx = expv.Current();
        Standard_Integer index = myMapVE.FindIndex(vtx);
        if (index == 0) {
          TopTools_ListOfShape theListOfShape;
          index = myMapVE.Add(vtx, theListOfShape);
        }
        myMapVE(index).Append(exp.Current());
      }
    }
    // wire must have at least one edge
    if (nbedge == 0) {
      BRepCheck::Add(lst,BRepCheck_EmptyWire);
    }
    // check if all edges are connected through vertices
    else if (nbedge >= 2) {
      TopTools_MapOfShape mapE;
      exp.ReInit();
      Propagate(myMapVE,exp.Current(),mapE);
      for (exp.ReInit(); exp.More(); exp.Next()) {
        if (!mapE.Contains(exp.Current())) {
          BRepCheck::Add(lst,BRepCheck_NotConnected);
          break;
        }
      }
    }
    if (lst.IsEmpty()) {
      lst.Append(BRepCheck_NoError);
    }
    myMapVE.Clear();
    myMin = Standard_True;
  }
}
//=======================================================================
//function : InContext
//purpose  : 
//=======================================================================
void BRepCheck_Wire::InContext(const TopoDS_Shape& S)
{

  if (myMap.IsBound(S)) {
    return;
  }
  BRepCheck_ListOfStatus thelist;
  myMap.Bind(S, thelist);

  BRepCheck_ListOfStatus& lst = myMap(S);

  // check if my wire is in <S>
  TopExp_Explorer exp(S,TopAbs_WIRE); 
  for ( ; exp.More(); exp.Next()) {
    if (exp.Current().IsSame(myShape)) {
      break;
    }
  }
  if (!exp.More()) {
    BRepCheck::Add(lst,BRepCheck_SubshapeNotInShape);
    return;
  }

  BRepCheck_Status st = BRepCheck_NoError;
  TopAbs_ShapeEnum styp = S.ShapeType();
  switch (styp) {

  case TopAbs_FACE:
    {
      TopoDS_Edge ed1,ed2; // bidon
      if (myGctrl) 
        st = SelfIntersect(TopoDS::Face(S),ed1,ed2,Standard_True);
      if (st != BRepCheck_NoError) break;
      st = Closed();
      if (st != BRepCheck_NoError) break;
      st = Orientation(TopoDS::Face(S));
      if (st != BRepCheck_NoError) break;
      st = Closed2d(TopoDS::Face(S));
    }
    break;
  default:
    break;
  }
  
  if (st != BRepCheck_NoError) 
    BRepCheck::Add(lst,st);
      
  if (lst.IsEmpty()) 
    lst.Append(BRepCheck_NoError);
}
//=======================================================================
//function : Blind
//purpose  : 
//=======================================================================
void BRepCheck_Wire::Blind()
{
  if (!myBlind) {
    // rien de plus que dans le minimum
    myBlind = Standard_True;
  }
}
//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================
BRepCheck_Status BRepCheck_Wire::Closed(const Standard_Boolean Update)
{

  if (myCdone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myCstat);
    }
    return myCstat;
  }

  myCdone = Standard_True; // ce sera fait...

  BRepCheck_ListIteratorOfListOfStatus itl(myMap(myShape));
  if (itl.Value() != BRepCheck_NoError) {
    myCstat = itl.Value();
    return myCstat; // deja enregistre
  }

  myCstat = BRepCheck_NoError;

  TopExp_Explorer exp,expv;
  TopTools_MapOfShape mapS;
  TopTools_DataMapOfShapeListOfShape Cradoc;
  myMapVE.Clear();
  // Checks if the oriented edges of the wire give a "closed" wire,
  // i-e if each oriented vertex on oriented edges is found 2 times...
  // myNbori = 0;
  for (exp.Init(myShape,TopAbs_EDGE);exp.More(); exp.Next()) {
    if (IsOriented(exp.Current())) {
      // myNbori++;
      if (!Cradoc.IsBound(exp.Current())) {
        TopTools_ListOfShape theListOfShape;
        Cradoc.Bind(exp.Current(), theListOfShape);
      }
      Cradoc(exp.Current()).Append(exp.Current());

      mapS.Add(exp.Current());
      for (expv.Init(exp.Current(),TopAbs_VERTEX); expv.More(); expv.Next()) {
        if (IsOriented(expv.Current())) {
          Standard_Integer index = myMapVE.FindIndex(expv.Current());
          if (index == 0) {
            TopTools_ListOfShape theListOfShape1;
            index = myMapVE.Add(expv.Current(), theListOfShape1);
          }
          myMapVE(index).Append(exp.Current());
        }
      }
    }
  }

  Standard_Integer theNbori = mapS.Extent();
  if (theNbori >= 2) {
    mapS.Clear();
    for (exp.ReInit(); exp.More(); exp.Next()) {
      if (IsOriented(exp.Current())) {
        break;
      }
    }
    Propagate(myMapVE,exp.Current(),mapS);
  }
  if (theNbori != mapS.Extent()) {
    myCstat = BRepCheck_NotConnected;
    if (Update) {
      BRepCheck::Add(myMap(myShape),myCstat);
    }
    return myCstat;
  }

  // Checks the number of occurence of an edge : maximum 2, and in this 
  // case, one time FORWARD and one time REVERSED

  Standard_Boolean yabug = Standard_False;
  for (TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itdm(Cradoc);
       itdm.More(); itdm.Next()) {
    if (itdm.Value().Extent() >= 3) {
      yabug = Standard_True;
    }
    else if (itdm.Value().Extent() == 2) {
      if (itdm.Value().First().Orientation() == 
          itdm.Value().Last().Orientation()) {
        yabug = Standard_True;
      }
    }
    if (yabug) {
      myCstat = BRepCheck_RedundantEdge;
      if (Update) {
        BRepCheck::Add(myMap(myShape),myCstat);
      }
      return myCstat;
    }
  }

  for (Standard_Integer i = 1; i<= myMapVE.Extent(); i++) {
    if (myMapVE(i).Extent()%2 != 0) {
      myCstat=BRepCheck_NotClosed;
      if (Update) {
        BRepCheck::Add(myMap(myShape),myCstat);
      }
      return myCstat;
    }
  }

  if (Update) {
    BRepCheck::Add(myMap(myShape),myCstat);
  }
  return myCstat;
}
//=======================================================================
//function : Closed2d
//purpose  : for periodic faces
//=======================================================================
BRepCheck_Status BRepCheck_Wire::Closed2d(const TopoDS_Face& theFace,
                                          const Standard_Boolean Update)
{

  // 3d closure checked?
  BRepCheck_Status aClosedStat = Closed();
  if (aClosedStat != BRepCheck_NoError) {
    if (Update) 
      BRepCheck::Add(myMap(myShape),aClosedStat);
    return aClosedStat;
  }

  // 20/03/02 akm vvv : (OCC234) Hence this method will be used to check
  //                    both periodic and non-periodic faces
  //   // this check is for periodic faces 
  BRepAdaptor_Surface aFaceSurface (theFace, Standard_False);
  // if (!aFaceSurface.IsUPeriodic() && !aFaceSurface.IsVPeriodic())
  // {
  //   if (Update) 
  //     BRepCheck::Add(myMap(myShape),aClosedStat);
  //   return aClosedStat;
  // }
  // 20/03/02 akm ^^^
  
  // count edges having FORWARD or REVERSED orientation
  Standard_Integer aNbOrirntedEdges = 0;
  TopExp_Explorer anEdgeExp(myShape,TopAbs_EDGE);
  for (;anEdgeExp.More(); anEdgeExp.Next()) {
    if (IsOriented(anEdgeExp.Current())) 
      aNbOrirntedEdges++;
  }
  if (aNbOrirntedEdges==0)
  {
    if (Update) 
      BRepCheck::Add(myMap(myShape),aClosedStat);
    return aClosedStat;
  }
  
  // all those edges must form a closed 2d contour and be found by WireExplorer

  Standard_Integer aNbFoundEdges = 0;

  BRepTools_WireExplorer aWireExp(TopoDS::Wire(myShape), theFace);
  TopoDS_Edge aFirstEdge = aWireExp.Current();
  TopoDS_Vertex aFirstVertex = aWireExp.CurrentVertex();
  TopoDS_Edge aLastEdge;
  for (;aWireExp.More(); aWireExp.Next())
  {
    aNbFoundEdges++;
    aLastEdge = aWireExp.Current();
  }

  if (aNbFoundEdges != aNbOrirntedEdges)
  {
    aClosedStat = BRepCheck_NotClosed;
    if (Update) 
      BRepCheck::Add(myMap(myShape),aClosedStat);
    return aClosedStat;
  }
    
  // Check distance between 2d ends of first and last edges
//  Modified by Sergey KHROMOV - Mon May 13 12:42:10 2002 Begin
//   First check if first and last edges are infinite:
  Standard_Real      aF;
  Standard_Real      aL;
  Standard_Boolean   isFirstInfinite = Standard_False;
  Standard_Boolean   isLastInfinite  = Standard_False;
  TopAbs_Orientation anOri;

  anOri = aFirstEdge.Orientation();
  BRep_Tool::Range(aFirstEdge, aF, aL);
  if ((anOri == TopAbs_FORWARD  && aF == -Precision::Infinite()) ||
      (anOri == TopAbs_REVERSED && aL ==  Precision::Infinite()))
    isFirstInfinite = Standard_True;

  anOri = aLastEdge.Orientation();
  BRep_Tool::Range(aLastEdge, aF, aL);
  if ((anOri == TopAbs_FORWARD  && aL ==  Precision::Infinite()) ||
      (anOri == TopAbs_REVERSED && aF == -Precision::Infinite()))
    isLastInfinite = Standard_True;

  if (isFirstInfinite && isLastInfinite) {
    if (Update) 
      BRepCheck::Add(myMap(myShape),aClosedStat);
    return aClosedStat;
  } else if (aFirstVertex.IsNull()) {
    aClosedStat = BRepCheck_NotClosed;
    if (Update) 
      BRepCheck::Add(myMap(myShape),aClosedStat);
    return aClosedStat;
  }
//  Modified by Sergey KHROMOV - Mon May 13 12:42:10 2002 End

  gp_Pnt2d p, p1, p2; // ends of prev edge, next edge, bidon

  // get first point
  BRep_Tool::UVPoints(aLastEdge, theFace, p2, p);
  if (aLastEdge.Orientation() == TopAbs_REVERSED) p = p2; 
  
//  Modified by Sergey KHROMOV - Mon Apr 22 10:36:33 2002 Begin
//   Standard_Real aTol, aUResol, aVResol;

//   // find 2d tolerance
//   aTol  = BRep_Tool::Tolerance(aFirstVertex);
//   aUResol = 2*aFaceSurface.UResolution(aTol);
//   aVResol = 2*aFaceSurface.VResolution(aTol);

  // get second point
  if (aFirstEdge.Orientation() == TopAbs_REVERSED)
    BRep_Tool::UVPoints(aFirstEdge, theFace, p2, p1);
  else 
    BRep_Tool::UVPoints(aFirstEdge, theFace, p1, p2);

//  Modified by Sergey KHROMOV - Thu Jun 20 10:55:42 2002 OCC325 Begin
// Check 2d distance for periodic faces with seam edge
  if (!IsClosed2dForPeriodicFace(theFace, p, p1, aFirstVertex)) {
    aClosedStat = BRepCheck_NotClosed;
    if (Update) 
      BRepCheck::Add(myMap(myShape),aClosedStat);
    return aClosedStat;
  }
//  Modified by Sergey KHROMOV - Thu Jun 20 10:58:05 2002 End

  // check distance
//   Standard_Real dfUDist=Abs(p.X()-p1.X());
//   Standard_Real dfVDist=Abs(p.Y()-p1.Y());
//   if (dfUDist > aUResol || dfVDist > aVResol)
//   {
  Standard_Real aTol3d = BRep_Tool::Tolerance(aFirstVertex);
  gp_Pnt        aPRef  = BRep_Tool::Pnt(aFirstVertex);
  gp_Pnt        aP1    = aFaceSurface.Value(p.X(),  p.Y());
  gp_Pnt        aP2    = aFaceSurface.Value(p1.X(), p1.Y());
  Standard_Real Dist1  = aPRef.Distance(aP1);
  Standard_Real Dist2  = aPRef.Distance(aP2);

  if (Dist1 > aTol3d || Dist2 > aTol3d) {
//  Modified by Sergey KHROMOV - Mon Apr 22 10:36:44 2002 End
#ifdef DEB
    cout << endl;
    cout << "------------------------------------------------------"   <<endl;
    cout << "--- BRepCheck Wire: Closed2d -> Erreur"                   <<endl;
    if (Dist1 > aTol3d)
      cout << "--- Dist1 (" << Dist1 << ") > Tol3d (" << aTol3d << ")" <<endl;
    if (Dist2 > aTol3d)
      cout << "--- Dist2 (" << Dist2 << ") > Tol3d (" << aTol3d << ")" <<endl;
    cout << "------------------------------------------------------"   <<endl;
#endif
    aClosedStat = BRepCheck_NotClosed;
    if (Update) 
      BRepCheck::Add(myMap(myShape),aClosedStat);
    return aClosedStat;
  }
  
  if (Update) 
    BRepCheck::Add(myMap(myShape),aClosedStat);
  return aClosedStat;
}
//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================
BRepCheck_Status BRepCheck_Wire::Orientation(const TopoDS_Face& F,
                                             const Standard_Boolean Update)
{
  BRepCheck_Status theOstat = Closed();
  if (theOstat != BRepCheck_NotClosed && theOstat != BRepCheck_NoError) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),theOstat);
    }
    return theOstat;
  }

  theOstat = BRepCheck_NoError;

  TopoDS_Vertex VF,VL;
#ifndef DEB
  TopAbs_Orientation orient, ortmp = TopAbs_FORWARD;
#else
  TopAbs_Orientation orient, ortmp;
#endif
  TopTools_ListOfShape ledge, ListOfPassedEdge;
  TopExp_Explorer exp,vte;
  TopTools_MapOfShape mapS;
  TopoDS_Edge theEdge,theRef;

  // Checks the orientation of the edges
  for (exp.Init(myShape,TopAbs_EDGE); exp.More(); exp.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    orient = edg.Orientation();
    if (IsOriented(edg)) {
      mapS.Add(edg);
      theEdge = edg;
      theRef = edg;
      for (vte.Init(edg,TopAbs_VERTEX);vte.More(); vte.Next()) {
        TopAbs_Orientation vto = vte.Current().Orientation();
        if (vto == TopAbs_FORWARD) {
          VF = TopoDS::Vertex(vte.Current());
        }
        else if (vto == TopAbs_REVERSED) {
          VL = TopoDS::Vertex(vte.Current());
        }
        if (!VF.IsNull() && !VL.IsNull()) {
          break;
        }
      }
      if (VF.IsNull() && VL.IsNull())
        theOstat = BRepCheck_InvalidDegeneratedFlag;
      break;
    }
  }
 
  if (theOstat == BRepCheck_NoError) {
    Standard_Integer Index = 1;
    Standard_Integer nbOriNoDegen=myMapVE.Extent();
//  Modified by Sergey KHROMOV - Tue May 21 17:12:45 2002 Begin
    Standard_Boolean isGoFwd     = Standard_True;

    if (VL.IsNull())
      isGoFwd = Standard_False;
//  Modified by Sergey KHROMOV - Tue May 21 17:12:45 2002 End

    while (Index < nbOriNoDegen) {
      ledge.Clear();
      ListOfPassedEdge.Clear();
      // on cherche les edges qui s`enchainent sur VL si !VL.IsNull 
      // sinon sur VF.
      
      Standard_Integer ind;
      if (!VL.IsNull()) {
        ind = myMapVE.FindIndex(VL);
      }
      else if (!VF.IsNull()) {
        ind = myMapVE.FindIndex(VF);
      }
      else {
        theOstat = BRepCheck_InvalidDegeneratedFlag;
        break;
      }

      for (TopTools_ListIteratorOfListOfShape itls(myMapVE(ind));
           itls.More(); itls.Next()) {
        const TopoDS_Edge & edg = TopoDS::Edge(itls.Value());

        orient = edg.Orientation();
        if (mapS.Contains(edg)) ortmp = GetOrientation(mapS,edg);

        //Add to list already passed outcoming edges
        if (mapS.Contains(edg) && ortmp == orient && !edg.IsSame(theEdge))
          for (vte.Init(edg,TopAbs_VERTEX); vte.More(); vte.Next())
            {
              TopAbs_Orientation vto = vte.Current().Orientation();
              if (!VL.IsNull())
                {
                  if (vto == TopAbs_FORWARD && VL.IsSame(vte.Current()))
                    {
                      ListOfPassedEdge.Append(edg);
                      break;
                    }
                }
              else // VF is not null
                {
                  if (vto == TopAbs_REVERSED && VF.IsSame(vte.Current()))
                    {
                      ListOfPassedEdge.Append(edg);
                      break;
                    }
                }
            }

        if (!mapS.Contains(edg) || ortmp != orient) {
          for (vte.Init(edg,TopAbs_VERTEX);vte.More(); vte.Next()) {
            TopAbs_Orientation vto = vte.Current().Orientation();
            if (!VL.IsNull()) {
              if (vto == TopAbs_FORWARD && VL.IsSame(vte.Current())) {
                // Si on travaille en 2d (face non nulle) ou 
                // si l'edge n'est pas degenere on l'ajoute
                if (!F.IsNull() || !BRep_Tool::Degenerated(edg))
                  ledge.Append(edg);
                break;
              }
            }
            else { // VF n`est pas nul
              if (vto == TopAbs_REVERSED && VF.IsSame(vte.Current())) {
                // Si on travaille en 2d (face non nulle) ou 
                // si l'edge n'est pas degenere on l'ajoute
                if (!F.IsNull() || !BRep_Tool::Degenerated(edg))
                  ledge.Append(edg);
                break;
              }
            }
          }
        }
      }
      Standard_Integer nbconnex = ledge.Extent();
      Standard_Boolean Changedesens = Standard_False;
      if (nbconnex == 0) {
        if (myCstat == BRepCheck_NotClosed) {
          if (VL.IsNull()) {
            if (Update) {
              BRepCheck::Add(myMap(myShape),theOstat);
            }
            return theOstat; // on sort
          }
          else {
            Index--; // parce que apres Index++ et on n`a pas enchaine
            VL.Nullify(); // on force a enchainer sur VF
            theEdge = theRef;
            Changedesens = Standard_True;
          }
        }
        else {
          theOstat = BRepCheck_BadOrientationOfSubshape;
          if (Update) {
            BRepCheck::Add(myMap(myShape),theOstat);
	    }
          return theOstat;
        }
      }

      // JAG 03/07   else if (nbconnex >= 2 && !F.IsNull())  // On essaie de voir en 2d
      else if (!F.IsNull()) { // On essaie de voir en 2d
        TopoDS_Vertex pivot;
        if (!VL.IsNull()) {
          pivot = VL;
        }
        else {
          pivot = VF;
        }

        ChoixUV(pivot,theEdge,F,ledge);
        nbconnex = ledge.Extent();
//      20/03/02 akm vvv : (OCC234) - The 2d exploration of wire with necessary
//                         checks is performed in Closed2d, here it's useless
//         if (nbconnex == 1 && !CheckLoopOrientation( pivot, theEdge, TopoDS::Edge(ledge.First()), F, ListOfPassedEdge ))
//         {
//           theOstat = BRepCheck_BadOrientationOfSubshape;
//           if (Update)
//             BRepCheck::Add(myMap(myShape),theOstat);
//           return theOstat;
//         }
//      20/03/02 akm ^^^
      }

      if (nbconnex >= 2) {
        theOstat = BRepCheck_BadOrientationOfSubshape;
        if (Update) {
          BRepCheck::Add(myMap(myShape),theOstat);
          }
        return theOstat;
      }
      else if (nbconnex == 1) {
        // decaler le vertex
        for (vte.Init(ledge.First(),TopAbs_VERTEX);vte.More(); vte.Next()) {
          TopAbs_Orientation vto = vte.Current().Orientation();
          if (!VL.IsNull()) {
            if (vto == TopAbs_REVERSED) {
              VL = TopoDS::Vertex(vte.Current());
              break;
            }
          }
          else { // VF n`est pas nul
            if (vto == TopAbs_FORWARD) {
              VF = TopoDS::Vertex(vte.Current());
              break;
            }
          }
        }
        mapS.Add(ledge.First());
        theEdge = TopoDS::Edge(ledge.First());
        if (!vte.More()) {
          if (!VL.IsNull()) {
            VL.Nullify();
          }
          else {
            VF.Nullify();
          }
        }
      }
      else if (!Changedesens) { //nbconnex == 0
        theOstat = BRepCheck_NotClosed;
        if (Update) {
          BRepCheck::Add(myMap(myShape),theOstat);
	  }
	return theOstat;
      }

      // On verifie la fermeture du wire en 2d (pas fait dans Closed())
      
      TopoDS_Vertex    aVRef;
      Standard_Boolean isCheckClose = Standard_False;

      if (isGoFwd && !VF.IsNull()) {
	aVRef        = VF;
	isCheckClose = Standard_True;
      } else if (!isGoFwd && !VL.IsNull()) {
	aVRef        = VL;
	isCheckClose = Standard_True;
      }

//       if (Index==1 && myCstat!=BRepCheck_NotClosed && 
// 	  !VF.IsNull() && !F.IsNull()) {
      if (Index==1 && myCstat!=BRepCheck_NotClosed && 
	  isCheckClose && !F.IsNull()) {
	ledge.Clear();
// 	ind = myMapVE.FindIndex(VF);
	ind = myMapVE.FindIndex(aVRef);
	for (TopTools_ListIteratorOfListOfShape itlsh(myMapVE(ind));
	     itlsh.More(); itlsh.Next()) {
	  const TopoDS_Edge & edg = TopoDS::Edge(itlsh.Value());
	  orient = edg.Orientation();
	  if (!theRef.IsSame(edg)) {
	    for (vte.Init(edg,TopAbs_VERTEX);vte.More(); vte.Next()) {
	      TopAbs_Orientation vto = vte.Current().Orientation();
// 	      if (vto == TopAbs_REVERSED && VF.IsSame(vte.Current())) {
	      if (vto == TopAbs_REVERSED && aVRef.IsSame(vte.Current())) {
		ledge.Append(edg);
		break;
	      }
	    }
	  }
	}
// 	ChoixUV(VF, theRef, F, ledge);
	ChoixUV(aVRef, theRef, F, ledge);
	if (ledge.Extent()==0) {
	  theOstat = BRepCheck_NotClosed;
	  if (Update) {
	    BRepCheck::Add(myMap(myShape),theOstat);
	  }
	  return theOstat;
	}
      }
      // Fin controle fermeture 2d

      Index ++;
    }
  }
  if (Update) {
    BRepCheck::Add(myMap(myShape),theOstat);
  }
  return theOstat;
}
//=======================================================================
//function : SelfIntersect
//purpose  : 
//=======================================================================
BRepCheck_Status BRepCheck_Wire::SelfIntersect(const TopoDS_Face& F,
					       TopoDS_Edge& retE1,
					       TopoDS_Edge& retE2,
					       const Standard_Boolean Update)
{


  Standard_Boolean ok;
  Standard_Integer i,j,Nbedges;
  Standard_Real first1,last1,first2,last2, tolint;
  gp_Pnt2d pfirst1,plast1,pfirst2,plast2;
  gp_Pnt P3d, P3d2;
  Handle(BRepAdaptor_HSurface) HS;
  Geom2dAdaptor_Curve C1, C2;
  Geom2dInt_GInter      Inter;
  IntRes2d_Domain myDomain1;
  TopTools_IndexedMapOfOrientedShape EMap;
  TopTools_MapOfOrientedShape auxmape;
  //
  ok=Standard_True;
  //-- on verifie plus loin avec les bonnes tolerances si on n a 
  //-- pas un point dans la tolerance d un vertex.
  tolint = 1.e-10; 
  HS = new BRepAdaptor_HSurface();
  HS->ChangeSurface().Initialize(F,Standard_False);
  //
  for (TopoDS_Iterator Iter1(myShape);Iter1.More();Iter1.Next()) {
    if (Iter1.Value().ShapeType() == TopAbs_EDGE) {
      EMap.Add(Iter1.Value());
    }
  }
  //
  Nbedges=EMap.Extent();
  if (!Nbedges) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),BRepCheck_EmptyWire);
    }
    return(BRepCheck_EmptyWire);
  }
  //
  IntRes2d_Domain *tabDom = new IntRes2d_Domain[Nbedges];
  TColGeom2d_Array1OfCurve tabCur(1,Nbedges);
  Bnd_Array1OfBox2d boxes(1,Nbedges);
  //
  for(i = 1; i <= Nbedges; i++) { 
    const TopoDS_Edge& E1 = TopoDS::Edge(EMap.FindKey(i));
    if (i == 1) {
      Handle(Geom2d_Curve) pcu = BRep_Tool::CurveOnSurface(E1, F, first1, last1);
      if (pcu.IsNull()) {
	retE1=E1;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),BRepCheck_SelfIntersectingWire);
	}
	delete [] tabDom;
	return(BRepCheck_SelfIntersectingWire);
      }
      //
      C1.Load(pcu);
      // To avoid exeption in Segment if C1 is BSpline - IFV
      if(!C1.IsPeriodic()) {
	if(C1.FirstParameter() > first1) {
	  first1 = C1.FirstParameter();
	}
	if(C1.LastParameter()  < last1 ){
	  last1  = C1.LastParameter();
	}
      }
      //
      BRep_Tool::UVPoints(E1, F, pfirst1, plast1);
      myDomain1.SetValues(pfirst1,first1,tolint, plast1,last1,tolint);
      //
      BndLib_Add2dCurve::Add(C1, first1, last1, Precision::PConfusion(), boxes(i));
    }//if (i == 1) {
    else {
      C1.Load(tabCur(i));
      myDomain1 = tabDom[i-1];
    }
    //
    // Self intersect of C1
    Inter.Perform(C1, myDomain1, tolint, tolint);
    //
    if(Inter.IsDone()) { 
      Standard_Integer nbp = Inter.NbPoints();
      Standard_Integer nbs = Inter.NbSegments();
      //
      for(Standard_Integer p=1;p<=nbp;p++) {
	const IntRes2d_IntersectionPoint& IP=Inter.Point(p);
	const IntRes2d_Transition& Tr1 = IP.TransitionOfFirst();
	const IntRes2d_Transition& Tr2 = IP.TransitionOfSecond();
	if(   Tr1.PositionOnCurve() == IntRes2d_Middle
	   || Tr2.PositionOnCurve() == IntRes2d_Middle) { 
	  //-- Verification des points avec les vraies tolerances (ie Tol en 3d)
	  //-- Si le point d intersection est dans la tolearnce d un des vertex
	  //-- on considere que cette intersection est bonne (pas d erreur)
	  Standard_Boolean localok = Standard_False;
	  Standard_Real f,l;
	  TopLoc_Location L;
	  const Handle(Geom_Curve) ConS = BRep_Tool::Curve(E1,L,f,l);
	  if(!ConS.IsNull()) { 
	    //-- on va tester en 3d. (ParamOnSecond donne le m resultat)
	    P3d = ConS->Value(IP.ParamOnFirst()); 
	    P3d.Transform(L.Transformation());
	    //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
	  } 
	  else {
	    gp_Pnt2d aP2d  = C1.Value(IP.ParamOnFirst());
	    P3d = HS->Value(aP2d.X(), aP2d.Y());
	  }
	  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
	  TopExp_Explorer ExplVtx;
	  for(ExplVtx.Init(E1,TopAbs_VERTEX); 
	      localok==Standard_False && ExplVtx.More();
	      ExplVtx.Next()) { 
	    gp_Pnt p3dvtt;
	    Standard_Real tolvtt, p3dvttDistanceP3d;
	    //
	    const TopoDS_Vertex& vtt = TopoDS::Vertex(ExplVtx.Current());
	    p3dvtt = BRep_Tool::Pnt(vtt);
	    tolvtt =  BRep_Tool::Tolerance(vtt);
	    tolvtt=tolvtt*tolvtt;
	    p3dvttDistanceP3d=p3dvtt.SquareDistance(P3d);
	    if(p3dvttDistanceP3d <=  tolvtt) { 
	      localok=Standard_True;
	    }
	  }
	  if(localok==Standard_False) { 
	    ok=0;
	    retE1=E1;
	    if (Update) {
	      BRepCheck::Add(myMap(myShape),BRepCheck_SelfIntersectingWire);
	      }
	    delete [] tabDom;
#ifdef DEB
	    static Standard_Integer numpoint=0;
	    cout<<"point p"<<++numpoint<<" "<<P3d.X()<<" "<<P3d.Y()<<" "<<P3d.Z()<<endl;cout.flush();
#endif
	    return(BRepCheck_SelfIntersectingWire);
	  }
	}
      }
    }// if(Inter.IsDone()) { 
    //
    for(j=i+1; j<=Nbedges; j++) {
      const TopoDS_Edge& E2 = TopoDS::Edge(EMap.FindKey(j));
      if (i == 1) {
	tabCur(j) = BRep_Tool::CurveOnSurface(E2,F,first2,last2);
	if (!tabCur(j).IsNull() && last2 > first2) {
	  C2.Load(tabCur(j));
	  // To avoid exeption in Segment if C2 is BSpline - IFV
	  if(!C2.IsPeriodic()) {
	    if(C2.FirstParameter() > first2) {
	      first2 = C2.FirstParameter();
	    }
	    if(C2.LastParameter()  < last2 ) {
	      last2  = C2.LastParameter();
	    }
	  }
	  //
	  BRep_Tool::UVPoints(E2,F,pfirst2,plast2);
	  tabDom[j-1].SetValues(pfirst2,first2,tolint,plast2,last2,tolint);
	  
	  BndLib_Add2dCurve::Add( C2, first2, last2, Precision::PConfusion(), boxes(j) );
	}
	else {
	  delete [] tabDom;
#ifdef DEB
	  cout<<"BRepCheck_NoCurveOnSurface or BRepCheck_InvalidRange"<<endl;cout.flush();
#endif
	  if(tabCur(j).IsNull()) {
	    return(BRepCheck_NoCurveOnSurface);
	  }
	  return (BRepCheck_InvalidRange);
	}
      }// if (i == 1) {
      else {
	C2.Load(tabCur(j));
      }
      //
      if (boxes(i).IsOut( boxes(j))) {
	continue;
      }
      //modified by NIZNHY-PKV Fri Oct 29 10:09:01 2010f
      if (E1.IsSame(E2)) {
	continue;
      }
      //modified by NIZNHY-PKV Fri Oct 29 10:09:02 2010t
      //
      //-- ************************************************************
      //-- ******* I n t e r s e c t i o n   C 1   e t   C 2   ********
      //-- ************************************************************
      Inter.Perform(C1,myDomain1,C2,tabDom[j-1],tolint,tolint);
      //
      if(Inter.IsDone()) { 
	Standard_Integer nbp, nbs;
	Standard_Real IP_ParamOnFirst, IP_ParamOnSecond;
	IntRes2d_Transition Tr1,Tr2;
	TopTools_ListOfShape CommonVertices;
	TopTools_ListIteratorOfListOfShape itl;
	TopTools_MapOfShape Vmap;
	//
	TopoDS_Iterator it( E1 );
	for (; it.More(); it.Next()) {
	  Vmap.Add( it.Value() );
	}
	//
	it.Initialize( E2 );
	for (; it.More(); it.Next()) {
	  const TopoDS_Shape& V = it.Value();
	  if (Vmap.Contains( V )) {
	    CommonVertices.Append( V );
	  }
	}
	//
	nbp = Inter.NbPoints();
	nbs = Inter.NbSegments();
	IP_ParamOnFirst  = 0.;
	IP_ParamOnSecond = 0.;
	//
	//// **** Points of intersection **** ////
	for (Standard_Integer p = 1; p <= nbp; p++)  {
	  const IntRes2d_IntersectionPoint& IP = Inter.Point(p);
	  IP_ParamOnFirst  = IP.ParamOnFirst();
	  IP_ParamOnSecond = IP.ParamOnSecond();
	  Tr1 = IP.TransitionOfFirst();
	  Tr2 = IP.TransitionOfSecond();
	  if(   Tr1.PositionOnCurve() == IntRes2d_Middle
	     || Tr2.PositionOnCurve() == IntRes2d_Middle)   {
	    //-- Verification des points avec les vraies tolerances (ie Tol en 3d)
	    //-- Si le point d intersection est dans la tolearnce d un des vertex
	    //-- on considere que cette intersection est bonne (pas d erreur)
	    Standard_Boolean localok = Standard_False;  
	    Standard_Real f1,l1, f2, l2;
	    TopLoc_Location L, L2;
	    //
	    const Handle(Geom_Curve) ConS = BRep_Tool::Curve(E1,L,f1,l1);    
	    const Handle(Geom_Curve) ConS2 = BRep_Tool::Curve(E2,L2,f2,l2);  
	    //gka protect against working out of edge range
	    if ( f1-IP_ParamOnFirst > ::Precision::PConfusion() || 
		IP_ParamOnFirst-l1 > ::Precision::PConfusion() || 
		f2-IP_ParamOnSecond > ::Precision::PConfusion() || 
		IP_ParamOnSecond-l2 > ::Precision::PConfusion() ) 
	      continue;
	    Standard_Real tolvtt;
	    //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
	    if (!ConS.IsNull()) { 
	      P3d = ConS->Value(IP_ParamOnFirst); 
	      P3d.Transform(L.Transformation());
	    } 
	    else {
	      gp_Pnt2d aP2d  = C1.Value(IP_ParamOnFirst);
	      P3d = HS->Value(aP2d.X(), aP2d.Y());
	    }
	    //
	    if (!ConS2.IsNull()) {
	      P3d2 = ConS2->Value(IP_ParamOnSecond); 
	      P3d2.Transform(L2.Transformation());
	    } 
	    else {
	      gp_Pnt2d aP2d  = C2.Value(IP_ParamOnSecond);
	      P3d2 = HS->Value(aP2d.X(), aP2d.Y());
	    }
	    //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
	    itl.Initialize( CommonVertices );
	    for (; itl.More(); itl.Next()) {
	      Standard_Real p3dvttDistanceP3d, p3dvttDistanceP3d2;
	      gp_Pnt p3dvtt;
	      //
	      const TopoDS_Vertex& vtt = TopoDS::Vertex(itl.Value());
	      p3dvtt = BRep_Tool::Pnt(vtt);
	      tolvtt =  BRep_Tool::Tolerance(vtt);
	      tolvtt=1.1*tolvtt;
	      tolvtt=tolvtt*tolvtt;
	      p3dvttDistanceP3d  = p3dvtt.SquareDistance(P3d);
	      p3dvttDistanceP3d2 = p3dvtt.SquareDistance(P3d2);
	      //
	      if (p3dvttDistanceP3d<=tolvtt && p3dvttDistanceP3d2<=tolvtt)  { 
		localok = Standard_True;
		break;
	      }
	    }
	    
	    //-- --------------------------------------------------------
	    //-- Verification sur le baillement maximum entre les 2 edges
	    //--
	    //-- On verifie la distance des edges a la courbe joignant 
	    //-- le point d intersection au vertex (s il existe)
	    if (localok == Standard_False && !CommonVertices.IsEmpty()) {
#ifdef DEB	
	      cout << "\n------------------------------------------------------\n" <<endl;
	      cout << "\n--- BRepCheck Wire: AutoIntersection Phase1 -> Erreur \n" <<endl;
	      
#endif
	      Standard_Boolean yaunvtxproche;
	      Standard_Real distauvtxleplusproche,VParaOnEdge1,VParaOnEdge2;
	      gp_Pnt VertexLePlusProche;
	      //
	      yaunvtxproche=Standard_False;
	      VParaOnEdge1 =0.;
	      VParaOnEdge2 =0.;
	      distauvtxleplusproche=RealLast();
	      //Find the nearest common vertex
	      itl.Initialize( CommonVertices );
	      for (; itl.More(); itl.Next())   {
		Standard_Real tolvtt, disptvtx;
		gp_Pnt p3dvtt;
		//
		const TopoDS_Vertex& vtt = TopoDS::Vertex(itl.Value());
		p3dvtt = BRep_Tool::Pnt(vtt);
		tolvtt = BRep_Tool::Tolerance(vtt);
		disptvtx = P3d.Distance(p3dvtt);
		if (disptvtx < distauvtxleplusproche)	{
		  VertexLePlusProche = p3dvtt; 
		  distauvtxleplusproche = disptvtx;
		  VParaOnEdge1 = BRep_Tool::Parameter(vtt,E1);
		  VParaOnEdge2 = BRep_Tool::Parameter(vtt,E2);
		}
		// eap: case of closed edge
		else if (IsEqual(distauvtxleplusproche, disptvtx)) {
		  Standard_Real newVParaOnEdge1 = BRep_Tool::Parameter(vtt,E1);
		  Standard_Real newVParaOnEdge2 = BRep_Tool::Parameter(vtt,E2);
		  if (Abs(IP_ParamOnFirst - VParaOnEdge1) + Abs(IP_ParamOnSecond - VParaOnEdge2)
		      >
		      Abs(IP_ParamOnFirst - newVParaOnEdge1) + Abs(IP_ParamOnSecond - newVParaOnEdge2)) {
		    VertexLePlusProche = p3dvtt;
		    VParaOnEdge1 = newVParaOnEdge1;
		    VParaOnEdge2 = newVParaOnEdge2;
		  }
		}
	      }
	      //Patch: extraordinar situation (e.g. tolerance(v) == 0.)
	      //  Modified by skv - Wed Jul 23 12:28:11 2003 OCC1764 Begin
	      // if (VertexLePlusProche.Distance( P3d ) <= gp::Resolution())
	      if (VertexLePlusProche.Distance(P3d)  <= gp::Resolution() ||
		  VertexLePlusProche.Distance(P3d2) <= gp::Resolution()) {
		    //  Modified by skv - Wed Jul 23 12:28:12 2003 OCC1764 End
		localok = Standard_True;
	      }
	      else {
		gp_Lin Lig( VertexLePlusProche, gp_Vec(VertexLePlusProche,P3d) );
		Standard_Real du1 = 0.1*(IP_ParamOnFirst -VParaOnEdge1);
		Standard_Real du2 = 0.1*(IP_ParamOnSecond-VParaOnEdge2);
		Standard_Real maxd1 = 0., maxd2 = 0.;
		Standard_Integer k;
		
		localok = Standard_True;
		Standard_Real tole1 = BRep_Tool::Tolerance(E1);
		for (k = 2; localok && k < 9; k++)	{ 
		  Standard_Real u = VParaOnEdge1 + k*du1;  // voyons deja voir si ca marche
		  gp_Pnt P1;
		  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
		  if (!ConS.IsNull()) {
		    P1 = ConS->Value(u);
		    P1.Transform(L.Transformation());
		  } 
		  else {
		    gp_Pnt2d aP2d  = C1.Value(u);
		    P1 = HS->Value(aP2d.X(), aP2d.Y());
		  }
		  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
		  Standard_Real d1 = Lig.Distance(P1);
		  if (d1 > maxd1) {
		    maxd1 = d1;
		  }
		  if (d1 > tole1*2.0){
		    localok = Standard_False;
		  }
		}
		//-- meme chose pour edge2
		//  Modified by skv - Wed Jul 23 12:22:20 2003 OCC1764 Begin
		gp_Dir aTmpDir(P3d2.XYZ().Subtracted(VertexLePlusProche.XYZ()));
		
		Lig.SetDirection(aTmpDir);
		//  Modified by skv - Wed Jul 23 12:22:23 2003 OCC1764 End
		Standard_Real tole2 = BRep_Tool::Tolerance(E2);
		for (k = 2; localok && k < 9; k++) {
		  Standard_Real u = VParaOnEdge2 + k*du2;  // voyons deja voir si ca marche
		  gp_Pnt        P2;
		  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
		  if (!ConS2.IsNull()) {
		    P2 = ConS2->Value(u);
		    P2.Transform(L2.Transformation());
		  }
		  else {
		    gp_Pnt2d aP2d  = C2.Value(u);
		    P2 = HS->Value(aP2d.X(), aP2d.Y());
		  }
		  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
		  Standard_Real d2 = Lig.Distance(P2);
		  if (d2 > maxd2) {
		    maxd2 = d2;
		  }
		  if (d2 > tole2*2.0){
		    localok = Standard_False;
		  }
		}
#ifdef DEB
		if(localok) { 
		  printf("--- BRepCheck Wire: AutoIntersection Phase2 -> Bon \n");
		  printf("--- distance Point Vertex : %10.7g (tol %10.7g)\n",distauvtxleplusproche,tolvtt);
		  printf("--- Erreur Max sur E1 : %10.7g  Tol_Edge:%10.7g\n",maxd1,tole1);
		  printf("--- Erreur Max sur E2 : %10.7g  Tol_Edge:%10.7g\n",maxd2,tole2);
		  fflush(stdout);
		}
		else { 
		  printf("--- BRepCheck Wire: AutoIntersection Phase2 -> Erreur \n");
		  printf("--- distance Point Vertex : %10.7g (tol %10.7g)\n",distauvtxleplusproche,tolvtt);
		  printf("--- Erreur Max sur E1 : %10.7g  Tol_Edge:%10.7g\n",maxd1,tole1);
		  printf("--- Erreur Max sur E2 : %10.7g  Tol_Edge:%10.7g\n",maxd2,tole2);
		  fflush(stdout);
		}
#endif
	      } //end of else (construction of the line Lig)
	    } //end of if (localok == Standard_False && !CommonVertices.IsEmpty())
	    //
	    if(localok==Standard_False)	  { 
	      ok=0;
	      retE1=E1;
	      retE2=E2;
	      if (Update) {
		BRepCheck::Add(myMap(myShape),BRepCheck_SelfIntersectingWire);
		}
#ifdef DEB
	      static Standard_Integer numpoint1=0;
	      cout<<"point p"<<++numpoint1<<" "<<P3d.X()<<" "<<P3d.Y()<<" "<<P3d.Z()<<endl;
	      cout.flush();
#endif
	      delete [] tabDom;
	      return(BRepCheck_SelfIntersectingWire);
	    } //-- localok == False
	  } //end of if(Tr1.PositionOnCurve() == IntRes2d_Middle || Tr2.PositionOnCurve() == IntRes2d_Middle)
	} //end of for (Standard_Integer p=1; p <= nbp; p++)    
	////
	//// **** Segments of intersection **** ////
	for (Standard_Integer s = 1; s <= nbs; ++s) {
	  const IntRes2d_IntersectionSegment& Seg = Inter.Segment(s);
	  if (Seg.HasFirstPoint() && Seg.HasLastPoint())   { 
	    Standard_Boolean localok;
	    Standard_Integer k;
	    IntRes2d_IntersectionPoint PSeg [2];
	    IntRes2d_Position aPCR1, aPCR2;
	    //
	    localok = Standard_False;
	    PSeg[0] = Seg.FirstPoint();
	    PSeg[1] = Seg.LastPoint();
	    // At least one of extremities of the segment must be inside
	    // the tolerance of a common vertex
	    for (k = 0; k < 2; ++k) {
	      IP_ParamOnFirst  = PSeg[k].ParamOnFirst();
	      IP_ParamOnSecond = PSeg[k].ParamOnSecond();
	      Tr1 = PSeg[k].TransitionOfFirst();
	      Tr2 = PSeg[k].TransitionOfSecond();
	      aPCR1=Tr1.PositionOnCurve();
	      aPCR2=Tr2.PositionOnCurve();
	      //
	      if(aPCR1!=IntRes2d_Middle && aPCR2!=IntRes2d_Middle)  {
		GeomAbs_CurveType aCT1, aCT2;
		//ZZ
		aCT1=C1.GetType();
		aCT2=C2.GetType();
		if (aCT1==GeomAbs_Line && aCT2==GeomAbs_Line) {
		  // check for the two lines coincidence
		  Standard_Real aPAR_T, aT11, aT12, aT21, aT22, aT1m, aT2m;
		  Standard_Real aD2, aTolE1, aTolE2,  aTol2, aDot;
		  gp_Lin2d aL1, aL2;
		  gp_Pnt2d aP1m;
		  //
		  aPAR_T=0.43213918;
		  //
		  aTolE1=BRep_Tool::Tolerance(E1);
		  aTolE2=BRep_Tool::Tolerance(E2);
		  aTol2=aTolE1+aTolE2;
		  aTol2=aTol2*aTol2;
		  //
		  aL1=C1.Line();
		  aL2=C2.Line();
		  //
		  aT11=PSeg[0].ParamOnFirst();
		  aT12=PSeg[1].ParamOnFirst();
		  aT21=PSeg[0].ParamOnSecond();
		  aT22=PSeg[1].ParamOnSecond();
		  //
		  aT1m=(1.-aPAR_T)*aT11 + aPAR_T*aT12;
		  aP1m=C1.Value(aT1m);
		  //
		  aD2=aL2.SquareDistance(aP1m);
		  if (aD2<aTol2) {
		    aT2m=ElCLib::Parameter(aL2, aP1m);
		    if (aT2m>aT21 && aT2m<aT22) {
		      const gp_Dir2d& aDir1=aL1.Direction();
		      const gp_Dir2d& aDir2=aL2.Direction();
		      aDot=aDir1*aDir2;
		      if (aDot<0.) {
			aDot=-aDot;
		      }
		      //
		      if ((1.-aDot)<5.e-11){//0.00001 rad
			localok = Standard_False;
			break;// from for (k = 0; k < 2; ++k){...
		      }
		    }//if (aT2m>aT21 && aT2m<aT22) {
		  }//if (aD2<aTol2) {
		}//if (aCT1==GeomAbs_Line && aCT2==GeomAbs_Line) {
		//ZZ
		localok = Standard_True;
		break;
	      }
	      //
	      Standard_Real f,l, tolvtt;
	      TopLoc_Location L, L2;
	      const Handle(Geom_Curve)& ConS = BRep_Tool::Curve(E1,L,f,l);    
	      const Handle(Geom_Curve)& ConS2 = BRep_Tool::Curve(E2,L2,f,l);    
	      //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
	      if (!ConS.IsNull()) { 
		P3d = ConS->Value(IP_ParamOnFirst); 
		P3d.Transform(L.Transformation());
	      } else {
		gp_Pnt2d aP2d  = C1.Value(IP_ParamOnFirst);
		P3d = HS->Value(aP2d.X(), aP2d.Y());
	      }
	      if (!ConS2.IsNull()) {
		P3d2 = ConS2->Value(IP_ParamOnSecond); 
		P3d2.Transform(L2.Transformation());
	      } else {
		gp_Pnt2d aP2d  = C2.Value(IP_ParamOnSecond);
		P3d2 = HS->Value(aP2d.X(), aP2d.Y());
	      }
	      //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
	      itl.Initialize( CommonVertices );
	      for (; itl.More(); itl.Next()) {
		Standard_Real p3dvttDistanceP3d, p3dvttDistanceP3d2;
		gp_Pnt p3dvtt;
		//
		const TopoDS_Vertex& vtt = TopoDS::Vertex(itl.Value());
		p3dvtt = BRep_Tool::Pnt(vtt);
		tolvtt =  BRep_Tool::Tolerance(vtt);
		tolvtt=1.1*tolvtt;
		tolvtt=tolvtt*tolvtt;
		p3dvttDistanceP3d  = p3dvtt.SquareDistance(P3d);
		p3dvttDistanceP3d2 = p3dvtt.SquareDistance(P3d2);
		if (p3dvttDistanceP3d <= tolvtt && p3dvttDistanceP3d2 <= tolvtt) { 
		  localok = Standard_True;
		  break;
		}
	      }
	      if (localok == Standard_True) {
		break;
	      }
	    } //end of for (k = 0; k < 2; k++)
	    //
	    if(localok==Standard_False)	  { 
	      ok=0;
	      retE1=E1;
	      retE2=E2;
	      if (Update) {
		BRepCheck::Add(myMap(myShape),BRepCheck_SelfIntersectingWire);
	      }
#ifdef DEB
	      static Standard_Integer numpoint1=0;
	      cout<<"point p"<<++numpoint1<<" "<<P3d.X()<<" "<<P3d.Y()<<" "<<P3d.Z()<<endl;
	      cout.flush();
#endif
	      delete [] tabDom;
	      return(BRepCheck_SelfIntersectingWire);
	    } //-- localok == False
	  } //end of if(Seg.HasFirstPoint() && Seg.HasLastPoint())
	} //end of for (Standard_Integer s = 1; s <= nbs; p++)
      } //-- Inter.IsDone()
    } //end of for( j = i+1; j<=Nbedges; j++)
  } //end of for(i = 1; i <= Nbedges; i++)
  //
  delete [] tabDom;
  if (Update) {
    BRepCheck::Add(myMap(myShape),BRepCheck_NoError);
  }
  //
  return (BRepCheck_NoError);
}
//=======================================================================
//function : GeometricControls
//purpose  : 
//=======================================================================
void BRepCheck_Wire::GeometricControls(const Standard_Boolean B)
{
  if (myGctrl != B) {
    if (B) {
      myCdone = Standard_False;
    }
    myGctrl = B;
  }
}
//=======================================================================
//function : GeometricControls
//purpose  : 
//=======================================================================
Standard_Boolean BRepCheck_Wire::GeometricControls() const
{
  return myGctrl;
}


//=======================================================================
//function : Propagate
//purpose  : fill <mapE> with edges connected to <edg> through vertices
//           contained in <mapVE>
//=======================================================================
static void Propagate(const TopTools_IndexedDataMapOfShapeListOfShape& mapVE,
		      const TopoDS_Shape& edg,
		      TopTools_MapOfShape& mapE)
{
  if (mapE.Contains(edg)) {
    return;
  }
  mapE.Add(edg); // attention, si oriented == Standard_True, edg doit
                 // etre FORWARD ou REVERSED. Ce n`est pas verifie.
                 // =============
                 // attention, if oriented == Standard_True, <edg> must
                 // be FORWARD or REVERSED. That is not checked.
  
  TopExp_Explorer ex;
  for (ex.Init(edg,TopAbs_VERTEX); ex.More(); ex.Next()) {
    const TopoDS_Vertex& vtx = TopoDS::Vertex(ex.Current());
    // debug jag sur vertex
    Standard_Integer indv = mapVE.FindIndex(vtx);
    if (indv != 0) {
      for (TopTools_ListIteratorOfListOfShape itl(mapVE(indv)); itl.More(); itl.Next()) {
	if (!itl.Value().IsSame(edg) &&
	    !mapE.Contains(itl.Value())) {
	  Propagate(mapVE,itl.Value(),mapE);
	}
      }
    }
  }
}
//=======================================================================
//function : GetOrientation
//purpose  : 
//=======================================================================

static TopAbs_Orientation GetOrientation(const TopTools_MapOfShape& mapE,
					 const TopoDS_Edge& edg)
{
  TopTools_MapIteratorOfMapOfShape itm(mapE);
  for ( ; itm.More(); itm.Next()) {
    if (itm.Key().IsSame(edg)) {
      break;
    }
  }
  return itm.Key().Orientation();
}
//=======================================================================
//function : ChoixUV
//purpose  : 
//=======================================================================
 void ChoixUV(const TopoDS_Vertex& V,
	      const TopoDS_Edge& Edg,
	      const TopoDS_Face& F,
	      TopTools_ListOfShape& L)
{
  TopTools_ListIteratorOfListOfShape It( L );
  while (It.More())
    {
      if (Edg.IsSame( It.Value() ))
	L.Remove( It );
      else
	It.Next();
    }

  Standard_Integer index = 0, imin = 0;
  TopoDS_Edge Evois;
  gp_Pnt2d PntRef, Pnt;
  gp_Vec2d DerRef, Der;
  Standard_Real MinAngle, MaxAngle, angle;
  Standard_Real gpResolution=gp::Resolution();
  TopAbs_Orientation aVOrientation, aEdgOrientation;
#ifndef DEB
  Standard_Real dist2d = 0, p = 0;
#else
  Standard_Real dist2d, p;
#endif
  Standard_Real f, l, parpiv;
  Standard_Real tolv = BRep_Tool::Tolerance(V);
  BRepAdaptor_Surface Ads(F,Standard_False); // no restriction
  Standard_Real ures = Ads.UResolution(tolv);
  Standard_Real vres = Ads.VResolution(tolv);
  Standard_Real tol = Max(ures,vres);
  if(tol<=0.0) { 
#ifdef DEB 
    //-- lbr le 29 jan 98 
    cout<<"BRepCheck_Wire : UResolution et VResolution = 0.0 (Face trop petite ?)"<<endl;cout.flush();
#endif
  }
  else {
    tol += tol; //pour YFR.
  }
  //
  Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(Edg, F, f, l);
  if (C2d.IsNull()) {// JAG 10.12.96
    return;
  }
  
  aVOrientation=V.Orientation();
  aEdgOrientation=Edg.Orientation();

  parpiv =(aVOrientation==aEdgOrientation) ? f : l;
      
  MinAngle = RealLast();
  MaxAngle = RealFirst();

  CurveDirForParameter(C2d, parpiv, PntRef, DerRef);

  if (aVOrientation != aEdgOrientation){
    DerRef.Reverse();
  }
  //
  It.Initialize(L);
  for (; It.More(); It.Next()) {
    index++;
    const TopoDS_Edge& aE=TopoDS::Edge(It.Value());
    C2d = BRep_Tool::CurveOnSurface(aE, F, f, l);
    if(C2d.IsNull()) {
      continue;
    }

    p =(aVOrientation != aE.Orientation()) ? f : l;
    //
    Pnt = C2d->Value(p);
    dist2d = Pnt.Distance( PntRef );
    if (dist2d > tol){
      continue;
    }
    //
    CurveDirForParameter(C2d, p, Pnt, Der);
    
    if (aVOrientation == aE.Orientation()){
      Der.Reverse();
    }
    
    if (DerRef.Magnitude() <= gpResolution || 
	Der.Magnitude() <= gpResolution){
      continue;
    }
    //
    angle = DerRef.Angle( Der );
    angle *= -1.;
    if (angle < 0.)
      angle += 2.*PI;
    
    if (F.Orientation() == TopAbs_FORWARD) { 
      if (angle < MinAngle) {
	imin = index;
	MinAngle = angle;
      }
    } 
    else { //F.Orientation() != TopAbs_FORWARD
      if (angle > MaxAngle){
	imin = index;
	MaxAngle = angle;
      }
    }
  }//end of for
  //
  // Mise a jour ledge
  if (imin == 0)
    if (L.Extent() == 1) {
      Standard_Boolean onjette = 0; //all right
      Evois = TopoDS::Edge(L.First());
      if (dist2d > tol) {
#ifdef DEB 
	cout<<"BRepCheckWire : controle fermeture en 2d --> faux"<<endl;cout.flush();
#endif
	if(Evois.IsNull() || BRep_Tool::Degenerated(Edg) ||
	   BRep_Tool::Degenerated(Evois)){
	  onjette = 1; //bad
	}
	else {
	  Ads.Initialize(F);
	  Standard_Real dumax = 0.01 * (Ads.LastUParameter() - Ads.FirstUParameter());
	  Standard_Real dvmax = 0.01 * (Ads.LastVParameter() - Ads.FirstVParameter());
	  Standard_Real dumin = Abs(Pnt.X() - PntRef.X());
	  Standard_Real dvmin = Abs(Pnt.Y() - PntRef.Y());
	  if(dumin > dumax || dvmin > dvmax){
	    onjette = 1;
	  }
	  else {
	    BRepAdaptor_Curve bcEdg(Edg,F);
	    BRepAdaptor_Curve bcEvois(Evois,F);
	    gp_Pnt pEdg = bcEdg.Value(parpiv);
	    gp_Pnt pEvois = bcEvois.Value(p);
	    Standard_Real d3d = pEvois.Distance(pEdg);
#ifdef DEB
	    cout<<"point P            "<<pEdg.X()<<" "<<pEdg.Y()<<" "<<pEdg.Z()<<endl;
	    cout<<"distance 3d      : "<<d3d<<endl;
	    cout<<"tolerance vertex : "<<tolv<<endl;
	    cout.flush();
#endif
	    //if(d3d > tolv){
	    if(d3d > 2.*tolv){
	      onjette = 1;
	    }
#ifdef DEB
	    else
	      cout<<"controle fermeture en 3d --> ok"<<endl;cout.flush();
#endif
	  }
	}
      } //if (dist2d > tol)
      else {//angle was not defined but points are close
	onjette = 0;
      }
      if(onjette) {
#ifdef DEB
	cout<<"controle fermeture en 3d --> faux"<<endl;cout.flush();
#endif
	L.Clear();
      }
    }
    else {
      L.Clear();
    }
  else  {
    index = 1;
    while (index < imin) {
      L.RemoveFirst();
      index++;
    }
    It.Initialize(L);
    It.Next();
    while (It.More())
      L.Remove(It);
  }
}
//=======================================================================
//function : CurveDirForParameter
//purpose  : 
//=======================================================================
void CurveDirForParameter(const Handle(Geom2d_Curve)& aC2d,
			  const Standard_Real aPrm,
			  gp_Pnt2d& Pnt,
			  gp_Vec2d& aVec2d)
{
  Standard_Real aTol=gp::Resolution();
  Standard_Integer i;

  aC2d->D1(aPrm, Pnt, aVec2d);
  //
  if (aVec2d.Magnitude() <= aTol) {
    for (i = 2; i <= 100; i++){
      aVec2d = aC2d->DN(aPrm, i);
      if (aVec2d.Magnitude() > aTol) {
	break;
      }
    }
  }
}

//  Modified by Sergey KHROMOV - Wed May 22 10:44:06 2002 OCC325 Begin
//=======================================================================
//function : GetPnts2d
//purpose  : this function returns the parametric points of theVertex on theFace.
//           If theVertex is a start and end vertex of theEdge hasSecondPnt
//           becomes Standard_True and aPnt2 returns the second parametric point.
//           Returns Standard_True if paraametric points are successfully found.
//=======================================================================

static Standard_Boolean GetPnt2d(const TopoDS_Vertex    &theVertex,
				 const TopoDS_Edge      &theEdge,
				 const TopoDS_Face      &theFace,
				       gp_Pnt2d         &aPnt)
{
  Handle(Geom2d_Curve) aPCurve;
  Standard_Real        aFPar;
  Standard_Real        aLPar;
  Standard_Real        aParOnEdge;
  TopoDS_Vertex        aFirstVtx;
  TopoDS_Vertex        aLastVtx;

  TopExp::Vertices(theEdge, aFirstVtx, aLastVtx);

  if (!theVertex.IsSame(aFirstVtx) && !theVertex.IsSame(aLastVtx))
    return Standard_False;

  aPCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, aFPar, aLPar);

  if (aPCurve.IsNull())
    return Standard_False;

  aParOnEdge = BRep_Tool::Parameter(theVertex, theEdge);
  aPnt       = aPCurve->Value(aParOnEdge);

  return Standard_True;
}

//=======================================================================
//function : Closed2dForPeriodicFace
//purpose  : Checks the distance between first point of the first edge
//           and last point of the last edge in 2d for periodic face.
//=======================================================================
static Standard_Boolean IsClosed2dForPeriodicFace
                        (const TopoDS_Face   &theFace,
			 const gp_Pnt2d      &theP1,
			 const gp_Pnt2d      &theP2,
			 const TopoDS_Vertex &theVertex)
{
// Check 2d distance for periodic faces with seam edge.
// Searching for seam edges
  TopTools_ListOfShape aSeamEdges;
  TopTools_MapOfShape  NotSeams;
  TopTools_MapOfShape  ClosedEdges;
  TopExp_Explorer      anExp(theFace, TopAbs_EDGE);

  for (;anExp.More(); anExp.Next()) {
    TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());

    if (NotSeams.Contains(anEdge))
      continue;

    if (!IsOriented(anEdge) ||
	!BRep_Tool::IsClosed(anEdge, theFace)) {
      NotSeams.Add(anEdge);
      continue;
    }

    if (!ClosedEdges.Add(anEdge))
      aSeamEdges.Append(anEdge);
  }

  if (aSeamEdges.Extent() == 0)
    return Standard_True;

// check if theVertex lies on one of the seam edges
  BRepAdaptor_Surface aFaceSurface (theFace, Standard_False);
  Standard_Real       aTol      = BRep_Tool::Tolerance(theVertex);
  Standard_Real       aUResol   = aFaceSurface.UResolution(aTol);
  Standard_Real       aVResol   = aFaceSurface.VResolution(aTol);
  Standard_Real       aVicinity = Sqrt(aUResol*aUResol + aVResol*aVResol);
  Standard_Real       aDistP1P2 = theP1.Distance(theP2);


  TopTools_ListIteratorOfListOfShape anIter(aSeamEdges);

  for (; anIter.More(); anIter.Next()) {
    TopoDS_Edge aSeamEdge = TopoDS::Edge(anIter.Value());

    anExp.Init(aSeamEdge, TopAbs_VERTEX);
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aVtx = anExp.Current();

// We found an edge. Check the distance between two given points
//  to be lower than the computed tolerance.
      if (IsOriented(aVtx) && aVtx.IsSame(theVertex)) {
	gp_Pnt2d         aPnt1;
	gp_Pnt2d         aPnt2;
	Standard_Real    a2dTol;

	if (!GetPnt2d(theVertex, aSeamEdge, theFace, aPnt1))
	  continue;

	aSeamEdge = TopoDS::Edge(aSeamEdge.Reversed());

	if (!GetPnt2d(theVertex, aSeamEdge, theFace, aPnt2))
	  continue;

	a2dTol = aPnt1.Distance(aPnt2)*1.e-2;
	a2dTol = Max(a2dTol, aVicinity);

	if (aDistP1P2 > a2dTol)
	  return Standard_False;
      }
    }
  }

  return Standard_True;
}
//  Modified by Sergey KHROMOV - Thu Jun 20 10:58:05 2002 End
