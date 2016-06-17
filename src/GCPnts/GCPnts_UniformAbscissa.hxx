// Created on: 1991-02-27
// Created by: Jean Claude Vauthier
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

#ifndef _GCPnts_UniformAbscissa_HeaderFile
#define _GCPnts_UniformAbscissa_HeaderFile

#include <StdFail_NotDone.hxx>
#include <TColStd_HArray1OfReal.hxx>

class Standard_DomainError;
class Standard_ConstructionError;
class Standard_OutOfRange;
class StdFail_NotDone;
class Adaptor3d_Curve;
class Adaptor2d_Curve2d;

//! This class allows to compute a uniform distribution of points
//! on a curve (ie the points will all be equally distant).
class GCPnts_UniformAbscissa 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creation of a indefinite UniformAbscissa
  Standard_EXPORT GCPnts_UniformAbscissa();
  
  //! Computes a uniform abscissa distribution of points on
  //! the Curve <C>. Parameter Toler is equal Precision::Confusion by default.
  //! It Is used for more precise calculation of curve length
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor3d_Curve& C, const Standard_Real Abscissa, const Standard_Real Toler = -1);
  
  //! Computes a Uniform abscissa distribution of points
  //! on a part of the Curve <C>. Parameter Toler is equal Precision::Confusion by default.
  //! It Is used for more precise calculation of curve length
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor3d_Curve& C, const Standard_Real Abscissa, const Standard_Real U1, const Standard_Real U2, const Standard_Real Toler = -1);
  
  //! Computes a uniform abscissa distribution of points on
  //! the Curve <C>.
  //! <NbPoints> defines the nomber of desired points.
  //! Parameter Toler is equal Precision::Confusion by default.
  //! It Is used for more precise calculation of curve length
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor3d_Curve& C, const Standard_Integer NbPoints, const Standard_Real Toler = -1);
  
  //! Computes a Uniform abscissa distribution of points
  //! on a part of the Curve <C>.
  //! Parameter Toler is equal Precision::Confusion by default.
  //! It Is used for more precise calculation of curve length
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor3d_Curve& C, const Standard_Integer NbPoints, const Standard_Real U1, const Standard_Real U2, const Standard_Real Toler = -1);
  
  //! Initialize the algoritms with <C>, <Abscissa>, <Toler>
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Real Abscissa, const Standard_Real Toler = -1);
  
  //! Initialize the algoritms with <C>, <Abscissa>, <U1>,
  //! <U2>, <Toler>
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Real Abscissa, const Standard_Real U1, const Standard_Real U2, const Standard_Real Toler = -1);
  
  //! Initialize the algoritms with <C>, <NbPoints>, <Toler> and
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Integer NbPoints, const Standard_Real Toler = -1);
  
  //! Initialize the algoritms with <C>, <Abscissa>, <U1>,
  //! <U2>, <Toler>.
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Integer NbPoints, const Standard_Real U1, const Standard_Real U2, const Standard_Real Toler = -1);
  
  //! Computes a uniform abscissa distribution of points on
  //! the Curve2d <C>.
  //! Parameter Toler is equal Precision::Confusion by default.
  //! It Is used for more precise calculation of curve length
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& C, const Standard_Real Abscissa, const Standard_Real Toler = -1);
  
  //! Computes a Uniform abscissa distribution of points
  //! on a part of the Curve2d <C>.
  //! Parameter Toler is equal Precision::Confusion by default.
  //! It Is used for more precise calculation of curve length
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& C, const Standard_Real Abscissa, const Standard_Real U1, const Standard_Real U2, const Standard_Real Toler = -1);
  
  //! Computes a uniform abscissa distribution of points on
  //! the Curve2d <C>.
  //! <NbPoints> defines the nomber of desired points.
  //! Parameter Toler is equal Precision::Confusion by default.
  //! It Is used for more precise calculation of curve length
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& C, const Standard_Integer NbPoints, const Standard_Real Toler = -1);
  
  //! Computes a Uniform abscissa distribution of points
  //! on a part of the Curve2d <C>.
  //! Parameter Toler is equal Precision::Confusion by default.
  //! It Is used for more precise calculation of curve length
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& C, const Standard_Integer NbPoints, const Standard_Real U1, const Standard_Real U2, const Standard_Real Toler = -1);
  
  //! Initialize the algoritms with <C>, <Abscissa>, <Toler>
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Real Abscissa, const Standard_Real Toler = -1);
  
  //! Initialize the algoritms with <C>, <Abscissa>, <U1>,
  //! <U2>, <Toler>
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Real Abscissa, const Standard_Real U1, const Standard_Real U2, const Standard_Real Toler = -1);
  
  //! Initialize the algoritms with <C>, <NbPoints>, <Toler> and
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Integer NbPoints, const Standard_Real Toler = -1);
  
  //! Initialize the algoritms with <C>, <Abscissa>, <U1>,
  //! <U2>, <Toler>.
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Integer NbPoints, const Standard_Real U1, const Standard_Real U2, const Standard_Real Toler = -1);
  
  Standard_Boolean IsDone () const
  {
    return myDone;
  }
  
  Standard_Integer NbPoints () const
  {
    StdFail_NotDone_Raise_if (!myDone, "GCPnts_UniformAbscissa::NbPoints()");
    return myNbPoints;
  }
  
  //! returns the computed Parameter of index <Index>.
  Standard_Real Parameter (const Standard_Integer Index) const
  {
    StdFail_NotDone_Raise_if (!myDone, "GCPnts_UniformAbscissa::Parameter()");
    return myParams->Value (Index);
  }
  
  //! returne the current abscissa
  //! ie the distance between two consecutive points
  Standard_Real Abscissa () const
  {
    StdFail_NotDone_Raise_if (!myDone, "GCPnts_UniformAbscissa::Abscissa()");
    return myAbscissa;
  }

private:
  Standard_Boolean myDone;
  Standard_Integer myNbPoints;
  Standard_Real myAbscissa;
  Handle(TColStd_HArray1OfReal) myParams;
};

#endif // _GCPnts_UniformAbscissa_HeaderFile
