// Created on: 1996-12-05
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _GeomFill_Tensor_HeaderFile
#define _GeomFill_Tensor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Array1.hxx>
#include <math_Vector.hxx>
class math_Matrix;

//! used to store the "gradient of gradient"
class GeomFill_Tensor
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_Tensor(const int NbRow, const int NbCol, const int NbMat);

  //! Initialize all the elements of a Tensor to InitialValue.
  Standard_EXPORT void Init(const double InitialValue);

  //! accesses (in read or write mode) the value of index <Row>,
  //! <Col> and <Mat> of a Tensor.
  //! An exception is raised if <Row>, <Col> or <Mat> are not
  //! in the correct range.
  const double& Value(const int Row, const int Col, const int Mat) const;

  const double& operator()(const int Row, const int Col, const int Mat) const
  {
    return Value(Row, Col, Mat);
  }

  //! accesses (in read or write mode) the value of index <Row>,
  //! <Col> and <Mat> of a Tensor.
  //! An exception is raised if <Row>, <Col> or <Mat> are not
  //! in the correct range.
  double& ChangeValue(const int Row, const int Col, const int Mat);

  double& operator()(const int Row, const int Col, const int Mat)
  {
    return ChangeValue(Row, Col, Mat);
  }

  Standard_EXPORT void Multiply(const math_Vector& Right, math_Matrix& Product) const;

private:
  NCollection_Array1<double> Tab;
  int                        nbrow;
  int                        nbcol;
  int                        nbmat;
  int                        nbmtcl;
};

#include <GeomFill_Tensor.lxx>

#endif // _GeomFill_Tensor_HeaderFile
