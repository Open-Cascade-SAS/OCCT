// Created on: 1995-08-01
// Created by: Stagiaire Alain JOURDAIN
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <IntPoly_ShapeSection.ixx>

#include <IntPoly_SequenceOfSequenceOfPnt.hxx>
#include <IntPoly_IndexedMapOfPnt.hxx>
#include <IntPoly_PntHasher.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_XYZ.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>


//=======================================================================
//function : IntPoly_ShapeSection
//purpose  : 
//=======================================================================

IntPoly_ShapeSection::IntPoly_ShapeSection()
{}


//=======================================================================
//function : IntPoly_ShapeSection
//purpose  : 
//=======================================================================

IntPoly_ShapeSection::IntPoly_ShapeSection(const TopoDS_Shape& S1,
					   const TopoDS_Shape& S2)
{
  myShape1 = S1;
  myShape2 = S2;
  Section();
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void IntPoly_ShapeSection::Section()
{
  Explore();
  Standard_Integer NbLinks = myMapBegPoints.Extent();
  Standard_Integer Result,i = 1;
  Standard_Real Big = Precision::Infinite();
  gp_Pnt BegPoint,EndPoint,OutPoint;

  while (i <= NbLinks) {
    if (!(Precision::IsInfinite((myMapBegPoints.FindKey(i)).X()))) {
      BegPoint = myMapBegPoints.FindKey(i);
      EndPoint = myMapEndPoints.FindKey(i);
      myCpt++;
      myMapBegPoints.Substitute(i,gp_Pnt(Big,myCpt,myCpt));
      myMapEndPoints.Substitute(i,gp_Pnt(Big,myCpt,myCpt));
      Result = Concat(BegPoint,EndPoint,OutPoint);
      if (Result == 2) 
	ForwConstruction(OutPoint);
      else
	if (Result == 1)
	  PrevConstruction(OutPoint);
	else {
	  ForwConstruction(EndPoint);
	  PrevConstruction(BegPoint);   
	}
    }
    i++;
  }
  
  i = 1;
  while (i < mySection.Length()) {
    ConcatSection(mySection.ChangeValue(i),mySection.Length(),i+1);
    i++;
  }
  myNbEdges = mySection.Length();
}



//=======================================================================
//function : Explore
//purpose  : 
//=======================================================================

inline Standard_Boolean SAMESIGN(Standard_Real a,
				 Standard_Real b,
				 Standard_Real c)
{
  return ((a > 0.0 && b > 0.0 && c > 0.0) ||
	  (a < 0.0 && b < 0.0 && c < 0.0) ||
	  (a == 0.0 && b == 0.0 && c == 0.0));
}

inline void MINMAX(Standard_Real a,Standard_Real b,Standard_Real c,
		   Standard_Real &m,Standard_Real &M) 
{ 
  m = M = a;
  if (b < c) {  
    if (b < m) m = b;
    if (c > M) M = c; 
  } 
  else {
    if (c < m) m = c; 
    if (b > M) M = b; 
  } 
}

void IntPoly_ShapeSection::Explore()
{
  TopExp_Explorer ex1,ex2;
  Standard_Integer i,i1,i2,i3,j,j1,j2,j3;
  Standard_Integer NbTrian1,NbFaces1,NbTriATotal;
  Standard_Integer NbTrian2,NbFaces2,NbTriBTotal;
  Standard_Real Big = Precision::Infinite();
  Standard_Real min,minfx,minfy,minfz,max,maxfx,maxfy,maxfz;
  TopLoc_Location Loc1,Loc2;
  Handle(Poly_Triangulation) Tr1,Tr2;
  myCpt = 0;

  //-----------------------------------------------------------------
  //--  Traitement pour recuperer le nombre de faces et le nombre
  //--  de triangles des shapes 1 et 2
  //-----------------------------------------------------------------
  NbFaces1 = NbTriATotal = 0;
  for (ex1.Init(myShape1,TopAbs_FACE);ex1.More();ex1.Next()) {
    Tr1 = (BRep_Tool::Triangulation(TopoDS::Face(ex1.Current()),Loc1));
    if (!Tr1.IsNull()) {
      NbTriATotal += Tr1->NbTriangles();
      NbFaces1++;
    }
  }
  NbFaces2 = NbTriBTotal = 0;
  for (ex2.Init(myShape2,TopAbs_FACE);ex2.More();ex2.Next()) {
    Tr2 = (BRep_Tool::Triangulation(TopoDS::Face(ex2.Current()),Loc2));
    if (!Tr2.IsNull()) {
      NbTriBTotal += Tr2->NbTriangles();
      NbFaces2++;
    }
  }

  if (NbTriATotal < NbTriBTotal) {
    TopoDS_Shape S = myShape2;
    myShape2 = myShape1;
    myShape1 = S;
    Standard_Integer Nb = NbTriBTotal;
    NbTriBTotal = NbTriATotal;
    NbTriATotal = Nb;
    Nb       = NbFaces2;
    NbFaces2 = NbFaces1;
    NbFaces1 = Nb;
  }

  //-----------------------------------------------------------------
  //--  Tableaux des sommets des NbTriATotal triangles du shape 1
  //-----------------------------------------------------------------
  TColgp_Array1OfPnt TA1(1,NbTriATotal);
  TColgp_Array1OfPnt TA2(1,NbTriATotal);
  TColgp_Array1OfPnt TA3(1,NbTriATotal);
  //-----------------------------------------------------------------
  //-- Tableaux des MinMax des NbTriATotal triangles du shape 1
  //-----------------------------------------------------------------
  TColStd_Array1OfReal MinAx(1,NbTriATotal);
  TColStd_Array1OfReal MinAy(1,NbTriATotal);
  TColStd_Array1OfReal MinAz(1,NbTriATotal);
  TColStd_Array1OfReal MaxAx(1,NbTriATotal);
  TColStd_Array1OfReal MaxAy(1,NbTriATotal);
  TColStd_Array1OfReal MaxAz(1,NbTriATotal);
  //-----------------------------------------------------------------
  //-- Tableaux des MinMax des NbFaces1 faces du shape 1
  //-----------------------------------------------------------------
  TColStd_Array1OfReal MinFAx(1,NbFaces1);
  TColStd_Array1OfReal MinFAy(1,NbFaces1);
  TColStd_Array1OfReal MinFAz(1,NbFaces1);
  TColStd_Array1OfReal MaxFAx(1,NbFaces1);
  TColStd_Array1OfReal MaxFAy(1,NbFaces1);
  TColStd_Array1OfReal MaxFAz(1,NbFaces1);
  //-----------------------------------------------------------------
  //-- Tableau contenant le nombre de triangles des faces du shape 1
  //-----------------------------------------------------------------
  TColStd_Array1OfInteger TNbTr1(1,NbFaces1);

  //-----------------------------------------------------------------
  //--  Preparation : Mise a jour des tableaux et calcul des MinMax
  //-----------------------------------------------------------------
  NbTriATotal = NbFaces1 = 0;
  for (ex1.Init(myShape1,TopAbs_FACE);ex1.More();ex1.Next()) {
    Tr1 = (BRep_Tool::Triangulation(TopoDS::Face(ex1.Current()),Loc1));
    if (!Tr1.IsNull()) {
      NbTrian1 = Tr1->NbTriangles();
      NbFaces1++;
      TNbTr1.SetValue(NbFaces1,NbTrian1);
      const Poly_Array1OfTriangle& TabTrian1 = Tr1->Triangles();
      const TColgp_Array1OfPnt&    TabNodes1 = Tr1->Nodes();
      minfx = minfy = minfz = -Big;
      maxfx = maxfy = maxfz =  Big;
      for (i = 1;i <= NbTrian1;i++) {
	TabTrian1(i).Get(i1,i2,i3);
	NbTriATotal++;
	TA1.SetValue(NbTriATotal,TabNodes1(i1).Transformed(Loc1.Transformation()));
	TA2.SetValue(NbTriATotal,TabNodes1(i2).Transformed(Loc1.Transformation()));
	TA3.SetValue(NbTriATotal,TabNodes1(i3).Transformed(Loc1.Transformation()));
	MINMAX(TA1.Value(NbTriATotal).X(),
	       TA2.Value(NbTriATotal).X(),
	       TA3.Value(NbTriATotal).X(),min,max);
	MinAx.SetValue(NbTriATotal,min);
	MaxAx.SetValue(NbTriATotal,max);
	if (min < minfx) minfx = min;
	if (max > maxfx) maxfx = max;
	MINMAX(TA1.Value(NbTriATotal).Y(),
	       TA2.Value(NbTriATotal).Y(),
	       TA3.Value(NbTriATotal).Y(),min,max);
	MinAy.SetValue(NbTriATotal,min);
	MaxAy.SetValue(NbTriATotal,max);
	if (min < minfy) minfy = min;
	if (max > maxfy) maxfy = max;
	MINMAX(TA1.Value(NbTriATotal).Z(),
	       TA2.Value(NbTriATotal).Z(),
	       TA3.Value(NbTriATotal).Z(),min,max);
	MinAz.SetValue(NbTriATotal,min);
	MaxAz.SetValue(NbTriATotal,max);
	if (min < minfz) minfz = min;
	if (max > maxfz) maxfz = max;
      }
      MinFAx.SetValue(NbFaces1,minfx);
      MaxFAx.SetValue(NbFaces1,maxfx);
      MinFAy.SetValue(NbFaces1,minfy);
      MaxFAy.SetValue(NbFaces1,maxfy);
      MinFAz.SetValue(NbFaces1,minfz);
      MaxFAz.SetValue(NbFaces1,maxfz);
    }
  }

  //-----------------------------------------------------------------
  //--  Tableaux des sommets des NbTriBTotal triangles du shape 2
  //-----------------------------------------------------------------
  TColgp_Array1OfPnt TB1(1,NbTriBTotal);
  TColgp_Array1OfPnt TB2(1,NbTriBTotal);
  TColgp_Array1OfPnt TB3(1,NbTriBTotal);
  //-----------------------------------------------------------------
  //-- Tableaux des MinMax des NbTriBTotal triangles du shape 2
  //-----------------------------------------------------------------
  TColStd_Array1OfReal MinBx(1,NbTriBTotal);
  TColStd_Array1OfReal MinBy(1,NbTriBTotal);
  TColStd_Array1OfReal MinBz(1,NbTriBTotal);
  TColStd_Array1OfReal MaxBx(1,NbTriBTotal);
  TColStd_Array1OfReal MaxBy(1,NbTriBTotal);
  TColStd_Array1OfReal MaxBz(1,NbTriBTotal);
  //-----------------------------------------------------------------
  //-- Tableaux des MinMax des NbFaces2 faces du shape 2
  //-----------------------------------------------------------------
  TColStd_Array1OfReal MinFBx(1,NbFaces2);
  TColStd_Array1OfReal MinFBy(1,NbFaces2);
  TColStd_Array1OfReal MinFBz(1,NbFaces2);
  TColStd_Array1OfReal MaxFBx(1,NbFaces2);
  TColStd_Array1OfReal MaxFBy(1,NbFaces2);
  TColStd_Array1OfReal MaxFBz(1,NbFaces2);
  //-----------------------------------------------------------------
  //-- Tableau contenant le nombre de triangles des faces du shape 2
  //-----------------------------------------------------------------
  TColStd_Array1OfInteger TNbTr2(1,NbFaces2);

  //-----------------------------------------------------------------
  //--  Preparation : Mise a jour des tableaux et calcul des MinMax
  //-----------------------------------------------------------------
  NbTriBTotal = NbFaces2 = 0;
  for (ex2.Init(myShape2,TopAbs_FACE);ex2.More();ex2.Next()) {
    Tr2 = (BRep_Tool::Triangulation(TopoDS::Face(ex2.Current()),Loc2));
    if (!Tr2.IsNull()) {
      NbTrian2 = Tr2->NbTriangles();
      NbFaces2++;
      TNbTr2.SetValue(NbFaces2,NbTrian2);
      const Poly_Array1OfTriangle& TabTrian2 = Tr2->Triangles();
      const TColgp_Array1OfPnt&    TabNodes2 = Tr2->Nodes();
      minfx = minfy = minfz = -Big;
      maxfx = maxfy = maxfz =  Big;
      for (j = 1;j <= NbTrian2;j++) {
	TabTrian2(j).Get(j1,j2,j3);
	NbTriBTotal++;
	TB1.SetValue(NbTriBTotal,TabNodes2(j1).Transformed(Loc2.Transformation()));
	TB2.SetValue(NbTriBTotal,TabNodes2(j2).Transformed(Loc2.Transformation()));
	TB3.SetValue(NbTriBTotal,TabNodes2(j3).Transformed(Loc2.Transformation()));
	MINMAX(TB1.Value(NbTriBTotal).X(),
	       TB2.Value(NbTriBTotal).X(),
	       TB3.Value(NbTriBTotal).X(),min,max);
	MinBx.SetValue(NbTriBTotal,min);
	MaxBx.SetValue(NbTriBTotal,max);
	if (min < minfx) minfx = min;
	if (max > maxfx) maxfx = max;
	MINMAX(TB1.Value(NbTriBTotal).Y(),
	       TB2.Value(NbTriBTotal).Y(),
	       TB3.Value(NbTriBTotal).Y(),min,max);
	MinBy.SetValue(NbTriBTotal,min);
	MaxBy.SetValue(NbTriBTotal,max);
	if (min < minfy) minfy = min;
	if (max > maxfy) maxfy = max;
	MINMAX(TB1.Value(NbTriBTotal).Z(),
	       TB2.Value(NbTriBTotal).Z(),
	       TB3.Value(NbTriBTotal).Z(),min,max);
	MinBz.SetValue(NbTriBTotal,min);
	MaxBz.SetValue(NbTriBTotal,max);
	if (min < minfz) minfz = min;
	if (max > maxfz) maxfz = max;
      }
      MinFBx.SetValue(NbFaces2,minfx);
      MaxFBx.SetValue(NbFaces2,maxfx);
      MinFBy.SetValue(NbFaces2,minfy);
      MaxFBy.SetValue(NbFaces2,maxfy);
      MinFBz.SetValue(NbFaces2,minfz);
      MaxFBz.SetValue(NbFaces2,maxfz);
    }
  }

  Standard_Integer ii,jj,fi,fj;
  i = j = 0;
  for (fi = 1;fi <= NbFaces1;fi++) {
    NbTrian1 = TNbTr1.Value(fi);
    i += NbTrian1;
    j = 0;
    for (fj = 1;fj <= NbFaces2;fj++) {
      NbTrian2 = TNbTr2.Value(fj);
      if (MinFAx.Value(fi) > MaxFBx.Value(fj) || 
	  MinFBx.Value(fj) > MaxFAx.Value(fi)) {
	j += NbTrian2;
	continue;
      }
      else {
	if (MinFAy.Value(fi) > MaxFBy.Value(fj) || 
	    MinFBy.Value(fj) > MaxFAy.Value(fi)) {
	  j += NbTrian2;
	  continue;
	}
	else if (MinFAz.Value(fi) > MaxFBz.Value(fj) || 
		 MinFBz.Value(fj) > MaxFAz.Value(fi)) {
	  j += NbTrian2;
	  continue;
	}
      }
      i -= NbTrian1;
      j += NbTrian2;

      for (ii = 1;ii <= NbTrian1;ii++) {
	i++;
	j -= NbTrian2;
	const gp_Pnt& A1 = TA1.Value(i);
	const gp_Pnt& A2 = TA2.Value(i);
	const gp_Pnt& A3 = TA3.Value(i);
	gp_Vec OA1(A1.X(), A1.Y(), A1.Z());
	gp_Vec VA0 = gp_Vec(A1,A2);
	gp_Vec VA  = gp_Vec(A1,A3);
	VA0.Cross(VA);
	VA0.Normalize();
	
	for (jj = 1;jj <= NbTrian2;jj++) {
	  j++;
	  if(MinAx.Value(i) > MaxBx.Value(j) || 
	     MinBx.Value(j) > MaxAx.Value(i)) {
	    continue;
	  }
	  else {
	    if (MinAy.Value(i) > MaxBy.Value(j) || 
		MinBy.Value(j) > MaxAy.Value(i)) {
	      continue;
	    }
	    else if (MinAz.Value(i) > MaxBz.Value(j) || 
		     MinBz.Value(j) > MaxAz.Value(i)) {
	      continue;
	    }
	  }
	  const gp_Pnt& B1 = TB1.Value(j);
	  const gp_Pnt& B2 = TB2.Value(j);
	  const gp_Pnt& B3 = TB3.Value(j);
	  gp_Vec VB0(B1,B2);
	  gp_Vec VB(B1,B3);
	  VB0.Cross(VB);
	  VB0.Normalize();
	  gp_Vec OB1(B1.XYZ());
	  gp_Vec V1(B1,A1);
	  gp_Vec V2(B1,A2);
	  gp_Vec V3(B1,A3);
	  Standard_Real h1 = VB0.Dot(V1);
	  Standard_Real h2 = VB0.Dot(V2);
	  Standard_Real h3 = VB0.Dot(V3);
	  Standard_Real ah1 = Abs(h1);
	  Standard_Real ah2 = Abs(h2);
	  Standard_Real ah3 = Abs(h3);
	  
	  if(!SAMESIGN(h1,h2,h3)) { 
	    myFirstTime = Standard_True;
	    if (h1 == 0.0) {
	      if (IsInside(A1,B1,B2,B3,VB0)) {
		myFirstTime = Standard_False;
		myBegPoint = A1;
		if (h2 == 0.0) {
		  if (IsInside(A2,B1,B2,B3,VB0)) {
		    if (!IsEqual(A2,myBegPoint)) {
		      myEndPoint = A2;
		      InsertInMap();
		      continue;
		    }
		  }
		}
		if (h3 == 0.0) {
		  if (IsInside(A3,B1,B2,B3,VB0)) {
		    if (!IsEqual(A3,myBegPoint)) {
		      myEndPoint = A3;
		      InsertInMap();
		      continue;
		    }
		  }
		}
	      }
	    }
	    if (h2 == 0.0) {   //  h1 <> 0
	      if (IsInside(A2,B1,B2,B3,VB0)) {
		myFirstTime = Standard_False;
		myBegPoint = A2;
		if (h3 == 0.0) {
		  if (IsInside(A3,B1,B2,B3,VB0)) {
		    if (!IsEqual(A3,myBegPoint)) {
		      myEndPoint = A3;
		      InsertInMap();
		      continue;
		    }
		  }
		}
	      }
	    }
	    if (h3 == 0.0) {  //  h1 <> 0  and  h2 <> 0
	      if (IsInside(A3,B1,B2,B3,VB0)) {
		myFirstTime = Standard_False;
		myBegPoint = A3;
	      }
	    }
	    if (Intersect(B1,B2,B3,OB1,VB0,V1,V2,h1,h2,ah1,ah2))
	      continue;
	    if (Intersect(B1,B2,B3,OB1,VB0,V2,V3,h2,h3,ah2,ah3)) 
	      continue;
	    if (Intersect(B1,B2,B3,OB1,VB0,V3,V1,h3,h1,ah3,ah1)) 
	      continue;
	    
	    V1 = gp_Vec(A1,B1);
	    V2 = gp_Vec(A1,B2);
	    V3 = gp_Vec(A1,B3);
	    h1 = VA0.Dot(V1);
	    h2 = VA0.Dot(V2);
	    h3 = VA0.Dot(V3);
	    ah1 = Abs(h1);
	    ah2 = Abs(h2);
	    ah3 = Abs(h3);
	    
	    if(!SAMESIGN(h1,h2,h3)) { 
	      if (h1 == 0.0) {
		if (IsInside(B1,A1,A2,A3,VA0)) {
		  if (myFirstTime) {
		    myFirstTime = Standard_False;
		    myBegPoint = B1;
		  }
		  else {
		    if (!IsEqual(B1,myBegPoint)) {
		      myEndPoint = B1;
		      InsertInMap();
		      continue;
		    }
		  }
		  if (h2 == 0.0) {
		    if (IsInside(B2,A1,A2,A3,VA0)) {
		      if (!IsEqual(B2,myBegPoint)) {
			myEndPoint = B2;
			InsertInMap();
			continue;
		      }
		    }
		  }
		  if (h3 == 0.0) {
		    if (IsInside(B3,A1,A2,A3,VA0)) {
		      if (!IsEqual(B3,myBegPoint)) {
			myEndPoint = B3;
			InsertInMap();
			continue;
		      }
		    }
		  }
		}
	      }
	      if (h2 == 0.0) {   //  h1 <> 0
		if (IsInside(B2,A1,A2,A3,VA0)) {
		  if (myFirstTime) {
		    myFirstTime = Standard_False;
		    myBegPoint = B2;
		  }
		  else {
		    if (!IsEqual(B2,myBegPoint)) {
		      myEndPoint = B2;
		      InsertInMap();
		      continue;
		    }
		  }
		  if (h3 == 0.0) {
		    if (IsInside(B3,A1,A2,A3,VA0)) {
		      if (!IsEqual(B3,myBegPoint)) {
			myEndPoint = B3;
			InsertInMap();
			continue;
		      }
		    }
		  }
		}
	      }
	      if (h3 == 0.0) {  //  h1 <> 0  and  h2 <> 0
		if (IsInside(B3,A1,A2,A3,VA0)) {
		  if (myFirstTime) {
		    myBegPoint = B3;
		    myFirstTime = Standard_False;
		  }
		  else {
		    if (!IsEqual(B3,myBegPoint)) {
		      myEndPoint = B3;
		      InsertInMap();
		      continue;
		    }
		  }
		}
	      }
	      if (Intersect(A1,A2,A3,OA1,VA0,V1,V2,h1,h2,ah1,ah2)) 
		continue;
	      if (Intersect(A1,A2,A3,OA1,VA0,V2,V3,h2,h3,ah2,ah3)) 
		continue;
	      if (Intersect(A1,A2,A3,OA1,VA0,V3,V1,h3,h1,ah3,ah1)) 
		continue;
	    }
	  }
	}
      }
    }
  }
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================
#define DIFFSIGN(a,b) (((a)>0.0 && (b)<0.0) || ((a)<0.0 && (b)>0.0))

Standard_Boolean IntPoly_ShapeSection::Intersect(const gp_Pnt& S1,
						 const gp_Pnt& S2,
						 const gp_Pnt& S3,
						 const gp_Vec& OS1,
						 const gp_Vec& VS0,
						 const gp_Vec& V1,
						 const gp_Vec& V2,
						 Standard_Real& h1,
						 Standard_Real& h2,
						 Standard_Real& ah1,
						 Standard_Real& ah2)
{ 
  if (DIFFSIGN(h1,h2)) {
    gp_XYZ OP;
    OP.SetLinearForm(ah2/(ah1+ah2),V1.XYZ(),
		     ah1/(ah1+ah2),V2.XYZ(),
		     OS1.XYZ());
    gp_Pnt P(OP.X(),OP.Y(),OP.Z());
    if (IsInside(P,S1,S2,S3,VS0)) {
      if (myFirstTime) {
	myFirstTime = Standard_False;
	myBegPoint = P;
      }
      else {
	if (!IsEqual(P,myBegPoint)) {
	  myEndPoint = P;
	  InsertInMap();
	  return (Standard_True);
	}
      }
    }
  }
  return (Standard_False);
}


//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================
#define epsilon 0.000000000001

Standard_Boolean IntPoly_ShapeSection::IsEqual(const gp_Pnt& Pt1, const gp_Pnt& Pt2)
{ 
  return (((Pt1.X() <= Pt2.X() && Pt2.X() < Pt1.X()+epsilon) ||
           (Pt2.X() <= Pt1.X() && Pt1.X() < Pt2.X()+epsilon)) && 
          ((Pt1.Y() <= Pt2.Y() && Pt2.Y() < Pt1.Y()+epsilon) ||
           (Pt2.Y() <= Pt1.Y() && Pt1.Y() < Pt2.Y()+epsilon)) && 
          ((Pt1.Z() <= Pt2.Z() && Pt2.Z() < Pt1.Z()+epsilon) ||
           (Pt2.Z() <= Pt1.Z() && Pt1.Z() < Pt2.Z()+epsilon)));
}


//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================

static Standard_Boolean IsInside1(Standard_Real PU,
				  Standard_Real PV,
				  Standard_Real *Pu,
				  Standard_Real *Pv)
{
  Standard_Real u = Pu[0] - PU;
  Standard_Real v = Pv[0] - PV;
  Standard_Real nu,nv;
  Standard_Integer SH = (v < 0.0)? -1 : 1;
  Standard_Integer NH,i,ip1 = 1;
  Standard_Integer NbCrossing = 0;

  for (i = 0;i < 3;i++,ip1++) {
    nu = Pu[ip1] - PU;
    nv = Pv[ip1] - PV;
    NH = (nv < 0.0)? -1 : 1;
    if (NH != SH) {
      if (u > 0.0 && nu > 0.0) NbCrossing++;
      else if (u > 0.0 || nu > 0.0) 
	if ((u - v * (nu - u) / (nv - v)) > 0.0) NbCrossing++;
      SH = NH;
    }
    u = nu;
    v = nv;
  }
  return (NbCrossing & 1);
}

#define TOLBRUIT 0.00000000000001

Standard_Boolean IntPoly_ShapeSection::IsInside(const gp_Pnt& P,
						const gp_Pnt& P1,
						const gp_Pnt& P2,
						const gp_Pnt& P3, 
						const gp_Vec& N0) 
{ 
  if (IsEqual(P,P1) || IsEqual(P,P2) || IsEqual(P,P3))
    return(Standard_True);
  Standard_Real Nx = Abs(N0.X());
  Standard_Real Ny = Abs(N0.Y());
  Standard_Real Nz = Abs(N0.Z());
  Standard_Real PU,PV,Pu[4],Pv[4];
  Standard_Integer Ind = 1;    //-- 1:x  2:y  3:z
  if (Nx > Ny) {               //-- x or z  
    if (Nx > Nz) Ind = 1;
    else         Ind = 3;
  }
  else {                       //-- y or z 
    if (Ny > Nz) Ind = 2;
    else         Ind = 3; 
  }
  if (Ind == 1) { 
    PU = P.Y();
    PV = P.Z();
    Pu[0] = Pu[3] = P1.Y();
    Pv[0] = Pv[3] = P1.Z();
    Pu[1] = P2.Y();
    Pv[1] = P2.Z();
    Pu[2] = P3.Y();
    Pv[2] = P3.Z();
  }
  else if (Ind == 2) { 
    PU = P.Z();
    PV = P.X();
    Pu[0] = Pu[3] = P1.Z();
    Pv[0] = Pv[3] = P1.X();
    Pu[1] = P2.Z();
    Pv[1] = P2.X();
    Pu[2] = P3.Z();
    Pv[2] = P3.X();
  }
  else {
    PU = P.X();
    PV = P.Y();
    Pu[0] = Pu[3] = P1.X();
    Pv[0] = Pv[3] = P1.Y();
    Pu[1] = P2.X();
    Pv[1] = P2.Y();
    Pu[2] = P3.X();
    Pv[2] = P3.Y();
  }
  if (IsInside1(PU,PV,Pu,Pv)) 
    return(Standard_True);
 
  if (IsInside1(PU+TOLBRUIT,PV,Pu,Pv)) 
    return(Standard_True);

  if (IsInside1(PU-TOLBRUIT,PV,Pu,Pv)) 
    return(Standard_True);

  if (IsInside1(PU,PV+TOLBRUIT,Pu,Pv)) 
    return(Standard_True); 
 
  if (IsInside1(PU,PV-TOLBRUIT,Pu,Pv)) 
    return(Standard_True);

  return(Standard_False);
}


//=======================================================================
//function : InsertInMap
//purpose  : 
//=======================================================================

void IntPoly_ShapeSection::InsertInMap()
{
  Standard_Integer Index;
  Standard_Real Big = Precision::Infinite();
  
  if (myMapBegPoints.Contains(myBegPoint)) {
    Index = myMapBegPoints.FindIndex(myBegPoint);
    Insert(myMapEndPoints.FindKey(Index),myBegPoint,myEndPoint);
    myCpt++;
    myMapBegPoints.Substitute(Index,gp_Pnt(Big,myCpt,myCpt));
    myMapEndPoints.Substitute(Index,gp_Pnt(Big,myCpt,myCpt));
  }
  else if (myMapEndPoints.Contains(myEndPoint)) {
    Index = myMapEndPoints.FindIndex(myEndPoint);
    Insert(myMapBegPoints.FindKey(Index),myEndPoint,myBegPoint);
    myCpt++;
    myMapBegPoints.Substitute(Index,gp_Pnt(Big,myCpt,myCpt));
    myMapEndPoints.Substitute(Index,gp_Pnt(Big,myCpt,myCpt));
  }
  else {
    myMapBegPoints.Add(myBegPoint);
    myMapEndPoints.Add(myEndPoint);
  }  
}


//=======================================================================
//function : Insert
//purpose  : 
//=======================================================================

void IntPoly_ShapeSection::Insert(const gp_Pnt& OldPnt,
				  const gp_Pnt& ComPnt,
				  const gp_Pnt& NewPnt)
{
  Standard_Integer i = 0;
  Standard_Integer NbSection = mySection.Length();
  Standard_Boolean IsInSection = Standard_False;
  
  while (i < NbSection) {
    i++;
    TColgp_SequenceOfPnt& CurSection = mySection.ChangeValue(i);
    if (IsEqual(OldPnt,CurSection.First())) {
      IsInSection = Standard_True;
      CurSection.Prepend(ComPnt);
      CurSection.Prepend(NewPnt);
      break;
    }
    if (IsEqual(OldPnt,CurSection.Last())) {
      IsInSection = Standard_True;
      CurSection.Append(ComPnt);
      CurSection.Append(NewPnt);
      break;
    }
    if (IsEqual(NewPnt,CurSection.First())) {
      IsInSection = Standard_True;
      CurSection.Prepend(ComPnt);
      CurSection.Prepend(OldPnt);
      break;
    }
    if (IsEqual(NewPnt,CurSection.Last())) {
      IsInSection = Standard_True;
      CurSection.Append(ComPnt);
      CurSection.Append(OldPnt);
      break;
    }
  }
  if (!(IsInSection)) {
    TColgp_SequenceOfPnt EmptySec;
    EmptySec.Append(OldPnt);
    EmptySec.Append(ComPnt);
    EmptySec.Append(NewPnt);
    mySection.Append(EmptySec);
  }  
}


//=======================================================================
//function : Concat
//purpose  : 
//=======================================================================

Standard_Integer IntPoly_ShapeSection::Concat(const gp_Pnt& BegPnt,
					      const gp_Pnt& EndPnt,
					      gp_Pnt& OutPnt)
{
  Standard_Integer i = 0;
  Standard_Integer NbSection = mySection.Length();
  Standard_Integer ConcatIdx = 0;
  
  while (i < NbSection) {
    i++;
    TColgp_SequenceOfPnt& CurSection = mySection.ChangeValue(i);
    if (IsEqual(BegPnt,CurSection.First())) {
      ConcatIdx = 1;
      myIndex = i;
      CurSection.Prepend(EndPnt);
      OutPnt = EndPnt;
      break;
    }
    if (IsEqual(BegPnt,CurSection.Last())) {
      ConcatIdx = 2;
      myIndex = i;
      CurSection.Append(EndPnt);
      OutPnt = EndPnt;
      break;
    }
    if (IsEqual(EndPnt,CurSection.First())) {
      ConcatIdx = 1;
      myIndex = i;
      CurSection.Prepend(BegPnt);
      OutPnt = BegPnt;
      break;
    }
    if (IsEqual(EndPnt,CurSection.Last())) {
      ConcatIdx = 2;
      myIndex = i;
      CurSection.Append(BegPnt);
      OutPnt = BegPnt;
      break;
    }
  }
  if (ConcatIdx == 0) {
    TColgp_SequenceOfPnt EmptySec;
    EmptySec.Append(BegPnt);
    EmptySec.Append(EndPnt);
    mySection.Append(EmptySec);
    NbSection++;
    myIndex = NbSection;
  }  
  return ConcatIdx;
}


//=======================================================================
//function : ConcatSection
//purpose  : 
//=======================================================================

void IntPoly_ShapeSection::ConcatSection(TColgp_SequenceOfPnt& Section,
					 const Standard_Integer NbSection,
					 const Standard_Integer Index)
{
  Standard_Integer j;
  Standard_Integer i = Index;
  gp_Pnt BegPnt = Section.First();
  gp_Pnt EndPnt = Section.Last();

  while (i <= NbSection) {
    TColgp_SequenceOfPnt& CurSection = mySection.ChangeValue(i);
    Standard_Integer CurSection_Length = CurSection.Length(); 
    if (IsEqual(BegPnt,CurSection.First())) {
      for (j = 2;j <= CurSection_Length;j++) 
	Section.Prepend(CurSection.Value(j));
      mySection.Remove(i);
      ConcatSection(Section,NbSection-1,Index);
      break;
    }
    else
      if (IsEqual(BegPnt,CurSection.Last())) {
	for (j = CurSection_Length-1;j >= 1;j--) 
	  Section.Prepend(CurSection.Value(j));
	mySection.Remove(i);
	ConcatSection(Section,NbSection-1,Index);
 	break;
      }
      else
	if (IsEqual(EndPnt,CurSection.First())) {
	  for (j = 2;j <= CurSection_Length;j++) 
	    Section.Append(CurSection.Value(j));
	  mySection.Remove(i);
	  ConcatSection(Section,NbSection-1,Index);
	  break;
	}
	else
	  if (IsEqual(EndPnt,CurSection.Last())) {
	    for (j = CurSection_Length-1;j >= 1;j--) 
	      Section.Append(CurSection.Value(j));
	    mySection.Remove(i);
	    ConcatSection(Section,NbSection-1,Index);
	    break;
	  }
    i++;
  }
}


//=======================================================================
//function : ForwContruction
//purpose  : 
//=======================================================================

void IntPoly_ShapeSection::ForwConstruction(const gp_Pnt& Point)
{
  if (myMapBegPoints.Contains(Point)) {
    Standard_Integer Index = myMapBegPoints.FindIndex(Point);
    gp_Pnt Pnt = myMapEndPoints.FindKey(Index);
    (mySection.ChangeValue(myIndex)).Append(Pnt);
    Standard_Real Big = Precision::Infinite();
    myCpt++;
    myMapBegPoints.Substitute(Index,gp_Pnt(Big,myCpt,myCpt));
    myMapEndPoints.Substitute(Index,gp_Pnt(Big,myCpt,myCpt));
    ForwConstruction(Pnt);
  }
}


//=======================================================================
//function : PrevContruction
//purpose  : 
//=======================================================================

void IntPoly_ShapeSection::PrevConstruction(const gp_Pnt& Point)
{
  if (myMapEndPoints.Contains(Point)) {
    Standard_Integer Index = myMapEndPoints.FindIndex(Point);
    gp_Pnt Pnt = myMapBegPoints.FindKey(Index);
    (mySection.ChangeValue(myIndex)).Prepend(Pnt);
    Standard_Real Big = Precision::Infinite();
    myCpt++;
    myMapBegPoints.Substitute(Index,gp_Pnt(Big,myCpt,myCpt));
    myMapEndPoints.Substitute(Index,gp_Pnt(Big,myCpt,myCpt));
    PrevConstruction(Pnt);
  }
}


//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

Standard_Integer IntPoly_ShapeSection::NbEdges()
{ return myNbEdges; }


//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

TopoDS_Edge IntPoly_ShapeSection::Edge(const Standard_Integer Index)
{
  const TColgp_SequenceOfPnt& CurSection = mySection.ChangeValue(Index);
  Standard_Integer NbPoints = CurSection.Length();
  TColgp_Array1OfPnt TabPnt(1,NbPoints);
  //gp_Pnt CurPoint;
  for (Standard_Integer i = 1 ; i <= NbPoints ; i++) {
    TabPnt.SetValue(i,CurSection.Value(i));
  }
  Handle(Poly_Polygon3D) Pol = new Poly_Polygon3D(TabPnt);
  TopoDS_Edge Edge;
  BRep_Builder B;
  B.MakeEdge(Edge,Pol);
  return Edge;
}







