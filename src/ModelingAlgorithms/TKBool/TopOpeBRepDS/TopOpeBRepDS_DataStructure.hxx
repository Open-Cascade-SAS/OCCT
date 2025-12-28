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
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_SurfaceData.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_CurveData.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_PointData.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ShapeData.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_Surface.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ShapeWithState.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
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
  Standard_EXPORT int AddSurface(const TopOpeBRepDS_Surface& S);

  Standard_EXPORT void RemoveSurface(const int I);

  Standard_EXPORT bool KeepSurface(const int I) const;

  Standard_EXPORT bool KeepSurface(TopOpeBRepDS_Surface& S) const;

  Standard_EXPORT void ChangeKeepSurface(const int I, const bool FindKeep);

  Standard_EXPORT void ChangeKeepSurface(TopOpeBRepDS_Surface& S, const bool FindKeep);

  //! Insert a new curve. Returns the index.
  Standard_EXPORT int AddCurve(const TopOpeBRepDS_Curve& S);

  Standard_EXPORT void RemoveCurve(const int I);

  Standard_EXPORT bool KeepCurve(const int I) const;

  Standard_EXPORT bool KeepCurve(const TopOpeBRepDS_Curve& C) const;

  Standard_EXPORT void ChangeKeepCurve(const int I, const bool FindKeep);

  Standard_EXPORT void ChangeKeepCurve(TopOpeBRepDS_Curve& C, const bool FindKeep);

  //! Insert a new point. Returns the index.
  Standard_EXPORT int AddPoint(const TopOpeBRepDS_Point& PDS);

  //! Insert a new point. Returns the index.
  Standard_EXPORT int AddPointSS(const TopOpeBRepDS_Point& PDS,
                                              const TopoDS_Shape&       S1,
                                              const TopoDS_Shape&       S2);

  Standard_EXPORT void RemovePoint(const int I);

  Standard_EXPORT bool KeepPoint(const int I) const;

  Standard_EXPORT bool KeepPoint(const TopOpeBRepDS_Point& P) const;

  Standard_EXPORT void ChangeKeepPoint(const int I, const bool FindKeep);

  Standard_EXPORT void ChangeKeepPoint(TopOpeBRepDS_Point& P, const bool FindKeep);

  //! Insert a shape S. Returns the index.
  Standard_EXPORT int AddShape(const TopoDS_Shape& S);

  //! Insert a shape S which ancestor is I = 1 or 2. Returns the index.
  Standard_EXPORT int AddShape(const TopoDS_Shape& S, const int I);

  Standard_EXPORT bool KeepShape(const int I,
                                             const bool FindKeep = true) const;

  Standard_EXPORT bool KeepShape(const TopoDS_Shape&    S,
                                             const bool FindKeep = true) const;

  Standard_EXPORT void ChangeKeepShape(const int I, const bool FindKeep);

  Standard_EXPORT void ChangeKeepShape(const TopoDS_Shape& S, const bool FindKeep);

  Standard_EXPORT void InitSectionEdges();

  Standard_EXPORT int AddSectionEdge(const TopoDS_Edge& E);

  Standard_EXPORT const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& SurfaceInterferences(
    const int I) const;

  Standard_EXPORT NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& ChangeSurfaceInterferences(
    const int I);

  Standard_EXPORT const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& CurveInterferences(
    const int I) const;

  Standard_EXPORT NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& ChangeCurveInterferences(
    const int I);

  Standard_EXPORT const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& PointInterferences(
    const int I) const;

  Standard_EXPORT NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& ChangePointInterferences(
    const int I);

  Standard_EXPORT const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& ShapeInterferences(
    const TopoDS_Shape&    S,
    const bool FindKeep = true) const;

  Standard_EXPORT NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& ChangeShapeInterferences(const TopoDS_Shape& S);

  Standard_EXPORT const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& ShapeInterferences(
    const int I,
    const bool FindKeep = true) const;

  Standard_EXPORT NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& ChangeShapeInterferences(
    const int I);

  Standard_EXPORT const NCollection_List<TopoDS_Shape>& ShapeSameDomain(const TopoDS_Shape& S) const;

  Standard_EXPORT NCollection_List<TopoDS_Shape>& ChangeShapeSameDomain(const TopoDS_Shape& S);

  Standard_EXPORT const NCollection_List<TopoDS_Shape>& ShapeSameDomain(const int I) const;

  Standard_EXPORT NCollection_List<TopoDS_Shape>& ChangeShapeSameDomain(const int I);

  Standard_EXPORT NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeData, TopTools_ShapeMapHasher>& ChangeShapes();

  Standard_EXPORT void AddShapeSameDomain(const TopoDS_Shape& S, const TopoDS_Shape& SSD);

  Standard_EXPORT void RemoveShapeSameDomain(const TopoDS_Shape& S, const TopoDS_Shape& SSD);

  Standard_EXPORT int SameDomainRef(const int I) const;

  Standard_EXPORT int SameDomainRef(const TopoDS_Shape& S) const;

  Standard_EXPORT void SameDomainRef(const int I, const int Ref);

  Standard_EXPORT void SameDomainRef(const TopoDS_Shape& S, const int Ref);

  Standard_EXPORT TopOpeBRepDS_Config SameDomainOri(const int I) const;

  Standard_EXPORT TopOpeBRepDS_Config SameDomainOri(const TopoDS_Shape& S) const;

  Standard_EXPORT void SameDomainOri(const int I, const TopOpeBRepDS_Config Ori);

  Standard_EXPORT void SameDomainOri(const TopoDS_Shape& S, const TopOpeBRepDS_Config Ori);

  Standard_EXPORT int SameDomainInd(const int I) const;

  Standard_EXPORT int SameDomainInd(const TopoDS_Shape& S) const;

  Standard_EXPORT void SameDomainInd(const int I, const int Ind);

  Standard_EXPORT void SameDomainInd(const TopoDS_Shape& S, const int Ind);

  Standard_EXPORT int AncestorRank(const int I) const;

  Standard_EXPORT int AncestorRank(const TopoDS_Shape& S) const;

  Standard_EXPORT void AncestorRank(const int I, const int Ianc);

  Standard_EXPORT void AncestorRank(const TopoDS_Shape& S, const int Ianc);

  Standard_EXPORT void AddShapeInterference(const TopoDS_Shape&                      S,
                                            const occ::handle<TopOpeBRepDS_Interference>& I);

  Standard_EXPORT void RemoveShapeInterference(const TopoDS_Shape&                      S,
                                               const occ::handle<TopOpeBRepDS_Interference>& I);

  Standard_EXPORT void FillShapesSameDomain(const TopoDS_Shape&    S1,
                                            const TopoDS_Shape&    S2,
                                            const bool refFirst = true);

  Standard_EXPORT void FillShapesSameDomain(const TopoDS_Shape&       S1,
                                            const TopoDS_Shape&       S2,
                                            const TopOpeBRepDS_Config c1,
                                            const TopOpeBRepDS_Config c2,
                                            const bool    refFirst = true);

  Standard_EXPORT void UnfillShapesSameDomain(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  Standard_EXPORT int NbSurfaces() const;

  Standard_EXPORT int NbCurves() const;

  Standard_EXPORT void ChangeNbCurves(const int N);

  Standard_EXPORT int NbPoints() const;

  Standard_EXPORT int NbShapes() const;

  Standard_EXPORT int NbSectionEdges() const;

  //! Returns the surface of index <I>.
  Standard_EXPORT const TopOpeBRepDS_Surface& Surface(const int I) const;

  //! Returns the surface of index <I>.
  Standard_EXPORT TopOpeBRepDS_Surface& ChangeSurface(const int I);

  //! Returns the Curve of index <I>.
  Standard_EXPORT const TopOpeBRepDS_Curve& Curve(const int I) const;

  //! Returns the Curve of index <I>.
  Standard_EXPORT TopOpeBRepDS_Curve& ChangeCurve(const int I);

  //! Returns the point of index <I>.
  Standard_EXPORT const TopOpeBRepDS_Point& Point(const int I) const;

  //! Returns the point of index <I>.
  Standard_EXPORT TopOpeBRepDS_Point& ChangePoint(const int I);

  //! returns the shape of index I stored in
  //! the map myShapes, accessing a list of interference.
  Standard_EXPORT const TopoDS_Shape& Shape(const int I,
                                            const bool FindKeep = true) const;

  //! returns the index of shape <S> stored in
  //! the map myShapes, accessing a list of interference.
  //! returns 0 if <S> is not in the map.
  Standard_EXPORT int Shape(const TopoDS_Shape&    S,
                                         const bool FindKeep = true) const;

  Standard_EXPORT const TopoDS_Edge& SectionEdge(
    const int I,
    const bool FindKeep = true) const;

  Standard_EXPORT int
    SectionEdge(const TopoDS_Edge& E, const bool FindKeep = true) const;

  Standard_EXPORT bool
    IsSectionEdge(const TopoDS_Edge& E, const bool FindKeep = true) const;

  //! Returns True if <S> has new geometries, i.e :
  //! True si :
  //! HasShape(S) True
  //! S a une liste d'interferences non vide.
  //! S = SOLID, FACE, EDGE : true/false
  //! S = SHELL, WIRE, VERTEX : false.
  Standard_EXPORT bool HasGeometry(const TopoDS_Shape& S) const;

  //! Returns True if <S> est dans myShapes
  Standard_EXPORT bool HasShape(const TopoDS_Shape&    S,
                                            const bool FindKeep = true) const;

  Standard_EXPORT void SetNewSurface(const TopoDS_Shape& F, const occ::handle<Geom_Surface>& S);

  Standard_EXPORT bool HasNewSurface(const TopoDS_Shape& F) const;

  Standard_EXPORT const occ::handle<Geom_Surface>& NewSurface(const TopoDS_Shape& F) const;

  Standard_EXPORT void Isfafa(const bool isfafa);

  Standard_EXPORT bool Isfafa() const;

  Standard_EXPORT NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeWithState, TopTools_ShapeMapHasher>& ChangeMapOfShapeWithStateObj();

  Standard_EXPORT NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeWithState, TopTools_ShapeMapHasher>& ChangeMapOfShapeWithStateTool();

  Standard_EXPORT NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeWithState, TopTools_ShapeMapHasher>& ChangeMapOfShapeWithState(
    const TopoDS_Shape& aShape,
    bool&   aFlag);

  Standard_EXPORT const TopOpeBRepDS_ShapeWithState& GetShapeWithState(
    const TopoDS_Shape& aShape) const;

  Standard_EXPORT NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& ChangeMapOfRejectedShapesObj();

  Standard_EXPORT NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& ChangeMapOfRejectedShapesTool();

  friend class TopOpeBRepDS_SurfaceExplorer;
  friend class TopOpeBRepDS_CurveExplorer;
  friend class TopOpeBRepDS_PointExplorer;

private:
  Standard_EXPORT bool
    FindInterference(NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& IT,
                     const occ::handle<TopOpeBRepDS_Interference>&       I) const;

  int                            myNbSurfaces;
  NCollection_DataMap<int, TopOpeBRepDS_SurfaceData>                   mySurfaces;
  int                            myNbCurves;
  NCollection_DataMap<int, TopOpeBRepDS_CurveData>                     myCurves;
  int                            myNbPoints;
  NCollection_DataMap<int, TopOpeBRepDS_PointData>                     myPoints;
  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeData, TopTools_ShapeMapHasher>                 myShapes;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                  mySectionEdges;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>             myEmptyListOfInterference;
  NCollection_List<TopoDS_Shape>                        myEmptyListOfShape;
  TopoDS_Shape                                myEmptyShape;
  TopOpeBRepDS_Point                          myEmptyPoint;
  TopOpeBRepDS_Surface                        myEmptySurface;
  TopOpeBRepDS_Curve                          myEmptyCurve;
  occ::handle<Geom_Surface>                        myEmptyGSurface;
  NCollection_DataMap<TopoDS_Shape, occ::handle<Geom_Surface>, TopTools_ShapeMapHasher>                   myNewSurface;
  bool                            myIsfafa;
  int                            myI;
  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeWithState, TopTools_ShapeMapHasher> myMapOfShapeWithStateObj;
  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeWithState, TopTools_ShapeMapHasher> myMapOfShapeWithStateTool;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                  myMapOfRejectedShapesObj;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                  myMapOfRejectedShapesTool;
};

#endif // _TopOpeBRepDS_DataStructure_HeaderFile
