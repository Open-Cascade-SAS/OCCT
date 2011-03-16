// File:	IntPolyh_Triangle.cxx
// Created:	Mon Mar  8 09:32:00 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


#include <IntPolyh_Triangle.ixx>
#include <IntPolyh_Point.ixx>
#include <IntPolyh_Edge.ixx>
#include <IntPolyh_StartPoint.ixx>
#include <IntPolyh_Couple.ixx>
#include <stdio.h>

#define MyTolerance 10.0e-7
#define MyConfusionPrecision 10.0e-12
#define SquareMyConfusionPrecision 10.0e-24

//# ifdef DEB
  //#define MYDEBUG DEB
//# else
  //#define MYDEBUG 0
//# endif

IntPolyh_Triangle::IntPolyh_Triangle() : p1(-1),p2(-1),p3(-1),
  e1(-1),oe1(0),e2(-1),oe2(0),e3(-1),oe3(0),
  II(0),IP(1),Fleche(0.0) { } 

IntPolyh_Triangle::IntPolyh_Triangle(const Standard_Integer a,const Standard_Integer b,
				     const Standard_Integer c) : p1(a),p2(b),p3(c),
				     e1(-1),oe1(0),e2(-1),oe2(0),e3(-1),oe3(0),II(0),IP(1),Fleche(0.0) { }

Standard_Integer IntPolyh_Triangle::FirstPoint() const { return(p1); } 
Standard_Integer IntPolyh_Triangle::SecondPoint() const { return(p2); }
Standard_Integer IntPolyh_Triangle::ThirdPoint() const { return(p3); }
Standard_Integer IntPolyh_Triangle::FirstEdge() const { return(e1); }
Standard_Integer IntPolyh_Triangle::FirstEdgeOrientation() const { return(oe1); }
Standard_Integer IntPolyh_Triangle::SecondEdge() const { return(e2); }
Standard_Integer IntPolyh_Triangle::SecondEdgeOrientation() const { return(oe2); }
Standard_Integer IntPolyh_Triangle::ThirdEdge() const { return(e3); }
Standard_Integer IntPolyh_Triangle::ThirdEdgeOrientation() const { return(oe3); }
Standard_Real IntPolyh_Triangle::GetFleche() const { return(Fleche); }
Standard_Integer IntPolyh_Triangle::IndiceIntersectionPossible() const { return(IP); }
Standard_Integer IntPolyh_Triangle::IndiceIntersection() const { return(II); }

void IntPolyh_Triangle::SetFirstPoint(const Standard_Integer a) { p1=a; } 
void IntPolyh_Triangle::SetSecondPoint(const Standard_Integer b) { p2=b; }    
void IntPolyh_Triangle::SetThirdPoint(const Standard_Integer c) { p3=c; }
void IntPolyh_Triangle::SetFirstEdge(const Standard_Integer e, const Standard_Integer oe) { e1=e; oe1=oe;}
void IntPolyh_Triangle::SetSecondEdge(const Standard_Integer f, const Standard_Integer of) { e2=f; oe2=of; }
void IntPolyh_Triangle::SetThirdEdge(const Standard_Integer g, const Standard_Integer og) { e3=g; oe3=og; }
void IntPolyh_Triangle::SetFleche(const Standard_Real A) { Fleche=A;}
void IntPolyh_Triangle::SetIndiceIntersectionPossible(const Standard_Integer I) { IP=I; }
void IntPolyh_Triangle::SetIndiceIntersection(const Standard_Integer I) { II=I; }


Standard_Integer IntPolyh_Triangle::GetEdgeNumber(const Standard_Integer EdgeIndex) const { 
  if(EdgeIndex==1)
    return(e1);
  if(EdgeIndex==2)
    return(e2);
  if(EdgeIndex==3)
    return(e3);

  return 0;

}  

void IntPolyh_Triangle::SetEdge(const Standard_Integer EdgeIndex,
				const Standard_Integer EdgeNumber) {
  if(EdgeIndex==1)
    e1=EdgeNumber;
  if(EdgeIndex==2)
    e2=EdgeNumber;
  if(EdgeIndex==3)
    e3=EdgeNumber;
}  

Standard_Integer IntPolyh_Triangle::GetEdgeOrientation(const Standard_Integer EdgeIndex) const {
  if(EdgeIndex==1)
    return(oe1);
  if(EdgeIndex==2)
    return(oe2);
  if(EdgeIndex==3)
    return(oe3);

  return 0;

}  

void IntPolyh_Triangle::SetEdgeOrientation(const Standard_Integer EdgeIndex,
					   const Standard_Integer OrEd) {
  if(EdgeIndex==1)
    oe1=OrEd;
  if(EdgeIndex==2)
    oe2=OrEd;
  if(EdgeIndex==3)
    oe3=OrEd;
}  

      /*Calcul de la fleche pour un triangle**************
	Distance entre le plan forme par le triangle et
	le barycentre situe sur la surface calcule avec les coordonnees Gu,Gv
	(coordonnees du barycentre du triangle dans l'espace UV)*/

void IntPolyh_Triangle::TriangleDeflection(const Handle(Adaptor3d_HSurface)& MySurface,
					   const IntPolyh_ArrayOfPoints& TPoints){

  const IntPolyh_Point & P1 = TPoints[p1];
  const IntPolyh_Point & P2 = TPoints[p2];
  const IntPolyh_Point & P3 = TPoints[p3];
 
  Standard_Real Gu=(P1.U()+P2.U()+P3.U())/3.0;
  Standard_Real Gv=(P1.V()+P2.V()+P3.V())/3.0;

  gp_Pnt PtXYZ = (MySurface)->Value( Gu, Gv);
  IntPolyh_Point BarycentreReel(PtXYZ.X(), PtXYZ.Y(), PtXYZ.Z(), Gu, Gv);
  IntPolyh_Point NormaleTri;
  NormaleTri.Cross(P2-P1,P3-P1);
  Standard_Real SqNorme=NormaleTri.SquareModulus();
  
  if (SqNorme > SquareMyConfusionPrecision) {
    NormaleTri=NormaleTri/sqrt(SqNorme);
    Fleche=Abs(NormaleTri.Dot( BarycentreReel-P1));
  }
  else {
    // On calcule la fleche sur le plus grand des edges
    // calcul des longueurs des cotes au carre
	
      Standard_Real L12 = P1.SquareDistance(P2);
      Standard_Real L23 = P2.SquareDistance(P3);
      Standard_Real L31 = P3.SquareDistance(P1);

      if (L12<SquareMyConfusionPrecision) {
# if MYDEBUG
	printf("\nTriangleDeflection() from IntPolyh_Triangle : L12=0\n");
	P1.Dump();
	P2.Dump();
# endif
      }
      if (L23<SquareMyConfusionPrecision) {
# if MYDEBUG
	printf("\nTriangleDeflection() from IntPolyh_Triangle : L23=0\n");
	P2.Dump();
	P3.Dump();
# endif
      }
      if (L31<SquareMyConfusionPrecision) {
# if MYDEBUG
	printf("\nTriangleDeflection() from IntPolyh_Triangle : L31=0\n");
	P3.Dump();
	P1.Dump();
# endif
      }

      IntPolyh_Point Milieu; // milieu du plus grand des edges

      if ((L12>L23) && (L12>L31))
	Milieu.Middle( MySurface,P1, P2);
      else if ((L23>L31) && (L23>L12))
	Milieu.Middle( MySurface,P2, P3);
      else if ((L31>L12) && (L31>L23))
	Milieu.Middle( MySurface,P3, P1);
	
	
      gp_Pnt PtXYZ = (MySurface)->Value( Milieu.U(), Milieu.V());
      IntPolyh_Point MilieuReel(PtXYZ.X(), PtXYZ.Y(), PtXYZ.Z(), Milieu.U(), Milieu.V());
      Fleche = sqrt(Milieu.SquareDistance(MilieuReel));
    }
}

Standard_Integer IntPolyh_Triangle::CheckCommonEdge(const Standard_Integer PT1,
						    const Standard_Integer PT2,
						    const Standard_Integer PT3,
						    const Standard_Integer Index,
						    const IntPolyh_ArrayOfTriangles &TTriangles) const {
  Standard_Integer P1,P2,P3,res=-1;
  P1=TTriangles[Index].FirstPoint();
  P2=TTriangles[Index].SecondPoint();
  P3=TTriangles[Index].ThirdPoint();
  
  if ( (P1==PT1)||(P1==PT2) ) {
    if ( ( (P2==PT1)||(P2==PT2) )&&(P3!=PT3) ) res = Index; //edge commun P1P2
    else if ( ( (P3==PT1)||(P3==PT2) )&&(P2!=PT3) ) res = Index;//edge commun P1P3
  }
  else if ( (P2==PT1)||(P2==PT2) ) {
    if ( ( (P3==PT1)||(P3==PT2) )&&(P1!=PT3) ) res = Index; //edge commun P2P3
  }
  else res=-1;
  return(res);
}

Standard_Integer IntPolyh_Triangle::GetNextTriangle2(const Standard_Integer NumTri,
						     const Standard_Integer NumEdge,
						     const IntPolyh_ArrayOfEdges &TEdges) const {
  Standard_Integer NumNextTri=-1;
  if (NumEdge==1) {
    const IntPolyh_Edge & Edge1=TEdges[e1];
    if(Edge1.FirstTriangle()==NumTri)
      NumNextTri=Edge1.SecondTriangle();
    else
      NumNextTri=Edge1.FirstTriangle();
  }
  else if (NumEdge==2) {
    const IntPolyh_Edge & Edge2=TEdges[e2];
    if(Edge2.FirstTriangle()==NumTri)
      NumNextTri=Edge2.SecondTriangle();
    else
      NumNextTri=Edge2.FirstTriangle();
  }
  else if (NumEdge==3) {
    const IntPolyh_Edge & Edge3=TEdges[e3];
    if(Edge3.FirstTriangle()==NumTri)
      NumNextTri=Edge3.SecondTriangle();
    else
      NumNextTri=Edge3.FirstTriangle();
  }
  else {
# if MYDEBUG
    printf("\nTriangleDeflection() from IntPolyh_Triangle :\n");
    printf("Edge index != {1,2,3}, Edge Number=%d\n",NumEdge);
# endif
  }
  return (NumNextTri);
}


void IntPolyh_Triangle::LinkEdges2Triangle(const IntPolyh_ArrayOfEdges & TEdges,
					   const Standard_Integer edge1,
					   const Standard_Integer edge2,
					   const Standard_Integer edge3) {
  if( (edge1<0)||(edge2<0)||(edge3<0) ) {
# if MYDEBUG
    printf("LinkEdges2Triangle() from IntPolyh_Triangle.cxx :/n");
    printf("Some edges are unknown LinkEdges2Triangle impossible\n");
    printf("edge1=%d edge2=%d edge3=%d\n",edge1,edge2,edge3);
# endif
  }
  else {
    e1=edge1;  
    e2=edge2;
    e3=edge3;
    
    if(TEdges[e1].FirstPoint()==p1) oe1=1;
    else oe1=-1;
    if(TEdges[e2].FirstPoint()==p2) oe2=1;
    else oe2=-1;
    if(TEdges[e3].FirstPoint()==p3) oe3=1;
    else oe3=-1;
  }
}

void GetInfoTA(const Standard_Integer numP1,
	       const Standard_Integer numP2,
	       const Standard_Integer numTA,
	       const IntPolyh_ArrayOfTriangles & TTriangles,
	       Standard_Integer & numP3b,
	       Standard_Integer & P3bIndex,
	       Standard_Integer & Edge2b,
	       Standard_Integer & Edge3b) {
  /// On veut savoir quel est le troisieme point du triangle
  /// adjacent (TriAdj) et quel sont les edges partant de ce point
  const IntPolyh_Triangle & TriAdj=TTriangles[numTA];
  Standard_Integer P1b=TriAdj.FirstPoint();
  Standard_Integer P2b=TriAdj.SecondPoint();
  Standard_Integer P3b=TriAdj.ThirdPoint();

  if ( (P1b!=numP1)&&(P1b!=numP2) ) { 
    numP3b=P1b; 
    P3bIndex=1;
    if (P2b==numP1) {
      ///P1bP2b==numP3bnumP1:Edge3b donc dans ce cas
     Edge3b=TriAdj.FirstEdge();
     /// Donc P1bP3b==numP3bnumP2:Edge2b
     Edge2b=TriAdj.ThirdEdge();
   }
    else {
     Edge2b=TriAdj.FirstEdge();
     Edge3b=TriAdj.ThirdEdge();
    }
  }
  else if( (P2b!=numP1)&&(P2b!=numP2) ) { 
    numP3b=P2b; 
    P3bIndex=2;
    if (P1b==numP1) {
      ///P2bP1b==numP3bnumP1:Edge3b donc dans ce cas
     Edge3b=TriAdj.FirstEdge();
     /// Donc P2bP3b==numP3bnumP2:Edge2b
     Edge2b=TriAdj.SecondEdge();
   }
    else {
     Edge2b=TriAdj.FirstEdge();
     Edge3b=TriAdj.SecondEdge();
    }
  }
  else if( (P3b!=numP1)&&(P3b!=numP2) ) { 
    numP3b=P3b; 
    P3bIndex=3; 
    if (P2b==numP1) {
      ///P3bP2b==numP3bnumP1:Edge3b donc dans ce cas
     Edge3b=TriAdj.SecondEdge();
     /// Donc P3bP1b==numP3bnumP2:Edge2b
     Edge2b=TriAdj.ThirdEdge();
   }
    else {
     Edge2b=TriAdj.SecondEdge();
     Edge3b=TriAdj.ThirdEdge();
    }
  }      
  if(numP3b<0) {
# if MYDEBUG
    printf("ERROR1 refinement with null Point\n");
# endif
  }
}

void NewTriangle(const Standard_Integer P1,
		 const Standard_Integer P2,
		 const Standard_Integer P3,
		 IntPolyh_ArrayOfTriangles &TTriangles,
		 const Handle(Adaptor3d_HSurface)& MySurface,
		 IntPolyh_ArrayOfPoints &TPoints) {
  const Standard_Integer FinTT = TTriangles.NbTriangles();
  TTriangles[FinTT].SetFirstPoint(P1);
  TTriangles[FinTT].SetSecondPoint(P2);
  TTriangles[FinTT].SetThirdPoint(P3);
  TTriangles[FinTT].TriangleDeflection(MySurface, TPoints);
  TTriangles.IncNbTriangles();
}

void NewEdge(const Standard_Integer P1,
	     const Standard_Integer P2,
	     const Standard_Integer T1,
	     const Standard_Integer T2,
	     IntPolyh_ArrayOfEdges & TEdges) {
//#ifndef DEB
  const Standard_Integer FinTE = TEdges.NbEdges();
//#else
//  const FinTE = TEdges.NbEdges();
//#endif
  TEdges[FinTE].SetFirstPoint(P1);
  TEdges[FinTE].SetSecondPoint(P2);
  TEdges[FinTE].SetFirstTriangle(T1);
  TEdges[FinTE].SetSecondTriangle(T2);
  TEdges.IncNbEdges();
}

void OldEdge(const Standard_Integer EdgeN,
	     const Standard_Integer NumTri,
	     const Standard_Integer NewTriNum,
	     IntPolyh_ArrayOfEdges & TEdges) {
  if(TEdges[EdgeN].FirstTriangle()==NumTri) TEdges[EdgeN].SetFirstTriangle(NewTriNum);
  else TEdges[EdgeN].SetSecondTriangle(NewTriNum);
}

void TestOldEdgeB(const Standard_Integer NumTA,
		  const Standard_Integer numPtT1,
		  const Standard_Integer numPtT2,
		  const Standard_Integer T1,
		  const Standard_Integer T2,
		  const IntPolyh_ArrayOfTriangles & TTriangles,
		  const Standard_Integer Edge1,
		  const Standard_Integer Edge3,
		  IntPolyh_ArrayOfEdges & TEdges ) {

  if( (TEdges[Edge1].FirstPoint() == numPtT1)
     ||(TEdges[Edge1].SecondPoint()== numPtT1) ) {
    /// L'edge1 est commun aux triangles NumTA et T1 
    if(TEdges[Edge1].FirstTriangle()==NumTA) 
      TEdges[Edge1].SetFirstTriangle(T1);
    else TEdges[Edge1].SetSecondTriangle(T1);
    
    if(TEdges[Edge3].FirstTriangle()==NumTA) 
      TEdges[Edge3].SetFirstTriangle(T2);
    else TEdges[Edge3].SetSecondTriangle(T2);
  }
  else {
    /// L'edge3 est commun aux triangles NumTA et T1 
    if(TEdges[Edge3].FirstTriangle()==NumTA) 
      TEdges[Edge3].SetFirstTriangle(T1);
    else TEdges[Edge3].SetSecondTriangle(T1);      
    if(TEdges[Edge1].FirstTriangle()==NumTA) 
      TEdges[Edge1].SetFirstTriangle(T2);
    else TEdges[Edge1].SetSecondTriangle(T2);
  }
}


void IntPolyh_Triangle::MiddleRefinement(const Standard_Integer NumTri,
					 const Handle(Adaptor3d_HSurface)& MySurface,
					 IntPolyh_ArrayOfPoints &TPoints,
					 IntPolyh_ArrayOfTriangles &TTriangles,
					 IntPolyh_ArrayOfEdges & TEdges) {

  Standard_Integer FinTE = TEdges.NbEdges();
  Standard_Integer FinTT = TTriangles.NbTriangles();
  
  ///Raffinage de la maille et de ses voisines par le milieu du plus grand des cotes

  Standard_Integer numP1 = FirstPoint();
  Standard_Integer numP2 = SecondPoint();
  Standard_Integer numP3 = ThirdPoint();

  IntPolyh_Point P1 = TPoints[numP1];
  IntPolyh_Point P2 = TPoints[numP2];
  IntPolyh_Point P3 = TPoints[numP3];


  ///calcul des longueurs des cotes au carre
	
  Standard_Real L12 = P1.SquareDistance(P2);
  Standard_Real L23 = P2.SquareDistance(P3);
  Standard_Real L31 = P3.SquareDistance(P1);

  if ((L12>L23) && (L12>L31)) {
    const Standard_Integer FinTP = TPoints.NbPoints();
    (TPoints[FinTP]).Middle( MySurface,P1, P2);
    
    ///les nouveaux triangles
    Standard_Integer T1,T2,T3,T4;

    T1=FinTT;
    NewTriangle(numP2,numP3,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    T2=FinTT;;
    NewTriangle(numP3,numP1,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;

    ///***AFFINAGE DU TRIANGLE ADJACENT***

    Standard_Integer numTA = GetNextTriangle2(NumTri,1,TEdges);

    if (numTA>=0) {
      Standard_Integer numP3b = -1;
      Standard_Integer P3bIndex = -1;

      Standard_Integer Edge2b = -1;
      Standard_Integer Edge3b = -1;
      
      GetInfoTA(numP1,numP2,numTA,TTriangles,numP3b,P3bIndex,Edge2b,Edge3b);
      
      T3=FinTT;
      NewTriangle(numP2,numP3b,FinTP,TTriangles,MySurface,TPoints);
      FinTT++;
      T4=FinTT;
      NewTriangle(numP3b,numP1,FinTP,TTriangles,MySurface,TPoints);

      ///On cree les nouveaux edges
      Standard_Integer E1,E2,E3,E4;
      
      E1=FinTE;
      NewEdge(numP1,FinTP,T2,T4,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP2,T1,T3,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP3,T1,T2,TEdges);
      FinTE++;
      E4=FinTE;
      NewEdge(FinTP,numP3b,T3,T4,TEdges);

      ///On met a jour les anciens edges
      OldEdge(e2,NumTri,T1,TEdges);
      OldEdge(e3,NumTri,T2,TEdges);
      OldEdge(Edge2b,numTA,T3,TEdges);
      OldEdge(Edge3b,numTA,T4,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,e2,E3,E2);
      TTriangles[T2].LinkEdges2Triangle(TEdges,e3,E1,E3);
      TTriangles[T3].LinkEdges2Triangle(TEdges,Edge2b,E4,E2);
      TTriangles[T4].LinkEdges2Triangle(TEdges,Edge3b,E1,E4);

      ///On tue le triangle adjacent
      TTriangles[numTA].Fleche=-1.0;
      TTriangles[numTA].IP=0;

    }
    else { ///seulement deux nouveaux triangles
      //on cree les nouveaux edges avec T1 et T2
      Standard_Integer E1,E2,E3;
      
      E1=FinTE;
      NewEdge(numP1,FinTP,T2,-1,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP2,T1,-1,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP3,T1,T2,TEdges);

      ///On met a jour les anciens edges
      OldEdge(e2,NumTri,T1,TEdges);
      OldEdge(e3,NumTri,T2,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,e2,E3,E2);
      TTriangles[T2].LinkEdges2Triangle(TEdges,e3,E1,E3);
    }
  }
  
  else if ((L23>L31) && (L23>L12)){
    const Standard_Integer FinTP = TPoints.NbPoints();
    (TPoints[FinTP]).Middle(MySurface, P2,P3);

    ///les nouveaux triangles
    Standard_Integer T1,T2,T3,T4;    
    
    T1=FinTT;
    NewTriangle(numP1,numP2,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    T2=FinTT;
    NewTriangle(numP3,numP1,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    
    ///*RAFFINAGE DU TRIANGLE ADJACENT***

    Standard_Integer numTA = GetNextTriangle2(NumTri,2,TEdges);

    if (numTA>=0) {
      Standard_Integer numP1b=-1;
      Standard_Integer P1bIndex = -1;

      Standard_Integer Edge1b = -1;
      Standard_Integer Edge3b = -1;

      GetInfoTA(numP2,numP3,numTA,TTriangles,numP1b,P1bIndex,Edge3b,Edge1b);     

      T3=FinTT;
      NewTriangle(numP2,numP1b,FinTP,TTriangles,MySurface,TPoints);
      FinTT++;
      T4=FinTT;
      NewTriangle(numP1b,numP3,FinTP,TTriangles,MySurface,TPoints);

      ///Nouveaux Edges
      Standard_Integer E1,E2,E3,E4;

      E1=FinTE;
      NewEdge(numP2,FinTP,T1,T3,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP3,T2,T4,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP1,T1,T2,TEdges);
      FinTE++;
      E4=FinTE;
      NewEdge(FinTP,numP1b,T3,T4,TEdges);

      ///On met a jour les anciens edges
      OldEdge(e1,NumTri,T1,TEdges);
      OldEdge(e3,NumTri,T2,TEdges);
      OldEdge(Edge1b,numTA,T3,TEdges);
      OldEdge(Edge3b,numTA,T4,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,e1,E1,E3);
      TTriangles[T2].LinkEdges2Triangle(TEdges,e3,E3,E2);
      TTriangles[T3].LinkEdges2Triangle(TEdges,Edge1b,E4,E1);
      TTriangles[T4].LinkEdges2Triangle(TEdges,Edge3b,E2,E4);

      ///On tue le triangle adjacent
      TTriangles[numTA].Fleche=-1.0;
      TTriangles[numTA].IP=0;
    }
    else { ///seulement deux nouveaux triangles
      ///Nouveaux Edges
      Standard_Integer E1,E2,E3;

      E1=FinTE;
      NewEdge(numP2,FinTP,T1,-1,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP3,T2,-1,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP1,T1,T2,TEdges);

      ///On met a jour les anciens edges
      OldEdge(e1,NumTri,T1,TEdges);
      OldEdge(e3,NumTri,T2,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,e1,E1,E3);
      TTriangles[T2].LinkEdges2Triangle(TEdges,e3,E3,E2);
    }
  }
    else {
    const Standard_Integer FinTP = TPoints.NbPoints();
    (TPoints[FinTP]).Middle(MySurface, P3,P1);

    Standard_Integer T1,T2,T3,T4;
    
    T1=FinTT;
    NewTriangle(numP1,numP2,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    T2=FinTT;
    NewTriangle(numP2,numP3,FinTP,TTriangles,MySurface,TPoints);
    FinTT++;
    
    ///*RAFFINAGE DU TRIANGLE ADJACENT***

    Standard_Integer numTA = GetNextTriangle2(NumTri,3,TEdges);

    if (numTA>=0) {

      Standard_Integer numP2b = -1;
      Standard_Integer P2bIndex = -1;
      
      Standard_Integer Edge1b = -1;
      Standard_Integer Edge2b = -1;
     
      GetInfoTA(numP3,numP1,numTA,TTriangles,numP2b,P2bIndex,Edge1b,Edge2b);

      T3=FinTT;
      NewTriangle(numP1,numP2b,FinTP,TTriangles,MySurface,TPoints);
      FinTT++;
      T4=FinTT;
      NewTriangle(numP2b,numP3,FinTP,TTriangles,MySurface,TPoints);

      ///Nouveaux Edges
      Standard_Integer E1,E2,E3,E4;

      E1=FinTE;
      NewEdge(numP2,FinTP,T1,T2,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP3,T2,T4,TEdges);
      FinTE++;
      E3=FinTE;
      NewEdge(FinTP,numP2b,T4,T3,TEdges);
      FinTE++;
      E4=FinTE;
      NewEdge(FinTP,numP1,T1,T3,TEdges);

      ///On met a jour les anciens edges
      OldEdge(e1,NumTri,T1,TEdges);
      OldEdge(e2,NumTri,T2,TEdges);
      OldEdge(Edge1b,numTA,T3,TEdges);
      OldEdge(Edge2b,numTA,T4,TEdges);
      
      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,e1,E1,E4);
      TTriangles[T2].LinkEdges2Triangle(TEdges,e2,E2,E1);
      TTriangles[T3].LinkEdges2Triangle(TEdges,Edge1b,E3,E4);
      TTriangles[T4].LinkEdges2Triangle(TEdges,Edge2b,E2,E3);

      ///On tue le triangle adjacent
      TTriangles[numTA].Fleche=-1.0;
      TTriangles[numTA].IP=0;
    }
    else { ///seulement deux nouveaux triangles
      ///Nouveaux Edges
      Standard_Integer E1,E2,E4;

      E1=FinTE;
      NewEdge(numP2,FinTP,T1,T2,TEdges);
      FinTE++;
      E2=FinTE;
      NewEdge(FinTP,numP3,T2,-1,TEdges);
      FinTE++;
      E4=FinTE;
      NewEdge(FinTP,numP1,T1,-1,TEdges);

      ///On met a jour les anciens edges
      OldEdge(e1,NumTri,T1,TEdges);
      OldEdge(e2,NumTri,T2,TEdges);

      /// On remplit les nouveaux triangles avec les edges
      TTriangles[T1].LinkEdges2Triangle(TEdges,e1,E1,E4);
      TTriangles[T2].LinkEdges2Triangle(TEdges,e2,E2,E1);
    }
  }
  /// Le triangle traite est maintenant obsolete 
  ///***On tue le triangle***
  Fleche=-1.0;
  IP=0;

  TPoints.IncNbPoints();
}

void IntPolyh_Triangle::MultipleMiddleRefinement(const Standard_Integer NbAffinages,
						 const Standard_Integer NumTri,
						 const Handle(Adaptor3d_HSurface)& MySurface,
						 IntPolyh_ArrayOfPoints &TPoints,
						 IntPolyh_ArrayOfTriangles &TTriangles,
						 IntPolyh_ArrayOfEdges & TEdges) {

  const Standard_Integer FinTTInit = TTriangles.NbTriangles();

  //On sait qu'il faut affiner au moins une fois
  TTriangles[NumTri].MiddleRefinement(NumTri,MySurface,TPoints,
				      TTriangles,TEdges);

  if (NbAffinages>1) {
    Standard_Integer MyNbAffinages=0;
    if (NbAffinages > 5)
      MyNbAffinages = 4;//5 est le maximum et on a deja affine une fois
    //On a decide d'arreter a 5 car avec un triangle on peut en obtenir 1024
    else MyNbAffinages = NbAffinages-1;//dans tous les cas MyNbAffinages>0
    

    //Un affinage peut donner deux ou quatre nouveaux triangles
    // ils seront ajoute a la fin du tableau de triangles, et auront comme indice
    // FinTTInit, FinTTInit+1,... 


    Standard_Integer NombreReelsAffinages = 4;
    for(Standard_Integer iii=1; iii<MyNbAffinages; iii++) 
      NombreReelsAffinages*=4;
    //Avec ce calcul on fait l'hypothese que chaque triangle affine donne quatre nouveaux triangles
    //ce qui peut etre faux si on n'affine pas le triangle adjacent
    //dans quel cas on n'obtient que deux nouveaux triangles
    
    Standard_Integer FinTTAffinage = FinTTInit + NombreReelsAffinages;
    
    for(Standard_Integer NumTriangle=FinTTInit; NumTriangle < FinTTAffinage; NumTriangle++)
      TTriangles[NumTriangle].MiddleRefinement(NumTriangle,MySurface,TPoints,
					       TTriangles,TEdges);
  }
}

Standard_Integer IntPolyh_Triangle::CompareBoxTriangle(const Bnd_Box &b,
						       const IntPolyh_ArrayOfPoints &TPoints) const{
  Standard_Integer Test=0;
  Bnd_Box maboite;
  const IntPolyh_Point&    PA=TPoints[p1]; 
  const IntPolyh_Point&    PB=TPoints[p2]; 
  const IntPolyh_Point&    PC=TPoints[p3]; 
  gp_Pnt pntA(PA.X(),PA.Y(),PA.Z());
  gp_Pnt pntB(PB.X(),PB.Y(),PB.Z());
  gp_Pnt pntC(PC.X(),PC.Y(),PC.Z());
  maboite.Add(pntA);
  maboite.Add(pntB);
  maboite.Add(pntC);
  maboite.Enlarge(Fleche+MyTolerance);
  if (maboite.IsOut(b))
    Test=0;
  else  
    Test=1;
  return(Test);
  //Pour gagner du temps on pourrait envisager de garder la boite englobante dans la structure du triangle
}

void IntPolyh_Triangle::MultipleMiddleRefinement2(const Standard_Real CritereAffinage,
						  const Bnd_Box &b,//boite englobante de l'autre surface
						  const Standard_Integer NumTri,
						  const Handle(Adaptor3d_HSurface)& MySurface,
						  IntPolyh_ArrayOfPoints &TPoints,
						  IntPolyh_ArrayOfTriangles &TTriangles,
						  IntPolyh_ArrayOfEdges & TEdges) {

  const Standard_Integer FinTTInit = TTriangles.NbTriangles();
  Standard_Integer CritereArret=FinTTInit+250;

  //On sait qu'il faut affiner une fois au moins
  MiddleRefinement(NumTri,MySurface,TPoints,
		   TTriangles,TEdges);

  Standard_Integer FinTT = TTriangles.NbTriangles();// FinTT n'est pas une constante, elle augmente avec l'affinage

  for(Standard_Integer iii=FinTTInit; iii<(FinTT=TTriangles.NbTriangles()); iii++) {
    IntPolyh_Triangle& TriangleCourant = TTriangles[iii];
    if(TriangleCourant.CompareBoxTriangle(b,TPoints)==0)
      //On n'affine pas le triangle
      TriangleCourant.IP=0;
    else if (TriangleCourant.Fleche > CritereAffinage)
      TriangleCourant.MiddleRefinement(iii,MySurface,TPoints,
				       TTriangles,TEdges);
      
    if ( FinTT > CritereArret )//critere d'arret 250 nouveaux triangles
      iii = FinTT;
  }
}

void IntPolyh_Triangle::SetEdgeandOrientation(const Standard_Integer EdgeIndex,
					      const IntPolyh_ArrayOfEdges &TEdges) {
  const Standard_Integer FinTE = TEdges.NbEdges();
#ifndef DEB
  Standard_Integer PE1 =0,PE2 =0;
#else
  Standard_Integer PE1,PE2;
#endif
  Standard_Integer Test=1;

  if (EdgeIndex==1) { PE1=p1; PE2=p2; }
  else if (EdgeIndex==2) { PE1=p2; PE2=p3; }
  else if (EdgeIndex==3) { PE1=p3; PE2=p1; }
  else { 
# if MYDEBUG
    printf("SetEdgeandOrientation() from IntPolyh_Triangle.cxx : No edge, No Edge\n");
# endif
    Test=0;
  }
  if (Test!=0) {
    for(Standard_Integer iioo=0; iioo<FinTE; iioo++) {
      Standard_Integer EFP=TEdges[iioo].FirstPoint();
      if (EFP==PE1) {
	Standard_Integer ESP=TEdges[iioo].SecondPoint();
	if (ESP!=EFP) {
	  if (ESP==PE2) {
	    SetEdgeOrientation(EdgeIndex,1);
	    SetEdge(EdgeIndex,iioo);
	    iioo=FinTE;
	  }
	}
	else {
# if MYDEBUG
	  printf("SetEdgeandOrientation() from IntPolyh_Triangle.cxx : WARNING NULL EDGE\n");
# endif
	  Test=0;
	}
      }
      else if (EFP==PE2) {
	Standard_Integer ESP=TEdges[iioo].SecondPoint();
	if (ESP!=EFP) {
	  if (ESP==PE1) {
	    SetEdgeOrientation(EdgeIndex,-1);
	    SetEdge(EdgeIndex,iioo);
	    iioo=FinTE;
	  }
	}
	else {
# if MYDEBUG
	  printf("SetEdgeandOrientation() from IntPolyh_Triangle.cxx : WARNING NULL EDGE\n");
# endif
	}   
      }
    }
  }
}


void IntPolyh_Triangle::Dump (const Standard_Integer i) const { 
  printf("\nTriangle(%3d) : Points %5d %5d %5d Edges %5d %5d %5d fleche: %8f  intersection possible %8d  intersection: %5d\n"
	 ,i,p1,p2,p3,e1,e2,e3,Fleche,IP,II);
}


void IntPolyh_Triangle::DumpFleche (const Standard_Integer i) const { 
  printf("\nTriangle(%3d) fleche: %5f\n",i,Fleche);
}



/* Affinage par le barycentre
      calcul du barycentre
      remaillage

  void IntPolyh_MaillageAffinage::RefinementG(const Standard_Integer SurfID,
  IntPolyh_Triangle& MonTriangle){
  
  Handle(Adaptor3d_HSurface) MaSurface=(SurfID==1)? MaSurface1:MaSurface2;
  IntPolyh_ArrayOfPoints &TPoints=(SurfID==1)? TPoints1:TPoints2;
  IntPolyh_ArrayOfTriangles &TTriangles=(SurfID==1)? TTriangles1:TTriangles2;
  Standard_Integer NbSamplesU=(SurfID==1)? NbSamplesU1:NbSamplesU2;
  Standard_Integer NbSamplesV=(SurfID==1)? NbSamplesV1:NbSamplesV2;
  
  Standard_Integer FinTP = TPoints.NbPoints();
  Standard_Integer FinTT = TTriangles.NbTriangles();


  Standard_Integer P1 = MonTriangle.FirstPoint();
  Standard_Integer P2 = MonTriangle.SecondPoint();
  Standard_Integer P3 = MonTriangle.ThirdPoint();
  
  
  Standard_Real U,V,x,y,z;
  U = (TPoints[P1].U()+TPoints[P2].U()+TPoints[P3].U())/3.0;
  V = (TPoints[P1].V()+TPoints[P2].V()+TPoints[P3].V())/3.0;
  
  gp_Pnt PtXYZ = (MaSurface)->Value(U, V);
  IntPolyh_Point BarycentreTriangleReel(PtXYZ.X(), PtXYZ.Y(), PtXYZ.Z(), U, V);
  TPoints[FinTP]=BarycentreTriangleReel;
  
  TTriangles[FinTT].SetFirstPoint(P1);
  TTriangles[FinTT].SetSecondPoint(P2);
  TTriangles[FinTT].SetThirdPoint(FinTP);
  (TTriangles[FinTT]).TriangleDeflection(MaSurface, TPoints);
  FinTT++;
  
  TTriangles[FinTT].SetFirstPoint(P2);
  TTriangles[FinTT].SetSecondPoint(P3);
  TTriangles[FinTT].SetThirdPoint(FinTP);
  (TTriangles[FinTT]).TriangleDeflection(MaSurface, TPoints);
  FinTT++;
  
  TTriangles[FinTT].SetFirstPoint(P3);
  TTriangles[FinTT].SetSecondPoint(P1);
  TTriangles[FinTT].SetThirdPoint(FinTP);
  (TTriangles[FinTT]).TriangleDeflection(MaSurface, TPoints);
  FinTT++;
  
  //  FinTP++;
  TPoints.IncNbPoints();
  TTriangles.SetNbTriangles(FinTT);
  //   Le triangle traite est maintenant obsolete 
  MonTriangle.SetFleche(-1.0);
  }*/



