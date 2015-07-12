// Created on: 1991-04-15
// Created by: Philippe DAUTRY
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

#ifndef _Geom2dGcc_QCurve_HeaderFile
#define _Geom2dGcc_QCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GccEnt_Position.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Standard_Boolean.hxx>
class Geom2dAdaptor_Curve;


//! Creates a qualified 2d line.
class Geom2dGcc_QCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dGcc_QCurve(const Geom2dAdaptor_Curve& Curve, const GccEnt_Position Qualifier);
  
  Standard_EXPORT Geom2dAdaptor_Curve Qualified() const;
  
  Standard_EXPORT GccEnt_Position Qualifier() const;
  
  //! Returns true if the solution is unqualified and false in the
  //! other cases.
  Standard_EXPORT Standard_Boolean IsUnqualified() const;
  
  //! Returns true if the solution is Enclosing the Curv and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsEnclosing() const;
  
  //! Returns true if the solution is Enclosed in the Curv and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsEnclosed() const;
  
  //! Returns true if the solution is Outside the Curv and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsOutside() const;




protected:





private:



  GccEnt_Position TheQualifier;
  Geom2dAdaptor_Curve TheQualified;


};







#endif // _Geom2dGcc_QCurve_HeaderFile
