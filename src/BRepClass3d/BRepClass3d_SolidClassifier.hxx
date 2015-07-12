// Created on: 1994-03-30
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepClass3d_SolidClassifier_HeaderFile
#define _BRepClass3d_SolidClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepClass3d_SClassifier.hxx>
#include <Standard_Real.hxx>
class Standard_DomainError;
class TopoDS_Shape;
class gp_Pnt;


//! Provides an algorithm to classify a point in a solid.
class BRepClass3d_SolidClassifier  : public BRepClass3d_SClassifier
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! empty constructor
  Standard_EXPORT BRepClass3d_SolidClassifier();
  
  Standard_EXPORT void Load (const TopoDS_Shape& S);
  
  //! Constructor from a Shape.
  Standard_EXPORT BRepClass3d_SolidClassifier(const TopoDS_Shape& S);
  
  //! Constructor to classify the point P with the
  //! tolerance Tol on the solid S.
  Standard_EXPORT BRepClass3d_SolidClassifier(const TopoDS_Shape& S, const gp_Pnt& P, const Standard_Real Tol);
  
  //! Classify the point P with the
  //! tolerance Tol on the solid S.
  Standard_EXPORT void Perform (const gp_Pnt& P, const Standard_Real Tol);
  
  //! Classify an infinite point with the
  //! tolerance Tol on the solid S.
  //! Useful for compute the orientation of a solid.
  Standard_EXPORT void PerformInfinitePoint (const Standard_Real Tol);
  
  Standard_EXPORT void Destroy();
~BRepClass3d_SolidClassifier()
{
  Destroy();
}




protected:





private:



  Standard_Boolean aSolidLoaded;
  BRepClass3d_SolidExplorer explorer;
  Standard_Boolean isaholeinspace;


};







#endif // _BRepClass3d_SolidClassifier_HeaderFile
