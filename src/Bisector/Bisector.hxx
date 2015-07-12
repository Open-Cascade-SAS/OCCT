// Created on: 1994-05-19
// Created by: Yves FRICAUD
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

#ifndef _Bisector_HeaderFile
#define _Bisector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom2d_Curve;
class Bisector_Curve;
class Bisector_BisecAna;
class Bisector_BisecPC;
class Bisector_BisecCC;
class Bisector_Bisec;
class Bisector_Inter;
class Bisector_PointOnBis;
class Bisector_PolyBis;
class Bisector_FunctionH;
class Bisector_FunctionInter;


//! This package provides the bisecting line between two
//! geometric elements.
class Bisector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Boolean IsConvex (const Handle(Geom2d_Curve)& Cu, const Standard_Real Sign);




protected:





private:




friend class Bisector_Curve;
friend class Bisector_BisecAna;
friend class Bisector_BisecPC;
friend class Bisector_BisecCC;
friend class Bisector_Bisec;
friend class Bisector_Inter;
friend class Bisector_PointOnBis;
friend class Bisector_PolyBis;
friend class Bisector_FunctionH;
friend class Bisector_FunctionInter;

};







#endif // _Bisector_HeaderFile
