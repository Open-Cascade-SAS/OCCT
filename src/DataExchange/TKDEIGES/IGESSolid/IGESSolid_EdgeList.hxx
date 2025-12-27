// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESSolid_EdgeList_HeaderFile
#define _IGESSolid_EdgeList_HeaderFile

#include <Standard.hxx>

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESSolid_HArray1OfVertexList.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESSolid_VertexList;

//! defines EdgeList, Type <504> Form <1>
//! in package IGESSolid
//! EdgeList is defined as a segment joining two vertices
//! It contains one or more edge tuples.
class IGESSolid_EdgeList : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESSolid_EdgeList();

  //! This method is used to set the fields of the class
  //! EdgeList
  //! - curves           : the model space curves
  //! - startVertexList  : the vertex list that contains the
  //! start vertices
  //! - startVertexIndex : the index of the vertex in the
  //! corresponding vertex list
  //! - endVertexList    : the vertex list that contains the
  //! end vertices
  //! - endVertexIndex   : the index of the vertex in the
  //! corresponding vertex list
  //! raises exception if size of curves,startVertexList,startVertexIndex,
  //! endVertexList and endVertexIndex do no match
  Standard_EXPORT void Init(const occ::handle<IGESData_HArray1OfIGESEntity>&  curves,
                            const occ::handle<IGESSolid_HArray1OfVertexList>& startVertexList,
                            const occ::handle<TColStd_HArray1OfInteger>&      startVertexIndex,
                            const occ::handle<IGESSolid_HArray1OfVertexList>& endVertexList,
                            const occ::handle<TColStd_HArray1OfInteger>&      endVertexIndex);

  //! returns the number of edges in the edge list
  Standard_EXPORT int NbEdges() const;

  //! returns the num'th model space curve
  //! raises Exception if num <= 0 or num > NbEdges()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> Curve(const int num) const;

  //! returns the num'th start vertex list
  //! raises Exception if num <= 0 or num > NbEdges()
  Standard_EXPORT occ::handle<IGESSolid_VertexList> StartVertexList(const int num) const;

  //! returns the index of num'th start vertex in
  //! the corresponding start vertex list
  //! raises Exception if num <= 0 or num > NbEdges()
  Standard_EXPORT int StartVertexIndex(const int num) const;

  //! returns the num'th end vertex list
  //! raises Exception if num <= 0 or num > NbEdges()
  Standard_EXPORT occ::handle<IGESSolid_VertexList> EndVertexList(const int num) const;

  //! returns the index of num'th end vertex in
  //! the corresponding end vertex list
  //! raises Exception if num <= 0 or num > NbEdges()
  Standard_EXPORT int EndVertexIndex(const int num) const;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_EdgeList, IGESData_IGESEntity)

private:
  occ::handle<IGESData_HArray1OfIGESEntity>  theCurves;
  occ::handle<IGESSolid_HArray1OfVertexList> theStartVertexList;
  occ::handle<TColStd_HArray1OfInteger>      theStartVertexIndex;
  occ::handle<IGESSolid_HArray1OfVertexList> theEndVertexList;
  occ::handle<TColStd_HArray1OfInteger>      theEndVertexIndex;
};

#endif // _IGESSolid_EdgeList_HeaderFile
