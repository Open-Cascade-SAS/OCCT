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

#include <TColStd_Array2OfInteger.hxx>
#include <FEmTool_HAssemblyTable.hxx>
#include <math_Vector.hxx>
#include <FEmTool_SeqOfLinConstr.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class FEmTool_ProfileMatrix;
class math_Matrix;

//! Assemble and solve system from (one dimensional) Finite Elements
class FEmTool_Assembly
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT FEmTool_Assembly(const TColStd_Array2OfInteger&        Dependence,
                                   const Handle(FEmTool_HAssemblyTable)& Table);

  //! Nullify all Matrix 's Coefficient
  Standard_EXPORT void NullifyMatrix();

  //! Add an elementary Matrix in the assembly Matrix
  //! if  Dependence(Dimension1,Dimension2)  is  False
  Standard_EXPORT void AddMatrix(const Standard_Integer Element,
                                 const Standard_Integer Dimension1,
                                 const Standard_Integer Dimension2,
                                 const math_Matrix&     Mat);

  //! Nullify  all  Coordinate of  assembly  Vector (second member)
  Standard_EXPORT void NullifyVector();

  //! Add an elementary Vector in the assembly Vector (second member)
  Standard_EXPORT void AddVector(const Standard_Integer Element,
                                 const Standard_Integer Dimension,
                                 const math_Vector&     Vec);

  //! Delete all Constraints.
  Standard_EXPORT void ResetConstraint();

  //! Nullify all Constraints.
  Standard_EXPORT void NullifyConstraint();

  Standard_EXPORT void AddConstraint(const Standard_Integer IndexofConstraint,
                                     const Standard_Integer Element,
                                     const Standard_Integer Dimension,
                                     const math_Vector&     LinearForm,
                                     const Standard_Real    Value);

  //! Solve the assembly system
  //! Returns Standard_False if the computation failed.
  Standard_EXPORT Standard_Boolean Solve();

  Standard_EXPORT void Solution(math_Vector& Solution) const;

  Standard_EXPORT Standard_Integer NbGlobVar() const;

  Standard_EXPORT void GetAssemblyTable(Handle(FEmTool_HAssemblyTable)& AssTable) const;

protected:
private:
  TColStd_Array2OfInteger        myDepTable;
  Handle(FEmTool_HAssemblyTable) myRefTable;
  Standard_Boolean               IsSolved;
  Handle(FEmTool_ProfileMatrix)  H;
  math_Vector                    B;
  Handle(FEmTool_ProfileMatrix)  GHGt;
  FEmTool_SeqOfLinConstr         G;
  TColStd_SequenceOfReal         C;
};

#endif // _FEmTool_Assembly_HeaderFile
