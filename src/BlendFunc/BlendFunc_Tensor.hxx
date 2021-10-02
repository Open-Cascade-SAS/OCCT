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

#ifndef _BlendFunc_Tensor_HeaderFile
#define _BlendFunc_Tensor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <math_Vector.hxx>
class math_Matrix;


//! used to store the "gradient of gradient"
class BlendFunc_Tensor 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BlendFunc_Tensor(const Standard_Integer NbRow, const Standard_Integer NbCol, const Standard_Integer NbMat);
  
  //! Initialize all the elements of a Tensor to InitialValue.
  Standard_EXPORT void Init (const Standard_Real InitialValue);
  
  //! accesses (in read or write mode) the value of index <Row>,
  //! <Col> and <Mat> of a Tensor.
  //! An exception is raised if <Row>, <Col> or <Mat> are not
  //! in the correct range.
    const Standard_Real& Value (const Standard_Integer Row, const Standard_Integer Col, const Standard_Integer Mat) const;
  const Standard_Real& operator() (const Standard_Integer Row, const Standard_Integer Col, const Standard_Integer Mat) const
{
  return Value(Row,Col,Mat);
}
  
  //! accesses (in read or write mode) the value of index <Row>,
  //! <Col> and <Mat> of a Tensor.
  //! An exception is raised if <Row>, <Col> or <Mat> are not
  //! in the correct range.
    Standard_Real& ChangeValue (const Standard_Integer Row, const Standard_Integer Col, const Standard_Integer Mat);
  Standard_Real& operator() (const Standard_Integer Row, const Standard_Integer Col, const Standard_Integer Mat)
{
  return ChangeValue(Row,Col,Mat);
}
  
  Standard_EXPORT void Multiply (const math_Vector& Right, math_Matrix& Product) const;




protected:





private:



  TColStd_Array1OfReal Tab;
  Standard_Integer nbrow;
  Standard_Integer nbcol;
  Standard_Integer nbmat;
  Standard_Integer nbmtcl;


};


#include <BlendFunc_Tensor.lxx>





#endif // _BlendFunc_Tensor_HeaderFile
