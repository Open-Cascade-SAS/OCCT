// Created on: 1994-08-25
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepTools_Modifier_HeaderFile
#define _BRepTools_Modifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopLoc_Location.hxx>
#include <Message_ProgressRange.hxx>

class BRepTools_Modification;
class Geom_Curve;
class Geom_Surface;

//! Performs geometric modifications on a shape.
class BRepTools_Modifier
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty Modifier.
  Standard_EXPORT BRepTools_Modifier(bool theMutableInput = false);

  //! Creates a modifier on the shape <S>.
  Standard_EXPORT BRepTools_Modifier(const TopoDS_Shape& S);

  //! Creates a modifier on the shape <S>, and performs
  //! the modifications described by <M>.
  Standard_EXPORT BRepTools_Modifier(const TopoDS_Shape&                   S,
                                     const occ::handle<BRepTools_Modification>& M);

  //! Initializes the modifier with the shape <S>.
  Standard_EXPORT void Init(const TopoDS_Shape& S);

  //! Performs the modifications described by <M>.
  Standard_EXPORT void Perform(const occ::handle<BRepTools_Modification>& M,
                               const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Returns true if the modification has
  //! been computed successfully.
  bool IsDone() const;

  //! Returns the current mutable input state
  Standard_EXPORT bool IsMutableInput() const;

  //! Sets the mutable input state
  //! If true then the input (original) shape can be modified
  //! during modification process
  Standard_EXPORT void SetMutableInput(bool theMutableInput);

  //! Returns the modified shape corresponding to <S>.
  const TopoDS_Shape& ModifiedShape(const TopoDS_Shape& S) const;

private:
  struct NewCurveInfo
  {
    occ::handle<Geom_Curve> myCurve;
    TopLoc_Location    myLoc;
    double      myToler;
  };

  struct NewSurfaceInfo
  {
    occ::handle<Geom_Surface> mySurface;
    TopLoc_Location      myLoc;
    double        myToler;
    bool     myRevWires;
    bool     myRevFace;
  };

  Standard_EXPORT void Put(const TopoDS_Shape& S);

  Standard_EXPORT bool
    Rebuild(const TopoDS_Shape&                   S,
            const occ::handle<BRepTools_Modification>& M,
            bool&                     theNewGeom,
            const Message_ProgressRange&          theProgress = Message_ProgressRange());

  Standard_EXPORT void CreateNewVertices(const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theMVE,
                                         const occ::handle<BRepTools_Modification>&            M);

  Standard_EXPORT void FillNewCurveInfo(const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theMEF,
                                        const occ::handle<BRepTools_Modification>&            M);

  Standard_EXPORT void FillNewSurfaceInfo(const occ::handle<BRepTools_Modification>& M);

  Standard_EXPORT void CreateOtherVertices(const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theMVE,
                                           const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theMEF,
                                           const occ::handle<BRepTools_Modification>&            M);

  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>                                              myMap;
  TopoDS_Shape                                                              myShape;
  bool                                                          myDone;
  NCollection_DataMap<TopoDS_Edge, NewCurveInfo, TopTools_ShapeMapHasher>   myNCInfo;
  NCollection_DataMap<TopoDS_Face, NewSurfaceInfo, TopTools_ShapeMapHasher> myNSInfo;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                                                       myNonUpdFace;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                                                       myHasNewGeom;
  bool                                                          myMutableInput;
};

#include <BRepTools_Modifier.lxx>

#endif // _BRepTools_Modifier_HeaderFile
