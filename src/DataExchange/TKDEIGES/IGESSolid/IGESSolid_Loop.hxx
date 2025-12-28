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

#ifndef _IGESSolid_Loop_HeaderFile
#define _IGESSolid_Loop_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESBasic_HArray1OfHArray1OfInteger;
class IGESBasic_HArray1OfHArray1OfIGESEntity;

//! defines Loop, Type <508> Form Number <1>
//! in package IGESSolid
//! A Loop entity specifies a bound of a face. It represents
//! a connected collection of face boundaries, seams, and
//! poles of a single face.
//!
//! From IGES-5.3, a Loop can be free with Form Number 0,
//! else it is a bound of a face (it is the default)
class IGESSolid_Loop : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESSolid_Loop();

  //! This method is used to set the fields of the class Loop
  //! - types              : 0 = Edge; 1 = Vertex
  //! - edges              : Pointer to the EdgeList or VertexList
  //! - index              : Index of the edge into the EdgeList
  //! VertexList entity
  //! - orient             : Orientation flag of the edge
  //! - nbParameterCurves  : the number of parameter space curves
  //! for each edge
  //! - isoparametricFlags : the isoparametric flag of the
  //! parameter space curve
  //! - curves             : the parameter space curves
  //! raises exception if length of types, edges, index, orient and
  //! nbParameterCurves do not match or the length of
  //! isoparametricFlags and curves do not match
  Standard_EXPORT void Init(const occ::handle<NCollection_HArray1<int>>&            types,
                            const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&        edges,
                            const occ::handle<NCollection_HArray1<int>>&            index,
                            const occ::handle<NCollection_HArray1<int>>&            orient,
                            const occ::handle<NCollection_HArray1<int>>&            nbParameterCurves,
                            const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>& isoparametricFlags,
                            const occ::handle<IGESBasic_HArray1OfHArray1OfIGESEntity>& curves);

  //! Tells if a Loop is a Bound (FN 1) else it is free (FN 0)
  Standard_EXPORT bool IsBound() const;

  //! Sets or Unset the Bound Status (from Form Number)
  //! Default is True
  Standard_EXPORT void SetBound(const bool bound);

  //! returns the number of edge tuples
  Standard_EXPORT int NbEdges() const;

  //! returns the type of Index'th edge (0 = Edge, 1 = Vertex)
  //! raises exception if Index <= 0 or Index > NbEdges()
  Standard_EXPORT int EdgeType(const int Index) const;

  //! return the EdgeList or VertexList corresponding to the Index
  //! raises exception if Index <= 0 or Index > NbEdges()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> Edge(const int Index) const;

  //! returns the orientation flag corresponding to Index'th edge
  //! raises exception if Index <= 0 or Index > NbEdges()
  Standard_EXPORT bool Orientation(const int Index) const;

  //! return the number of parameter space curves associated with
  //! Index'th Edge
  //! raises exception if Index <= 0 or Index > NbEdges()
  Standard_EXPORT int NbParameterCurves(const int Index) const;

  Standard_EXPORT bool IsIsoparametric(const int EdgeIndex,
                                                   const int CurveIndex) const;

  //! returns the CurveIndex'th parameter space curve associated with
  //! EdgeIndex'th edge
  //! raises exception if EdgeIndex <= 0 or EdgeIndex > NbEdges() or
  //! if CurveIndex <= 0 or CurveIndex > NbParameterCurves(EdgeIndex)
  Standard_EXPORT occ::handle<IGESData_IGESEntity> ParametricCurve(
    const int EdgeIndex,
    const int CurveIndex) const;

  //! raises exception If num <= 0 or num > NbEdges()
  Standard_EXPORT int ListIndex(const int num) const;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_Loop, IGESData_IGESEntity)

private:
  occ::handle<NCollection_HArray1<int>>               theTypes;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>           theEdges;
  occ::handle<NCollection_HArray1<int>>               theIndex;
  occ::handle<NCollection_HArray1<int>>               theOrientationFlags;
  occ::handle<NCollection_HArray1<int>>               theNbParameterCurves;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger>    theIsoparametricFlags;
  occ::handle<IGESBasic_HArray1OfHArray1OfIGESEntity> theCurves;
};

#endif // _IGESSolid_Loop_HeaderFile
