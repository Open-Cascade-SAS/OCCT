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
#include <Standard_Type.hxx>

#include <TopTools_DataMapOfIntegerListOfShape.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Filter.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class SelectMgr_EntityOwner;
class TopoDS_Edge;


class AIS_BadEdgeFilter;
DEFINE_STANDARD_HANDLE(AIS_BadEdgeFilter, SelectMgr_Filter)

//! A Class
class AIS_BadEdgeFilter : public SelectMgr_Filter
{

public:

  
  //! Constructs an empty filter object for bad edges.
  Standard_EXPORT AIS_BadEdgeFilter();
  
  Standard_EXPORT virtual Standard_Boolean ActsOn (const TopAbs_ShapeEnum aType) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& EO) const Standard_OVERRIDE;
  
  //! sets  <myContour> with  current  contour. used  by
  //! IsOk.
  Standard_EXPORT void SetContour (const Standard_Integer Index);
  
  //! Adds an  edge  to the list  of non-selectionnable
  //! edges.
  Standard_EXPORT void AddEdge (const TopoDS_Edge& anEdge, const Standard_Integer Index);
  
  //! removes from the  list of non-selectionnable edges
  //! all edges in the contour <Index>.
  Standard_EXPORT void RemoveEdges (const Standard_Integer Index);




  DEFINE_STANDARD_RTTIEXT(AIS_BadEdgeFilter,SelectMgr_Filter)

protected:




private:


  TopTools_DataMapOfIntegerListOfShape myBadEdges;
  Standard_Integer myContour;


};







#endif // _AIS_BadEdgeFilter_HeaderFile
