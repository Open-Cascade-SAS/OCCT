// File:	BRepFill_ShapeLaw.cxx
// Created:	Mon Aug 17 10:30:37 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <BRepFill_ShapeLaw.ixx>

#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepLProp.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomFill_UniformSection.hxx>
#include <GeomFill_EvolvedSection.hxx>
#include <GeomFill_HArray1OfSectionLaw.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>

#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>


#include <Precision.hxx>


//=======================================================================
//function : Create
//purpose  : On traite le cas du Vertex en construisant, une line,
//           ayant le vertex pour origine
//=======================================================================
BRepFill_ShapeLaw::BRepFill_ShapeLaw(const TopoDS_Vertex& V,
					 const Standard_Boolean Build) 
                             : vertex(Standard_True),
			       myShape(V)
{
  TheLaw.Nullify();
  uclosed = Standard_False;
  vclosed = Standard_True; // loi constante
  myEdges =  new (TopTools_HArray1OfShape) (1, 1);
  myEdges->SetValue(1, V);

  if (Build) {
    myLaws = new (GeomFill_HArray1OfSectionLaw) (1, 1); 
//    gp_Pnt Origine;
    gp_Dir D(1,0,0); //Suivant la normal
    Handle(Geom_Line) L = new (Geom_Line)(BRep_Tool::Pnt(V), D);
    Standard_Real Last =  2*BRep_Tool::Tolerance(V)+Precision::PConfusion();
    Handle(Geom_TrimmedCurve) TC = new (Geom_TrimmedCurve) (L, 0, Last);

    myLaws->ChangeValue(1) = 
      new (GeomFill_UniformSection)(TC);
  }
}

//=======================================================================
//function : Create
//purpose  : Wire
//=======================================================================

BRepFill_ShapeLaw::BRepFill_ShapeLaw(const TopoDS_Wire& W,
					 const Standard_Boolean Build) 
                                        : vertex(Standard_False),
					  myShape(W)
					  

{
  TheLaw.Nullify();
  Init(Build);
}

//=======================================================================
//function : Create
//purpose  : Wire evolutif
//=======================================================================

BRepFill_ShapeLaw::BRepFill_ShapeLaw(const TopoDS_Wire& W,
				     const Handle(Law_Function)& L,
				     const Standard_Boolean Build) 
                                        : vertex(Standard_False),
					  myShape(W)

{
  TheLaw = L;
  Init(Build);
}

//=======================================================================
//function : Init
//purpose  : Cas du wire : On cree une table de GeomFill_SectionLaw
//=======================================================================
void BRepFill_ShapeLaw::Init(const Standard_Boolean Build)
{
  vclosed = Standard_True;
  BRepTools_WireExplorer wexp;
  TopoDS_Edge E;
  Standard_Integer NbEdge,ii;
  Standard_Real First, Last;
  TopoDS_Wire W;
  W = TopoDS::Wire(myShape);

  for (NbEdge=0, wexp.Init(W); wexp.More(); wexp.Next()) {
    E = wexp.Current();
    if( !E.IsNull() && !BRep_Tool::Degenerated(E)) {
      Handle(Geom_Curve) C = BRep_Tool::Curve(E,First,Last);
      if( !C.IsNull() ) {
	NbEdge++;
      }
    }
  }
  
  myLaws  = new GeomFill_HArray1OfSectionLaw (1, NbEdge);
  myEdges = new TopTools_HArray1OfShape      (1, NbEdge);

  ii = 1;
  
  for(wexp.Init(W); wexp.More(); wexp.Next()) {
    E = wexp.Current();
    if ( !E.IsNull() && !BRep_Tool::Degenerated(wexp.Current())) {
      Handle(Geom_Curve) C = BRep_Tool::Curve(E,First,Last);
      if( !C.IsNull() ) {
	myEdges->SetValue(ii, E);
	if(Build) {
	  //Handle(Geom_Curve) C = BRep_Tool::Curve(E,First,Last);
	  if (E.Orientation() == TopAbs_REVERSED) {
	    Standard_Real aux;
	    Handle(Geom_Curve) CBis;
	    CBis = C->Reversed(); // Pour eviter de deteriorer la topologie
	    aux = C->ReversedParameter(First);
	    First = C->ReversedParameter(Last);
	    Last = aux;
	    C = CBis;
	  }

	  Standard_Boolean IsReallyClosed = E.Closed();
	  //IFV - some checking when closed flag is wrong
	  if(IsReallyClosed) {
	    TopoDS_Vertex V1, V2;
	    TopExp::Vertices(E, V1, V2);
	    if(V1.IsNull() || V2.IsNull()) {
	      IsReallyClosed = Standard_False;
	    }
	    else {
	      IsReallyClosed = V1.IsSame(V2);
	    }
	  }

	  if ((ii>1) || !IsReallyClosed ) { // On trimme C
	    Handle(Geom_TrimmedCurve) TC = new Geom_TrimmedCurve(C,First, Last);
	    C = TC;
	  }
	  // sinon On garde l'integrite de la courbe
	  if (TheLaw.IsNull()) {
	    myLaws->ChangeValue(ii) = new GeomFill_UniformSection(C);
	  }
	  else {
	    myLaws->ChangeValue(ii) = new GeomFill_EvolvedSection(C, TheLaw);
	  }
	}
	ii++;
      }
    }
  }

//  cout << "new law" << endl;

  // La loi est elle ferme en U ?
  uclosed = W.Closed();
  if (!uclosed) {
    // le flag n'etant pas tres sur, on fait une verif
    TopoDS_Edge Edge1, Edge2;
    TopoDS_Vertex V1,V2;
    Edge1 = TopoDS::Edge (myEdges->Value(myEdges->Length()));
    Edge2 = TopoDS::Edge (myEdges->Value(1));

    if( Edge1.Orientation() == TopAbs_REVERSED) {
      V1 = TopExp::FirstVertex(Edge1);
    }
    else {
      V1 = TopExp::LastVertex(Edge1);
    }

    if ( Edge2.Orientation() == TopAbs_REVERSED) {
      V2 = TopExp::LastVertex(Edge2);
    }
    else {
      V2 = TopExp::FirstVertex(Edge2);
    } 
    if (V1.IsSame(V2)) {
      uclosed = Standard_True;
    }
    else {
      BRepAdaptor_Curve Curve1(Edge1);
      BRepAdaptor_Curve Curve2(Edge2);
      Standard_Real U1 = BRep_Tool::Parameter(V1,Edge1);
      Standard_Real U2 = BRep_Tool::Parameter(V2,Edge2);
      Standard_Real Eps = BRep_Tool::Tolerance(V2) + 
	                  BRep_Tool::Tolerance(V1);
      
      uclosed = Curve1.Value(U1).IsEqual(Curve2.Value(U2), Eps);
    }
  } 
}


//=======================================================================
//function : IsVertex
//purpose  :
//=======================================================================
 Standard_Boolean BRepFill_ShapeLaw::IsVertex() const
{
  return vertex;
}

//=======================================================================
//function : IsConstant
//purpose  :
//=======================================================================
 Standard_Boolean BRepFill_ShapeLaw::IsConstant() const
{
  return TheLaw.IsNull();
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
 TopoDS_Vertex 
 BRepFill_ShapeLaw::Vertex(const Standard_Integer Index, 
			   const Standard_Real Param) const
{ 
  TopoDS_Edge E;
  TopoDS_Vertex V; 
  if (Index <= myEdges->Length()) {
    E =  TopoDS::Edge(myEdges->Value(Index));
    if (E.Orientation() == TopAbs_REVERSED) 
       V = TopExp::LastVertex(E);
    else V = TopExp::FirstVertex(E);
  }
  else if (Index == myEdges->Length()+1) {
    E =  TopoDS::Edge(myEdges->Value(Index-1));
    if (E.Orientation() == TopAbs_REVERSED) 
      V = TopExp::FirstVertex(E);
    else V = TopExp::LastVertex(E);
  }

  if (!TheLaw.IsNull()) {    
    gp_Trsf T;
    T.SetScale(gp_Pnt(0, 0, 0), TheLaw->Value(Param));
    TopLoc_Location L(T);
    V.Move(L);
  }
  return V;
}


///=======================================================================
//function : VertexTol
//purpose  : Evalue le trou entre 2 edges de la section
//=======================================================================
 Standard_Real BRepFill_ShapeLaw::VertexTol(const Standard_Integer Index,
					      const Standard_Real Param) const
{
  Standard_Real Tol = Precision::Confusion();
  Standard_Integer I1, I2;
  if ( (Index==0) || (Index==myEdges->Length()) ) {
    if (!uclosed) return Tol; //Le moins faux possible
    I1 = myEdges->Length();
    I2 = 1;
  }
  else {
    I1 = Index;
    I2 = I1 +1;
  } 

  Handle(GeomFill_SectionLaw) Loi;
  Standard_Integer NbPoles,  NbKnots, Degree;
  Handle(TColgp_HArray1OfPnt) Poles;
  Handle(TColStd_HArray1OfReal) Knots, Weigth;
  Handle(TColStd_HArray1OfInteger) Mults;
  Handle(Geom_BSplineCurve) BS;
  gp_Pnt PFirst;

  Loi = myLaws->Value(I1);
  Loi->SectionShape( NbPoles,  NbKnots, Degree);
  Poles = new (TColgp_HArray1OfPnt) (1, NbPoles);
  Weigth = new  (TColStd_HArray1OfReal) (1, NbPoles);
  Loi->D0(Param, Poles->ChangeArray1(), Weigth->ChangeArray1());
  Knots = new  (TColStd_HArray1OfReal) (1, NbKnots);
  Loi->Knots(Knots->ChangeArray1());
  Mults = new (TColStd_HArray1OfInteger) (1, NbKnots);
  Loi->Mults(Mults->ChangeArray1());
  BS = new (Geom_BSplineCurve) (Poles->Array1(), 
				Weigth->Array1(), 
				Knots->Array1(), 
				Mults->Array1(), 
				Degree,
				Loi->IsUPeriodic());
  PFirst = BS->Value( Knots->Value(Knots->Length()) );

  Loi = myLaws->Value(I2);
  Loi->SectionShape( NbPoles,  NbKnots, Degree);
  Poles = new (TColgp_HArray1OfPnt) (1, NbPoles);
  Weigth = new  (TColStd_HArray1OfReal) (1, NbPoles);
  Loi->D0(Param, Poles->ChangeArray1(), Weigth->ChangeArray1());
  Knots = new  (TColStd_HArray1OfReal) (1, NbKnots);
  Loi->Knots(Knots->ChangeArray1());
  Mults = new (TColStd_HArray1OfInteger) (1, NbKnots);
  Loi->Mults(Mults->ChangeArray1());
  BS = new (Geom_BSplineCurve) (Poles->Array1(), 
				Weigth->Array1(), 
				Knots->Array1(), 
				Mults->Array1(), 
				Degree,
				Loi->IsUPeriodic());
  Tol += PFirst.Distance(BS->Value( Knots->Value(1)));
  return Tol;
}

//=======================================================================
//function : ConcatenedLaw
//purpose  : 
//=======================================================================

 Handle(GeomFill_SectionLaw) BRepFill_ShapeLaw::ConcatenedLaw() const
{
  Handle(GeomFill_SectionLaw) Law;
  if (myLaws->Length() == 1) 
    return myLaws->Value(1);
  else {
    TopoDS_Wire W;
    TopoDS_Vertex V;
    W = TopoDS::Wire(myShape);
    if(!W.IsNull()) {
      //  Concatenation des aretes
      Standard_Integer ii;
      Standard_Real epsV, f, l;
      Standard_Boolean Bof;
      Handle(Geom_Curve) Composite;
      Handle(Geom_TrimmedCurve) TC;
      Composite = BRep_Tool::Curve(Edge(1), f, l);
      TC = new (Geom_TrimmedCurve)(Composite, f, l);
      GeomConvert_CompCurveToBSplineCurve Concat(TC); 
  
      for (ii=2, Bof=Standard_True; ii<=myEdges->Length() && Bof; ii++){
	Composite = BRep_Tool::Curve(Edge(ii),f, l);
	TC = new (Geom_TrimmedCurve)(Composite, f, l);
	Bof = TopExp::CommonVertex(Edge(ii-1), Edge(ii), V);
	if (Bof) {epsV = BRep_Tool::Tolerance(V);}
	else epsV = 10*Precision::PConfusion();
	Bof = Concat.Add(TC, epsV, Standard_True, 
			 Standard_False, 20);
	if (!Bof) Bof = Concat.Add(TC, 200*epsV,
				   Standard_True, Standard_False, 20);
#if DEB
	if (!Bof) 
	  cout << "BRepFill_ShapeLaw::ConcatenedLaw INCOMPLET !!!" 
	  << endl;
#endif
      }
      Composite = Concat.BSplineCurve();

      if (TheLaw.IsNull()) {
	Law = new (GeomFill_UniformSection)(Composite);
      }
      else {
	Law = new (GeomFill_EvolvedSection)(Composite, TheLaw);
      }
    }
  }
  return Law;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================
 GeomAbs_Shape BRepFill_ShapeLaw::Continuity(const Standard_Integer Index,
					       const Standard_Real TolAngular) const
{
 
 TopoDS_Edge Edge1, Edge2;
  if ( (Index==0) || (Index==myEdges->Length()) ) {
    if (!uclosed) return GeomAbs_C0; //Le moins faux possible

    Edge1 = TopoDS::Edge (myEdges->Value(myEdges->Length()));
    Edge2 = TopoDS::Edge (myEdges->Value(1));
  }
  else {
    Edge1 = TopoDS::Edge (myEdges->Value(Index));
    Edge2 = TopoDS::Edge (myEdges->Value(Index+1));
  } 
 
 TopoDS_Vertex V1,V2;
 if ( Edge1.Orientation() == TopAbs_REVERSED) {
   V1 = TopExp::FirstVertex(Edge1);
 }
 else {
   V1 = TopExp::LastVertex(Edge1);
 }
 if ( Edge2.Orientation() == TopAbs_REVERSED) {
   V2 = TopExp::LastVertex(Edge2);
 }
 else {
   V2 = TopExp::FirstVertex(Edge2);
 }

 Standard_Real U1 = BRep_Tool::Parameter(V1,Edge1);
 Standard_Real U2 = BRep_Tool::Parameter(V2,Edge2);
 BRepAdaptor_Curve Curve1(Edge1);
 BRepAdaptor_Curve Curve2(Edge2);
 Standard_Real Eps = BRep_Tool::Tolerance(V2) +
                     BRep_Tool::Tolerance(V1);
 GeomAbs_Shape cont;
 cont = BRepLProp::Continuity(Curve1,Curve2,U1,U2, Eps, TolAngular);

 return cont;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================
 void BRepFill_ShapeLaw::D0(const Standard_Real U, TopoDS_Shape& S)
{
  S = myShape;
  if (!TheLaw.IsNull()) {
    gp_Trsf T;
    T.SetScale(gp_Pnt(0, 0, 0), TheLaw->Value(U));
    TopLoc_Location L(T);
    S.Move(L);
  }
} 
