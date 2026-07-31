// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _HLRBRep_InternalAlgo_HeaderFile
#define _HLRBRep_InternalAlgo_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepTopAdaptor_Tool.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class HLRBRep_Data;
class HLRTopoBRep_OutLiner;
class HLRBRep_ShapeBounds;

class HLRBRep_InternalAlgo : public Standard_Transient
{

public:
  Standard_EXPORT HLRBRep_InternalAlgo();

  Standard_EXPORT HLRBRep_InternalAlgo(const occ::handle<HLRBRep_InternalAlgo>& A);

  //! set the projector.
  Standard_EXPORT void Projector(const HLRAlgo_Projector& P);

  //! set the projector.
  Standard_EXPORT HLRAlgo_Projector& Projector();

  //! update the DataStructure.
  Standard_EXPORT void Update();

  //! add the shape <S>.
  Standard_EXPORT void Load(const occ::handle<HLRTopoBRep_OutLiner>& S,
                            const occ::handle<Standard_Transient>&   SData,
                            const int                                nbIso = 0);

  //! add the shape <S>.
  Standard_EXPORT void Load(const occ::handle<HLRTopoBRep_OutLiner>& S, const int nbIso = 0);

  //! return the index of the Shape <S> and return 0 if
  //! the Shape <S> is not found.
  Standard_EXPORT int Index(const occ::handle<HLRTopoBRep_OutLiner>& S) const;

  //! remove the Shape of Index <I>.
  Standard_EXPORT void Remove(const int I);

  //! Change the Shape Data of the Shape of index <I>.
  Standard_EXPORT void ShapeData(const int I, const occ::handle<Standard_Transient>& SData);

  Standard_EXPORT NCollection_Sequence<HLRBRep_ShapeBounds>& SeqOfShapeBounds();

  Standard_EXPORT int NbShapes() const;

  Standard_EXPORT HLRBRep_ShapeBounds& ShapeBounds(const int I);

  //! init the status of the selected edges depending of
  //! the back faces of a closed shell.
  Standard_EXPORT void InitEdgeStatus();

  //! select all the DataStructure.
  Standard_EXPORT void Select();

  //! select only the Shape of index <I>.
  Standard_EXPORT void Select(const int I);

  //! select only the edges of the Shape <S>.
  Standard_EXPORT void SelectEdge(const int I);

  //! select only the faces of the Shape <S>.
  Standard_EXPORT void SelectFace(const int I);

  //! set to visible all the edges.
  Standard_EXPORT void ShowAll();

  //! set to visible all the edges of the Shape <S>.
  Standard_EXPORT void ShowAll(const int I);

  //! set to hide all the edges.
  Standard_EXPORT void HideAll();

  //! set to hide all the edges of the Shape <S>.
  Standard_EXPORT void HideAll(const int I);

  //! own hiding of all the shapes of the DataStructure
  //! without hiding by each other.
  Standard_EXPORT void PartialHide();

  //! hide all the DataStructure.
  Standard_EXPORT void Hide();

  //! hide the Shape <S> by itself.
  Standard_EXPORT void Hide(const int I);

  //! hide the Shape <S1> by the shape <S2>.
  Standard_EXPORT void Hide(const int I, const int J);

  Standard_EXPORT void Debug(const bool deb);

  Standard_EXPORT bool Debug() const;

  Standard_EXPORT occ::handle<HLRBRep_Data> DataStructure() const;

  DEFINE_STANDARD_RTTIEXT(HLRBRep_InternalAlgo, Standard_Transient)

private:
  //! first if <SideFace> own hiding of the side faces.
  //! After hiding of the selected parts of the
  //! DataStructure.
  Standard_EXPORT void HideSelected(const int I, const bool SideFace);

  occ::handle<HLRBRep_Data>                                                       myDS;
  HLRAlgo_Projector                                                               myProj;
  NCollection_Sequence<HLRBRep_ShapeBounds>                                       myShapes;
  NCollection_DataMap<TopoDS_Shape, BRepTopAdaptor_Tool, TopTools_ShapeMapHasher> myMapOfShapeTool;
  bool                                                                            myDebug;
};

#endif // _HLRBRep_InternalAlgo_HeaderFile
