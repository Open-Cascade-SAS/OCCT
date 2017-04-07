// Created on: 1999-03-05
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
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

//  modified by Edward AGAPOV (eap) Tue Jan 22 2002 (bug occ53)
//  - improve SectionLine table management (avoid memory reallocation)
//  - some protection against arrays overflow
//  modified by Edward AGAPOV (eap) Thu Feb 14 2002 (occ139)
//  - make Section Line parts rightly connected (prepend 2nd part to the 1st)
//  - TriangleShape() for debugging purpose
//  Modified by skv - Thu Sep 25 17:42:42 2003 OCC567
//  modified by ofv Thu Apr  8 14:58:13 2004 fip

#include <Adaptor3d_HSurface.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>
#include <IntPolyh_ListOfCouples.hxx>
#include <IntPolyh_Couple.hxx>
#include <IntPolyh_Edge.hxx>
#include <IntPolyh_MaillageAffinage.hxx>
#include <IntPolyh_Point.hxx>
#include <IntPolyh_SectionLine.hxx>
#include <IntPolyh_StartPoint.hxx>
#include <IntPolyh_Triangle.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <NCollection_UBTree.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <algorithm>
#include <NCollection_IndexedDataMap.hxx>

typedef NCollection_Array1<Standard_Integer> IntPolyh_ArrayOfInteger;
typedef NCollection_IndexedDataMap
  <Standard_Integer,
   IntPolyh_ArrayOfInteger,
   TColStd_MapIntegerHasher> IntPolyh_IndexedDataMapOfIntegerArrayOfInteger;


static Standard_Real MyTolerance=10.0e-7;
static Standard_Real MyConfusionPrecision=10.0e-12;
static Standard_Real SquareMyConfusionPrecision=10.0e-24;
//
static
  inline Standard_Real maxSR(const Standard_Real a,
                             const Standard_Real b,
                             const Standard_Real c);

static
  inline Standard_Real minSR(const Standard_Real a,
                             const Standard_Real b,
                             const Standard_Real c);
static
  Standard_Integer project6(const IntPolyh_Point &ax, 
                            const IntPolyh_Point &p1,
                            const IntPolyh_Point &p2, 
                            const IntPolyh_Point &p3, 
                            const IntPolyh_Point &q1,
                            const IntPolyh_Point &q2, 
                            const IntPolyh_Point &q3);
static
  void TestNbPoints(const Standard_Integer ,
                    Standard_Integer &NbPoints,
                    Standard_Integer &NbPointsTotal,
                    const IntPolyh_StartPoint &Pt1,
                    const IntPolyh_StartPoint &Pt2,
                    IntPolyh_StartPoint &SP1,
                    IntPolyh_StartPoint &SP2);
static
  void CalculPtsInterTriEdgeCoplanaires(const Standard_Integer TriSurfID,
                                        const IntPolyh_Point &NormaleTri,
                                        const IntPolyh_Triangle &Tri1,
                                        const IntPolyh_Triangle &Tri2,
                                        const IntPolyh_Point &PE1,
                                        const IntPolyh_Point &PE2,
                                        const IntPolyh_Point &Edge,
                                        const Standard_Integer EdgeIndex,
                                        const IntPolyh_Point &PT1,
                                        const IntPolyh_Point &PT2,
                                        const IntPolyh_Point &Cote,
                                        const Standard_Integer CoteIndex,
                                        IntPolyh_StartPoint &SP1,
                                        IntPolyh_StartPoint &SP2,
                                        Standard_Integer &NbPoints); 
static
  Standard_Boolean CheckCoupleAndGetAngle(const Standard_Integer T1, 
                                          const Standard_Integer T2,
                                          Standard_Real& Angle, 
                                          IntPolyh_ListOfCouples &TTrianglesContacts);
static
  Standard_Boolean CheckCoupleAndGetAngle2(const Standard_Integer T1,
                                           const Standard_Integer T2,
                                           const Standard_Integer T11, 
                                           const Standard_Integer T22,
                                           IntPolyh_ListIteratorOfListOfCouples& theItCT11,
                                           IntPolyh_ListIteratorOfListOfCouples& theItCT22,
                                           Standard_Real & Angle,
                                           IntPolyh_ListOfCouples &TTrianglesContacts);
static
  Standard_Integer CheckNextStartPoint(IntPolyh_SectionLine & SectionLine,
                                       IntPolyh_ArrayOfTangentZones & TTangentZones,
                                       IntPolyh_StartPoint & SP,
                                       const Standard_Boolean Prepend=Standard_False); 

static
  Standard_Boolean IsDegenerated(const Handle(Adaptor3d_HSurface)& aS,
                                 const Standard_Integer aIndex,
                                 const Standard_Real aTol2,
                                 Standard_Real& aDegX);
static
  void DegeneratedIndex(const TColStd_Array1OfReal& Xpars,
                        const Standard_Integer aNbX,
                        const Handle(Adaptor3d_HSurface)& aS,
                        const Standard_Integer aIsoDirection,
                        Standard_Integer& aI1,
                        Standard_Integer& aI2);
static
  void EnlargeZone(const Handle(Adaptor3d_HSurface)& MaSurface,
                   Standard_Real &u0, 
                   Standard_Real &u1, 
                   Standard_Real &v0, 
                   Standard_Real &v1);

//=======================================================================
//class : IntPolyh_BoxBndTreeSelector
//purpose  : Select interfering bounding boxes
//=======================================================================
typedef NCollection_UBTree<Standard_Integer, Bnd_Box> IntPolyh_BoxBndTree;
class IntPolyh_BoxBndTreeSelector : public IntPolyh_BoxBndTree::Selector {
 public:
  IntPolyh_BoxBndTreeSelector(const Bnd_Box& theBox) : myBox(theBox) {}
  //
  virtual Standard_Boolean Reject(const Bnd_Box& theOther) const
  {
    return myBox.IsOut(theOther);
  }
  //
  virtual Standard_Boolean Accept(const Standard_Integer &theInd)
  {
    myIndices.Append(theInd);
    return Standard_True;
  }
  //
  const TColStd_ListOfInteger& Indices() const
  {
    return myIndices;
  }
 private:
  Bnd_Box myBox;
  TColStd_ListOfInteger myIndices;
};

//=======================================================================
//function : GetInterferingTriangles
//purpose  : Returns indices of the triangles with interfering bounding boxes
//=======================================================================
static
  void GetInterferingTriangles(IntPolyh_ArrayOfTriangles& theTriangles1,
                               const IntPolyh_ArrayOfPoints& thePoints1,
                               IntPolyh_ArrayOfTriangles& theTriangles2,
                               const IntPolyh_ArrayOfPoints& thePoints2,
                               IntPolyh_IndexedDataMapOfIntegerArrayOfInteger& theCouples)
{
  // To find the triangles with interfering bounding boxes
  // use the algorithm of unbalanced binary tree of overlapping bounding boxes
  IntPolyh_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  // 1. Fill the tree with the boxes of the triangles from second surface
  Standard_Integer i, aNbT2 = theTriangles2.NbItems();
  for (i = 0; i < aNbT2; ++i) {
    IntPolyh_Triangle& aT = theTriangles2[i];
    if (!aT.IsIntersectionPossible() || aT.IsDegenerated()) {
      continue;
    }
    //
    const Bnd_Box& aBox = aT.BoundingBox(thePoints2);
    aTreeFiller.Add(i, aBox);
  }
  //
  // 2. Shake the tree filler
  aTreeFiller.Fill();
  //
  // 3. Find boxes interfering with the first triangles
  Standard_Integer aNbT1 = theTriangles1.NbItems();
  for (i = 0; i < aNbT1; ++i) {
    IntPolyh_Triangle& aT = theTriangles1[i];
    if (!aT.IsIntersectionPossible() || aT.IsDegenerated()) {
      continue;
    }
    //
    const Bnd_Box& aBox = aT.BoundingBox(thePoints1);
    //
    IntPolyh_BoxBndTreeSelector aSelector(aBox);
    if (!aBBTree.Select(aSelector)) {
      continue;
    }
    //
    const TColStd_ListOfInteger& aLI = aSelector.Indices();
    // Sort the indices
    IntPolyh_ArrayOfInteger anArr(1, aLI.Extent());
    TColStd_ListIteratorOfListOfInteger aItLI(aLI);
    for (Standard_Integer j = 1; aItLI.More(); aItLI.Next(), ++j) {
      anArr(j) = aItLI.Value();
    }
    //
    std::sort(anArr.begin(), anArr.end());
    //
    theCouples.Add(i, anArr);
  }
}

//=======================================================================
//function : IntPolyh_MaillageAffinage
//purpose  : 
//=======================================================================
IntPolyh_MaillageAffinage::IntPolyh_MaillageAffinage
  (const Handle(Adaptor3d_HSurface)& Surface1,
   const Handle(Adaptor3d_HSurface)& Surface2,
   const Standard_Integer )
:
  MaSurface1(Surface1), 
  MaSurface2(Surface2), 
  NbSamplesU1(10), 
  NbSamplesU2(10), 
  NbSamplesV1(10), 
  NbSamplesV2(10),
  FlecheMax1(0.0), 
  FlecheMax2(0.0), 
  FlecheMin1(0.0), 
  FlecheMin2(0.0),
  FlecheMoy1(0.0), 
  FlecheMoy2(0.0), 
  myEnlargeZone(Standard_False) 
{ 
}
//=======================================================================
//function : IntPolyh_MaillageAffinage
//purpose  : 
//=======================================================================
IntPolyh_MaillageAffinage::IntPolyh_MaillageAffinage
  (const Handle(Adaptor3d_HSurface)& Surface1,
   const Standard_Integer NbSU1,
   const Standard_Integer NbSV1,
   const Handle(Adaptor3d_HSurface)& Surface2,
   const Standard_Integer NbSU2,
   const Standard_Integer NbSV2,
   const Standard_Integer )
:
  MaSurface1(Surface1), 
  MaSurface2(Surface2), 
  NbSamplesU1(NbSU1), 
  NbSamplesU2(NbSU2), 
  NbSamplesV1(NbSV1), 
  NbSamplesV2(NbSV2),
  FlecheMax1(0.0), 
  FlecheMax2(0.0), 
  FlecheMin1(0.0), 
  FlecheMin2(0.0),
  FlecheMoy1(0.0), 
  FlecheMoy2(0.0), 
  myEnlargeZone(Standard_False)
{ 
}
//=======================================================================
//function : FillArrayOfPnt
//purpose  : Compute points on one surface and fill an array of points
//=======================================================================
void IntPolyh_MaillageAffinage::FillArrayOfPnt
  (const Standard_Integer SurfID)
{
  Standard_Integer NbSamplesU, NbSamplesV, i, aNbSamplesU1, aNbSamplesV1;
  Standard_Real u0, u1, v0, v1, aU, aV, dU, dV;
  //
  const Handle(Adaptor3d_HSurface)& MaSurface=(SurfID==1)? MaSurface1 : MaSurface2;
  NbSamplesU=(SurfID==1)? NbSamplesU1:NbSamplesU2;
  NbSamplesV=(SurfID==1)? NbSamplesV1:NbSamplesV2;
  //
  u0 = (MaSurface)->FirstUParameter();  
  u1 = (MaSurface)->LastUParameter();
  v0 = (MaSurface)->FirstVParameter();  
  v1 = (MaSurface)->LastVParameter();  

  if(myEnlargeZone) { 
    EnlargeZone(MaSurface, u0, u1, v0, v1);
  }
  //
  TColStd_Array1OfReal aUpars(1, NbSamplesU);
  TColStd_Array1OfReal aVpars(1, NbSamplesV);
  //
  aNbSamplesU1=NbSamplesU-1;
  aNbSamplesV1=NbSamplesV-1;
  //
  dU=(u1-u0)/Standard_Real(aNbSamplesU1);
  dV=(v1-v0)/Standard_Real(aNbSamplesV1);
  //
  for (i=0; i<NbSamplesU; ++i) {
    aU=u0+i*dU;
    if (i==aNbSamplesU1) {
      aU=u1;
    }
    aUpars.SetValue(i+1, aU);
  }
  //
  for (i=0; i<NbSamplesV; ++i) {
    aV=v0+i*dV;
    if (i==aNbSamplesV1) {
      aV=v1;
    }
    aVpars.SetValue(i+1, aV);
  }
  //
  FillArrayOfPnt(SurfID, aUpars, aVpars);
}
//=======================================================================
//function : FillArrayOfPnt
//purpose  : Compute points on one surface and fill an array of points
//           FILL AN ARRAY OF POINTS
//=======================================================================
void IntPolyh_MaillageAffinage::FillArrayOfPnt
  (const Standard_Integer SurfID,
   const Standard_Boolean isShiftFwd)
{
  Standard_Integer NbSamplesU, NbSamplesV, i, aNbSamplesU1, aNbSamplesV1; 
  Standard_Real u0, u1, v0, v1, aU, aV, dU, dV;
  const Handle(Adaptor3d_HSurface)& MaSurface=(SurfID==1)? MaSurface1 : MaSurface2;
  NbSamplesU=(SurfID==1)? NbSamplesU1:NbSamplesU2;
  NbSamplesV=(SurfID==1)? NbSamplesV1:NbSamplesV2;

  u0 = (MaSurface)->FirstUParameter();  
  u1 = (MaSurface)->LastUParameter();
  v0 = (MaSurface)->FirstVParameter();  
  v1 = (MaSurface)->LastVParameter();  

  if(myEnlargeZone) {
    EnlargeZone(MaSurface, u0, u1, v0, v1);
  }
  //
  TColStd_Array1OfReal aUpars(1, NbSamplesU);
  TColStd_Array1OfReal aVpars(1, NbSamplesV);
  //
  aNbSamplesU1=NbSamplesU-1;
  aNbSamplesV1=NbSamplesV-1;
  //
  dU=(u1-u0)/Standard_Real(aNbSamplesU1);
  dV=(v1-v0)/Standard_Real(aNbSamplesV1);
  //
  for (i=0; i<NbSamplesU; ++i) {
    aU=u0+i*dU;
    if (i==aNbSamplesU1) {
      aU=u1;
    }
    aUpars.SetValue(i+1, aU);
  }
  //
  for (i=0; i<NbSamplesV; ++i) {
    aV=v0+i*dV;
    if (i==aNbSamplesV1) {
      aV=v1;
    }
    aVpars.SetValue(i+1, aV);
  }
  //
  FillArrayOfPnt(SurfID, isShiftFwd, aUpars, aVpars);  
}
//=======================================================================
//function : FillArrayOfPnt
//purpose  : Compute points on one surface and fill an array of points
//=======================================================================
void IntPolyh_MaillageAffinage::FillArrayOfPnt
  (const Standard_Integer SurfID, 
   const TColStd_Array1OfReal& Upars,
   const TColStd_Array1OfReal& Vpars)
{
  Standard_Boolean bDegI, bDeg;
  Standard_Integer aNbU, aNbV, iCnt, i, j;
  Standard_Integer aID1, aID2, aJD1, aJD2;
  Standard_Real aTol, aU, aV, aX, aY, aZ;
  gp_Pnt aP;
  //
  aNbU=(SurfID==1)? NbSamplesU1 : NbSamplesU2;
  aNbV=(SurfID==1)? NbSamplesV1 : NbSamplesV2;
  Bnd_Box& aBox = (SurfID==1) ? MyBox1 : MyBox2;
  Handle(Adaptor3d_HSurface)& aS=(SurfID==1)? MaSurface1:MaSurface2;
  IntPolyh_ArrayOfPoints &TPoints=(SurfID==1)? TPoints1:TPoints2;
  //
  aJD1=0;
  aJD2=0;
  aID1=0;
  aID2=0;
  DegeneratedIndex(Vpars, aNbV, aS, 1, aJD1, aJD2);
  if (!(aJD1 || aJD2)) {
    DegeneratedIndex(Upars, aNbU, aS, 2, aID1, aID2);
  }
  //
  TPoints.Init(aNbU*aNbV);
  iCnt=0;
  for(i=1; i<=aNbU; ++i){
    bDegI=(aID1==i || aID2==i);
    aU=Upars(i);
    for(j=1; j<=aNbV; ++j){
      aV=Vpars(j);
      aP=aS->Value(aU, aV);
      aP.Coord(aX, aY, aZ);
      IntPolyh_Point& aIP=TPoints[iCnt];
      aIP.Set(aX, aY, aZ, aU, aV);
      //
      bDeg=bDegI || (aJD1==j || aJD2==j);
      if (bDeg) {
        aIP.SetDegenerated(bDeg);
      }
      ++iCnt;
      aBox.Add(aP);
    }
  }
  //
  TPoints.SetNbItems(iCnt);
  //
  IntCurveSurface_ThePolyhedronOfHInter polyhedron(aS, Upars, Vpars);
  //
  aTol=polyhedron.DeflectionOverEstimation();
  aTol*=1.2;

  Standard_Real a1,a2,a3,b1,b2,b3;
  //
  aBox.Get(a1,a2,a3,b1,b2,b3);
  aBox.Update(a1-aTol,a2-aTol,a3-aTol,b1+aTol,b2+aTol,b3+aTol);
  aBox.Enlarge(MyTolerance);
}

//=======================================================================
//function : FillArrayOfPnt
//purpose  : Compute points on one surface and fill an array of points
//           REMPLISSAGE DU TABLEAU DE POINTS
//=======================================================================
void IntPolyh_MaillageAffinage::FillArrayOfPnt
  (const Standard_Integer SurfID,
   const Standard_Boolean isShiftFwd,
   const TColStd_Array1OfReal& Upars,
   const TColStd_Array1OfReal& Vpars)
{
  Standard_Boolean bDegI, bDeg;
  Standard_Integer aNbU, aNbV, iCnt, i, j;
  Standard_Integer aID1, aID2, aJD1, aJD2;
  Standard_Real Tol, resol, aU, aV, aMag;
  Standard_Real aX, aY, aZ;
  gp_Pnt aP;
  gp_Vec aDU, aDV, aNorm;
  //
  aNbU=(SurfID==1)? NbSamplesU1:NbSamplesU2;
  aNbV=(SurfID==1)? NbSamplesV1:NbSamplesV2; 
  Bnd_Box& aBox = (SurfID==1) ? MyBox1 : MyBox2;
  Handle(Adaptor3d_HSurface) aS=(SurfID==1)? MaSurface1:MaSurface2;
  IntPolyh_ArrayOfPoints &TPoints=(SurfID==1)? TPoints1:TPoints2;
  //
  resol = gp::Resolution();
  //
  IntCurveSurface_ThePolyhedronOfHInter polyhedron(aS, Upars, Vpars);
  Tol=polyhedron.DeflectionOverEstimation();
  aJD1=0;
  aJD2=0;
  aID1=0;
  aID2=0;
  DegeneratedIndex(Vpars, aNbV, aS, 1, aJD1, aJD2);
  if (!(aJD1 || aJD2)) {
    DegeneratedIndex(Upars, aNbU, aS, 2, aID1, aID2);
  }
  //
  TPoints.Init(aNbU*aNbV);
  iCnt=0;
  for(i=1; i<=aNbU; ++i){
    bDegI=(aID1==i || aID2==i);
    aU = Upars(i);
    for(j=1; j<=aNbV; ++j){
      aV = Vpars(j);
      aS->D1(aU, aV, aP, aDU, aDV);
      
      aNorm = aDU.Crossed(aDV);
      aMag = aNorm.Magnitude();
      if (aMag > resol) {
        aNorm /= aMag;
        aNorm.Multiply(Tol*1.5);
        //
        if (isShiftFwd) {
          aP.Translate(aNorm);
        }
        else{
          aP.Translate(aNorm.Reversed());
        }
      }
      
      IntPolyh_Point& aIP=TPoints[iCnt];
      aP.Coord(aX, aY, aZ);
      aIP.Set(aX, aY, aZ, aU, aV);
      //
      bDeg=bDegI || (aJD1==j || aJD2==j);
      if (bDeg) {
        aIP.SetDegenerated(bDeg);
      }
      ++iCnt;
      aBox.Add(aP);
    }
  }
  //
  TPoints.SetNbItems(iCnt);
  //
  Tol*=1.2;
  //
  Standard_Real a1,a2,a3,b1,b2,b3;
  //
  aBox.Get(a1,a2,a3,b1,b2,b3);
  aBox.Update(a1-Tol,a2-Tol,a3-Tol,b1+Tol,b2+Tol,b3+Tol);
  aBox.Enlarge(MyTolerance);
}
//=======================================================================
//function : CommonBox
//purpose  : Compute the common box  witch is the intersection
//           of the two bounding boxes,  and mark the points of
//           the two surfaces that are inside.
//           REJECTION BOUNDING BOXES
//           DETERMINATION OF THE COMMON BOX
//=======================================================================
void IntPolyh_MaillageAffinage::CommonBox (const Bnd_Box &,
                                           const Bnd_Box &,
                                           Standard_Real &XMin,
                                           Standard_Real &YMin,
                                           Standard_Real &ZMin,
                                           Standard_Real &XMax,
                                           Standard_Real &YMax,
                                           Standard_Real &ZMax) 
{
  Standard_Real x10,y10,z10,x11,y11,z11;
  Standard_Real x20,y20,z20,x21,y21,z21;

  MyBox1.Get(x10,y10,z10,x11,y11,z11);
  MyBox2.Get(x20,y20,z20,x21,y21,z21);
  XMin = 0.;
  YMin = 0.;
  ZMin = 0.;
  XMax = 0.;
  YMax = 0.;
  ZMax = 0.;

  if((x10>x21)||(x20>x11)||(y10>y21)||(y20>y11)||(z10>z21)||(z20>z11)) {
  }
  else {
    if(x11<=x21) XMax=x11; else { if(x21<=x11) XMax=x21;}
    if(x20<=x10) XMin=x10; else { if(x10<=x20) XMin=x20;}
    if(y11<=y21) YMax=y11; else { if(y21<=y11) YMax=y21;}
    if(y20<=y10) YMin=y10; else { if(y10<=y20) YMin=y20;}
    if(z11<=z21) ZMax=z11; else { if(z21<=z11) ZMax=z21;}
    if(z20<=z10) ZMin=z10; else { if(z10<=z20) ZMin=z20;}

    if(((XMin==XMax)&&(!(YMin==YMax)&&!(ZMin==ZMax)))
        ||((YMin==YMax)&&(!(XMin==XMax)&&!(ZMin==ZMax)))//ou exclusif ??
        ||((ZMin==ZMax)&&(!(XMin==XMax)&&!(YMin==YMax)))) {
    }
  }
  //
  Standard_Real X,Y,Z;
  X=XMax-XMin; 
  Y=YMax-YMin; 
  Z=ZMax-ZMin; 
  //extension of the box
  if( (X==0)&&(Y!=0) ) X=Y*0.1;
  else if( (X==0)&&(Z!=0) ) X=Z*0.1;
  else X*=0.1;

  if( (Y==0)&&(X!=0) ) Y=X*0.1;
  else if( (Y==0)&&(Z!=0) ) Y=Z*0.1;
  else Y*=0.1;

  if( (Z==0)&&(X!=0) ) Z=X*0.1;
  else if( (Z==0)&&(Y!=0) ) Z=Y*0.1;
  else Z*=0.1;


  if( (X==0)&&(Y==0)&&(Z==0) ) {


  }
  XMin-=X; XMax+=X;
  YMin-=Y; YMax+=Y;
  ZMin-=Z; ZMax+=Z;

  //Marking of points included in the common
  const Standard_Integer FinTP1 = TPoints1.NbItems();
//  for(Standard_Integer i=0; i<FinTP1; i++) {
  Standard_Integer i ;
  for( i=0; i<FinTP1; i++) {
    IntPolyh_Point & Pt1 = TPoints1[i];
    Standard_Integer r;
    if(Pt1.X()<XMin) { 
      r=1; 
    }   
    else { 
      if(Pt1.X()>XMax) {
        r=2;
      } else {
        r=0; 
      } 
    }
    if(Pt1.Y()<YMin) { 
      r|=4; 
    }  
    else { 
      if(Pt1.Y()>YMax) {
        r|=8; 
      }
    }
    if(Pt1.Z()<ZMin) {
      r|=16; 
    } else {
      if(Pt1.Z()>ZMax) {
        r|=32;
      }
    }
    Pt1.SetPartOfCommon(r);
  }

  const Standard_Integer FinTP2 = TPoints2.NbItems();
  for(Standard_Integer ii=0; ii<FinTP2; ii++) {
    IntPolyh_Point & Pt2 = TPoints2[ii];
    Standard_Integer rr;
    if(Pt2.X()<XMin) { 
      rr=1;
    }   
    else { 
      if(Pt2.X()>XMax) {
        rr=2;
      } else {
        rr=0;
      } 
    }
    if(Pt2.Y()<YMin) {
      rr|=4; 
    }  
    else { 
      if(Pt2.Y()>YMax) {
        rr|=8; 
      }
    }
    if(Pt2.Z()<ZMin) {
      rr|=16;
    }
    else {
      if(Pt2.Z()>ZMax) {
        rr|=32;
      }
    }
    Pt2.SetPartOfCommon(rr);
  }
}
//=======================================================================
//function : FillArrayOfEdges
//purpose  : Compute edges from the array of points
//           FILL THE ARRAY OF EDGES
//=======================================================================
void IntPolyh_MaillageAffinage::FillArrayOfEdges
  (const Standard_Integer SurfID)
{

  IntPolyh_ArrayOfEdges &TEdges=(SurfID==1)? TEdges1:TEdges2;
  IntPolyh_ArrayOfTriangles &TTriangles=(SurfID==1)? TTriangles1:TTriangles2;
  Standard_Integer NbSamplesU=(SurfID==1)? NbSamplesU1:NbSamplesU2;
  Standard_Integer NbSamplesV=(SurfID==1)? NbSamplesV1:NbSamplesV2;

  //NbEdges = 3 + 3*(NbSamplesV-2) + 3*(NbSamplesU-2) + 
  //        + 3*(NbSamplesU-2)*(NbSamplesV-2) + (NbSamplesV-1) + (NbSamplesU-1);
  //NbSamplesU and NbSamples cannot be less than 2, so
  Standard_Integer NbEdges = 3*NbSamplesU*NbSamplesV - 2*(NbSamplesU+NbSamplesV) + 1;
  TEdges.Init(NbEdges);

  Standard_Integer CpteurTabEdges=0;

  //maillage u0 v0
  TEdges[CpteurTabEdges].SetFirstPoint(0);                // U V
  TEdges[CpteurTabEdges].SetSecondPoint(1);             // U V+1
  TEdges[CpteurTabEdges].SetSecondTriangle(0);
  TTriangles[0].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
  CpteurTabEdges++;

  TEdges[CpteurTabEdges].SetFirstPoint(0);                // U V
  TEdges[CpteurTabEdges].SetSecondPoint(NbSamplesV);    // U+1 V
  TEdges[CpteurTabEdges].SetFirstTriangle(1);
  TTriangles[1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
  CpteurTabEdges++;
  
  TEdges[CpteurTabEdges].SetFirstPoint(0);                // U V
  TEdges[CpteurTabEdges].SetSecondPoint(NbSamplesV+1);  // U+1 V+1
  TEdges[CpteurTabEdges].SetFirstTriangle(0);
  TTriangles[0].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
  TEdges[CpteurTabEdges].SetSecondTriangle(1);
  TTriangles[1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
  CpteurTabEdges++;
  
  //maillage surU=u0
  Standard_Integer PntInit=1;
  Standard_Integer BoucleMeshV;
  for(BoucleMeshV=1; BoucleMeshV<NbSamplesV-1;BoucleMeshV++){
    TEdges[CpteurTabEdges].SetFirstPoint(PntInit);                // U V
    TEdges[CpteurTabEdges].SetSecondPoint(PntInit+1);             // U V+1
    //    TEdges[CpteurTabEdges].SetFirstTriangle(-1);
    TEdges[CpteurTabEdges].SetSecondTriangle(BoucleMeshV*2);
    TTriangles[BoucleMeshV*2].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    CpteurTabEdges++;
    
    TEdges[CpteurTabEdges].SetFirstPoint(PntInit);                // U V
    TEdges[CpteurTabEdges].SetSecondPoint(PntInit+NbSamplesV+1);    // U+1 V+1
    TEdges[CpteurTabEdges].SetFirstTriangle(BoucleMeshV*2);
    TTriangles[BoucleMeshV*2].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    TEdges[CpteurTabEdges].SetSecondTriangle(BoucleMeshV*2+1);
    TTriangles[BoucleMeshV*2+1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    CpteurTabEdges++;
    
    TEdges[CpteurTabEdges].SetFirstPoint(PntInit);                // U V
    TEdges[CpteurTabEdges].SetSecondPoint(PntInit+NbSamplesV);  // U+1 V
    TEdges[CpteurTabEdges].SetFirstTriangle(BoucleMeshV*2+1);
    TTriangles[BoucleMeshV*2+1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    TEdges[CpteurTabEdges].SetSecondTriangle(BoucleMeshV*2-2);
    TTriangles[BoucleMeshV*2-2].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    CpteurTabEdges++;
    PntInit++;
  }  
        
  //maillage sur V=v0
  PntInit=NbSamplesV;
  for(BoucleMeshV=1; BoucleMeshV<NbSamplesU-1;BoucleMeshV++){      
    TEdges[CpteurTabEdges].SetFirstPoint(PntInit);    // U V
    TEdges[CpteurTabEdges].SetSecondPoint(PntInit+1); // U V+1
    TEdges[CpteurTabEdges].SetFirstTriangle((BoucleMeshV-1)*(NbSamplesV-1)*2+1);
    TTriangles[(BoucleMeshV-1)*(NbSamplesV-1)*2+1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    TEdges[CpteurTabEdges].SetSecondTriangle(BoucleMeshV*(NbSamplesV-1)*2);
    TTriangles[BoucleMeshV*(NbSamplesV-1)*2].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    CpteurTabEdges++;
    
    TEdges[CpteurTabEdges].SetFirstPoint(PntInit); // U V
    TEdges[CpteurTabEdges].SetSecondPoint(PntInit+NbSamplesV+1);     // U+1 V+1
    TEdges[CpteurTabEdges].SetFirstTriangle(BoucleMeshV*(NbSamplesV-1)*2);
    TTriangles[BoucleMeshV*(NbSamplesV-1)*2].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    TEdges[CpteurTabEdges].SetSecondTriangle(BoucleMeshV*(NbSamplesV-1)*2+1);
    TTriangles[BoucleMeshV*(NbSamplesV-1)*2+1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    CpteurTabEdges++;
    
    TEdges[CpteurTabEdges].SetFirstPoint(PntInit);  // U V
    TEdges[CpteurTabEdges].SetSecondPoint(PntInit+NbSamplesV);    // U+1 V
    TEdges[CpteurTabEdges].SetFirstTriangle(BoucleMeshV*(NbSamplesV-1)*2+1);
    TTriangles[BoucleMeshV*(NbSamplesV-1)*2+1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    CpteurTabEdges++;
    PntInit+=NbSamplesV;
  }
      
  PntInit=NbSamplesV+1;
  //To provide recursion I associate a point with three edges  
  for(Standard_Integer BoucleMeshU=1; BoucleMeshU<NbSamplesU-1; BoucleMeshU++){
    for(BoucleMeshV=1; BoucleMeshV<NbSamplesV-1;BoucleMeshV++){
      TEdges[CpteurTabEdges].SetFirstPoint(PntInit);                // U V
      TEdges[CpteurTabEdges].SetSecondPoint(PntInit+1);             // U V+1
      TEdges[CpteurTabEdges].SetFirstTriangle((NbSamplesV-1)*2*(BoucleMeshU-1)+BoucleMeshV*2+1);
      TTriangles[(NbSamplesV-1)*2*(BoucleMeshU-1)+BoucleMeshV*2+1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
      TEdges[CpteurTabEdges].SetSecondTriangle((NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2);
      TTriangles[(NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
      CpteurTabEdges++;
      
      TEdges[CpteurTabEdges].SetFirstPoint(PntInit);                // U V
      TEdges[CpteurTabEdges].SetSecondPoint(PntInit+NbSamplesV+1);    // U+1 V+1 
      TEdges[CpteurTabEdges].SetFirstTriangle((NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2);
      TTriangles[(NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
      TEdges[CpteurTabEdges].SetSecondTriangle((NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2+1);
      TTriangles[(NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2+1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
      CpteurTabEdges++;
      
      TEdges[CpteurTabEdges].SetFirstPoint(PntInit);                // U V
      TEdges[CpteurTabEdges].SetSecondPoint(PntInit+NbSamplesV);  // U+1 V
      TEdges[CpteurTabEdges].SetFirstTriangle((NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2+1);
      TTriangles[(NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2+1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
      TEdges[CpteurTabEdges].SetSecondTriangle((NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2-2);
      TTriangles[(NbSamplesV-1)*2*BoucleMeshU+BoucleMeshV*2-2].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
      CpteurTabEdges++;            
      PntInit++;//Pass to the next point
    }
    PntInit++;//Pass the last point of the column
    PntInit++;//Pass the first point of the next column
  }
        
  //close mesh on U=u1
  PntInit=(NbSamplesU-1)*NbSamplesV; //point U=u1 V=0
  for(BoucleMeshV=0; BoucleMeshV<NbSamplesV-1; BoucleMeshV++){
    TEdges[CpteurTabEdges].SetFirstPoint(PntInit);           //U=u1 V
    TEdges[CpteurTabEdges].SetSecondPoint(PntInit+1);        //U=u1 V+1
    TEdges[CpteurTabEdges].SetFirstTriangle((NbSamplesU-2)*(NbSamplesV-1)*2+BoucleMeshV*2+1);
    TTriangles[(NbSamplesU-2)*(NbSamplesV-1)*2+BoucleMeshV*2+1].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    CpteurTabEdges++;
    PntInit++;
  }
  
  //close mesh on V=v1
  for(BoucleMeshV=0; BoucleMeshV<NbSamplesU-1;BoucleMeshV++){      
    TEdges[CpteurTabEdges].SetFirstPoint(NbSamplesV-1+BoucleMeshV*NbSamplesV);       // U V=v1
    TEdges[CpteurTabEdges].SetSecondPoint(NbSamplesV-1+(BoucleMeshV+1)*NbSamplesV);  //U+1 V=v1
    TEdges[CpteurTabEdges].SetSecondTriangle(BoucleMeshV*2*(NbSamplesV-1)+(NbSamplesV-2)*2);
    TTriangles[BoucleMeshV*2*(NbSamplesV-1)+(NbSamplesV-2)*2].SetEdgeAndOrientation(TEdges[CpteurTabEdges], CpteurTabEdges);
    CpteurTabEdges++;
  }
  TEdges.SetNbItems(CpteurTabEdges);
}

//=======================================================================
//function : FillArrayOfTriangles
//purpose  : Compute triangles from the array of points, and --
//           mark the triangles  that use marked points by the
//           CommonBox function.
//           FILL THE ARRAY OF TRIANGLES
//=======================================================================
void IntPolyh_MaillageAffinage::FillArrayOfTriangles
  (const Standard_Integer SurfID)
{
  Standard_Integer CpteurTabTriangles=0;
  Standard_Integer PntInit=0;

  IntPolyh_ArrayOfPoints &TPoints=(SurfID==1)? TPoints1:TPoints2;
  IntPolyh_ArrayOfTriangles &TTriangles=(SurfID==1)? TTriangles1:TTriangles2;
  Standard_Integer NbSamplesU=(SurfID==1)? NbSamplesU1:NbSamplesU2;
  Standard_Integer NbSamplesV=(SurfID==1)? NbSamplesV1:NbSamplesV2;

  TTriangles.Init(2*(NbSamplesU-1)*(NbSamplesV-1));
  //To provide recursion, I associate a point with two triangles  
  for(Standard_Integer BoucleMeshU=0; BoucleMeshU<NbSamplesU-1; BoucleMeshU++){
    for(Standard_Integer BoucleMeshV=0; BoucleMeshV<NbSamplesV-1;BoucleMeshV++){
      
      // FIRST TRIANGLE
      TTriangles[CpteurTabTriangles].SetFirstPoint(PntInit);               // U V
      TTriangles[CpteurTabTriangles].SetSecondPoint(PntInit+1);            // U V+1
      TTriangles[CpteurTabTriangles].SetThirdPoint(PntInit+NbSamplesV+1); // U+1 V+1

      // IF ITS EDGE CONTACTS WITH THE COMMON BOX IP REMAINS = A 1
      if( ( (TPoints[PntInit].PartOfCommon()) & (TPoints[PntInit+1].PartOfCommon()) )
         &&( (TPoints[PntInit+1].PartOfCommon()) & (TPoints[PntInit+NbSamplesV+1].PartOfCommon()))
         &&( (TPoints[PntInit+NbSamplesV+1].PartOfCommon()) & (TPoints[PntInit].PartOfCommon())) ) 
        //IF NOT IP=0
        TTriangles[CpteurTabTriangles].SetIntersectionPossible(Standard_False);

      CpteurTabTriangles++;

      //SECOND TRIANGLE
      TTriangles[CpteurTabTriangles].SetFirstPoint(PntInit);               // U V
      TTriangles[CpteurTabTriangles].SetSecondPoint(PntInit+NbSamplesV+1); // U+1 V+1
      TTriangles[CpteurTabTriangles].SetThirdPoint(PntInit+NbSamplesV);    // U+1 V 


      if( ( (TPoints[PntInit].PartOfCommon()) & (TPoints[PntInit+NbSamplesV+1].PartOfCommon()) )
         &&( (TPoints[PntInit+NbSamplesV+1].PartOfCommon()) & (TPoints[PntInit+NbSamplesV].PartOfCommon()))
         &&( (TPoints[PntInit+NbSamplesV].PartOfCommon()) & (TPoints[PntInit].PartOfCommon())) ) 
        TTriangles[CpteurTabTriangles].SetIntersectionPossible(Standard_False);


      CpteurTabTriangles++;

      PntInit++;//Pass to the next point
    }
    PntInit++;//Pass the last point of the column
  }
  TTriangles.SetNbItems(CpteurTabTriangles);
  const Standard_Integer FinTT = TTriangles.NbItems();
  if (FinTT==0) {
  }
}
//=======================================================================
//function : CommonPartRefinement
//purpose  : Refine systematicaly all marked triangles of both surfaces
//           REFINING OF THE COMMON
//=======================================================================
void IntPolyh_MaillageAffinage::CommonPartRefinement() 
{
  Standard_Integer FinInit1 = TTriangles1.NbItems();
  for(Standard_Integer i=0; i<FinInit1; i++) {
    if(TTriangles1[i].IsIntersectionPossible())
      TTriangles1[i].MiddleRefinement(i,MaSurface1,TPoints1,TTriangles1,TEdges1);
  }

  Standard_Integer FinInit2=TTriangles2.NbItems();
  for(Standard_Integer ii=0; ii<FinInit2; ii++) {
    if(TTriangles2[ii].IsIntersectionPossible())
      TTriangles2[ii].MiddleRefinement(ii,MaSurface2,TPoints2,TTriangles2,TEdges2); 
  }

}
//=======================================================================
//function : LocalSurfaceRefinement
//purpose  : Refine systematicaly all marked triangles of ONE surface
//=======================================================================
void IntPolyh_MaillageAffinage::LocalSurfaceRefinement(const Standard_Integer SurfID) {
//refine locally, but systematically the chosen surface
  if (SurfID==1) {
    const Standard_Integer FinInit1 = TTriangles1.NbItems();
    for(Standard_Integer i=0; i<FinInit1; i++) {
      if(TTriangles1[i].IsIntersectionPossible())
        TTriangles1[i].MiddleRefinement(i,MaSurface1,TPoints1,TTriangles1,TEdges1);
    }
  }
  //
  if (SurfID==2) {
    const Standard_Integer FinInit2 = TTriangles2.NbItems();
    for(Standard_Integer ii=0; ii<FinInit2; ii++) {
      if(TTriangles2[ii].IsIntersectionPossible()) 
        TTriangles2[ii].MiddleRefinement(ii,MaSurface2,TPoints2,TTriangles2,TEdges2); 
    }
  }
}
//=======================================================================
//function : ComputeDeflections
//purpose  : Compute deflection  for   all  triangles  of  one
//           surface,and sort min and max of deflections
//           REFINING PART
//           Calculation of the deflection of all triangles
//           --> deflection max
//           --> deflection min
//=======================================================================
void IntPolyh_MaillageAffinage::ComputeDeflections
  (const Standard_Integer SurfID)
{
  Handle(Adaptor3d_HSurface) aSurface=(SurfID==1)? MaSurface1:MaSurface2;
  IntPolyh_ArrayOfPoints &TPoints=(SurfID==1)? TPoints1:TPoints2;
  IntPolyh_ArrayOfTriangles &TTriangles=(SurfID==1)? TTriangles1:TTriangles2;
  Standard_Real &FlecheMin=(SurfID==1)? FlecheMin1:FlecheMin2;
  Standard_Real &FlecheMoy=(SurfID==1)? FlecheMoy1:FlecheMoy2;
  Standard_Real &FlecheMax=(SurfID==1)? FlecheMax1:FlecheMax2;

  FlecheMax=-RealLast();
  FlecheMin=RealLast();
  FlecheMoy=0.0;
  const Standard_Integer FinTT = TTriangles.NbItems();
  
  for(Standard_Integer i = 0; i < FinTT; i++) {
    IntPolyh_Triangle& aTriangle = TTriangles[i];
    Standard_Real Fleche = aTriangle.ComputeDeflection(aSurface, TPoints);
    if (Fleche > FlecheMax)
      FlecheMax = Fleche;
    if (Fleche < FlecheMin)
      FlecheMin = Fleche;
  }
}

//=======================================================================
//function : TrianglesDeflectionsRefinement
//purpose  : Refinement of the triangles depending on the deflection
//=======================================================================
static
  void TrianglesDeflectionsRefinement(const Handle(Adaptor3d_HSurface)& theS1,
                                      IntPolyh_ArrayOfTriangles& theTriangles1,
                                      IntPolyh_ArrayOfEdges& theEdges1,
                                      IntPolyh_ArrayOfPoints& thePoints1,
                                      const Standard_Real theFlecheCritique1,
                                      const Handle(Adaptor3d_HSurface)& theS2,
                                      IntPolyh_ArrayOfTriangles& theTriangles2,
                                      IntPolyh_ArrayOfEdges& theEdges2,
                                      IntPolyh_ArrayOfPoints& thePoints2,
                                      const Standard_Real theFlecheCritique2)
{
  // Find the intersecting triangles
  IntPolyh_IndexedDataMapOfIntegerArrayOfInteger aDMILI;
  GetInterferingTriangles(theTriangles1, thePoints1, theTriangles2, thePoints2, aDMILI);
  //
  // Interfering triangles of second surface
  TColStd_MapOfInteger aMIntS2;
  // Since the number of the triangles may change during refinement,
  // save the number of triangles before refinement
  Standard_Integer FinTT1 = theTriangles1.NbItems();
  Standard_Integer FinTT2 = theTriangles2.NbItems();
  //
  // Analyze interfering triangles
  for (Standard_Integer i_S1 = 0; i_S1 < FinTT1; i_S1++) {
    IntPolyh_Triangle& aTriangle1 = theTriangles1[i_S1];
    if (!aTriangle1.IsIntersectionPossible()) {
      continue;
    }
    //
    const IntPolyh_ArrayOfInteger *pLI = aDMILI.Seek(i_S1);
    if (!pLI || !pLI->Length()) {
      // Mark non-interfering triangles of S1 to avoid their repeated usage
      aTriangle1.SetIntersectionPossible(Standard_False);
      continue;
    }
    //
    if (aTriangle1.Deflection() > theFlecheCritique1) {
      aTriangle1.MiddleRefinement(i_S1, theS1, thePoints1, theTriangles1, theEdges1);
    }
    //
    IntPolyh_ArrayOfInteger::Iterator Iter(*pLI);
    for (; Iter.More(); Iter.Next()) {
      Standard_Integer i_S2 = Iter.Value();
      if (aMIntS2.Add(i_S2)) {
        IntPolyh_Triangle & aTriangle2 = theTriangles2[i_S2];
        if (aTriangle2.Deflection() > theFlecheCritique2) {
          // Refinement of the larger triangles
          aTriangle2.MiddleRefinement(i_S2, theS2, thePoints2, theTriangles2, theEdges2);
        }
      }
    }
  }
  //
  // Mark non-interfering triangles of S2 to avoid their repeated usage
  if (aMIntS2.Extent() < FinTT2) {
    for (Standard_Integer i_S2 = 0; i_S2 < FinTT2; i_S2++) {
      if (!aMIntS2.Contains(i_S2)) {
        theTriangles2[i_S2].SetIntersectionPossible(Standard_False);
      }
    }
  }
}
//=======================================================================
//function : LargeTrianglesDeflectionsRefinement
//purpose  : Refinement of the large triangles in case one surface is
//           much smaller then the other.
//=======================================================================
static
  void LargeTrianglesDeflectionsRefinement(const Handle(Adaptor3d_HSurface)& theS,
                                           IntPolyh_ArrayOfTriangles& theTriangles,
                                           IntPolyh_ArrayOfEdges& theEdges,
                                           IntPolyh_ArrayOfPoints& thePoints,
                                           const Bnd_Box& theOppositeBox)
{
  // Find all triangles of the bigger surface with bounding boxes
  // overlapping the bounding box the other surface
  TColStd_ListOfInteger aLIT;
  Standard_Integer i, aNbT = theTriangles.NbItems();
  for (i = 0; i < aNbT; ++i) {
    IntPolyh_Triangle& aTriangle = theTriangles[i];
    if (!aTriangle.IsIntersectionPossible() || aTriangle.IsDegenerated()) {
      continue;
    }
    //
    const Bnd_Box& aBox = aTriangle.BoundingBox(thePoints);
    if (aBox.IsVoid()) {
      continue;
    }
    //
    if (aBox.IsOut(theOppositeBox)) {
      aTriangle.SetIntersectionPossible(Standard_False);
      continue;
    }
    //
    aLIT.Append(i);
  }
  //
  if (aLIT.IsEmpty()) {
    return;
  }
  //
  // One surface is very small relatively to the other.
  // The criterion of refining for large surface depends on the size of
  // the bounding box of the other - since the criterion should be minimized,
  // the smallest side of the bounding box is taken
  Standard_Real x0, y0, z0, x1, y1, z1;
  theOppositeBox.Get(x0, y0, z0, x1, y1, z1);
  Standard_Real dx = Abs(x1 - x0);
  Standard_Real dy = Abs(y1 - y0);
  Standard_Real diag = Abs(z1 - z0);
  Standard_Real dd = (dx > dy) ? dy : dx;
  if (diag > dd) diag = dd;

  // calculation of the criterion of refining
  Standard_Real CritereAffinage = 0.0;
  Standard_Real DiagPonderation = 0.5;
  CritereAffinage = diag*DiagPonderation;
  //
  // The deflection of the greater is compared to the size of the smaller
  TColStd_ListIteratorOfListOfInteger Iter(aLIT);
  for (; Iter.More(); Iter.Next()) {
    i = Iter.Value();
    IntPolyh_Triangle & aTriangle = theTriangles[i];
    if (aTriangle.Deflection() > CritereAffinage) {
      aTriangle.MultipleMiddleRefinement(CritereAffinage, theOppositeBox, i,
                                         theS, thePoints, theTriangles, theEdges);
    }
    else {
      aTriangle.MiddleRefinement(i, theS, thePoints, theTriangles, theEdges);
    }
  }
}
//=======================================================================
//function : TrianglesDeflectionsRefinementBSB
//purpose  : Refine both surfaces using UBTree as rejection.
//           The criterion used to refine a triangle are:
//           - The deflection;
//           - The  size of the - bounding boxes (one surface may be
//           very small compared to the other).
//=======================================================================
void IntPolyh_MaillageAffinage::TrianglesDeflectionsRefinementBSB()
{
  // To estimate a surface in general it can be interesting
  // to calculate all deflections
  ComputeDeflections(1);
  // Check deflection at output
  Standard_Real FlecheCritique1;
  if (FlecheMin1 > FlecheMax1) {
    return;
  }
  else {
    // fleche min + (flechemax-flechemin) * 80/100
    FlecheCritique1 = FlecheMin1*0.2 + FlecheMax1*0.8;
  }

  // Compute deflections for the second surface
  ComputeDeflections(2);

  //-- Check arrows at output
  Standard_Real FlecheCritique2;
  if (FlecheMin2 > FlecheMax2) {
    return;
  }
  else {
    //fleche min + (flechemax-flechemin) * 80/100
    FlecheCritique2 = FlecheMin2*0.2 + FlecheMax2*0.8;
  }

  // The greatest of two bounding boxes created in FillArrayOfPoints is found.
  // Then this value is weighted depending on the discretization 
  // (NbSamplesU and NbSamplesV)
  Standard_Real diag1, diag2;
  Standard_Real x0, y0, z0, x1, y1, z1;

  MyBox1.Get(x0, y0, z0, x1, y1, z1);
  x0 -= x1; y0 -= y1; z0 -= z1;
  diag1 = x0*x0 + y0*y0 + z0*z0;
  const Standard_Real NbSamplesUV1 = Standard_Real(NbSamplesU1) * Standard_Real(NbSamplesV1);
  diag1 /= NbSamplesUV1;

  MyBox2.Get(x0, y0, z0, x1, y1, z1);
  x0 -= x1; y0 -= y1; z0 -= z1;
  diag2 = x0*x0 + y0*y0 + z0*z0;
  const Standard_Real NbSamplesUV2 = Standard_Real(NbSamplesU2) * Standard_Real(NbSamplesV2);
  diag2 /= NbSamplesUV2;

  // The surface with the greatest bounding box is "discretized"
  if (diag1 < diag2) {
    // second is discretized
    if (FlecheCritique2 < diag1) {
      // The corresponding sizes are not too disproportional
      TrianglesDeflectionsRefinement(MaSurface1, TTriangles1, TEdges1, TPoints1, FlecheCritique1,
                                     MaSurface2, TTriangles2, TEdges2, TPoints2, FlecheCritique2);
    }
    else {
      // second surface is much larger then the first
      LargeTrianglesDeflectionsRefinement(MaSurface2, TTriangles2, TEdges2, TPoints2, MyBox1);
    }
  }
  else {
    // first is discretized
    if (FlecheCritique1 < diag2) {
      // The corresponding sizes are not too disproportional
      TrianglesDeflectionsRefinement(MaSurface2, TTriangles2, TEdges2, TPoints2, FlecheCritique2,
                                     MaSurface1, TTriangles1, TEdges1, TPoints1, FlecheCritique1);
    }
    else {
      // first surface is much larger then the second
      LargeTrianglesDeflectionsRefinement(MaSurface1, TTriangles1, TEdges1, TPoints1, MyBox2);
    }
  }
}
//=======================================================================
//function : maxSR
//purpose  : This function is used for the function project6
//=======================================================================
inline Standard_Real maxSR(const Standard_Real a,
                           const Standard_Real b,
                           const Standard_Real c)
{
  Standard_Real t = a;
  if (b > t) t = b;
  if (c > t) t = c;
  return t;
}
//=======================================================================
//function : minSR
//purpose  : This function is used for the function project6
//=======================================================================
inline Standard_Real minSR(const Standard_Real a,
                           const Standard_Real b,
                           const Standard_Real c)
{
  Standard_Real t = a;
  if (b < t) t = b;
  if (c < t) t = c;
  return t;
}
//=======================================================================
//function : project6
//purpose  : This function is used for the function TriContact
//=======================================================================
Standard_Integer project6(const IntPolyh_Point &ax, 
                          const IntPolyh_Point &p1,
                          const IntPolyh_Point &p2, 
                          const IntPolyh_Point &p3, 
                          const IntPolyh_Point &q1,
                          const IntPolyh_Point &q2, 
                          const IntPolyh_Point &q3)
{
  Standard_Real P1 = ax.Dot(p1);
  Standard_Real P2 = ax.Dot(p2);
  Standard_Real P3 = ax.Dot(p3);
  Standard_Real Q1 = ax.Dot(q1);
  Standard_Real Q2 = ax.Dot(q2);
  Standard_Real Q3 = ax.Dot(q3);
  
  Standard_Real mx1 = maxSR(P1, P2, P3);
  Standard_Real mn1 = minSR(P1, P2, P3);
  Standard_Real mx2 = maxSR(Q1, Q2, Q3);
  Standard_Real mn2 = minSR(Q1, Q2, Q3);
  
  if (mn1 > mx2) return 0;
  if (mn2 > mx1) return 0;
  return 1;
}
//=======================================================================
//function : TriContact
//purpose  : This fonction     Check if two triangles   are  in
//           contact or no,  return 1 if yes, return 0
//           if no.
//=======================================================================
Standard_Integer IntPolyh_MaillageAffinage::TriContact
  (const IntPolyh_Point &P1,
   const IntPolyh_Point &P2,
   const IntPolyh_Point &P3,
   const IntPolyh_Point &Q1,
   const IntPolyh_Point &Q2,
   const IntPolyh_Point &Q3,
   Standard_Real &Angle) const
{
  /**
     The first triangle is (p1,p2,p3).  The other is (q1,q2,q3).
     The edges are (e1,e2,e3) and (f1,f2,f3).
     The normals are n1 and m1
     Outwards are (g1,g2,g3) and (h1,h2,h3).*/
  
  if(maxSR(P1.X(),P2.X(),P3.X())<minSR(Q1.X(),Q2.X(),Q3.X())) return(0);
  if(maxSR(P1.Y(),P2.Y(),P3.Y())<minSR(Q1.Y(),Q2.Y(),Q3.Y())) return(0);
  if(maxSR(P1.Z(),P2.Z(),P3.Z())<minSR(Q1.Z(),Q2.Z(),Q3.Z())) return(0);

  if(minSR(P1.X(),P2.X(),P3.X())>maxSR(Q1.X(),Q2.X(),Q3.X())) return(0);
  if(minSR(P1.Y(),P2.Y(),P3.Y())>maxSR(Q1.Y(),Q2.Y(),Q3.Y())) return(0);
  if(minSR(P1.Z(),P2.Z(),P3.Z())>maxSR(Q1.Z(),Q2.Z(),Q3.Z())) return(0);
    
  IntPolyh_Point p1, p2, p3;
  IntPolyh_Point q1, q2, q3;
  IntPolyh_Point e1, e2, e3;
  IntPolyh_Point f1, f2, f3;
  IntPolyh_Point g1, g2, g3;
  IntPolyh_Point h1, h2, h3;
  IntPolyh_Point n1, m1;
  IntPolyh_Point z;

  IntPolyh_Point ef11, ef12, ef13;
  IntPolyh_Point ef21, ef22, ef23;
  IntPolyh_Point ef31, ef32, ef33;

  z.SetX(0.0);  z.SetY(0.0);  z.SetZ(0.0);


  p1.SetX(P1.X() - P1.X());  p1.SetY(P1.Y() - P1.Y());  p1.SetZ(P1.Z() - P1.Z());
  p2.SetX(P2.X() - P1.X());  p2.SetY(P2.Y() - P1.Y());  p2.SetZ(P2.Z() - P1.Z());
  p3.SetX(P3.X() - P1.X());  p3.SetY(P3.Y() - P1.Y());  p3.SetZ(P3.Z() - P1.Z());
  
  q1.SetX(Q1.X() - P1.X());  q1.SetY(Q1.Y() - P1.Y());  q1.SetZ(Q1.Z() - P1.Z());
  q2.SetX(Q2.X() - P1.X());  q2.SetY(Q2.Y() - P1.Y());  q2.SetZ(Q2.Z() - P1.Z());
  q3.SetX(Q3.X() - P1.X());  q3.SetY(Q3.Y() - P1.Y());  q3.SetZ(Q3.Z() - P1.Z());
  
  e1.SetX(p2.X() - p1.X());  e1.SetY(p2.Y() - p1.Y());  e1.SetZ(p2.Z() - p1.Z());
  e2.SetX(p3.X() - p2.X());  e2.SetY(p3.Y() - p2.Y());  e2.SetZ(p3.Z() - p2.Z());
  e3.SetX(p1.X() - p3.X());  e3.SetY(p1.Y() - p3.Y());  e3.SetZ(p1.Z() - p3.Z());

  f1.SetX(q2.X() - q1.X());  f1.SetY(q2.Y() - q1.Y());  f1.SetZ(q2.Z() - q1.Z());
  f2.SetX(q3.X() - q2.X());  f2.SetY(q3.Y() - q2.Y());  f2.SetZ(q3.Z() - q2.Z());
  f3.SetX(q1.X() - q3.X());  f3.SetY(q1.Y() - q3.Y());  f3.SetZ(q1.Z() - q3.Z());
  
  n1.Cross(e1, e2); //normal to the first triangle
  m1.Cross(f1, f2); //normal to the second triangle

  g1.Cross(e1, n1); 
  g2.Cross(e2, n1);
  g3.Cross(e3, n1);
  h1.Cross(f1, m1);
  h2.Cross(f2, m1);
  h3.Cross(f3, m1);

  ef11.Cross(e1, f1);
  ef12.Cross(e1, f2);
  ef13.Cross(e1, f3);
  ef21.Cross(e2, f1);
  ef22.Cross(e2, f2);
  ef23.Cross(e2, f3);
  ef31.Cross(e3, f1);
  ef32.Cross(e3, f2);
  ef33.Cross(e3, f3);
  
  // Now the testing is done

  if (!project6(n1, p1, p2, p3, q1, q2, q3)) return 0; //T2 is not higher or lower than T1
  if (!project6(m1, p1, p2, p3, q1, q2, q3)) return 0; //T1 is not higher of lower than T2
  
  if (!project6(ef11, p1, p2, p3, q1, q2, q3)) return 0; 
  if (!project6(ef12, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef13, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef21, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef22, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef23, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef31, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef32, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef33, p1, p2, p3, q1, q2, q3)) return 0;

  if (!project6(g1, p1, p2, p3, q1, q2, q3)) return 0; //T2 is outside of T1 in the plane of T1
  if (!project6(g2, p1, p2, p3, q1, q2, q3)) return 0; //T2 is outside of T1 in the plane of T1
  if (!project6(g3, p1, p2, p3, q1, q2, q3)) return 0; //T2 is outside of T1 in the plane of T1
  if (!project6(h1, p1, p2, p3, q1, q2, q3)) return 0; //T1 is outside of T2 in the plane of T2
  if (!project6(h2, p1, p2, p3, q1, q2, q3)) return 0; //T1 is outside of T2 in the plane of T2
  if (!project6(h3, p1, p2, p3, q1, q2, q3)) return 0; //T1 is outside of T2 in the plane of T2

  //Calculation of cosinus angle between two normals
  Standard_Real SqModn1=-1.0;
  Standard_Real SqModm1=-1.0;
  SqModn1=n1.SquareModulus();
  if (SqModn1>SquareMyConfusionPrecision){ 
    SqModm1=m1.SquareModulus();
  }
  if (SqModm1>SquareMyConfusionPrecision) {
    Angle=(n1.Dot(m1))/(sqrt(SqModn1)*sqrt(SqModm1));
  }
  return 1;
}
//=======================================================================
//function : TestNbPoints
//purpose  : This function is used by StartingPointsResearch() to control 
//           the  number of points found keep the result in conformity (1 or 2 points) 
//           void TestNbPoints(const Standard_Integer TriSurfID,
//=======================================================================
void TestNbPoints(const Standard_Integer ,
                  Standard_Integer &NbPoints,
                  Standard_Integer &NbPointsTotal,
                  const IntPolyh_StartPoint &Pt1,
                  const IntPolyh_StartPoint &Pt2,
                  IntPolyh_StartPoint &SP1,
                  IntPolyh_StartPoint &SP2)
{
  // already checked in TriangleEdgeContact
  //  if( (NbPoints==2)&&(Pt1.CheckSameSP(Pt2)) ) NbPoints=1;

  if(NbPoints>2) {

  }
  else {
    if ( (NbPoints==1)&&(NbPointsTotal==0) ) { 
      SP1=Pt1;
      NbPointsTotal=1;
    }
    else if ( (NbPoints==1)&&(NbPointsTotal==1) ) { 
      if(Pt1.CheckSameSP(SP1)!=1) {
        SP2=Pt1;
        NbPointsTotal=2;
      }
    }
    else if( (NbPoints==1)&&(NbPointsTotal==2) ) {
      if ( (SP1.CheckSameSP(Pt1))||(SP2.CheckSameSP(Pt1)) ) 
        NbPointsTotal=2;
      else NbPointsTotal=3;
    }
    else if( (NbPoints==2)&&(NbPointsTotal==0) ) {
      SP1=Pt1;
      SP2=Pt2;
      NbPointsTotal=2;
    }
    else if( (NbPoints==2)&&(NbPointsTotal==1) ) {//there is also Pt1 != Pt2 
      if(SP1.CheckSameSP(Pt1)) {
        SP2=Pt2;
        NbPointsTotal=2;
      }
      else if (SP1.CheckSameSP(Pt2)) {
        SP2=Pt1;
        NbPointsTotal=2;
      }
      else NbPointsTotal=3;///case SP1!=Pt1 && SP1!=Pt2!
    }
    else if( (NbPoints==2)&&(NbPointsTotal==2) ) {//there is also SP1!=SP2
      if( (SP1.CheckSameSP(Pt1))||(SP1.CheckSameSP(Pt2)) ) {
        if( (SP2.CheckSameSP(Pt1))||(SP2.CheckSameSP(Pt2)) )
          NbPointsTotal=2;
        else NbPointsTotal=3;
      }
      else NbPointsTotal=3;
    }
  }
}
//=======================================================================
//function : StartingPointsResearch
//purpose  : From  two  triangles compute intersection  points.
//           If I found   more  than two intersection  points
//           it  means that those triangle are coplanar
//=======================================================================
Standard_Integer IntPolyh_MaillageAffinage::StartingPointsResearch
  (const Standard_Integer T1,
   const Standard_Integer T2,
   IntPolyh_StartPoint &SP1, 
   IntPolyh_StartPoint &SP2) const 
{
  const IntPolyh_Triangle &Tri1=TTriangles1[T1];
  const IntPolyh_Triangle &Tri2=TTriangles2[T2];

  const IntPolyh_Point &P1=TPoints1[Tri1.FirstPoint()];
  const IntPolyh_Point &P2=TPoints1[Tri1.SecondPoint()];
  const IntPolyh_Point &P3=TPoints1[Tri1.ThirdPoint()];
  const IntPolyh_Point &Q1=TPoints2[Tri2.FirstPoint()];
  const IntPolyh_Point &Q2=TPoints2[Tri2.SecondPoint()];
  const IntPolyh_Point &Q3=TPoints2[Tri2.ThirdPoint()];



  /* The first triangle is (p1,p2,p3).  The other is (q1,q2,q3).
     The sides are (e1,e2,e3) and (f1,f2,f3).
     The normals are n1 and m1*/

  const IntPolyh_Point  e1=P2-P1;
  const IntPolyh_Point  e2=P3-P2;
  const IntPolyh_Point  e3=P1-P3;

  const IntPolyh_Point  f1=Q2-Q1;
  const IntPolyh_Point  f2=Q3-Q2;
  const IntPolyh_Point  f3=Q1-Q3;
  

  IntPolyh_Point nn1,mm1;
  nn1.Cross(e1, e2); //normal to the first triangle
  mm1.Cross(f1, f2); //normal to the second triangle

  Standard_Real nn1modulus, mm1modulus;
  nn1modulus=sqrt(nn1.SquareModulus());
  mm1modulus=sqrt(mm1.SquareModulus());

  //-------------------------------------------------
  ///calculation of intersections points between triangles
  //-------------------------------------------------
  Standard_Integer NbPoints=0;
  Standard_Integer NbPointsTotal=0;


    ///check T1 normal
    if(Abs(nn1modulus)<MyConfusionPrecision) {//10.0e-20){

    }
    else {
      const IntPolyh_Point n1=nn1.Divide(nn1modulus);
      ///T2 edges with T1
      if(NbPointsTotal<3) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(1,1,Tri1,Tri2,P1,P2,P3,e1,e2,e3,Q1,Q2,f1,n1,Pt1,Pt2);
        TestNbPoints(1,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
      
      if(NbPointsTotal<3) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(1,2,Tri1,Tri2,P1,P2,P3,e1,e2,e3,Q2,Q3,f2,n1,Pt1,Pt2);
        TestNbPoints(1,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
      
      if(NbPointsTotal<3) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(1,3,Tri1,Tri2,P1,P2,P3,e1,e2,e3,Q3,Q1,f3,n1,Pt1,Pt2);
        TestNbPoints(1,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
    }

    ///check T2 normal
    if(Abs(mm1modulus)<MyConfusionPrecision) {//10.0e-20){

    }
    else {
      const IntPolyh_Point m1=mm1.Divide(mm1modulus);
      ///T1 edges with T2
      if(NbPointsTotal<3) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(2,1,Tri1,Tri2,Q1,Q2,Q3,f1,f2,f3,P1,P2,e1,m1,Pt1,Pt2);
        TestNbPoints(2,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
      
      if(NbPointsTotal<3) { 
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(2,2,Tri1,Tri2,Q1,Q2,Q3,f1,f2,f3,P2,P3,e2,m1,Pt1,Pt2);
        TestNbPoints(2,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
      
      if(NbPointsTotal<3) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(2,3,Tri1,Tri2,Q1,Q2,Q3,f1,f2,f3,P3,P1,e3,m1,Pt1,Pt2);
        TestNbPoints(2,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
    }
  //  no need already checked in  TestNbPoints
  //  if( (NbPointsTotal==2)&&(SP1.CheckSameSP(SP2)) ) {
  //  NbPointsTotal=1;
  //SP1.SetCoupleValue(T1,T2);
  // }
  //  else 
  if(NbPointsTotal==2) {
    SP1.SetCoupleValue(T1,T2);
    SP2.SetCoupleValue(T1,T2);
  }
  else if(NbPointsTotal==1)
    SP1.SetCoupleValue(T1,T2);
  else if(NbPointsTotal==3)
    SP1.SetCoupleValue(T1,T2);

  return (NbPointsTotal);
}
//=======================================================================
//function : NextStartingPointsResearch
//purpose  : from  two triangles  and an intersection   point I
//           seach the other point (if it exist).
//           This function is used by StartPointChain
//=======================================================================
Standard_Integer IntPolyh_MaillageAffinage::NextStartingPointsResearch
  (const Standard_Integer T1,
   const Standard_Integer T2,
   const IntPolyh_StartPoint &SPInit,
   IntPolyh_StartPoint &SPNext) const 
{
  Standard_Integer NbPointsTotal=0;
  Standard_Integer EdgeInit1=SPInit.E1();
  Standard_Integer EdgeInit2=SPInit.E2();
  if( (T1<0)||(T2<0) ) NbPointsTotal=0;
  else {
    
    const IntPolyh_Triangle &Tri1=TTriangles1[T1];
    const IntPolyh_Triangle &Tri2=TTriangles2[T2];

    const IntPolyh_Point &P1=TPoints1[Tri1.FirstPoint()];
    const IntPolyh_Point &P2=TPoints1[Tri1.SecondPoint()];
    const IntPolyh_Point &P3=TPoints1[Tri1.ThirdPoint()];
    const IntPolyh_Point &Q1=TPoints2[Tri2.FirstPoint()];
    const IntPolyh_Point &Q2=TPoints2[Tri2.SecondPoint()];
    const IntPolyh_Point &Q3=TPoints2[Tri2.ThirdPoint()];
    
  /* The first triangle is (p1,p2,p3).  The other is (q1,q2,q3).
     The edges are (e1,e2,e3) and (f1,f2,f3).
     The normals are n1 and m1*/

    const IntPolyh_Point  e1=P2-P1;
    const IntPolyh_Point  e2=P3-P2;
    const IntPolyh_Point  e3=P1-P3;
    
    const IntPolyh_Point  f1=Q2-Q1;
    const IntPolyh_Point  f2=Q3-Q2;
    const IntPolyh_Point  f3=Q1-Q3;
    
    IntPolyh_Point nn1,mm1;
    nn1.Cross(e1, e2); //normal to the first triangle
    mm1.Cross(f1, f2); //normal to the second triangle

    Standard_Real nn1modulus, mm1modulus;
    nn1modulus=sqrt(nn1.SquareModulus());
    mm1modulus=sqrt(mm1.SquareModulus());
    
    //-------------------------------------------------
    ///calculation of intersections points between triangles
    //-------------------------------------------------

    Standard_Integer NbPoints=0;
    IntPolyh_StartPoint SP1,SP2;

    ///check T1 normal
    if(Abs(nn1modulus)<MyConfusionPrecision) {//10.0e-20){

    }
    else {
      const IntPolyh_Point n1=nn1.Divide(nn1modulus);
      ///T2 edges with T1
      if( (NbPointsTotal<3)&&(EdgeInit2!=Tri2.FirstEdge()) ) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(1,1,Tri1,Tri2,P1,P2,P3,e1,e2,e3,Q1,Q2,f1,n1,Pt1,Pt2);
        TestNbPoints(1,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
      
      if( (NbPointsTotal<3)&&(EdgeInit2!=Tri2.SecondEdge()) ) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(1,2,Tri1,Tri2,P1,P2,P3,e1,e2,e3,Q2,Q3,f2,n1,Pt1,Pt2);
        TestNbPoints(1,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
      
      if( (NbPointsTotal<3)&&(EdgeInit2!=Tri2.ThirdEdge()) ) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(1,3,Tri1,Tri2,P1,P2,P3,e1,e2,e3,Q3,Q1,f3,n1,Pt1,Pt2);
        TestNbPoints(1,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
    }
    ///check T2 normal
    if(Abs(mm1modulus)<MyConfusionPrecision) {//10.0e-20){

    }
    else {
      const IntPolyh_Point m1=mm1.Divide(mm1modulus);
      ///T1 edges with T2
      if( (NbPointsTotal<3)&&(EdgeInit1!=Tri1.FirstEdge()) ) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(2,1,Tri1,Tri2,Q1,Q2,Q3,f1,f2,f3,P1,P2,e1,m1,Pt1,Pt2);
        TestNbPoints(2,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
      
      if( (NbPointsTotal<3)&&(EdgeInit1!=Tri1.SecondEdge()) ) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(2,2,Tri1,Tri2,Q1,Q2,Q3,f1,f2,f3,P2,P3,e2,m1,Pt1,Pt2);
        TestNbPoints(2,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
      
      if( (NbPointsTotal<3)&&(EdgeInit1!=Tri1.ThirdEdge()) ) {
        IntPolyh_StartPoint Pt1,Pt2;
        NbPoints=TriangleEdgeContact(2,3,Tri1,Tri2,Q1,Q2,Q3,f1,f2,f3,P3,P1,e3,m1,Pt1,Pt2);
        TestNbPoints(2,NbPoints,NbPointsTotal,Pt1,Pt2,SP1,SP2);
      }
    }
      
    if (NbPointsTotal==1) {
      if(SP1.CheckSameSP(SPInit))
        NbPointsTotal=0;
      else {
        SPNext=SP1;
      }
    }
    else if( (NbPointsTotal==2)&&(SP1.CheckSameSP(SPInit)) ) {
      NbPointsTotal=1;//SP1 et SPInit sont identiques
      SPNext=SP2;
    }
    else if( (NbPointsTotal==2)&&(SP2.CheckSameSP(SPInit)) ) {
      NbPointsTotal=1;//SP2 et SPInit sont identiques
      SPNext=SP1;
    }

    else if(NbPointsTotal>1) {

    }
  }
  SPNext.SetCoupleValue(T1,T2);
  return (NbPointsTotal);
}
//=======================================================================
//function : CalculPtsInterTriEdgeCoplanaires
//purpose  : 
//=======================================================================
void CalculPtsInterTriEdgeCoplanaires(const Standard_Integer TriSurfID,
                                      const IntPolyh_Point &NormaleTri,
                                      const IntPolyh_Triangle &Tri1,
                                      const IntPolyh_Triangle &Tri2,
                                      const IntPolyh_Point &PE1,
                                      const IntPolyh_Point &PE2,
                                      const IntPolyh_Point &Edge,
                                      const Standard_Integer EdgeIndex,
                                      const IntPolyh_Point &PT1,
                                      const IntPolyh_Point &PT2,
                                      const IntPolyh_Point &Cote,
                                      const Standard_Integer CoteIndex,
                                      IntPolyh_StartPoint &SP1,
                                      IntPolyh_StartPoint &SP2,
                                      Standard_Integer &NbPoints)
{
  Standard_Real aDE, aDC;
  //
  gp_Vec aVE(Edge.X(), Edge.Y(), Edge.Z());
  gp_Vec aVC(Cote.X(), Cote.Y(), Cote.Z());
  //
  aDE = aVE.SquareMagnitude();
  aDC = aVC.SquareMagnitude();
  //
  if (aDE > SquareMyConfusionPrecision) {
    aVE.Divide(aDE);
  }
  //
  if (aDC > SquareMyConfusionPrecision) {
    aVC.Divide(aDC);
  }
  //
  if (!aVE.IsParallel(aVC, MyConfusionPrecision)) {
    ///Edge and side are not parallel
    IntPolyh_Point Per;
    Per.Cross(NormaleTri,Cote);
    Standard_Real p1p = Per.Dot(PE1);
    Standard_Real p2p = Per.Dot(PE2);
    Standard_Real p0p = Per.Dot(PT1);
    ///The edge are PT1 are projected on the perpendicular of the side in the plane of the triangle
    if ( ( ((p1p>=p0p)&&(p0p>=p2p) )||( (p1p<=p0p)&&(p0p<=p2p) )) && (Abs(p1p-p2p) > MyConfusionPrecision)) {
      Standard_Real lambda=(p1p-p0p)/(p1p-p2p);
      if (lambda<-MyConfusionPrecision) {

      }
      IntPolyh_Point PIE;
      if (Abs(lambda)<MyConfusionPrecision)//lambda=0
        PIE=PE1;
      else if (Abs(lambda)>1.0-MyConfusionPrecision)//lambda=1
        PIE=PE2;
      else
        PIE=PE1+Edge*lambda;

      Standard_Real alpha=RealLast();
      if(Cote.X()!=0) alpha=(PIE.X()-PT1.X())/Cote.X();
      else if (Cote.Y()!=0) alpha=(PIE.Y()-PT1.Y())/Cote.Y();
      else if (Cote.Z()!=0) alpha=(PIE.Z()-PT1.Z())/Cote.Z();
      else {

      }
      
      if (alpha<-MyConfusionPrecision) {

      }
      else {
        if (NbPoints==0) {
          SP1.SetXYZ(PIE.X(),PIE.Y(),PIE.Z());
          if (TriSurfID==1) {
            if(Abs(alpha)<MyConfusionPrecision) {//alpha=0
              SP1.SetUV1(PT1.U(),PT1.V());
              SP1.SetUV1(PIE.U(),PIE.V());
              SP1.SetEdge1(-1);
            }
            if(Abs(alpha)>1.0-MyConfusionPrecision) {//alpha=1
              SP1.SetUV1(PT2.U(),PT2.V());
              SP1.SetUV1(PIE.U(),PIE.V());
              SP1.SetEdge1(-1);
            }
            else {
              SP1.SetUV1(PT1.U()+Cote.U()*alpha,PT1.V()+Cote.V()*alpha);
              SP1.SetUV2(PIE.U(),PIE.V());
              SP1.SetEdge1(Tri1.GetEdgeNumber(CoteIndex));
              if (Tri1.GetEdgeOrientation(CoteIndex)>0) SP1.SetLambda1(alpha);
              else SP1.SetLambda1(1.0-alpha);
            }
            NbPoints++;
          }
          else if (TriSurfID==2) {
            if(Abs(alpha)<MyConfusionPrecision) {//alpha=0
              SP1.SetUV1(PT1.U(),PT1.V());
              SP1.SetUV1(PIE.U(),PIE.V());
              SP1.SetEdge2(-1);
            }
            if(Abs(alpha)>1.0-MyConfusionPrecision) {//alpha=1
              SP1.SetUV1(PT2.U(),PT2.V());
              SP1.SetUV1(PIE.U(),PIE.V());
              SP1.SetEdge2(-1);
            }
            else {
              SP1.SetUV1(PIE.U(),PIE.V());
              SP1.SetUV2(PT1.U()+Cote.U()*alpha,PT1.V()+Cote.V()*alpha);
              SP1.SetEdge2(Tri2.GetEdgeNumber(CoteIndex));
              if (Tri2.GetEdgeOrientation(CoteIndex)>0) SP1.SetLambda2(alpha);
              else SP1.SetLambda2(1.0-alpha);
            }
            NbPoints++;
          }
          else {

          }
        }

        else if (NbPoints==1) {
          SP2.SetXYZ(PIE.X(),PIE.Y(),PIE.Z());
          if (TriSurfID==1) {
            if(Abs(alpha)<MyConfusionPrecision) {//alpha=0
              SP2.SetUV1(PT1.U(),PT1.V());
              SP2.SetUV1(PIE.U(),PIE.V());
              SP2.SetEdge1(-1);
            }
            if(Abs(alpha)>1.0-MyConfusionPrecision) {//alpha=1
              SP2.SetUV1(PT2.U(),PT2.V());
              SP2.SetUV1(PIE.U(),PIE.V());
              SP2.SetEdge1(-1);
            }
            else {
              SP2.SetUV1(PT1.U()+Cote.U()*alpha,PT1.V()+Cote.V()*alpha);
              SP2.SetUV2(PIE.U(),PIE.V());
              SP2.SetEdge1(Tri1.GetEdgeNumber(CoteIndex));
              if (Tri1.GetEdgeOrientation(CoteIndex)>0) SP2.SetLambda1(alpha);
              else SP2.SetLambda1(1.0-alpha);
            }
            NbPoints++;
          }
          else if (TriSurfID==2) {
            if(Abs(alpha)<MyConfusionPrecision) {//alpha=0
              SP2.SetUV1(PT1.U(),PT1.V());
              SP2.SetUV1(PIE.U(),PIE.V());
              SP2.SetEdge2(-1);
            }
            if(Abs(alpha)>1.0-MyConfusionPrecision) {//alpha=1
              SP2.SetUV1(PT2.U(),PT2.V());
              SP2.SetUV1(PIE.U(),PIE.V());
              SP2.SetEdge2(-1);
            }
            else {
              SP2.SetUV1(PIE.U(),PIE.V());
              SP2.SetUV2(PT1.U()+Cote.U()*alpha,PT1.V()+Cote.V()*alpha);
              SP2.SetEdge2(Tri2.GetEdgeNumber(CoteIndex));
              if (Tri1.GetEdgeOrientation(CoteIndex)>0) SP2.SetLambda2(alpha);
              else SP2.SetLambda2(1.0-alpha);
            }
            NbPoints++;
          }
          else {

          }
        }

        else if( (NbPoints>2)||(NbPoints<0) ) {

          }
      }
    }
  }
  else {    
    //Side and Edge are parallel, with previous 
    //rejections they are at the same side
    //The points are projected on that side
    Standard_Real pe1p= Cote.Dot(PE1);
    Standard_Real pe2p= Cote.Dot(PE2);
    Standard_Real pt1p= Cote.Dot(PT1);
    Standard_Real pt2p= Cote.Dot(PT2);
    Standard_Real lambda1=0., lambda2=0., alpha1=0., alpha2=0.;
    IntPolyh_Point PEP1,PTP1,PEP2,PTP2;

    if (pe1p>pe2p) {
      if ( (pt1p<pe1p)&&(pe1p<=pt2p) ) {
        lambda1=0.0;
        PEP1=PE1;
        alpha1=((pe1p-pt1p)/(pt2p-pt1p));
        PTP1=PT1+Cote*alpha1;
        NbPoints=1;
        if (pt1p<=pe2p) {
          lambda2=1.0;
          PEP2=PE2;
          alpha2=((pe2p-pt1p)/(pt2p-pt1p));
          PTP2=PT1+Cote*alpha2;
          NbPoints=2;
        }
        else {
          lambda2=((pt1p-pe1p)/(pe2p-pe1p));
          PEP2=PE1+Edge*lambda2;
          alpha2=0.0;
          PTP2=PT1;
          NbPoints=2;
        }
      }
      else if( (pt2p<pe1p)&&(pe1p<=pt1p) ) {
        lambda1=0.0;
        PEP1=PE1;
        alpha1=((pt1p-pe1p)/(pt1p-pt2p));
        PTP1=PT1+Cote*alpha1;
        NbPoints=1;
        if (pt2p<=pe2p) {
          lambda2=1.0;
          PEP2=PE2;
          alpha2=((pe2p-pt1p)/(pt2p-pt1p));
          PTP2=PT1+Cote*alpha2;
          NbPoints=2;
        }
        else {
          lambda2=((pt2p-pe1p)/(pe2p-pe1p));
          PEP2=PE1+Edge*lambda2;
          alpha2=1.0;
          PTP2=PT2;
          NbPoints=2;
        }
      }
    }
    
    if (pe1p<pe2p) {
      if ( (pt1p<pe2p)&&(pe2p<=pt2p) ) {
        lambda1=1.0;
        PEP1=PE2;
        alpha1=((pe2p-pt1p)/(pt2p-pt1p));
        PTP1=PT1+Cote*alpha1;
        NbPoints=1;
        if (pt1p<=pe1p) {
          lambda2=0.0;
          PEP2=PE1;
          alpha2=((pe1p-pt1p)/(pt2p-pt1p));
          PTP2=PT1+Cote*alpha2;
          NbPoints=2;
        }
        else {
          lambda2=((pt1p-pe1p)/(pe2p-pe1p));
          PEP2=PE2+Edge*lambda2;
          alpha2=0.0;
          PTP2=PT1;
          NbPoints=2;
        }
      }
      else if( (pt2p<pe2p)&&(pe2p<=pt1p) ) {
        lambda1=1.0;
        PEP1=PE2;
        alpha1=((pt1p-pe2p)/(pt1p-pt2p));
        PTP1=PT1+Cote*alpha1;
        NbPoints=1;
        if (pt2p<=pe1p) {
          lambda2=0.0;
          PEP2=PE1;
          alpha2=((pe1p-pt1p)/(pt2p-pt1p));
          PTP2=PT1+Cote*alpha2;
          NbPoints=2;
        }
        else {
          lambda2=((pt2p-pe1p)/(pe2p-pe1p));
          PEP2=PE1+Edge*lambda2;
          alpha2=1.0;
          PTP2=PT2;
          NbPoints=2;
        }
      }
    }
  
    if (NbPoints!=0) {
      SP1.SetXYZ(PEP1.X(),PEP1.Y(),PEP1.Z());
      if (TriSurfID==1) {///cote appartient a Tri1
        SP1.SetUV1(PTP1.U(),PTP1.V());          
        SP1.SetUV2(PEP1.U(),PEP1.V());
        SP1.SetEdge1(Tri1.GetEdgeNumber(CoteIndex));

        if(Tri1.GetEdgeOrientation(CoteIndex)>0) SP1.SetLambda1(alpha1);
        else SP1.SetLambda1(1.0-alpha1);
        
        if(Tri2.GetEdgeOrientation(EdgeIndex)>0) SP1.SetLambda2(lambda1);
        else SP1.SetLambda2(1.0-lambda1);
      }
      if (TriSurfID==2) {///cote appartient a Tri2
        SP1.SetUV1(PEP1.U(),PTP1.V());          
        SP1.SetUV2(PTP1.U(),PEP1.V());
        SP1.SetEdge2(Tri2.GetEdgeNumber(CoteIndex));

        if(Tri2.GetEdgeOrientation(CoteIndex)>0) SP1.SetLambda1(alpha1);
        else SP1.SetLambda1(1.0-alpha1);
        
        if(Tri1.GetEdgeOrientation(EdgeIndex)>0) SP1.SetLambda2(lambda1);
        else SP1.SetLambda2(1.0-lambda1);
      }
      
      //It is checked if PEP1!=PEP2
      if ( (NbPoints==2)&&(Abs(PEP1.U()-PEP2.U())<MyConfusionPrecision)
         &&(Abs(PEP1.V()-PEP2.V())<MyConfusionPrecision) ) NbPoints=1;
      if (NbPoints==2) {
        SP2.SetXYZ(PEP2.X(),PEP2.Y(),PEP2.Z());
        if (TriSurfID==1) {
          SP2.SetUV1(PTP2.U(),PTP2.V());          
          SP2.SetUV2(PEP2.U(),PEP2.V());
          SP2.SetEdge1(Tri1.GetEdgeNumber(CoteIndex));

          if(Tri1.GetEdgeOrientation(CoteIndex)>0) SP2.SetLambda1(alpha1);
          else SP2.SetLambda1(1.0-alpha1);
          
          if(Tri2.GetEdgeOrientation(EdgeIndex)>0) SP2.SetLambda2(lambda1);
          else SP2.SetLambda2(1.0-lambda1);
        }
        if (TriSurfID==2) {
          SP2.SetUV1(PEP2.U(),PTP2.V());          
          SP2.SetUV2(PTP2.U(),PEP2.V());
          SP2.SetEdge2(Tri2.GetEdgeNumber(CoteIndex));

          if(Tri1.GetEdgeOrientation(CoteIndex)>0) SP2.SetLambda1(alpha1);
          else SP2.SetLambda1(1.0-alpha1);
          
          if(Tri2.GetEdgeOrientation(EdgeIndex)>0) SP2.SetLambda2(lambda1);
          else SP2.SetLambda2(1.0-lambda1);
        } 
      }
    }
  }
  //Filter if the point is placed on top, the edge is set  to -1
  if (NbPoints>0) {
    if(Abs(SP1.Lambda1())<MyConfusionPrecision)
      SP1.SetEdge1(-1);
    if(Abs(SP1.Lambda1()-1)<MyConfusionPrecision)
      SP1.SetEdge1(-1);
    if(Abs(SP1.Lambda2())<MyConfusionPrecision)
      SP1.SetEdge2(-1);
    if(Abs(SP1.Lambda2()-1)<MyConfusionPrecision)
      SP1.SetEdge2(-1);
  }
  if (NbPoints==2) {
    if(Abs(SP2.Lambda1())<MyConfusionPrecision)
      SP2.SetEdge1(-1);
    if(Abs(SP2.Lambda1()-1)<MyConfusionPrecision)
      SP2.SetEdge1(-1);
    if(Abs(SP2.Lambda2())<MyConfusionPrecision)
      SP2.SetEdge2(-1);
    if(Abs(SP2.Lambda2()-1)<MyConfusionPrecision)
      SP2.SetEdge2(-1);
  }
}

//=======================================================================
//function : TriangleEdgeContact
//purpose  : 
//=======================================================================
Standard_Integer IntPolyh_MaillageAffinage::TriangleEdgeContact
  (const Standard_Integer TriSurfID,
   const Standard_Integer EdgeIndex,
   const IntPolyh_Triangle &Tri1,
   const IntPolyh_Triangle &Tri2,
   const IntPolyh_Point &PT1,
   const IntPolyh_Point &PT2,
   const IntPolyh_Point &PT3,
   const IntPolyh_Point &Cote12,
   const IntPolyh_Point &Cote23,
   const IntPolyh_Point &Cote31,
   const IntPolyh_Point &PE1,const IntPolyh_Point &PE2,
   const IntPolyh_Point &Edge,
   const IntPolyh_Point &NormaleT,
   IntPolyh_StartPoint &SP1,IntPolyh_StartPoint &SP2) const 
{

  Standard_Real lambda =0., alpha =0., beta =0.;

  //One of edges on which the point is located is known
  if (TriSurfID==1) {
    SP1.SetEdge2(Tri2.GetEdgeNumber(EdgeIndex));
    SP2.SetEdge2(Tri2.GetEdgeNumber(EdgeIndex));
  }
  else if (TriSurfID==2) {
    SP1.SetEdge1(Tri1.GetEdgeNumber(EdgeIndex));
    SP2.SetEdge1(Tri1.GetEdgeNumber(EdgeIndex));
  }
  else {

  }

  /**The edge is projected on the normal in the triangle if yes 
  --> a free intersection (point I)--> Start point is found */
  Standard_Integer NbPoints=0;
  if(NormaleT.SquareModulus()==0) {

  }
  else if( (Cote12.SquareModulus()==0)
       ||(Cote23.SquareModulus()==0)
       ||(Cote31.SquareModulus()==0) ) {

  }
  else if(Edge.SquareModulus()==0) {

  }
  else {
    Standard_Real pe1 = NormaleT.Dot(PE1);
    Standard_Real pe2 = NormaleT.Dot(PE2);
    Standard_Real pt1 = NormaleT.Dot(PT1);  
    
    // PE1I = lambda.Edge
   
    if( (Abs(pe1-pt1)<MyConfusionPrecision)&&(Abs(pe2-pt1)<MyConfusionPrecision)) {
      //edge and triangle are coplanar (two contact points at maximum)


      //the tops of the triangle are projected on the perpendicular to the edge 
      IntPolyh_Point PerpEdge;
      PerpEdge.Cross(NormaleT,Edge);
      Standard_Real pp1 = PerpEdge.Dot(PT1);
      Standard_Real pp2 = PerpEdge.Dot(PT2);
      Standard_Real pp3 = PerpEdge.Dot(PT3);
      Standard_Real ppe1 = PerpEdge.Dot(PE1);
      

      if ( (Abs(pp1-pp2)<MyConfusionPrecision)&&(Abs(pp1-pp3)<MyConfusionPrecision) ) {

      }
      else {
        if ( ( (pp1>=ppe1)&&(pp2<=ppe1)&&(pp3<=ppe1) ) || ( (pp1<=ppe1)&&(pp2>=ppe1)&&(pp3>=ppe1) ) ){
          //there are two sides (common top PT1) that can cut the edge
          
          //first side
          CalculPtsInterTriEdgeCoplanaires(TriSurfID,NormaleT,Tri1,Tri2,PE1,PE2,Edge,EdgeIndex,
                                           PT1,PT2,Cote12,1,SP1,SP2,NbPoints);
          
          if ( (NbPoints>1)&&(Abs(SP2.U1()-SP1.U1())<MyConfusionPrecision)
              &&(Abs(SP1.V1()-SP2.V1())<MyConfusionPrecision) ) NbPoints=1;
          
          //second side
          if (NbPoints<2) CalculPtsInterTriEdgeCoplanaires(TriSurfID,NormaleT,Tri1,Tri2,PE1,PE2,Edge,EdgeIndex,
                                                           PT3,PT1,Cote31,3,SP1,SP2,NbPoints);
        }
        
        if ( (NbPoints>1)&&(Abs(SP1.U1()-SP2.U1())<MyConfusionPrecision)
            &&(Abs(SP1.V2()-SP2.V1())<MyConfusionPrecision) ) NbPoints=1;
        if (NbPoints>=2) return(NbPoints);
        
        else if ( ( ( (pp2>=ppe1)&&(pp1<=ppe1)&&(pp3<=ppe1) ) || ( (pp2<=ppe1)&&(pp1>=ppe1)&&(pp3>=ppe1) ) )
                 && (NbPoints<2) ) {
          //there are two sides (common top PT2) that can cut the edge
          
          //first side
          CalculPtsInterTriEdgeCoplanaires(TriSurfID,NormaleT,Tri1,Tri2,PE1,PE2,Edge,EdgeIndex,
                                           PT1,PT2,Cote12,1,SP1,SP2,NbPoints);
          
          if ( (NbPoints>1)&&(Abs(SP2.U1()-SP1.U1())<MyConfusionPrecision)
              &&(Abs(SP1.V1()-SP2.V1())<MyConfusionPrecision) ) NbPoints=1;
          
          //second side
          if(NbPoints<2) CalculPtsInterTriEdgeCoplanaires(TriSurfID,NormaleT,Tri1,Tri2,PE1,PE2,Edge,EdgeIndex,
                                                          PT2,PT3,Cote23,2,SP1,SP2,NbPoints);
        }
        if ( (NbPoints>1)&&(Abs(SP2.U1()-SP1.U1())<MyConfusionPrecision)
            &&(Abs(SP1.V1()-SP2.V1())<MyConfusionPrecision) ) NbPoints=1;
        if (NbPoints>=2) return(NbPoints);
        
        else if ( (( (pp3>=ppe1)&&(pp1<=ppe1)&&(pp2<=ppe1) ) || ( (pp3<=ppe1)&&(pp1>=ppe1)&&(pp2>=ppe1) ))
                && (NbPoints<2) ) {
          //there are two sides (common top PT3) that can cut the edge
          
          //first side
          CalculPtsInterTriEdgeCoplanaires(TriSurfID,NormaleT,Tri1,Tri2,PE1,PE2,Edge,EdgeIndex,
                                           PT3,PT1,Cote31,3,SP1,SP2,NbPoints);
          
          if ( (NbPoints>1)&&(Abs(SP2.U1()-SP1.U1())<MyConfusionPrecision)
              &&(Abs(SP1.V1()-SP2.V1())<MyConfusionPrecision) ) NbPoints=1;
          
          //second side
          if (NbPoints<2) CalculPtsInterTriEdgeCoplanaires(TriSurfID,NormaleT,Tri1,Tri2,PE1,PE2,Edge,EdgeIndex,
                                                           PT2,PT3,Cote23,2,SP1,SP2,NbPoints);
        }
        if ( (NbPoints>1)&&(Abs(SP2.U1()-SP1.U1())<MyConfusionPrecision)
            &&(Abs(SP2.V1()-SP1.V1())<MyConfusionPrecision) ) NbPoints=1;
        if (NbPoints>=2) return(NbPoints);
      }
    }

    //------------------------------------------------------
    // NON COPLANAR edge and triangle (a contact point)
    //------------------------------------------------------
    else if(   ( (pe1>=pt1)&&(pt1>=pe2) ) || ( (pe1<=pt1)&&(pt1<=pe2) )  ) { //
      lambda=(pe1-pt1)/(pe1-pe2);
      IntPolyh_Point PI;
      if (lambda<-MyConfusionPrecision) {

      }
      else if (Abs(lambda)<MyConfusionPrecision) {//lambda==0
        PI=PE1;
        if(TriSurfID==1) SP1.SetEdge2(-1);
        else SP1.SetEdge1(-1);
      }
      else if (Abs(lambda-1.0)<MyConfusionPrecision) {//lambda==1
        PI=PE2;
        if(TriSurfID==1) SP1.SetEdge2(-1);
        else SP1.SetEdge1(-1);
      }
      else {
        PI=PE1+Edge*lambda;
        if(TriSurfID==1) {
          if(Tri2.GetEdgeOrientation(EdgeIndex)>0)
            SP1.SetLambda2(lambda);
          else SP1.SetLambda2(1.0-lambda);
        }
        if(TriSurfID==2) {
          if(Tri1.GetEdgeOrientation(EdgeIndex)>0)
            SP1.SetLambda1(lambda);
          else SP1.SetLambda1(1.0-lambda);
        }
      }
        
      Standard_Real Cote23X=Cote23.X();
      Standard_Real D1=0.0;
      Standard_Real D3,D4;

      //Combination Eq1 Eq2
      if(Abs(Cote23X)>MyConfusionPrecision) {
        D1=Cote12.Y()-Cote12.X()*Cote23.Y()/Cote23X;
      }
      if(Abs(D1)>MyConfusionPrecision) {
        alpha = ( PI.Y()-PT1.Y()-(PI.X()-PT1.X())*Cote23.Y()/Cote23X )/D1;

        ///It is checked if 1.0>=alpha>=0.0
        if ((alpha<-MyConfusionPrecision)||(alpha>(1.0+MyConfusionPrecision))) return(0);
        else beta = (PI.X()-PT1.X()-alpha*Cote12.X())/Cote23X;
      }
      //Combination Eq1 and Eq2 with Cote23.X()==0
      else if ( (Abs(Cote12.X())>MyConfusionPrecision)
              &&(Abs(Cote23X)<MyConfusionPrecision) ) { //There is Cote23.X()==0
        alpha = (PI.X()-PT1.X())/Cote12.X();
        
        if ((alpha<-MyConfusionPrecision)||(alpha>(1.0+MyConfusionPrecision))) return(0);
        
        else if (Abs(Cote23.Y())>MyConfusionPrecision) beta = (PI.Y()-PT1.Y()-alpha*Cote12.Y())/Cote23.Y();
        else if (Abs(Cote23.Z())>MyConfusionPrecision) beta = (PI.Z()-PT1.Z()-alpha*Cote12.Z())/Cote23.Z();
        else  {

        }
      }
      //Combination Eq1 and Eq3
      else if ( (Abs(Cote23.X())>MyConfusionPrecision)
              &&(Abs( D3= (Cote12.Z()-Cote12.X()*Cote23.Z()/Cote23.X()) ) > MyConfusionPrecision) ) {
        
        alpha = (PI.Z()-PT1.Z()-(PI.X()-PT1.X())*Cote23.Z()/Cote23.X())/D3;
        
        if ( (alpha<-MyConfusionPrecision)||(alpha>(1.0+MyConfusionPrecision)) ) return(0);
        else beta = (PI.X()-PT1.X()-alpha*Cote12.X())/Cote23.X();
      }
      //Combination Eq2 and Eq3
      else if ( (Abs(Cote23.Y())>MyConfusionPrecision)
              &&(Abs( D4= (Cote12.Z()-Cote12.Y()*Cote23.Z()/Cote23.Y()) ) > MyConfusionPrecision) ) {
        
        alpha = (PI.Z()-PT1.Z()-(PI.Y()-PT1.Y())*Cote23.Z()/Cote23.Y())/D4;
        
        if ( (alpha<-MyConfusionPrecision)||(alpha>(1.0+MyConfusionPrecision)) ) return(0);
        else beta = (PI.Y()-PT1.Y()-alpha*Cote12.Y())/Cote23.Y();
      }
      //Combination Eq2 and Eq3 with Cote23.Y()==0
      else if ( (Abs(Cote12.Y())>MyConfusionPrecision)
             && (Abs(Cote23.Y())<MyConfusionPrecision) ) {
        alpha = (PI.Y()-PT1.Y())/Cote12.Y();
        
        if ( (alpha<-MyConfusionPrecision)||(alpha>(1.0+MyConfusionPrecision)) ) return(0);
        
        else if (Abs(Cote23.Z())>MyConfusionPrecision) beta = (PI.Z()-PT1.Z()-alpha*Cote12.Z())/Cote23.Z();
        
        else {
          printf("\nCote PT2PT3 nul1\n");
          PT2.Dump(2004);
          PT3.Dump(3004);
        }
      }
      //Combination Eq1 and Eq3 with Cote23.Z()==0
      else if ( (Abs(Cote12.Z())>MyConfusionPrecision)
             && (Abs(Cote23.Z())<MyConfusionPrecision) ) {
        alpha = (PI.Z()-PT1.Z())/Cote12.Z();
        
        if ( (alpha<-MyConfusionPrecision)||(alpha>(1.0+MyConfusionPrecision)) ) return(0);
        
        else if (Abs(Cote23.X())>MyConfusionPrecision) beta = (PI.X()-PT1.X()-alpha*Cote12.X())/Cote23.X();
        
        else {

        }
      }
      
      else { //Particular case not processed ?

        alpha=RealLast();
        beta=RealLast();
      }
      
      if( (beta<-MyConfusionPrecision)||(beta>(alpha+MyConfusionPrecision)) ) return(0);
      else {
        SP1.SetXYZ(PI.X(),PI.Y(),PI.Z());

        if (TriSurfID==1) {
          SP1.SetUV2(PI.U(),PI.V());
          SP1.SetUV1(PT1.U()+Cote12.U()*alpha+Cote23.U()*beta, PT1.V()+Cote12.V()*alpha+Cote23.V()*beta);
          NbPoints++;
          if (alpha<MyConfusionPrecision) {//alpha=0 --> beta==0
            SP1.SetXYZ(PT1.X(),PT1.Y(),PT1.Z());
            SP1.SetUV1(PT1.U(),PT1.V());
            SP1.SetEdge1(-1);
          }
          else if ( (beta<MyConfusionPrecision)&&(Abs(1-alpha)<MyConfusionPrecision) ) {//beta==0 alpha==1
            SP1.SetXYZ(PT2.X(),PT2.Y(),PT2.Z());
            SP1.SetUV1(PT2.U(),PT2.V());
            SP1.SetEdge1(-1);
          }
          else if ( (Abs(beta-1)<MyConfusionPrecision)&&(Abs(1-alpha)<MyConfusionPrecision) ) {//beta==1 alpha==1
            SP1.SetXYZ(PT3.X(),PT3.Y(),PT3.Z());
            SP1.SetUV1(PT3.U(),PT3.V());
            SP1.SetEdge1(-1);
          }
          else if (beta<MyConfusionPrecision) {//beta==0 
            SP1.SetEdge1(Tri1.GetEdgeNumber(1));
            if(Tri1.GetEdgeOrientation(1)>0)
              SP1.SetLambda1(alpha);
            else SP1.SetLambda1(1.0-alpha);
          }
          else if (Abs(beta-alpha)<MyConfusionPrecision) {//beta==alpha  
            SP1.SetEdge1(Tri1.GetEdgeNumber(3));
            if(Tri1.GetEdgeOrientation(3)>0)
              SP1.SetLambda1(1.0-alpha);
            else SP1.SetLambda1(alpha);
          }
          else if (Abs(alpha-1)<MyConfusionPrecision) {//alpha==1
            SP1.SetEdge1(Tri1.GetEdgeNumber(2));
            if(Tri1.GetEdgeOrientation(2)>0)
              SP1.SetLambda1(beta);
            else SP1.SetLambda1(1.0-beta);
          }
        }
        else if(TriSurfID==2) {
          SP1.SetUV1(PI.U(),PI.V());
          SP1.SetUV2(PT1.U()+Cote12.U()*alpha+Cote23.U()*beta, PT1.V()+Cote12.V()*alpha+Cote23.V()*beta);
          NbPoints++;
          if (alpha<MyConfusionPrecision) {//alpha=0 --> beta==0
            SP1.SetXYZ(PT1.X(),PT1.Y(),PT1.Z());
            SP1.SetUV2(PT1.U(),PT1.V());
            SP1.SetEdge2(-1);
          }
          else if ( (beta<MyConfusionPrecision)&&(Abs(1-alpha)<MyConfusionPrecision) ) {//beta==0 alpha==1
            SP1.SetXYZ(PT2.X(),PT2.Y(),PT2.Z());
            SP1.SetUV2(PT2.U(),PT2.V());
            SP1.SetEdge2(-1);
          }
          else if ( (Abs(beta-1)<MyConfusionPrecision)&&(Abs(1-alpha)<MyConfusionPrecision) ) {//beta==1 alpha==1
            SP1.SetXYZ(PT3.X(),PT3.Y(),PT3.Z());
            SP1.SetUV2(PT3.U(),PT3.V());
            SP1.SetEdge2(-1);
          }
          else if (beta<MyConfusionPrecision) { //beta==0
            SP1.SetEdge2(Tri2.GetEdgeNumber(1));
            if(Tri2.GetEdgeOrientation(1)>0)
              SP1.SetLambda2(alpha);
            else SP1.SetLambda2(1.0-alpha);
          }
          else if (Abs(beta-alpha)<MyConfusionPrecision) {//beta==alpha
            SP1.SetEdge2(Tri2.GetEdgeNumber(3));
            if(Tri2.GetEdgeOrientation(3)>0)
              SP1.SetLambda2(1.0-alpha);
            else SP1.SetLambda2(alpha);
          }
          else if (Abs(alpha-1)<MyConfusionPrecision) {//alpha==1
            SP1.SetEdge2(Tri2.GetEdgeNumber(2));        
            if(Tri2.GetEdgeOrientation(2)>0)
              SP1.SetLambda2(alpha);
            else SP1.SetLambda2(1.0-alpha);
          }
        }
        else {

        }
      }
    }
    else return 0;
  }
  return (NbPoints);
}
//=======================================================================
//function : TriangleCompare
//purpose  : Analyze  each couple of  triangles from the two --
//           array  of triangles,  to   see  if they are  in
//           contact,  and  compute the  incidence.  Then  put
//           couples  in contact  in  the  array  of  couples
//=======================================================================
Standard_Integer IntPolyh_MaillageAffinage::TriangleCompare ()
{
  // Find couples with interfering bounding boxes
  IntPolyh_IndexedDataMapOfIntegerArrayOfInteger aDMILI;
  GetInterferingTriangles(TTriangles1, TPoints1,
                          TTriangles2, TPoints2,
                          aDMILI);
  if (aDMILI.IsEmpty()) {
    return 0;
  }
  //
  Standard_Real CoupleAngle = -2.0;
  //
  // Intersection of the triangles
  Standard_Integer i, aNb = aDMILI.Extent();
  for (i = 1; i <= aNb; ++i) {
    const Standard_Integer i_S1 = aDMILI.FindKey(i);
    IntPolyh_Triangle &Triangle1 =  TTriangles1[i_S1];
    const IntPolyh_Point& P1 = TPoints1[Triangle1.FirstPoint()];
    const IntPolyh_Point& P2 = TPoints1[Triangle1.SecondPoint()];
    const IntPolyh_Point& P3 = TPoints1[Triangle1.ThirdPoint()];
    //
    const IntPolyh_ArrayOfInteger& aLI2 = aDMILI(i);
    IntPolyh_ArrayOfInteger::Iterator aItLI(aLI2);
    for (; aItLI.More(); aItLI.Next()) {
      const Standard_Integer i_S2 = aItLI.Value();
      IntPolyh_Triangle &Triangle2 =  TTriangles2[i_S2];
      const IntPolyh_Point& Q1 = TPoints2[Triangle2.FirstPoint()];
      const IntPolyh_Point& Q2 = TPoints2[Triangle2.SecondPoint()];
      const IntPolyh_Point& Q3 = TPoints2[Triangle2.ThirdPoint()];
      //
      if (TriContact(P1, P2, P3, Q1, Q2, Q3, CoupleAngle)) {
        IntPolyh_Couple aCouple(i_S1, i_S2, CoupleAngle);
        TTrianglesContacts.Append(aCouple);
        //
        Triangle1.SetIntersection(Standard_True);
        Triangle2.SetIntersection(Standard_True);
      }
    }
  }
  return TTrianglesContacts.Extent();
}

//=======================================================================
//function : CheckCoupleAndGetAngle
//purpose  : 
//=======================================================================
Standard_Boolean CheckCoupleAndGetAngle(const Standard_Integer T1, 
                                        const Standard_Integer T2,
                                        Standard_Real& Angle, 
                                        IntPolyh_ListOfCouples &TTrianglesContacts) 
{
  IntPolyh_ListIteratorOfListOfCouples aIt(TTrianglesContacts);
  for (; aIt.More(); aIt.Next()) {
    IntPolyh_Couple& TestCouple = aIt.ChangeValue();
    if (!TestCouple.IsAnalyzed()) {
      if (TestCouple.FirstValue() == T1 && TestCouple.SecondValue() == T2) {
        TestCouple.SetAnalyzed(Standard_True);
        Angle = TestCouple.Angle();
        return Standard_True;
      }
    }
  }
  return Standard_False;
}
//=======================================================================
//function : CheckCoupleAndGetAngle2
//purpose  : 
//=======================================================================
Standard_Boolean CheckCoupleAndGetAngle2(const Standard_Integer T1,
                                         const Standard_Integer T2,
                                         const Standard_Integer T11,
                                         const Standard_Integer T22,
                                         IntPolyh_ListIteratorOfListOfCouples& theItCT11,
                                         IntPolyh_ListIteratorOfListOfCouples& theItCT22,
                                         Standard_Real & Angle,
                                         IntPolyh_ListOfCouples &TTrianglesContacts) 
{
  ///couple T1 T2 is found in the list
  ///T11 and T22 are two other triangles implied  in the contact edge edge
  /// CT11 couple( T1,T22) and CT22 couple (T2,T11)
  /// these couples will be marked if there is a start point
  Standard_Boolean Test1 , Test2, Test3;
  Test1 = Test2 = Test3 = Standard_False;
  //
  IntPolyh_ListIteratorOfListOfCouples aIt(TTrianglesContacts);
  for (; aIt.More(); aIt.Next()) {
    IntPolyh_Couple& TestCouple = aIt.ChangeValue();
    if (TestCouple.IsAnalyzed()) {
      continue;
    }
    //
    if (TestCouple.FirstValue() == T1) {
      if (TestCouple.SecondValue() == T2) {
        Test1 = Standard_True;
        TestCouple.SetAnalyzed(Standard_True);
        Angle = TestCouple.Angle();
      }
      else if (TestCouple.SecondValue() == T22) {
        Test2 = Standard_True;
        theItCT11 = aIt;
        Angle = TestCouple.Angle();
      }
    }
    else if (TestCouple.FirstValue() == T11) {
      if (TestCouple.SecondValue() == T2) {
        Test3 = Standard_True;
        theItCT22 = aIt;
        Angle = TestCouple.Angle();
      }
    }
    //
    if (Test1 && Test2 && Test3) {
      break;
    }
  }
  return Test1;
}
//=======================================================================
//function : CheckNextStartPoint
//purpose  : it is checked if the point is not a top
//           then it is stored in one or several valid arrays with 
// the proper list number
//=======================================================================
Standard_Integer CheckNextStartPoint(IntPolyh_SectionLine & SectionLine,
                                     IntPolyh_ArrayOfTangentZones & TTangentZones,
                                     IntPolyh_StartPoint & SP,
                                     const Standard_Boolean Prepend)//=Standard_False) 
{
  Standard_Integer Test=1;
  if( (SP.E1()==-1)||(SP.E2()==-1) ) {
    //The tops of triangle are analyzed
    //It is checked if they are not in the array TTangentZones
    Standard_Integer FinTTZ=TTangentZones.NbItems();
    for(Standard_Integer uiui=0; uiui<FinTTZ; uiui++) {
      IntPolyh_StartPoint TestSP=TTangentZones[uiui];
      if ( (Abs(SP.U1()-TestSP.U1())<MyConfusionPrecision)
          &&(Abs(SP.V1()-TestSP.V1())<MyConfusionPrecision) ) {
        if ( (Abs(SP.U2()-TestSP.U2())<MyConfusionPrecision)
            &&(Abs(SP.V2()-TestSP.V2())<MyConfusionPrecision) ) {
          Test=0;//SP is already in the list of  tops
          uiui=FinTTZ;
        }
      }
    }
    if (Test) {//the top does not belong to the list of TangentZones
      SP.SetChainList(-1);
      TTangentZones[FinTTZ]=SP;
      TTangentZones.IncrementNbItems();
      Test=0;//the examined point is a top
    }
  }
  else if (Test) {
    if (Prepend)
      SectionLine.Prepend(SP);
    else {
      SectionLine[SectionLine.NbStartPoints()]=SP;
      SectionLine.IncrementNbStartPoints();
    }

  }
  //if the point is not a top Test=1
  //The chain is continued
  return(Test); 
}
//=======================================================================
//function : StartPointsChain
//purpose  : Loop on the array of couples. Compute StartPoints.
//           Try to chain  the StartPoints into SectionLines or
//           put  the  point  in  the    ArrayOfTangentZones if
//           chaining it, is not possible.
//=======================================================================
Standard_Integer IntPolyh_MaillageAffinage::StartPointsChain
  (IntPolyh_ArrayOfSectionLines& TSectionLines,
   IntPolyh_ArrayOfTangentZones& TTangentZones) 
{
  //Loop on the array of couples filled in the function COMPARE()
  IntPolyh_ListIteratorOfListOfCouples aIt(TTrianglesContacts);
  for (; aIt.More(); aIt.Next()) {
    IntPolyh_Couple& aCouple = aIt.ChangeValue();
    // Check if the couple of triangles has not been already examined.
    if(!aCouple.IsAnalyzed()) {

      Standard_Integer SectionLineIndex=TSectionLines.NbItems();
      // fill last section line if still empty (eap)
      if (SectionLineIndex > 0
          &&
          TSectionLines[SectionLineIndex-1].NbStartPoints() == 0)
        SectionLineIndex -= 1;
      else
        TSectionLines.IncrementNbItems();

      IntPolyh_SectionLine &  MySectionLine=TSectionLines[SectionLineIndex];
      if (MySectionLine.GetN() == 0) // eap
        MySectionLine.Init(10000);//Initialisation of array of StartPoint

      Standard_Integer NbPoints=-1;
      Standard_Integer T1I, T2I;
      T1I = aCouple.FirstValue();
      T2I = aCouple.SecondValue();
      
      // Start points for the current couple are found
      IntPolyh_StartPoint SP1, SP2;
      NbPoints=StartingPointsResearch(T1I,T2I,SP1, SP2);//first calculation
      aCouple.SetAnalyzed(Standard_True);//the couple is marked

      if(NbPoints==1) {// particular case top/triangle or edge/edge
        //the start point is input in the array
        SP1.SetChainList(SectionLineIndex);
        SP1.SetAngle(aCouple.Angle());
        //it is checked if the point is not atop of the triangle
        if(CheckNextStartPoint(MySectionLine,TTangentZones,SP1)) {
          IntPolyh_StartPoint SPNext1;
          Standard_Integer TestSP1=0;
          
          //chain of a side
          IntPolyh_StartPoint SP11;//=SP1;
          if(SP1.E1()>=0) { //&&(SP1.E2()!=-1) already tested if the point is not a top
            Standard_Integer NextTriangle1=0;
            if (TEdges1[SP1.E1()].FirstTriangle()!=T1I) NextTriangle1=TEdges1[SP1.E1()].FirstTriangle();
            else NextTriangle1=TEdges1[SP1.E1()].SecondTriangle();
            
            Standard_Real Angle=-2.0;
            if (CheckCoupleAndGetAngle(NextTriangle1,T2I,Angle,TTrianglesContacts)) {
              //it is checked if the couple exists and is marked
              Standard_Integer NbPoints11=0;
              NbPoints11=NextStartingPointsResearch(NextTriangle1,T2I,SP1,SP11);
              if (NbPoints11==1) {
                SP11.SetChainList(SectionLineIndex);
                SP11.SetAngle(Angle);
                
                if(CheckNextStartPoint(MySectionLine,TTangentZones,SP11)) {            
                  Standard_Integer EndChainList=1;
                  while (EndChainList!=0) {
                    TestSP1=GetNextChainStartPoint(SP11,SPNext1,MySectionLine,TTangentZones);
                    if(TestSP1==1) {
                      SPNext1.SetChainList(SectionLineIndex);
                      if(CheckNextStartPoint(MySectionLine,TTangentZones,SPNext1))
                        SP11=SPNext1;
                      else EndChainList=0;
                    }
                    else EndChainList=0; //There is no next point
                  }
                }
               
              }
              else {
                if(NbPoints11>1) {//The point is input in the array TTangentZones
                  TTangentZones[TTangentZones.NbItems()]=SP11;//default list number = -1
                  TTangentZones.IncrementNbItems();
                }
                else {

                }
              }
            }
          }
          else if (SP1.E2()<0){

          }
          //chain of the other side
          IntPolyh_StartPoint SP12;//=SP1;
          if (SP1.E2()>=0) { //&&(SP1.E1()!=-1) already tested
            Standard_Integer NextTriangle2;
            if (TEdges2[SP1.E2()].FirstTriangle()!=T2I) NextTriangle2=TEdges2[SP1.E2()].FirstTriangle();
            else NextTriangle2=TEdges2[SP1.E2()].SecondTriangle();
            
            Standard_Real Angle=-2.0;
            if(CheckCoupleAndGetAngle(T1I,NextTriangle2,Angle,TTrianglesContacts)) {
              Standard_Integer NbPoints12=0;
              NbPoints12=NextStartingPointsResearch(T1I,NextTriangle2,SP1, SP12);
              if (NbPoints12==1) {
                
                SP12.SetChainList(SectionLineIndex);
                SP12.SetAngle(Angle);
                Standard_Boolean Prepend = Standard_True; // eap
                
                if(CheckNextStartPoint(MySectionLine,TTangentZones,SP12, Prepend)) {
                  Standard_Integer EndChainList=1;
                  while (EndChainList!=0) {
                    TestSP1=GetNextChainStartPoint(SP12,SPNext1,
                                                   MySectionLine,TTangentZones,
                                                   Prepend); // eap
                    if(TestSP1==1) {
                      SPNext1.SetChainList(SectionLineIndex);
                      if(CheckNextStartPoint(MySectionLine,TTangentZones,SPNext1,Prepend))
                        SP12=SPNext1;
                      else EndChainList=0;
                    }
                    else EndChainList=0; //there is no next point
                  }
                }
                
                else {
                  if(NbPoints12>1) {//The points are input in the array TTangentZones
                    TTangentZones[TTangentZones.NbItems()]=SP12;//default list number = -1
                    TTangentZones.IncrementNbItems();
                  }
                  else {

                  }
                }
              }
            }
          }
          else if(SP1.E1()<0){

          }
        }
      }
      else if(NbPoints==2) {
        //the start points are input in the array 
        IntPolyh_StartPoint SPNext2;
        Standard_Integer TestSP2=0;
        Standard_Integer EndChainList=1;

        SP1.SetChainList(SectionLineIndex);
        SP1.SetAngle(aCouple.Angle());
        if(CheckNextStartPoint(MySectionLine,TTangentZones,SP1)) {

          //chain of a side
          while (EndChainList!=0) {
            TestSP2=GetNextChainStartPoint(SP1,SPNext2,MySectionLine,TTangentZones);
            if(TestSP2==1) {
              SPNext2.SetChainList(SectionLineIndex);
              if(CheckNextStartPoint(MySectionLine,TTangentZones,SPNext2))
                SP1=SPNext2;
              else EndChainList=0;
            }
            else EndChainList=0; //there is no next point
          }
        }
        
        SP2.SetChainList(SectionLineIndex);
        SP2.SetAngle(aCouple.Angle());
        Standard_Boolean Prepend = Standard_True; // eap

        if(CheckNextStartPoint(MySectionLine,TTangentZones,SP2,Prepend)) {
          
          //chain of the other side
          EndChainList=1;
          while (EndChainList!=0) {
            TestSP2=GetNextChainStartPoint(SP2,SPNext2,
                                           MySectionLine,TTangentZones,
                                           Prepend); // eap
            if(TestSP2==1) {
              SPNext2.SetChainList(SectionLineIndex);
              if(CheckNextStartPoint(MySectionLine,TTangentZones,SPNext2,Prepend))
                SP2=SPNext2;
              else EndChainList=0;
            }
            else EndChainList=0; //there is no next point
          }
        }
      }

      else if( (NbPoints>2)&&(NbPoints<7) ) {
        //More than two start points 
        //the start point is input in the table
        SP1.SetChainList(SectionLineIndex);
        CheckNextStartPoint(MySectionLine,TTangentZones,SP1);
      }
      
      else {

      }
    }
  }

  return(1);
}
//=======================================================================
//function : GetNextChainStartPoint
//purpose  : Mainly  used  by StartPointsChain(), this function
//           try to compute the next StartPoint.
//           GetNextChainStartPoint is used only if it is known that there are 2 contact points
//=======================================================================
Standard_Integer IntPolyh_MaillageAffinage::GetNextChainStartPoint
  (const IntPolyh_StartPoint & SP,
   IntPolyh_StartPoint & SPNext,
   IntPolyh_SectionLine & MySectionLine,
   IntPolyh_ArrayOfTangentZones & TTangentZones,
   const Standard_Boolean Prepend) 
{
  Standard_Integer NbPoints=0;
  if( (SP.E1()>=0)&&(SP.E2()==-2) ) {
    //case if the point is on edge of T1
    Standard_Integer NextTriangle1;
    if (TEdges1[SP.E1()].FirstTriangle()!=SP.T1()) NextTriangle1=TEdges1[SP.E1()].FirstTriangle();
    else 
      NextTriangle1=TEdges1[SP.E1()].SecondTriangle();
    //If is checked if two triangles intersect
    Standard_Real Angle= -2.0;
    if (CheckCoupleAndGetAngle(NextTriangle1,SP.T2(),Angle,TTrianglesContacts)) {
      NbPoints=NextStartingPointsResearch(NextTriangle1,SP.T2(),SP,SPNext);
      if( NbPoints!=1 ) {
        if (NbPoints>1)
          CheckNextStartPoint(MySectionLine,TTangentZones,SPNext,Prepend);
        else {

        NbPoints=0;
        }
      }
      else 
        SPNext.SetAngle(Angle);
    }
    else NbPoints=0;//this couple does not intersect
  }
  else if( (SP.E1()==-2)&&(SP.E2()>=0) ) {
    //case if the point is on edge of T2
    Standard_Integer NextTriangle2;
    if (TEdges2[SP.E2()].FirstTriangle()!=SP.T2()) NextTriangle2=TEdges2[SP.E2()].FirstTriangle();
    else 
      NextTriangle2=TEdges2[SP.E2()].SecondTriangle();
    Standard_Real Angle= -2.0;
    if (CheckCoupleAndGetAngle(SP.T1(),NextTriangle2,Angle,TTrianglesContacts)) {
      NbPoints=NextStartingPointsResearch(SP.T1(),NextTriangle2,SP,SPNext);
      if( NbPoints!=1 ) {
        if (NbPoints>1)
          CheckNextStartPoint(MySectionLine,TTangentZones,SPNext,Prepend);
        else {

        NbPoints=0;
        }
      }
      else
        SPNext.SetAngle(Angle);
    }
    else NbPoints=0;
  }
  else if( (SP.E1()==-2)&&(SP.E2()==-2) ) { 
    ///no edge is touched or cut

    NbPoints=0;
  }
  else if( (SP.E1()>=0)&&(SP.E2()>=0) ) {
    ///the point is located on two edges
      Standard_Integer NextTriangle1;
      if (TEdges1[SP.E1()].FirstTriangle()!=SP.T1()) NextTriangle1=TEdges1[SP.E1()].FirstTriangle();
      else 
        NextTriangle1=TEdges1[SP.E1()].SecondTriangle();
      Standard_Integer NextTriangle2;
      if (TEdges2[SP.E2()].FirstTriangle()!=SP.T2()) NextTriangle2=TEdges2[SP.E2()].FirstTriangle();
      else 
        NextTriangle2=TEdges2[SP.E2()].SecondTriangle();
      Standard_Real Angle= -2.0;

      IntPolyh_ListIteratorOfListOfCouples aItCT11, aItCT22;
      if (CheckCoupleAndGetAngle2(NextTriangle1,NextTriangle2,
                                  SP.T1(),SP.T2(), aItCT11, aItCT22,
                                  Angle,TTrianglesContacts)) {
        NbPoints=NextStartingPointsResearch(NextTriangle1,NextTriangle2,SP,SPNext);
        if( NbPoints!=1 ) {
          if (NbPoints>1) {
            ///The new point is checked
            if(CheckNextStartPoint(MySectionLine,TTangentZones,SPNext,Prepend)>0) {
            }
            else {

            }
          }
          NbPoints=0;
        }
        else {//NbPoints==1
          SPNext.SetAngle(Angle);
          if (aItCT11.More()) aItCT11.ChangeValue().SetAnalyzed(Standard_True);
          if (aItCT22.More()) aItCT22.ChangeValue().SetAnalyzed(Standard_True);
        }
      }
      else NbPoints=0;
    }
  else if( (SP.E1()==-1)||(SP.E2()==-1) ) {
    ///the points are tops of triangle
    ///the point is atored in an intermediary array
  }
  return(NbPoints);
}
//=======================================================================
//function : GetArrayOfPoints
//purpose  : 
//=======================================================================
const IntPolyh_ArrayOfPoints& IntPolyh_MaillageAffinage::GetArrayOfPoints
  (const Standard_Integer SurfID)const
{
 if (SurfID==1)
 return(TPoints1);
 return(TPoints2);
}
//=======================================================================
//function : GetArrayOfEdges
//purpose  : 
//=======================================================================
const IntPolyh_ArrayOfEdges& IntPolyh_MaillageAffinage::GetArrayOfEdges
  (const Standard_Integer SurfID)const
{
 if (SurfID==1)
 return(TEdges1);
 return(TEdges2);
}
//=======================================================================
//function : GetArrayOfTriangles
//purpose  : 
//=======================================================================
const IntPolyh_ArrayOfTriangles& 
  IntPolyh_MaillageAffinage::GetArrayOfTriangles
  (const Standard_Integer SurfID)const{
  if (SurfID==1)
  return(TTriangles1);
  return(TTriangles2);
}

//=======================================================================
//function : GetBox
//purpose  : 
//=======================================================================
Bnd_Box IntPolyh_MaillageAffinage::GetBox(const Standard_Integer SurfID) const
{
  if (SurfID==1)
  return(MyBox1);
  return(MyBox2);
}
//=======================================================================
//function : GetArrayOfCouples
//purpose  : 
//=======================================================================
IntPolyh_ListOfCouples &IntPolyh_MaillageAffinage::GetCouples()
{
  return TTrianglesContacts;
}
//=======================================================================
//function : SetEnlargeZone
//purpose  : 
//=======================================================================
void IntPolyh_MaillageAffinage::SetEnlargeZone(Standard_Boolean& EnlargeZone)
{
  myEnlargeZone = EnlargeZone;
}
//=======================================================================
//function : GetEnlargeZone
//purpose  : 
//=======================================================================
Standard_Boolean IntPolyh_MaillageAffinage::GetEnlargeZone() const
{
  return myEnlargeZone;
}

//=======================================================================
//function : GetMinDeflection
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_MaillageAffinage::GetMinDeflection(const Standard_Integer SurfID) const
{
  return (SurfID==1)? FlecheMin1:FlecheMin2;
}

//=======================================================================
//function : GetMaxDeflection
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_MaillageAffinage::GetMaxDeflection(const Standard_Integer SurfID) const
{
  return (SurfID==1)? FlecheMax1:FlecheMax2;
}

//=======================================================================
//function : DegeneratedIndex
//purpose  : 
//=======================================================================
void DegeneratedIndex(const TColStd_Array1OfReal& aXpars,
                      const Standard_Integer aNbX,
                      const Handle(Adaptor3d_HSurface)& aS,
                      const Standard_Integer aIsoDirection,
                      Standard_Integer& aI1,
                      Standard_Integer& aI2)
{
  Standard_Integer i;
  Standard_Boolean bDegX1, bDegX2;
  Standard_Real aDegX1, aDegX2, aTol2, aX;
  //
  aI1=0;
  aI2=0;
  aTol2=MyTolerance*MyTolerance;
  //
  if (aIsoDirection==1){ // V=const
    bDegX1=IsDegenerated(aS, 1, aTol2, aDegX1);
    bDegX2=IsDegenerated(aS, 2, aTol2, aDegX2);
  }
  else if (aIsoDirection==2){ // U=const
    bDegX1=IsDegenerated(aS, 3, aTol2, aDegX1);
    bDegX2=IsDegenerated(aS, 4, aTol2, aDegX2);
  }
  else {
    return;
  }
  //
  if (!(bDegX1 || bDegX2)) {
    return;
  }
  //  
  for(i=1; i<=aNbX; ++i) {
    aX=aXpars(i);
    if (bDegX1) {
      if (fabs(aX-aDegX1) < MyTolerance) {
        aI1=i;
      }
    }
    if (bDegX2) {
      if (fabs(aX-aDegX2) < MyTolerance) {
        aI2=i;
      }
    }
  }
}
//=======================================================================
//function : IsDegenerated
//purpose  : 
//=======================================================================
Standard_Boolean IsDegenerated(const Handle(Adaptor3d_HSurface)& aS,
                               const Standard_Integer aIndex,
                               const Standard_Real aTol2,
                               Standard_Real& aDegX)
{
  Standard_Boolean bRet;
  Standard_Integer i, aNbP;
  Standard_Real aU, dU, aU1, aU2, aV, dV, aV1, aV2, aD2;
  gp_Pnt aP1, aP2;
  //
  bRet=Standard_False;
  aNbP=3;
  aDegX=99;
  //
  aU1=aS->FirstUParameter();
  aU2=aS->LastUParameter();
  aV1=aS->FirstVParameter();
  aV2=aS->LastVParameter();
  //
  if (aIndex<3) { // V=const
    aV=aV1;
    if (aIndex==2) {
      aV=aV2;
    }
    dU=(aU2-aU1)/(aNbP-1);
    aU=aU1;
    aP1=aS->Value(aU, aV);
    for (i=1; i<aNbP; ++i) {
      aU=i*dU;
      if (i==aNbP-1){
        aU=aU2;
      }
      aP2=aS->Value(aU, aV);
      aD2=aP1.SquareDistance(aP2);
      if (aD2>aTol2) {
        return bRet;
      }
      aP1=aP2;
    }
    aDegX=aV;
    bRet=!bRet;
  }
  else {// U=const
    aU=aU1;
    if (aIndex==4) {
      aU=aU2;
    }
    dV=(aV2-aV1)/(aNbP-1);
    aV=aV1;
    aP1=aS->Value(aU, aV);
    for (i=1; i<aNbP; ++i) {
      aV=i*dV;
      if (i==aNbP-1){
        aV=aV2;
      }
      aP2=aS->Value(aU, aV);
      aD2=aP1.SquareDistance(aP2);
      if (aD2>aTol2) {
        return bRet;
      }
      aP1=aP2;
    }
    bRet=!bRet;
    aDegX=aU;
  }
  //
  return bRet;
}
//=======================================================================
//function : EnlargeZone
//purpose  : 
//=======================================================================
void EnlargeZone(const Handle(Adaptor3d_HSurface)& MaSurface,
                 Standard_Real &u0, 
                 Standard_Real &u1, 
                 Standard_Real &v0, 
                 Standard_Real &v1) 
{
  if(MaSurface->GetType() == GeomAbs_BSplineSurface ||
     MaSurface->GetType() == GeomAbs_BezierSurface) {
    if((!MaSurface->IsUClosed() && !MaSurface->IsUPeriodic()) &&
       (Abs(u0) < 1.e+100 && Abs(u1) < 1.e+100) ) {
      Standard_Real delta_u = 0.01*Abs(u1 - u0);
      u0 -= delta_u;
      u1 += delta_u;
    }
    if((!MaSurface->IsVClosed() && !MaSurface->IsVPeriodic()) &&
       (Abs(v0) < 1.e+100 && Abs(v1) < 1.e+100) ) {
      Standard_Real delta_v = 0.01*Abs(v1 - v0);
      v0 -= delta_v;
      v1 += delta_v;
    }
  }
}
