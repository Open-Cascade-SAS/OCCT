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

#ifndef _CPnts_UniformDeflection_HeaderFile
#define _CPnts_UniformDeflection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
class Adaptor3d_Curve;
class Adaptor2d_Curve2d;


//! This class defines an algorithm to create a set of points
//! (with a given chordal deviation) at the
//! positions of constant deflection of a given parametrized curve or a trimmed
//! circle.
//! The continuity of the curve must be at least C2.
//!
//! the usage of the is the following.
//!
//! class myUniformDFeflection instantiates
//! UniformDeflection(Curve, Tool);
//!
//! Curve C; // Curve inherits from Curve or Curve2d from Adaptor2d
//! myUniformDeflection Iter1;
//! DefPntOfmyUniformDeflection P;
//!
//! for(Iter1.Initialize(C, Deflection, EPSILON, True);
//! Iter1.More();
//! Iter1.Next()) {
//! P = Iter1.Value();
//! ... make something with P
//! }
//! if(!Iter1.IsAllDone()) {
//! ... something wrong happened
//! }
class CPnts_UniformDeflection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creation of a indefinite UniformDeflection
  Standard_EXPORT CPnts_UniformDeflection();
  
  //! Computes a uniform deflection distribution of points
  //! on the curve <C>.
  //! <Deflection> defines the constant deflection value.
  //! The algorithm computes the number of points and the points.
  //! The curve <C> must be at least C2 else the computation can fail.
  //! If just some parts of the curve is C2 it is better to give the
  //! parameters bounds and to use the below constructor .
  //! if <WithControl> is True, the algorithm controls the estimate
  //! deflection
  //! when the curve is singular at the point P(u),the algorithm
  //! computes the next point as
  //! P(u + Max(CurrentStep,Abs(LastParameter-FirstParameter)))
  //! if the singularity is at the first point ,the next point
  //! calculated is the P(LastParameter)
  Standard_EXPORT CPnts_UniformDeflection(const Adaptor3d_Curve& C, const Standard_Real Deflection, const Standard_Real Resolution, const Standard_Boolean WithControl);
  
  //! As above with 2d curve
  Standard_EXPORT CPnts_UniformDeflection(const Adaptor2d_Curve2d& C, const Standard_Real Deflection, const Standard_Real Resolution, const Standard_Boolean WithControl);
  

  //! Computes an uniform deflection distribution of points on a part of
  //! the curve <C>. Deflection defines the step between the points.
  //! <U1> and <U2> define the distribution span.
  //! <U1> and <U2> must be in the parametric range of the curve.
  Standard_EXPORT CPnts_UniformDeflection(const Adaptor3d_Curve& C, const Standard_Real Deflection, const Standard_Real U1, const Standard_Real U2, const Standard_Real Resolution, const Standard_Boolean WithControl);
  
  //! As above with 2d curve
  Standard_EXPORT CPnts_UniformDeflection(const Adaptor2d_Curve2d& C, const Standard_Real Deflection, const Standard_Real U1, const Standard_Real U2, const Standard_Real Resolution, const Standard_Boolean WithControl);
  
  //! Initialize the algorithms with <C>, <Deflection>, <UStep>,
  //! <Resolution> and <WithControl>
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Real Deflection, const Standard_Real Resolution, const Standard_Boolean WithControl);
  
  //! Initialize the algorithms with <C>, <Deflection>, <UStep>,
  //! <Resolution> and <WithControl>
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Real Deflection, const Standard_Real Resolution, const Standard_Boolean WithControl);
  
  //! Initialize the algorithms with <C>, <Deflection>, <UStep>,
  //! <U1>, <U2> and <WithControl>
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Real Deflection, const Standard_Real U1, const Standard_Real U2, const Standard_Real Resolution, const Standard_Boolean WithControl);
  
  //! Initialize the algorithms with <C>, <Deflection>, <UStep>,
  //! <U1>, <U2> and <WithControl>
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Real Deflection, const Standard_Real U1, const Standard_Real U2, const Standard_Real Resolution, const Standard_Boolean WithControl);
  
  //! To know if all the calculus were done successfully
  //! (ie all the points have been computed). The calculus can fail if
  //! the Curve is not C1 in the considered domain.
  //! Returns True if the calculus was successful.
    Standard_Boolean IsAllDone() const;
  
  //! go to the next Point.
    void Next();
  
  //! returns True if it exists a next Point.
  Standard_EXPORT Standard_Boolean More();
  
  //! return the computed parameter
    Standard_Real Value() const;
  
  //! return the computed parameter
    gp_Pnt Point() const;




protected:





private:

  
  //! algorithm
  Standard_EXPORT void Perform();


  Standard_Boolean myDone;
  Standard_Boolean my3d;
  Standard_Address myCurve;
  Standard_Boolean myFinish;
  Standard_Real myTolCur;
  Standard_Boolean myControl;
  Standard_Integer myIPoint;
  Standard_Integer myNbPoints;
  Standard_Real myParams[3];
  gp_Pnt myPoints[3];
  Standard_Real myDwmax;
  Standard_Real myDeflection;
  Standard_Real myFirstParam;
  Standard_Real myLastParam;
  Standard_Real myDu;


};


#include <CPnts_UniformDeflection.lxx>





#endif // _CPnts_UniformDeflection_HeaderFile
