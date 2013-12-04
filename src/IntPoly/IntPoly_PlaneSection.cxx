// Created on: 1995-07-17
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

#include <IntPoly_PlaneSection.ixx>

#include <IntPoly_SequenceOfSequenceOfPnt2d.hxx>
#include <IntPoly_IndexedMapOfPnt2d.hxx>
#include <IntPoly_Pnt2dHasher.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <OSD_Chronometer.hxx>


//=======================================================================
//function : IntPoly_PlaneSection
//purpose  : 
//=======================================================================

IntPoly_PlaneSection::IntPoly_PlaneSection()
{}


//=======================================================================
//function : IntPoly_PlaneSection
//purpose  : 
//=======================================================================

IntPoly_PlaneSection::IntPoly_PlaneSection(const TopoDS_Shape& S,
					   const gp_Pln& P)
{
  myShape = S;
  myTransform.SetTransformation(P.Position()); 
  gp_Ax3 Ax0(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  myBackTransform.SetTransformation(P.Position(),Ax0);
  Section();
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void IntPoly_PlaneSection::Section()
{ 
  TopExp_Explorer ex;
  Standard_Integer NbTrian,i,i1,i2,i3,Result;
  Standard_Real Big = Precision::Infinite();
  Standard_Boolean Inter;
  //gp_Pnt Nod1,Nod2,Nod3;
  gp_Pnt2d BegPoint,EndPoint,OutPoint;
  TopLoc_Location Loc;
  Handle(Poly_Triangulation) Tr;
  myCpt = 0;
  
  for (ex.Init(myShape,TopAbs_FACE);ex.More();ex.Next()) {
    Tr = (BRep_Tool::Triangulation(TopoDS::Face(ex.Current()),Loc));
    if (!Tr.IsNull()) {
      NbTrian = Tr->NbTriangles();
      const Poly_Array1OfTriangle& TabTrian = Tr->Triangles();
      const TColgp_Array1OfPnt&    TabNodes = Tr->Nodes();
      TColgp_Array1OfPnt  TabTransfNodes(1,Tr->NbNodes());      
      for (i = 1;i <= Tr->NbNodes();i++) {
	if (!Loc.IsIdentity())
	  TabTransfNodes(i) = 
	    (TabNodes(i).Transformed(Loc.Transformation()))
	      .Transformed(myTransform);
	else 
	  TabTransfNodes(i) = TabNodes(i).Transformed(myTransform);
      }
      for (i = 1;i <= NbTrian;i++) {
	TabTrian(i).Get(i1,i2,i3);
	Inter = Intersect(TabTransfNodes(i1),
			  TabTransfNodes(i2),
			  TabTransfNodes(i3),
			  BegPoint,EndPoint);
	if (Inter) {
	  if (myMapBegPoints.Contains(BegPoint)) {
	    Standard_Integer Index = myMapBegPoints.FindIndex(BegPoint);
	    Insert(myMapEndPoints.FindKey(Index),BegPoint,EndPoint);
	    myCpt++;
	    myMapBegPoints.Substitute(Index,gp_Pnt2d(Big,myCpt));
	    myMapEndPoints.Substitute(Index,gp_Pnt2d(Big,myCpt));
	  }
	  else if (myMapEndPoints.Contains(EndPoint)) {
	    Standard_Integer Index = myMapEndPoints.FindIndex(EndPoint);
	    Insert(myMapBegPoints.FindKey(Index),EndPoint,BegPoint);
	    myCpt++;
	    myMapBegPoints.Substitute(Index,gp_Pnt2d(Big,myCpt));
	    myMapEndPoints.Substitute(Index,gp_Pnt2d(Big,myCpt));
	  }
	  else {
	    myMapBegPoints.Add(BegPoint);
	    myMapEndPoints.Add(EndPoint);
	  }  
	}
      }
    } 
  }
  
  Standard_Integer NbLinks = myMapBegPoints.Extent();
  i = 1;
  while (i <= NbLinks) {
    if (!(Precision::IsInfinite((myMapBegPoints.FindKey(i)).X()))) {
      BegPoint = myMapBegPoints.FindKey(i);
      EndPoint = myMapEndPoints.FindKey(i);
      myCpt++;
      myMapBegPoints.Substitute(i,gp_Pnt2d(Big,myCpt));
      myMapEndPoints.Substitute(i,gp_Pnt2d(Big,myCpt));
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
//function : Intersect
//purpose  : Determines the 2 2D-points resulting of the intersection
//           of the triangle (Point1,Point2,Point3) by the plane
//=======================================================================

Standard_Boolean IntPoly_PlaneSection::Intersect(const gp_Pnt& Point1,
						 const gp_Pnt& Point2,
						 const gp_Pnt& Point3,
						 gp_Pnt2d& OutPoint1,
						 gp_Pnt2d& OutPoint2)
{
  gp_Pnt Pt1 = Point1;
  gp_Pnt Pt2 = Point2;
  gp_Pnt Pt3 = Point3;
  Standard_Real z1 = Pt1.Z();
  Standard_Real z2 = Pt2.Z();
  Standard_Real z3 = Pt3.Z();
  Standard_Boolean intersection = Standard_False;

  if ((z1 == 0) && (z2 == 0) && (z3 == 0)) 
    return intersection;
  if ((z1 == 0) && (z2 == 0)) {
    intersection = Standard_True;
    OutPoint1.SetCoord(Pt1.X(),Pt1.Y());
    OutPoint2.SetCoord(Pt2.X(),Pt2.Y());
    return intersection;
  }
  if ((z2 == 0) && (z3 == 0)) {
    intersection = Standard_True;
    OutPoint1.SetCoord(Pt2.X(),Pt2.Y());
    OutPoint2.SetCoord(Pt3.X(),Pt3.Y());
    return intersection;
  }
  if ((z3 == 0) && (z1 == 0)) {
    intersection = Standard_True;
    OutPoint1.SetCoord(Pt3.X(),Pt3.Y());
    OutPoint2.SetCoord(Pt1.X(),Pt1.Y());
    return intersection;
  }
  if (z1 == 0) {
    if ((z2 < 0) && (z3 > 0)) 
      intersection = Standard_True;
    if ((z2 > 0) && (z3 < 0)) {
      intersection = Standard_True;
      gp_Pnt Pt = Pt2;
      Pt2 = Pt3;
      Pt3 = Pt; 
    }
    if (intersection) {
      Standard_Real u1 = Pt2.X();
      Standard_Real v1 = Pt2.Y();
      z1 = Pt2.Z();
      Standard_Real u2 = Pt3.X();
      Standard_Real v2 = Pt3.Y();
      z2 = Pt3.Z();
      Standard_Real U = u1 - z1 * (u2 - u1)/(z2 - z1);
      Standard_Real V = v1 - z1 * (v2 - v1)/(z2 - z1);
      OutPoint1.SetCoord(Pt1.X(),Pt1.Y());
      OutPoint2.SetCoord(U,V); 
      return intersection;
    }
  }
  else
    if (z2 == 0) {
      if ((z1 < 0) && (z3 > 0)) 
	intersection = Standard_True;
      if ((z1 > 0) && (z3 < 0)) {
	intersection = Standard_True;
	gp_Pnt Pt = Pt1;
	Pt1 = Pt3;
	Pt3 = Pt; 
      }
      if (intersection) {
	Standard_Real u1 = Pt1.X();
	Standard_Real v1 = Pt1.Y();
	z1 = Pt1.Z();
	Standard_Real u2 = Pt3.X();
	Standard_Real v2 = Pt3.Y();
	z2 = Pt3.Z();
	Standard_Real U = u1 - z1 * (u2 - u1)/(z2 - z1);
	Standard_Real V = v1 - z1 * (v2 - v1)/(z2 - z1);
	OutPoint1.SetCoord(Pt2.X(),Pt2.Y());
	OutPoint2.SetCoord(U,V); 
	return intersection;
      }
    }
    else
      if (z3 == 0) {
	if ((z2 < 0) && (z1 > 0)) 
	  intersection = Standard_True;
	if ((z2 > 0) && (z1 < 0)) {
	  intersection = Standard_True;
	  gp_Pnt Pt = Pt2;
	  Pt2 = Pt1;
	  Pt1 = Pt; 
	}
	if (intersection) {
	  Standard_Real u1 = Pt2.X();
	  Standard_Real v1 = Pt2.Y();
	  z1 = Pt2.Z();
	  Standard_Real u2 = Pt1.X();
	  Standard_Real v2 = Pt1.Y();
	  z2 = Pt1.Z();
	  Standard_Real U = u1 - z1 * (u2 - u1)/(z2 - z1);
	  Standard_Real V = v1 - z1 * (v2 - v1)/(z2 - z1);
	  OutPoint1.SetCoord(Pt3.X(),Pt3.Y());
	  OutPoint2.SetCoord(U,V); 
	  return intersection;
	}
      }
  
  if ((z1 < 0) && (z2 > 0)) 
    intersection = Standard_True;
  if ((z1 > 0) && (z2 < 0)) { 
    intersection = Standard_True;
    gp_Pnt Pt = Pt1;
    Pt1 = Pt2;
    Pt2 = Pt; 
  }
  if (intersection) {
    Standard_Real u1 = Pt1.X();
    Standard_Real v1 = Pt1.Y();
    z1 = Pt1.Z();
    Standard_Real u2 = Pt2.X();
    Standard_Real v2 = Pt2.Y();
    z2 = Pt2.Z();
    Standard_Real U = u1 - z1 * (u2 - u1)/(z2 - z1);
    Standard_Real V = v1 - z1 * (v2 - v1)/(z2 - z1);
    OutPoint1.SetCoord(U,V); 
    intersection = Standard_False;
    Pt1 = Point1;
    Pt2 = Point2;
    z1 = Pt1.Z();
    z2 = Pt2.Z();
    if ((z2 < 0) && (z3 > 0)) 
      intersection = Standard_True;
    if ((z2 > 0) && (z3 < 0)) {
      intersection = Standard_True;
      gp_Pnt Pt = Pt2;
      Pt2 = Pt3;
      Pt3 = Pt; 
    }
    if (intersection) {
      u1 = Pt2.X();
      v1 = Pt2.Y();
      z1 = Pt2.Z();
      u2 = Pt3.X();
      v2 = Pt3.Y();
      z2 = Pt3.Z();
      U = u1 - z1 * (u2 - u1)/(z2 - z1);
      V = v1 - z1 * (v2 - v1)/(z2 - z1);
      OutPoint2.SetCoord(U,V); 
      return intersection;
    }
    else {
      intersection = Standard_True;
      Pt3 = Point3;
      z3 = Pt3.Z();
      if ((z3 > 0) && (z1 < 0)) {
	gp_Pnt Pt = Pt3;
	Pt3 = Pt1;
	Pt1 = Pt; 
      }
      u1 = Pt3.X();
      v1 = Pt3.Y();
      z1 = Pt3.Z();
      u2 = Pt1.X();
      v2 = Pt1.Y();
      z2 = Pt1.Z();
      U = u1 - z1 * (u2 - u1)/(z2 - z1);
      V = v1 - z1 * (v2 - v1)/(z2 - z1);
      OutPoint2.SetCoord(U,V); 
      return intersection;
    }
  }
  else {
    if ((z2 < 0) && (z3 > 0)) 
      intersection = Standard_True;
    if ((z2 > 0) && (z3 < 0)) {
      intersection = Standard_True;
      gp_Pnt Pt = Pt2;
      Pt2 = Pt3;
      Pt3 = Pt; 
    }
    if (intersection) {
      Standard_Real u1 = Pt2.X();
      Standard_Real v1 = Pt2.Y();
      z1 = Pt2.Z();
      Standard_Real u2 = Pt3.X();
      Standard_Real v2 = Pt3.Y();
      z2 = Pt3.Z();
      Standard_Real U = u1 - z1 * (u2 - u1)/(z2 - z1);
      Standard_Real V = v1 - z1 * (v2 - v1)/(z2 - z1);
      OutPoint1.SetCoord(U,V); 
      Pt3 = Point3;
      z3 = Pt3.Z();
      if ((z3 > 0) && (z1 < 0)) {
	gp_Pnt Pt = Pt3;
	Pt3 = Pt1;
	Pt1 = Pt; 
      }
      u1 = Pt3.X();
      v1 = Pt3.Y();
      z1 = Pt3.Z();
      u2 = Pt1.X();
      v2 = Pt1.Y();
      z2 = Pt1.Z();
      U = u1 - z1 * (u2 - u1)/(z2 - z1);
      V = v1 - z1 * (v2 - v1)/(z2 - z1);
      OutPoint2.SetCoord(U,V); 
      return intersection;
    }
  }
  return intersection;
}


//=======================================================================
//function : Insert
//purpose  : Sees whether OldPnt and NewPnt begins or ends a section
//           created before and adds the 2 others points.
//           If not, creates a section
//=======================================================================

void IntPoly_PlaneSection::Insert(const gp_Pnt2d& OldPnt,
				  const gp_Pnt2d& ComPnt,
				  const gp_Pnt2d& NewPnt)
{
  Standard_Integer i = 0;
  Standard_Integer NbSection = mySection.Length();
  Standard_Boolean IsInSection = Standard_False;
  
  while (i < NbSection) {
    i++;
    TColgp_SequenceOfPnt2d& CurSection = mySection.ChangeValue(i);
    if (IntPoly_Pnt2dHasher::IsEqual(OldPnt,CurSection.First())) {
      IsInSection = Standard_True;
      CurSection.Prepend(ComPnt);
      CurSection.Prepend(NewPnt);
      break;
    }
    if (IntPoly_Pnt2dHasher::IsEqual(OldPnt,CurSection.Last())) {
      IsInSection = Standard_True;
      CurSection.Append(ComPnt);
      CurSection.Append(NewPnt);
      break;
    }
    if (IntPoly_Pnt2dHasher::IsEqual(NewPnt,CurSection.First())) {
      IsInSection = Standard_True;
      CurSection.Prepend(ComPnt);
      CurSection.Prepend(OldPnt);
      break;
    }
    if (IntPoly_Pnt2dHasher::IsEqual(NewPnt,CurSection.Last())) {
      IsInSection = Standard_True;
      CurSection.Append(ComPnt);
      CurSection.Append(OldPnt);
      break;
    }
  }
  if (!(IsInSection)) {
    TColgp_SequenceOfPnt2d EmptySec;
    EmptySec.Append(OldPnt);
    EmptySec.Append(ComPnt);
    EmptySec.Append(NewPnt);
    mySection.Append(EmptySec);
  }  
}


//=======================================================================
//function : Concat
//purpose  : sees whether BegPnt or EndPnt begins or ends a section
//           created before, and returns the other point to continue 
//           the construction
//           Returns 2 if the construction is 'Forward'
//           Returns 1 if the construction is 'Previous'
//           Returns 0 if not and creates a section
//=======================================================================

Standard_Integer IntPoly_PlaneSection::Concat(const gp_Pnt2d& BegPnt,
					      const gp_Pnt2d& EndPnt,
					      gp_Pnt2d& OutPnt)
{
  Standard_Integer i = 0;
  Standard_Integer NbSection = mySection.Length();
  Standard_Integer ConcatIdx = 0;
  
  while (i < NbSection) {
    i++;
    TColgp_SequenceOfPnt2d& CurSection = mySection.ChangeValue(i);
    if (IntPoly_Pnt2dHasher::IsEqual(BegPnt,CurSection.First())) {
      ConcatIdx = 1;
      myIndex = i;
      CurSection.Prepend(EndPnt);
      OutPnt = EndPnt;
      break;
    }
    if (IntPoly_Pnt2dHasher::IsEqual(BegPnt,CurSection.Last())) {
      ConcatIdx = 2;
      myIndex = i;
      CurSection.Append(EndPnt);
      OutPnt = EndPnt;
      break;
    }
    if (IntPoly_Pnt2dHasher::IsEqual(EndPnt,CurSection.First())) {
      ConcatIdx = 1;
      myIndex = i;
      CurSection.Prepend(BegPnt);
      OutPnt = BegPnt;
      break;
    }
    if (IntPoly_Pnt2dHasher::IsEqual(EndPnt,CurSection.Last())) {
      ConcatIdx = 2;
      myIndex = i;
      CurSection.Append(BegPnt);
      OutPnt = BegPnt;
      break;
    }
  }
  if (ConcatIdx == 0) {
    TColgp_SequenceOfPnt2d EmptySec;
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
//purpose  : Sees whether Section begins or ends another one in  mySection,
//           from the rank 'Index' to the last one 'NbSection'
//=======================================================================

void IntPoly_PlaneSection::ConcatSection(TColgp_SequenceOfPnt2d& Section,
					 const Standard_Integer NbSection,
					 const Standard_Integer Index)
{
  Standard_Integer j;
  Standard_Integer i = Index;
  gp_Pnt2d BegPnt = Section.First();
  gp_Pnt2d EndPnt = Section.Last();

  while (i <= NbSection) {
    TColgp_SequenceOfPnt2d& CurSection = mySection.ChangeValue(i);
    Standard_Integer CurSection_Length = CurSection.Length(); 
    if (IntPoly_Pnt2dHasher::IsEqual(BegPnt,CurSection.First())) {
      for (j = 2;j <= CurSection_Length;j++) 
	Section.Prepend(CurSection.Value(j));
      mySection.Remove(i);
      ConcatSection(Section,NbSection-1,Index);
      break;
    }
    else
      if (IntPoly_Pnt2dHasher::IsEqual(BegPnt,CurSection.Last())) {
	for (j = CurSection_Length-1;j >= 1;j--) 
	  Section.Prepend(CurSection.Value(j));
	mySection.Remove(i);
	ConcatSection(Section,NbSection-1,Index);
 	break;
      }
      else
	if (IntPoly_Pnt2dHasher::IsEqual(EndPnt,CurSection.First())) {
	  for (j = 2;j <= CurSection_Length;j++) 
	    Section.Append(CurSection.Value(j));
	  mySection.Remove(i);
	  ConcatSection(Section,NbSection-1,Index);
	  break;
	}
	else
	  if (IntPoly_Pnt2dHasher::IsEqual(EndPnt,CurSection.Last())) {
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
//purpose  : Builds a section from Point in this way  
//                    ___   ___   ___   ___
//               <---/__/--/__/--/__/--/__/--Point
//=======================================================================

void IntPoly_PlaneSection::ForwConstruction(const gp_Pnt2d& Point)
{
  if (myMapBegPoints.Contains(Point)) {
    Standard_Integer Index = myMapBegPoints.FindIndex(Point);
    gp_Pnt2d Pnt = myMapEndPoints.FindKey(Index);
    (mySection.ChangeValue(myIndex)).Append(Pnt);
    Standard_Real Big = Precision::Infinite();
    myCpt++;
    myMapBegPoints.Substitute(Index,gp_Pnt2d(Big,myCpt));
    myMapEndPoints.Substitute(Index,gp_Pnt2d(Big,myCpt));
    ForwConstruction(Pnt);
  }
}


//=======================================================================
//function : PrevContruction
//purpose  : Builds a section from Point in this way  
//                      ___   ___   ___   ___
//              Point--/__/--/__/--/__/--/__/--->
//=======================================================================

void IntPoly_PlaneSection::PrevConstruction(const gp_Pnt2d& Point)
{
  if (myMapEndPoints.Contains(Point)) {
    Standard_Integer Index = myMapEndPoints.FindIndex(Point);
    gp_Pnt2d Pnt = myMapBegPoints.FindKey(Index);
    (mySection.ChangeValue(myIndex)).Prepend(Pnt);
    Standard_Real Big = Precision::Infinite();
    myCpt++;
    myMapBegPoints.Substitute(Index,gp_Pnt2d(Big,myCpt));
    myMapEndPoints.Substitute(Index,gp_Pnt2d(Big,myCpt));
    PrevConstruction(Pnt);
  }
}


//=======================================================================
//function : NbEdges
//purpose  : Returns the final number of edges in the section
//=======================================================================

Standard_Integer IntPoly_PlaneSection::NbEdges()
{ return myNbEdges; }


//=======================================================================
//function : Edge
//purpose  : Builds the edges of the section
//=======================================================================

TopoDS_Edge IntPoly_PlaneSection::Edge(const Standard_Integer Index)
{
  const TColgp_SequenceOfPnt2d& CurSection = mySection.ChangeValue(Index);
  Standard_Integer NbPoints = CurSection.Length();
  TColgp_Array1OfPnt TabPnt(1,NbPoints);
  gp_Pnt CurPoint;
  for (Standard_Integer i = 1 ; i <= NbPoints ; i++) {
    CurPoint.SetX((CurSection.Value(i)).X());
    CurPoint.SetY((CurSection.Value(i)).Y());
    CurPoint.SetZ(0);
    TabPnt.SetValue(i,CurPoint.Transformed(myBackTransform));
  }
  Handle(Poly_Polygon3D) Pol = new Poly_Polygon3D(TabPnt);
  TopoDS_Edge Edge;
  BRep_Builder B;
  B.MakeEdge(Edge,Pol);
  return Edge;
}








