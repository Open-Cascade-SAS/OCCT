// File:	BRepCheck_Face.cxx
// Created:	Fri Dec 15 11:49:58 1995
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <BRepCheck_Face.ixx>

#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>

#include <BRep_TFace.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>

//#include <BRepAdaptor_Curve2d.hxx>
#include <BRepClass_FaceClassifier.hxx>
//#include <Geom2dInt_GInter.hxx>
#include <Geom2d_Curve.hxx>
#include <GProp_GProps.hxx>

#include <IntRes2d_Domain.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Pnt2d.hxx>



#include <BRepCheck.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

#include <BRepTopAdaptor_FClass2d.hxx>
#include <TopAbs_State.hxx>

#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopoDS_Vertex.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <BRepAdaptor_HSurface.hxx>


static Standard_Boolean Intersect(const TopoDS_Wire&,
				  const TopoDS_Wire&,
				  const TopoDS_Face&);
				  

static Standard_Boolean IsInside(const TopoDS_Wire& wir,
				 const Standard_Boolean Inside,
				 const BRepTopAdaptor_FClass2d& FClass2d,
				 const TopoDS_Face& F);

static Standard_Boolean CheckThin(const TopoDS_Shape& w,
				  const TopoDS_Shape& f);

//=======================================================================
//function : BRepCheck_Face
//purpose  : 
//=======================================================================

BRepCheck_Face::BRepCheck_Face (const TopoDS_Face& F)
{
  Init(F);
  myIntdone = Standard_False;
  myImbdone = Standard_False;
  myOridone = Standard_False;
  myGctrl   = Standard_True;
}

//=======================================================================
//function : Minimum
//purpose  : 
//=======================================================================

void BRepCheck_Face::Minimum()
{
  if (!myMin) {
    BRepCheck_ListOfStatus thelist;
    myMap.Bind(myShape, thelist);
    BRepCheck_ListOfStatus& lst = myMap(myShape);

    Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*) &myShape.TShape());
    if (TF->Surface().IsNull()) {
      BRepCheck::Add(lst,BRepCheck_NoSurface);
    }
    else {
      // Flag natural restriction???
    }
    if (lst.IsEmpty()) {
      lst.Append(BRepCheck_NoError);
    }
    myMin = Standard_True;
  }
}


//=======================================================================
//function : InContext
//purpose  : 
//=======================================================================

void BRepCheck_Face::InContext(const TopoDS_Shape& S)
{
  if (myMap.IsBound(S)) {
    return;
  }
  BRepCheck_ListOfStatus thelist;
  myMap.Bind(S, thelist);

  BRepCheck_ListOfStatus& lst = myMap(S);
  
  TopExp_Explorer exp(S,TopAbs_FACE);
  for (; exp.More(); exp.Next()) {
    if (exp.Current().IsSame(myShape)) {
      break;
    }
  }
  if (!exp.More()) {
    BRepCheck::Add(lst,BRepCheck_SubshapeNotInShape);
    return;
  }

  if (lst.IsEmpty()) {
    lst.Append(BRepCheck_NoError);
  }
}


//=======================================================================
//function : Blind
//purpose  : 
//=======================================================================

void BRepCheck_Face::Blind()
{
  if (!myBlind) {
    // rien de plus que dans le minimum
    myBlind = Standard_True;
  }
}


//=======================================================================
//function : IntersectWires
//purpose  : 
//=======================================================================

BRepCheck_Status BRepCheck_Face::IntersectWires(const Standard_Boolean Update)
{
  if (myIntdone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myIntres);
    }
    return myIntres;
  }

  myIntdone = Standard_True;
  myIntres = BRepCheck_NoError;
  // This method has to be called by an analyzer. It is assumed that
  // each edge has a correct 2d representation on the face.

  TopExp_Explorer exp1,exp2;

  // on mape les wires
  exp1.Init(myShape.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
  TopTools_ListOfShape theListOfShape;
  while (exp1.More()) {
    if (!myMapImb.IsBound(exp1.Current())) {
      myMapImb.Bind(exp1.Current(), theListOfShape);
    }
    else { // on a 2 fois le meme wire...
      myIntres = BRepCheck_RedundantWire;
      if (Update) {
	BRepCheck::Add(myMap(myShape),myIntres);
      }
      return myIntres;
    }
    exp1.Next();
  }
  
  Standard_Integer Nbwire, Index,Indexbis;
  Nbwire = myMapImb.Extent();

  Index = 1;
  while (Index < Nbwire) {
    for (exp1.Init(myShape,TopAbs_WIRE),Indexbis = 0;
	 exp1.More();exp1.Next()) {
      Indexbis++;
      if (Indexbis == Index) {
	break;
      }
    }
    TopoDS_Wire wir1 = TopoDS::Wire(exp1.Current());
    exp1.Next();
    for (; exp1.More(); exp1.Next()) {
      const TopoDS_Wire& wir2 = TopoDS::Wire(exp1.Current());
      if (Intersect(wir1,wir2,TopoDS::Face(myShape))) {
	myIntres = BRepCheck_IntersectingWires;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myIntres);
	}
	return myIntres;
      }
    }
    Index++;
  }
  if (Update) {
    BRepCheck::Add(myMap(myShape),myIntres);
  }
  return myIntres;
}


//=======================================================================
//function : ClassifyWires
//purpose  : 
//=======================================================================

BRepCheck_Status BRepCheck_Face::ClassifyWires(const Standard_Boolean Update)
{
  // It is assumed that each wire does not intersect any other one.
  if (myImbdone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myImbres);
    }
    return myImbres;
  }

  myImbdone = Standard_True;
  myImbres = IntersectWires();
  if (myImbres != BRepCheck_NoError) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myImbres);
    }
    return myImbres;
  }

  Standard_Integer Nbwire = myMapImb.Extent();
  if (Nbwire < 1) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myImbres);
    }
    return myImbres;
  }

  BRep_Builder B;
  TopExp_Explorer exp1,exp2;
  TopTools_ListOfShape theListOfShape;
  for (exp1.Init(myShape.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
       exp1.More();exp1.Next()) {

    const TopoDS_Wire& wir1 = TopoDS::Wire(exp1.Current());
    TopoDS_Shape aLocalShape = myShape.EmptyCopied();
    TopoDS_Face newFace = TopoDS::Face(aLocalShape);
//    TopoDS_Face newFace = TopoDS::Face(myShape.EmptyCopied());
#ifdef DEB
    TopAbs_Orientation orWire = 
#endif
      wir1.Orientation();
    newFace.Orientation(TopAbs_FORWARD);
    B.Add(newFace,wir1);

    BRepTopAdaptor_FClass2d FClass2d(newFace,Precision::PConfusion());
    Standard_Boolean WireBienOriente = Standard_False;
    if(FClass2d.PerformInfinitePoint() != TopAbs_OUT) { 
      WireBienOriente=Standard_True;
      //le wire donne definit un trou
      myMapImb.UnBind(wir1);
      myMapImb.Bind(wir1.Reversed(), theListOfShape);
    }

    for (exp2.Init(myShape.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
	 exp2.More();exp2.Next()) {
      const TopoDS_Wire& wir2 = TopoDS::Wire(exp2.Current());
      if (!wir2.IsSame(wir1)) {
	
	if (IsInside(wir2,WireBienOriente,FClass2d,newFace)) { 
	  myMapImb(wir1).Append(wir2);
	}
      }
    }
  }
  // On doit avoir 1 wire qui contient tous les autres, et les autres 
  // ne contenant rien (cas solide fini) ou
  // que des wires ne contenant rien : dans ce cas les wires doivent etre
  // des trous dans une face infinie.
  TopoDS_Wire Wext;
  for (TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(myMapImb);
       itm.More();
       itm.Next()) {
    if (!itm.Value().IsEmpty()) {
      if (Wext.IsNull()) {
	Wext = TopoDS::Wire(itm.Key());
      }
      else {
	myImbres = BRepCheck_InvalidImbricationOfWires;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myImbres);
	}
	return myImbres;
      }
    }
  }

  if (!Wext.IsNull()) {
    // verifies that the list contains nbwire-1 elements
    if (myMapImb(Wext).Extent() != Nbwire-1) {
      myImbres = BRepCheck_InvalidImbricationOfWires;
      if (Update) {
	BRepCheck::Add(myMap(myShape),myImbres);
      }
      return myImbres;
    }
  }
  // sortie sans erreurs
  if (Update) {
    BRepCheck::Add(myMap(myShape),myImbres);
  }
  return myImbres;
  
}


//=======================================================================
//function : OrientationOfWires
//purpose  : 
//=======================================================================

BRepCheck_Status BRepCheck_Face::OrientationOfWires
   (const Standard_Boolean Update)
{
  // WARNING : it is assumed that the edges of a wire are correctly oriented


  Standard_Boolean Infinite = myShape.Infinite();

  if (myOridone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myOrires);
    }
    return myOrires;
  }

  myOridone = Standard_True;
  myOrires = ClassifyWires();
  if (myOrires != BRepCheck_NoError) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myOrires);
    }
    return myOrires;
  }

  Standard_Integer Nbwire = myMapImb.Extent();
  TopoDS_Wire Wext;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(myMapImb);
  if (Nbwire == 1) {
    if (!Infinite) {
      Wext = TopoDS::Wire(itm.Key());
    }
  }
  else {
    for (;itm.More();itm.Next()) {
      if (!itm.Value().IsEmpty()) {
	Wext = TopoDS::Wire(itm.Key());
      }
    }
  }

  if (Wext.IsNull() && !Infinite) {
    if (Nbwire>0) myOrires = BRepCheck_InvalidImbricationOfWires;
    if (Update) {
      BRepCheck::Add(myMap(myShape),myOrires);
    }
    return myOrires;
  }

  // BRep_Builder B;
  TopExp_Explorer exp(myShape.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
  for (; exp.More(); exp.Next()) {
    const TopoDS_Wire& wir = TopoDS::Wire(exp.Current());
    if (!Wext.IsNull() && wir.IsSame(Wext)) {
      if (wir.Orientation() != Wext.Orientation()) {
	//le wire exterieur definit un trou
	if( CheckThin(wir,myShape.Oriented(TopAbs_FORWARD)) )
	  return myOrires;
	myOrires = BRepCheck_BadOrientationOfSubshape;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myOrires);
	}
	return myOrires;
      }
    }
    else {
      for (itm.Reset(); itm.More(); itm.Next()) {
	if (itm.Key().IsSame(wir)) {
	  break;
	}
      }
      // Pas de controle sur More()
      if (itm.Key().Orientation() == wir.Orientation()) {
	//le wire donne ne definit pas un trou
	myOrires = BRepCheck_BadOrientationOfSubshape;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myOrires);
	}
	return myOrires;
      }
    }
  }
  // sortie sans erreur
  if (Update) {
    BRepCheck::Add(myMap(myShape),myOrires);
  }
  return myOrires;
}


//=======================================================================
//function : SetUnorientable
//purpose  : 
//=======================================================================

void BRepCheck_Face::SetUnorientable()
{
  BRepCheck::Add(myMap(myShape),BRepCheck_UnorientableShape);
}


//=======================================================================
//function : IsUnorientable
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Face::IsUnorientable() const
{
  if (myOridone) {
    return (myOrires != BRepCheck_NoError);
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
//function : GeometricControls
//purpose  : 
//=======================================================================

void BRepCheck_Face::GeometricControls(const Standard_Boolean B)
{
  if (myGctrl != B) {
    if (B) {
      myIntdone = Standard_False; 
      myImbdone = Standard_False; 
      myOridone = Standard_False; 
    }
    myGctrl = B;
  }
}


//=======================================================================
//function : GeometricControls
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Face::GeometricControls() const
{
  return myGctrl;
}


//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

static Standard_Boolean Intersect(const TopoDS_Wire& wir1,
				  const TopoDS_Wire& wir2,
				  const TopoDS_Face& F)
{
  Standard_Real Inter2dTol = 1.e-10;
  TopExp_Explorer exp1,exp2;
//  BRepAdaptor_Curve2d cur1,cur2;

  //Find common vertices of two wires - non-manifold case
  TopTools_MapOfShape MapW1;
  TopTools_SequenceOfShape CommonVertices;
  for (exp1.Init( wir1, TopAbs_VERTEX ); exp1.More(); exp1.Next())
    MapW1.Add( exp1.Current() );
  for (exp2.Init( wir2, TopAbs_VERTEX ); exp2.More(); exp2.Next())
    {
      TopoDS_Shape V = exp2.Current();
      if (MapW1.Contains( V ))
	CommonVertices.Append( V );
    }

  // MSV 03.04.2002: create pure surface adaptor to avoid UVBounds computation
  //                 due to performance problem
  BRepAdaptor_Surface Surf(F,Standard_False);

  TColgp_SequenceOfPnt PntSeq;
  Standard_Integer i;
  for (i = 1; i <= CommonVertices.Length(); i++)
    {
      TopoDS_Vertex V = TopoDS::Vertex( CommonVertices(i) );
      gp_Pnt2d P2d = BRep_Tool::Parameters( V, F );
      gp_Pnt P = Surf.Value( P2d.X(), P2d.Y() );
      PntSeq.Append( P );
    }

  Geom2dAdaptor_Curve   C1,C2;
  gp_Pnt2d              pfirst1,plast1,pfirst2,plast2;
  Standard_Real         first1,last1,first2,last2;
  Geom2dInt_GInter      Inter;
  IntRes2d_Domain myDomain1,myDomain2;
  Bnd_Box2d Box1, Box2;

  for (exp1.Init(wir1,TopAbs_EDGE); exp1.More(); exp1.Next())
    {
      const TopoDS_Edge& edg1 = TopoDS::Edge(exp1.Current());
      //    cur1.Initialize(edg1,F);
      C1.Load( BRep_Tool::CurveOnSurface(edg1,F,first1,last1) );
      // To avoid exeption in Segment if C1 is BSpline - IFV
      if(C1.FirstParameter() > first1) first1 = C1.FirstParameter();
      if(C1.LastParameter()  < last1 ) last1  = C1.LastParameter();

      BRep_Tool::UVPoints(edg1,F,pfirst1,plast1);
      myDomain1.SetValues( pfirst1, first1, Inter2dTol, plast1, last1, Inter2dTol );
      Box1.SetVoid();
      BndLib_Add2dCurve::Add( C1, first1, last1, 0., Box1 );
      
      for (exp2.Init(wir2,TopAbs_EDGE); exp2.More(); exp2.Next())
	{
	  const TopoDS_Edge& edg2 = TopoDS::Edge(exp2.Current());
	  if (!edg1.IsSame(edg2))
	    {
	      //cur2.Initialize(edg2,F);
	      C2.Load( BRep_Tool::CurveOnSurface(edg2,F,first2,last2) );
	      // To avoid exeption in Segment if C2 is BSpline - IFV
	      if(C2.FirstParameter() > first2) first2 = C2.FirstParameter();
	      if(C2.LastParameter()  < last2 ) last2  = C2.LastParameter();

              Box2.SetVoid();
	      BndLib_Add2dCurve::Add( C2, first2, last2, 0., Box2 );
	      if (! Box1.IsOut( Box2 ))
		{
		  BRep_Tool::UVPoints(edg2,F,pfirst2,plast2);
		  myDomain2.SetValues( pfirst2, first2, Inter2dTol, plast2, last2, Inter2dTol );
		  Inter.Perform( C1, myDomain1, C2, myDomain2, Inter2dTol, Inter2dTol );
		  if (!Inter.IsDone())
		    return Standard_True;
		  if (Inter.NbSegments() > 0)
		    {
		      if (PntSeq.IsEmpty())
			return Standard_True;
		      else
			{
			  Standard_Integer NbCoinc = 0;
			  for (i = 1; i <= Inter.NbSegments(); i++)
			    {
			      if (!Inter.Segment(i).HasFirstPoint() || !Inter.Segment(i).HasLastPoint())
				return Standard_True;
			      gp_Pnt2d FirstP2d = Inter.Segment(i).FirstPoint().Value();
			      gp_Pnt2d LastP2d = Inter.Segment(i).LastPoint().Value();
			      gp_Pnt FirstP = Surf.Value( FirstP2d.X(), FirstP2d.Y() );
			      gp_Pnt LastP = Surf.Value( LastP2d.X(), LastP2d.Y() );
			      for (Standard_Integer j = 1; j <= PntSeq.Length(); j++)
				{
				  Standard_Real tolv = BRep_Tool::Tolerance( TopoDS::Vertex(CommonVertices(j)) );
				  if (FirstP.IsEqual( PntSeq(j), tolv ) || LastP.IsEqual( PntSeq(j), tolv ))
				    {
				      NbCoinc++;
				      break;
				    }
				}
			    }
			  if (NbCoinc == Inter.NbSegments())
			    return Standard_False;
			  return Standard_True;
			}
		    }
		  if (Inter.NbPoints() > 0)
		    {
		      if (PntSeq.IsEmpty())
			return Standard_True;
		      else
			{
			  Standard_Integer NbCoinc = 0;
			  for (i = 1; i <= Inter.NbPoints(); i++)
			    {
			      gp_Pnt2d P2d = Inter.Point(i).Value();
			      gp_Pnt P = Surf.Value( P2d.X(), P2d.Y() );
			      for (Standard_Integer j = 1; j <= PntSeq.Length(); j++)
				{
				  Standard_Real tolv = BRep_Tool::Tolerance( TopoDS::Vertex(CommonVertices(j)) );
				  if (P.IsEqual( PntSeq(j), tolv ))
				    {
				      NbCoinc++;
				      break;
				    }
				}
			    }
			  if (NbCoinc == Inter.NbPoints())
			    return Standard_False;
			  return Standard_True;
			}
		    }
		}
	    }
	}
    }
  return Standard_False;
}


//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================

static Standard_Boolean IsInside(const TopoDS_Wire& wir,
				 const Standard_Boolean WireBienOriente,
				 const BRepTopAdaptor_FClass2d& FClass2d,
				 const TopoDS_Face& F)
{
  // Standard_Real U,V;
  TopExp_Explorer exp;
  for (exp.Init(wir,TopAbs_EDGE);
       exp.More();
       exp.Next()) {

    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    Standard_Real f,l;
    Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(edg,F,f,l);
    Standard_Real prm;

    if (!Precision::IsNegativeInfinite(f) && 
	!Precision::IsPositiveInfinite(l)) {
      prm = (f+l)/2.;
    }
    else {
      if (Precision::IsNegativeInfinite(f) && 
	  Precision::IsPositiveInfinite(l)){
	prm = 0.;
      }
      else if (Precision::IsNegativeInfinite(f)) {
	prm = l-1.;
      }
      else {
	prm = f+1.;
      }
    }

    gp_Pnt2d pt2d(C2d->Value(prm));
#ifdef DEB
    TopAbs_State st2=
#endif
      FClass2d.Perform(pt2d,Standard_False);
//--    if(st1!=st2) { 
//--      static int p=0;
//--      printf("\n point p%d %g %g \n",++p,pt2d.X(),pt2d.Y());
//--    }
    
    if(WireBienOriente) { 
#ifndef DEB
      return(FClass2d.Perform(pt2d,Standard_False) == TopAbs_OUT);     
    }
    else { 
      return(FClass2d.Perform(pt2d,Standard_False) == TopAbs_IN);
    }
#else
      return(st2 == TopAbs_OUT);     
    }
    else { 
      return(st2 == TopAbs_IN);
    }
#endif
  }
  return Standard_False;
}

Standard_Boolean CheckThin(const TopoDS_Shape& w, const TopoDS_Shape& f)
{
  TopoDS_Face aF = TopoDS::Face(f);
  TopoDS_Wire aW = TopoDS::Wire(w);

  Standard_Integer nbE = 0;
  TopTools_ListOfShape lE;
  TopExp_Explorer exp(aW,TopAbs_EDGE);
  for(; exp.More(); exp.Next()) {
    const TopoDS_Shape& s = exp.Current();
    lE.Append(s);
    nbE++;
  }

  if( nbE != 2 ) return Standard_False;
  TopoDS_Edge e1 = TopoDS::Edge(lE.First());
  TopoDS_Edge e2 = TopoDS::Edge(lE.Last());

  TopoDS_Vertex v1, v2, v3, v4;
  TopExp::Vertices(e1,v1,v2);
  TopExp::Vertices(e2,v3,v4);

  if( v1.IsNull() || v2.IsNull() || 
      v3.IsNull() || v4.IsNull() ) return Standard_False;

  if( v1.IsSame(v2) || v3.IsSame(v4) )
    return Standard_False;

  Standard_Boolean sF = Standard_False, sL = Standard_False;
  if( v1.IsSame(v3) || v1.IsSame(v4) ) sF = Standard_True;
  if( v2.IsSame(v3) || v2.IsSame(v4) ) sL = Standard_True;

  if( !sF || !sL ) return Standard_False;

  TopAbs_Orientation e1or = e1.Orientation();
  TopAbs_Orientation e2or = e2.Orientation();
 
  Standard_Real f1 = 0., l1 = 0., f2 = 0., l2 = 0.;
  Handle(Geom2d_Curve) pc1 = BRep_Tool::CurveOnSurface(e1,aF,f1,l1);
  Handle(Geom2d_Curve) pc2 = BRep_Tool::CurveOnSurface(e2,aF,f2,l2);
  
  if( pc1.IsNull() || pc2.IsNull() ) return Standard_False;

  Standard_Real d1 = Abs(l1-f1)/100.;
  Standard_Real d2 = Abs(l2-f2)/100.;
  Standard_Real m1 = (l1+f1)*0.5;
  Standard_Real m2 = (l2+f2)*0.5;

  gp_Pnt2d p1f(pc1->Value(m1-d1));
  gp_Pnt2d p1l(pc1->Value(m1+d1));
  gp_Pnt2d p2f(pc2->Value(m2-d2));
  gp_Pnt2d p2l(pc2->Value(m2+d2));

  gp_Vec2d vc1(p1f,p1l);
  gp_Vec2d vc2(p2f,p2l);
  
  if( (vc1*vc2) >= 0. && e1or == e2or ) return Standard_False;

  return Standard_True;
}
