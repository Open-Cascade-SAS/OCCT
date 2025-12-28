// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// pdn 20.04.99 CTS22655 avoid of exceptions on empty loops

#include <IGESBasic_HArray1OfHArray1OfIGESEntity.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_Loop, IGESData_IGESEntity)

IGESSolid_Loop::IGESSolid_Loop() {}

void IGESSolid_Loop::Init(
  const occ::handle<NCollection_HArray1<int>>&                              Types,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& Edges,
  const occ::handle<NCollection_HArray1<int>>&                              Index,
  const occ::handle<NCollection_HArray1<int>>&                              Orient,
  const occ::handle<NCollection_HArray1<int>>&                              nbParameterCurves,
  const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>&                   IsoparametricFlags,
  const occ::handle<IGESBasic_HArray1OfHArray1OfIGESEntity>&                Curves)
{
  int nb = Types->Length();

  if (Types->Lower() != 1 || Edges->Lower() != 1 || nb != Edges->Length() || Index->Lower() != 1
      || nb != Index->Length() || Orient->Lower() != 1 || nb != Orient->Length()
      || nbParameterCurves->Lower() != 1 || nb != nbParameterCurves->Length()
      || IsoparametricFlags->Lower() != 1 || nb != IsoparametricFlags->Length()
      || Curves->Lower() != 1 || nb != Curves->Length())
    throw Standard_DimensionError("IGESSolid_Loop : Init");

  theTypes              = Types;
  theEdges              = Edges;
  theIndex              = Index;
  theOrientationFlags   = Orient;
  theNbParameterCurves  = nbParameterCurves;
  theIsoparametricFlags = IsoparametricFlags;
  theCurves             = Curves;

  InitTypeAndForm(508, 1);
}

bool IGESSolid_Loop::IsBound() const
{
  return (FormNumber() == 1);
}

void IGESSolid_Loop::SetBound(const bool bound)
{
  InitTypeAndForm(508, (bound ? 1 : 0));
}

int IGESSolid_Loop::NbEdges() const
{
  // pdn 20.04.99 CTS22655 to avoid exceptions on empty loops
  if (theEdges.IsNull())
    return 0;
  return theEdges->Length();
}

int IGESSolid_Loop::EdgeType(const int Index) const
{
  return theTypes->Value(Index);
}

occ::handle<IGESData_IGESEntity> IGESSolid_Loop::Edge(const int Index) const
{
  return theEdges->Value(Index);
}

bool IGESSolid_Loop::Orientation(const int Index) const
{
  return (theOrientationFlags->Value(Index) != 0);
}

int IGESSolid_Loop::NbParameterCurves(const int Index) const
{
  return theNbParameterCurves->Value(Index);
}

bool IGESSolid_Loop::IsIsoparametric(const int EdgeIndex, const int CurveIndex) const
{
  if (!theIsoparametricFlags->Value(EdgeIndex).IsNull())
    return (theIsoparametricFlags->Value(EdgeIndex)->Value(CurveIndex) != 0);
  else
    return false; // must say something
}

occ::handle<IGESData_IGESEntity> IGESSolid_Loop::ParametricCurve(const int EdgeIndex,
                                                                 const int CurveIndex) const
{
  occ::handle<IGESData_IGESEntity> acurve; // by default will be null
  if (!theCurves->Value(EdgeIndex).IsNull())
    acurve = theCurves->Value(EdgeIndex)->Value(CurveIndex);
  return acurve;
}

int IGESSolid_Loop::ListIndex(const int num) const
{
  return theIndex->Value(num);
}
