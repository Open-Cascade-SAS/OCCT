// Created on: 1994-10-24
// Created by: Christophe MARION
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

#ifndef _HLRTopoBRep_Data_HeaderFile
#define _HLRTopoBRep_Data_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <HLRTopoBRep_DataMapOfShapeFaceData.hxx>
#include <TopTools_MapOfShape.hxx>
#include <HLRTopoBRep_MapOfShapeListOfVData.hxx>
#include <HLRTopoBRep_DataMapIteratorOfMapOfShapeListOfVData.hxx>
#include <HLRTopoBRep_ListIteratorOfListOfVData.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Real.hxx>
class TopoDS_Edge;
class TopoDS_Face;
class TopoDS_Shape;
class TopoDS_Vertex;


//! Stores  the results  of  the  OutLine and  IsoLine
//! processes.
class HLRTopoBRep_Data 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRTopoBRep_Data();
  
  //! Clear of all the maps.
  Standard_EXPORT void Clear();
  
  //! Clear of all the data  not needed during and after
  //! the hiding process.
  Standard_EXPORT void Clean();
  
  //! Returns True if the Edge is split.
  Standard_EXPORT Standard_Boolean EdgeHasSplE (const TopoDS_Edge& E) const;
  
  //! Returns True if the Face has internal outline.
  Standard_EXPORT Standard_Boolean FaceHasIntL (const TopoDS_Face& F) const;
  
  //! Returns True if the Face has outlines on restriction.
  Standard_EXPORT Standard_Boolean FaceHasOutL (const TopoDS_Face& F) const;
  
  //! Returns True if the Face has isolines.
  Standard_EXPORT Standard_Boolean FaceHasIsoL (const TopoDS_Face& F) const;
  
  Standard_EXPORT Standard_Boolean IsSplEEdgeEdge (const TopoDS_Edge& E1, const TopoDS_Edge& E2) const;
  
  Standard_EXPORT Standard_Boolean IsIntLFaceEdge (const TopoDS_Face& F, const TopoDS_Edge& E) const;
  
  Standard_EXPORT Standard_Boolean IsOutLFaceEdge (const TopoDS_Face& F, const TopoDS_Edge& E) const;
  
  Standard_EXPORT Standard_Boolean IsIsoLFaceEdge (const TopoDS_Face& F, const TopoDS_Edge& E) const;
  
  Standard_EXPORT TopoDS_Shape NewSOldS (const TopoDS_Shape& New) const;
  
  //! Returns the list of the edges.
    const TopTools_ListOfShape& EdgeSplE (const TopoDS_Edge& E) const;
  
  //! Returns the list of the internal OutLines.
    const TopTools_ListOfShape& FaceIntL (const TopoDS_Face& F) const;
  
  //! Returns the list of the OutLines on restriction.
    const TopTools_ListOfShape& FaceOutL (const TopoDS_Face& F) const;
  
  //! Returns the list of the IsoLines.
    const TopTools_ListOfShape& FaceIsoL (const TopoDS_Face& F) const;
  
  //! Returns  True   if V is  an   outline vertex  on a
  //! restriction.
    Standard_Boolean IsOutV (const TopoDS_Vertex& V) const;
  
  //! Returns True if V is an internal outline vertex.
    Standard_Boolean IsIntV (const TopoDS_Vertex& V) const;
  
  Standard_EXPORT void AddOldS (const TopoDS_Shape& NewS, const TopoDS_Shape& OldS);
  
  Standard_EXPORT TopTools_ListOfShape& AddSplE (const TopoDS_Edge& E);
  
  Standard_EXPORT TopTools_ListOfShape& AddIntL (const TopoDS_Face& F);
  
  Standard_EXPORT TopTools_ListOfShape& AddOutL (const TopoDS_Face& F);
  
  Standard_EXPORT TopTools_ListOfShape& AddIsoL (const TopoDS_Face& F);
  
    void AddOutV (const TopoDS_Vertex& V);
  
    void AddIntV (const TopoDS_Vertex& V);
  
  Standard_EXPORT void InitEdge();
  
    Standard_Boolean MoreEdge() const;
  
  Standard_EXPORT void NextEdge();
  
    const TopoDS_Edge& Edge() const;
  
  //! Start an iteration on the vertices of E.
  Standard_EXPORT void InitVertex (const TopoDS_Edge& E);
  
    Standard_Boolean MoreVertex() const;
  
    void NextVertex();
  
  Standard_EXPORT const TopoDS_Vertex& Vertex() const;
  
  Standard_EXPORT Standard_Real Parameter() const;
  
  //! Insert before the current position.
  Standard_EXPORT void InsertBefore (const TopoDS_Vertex& V, const Standard_Real P);
  
  Standard_EXPORT void Append (const TopoDS_Vertex& V, const Standard_Real P);




protected:





private:



  TopTools_DataMapOfShapeShape myOldS;
  TopTools_DataMapOfShapeListOfShape mySplE;
  HLRTopoBRep_DataMapOfShapeFaceData myData;
  TopTools_MapOfShape myOutV;
  TopTools_MapOfShape myIntV;
  HLRTopoBRep_MapOfShapeListOfVData myEdgesVertices;
  HLRTopoBRep_DataMapIteratorOfMapOfShapeListOfVData myEIterator;
  HLRTopoBRep_ListIteratorOfListOfVData myVIterator;
  HLRTopoBRep_ListOfVData* myVList;


};


#include <HLRTopoBRep_Data.lxx>





#endif // _HLRTopoBRep_Data_HeaderFile
