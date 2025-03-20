// Created on: 1991-01-21
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _math_HeaderFile
#define _math_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Vector.hxx>

class math
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static Standard_Integer GaussPointsMax();

  Standard_EXPORT static void GaussPoints(const Standard_Integer Index, math_Vector& Points);

  Standard_EXPORT static void GaussWeights(const Standard_Integer Index, math_Vector& Weights);

  //! Returns the maximal number of points for that the values
  //! are stored in the table. If the number is greater then
  //! KronrodPointsMax, the points will be computed.
  Standard_EXPORT static Standard_Integer KronrodPointsMax();

  //! Returns a vector of Gauss points and a vector of their weights.
  //! The difference with the
  //! method GaussPoints is the following:
  //! - the points are returned in increasing order.
  //! - if Index is greater then GaussPointsMax, the points are
  //! computed.
  //! Returns Standard_True if Index is positive, Points' and Weights'
  //! length is equal to Index, Points and Weights are successfully computed.
  Standard_EXPORT static Standard_Boolean OrderedGaussPointsAndWeights(const Standard_Integer Index,
                                                                       math_Vector& Points,
                                                                       math_Vector& Weights);

  //! Returns a vector of Kronrod points and a vector of their
  //! weights for Gauss-Kronrod computation method.
  //! Index should be odd and greater then or equal to 3,
  //! as the number of Kronrod points is equal to 2*N + 1,
  //! where N is a number of Gauss points. Points and Weights should
  //! have the size equal to Index. Each even element of Points
  //! represents a Gauss point value of N-th Gauss quadrature.
  //! The values from Index equal to 3 to 123 are stored in a
  //! table (see the file math_Kronrod.cxx). If Index is greater,
  //! then points and weights will be computed. Returns Standard_True
  //! if Index is odd, it is equal to the size of Points and Weights
  //! and the computation of Points and Weights is performed successfully.
  //! Otherwise this method returns Standard_False.
  Standard_EXPORT static Standard_Boolean KronrodPointsAndWeights(const Standard_Integer Index,
                                                                  math_Vector&           Points,
                                                                  math_Vector&           Weights);
};

#endif // _math_HeaderFile
