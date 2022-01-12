// Created on: 1995-03-14
// Created by: Jacques GOUSSARD
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

#ifndef _GeomAPI_IntSS_HeaderFile
#define _GeomAPI_IntSS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomInt_IntSS.hxx>
#include <Standard_Integer.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Geom_Surface;
class Geom_Curve;


//! This class implements methods for
//! computing the intersection curves   between two surfaces.
//! The result is curves from Geom.  The "domain" used for
//! a surface   is the natural  parametric domain
//! unless the surface is a  RectangularTrimmedSurface
//! from Geom.
class GeomAPI_IntSS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty object. Use the
  //! function Perform for further initialization algorithm by two surfaces.
    GeomAPI_IntSS();
  
  //! Computes the intersection curves
  //! between the two surfaces S1 and S2. Parameter Tol defines the precision
  //! of curves computation. For most cases the value 1.0e-7 is recommended to use.
  //! Warning
  //! Use the function IsDone to verify that the intersections are successfully computed.I
    GeomAPI_IntSS(const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const Standard_Real Tol);
  
  //! Initializes an algorithm with the
  //! given arguments and computes the intersection curves between the two surfaces S1 and S2.
  //! Parameter Tol defines the precision of curves computation. For most
  //! cases the value 1.0e-7 is recommended to use.
  //! Warning
  //! Use function IsDone to verify that the intersections are successfully computed.
    void Perform (const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const Standard_Real Tol);
  
  //! Returns True if the intersection was successful.
    Standard_Boolean IsDone() const;
  
  //! Returns the number of computed intersection curves.
  //! Exceptions
  //! StdFail_NotDone if the computation fails.
    Standard_Integer NbLines() const;
  
  //! Returns the computed intersection curve of index Index.
  //! Exceptions
  //! StdFail_NotDone if the computation fails.
  //! Standard_OutOfRange if Index is out of range [1, NbLines] where NbLines
  //! is the number of computed intersection curves.
    const Handle(Geom_Curve)& Line (const Standard_Integer Index) const;




protected:





private:



  GeomInt_IntSS myIntersec;


};


#include <GeomAPI_IntSS.lxx>





#endif // _GeomAPI_IntSS_HeaderFile
