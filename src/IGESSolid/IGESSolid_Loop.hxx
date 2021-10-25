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

#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class IGESBasic_HArray1OfHArray1OfInteger;
class IGESBasic_HArray1OfHArray1OfIGESEntity;


class IGESSolid_Loop;
DEFINE_STANDARD_HANDLE(IGESSolid_Loop, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Handle(TColStd_HArray1OfInteger)& types, const Handle(IGESData_HArray1OfIGESEntity)& edges, const Handle(TColStd_HArray1OfInteger)& index, const Handle(TColStd_HArray1OfInteger)& orient, const Handle(TColStd_HArray1OfInteger)& nbParameterCurves, const Handle(IGESBasic_HArray1OfHArray1OfInteger)& isoparametricFlags, const Handle(IGESBasic_HArray1OfHArray1OfIGESEntity)& curves);
  
  //! Tells if a Loop is a Bound (FN 1) else it is free (FN 0)
  Standard_EXPORT Standard_Boolean IsBound() const;
  
  //! Sets or Unset the Bound Status (from Form Number)
  //! Default is True
  Standard_EXPORT void SetBound (const Standard_Boolean bound);
  
  //! returns the number of edge tuples
  Standard_EXPORT Standard_Integer NbEdges() const;
  
  //! returns the type of Index'th edge (0 = Edge, 1 = Vertex)
  //! raises exception if Index <= 0 or Index > NbEdges()
  Standard_EXPORT Standard_Integer EdgeType (const Standard_Integer Index) const;
  
  //! return the EdgeList or VertexList corresponding to the Index
  //! raises exception if Index <= 0 or Index > NbEdges()
  Standard_EXPORT Handle(IGESData_IGESEntity) Edge (const Standard_Integer Index) const;
  
  //! returns the orientation flag corresponding to Index'th edge
  //! raises exception if Index <= 0 or Index > NbEdges()
  Standard_EXPORT Standard_Boolean Orientation (const Standard_Integer Index) const;
  
  //! return the number of parameter space curves associated with
  //! Index'th Edge
  //! raises exception if Index <= 0 or Index > NbEdges()
  Standard_EXPORT Standard_Integer NbParameterCurves (const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Boolean IsIsoparametric (const Standard_Integer EdgeIndex, const Standard_Integer CurveIndex) const;
  
  //! returns the CurveIndex'th parameter space curve associated with
  //! EdgeIndex'th edge
  //! raises exception if EdgeIndex <= 0 or EdgeIndex > NbEdges() or
  //! if CurveIndex <= 0 or CurveIndex > NbParameterCurves(EdgeIndex)
  Standard_EXPORT Handle(IGESData_IGESEntity) ParametricCurve (const Standard_Integer EdgeIndex, const Standard_Integer CurveIndex) const;
  
  //! raises exception If num <= 0 or num > NbEdges()
  Standard_EXPORT Standard_Integer ListIndex (const Standard_Integer num) const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_Loop,IGESData_IGESEntity)

protected:




private:


  Handle(TColStd_HArray1OfInteger) theTypes;
  Handle(IGESData_HArray1OfIGESEntity) theEdges;
  Handle(TColStd_HArray1OfInteger) theIndex;
  Handle(TColStd_HArray1OfInteger) theOrientationFlags;
  Handle(TColStd_HArray1OfInteger) theNbParameterCurves;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) theIsoparametricFlags;
  Handle(IGESBasic_HArray1OfHArray1OfIGESEntity) theCurves;


};







#endif // _IGESSolid_Loop_HeaderFile
