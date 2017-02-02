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
//pdn 20.04.99 CTS22655 avoid of exceptions on empty loops

#include <IGESBasic_HArray1OfHArray1OfIGESEntity.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_Loop,IGESData_IGESEntity)

IGESSolid_Loop::IGESSolid_Loop ()    {  }


    void  IGESSolid_Loop::Init
  (const Handle(TColStd_HArray1OfInteger)& Types,
   const Handle(IGESData_HArray1OfIGESEntity)& Edges,
   const Handle(TColStd_HArray1OfInteger)& Index,
   const Handle(TColStd_HArray1OfInteger)& Orient,
   const Handle(TColStd_HArray1OfInteger)& nbParameterCurves,
   const Handle(IGESBasic_HArray1OfHArray1OfInteger)& IsoparametricFlags,
   const Handle(IGESBasic_HArray1OfHArray1OfIGESEntity)& Curves)
{
  Standard_Integer nb = Types->Length();

  if (Types->Lower()  != 1 ||
      Edges->Lower()  != 1 || nb != Edges->Length()  ||
      Index->Lower()  != 1 || nb != Index->Length()  ||
      Orient->Lower() != 1 || nb != Orient->Length() ||
      nbParameterCurves->Lower() != 1 || nb != nbParameterCurves->Length() ||
      IsoparametricFlags->Lower() != 1 || nb != IsoparametricFlags->Length() ||
      Curves->Lower() != 1 || nb != Curves->Length() )
    throw Standard_DimensionError("IGESSolid_Loop : Init");

  theTypes  = Types;
  theEdges  = Edges;
  theIndex  = Index;
  theOrientationFlags   = Orient;
  theNbParameterCurves  = nbParameterCurves;
  theIsoparametricFlags = IsoparametricFlags;
  theCurves = Curves;

  InitTypeAndForm(508,1);
}

    Standard_Boolean  IGESSolid_Loop::IsBound () const
      {  return (FormNumber() == 1);  }

    void  IGESSolid_Loop::SetBound (const Standard_Boolean bound)
      {  InitTypeAndForm(508, (bound ? 1 : 0));  }


    Standard_Integer  IGESSolid_Loop::NbEdges () const
{
  //pdn 20.04.99 CTS22655 to avoid exceptions on empty loops
  if(theEdges.IsNull()) return 0;
  return theEdges->Length();
}

    Standard_Integer  IGESSolid_Loop::EdgeType (const Standard_Integer Index) const
{
  return theTypes->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESSolid_Loop::Edge
  (const Standard_Integer Index) const
{
  return theEdges->Value(Index);
}

    Standard_Boolean IGESSolid_Loop::Orientation (const Standard_Integer Index) const
{
  return (theOrientationFlags->Value(Index) != 0);
}

    Standard_Integer  IGESSolid_Loop::NbParameterCurves
  (const Standard_Integer Index) const
{
  return theNbParameterCurves->Value(Index);
}

    Standard_Boolean  IGESSolid_Loop::IsIsoparametric
  (const Standard_Integer EdgeIndex, const Standard_Integer CurveIndex) const
{
  if (!theIsoparametricFlags->Value(EdgeIndex).IsNull()) return
    (theIsoparametricFlags->Value(EdgeIndex)->Value(CurveIndex) != 0);
  else return Standard_False;  // faut bien dire qq chose
}

    Handle(IGESData_IGESEntity)  IGESSolid_Loop::ParametricCurve
  (const Standard_Integer EdgeIndex, const Standard_Integer CurveIndex) const
{
  Handle(IGESData_IGESEntity) acurve;    // par defaut sera nulle
  if (!theCurves->Value(EdgeIndex).IsNull()) acurve =
    theCurves->Value(EdgeIndex)->Value(CurveIndex);
  return acurve;
}

    Standard_Integer  IGESSolid_Loop::ListIndex (const Standard_Integer num) const
{
  return theIndex->Value(num);
}
