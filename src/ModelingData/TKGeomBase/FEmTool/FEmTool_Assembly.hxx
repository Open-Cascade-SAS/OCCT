// Created on: 1997-10-29
// Created by: Roman BORISOV
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

#ifndef _FEmTool_Assembly_HeaderFile
#define _FEmTool_Assembly_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_HArray2.hxx>
#include <math_Vector.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Real.hxx>
class FEmTool_ProfileMatrix;
class math_Matrix;

//! Assemble and solve system from (one dimensional) Finite Elements
class FEmTool_Assembly
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT FEmTool_Assembly(
    const NCollection_Array2<int>&                                                 Dependence,
    const occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>& Table);

  //! Nullify all Matrix's Coefficient
  Standard_EXPORT void NullifyMatrix();

  //! Add an elementary Matrix in the assembly Matrix
  //! if Dependence(Dimension1,Dimension2) is False
  Standard_EXPORT void AddMatrix(const int          Element,
                                 const int          Dimension1,
                                 const int          Dimension2,
                                 const math_Matrix& Mat);

  //! Nullify all Coordinate of assembly Vector (second member)
  Standard_EXPORT void NullifyVector();

  //! Add an elementary Vector in the assembly Vector (second member)
  Standard_EXPORT void AddVector(const int Element, const int Dimension, const math_Vector& Vec);

  //! Delete all Constraints.
  Standard_EXPORT void ResetConstraint();

  //! Nullify all Constraints.
  Standard_EXPORT void NullifyConstraint();

  Standard_EXPORT void AddConstraint(const int          IndexofConstraint,
                                     const int          Element,
                                     const int          Dimension,
                                     const math_Vector& LinearForm,
                                     const double       Value);

  //! Solve the assembly system
  //! Returns false if the computation failed.
  Standard_EXPORT bool Solve();

  Standard_EXPORT void Solution(math_Vector& Solution) const;

  Standard_EXPORT int NbGlobVar() const;

  Standard_EXPORT void GetAssemblyTable(
    occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>& AssTable) const;

private:
  NCollection_Array2<int>                                                          myDepTable;
  occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>          myRefTable;
  bool                                                                             IsSolved;
  occ::handle<FEmTool_ProfileMatrix>                                               H;
  math_Vector                                                                      B;
  occ::handle<FEmTool_ProfileMatrix>                                               GHGt;
  NCollection_Sequence<NCollection_List<occ::handle<NCollection_HArray1<double>>>> G;
  NCollection_Sequence<double>                                                     C;
};

#endif // _FEmTool_Assembly_HeaderFile
