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

#ifndef _TopOpeBRepDS_DataStructure_HeaderFile
#define _TopOpeBRepDS_DataStructure_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_MapOfSurface.hxx>
#include <TopOpeBRepDS_MapOfCurve.hxx>
#include <TopOpeBRepDS_MapOfPoint.hxx>
#include <TopOpeBRepDS_MapOfShapeData.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_ShapeSurface.hxx>
#include <TopOpeBRepDS_IndexedDataMapOfShapeWithState.hxx>
#include <TopOpeBRepDS_Config.hxx>
class Geom_Surface;
class TopoDS_Edge;
class TopOpeBRepDS_Interference;
class TopOpeBRepDS_ShapeWithState;

//! The DataStructure stores :
//!
//! New geometries : points, curves, and surfaces.
//! Topological shapes : vertices, edges, faces.
//! The new geometries and the topological shapes have interferences.
class TopOpeBRepDS_DataStructure
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_DataStructure();

  //! reset the data structure
  Standard_EXPORT void Init();

  //! Insert a new surface. Returns the index.
  Standard_EXPORT Standard_Integer AddSurface(const TopOpeBRepDS_Surface& S);

  Standard_EXPORT void RemoveSurface(const Standard_Integer I);

  Standard_EXPORT Standard_Boolean KeepSurface(const Standard_Integer I) const;

  Standard_EXPORT Standard_Boolean KeepSurface(TopOpeBRepDS_Surface& S) const;

  Standard_EXPORT void ChangeKeepSurface(const Standard_Integer I, const Standard_Boolean FindKeep);

  Standard_EXPORT void ChangeKeepSurface(TopOpeBRepDS_Surface& S, const Standard_Boolean FindKeep);

  //! Insert a new curve. Returns the index.
  Standard_EXPORT Standard_Integer AddCurve(const TopOpeBRepDS_Curve& S);

  Standard_EXPORT void RemoveCurve(const Standard_Integer I);

  Standard_EXPORT Standard_Boolean KeepCurve(const Standard_Integer I) const;

  Standard_EXPORT Standard_Boolean KeepCurve(const TopOpeBRepDS_Curve& C) const;

  Standard_EXPORT void ChangeKeepCurve(const Standard_Integer I, const Standard_Boolean FindKeep);

  Standard_EXPORT void ChangeKeepCurve(TopOpeBRepDS_Curve& C, const Standard_Boolean FindKeep);

  //! Insert a new point. Returns the index.
  Standard_EXPORT Standard_Integer AddPoint(const TopOpeBRepDS_Point& PDS);

  //! Insert a new point. Returns the index.
  Standard_EXPORT Standard_Integer AddPointSS(const TopOpeBRepDS_Point& PDS,
                                              const TopoDS_Shape&       S1,
                                              const TopoDS_Shape&       S2);

  Standard_EXPORT void RemovePoint(const Standard_Integer I);

  Standard_EXPORT Standard_Boolean KeepPoint(const Standard_Integer I) const;

  Standard_EXPORT Standard_Boolean KeepPoint(const TopOpeBRepDS_Point& P) const;

  Standard_EXPORT void ChangeKeepPoint(const Standard_Integer I, const Standard_Boolean FindKeep);

  Standard_EXPORT void ChangeKeepPoint(TopOpeBRepDS_Point& P, const Standard_Boolean FindKeep);

  //! Insert a shape S. Returns the index.
  Standard_EXPORT Standard_Integer AddShape(const TopoDS_Shape& S);

  //! Insert a shape S which ancestor is I = 1 or 2. Returns the index.
  Standard_EXPORT Standard_Integer AddShape(const TopoDS_Shape& S, const Standard_Integer I);

  Standard_EXPORT Standard_Boolean KeepShape(const Standard_Integer I,
                                             const Standard_Boolean FindKeep = Standard_True) const;

  Standard_EXPORT Standard_Boolean KeepShape(const TopoDS_Shape&    S,
                                             const Standard_Boolean FindKeep = Standard_True) const;

  Standard_EXPORT void ChangeKeepShape(const Standard_Integer I, const Standard_Boolean FindKeep);

  Standard_EXPORT void ChangeKeepShape(const TopoDS_Shape& S, const Standard_Boolean FindKeep);

  Standard_EXPORT void InitSectionEdges();

  Standard_EXPORT Standard_Integer AddSectionEdge(const TopoDS_Edge& E);

  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& SurfaceInterferences(
    const Standard_Integer I) const;

  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeSurfaceInterferences(
    const Standard_Integer I);

  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& CurveInterferences(
    const Standard_Integer I) const;

  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeCurveInterferences(
    const Standard_Integer I);

  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& PointInterferences(
    const Standard_Integer I) const;

  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangePointInterferences(
    const Standard_Integer I);

  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& ShapeInterferences(
    const TopoDS_Shape&    S,
    const Standard_Boolean FindKeep = Standard_True) const;

  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeShapeInterferences(const TopoDS_Shape& S);

  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& ShapeInterferences(
    const Standard_Integer I,
    const Standard_Boolean FindKeep = Standard_True) const;

  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeShapeInterferences(
    const Standard_Integer I);

  Standard_EXPORT const TopTools_ListOfShape& ShapeSameDomain(const TopoDS_Shape& S) const;

  Standard_EXPORT TopTools_ListOfShape& ChangeShapeSameDomain(const TopoDS_Shape& S);

  Standard_EXPORT const TopTools_ListOfShape& ShapeSameDomain(const Standard_Integer I) const;

  Standard_EXPORT TopTools_ListOfShape& ChangeShapeSameDomain(const Standard_Integer I);

  Standard_EXPORT TopOpeBRepDS_MapOfShapeData& ChangeShapes();

  Standard_EXPORT void AddShapeSameDomain(const TopoDS_Shape& S, const TopoDS_Shape& SSD);

  Standard_EXPORT void RemoveShapeSameDomain(const TopoDS_Shape& S, const TopoDS_Shape& SSD);

  Standard_EXPORT Standard_Integer SameDomainRef(const Standard_Integer I) const;

  Standard_EXPORT Standard_Integer SameDomainRef(const TopoDS_Shape& S) const;

  Standard_EXPORT void SameDomainRef(const Standard_Integer I, const Standard_Integer Ref);

  Standard_EXPORT void SameDomainRef(const TopoDS_Shape& S, const Standard_Integer Ref);

  Standard_EXPORT TopOpeBRepDS_Config SameDomainOri(const Standard_Integer I) const;

  Standard_EXPORT TopOpeBRepDS_Config SameDomainOri(const TopoDS_Shape& S) const;

  Standard_EXPORT void SameDomainOri(const Standard_Integer I, const TopOpeBRepDS_Config Ori);

  Standard_EXPORT void SameDomainOri(const TopoDS_Shape& S, const TopOpeBRepDS_Config Ori);

  Standard_EXPORT Standard_Integer SameDomainInd(const Standard_Integer I) const;

  Standard_EXPORT Standard_Integer SameDomainInd(const TopoDS_Shape& S) const;

  Standard_EXPORT void SameDomainInd(const Standard_Integer I, const Standard_Integer Ind);

  Standard_EXPORT void SameDomainInd(const TopoDS_Shape& S, const Standard_Integer Ind);

  Standard_EXPORT Standard_Integer AncestorRank(const Standard_Integer I) const;

  Standard_EXPORT Standard_Integer AncestorRank(const TopoDS_Shape& S) const;

  Standard_EXPORT void AncestorRank(const Standard_Integer I, const Standard_Integer Ianc);

  Standard_EXPORT void AncestorRank(const TopoDS_Shape& S, const Standard_Integer Ianc);

  Standard_EXPORT void AddShapeInterference(const TopoDS_Shape&                      S,
                                            const Handle(TopOpeBRepDS_Interference)& I);

  Standard_EXPORT void RemoveShapeInterference(const TopoDS_Shape&                      S,
                                               const Handle(TopOpeBRepDS_Interference)& I);

  Standard_EXPORT void FillShapesSameDomain(const TopoDS_Shape&    S1,
                                            const TopoDS_Shape&    S2,
                                            const Standard_Boolean refFirst = Standard_True);

  Standard_EXPORT void FillShapesSameDomain(const TopoDS_Shape&       S1,
                                            const TopoDS_Shape&       S2,
                                            const TopOpeBRepDS_Config c1,
                                            const TopOpeBRepDS_Config c2,
                                            const Standard_Boolean    refFirst = Standard_True);

  Standard_EXPORT void UnfillShapesSameDomain(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  Standard_EXPORT Standard_Integer NbSurfaces() const;

  Standard_EXPORT Standard_Integer NbCurves() const;

  Standard_EXPORT void ChangeNbCurves(const Standard_Integer N);

  Standard_EXPORT Standard_Integer NbPoints() const;

  Standard_EXPORT Standard_Integer NbShapes() const;

  Standard_EXPORT Standard_Integer NbSectionEdges() const;

  //! Returns the surface of index <I>.
  Standard_EXPORT const TopOpeBRepDS_Surface& Surface(const Standard_Integer I) const;

  //! Returns the surface of index <I>.
  Standard_EXPORT TopOpeBRepDS_Surface& ChangeSurface(const Standard_Integer I);

  //! Returns the Curve of index <I>.
  Standard_EXPORT const TopOpeBRepDS_Curve& Curve(const Standard_Integer I) const;

  //! Returns the Curve of index <I>.
  Standard_EXPORT TopOpeBRepDS_Curve& ChangeCurve(const Standard_Integer I);

  //! Returns the point of index <I>.
  Standard_EXPORT const TopOpeBRepDS_Point& Point(const Standard_Integer I) const;

  //! Returns the point of index <I>.
  Standard_EXPORT TopOpeBRepDS_Point& ChangePoint(const Standard_Integer I);

  //! returns the shape of index I stored in
  //! the map myShapes, accessing a list of interference.
  Standard_EXPORT const TopoDS_Shape& Shape(const Standard_Integer I,
                                            const Standard_Boolean FindKeep = Standard_True) const;

  //! returns the index of shape <S> stored in
  //! the map myShapes, accessing a list of interference.
  //! returns 0 if <S> is not in the map.
  Standard_EXPORT Standard_Integer Shape(const TopoDS_Shape&    S,
                                         const Standard_Boolean FindKeep = Standard_True) const;

  Standard_EXPORT const TopoDS_Edge& SectionEdge(
    const Standard_Integer I,
    const Standard_Boolean FindKeep = Standard_True) const;

  Standard_EXPORT Standard_Integer
    SectionEdge(const TopoDS_Edge& E, const Standard_Boolean FindKeep = Standard_True) const;

  Standard_EXPORT Standard_Boolean
    IsSectionEdge(const TopoDS_Edge& E, const Standard_Boolean FindKeep = Standard_True) const;

  //! Returns True if <S> has new geometries, i.e :
  //! True si :
  //! HasShape(S) True
  //! S a une liste d'interferences non vide.
  //! S = SOLID, FACE, EDGE : true/false
  //! S = SHELL, WIRE, VERTEX : false.
  Standard_EXPORT Standard_Boolean HasGeometry(const TopoDS_Shape& S) const;

  //! Returns True if <S> est dans myShapes
  Standard_EXPORT Standard_Boolean HasShape(const TopoDS_Shape&    S,
                                            const Standard_Boolean FindKeep = Standard_True) const;

  Standard_EXPORT void SetNewSurface(const TopoDS_Shape& F, const Handle(Geom_Surface)& S);

  Standard_EXPORT Standard_Boolean HasNewSurface(const TopoDS_Shape& F) const;

  Standard_EXPORT const Handle(Geom_Surface)& NewSurface(const TopoDS_Shape& F) const;

  Standard_EXPORT void Isfafa(const Standard_Boolean isfafa);

  Standard_EXPORT Standard_Boolean Isfafa() const;

  Standard_EXPORT TopOpeBRepDS_IndexedDataMapOfShapeWithState& ChangeMapOfShapeWithStateObj();

  Standard_EXPORT TopOpeBRepDS_IndexedDataMapOfShapeWithState& ChangeMapOfShapeWithStateTool();

  Standard_EXPORT TopOpeBRepDS_IndexedDataMapOfShapeWithState& ChangeMapOfShapeWithState(
    const TopoDS_Shape& aShape,
    Standard_Boolean&   aFlag);

  Standard_EXPORT const TopOpeBRepDS_ShapeWithState& GetShapeWithState(
    const TopoDS_Shape& aShape) const;

  Standard_EXPORT TopTools_IndexedMapOfShape& ChangeMapOfRejectedShapesObj();

  Standard_EXPORT TopTools_IndexedMapOfShape& ChangeMapOfRejectedShapesTool();

  friend class TopOpeBRepDS_SurfaceExplorer;
  friend class TopOpeBRepDS_CurveExplorer;
  friend class TopOpeBRepDS_PointExplorer;

protected:
private:
  Standard_EXPORT Standard_Boolean
    FindInterference(TopOpeBRepDS_ListIteratorOfListOfInterference& IT,
                     const Handle(TopOpeBRepDS_Interference)&       I) const;

  Standard_Integer                            myNbSurfaces;
  TopOpeBRepDS_MapOfSurface                   mySurfaces;
  Standard_Integer                            myNbCurves;
  TopOpeBRepDS_MapOfCurve                     myCurves;
  Standard_Integer                            myNbPoints;
  TopOpeBRepDS_MapOfPoint                     myPoints;
  TopOpeBRepDS_MapOfShapeData                 myShapes;
  TopTools_IndexedMapOfShape                  mySectionEdges;
  TopOpeBRepDS_ListOfInterference             myEmptyListOfInterference;
  TopTools_ListOfShape                        myEmptyListOfShape;
  TopoDS_Shape                                myEmptyShape;
  TopOpeBRepDS_Point                          myEmptyPoint;
  TopOpeBRepDS_Surface                        myEmptySurface;
  TopOpeBRepDS_Curve                          myEmptyCurve;
  Handle(Geom_Surface)                        myEmptyGSurface;
  TopOpeBRepDS_ShapeSurface                   myNewSurface;
  Standard_Boolean                            myIsfafa;
  Standard_Integer                            myI;
  TopOpeBRepDS_IndexedDataMapOfShapeWithState myMapOfShapeWithStateObj;
  TopOpeBRepDS_IndexedDataMapOfShapeWithState myMapOfShapeWithStateTool;
  TopTools_IndexedMapOfShape                  myMapOfRejectedShapesObj;
  TopTools_IndexedMapOfShape                  myMapOfRejectedShapesTool;
};

#endif // _TopOpeBRepDS_DataStructure_HeaderFile
