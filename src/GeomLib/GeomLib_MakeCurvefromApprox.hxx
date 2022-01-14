// Created on: 1995-06-13
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomLib_MakeCurvefromApprox_HeaderFile
#define _GeomLib_MakeCurvefromApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AdvApprox_ApproxAFunction.hxx>
#include <Standard_Integer.hxx>
class Geom2d_BSplineCurve;
class Geom_BSplineCurve;


//! this class is used to  construct the BSpline curve
//! from an Approximation ( ApproxAFunction from AdvApprox).
class GeomLib_MakeCurvefromApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomLib_MakeCurvefromApprox(const AdvApprox_ApproxAFunction& Approx);
  
    Standard_Boolean IsDone() const;
  
  //! returns the number of 1D spaces of the Approx
  Standard_EXPORT Standard_Integer Nb1DSpaces() const;
  
  //! returns the number of 3D spaces of the Approx
  Standard_EXPORT Standard_Integer Nb2DSpaces() const;
  
  //! returns the number of 3D spaces of the Approx
  Standard_EXPORT Standard_Integer Nb3DSpaces() const;
  
  //! returns a polynomial curve whose poles correspond to
  //! the Index2d 2D space
  //! if Index2d not in the Range [1,Nb2dSpaces]
  //! if the Approx is not Done
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Curve2d (const Standard_Integer Index2d) const;
  
  //! returns a 2D curve building it from the 1D curve
  //! in x at Index1d and y at Index2d amongst the
  //! 1D curves
  //! if Index1d not in the Range [1,Nb1dSpaces]
  //! if Index2d not in the Range [1,Nb1dSpaces]
  //! if the Approx is not Done
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Curve2dFromTwo1d (const Standard_Integer Index1d, const Standard_Integer Index2d) const;
  
  //! returns a rational curve whose poles correspond to
  //! the index2d of the 2D space and whose weights correspond
  //! to one dimensional space of index 1d
  //! if Index1d not in the Range [1,Nb1dSpaces]
  //! if Index2d not in the Range [1,Nb2dSpaces]
  //! if the Approx is not Done
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Curve2d (const Standard_Integer Index1d, const Standard_Integer Index2d) const;
  
  //! returns a polynomial curve whose poles correspond to
  //! the Index3D 3D space
  //! if Index3D not in the Range [1,Nb3dSpaces]
  //! if the Approx is not Done
  Standard_EXPORT Handle(Geom_BSplineCurve) Curve (const Standard_Integer Index3d) const;
  
  //! returns a rational curve whose poles correspond to
  //! the index3D of the 3D space and whose weights correspond
  //! to the index1d 1D space.
  //! if Index1D not in the Range [1,Nb1dSpaces]
  //! if Index3D not in the Range [1,Nb3dSpaces]
  //! if the Approx is not Done
  Standard_EXPORT Handle(Geom_BSplineCurve) Curve (const Standard_Integer Index1D, const Standard_Integer Index3D) const;




protected:





private:



  AdvApprox_ApproxAFunction myApprox;


};


#include <GeomLib_MakeCurvefromApprox.lxx>





#endif // _GeomLib_MakeCurvefromApprox_HeaderFile
