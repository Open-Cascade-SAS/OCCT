// Created on: 1998-03-04
// Created by: Julia Gerasimova
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _AIS_BadEdgeFilter_HeaderFile
#define _AIS_BadEdgeFilter_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <SelectMgr_Filter.hxx>
#include <TopAbs_ShapeEnum.hxx>
class SelectMgr_EntityOwner;
class TopoDS_Edge;

//! A Class
class AIS_BadEdgeFilter : public SelectMgr_Filter
{

public:
  //! Constructs an empty filter object for bad edges.
  Standard_EXPORT AIS_BadEdgeFilter();

  Standard_EXPORT bool ActsOn(const TopAbs_ShapeEnum aType) const override;

  Standard_EXPORT bool IsOk(const occ::handle<SelectMgr_EntityOwner>& EO) const override;

  //! sets <myContour> with current contour. used by IsOk.
  Standard_EXPORT void SetContour(const int Index);

  //! Adds an edge to the list of non-selectable edges.
  Standard_EXPORT void AddEdge(const TopoDS_Edge& anEdge, const int Index);

  //! removes from the list of non-selectable edges
  //! all edges in the contour <Index>.
  Standard_EXPORT void RemoveEdges(const int Index);

  DEFINE_STANDARD_RTTIEXT(AIS_BadEdgeFilter, SelectMgr_Filter)

private:
  NCollection_DataMap<int, NCollection_List<TopoDS_Shape>> myBadEdges;
  int                                                      myContour;
};

#endif // _AIS_BadEdgeFilter_HeaderFile
