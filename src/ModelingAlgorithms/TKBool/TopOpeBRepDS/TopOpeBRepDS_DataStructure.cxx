// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_PointData.hxx>
#include <TopOpeBRepDS_ShapeData.hxx>
#include <TopOpeBRepDS_ShapeWithState.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceData.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

//=================================================================================================

TopOpeBRepDS_DataStructure::TopOpeBRepDS_DataStructure()
    : myNbSurfaces(0),
      myNbCurves(0),
      myNbPoints(0),
      myIsfafa(Standard_False),
      myI(0)
{
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::Init()
{
  myNbSurfaces = 0;
  myNbCurves   = 0;
  myNbPoints   = 0;
  mySurfaces.Clear();
  myCurves.Clear();
  myPoints.Clear();
  myShapes.Clear();
  // Begin modified by NIZHNY-MZV  Tue Apr 18 16:33:26 2000
  myMapOfShapeWithStateObj.Clear();
  myMapOfShapeWithStateTool.Clear();
  myMapOfRejectedShapesObj.Clear();
  myMapOfRejectedShapesTool.Clear();
  // End modified by NIZHNY-MZV  Tue Apr 18 16:33:32 2000
  InitSectionEdges();
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::AddSurface(const TopOpeBRepDS_Surface& S)
{
  myNbSurfaces++;
  // modified by NIZNHY-PKV Tue Oct 30 09:22:04 2001 f
  TopOpeBRepDS_SurfaceData aSD(S);
  mySurfaces.Bind(myNbSurfaces, aSD);
  // mySurfaces.Bind(myNbSurfaces,S);
  // modified by NIZNHY-PKV Tue Oct 30 09:22:20 2001 t
  return myNbSurfaces;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::RemoveSurface(const Standard_Integer I)
{
  mySurfaces.UnBind(I);
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::KeepSurface(const Standard_Integer I) const
{
  const TopOpeBRepDS_Surface& S = Surface(I);
  Standard_Boolean            b = S.Keep();
  return b;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::KeepSurface(TopOpeBRepDS_Surface& S) const
{
  Standard_Boolean b = S.Keep();
  return b;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::ChangeKeepSurface(const Standard_Integer I,
                                                   const Standard_Boolean FindKeep)
{
  TopOpeBRepDS_Surface& S = ChangeSurface(I);
  S.ChangeKeep(FindKeep);
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::ChangeKeepSurface(TopOpeBRepDS_Surface&  S,
                                                   const Standard_Boolean FindKeep)
{
  S.ChangeKeep(FindKeep);
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::AddCurve(const TopOpeBRepDS_Curve& C)
{
  myNbCurves++;

  // NYI : modifier const & sur Curve dans le CDL NYI
  TopOpeBRepDS_Curve* PC = (TopOpeBRepDS_Curve*)(void*)&C;
  PC->ChangeDSIndex(myNbCurves);

  TopOpeBRepDS_CurveData CD(C);
  myCurves.Bind(myNbCurves, CD);

  return myNbCurves;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::RemoveCurve(const Standard_Integer I)
{
  TopOpeBRepDS_Curve& C = ChangeCurve(I);

  TopoDS_Shape S1, S2;
  C.GetShapes(S1, S2);
  Handle(TopOpeBRepDS_Interference) I1, I2;
  C.GetSCI(I1, I2);
  if (!I1.IsNull())
    RemoveShapeInterference(S1, I1);
  if (!I2.IsNull())
    RemoveShapeInterference(S2, I2);
  C.ChangeKeep(Standard_False);

  // NYI : nullify interferences I1,I2
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::KeepCurve(const Standard_Integer I) const
{
  const TopOpeBRepDS_Curve& C = Curve(I);
  Standard_Boolean          b = C.Keep();
  return b;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::KeepCurve(const TopOpeBRepDS_Curve& C) const
{
  Standard_Boolean b = C.Keep();
  return b;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::ChangeKeepCurve(const Standard_Integer I,
                                                 const Standard_Boolean FindKeep)
{
  TopOpeBRepDS_Curve& C = ChangeCurve(I);
  C.ChangeKeep(FindKeep);
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::ChangeKeepCurve(TopOpeBRepDS_Curve&    C,
                                                 const Standard_Boolean FindKeep)
{
  C.ChangeKeep(FindKeep);
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::AddPoint(const TopOpeBRepDS_Point& PDS)
{
  myNbPoints++;
  myPoints.Bind(myNbPoints, PDS);
  return myNbPoints;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::AddPointSS(const TopOpeBRepDS_Point& PDS,
                                                        const TopoDS_Shape& /*S1*/,
                                                        const TopoDS_Shape& /*S2*/)
{
  Standard_Integer i = AddPoint(PDS);
  return i;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::RemovePoint(const Standard_Integer I)
{
  TopOpeBRepDS_Point& P = ChangePoint(I);
  P.ChangeKeep(Standard_False);
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::KeepPoint(const Standard_Integer I) const
{
  const TopOpeBRepDS_Point& P = Point(I);
  Standard_Boolean          b = P.Keep();
  return b;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::KeepPoint(const TopOpeBRepDS_Point& P) const
{
  Standard_Boolean b = P.Keep();
  return b;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::ChangeKeepPoint(const Standard_Integer I,
                                                 const Standard_Boolean FindKeep)
{
  TopOpeBRepDS_Point& P = ChangePoint(I);
  P.ChangeKeep(FindKeep);
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::ChangeKeepPoint(TopOpeBRepDS_Point&    P,
                                                 const Standard_Boolean FindKeep)
{
  P.ChangeKeep(FindKeep);
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::AddShape(const TopoDS_Shape& S)
{
  Standard_Integer iS = myShapes.FindIndex(S);
  if (iS == 0)
  {
    TopOpeBRepDS_ShapeData SD;
    iS = myShapes.Add(S, SD);
    // a shape is its own reference, oriented as itself
    SameDomainRef(iS, iS);
    SameDomainOri(iS, TopOpeBRepDS_SAMEORIENTED);
  }
  return iS;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::AddShape(const TopoDS_Shape&    S,
                                                      const Standard_Integer Ianc)
{
  Standard_Integer iS = myShapes.FindIndex(S);
  if (iS == 0)
  {
    TopOpeBRepDS_ShapeData SD;
    iS = myShapes.Add(S, SD);
    // a shape is its own reference, oriented as itself
    SameDomainRef(iS, iS);
    SameDomainOri(iS, TopOpeBRepDS_SAMEORIENTED);
    AncestorRank(iS, Ianc);
  }
  return iS;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::KeepShape(const Standard_Integer I,
                                                       const Standard_Boolean FindKeep) const
{
  Standard_Boolean b = Standard_False;
  if (I >= 1 && I <= myShapes.Extent())
  {
    const TopoDS_Shape& S = myShapes.FindKey(I);
    if (FindKeep)
      b = KeepShape(S);
    else
      b = Standard_True;
  }
  return b;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::KeepShape(const TopoDS_Shape&    S,
                                                       const Standard_Boolean FindKeep) const
{
  Standard_Boolean b = Standard_False;
  if ((!myShapes.IsEmpty()) && (!S.IsNull()) && myShapes.Contains(S))
  {
    const TopOpeBRepDS_ShapeData& SD = myShapes.FindFromKey(S);
    if (FindKeep)
      b = SD.Keep();
    else
      b = Standard_True;
  }
  return b;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::ChangeKeepShape(const Standard_Integer I,
                                                 const Standard_Boolean FindKeep)
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    const TopoDS_Shape& S = myShapes.FindKey(I);
    ChangeKeepShape(S, FindKeep);
  }
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::ChangeKeepShape(const TopoDS_Shape&    S,
                                                 const Standard_Boolean FindKeep)
{
  if (!S.IsNull() && myShapes.Contains(S))
  {
    TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromKey(S);
    SD.ChangeKeep(FindKeep);
  }
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::InitSectionEdges()
{
  mySectionEdges.Clear();
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::AddSectionEdge(const TopoDS_Edge& E)
{
  Standard_Integer iE = mySectionEdges.FindIndex(E);
  if (iE == 0)
    iE = mySectionEdges.Add(E);
  return iE;
}

//=================================================================================================

TopOpeBRepDS_MapOfShapeData& TopOpeBRepDS_DataStructure::ChangeShapes()
{
  return myShapes;
}

//=================================================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::SurfaceInterferences(
  const Standard_Integer I) const
{

  if (!mySurfaces.IsBound(I))
  {
    return myEmptyListOfInterference;
  }
  const TopOpeBRepDS_SurfaceData&        SD = mySurfaces.Find(I);
  const TopOpeBRepDS_ListOfInterference& LI = SD.Interferences();
  return LI;
}

//=================================================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::ChangeSurfaceInterferences(
  const Standard_Integer I)
{

  if (!mySurfaces.IsBound(I))
  {
    return myEmptyListOfInterference;
  }
  TopOpeBRepDS_SurfaceData&        SD = mySurfaces.ChangeFind(I);
  TopOpeBRepDS_ListOfInterference& LI = SD.ChangeInterferences();
  return LI;
}

//=================================================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::CurveInterferences(
  const Standard_Integer I) const
{

  if (!myCurves.IsBound(I))
  {
    return myEmptyListOfInterference;
  }
  const TopOpeBRepDS_CurveData&          CD = myCurves.Find(I);
  const TopOpeBRepDS_ListOfInterference& LI = CD.Interferences();
  return LI;
}

//=================================================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::ChangeCurveInterferences(
  const Standard_Integer I)
{

  if (!myCurves.IsBound(I))
  {
    return myEmptyListOfInterference;
  }
  TopOpeBRepDS_CurveData&          CD = myCurves.ChangeFind(I);
  TopOpeBRepDS_ListOfInterference& LI = CD.ChangeInterferences();
  return LI;
}

//=================================================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::PointInterferences(
  const Standard_Integer I) const
{

  if (!myPoints.IsBound(I))
  {
    return myEmptyListOfInterference;
  }
  const TopOpeBRepDS_PointData&          PD = myPoints.Find(I);
  const TopOpeBRepDS_ListOfInterference& LI = PD.Interferences();
  return LI;
}

//=================================================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::ChangePointInterferences(
  const Standard_Integer I)
{

  if (!myPoints.IsBound(I))
  {
    return myEmptyListOfInterference;
  }
  TopOpeBRepDS_PointData&          PD = myPoints.ChangeFind(I);
  TopOpeBRepDS_ListOfInterference& LI = PD.ChangeInterferences();
  return LI;
}

//=================================================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::ShapeInterferences(
  const TopoDS_Shape&    S,
  const Standard_Boolean FindKeep) const
{
  if (HasShape(S, FindKeep))
    return myShapes.FindFromKey(S).myInterferences;
  return myEmptyListOfInterference;
}

//=================================================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::ChangeShapeInterferences(
  const TopoDS_Shape& S)
{
  if (!HasShape(S))
    return myEmptyListOfInterference;
  TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromKey(S);
  return SD.myInterferences;
}

//=================================================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::ShapeInterferences(
  const Standard_Integer I,
  const Standard_Boolean FindKeep) const
{
  if (FindKeep && !KeepShape(I))
    return myEmptyListOfInterference;
  return myShapes.FindFromIndex(I).myInterferences;
}

//=================================================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_DataStructure::ChangeShapeInterferences(
  const Standard_Integer I)
{
  TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromIndex(I);
  return SD.myInterferences;
}

//=================================================================================================

const TopTools_ListOfShape& TopOpeBRepDS_DataStructure::ShapeSameDomain(const TopoDS_Shape& S) const
{
  if (!S.IsNull())
    if (myShapes.Contains(S))
    {
      const TopOpeBRepDS_ShapeData& SD = myShapes.FindFromKey(S);
      const TopTools_ListOfShape&   l  = SD.mySameDomain;
      return l;
    }
  return myEmptyListOfShape;
}

//=================================================================================================

TopTools_ListOfShape& TopOpeBRepDS_DataStructure::ChangeShapeSameDomain(const TopoDS_Shape& S)
{
  TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromKey(S);
  return SD.mySameDomain;
}

//=================================================================================================

const TopTools_ListOfShape& TopOpeBRepDS_DataStructure::ShapeSameDomain(
  const Standard_Integer I) const
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    const TopOpeBRepDS_ShapeData& SD = myShapes.FindFromIndex(I);
    const TopTools_ListOfShape&   l  = SD.mySameDomain;
    return l;
  }
  else
  {
    return myEmptyListOfShape;
  }
}

//=================================================================================================

TopTools_ListOfShape& TopOpeBRepDS_DataStructure::ChangeShapeSameDomain(const Standard_Integer I)
{
  TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromIndex(I);
  return SD.mySameDomain;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::AddShapeSameDomain(const TopoDS_Shape& S, const TopoDS_Shape& SSD)
{
  Standard_Boolean append = Standard_True;
  {
    TopTools_ListIteratorOfListOfShape it(ShapeSameDomain(S));
    for (; it.More(); it.Next())
    {
      const TopoDS_Shape& itS = it.Value();
      if (itS.IsSame(SSD))
      {
        append = Standard_False;
        break;
      }
    }
  }
  if (append)
  {
    ChangeShapeSameDomain(S).Append(SSD);
  }
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::RemoveShapeSameDomain(const TopoDS_Shape& S,
                                                       const TopoDS_Shape& SSD)
{
  TopTools_ListOfShape&              L = ChangeShapeSameDomain(S);
  TopTools_ListIteratorOfListOfShape it(L);
  while (it.More())
  {
    const TopoDS_Shape& itS    = it.Value();
    Standard_Boolean    remove = itS.IsSame(SSD);
    if (remove)
      L.Remove(it);
    else
      it.Next();
  }
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::SameDomainRef(const Standard_Integer I) const
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    return myShapes.FindFromIndex(I).mySameDomainRef;
  }
  return 0;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::SameDomainRef(const TopoDS_Shape& S) const
{
  if (S.IsNull())
    return 0;
  if (myShapes.Contains(S))
  {
    return myShapes.FindFromKey(S).mySameDomainRef;
  }
  return 0;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::SameDomainRef(const Standard_Integer I, const Standard_Integer Ref)
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromIndex(I);
    SD.mySameDomainRef         = Ref;
  }
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::SameDomainRef(const TopoDS_Shape& S, const Standard_Integer Ref)
{
  if (S.IsNull())
    return;
  if (myShapes.Contains(S))
  {
    TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromKey(S);
    SD.mySameDomainRef         = Ref;
  }
}

//=================================================================================================

TopOpeBRepDS_Config TopOpeBRepDS_DataStructure::SameDomainOri(const Standard_Integer I) const
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    return myShapes.FindFromIndex(I).mySameDomainOri;
  }
  return TopOpeBRepDS_UNSHGEOMETRY;
}

//=================================================================================================

TopOpeBRepDS_Config TopOpeBRepDS_DataStructure::SameDomainOri(const TopoDS_Shape& S) const
{
  if (!S.IsNull())
    if (myShapes.Contains(S))
    {
      return myShapes.FindFromKey(S).mySameDomainOri;
    }
  return TopOpeBRepDS_UNSHGEOMETRY;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::SameDomainOri(const Standard_Integer    I,
                                               const TopOpeBRepDS_Config Ori)
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromIndex(I);
    SD.mySameDomainOri         = Ori;
  }
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::SameDomainOri(const TopoDS_Shape& S, const TopOpeBRepDS_Config Ori)
{
  if (!S.IsNull())
    if (myShapes.Contains(S))
    {
      TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromKey(S);
      SD.mySameDomainOri         = Ori;
    }
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::SameDomainInd(const Standard_Integer I) const
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    return myShapes.FindFromIndex(I).mySameDomainInd;
  }
  return 0;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::SameDomainInd(const TopoDS_Shape& S) const
{
  if (!S.IsNull())
    if (myShapes.Contains(S))
    {
      return myShapes.FindFromKey(S).mySameDomainInd;
    }
  return 0;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::SameDomainInd(const Standard_Integer I, const Standard_Integer Ind)
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromIndex(I);
    SD.mySameDomainInd         = Ind;
  }
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::SameDomainInd(const TopoDS_Shape& S, const Standard_Integer Ind)
{
  if (S.IsNull())
    return;
  if (myShapes.Contains(S))
  {
    TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromKey(S);
    SD.mySameDomainInd         = Ind;
  }
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::AncestorRank(const Standard_Integer I) const
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    return myShapes.FindFromIndex(I).myAncestorRank;
  }
  return 0;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::AncestorRank(const TopoDS_Shape& S) const
{
  if (S.IsNull())
    return 0;
  if (myShapes.Contains(S))
  {
    return myShapes.FindFromKey(S).myAncestorRank;
  }
  return 0;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::AncestorRank(const Standard_Integer I, const Standard_Integer Ianc)
{
  if (I >= 1 && I <= myShapes.Extent())
  {
    TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromIndex(I);
    SD.myAncestorRank          = Ianc;
  }
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::AncestorRank(const TopoDS_Shape& S, const Standard_Integer Ianc)
{
  if (S.IsNull())
    return;
  if (myShapes.Contains(S))
  {
    TopOpeBRepDS_ShapeData& SD = myShapes.ChangeFromKey(S);
    SD.myAncestorRank          = Ianc;
  }
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::AddShapeInterference(const TopoDS_Shape&                      S,
                                                      const Handle(TopOpeBRepDS_Interference)& I)
{
  ChangeShapeInterferences(S).Append(I);
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::RemoveShapeInterference(const TopoDS_Shape&                      S,
                                                         const Handle(TopOpeBRepDS_Interference)& I)
{
  TopOpeBRepDS_ListOfInterference&              L = ChangeShapeInterferences(S);
  TopOpeBRepDS_ListIteratorOfListOfInterference it(L);
  Standard_Boolean                              b = FindInterference(it, I);
  if (b)
  {
    L.Remove(it);
  }
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::FillShapesSameDomain(const TopoDS_Shape&    S1,
                                                      const TopoDS_Shape&    S2,
                                                      const Standard_Boolean refFirst)
{
  TopAbs_Orientation o1 = S1.Orientation();
  TopAbs_Orientation o2 = S2.Orientation();

  Standard_Integer        iS1    = AddShape(S1, 1);
  TopOpeBRepDS_ShapeData& SD1    = myShapes.ChangeFromIndex(iS1);
  Standard_Boolean        isdef1 = SD1.myOrientationDef;
  Standard_Boolean        todef1 = (!isdef1);
  if (isdef1 && SD1.mySameDomainOri == TopOpeBRepDS_UNSHGEOMETRY)
    todef1 = Standard_True;
  if (todef1)
  {
    SD1.myOrientation    = o1;
    SD1.myOrientationDef = Standard_True;
  }

  Standard_Integer        iS2    = AddShape(S2, 2);
  TopOpeBRepDS_ShapeData& SD2    = myShapes.ChangeFromIndex(iS2);
  Standard_Boolean        isdef2 = SD2.myOrientationDef;
  Standard_Boolean        todef2 = (!isdef2);
  if (isdef2 && SD2.mySameDomainOri == TopOpeBRepDS_UNSHGEOMETRY)
    todef2 = Standard_True;
  if (todef2)
  {
    SD2.myOrientation    = o2;
    SD2.myOrientationDef = Standard_True;
  }

  Standard_Integer n1 = ShapeSameDomain(S1).Extent();
  Standard_Integer n2 = ShapeSameDomain(S2).Extent();

  AddShapeSameDomain(S1, S2);
  AddShapeSameDomain(S2, S1);

  Standard_Integer n11 = ShapeSameDomain(S1).Extent();
  Standard_Integer n22 = ShapeSameDomain(S2).Extent();

  Standard_Boolean cond = (n11 == n1) && (n22 == n2);
  cond                  = cond && (!todef1) && (!todef2);
  if (cond)
  {
    // nothing changed in SameDomain data of S1 and S2 : return
    return;
  }

  Standard_Integer r1 = SameDomainRef(S1);
  Standard_Integer r2 = SameDomainRef(S2);
  Standard_Integer r  = 0;
  // r1 == i1  r2 == i2 : shapes have no SameDomain ref : take S1 as reference
  // r1 == i1  r2 != i2 : S2 has a SameDomain reference : give it to S1
  // r1 != i1  r2 == i2 : S1 has a SameDomain reference : give it to S2
  // r1 != i1  r2 != i2 : S1,S2 have SameDomain reference : check equality
  if (r1 == iS1 && r2 == iS2)
    r = (refFirst ? iS1 : iS2);
  else if (r1 == iS1 && r2 != iS2)
    r = r2;
  else if (r1 != iS1 && r2 == iS2)
    r = r1;
  else if (r1 != iS1 && r2 != iS2)
  {
    if (r1 != r2)
    {
      //      throw Standard_ProgramError("FacesFiller::Insert SD 1");
    }
    r = (refFirst ? r1 : r2);
  }

  if (r == 0)
  {
    throw Standard_ProgramError("FacesFiller::Insert SD 2");
  }

  TopoDS_Shape                  Sr  = Shape(r);
  const TopOpeBRepDS_ShapeData& SD  = myShapes.FindFromKey(Sr);
  TopAbs_Orientation            oSr = SD.myOrientation;
  Sr.Orientation(oSr);

  if (r != r1 || todef1)
  { // S1 gets a new reference r
    TopOpeBRepDS_Config o = TopOpeBRepDS_SAMEORIENTED;
    if (r != iS1 || todef1)
    {
      Standard_Boolean sso = TopOpeBRepTool_ShapeTool::ShapesSameOriented(S1, Sr);
      if (!sso)
        o = TopOpeBRepDS_DIFFORIENTED;
    }
    SameDomainRef(iS1, r);
    SameDomainOri(iS1, o);
  }

  if (r != r2 || todef2)
  { // S2 gets a new reference r
    TopOpeBRepDS_Config o = TopOpeBRepDS_SAMEORIENTED;
    if (r != iS2 || todef2)
    {
      Standard_Boolean sso = TopOpeBRepTool_ShapeTool::ShapesSameOriented(S2, Sr);
      if (!sso)
        o = TopOpeBRepDS_DIFFORIENTED;
    }
    SameDomainRef(iS2, r);
    SameDomainOri(iS2, o);
  }

  // index
  SameDomainInd(S1, 1);
  SameDomainInd(S2, 2);
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::FillShapesSameDomain(const TopoDS_Shape&       S1,
                                                      const TopoDS_Shape&       S2,
                                                      const TopOpeBRepDS_Config c1,
                                                      const TopOpeBRepDS_Config c2,
                                                      const Standard_Boolean    refFirst)
{
  Standard_Integer        iS1    = AddShape(S1, 1);
  TopOpeBRepDS_ShapeData& SD1    = myShapes.ChangeFromIndex(iS1);
  Standard_Boolean        isdef1 = SD1.myOrientationDef;
  Standard_Boolean        todef1 = Standard_True;
  if (c1 == TopOpeBRepDS_UNSHGEOMETRY && isdef1)
    todef1 = Standard_False;

  Standard_Integer        iS2    = AddShape(S2, 2);
  TopOpeBRepDS_ShapeData& SD2    = myShapes.ChangeFromIndex(iS2);
  Standard_Boolean        isdef2 = SD2.myOrientationDef;
  Standard_Boolean        todef2 = Standard_True;
  if (c2 == TopOpeBRepDS_UNSHGEOMETRY && isdef2)
    todef2 = Standard_False;

  if (todef1 || todef2)
  {
    FillShapesSameDomain(S1, S2, refFirst);
  }

  if (todef1 && c1 == TopOpeBRepDS_UNSHGEOMETRY)
    SameDomainOri(S1, TopOpeBRepDS_UNSHGEOMETRY);
  if (todef2 && c2 == TopOpeBRepDS_UNSHGEOMETRY)
    SameDomainOri(S2, TopOpeBRepDS_UNSHGEOMETRY);
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::UnfillShapesSameDomain(const TopoDS_Shape& S1,
                                                        const TopoDS_Shape& S2)
{
  RemoveShapeSameDomain(S1, S2);
  RemoveShapeSameDomain(S2, S1);
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::NbSurfaces() const
{
  Standard_Integer n = myNbSurfaces;
  return n;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::NbCurves() const
{
  Standard_Integer n = myNbCurves;
  return n;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::ChangeNbCurves(const Standard_Integer n)
{
  myNbCurves = n;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::NbPoints() const
{
  Standard_Integer n = myNbPoints;
  return n;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::NbShapes() const
{
  Standard_Integer n = myShapes.Extent();
  return n;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::NbSectionEdges() const
{
  Standard_Integer n = mySectionEdges.Extent();
  return n;
}

//=================================================================================================

const TopOpeBRepDS_Surface& TopOpeBRepDS_DataStructure::Surface(const Standard_Integer I) const
{
  if (mySurfaces.IsBound(I))
    return mySurfaces(I).mySurface;
  else
    return myEmptySurface;
}

//=================================================================================================

TopOpeBRepDS_Surface& TopOpeBRepDS_DataStructure::ChangeSurface(const Standard_Integer I)
{
  if (mySurfaces.IsBound(I))
    return mySurfaces(I).mySurface;
  else
    return myEmptySurface;
}

//=================================================================================================

const TopOpeBRepDS_Curve& TopOpeBRepDS_DataStructure::Curve(const Standard_Integer I) const
{
  if (myCurves.IsBound(I))
  {
    const TopOpeBRepDS_CurveData& CD = myCurves(I);
    const TopOpeBRepDS_Curve&     C  = CD.myCurve;
    return C;
  }
  else
    return myEmptyCurve;
}

//=================================================================================================

TopOpeBRepDS_Curve& TopOpeBRepDS_DataStructure::ChangeCurve(const Standard_Integer I)
{
  if (myCurves.IsBound(I))
  {
    TopOpeBRepDS_CurveData& CD = myCurves.ChangeFind(I);
    TopOpeBRepDS_Curve&     C  = CD.myCurve;
    return C;
  }
  return myEmptyCurve;
}

//=================================================================================================

const TopOpeBRepDS_Point& TopOpeBRepDS_DataStructure::Point(const Standard_Integer I) const
{
  if (I < 1 || I > myNbPoints)
  {
    throw Standard_ProgramError("TopOpeBRepDS_DataStructure::Point");
  }

  if (myPoints.IsBound(I))
    return myPoints(I).myPoint;
  else
    return myEmptyPoint;
}

//=================================================================================================

TopOpeBRepDS_Point& TopOpeBRepDS_DataStructure::ChangePoint(const Standard_Integer I)
{
  if (I < 1 || I > myNbPoints)
  {
    throw Standard_ProgramError("TopOpeBRepDS_DataStructure::Point");
  }

  if (myPoints.IsBound(I))
    return myPoints(I).myPoint;
  else
    return myEmptyPoint;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepDS_DataStructure::Shape(const Standard_Integer I,
                                                      const Standard_Boolean FindKeep) const
{
  if (KeepShape(I, FindKeep))
  {
    const TopoDS_Shape& S = myShapes.FindKey(I);
    return S;
  }
  return myEmptyShape;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::Shape(const TopoDS_Shape&    S,
                                                   const Standard_Boolean FindKeep) const
{
  Standard_Integer i  = 0;
  Standard_Boolean hs = HasShape(S, FindKeep);
  if (hs)
  {
    i = myShapes.FindIndex(S);
  }
  return i;
}

//=================================================================================================

const TopoDS_Edge& TopOpeBRepDS_DataStructure::SectionEdge(const Standard_Integer I,
                                                           const Standard_Boolean FindKeep) const
{
  const TopoDS_Shape& S = mySectionEdges.FindKey(I);
  if (HasShape(S, FindKeep))
    return TopoDS::Edge(S);
  return TopoDS::Edge(myEmptyShape);
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_DataStructure::SectionEdge(const TopoDS_Edge&     S,
                                                         const Standard_Boolean FindKeep) const
{
  Standard_Integer i = 0;
  if (KeepShape(S, FindKeep))
    i = mySectionEdges.FindIndex(S);
  return i;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::IsSectionEdge(const TopoDS_Edge&     S,
                                                           const Standard_Boolean FindKeep) const
{
  Standard_Boolean b = KeepShape(S, FindKeep);
  if (b)
  {
    b = mySectionEdges.Contains(S);
  }
  return b;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::HasGeometry(const TopoDS_Shape& S) const
{
  Standard_Boolean has = HasShape(S);
  if (has)
  {
    has = !ShapeInterferences(S).IsEmpty();
  }
  return has;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::HasShape(const TopoDS_Shape&    S,
                                                      const Standard_Boolean FindKeep) const
{
  Standard_Boolean res = KeepShape(S, FindKeep);
  return res;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::SetNewSurface(const TopoDS_Shape& F, const Handle(Geom_Surface)& S)
{
  myNewSurface.Bind(F, S);
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::HasNewSurface(const TopoDS_Shape& F) const
{
  Standard_Boolean b = myNewSurface.IsBound(F);
  return b;
}

//=================================================================================================

const Handle(Geom_Surface)& TopOpeBRepDS_DataStructure::NewSurface(const TopoDS_Shape& F) const
{
  if (HasNewSurface(F))
    return myNewSurface.Find(F);
  return myEmptyGSurface;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::FindInterference(
  TopOpeBRepDS_ListIteratorOfListOfInterference& IT,
  const Handle(TopOpeBRepDS_Interference)&       I) const
{
  for (; IT.More(); IT.Next())
    if (IT.Value() == I)
      return Standard_True;
  return Standard_False;
}

//=================================================================================================

void TopOpeBRepDS_DataStructure::Isfafa(const Standard_Boolean isfafa)
{
  myIsfafa = isfafa;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_DataStructure::Isfafa() const
{
  return myIsfafa;
}

//
//=================================================================================================

TopOpeBRepDS_IndexedDataMapOfShapeWithState& TopOpeBRepDS_DataStructure::
  ChangeMapOfShapeWithStateObj()
{
  return myMapOfShapeWithStateObj;
}

//=================================================================================================

TopOpeBRepDS_IndexedDataMapOfShapeWithState& TopOpeBRepDS_DataStructure::
  ChangeMapOfShapeWithStateTool()
{
  return myMapOfShapeWithStateTool;
}

//=================================================================================================

TopOpeBRepDS_IndexedDataMapOfShapeWithState& TopOpeBRepDS_DataStructure::ChangeMapOfShapeWithState(
  const TopoDS_Shape& aShape,
  Standard_Boolean&   aFlag)
{
  static TopOpeBRepDS_IndexedDataMapOfShapeWithState dummy;
  aFlag = Standard_True;

  if (myMapOfShapeWithStateObj.Contains(aShape))
    return myMapOfShapeWithStateObj;

  if (myMapOfShapeWithStateTool.Contains(aShape))
    return myMapOfShapeWithStateTool;

  aFlag = Standard_False;
  return dummy;
}

//=================================================================================================

const TopOpeBRepDS_ShapeWithState& TopOpeBRepDS_DataStructure::GetShapeWithState(
  const TopoDS_Shape& aShape) const
{
  static TopOpeBRepDS_ShapeWithState dummy;
  if (myMapOfShapeWithStateObj.Contains(aShape))
    return myMapOfShapeWithStateObj.FindFromKey(aShape);
  else if (myMapOfShapeWithStateTool.Contains(aShape))
    return myMapOfShapeWithStateTool.FindFromKey(aShape);

  return dummy;
}

//=================================================================================================

TopTools_IndexedMapOfShape& TopOpeBRepDS_DataStructure::ChangeMapOfRejectedShapesObj()
{
  return myMapOfRejectedShapesObj;
}

//=================================================================================================

TopTools_IndexedMapOfShape& TopOpeBRepDS_DataStructure::ChangeMapOfRejectedShapesTool()
{
  return myMapOfRejectedShapesTool;
}
