// Created on: 1995-03-06
// Created by: Laurent PAINNOT
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

#ifndef _Poly_Triangle_HeaderFile
#define _Poly_Triangle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class Standard_OutOfRange;


//! Describes a component triangle of a triangulation
//! (Poly_Triangulation object).
//! A Triangle is defined by a triplet of nodes. Each node is an
//! index in the table of nodes specific to an existing
//! triangulation of a shape, and represents a point on the surface.
class Poly_Triangle 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a triangle and sets all indices to zero.
  Standard_EXPORT Poly_Triangle();
  
  //! Constructs a triangle and sets its three indices
  //! to N1, N2 and N3 respectively, where these node values
  //! are indices in the table of nodes specific to an existing
  //! triangulation of a shape.
  Standard_EXPORT Poly_Triangle(const Standard_Integer N1, const Standard_Integer N2, const Standard_Integer N3);
  
  //! Sets the value of  the three nodes of this triangle to N1, N2 and N3   respectively.
  Standard_EXPORT void Set (const Standard_Integer N1, const Standard_Integer N2, const Standard_Integer N3);
  
  //! Sets the value of  the Indexth node of this triangle to Node.
  //! Raises OutOfRange if Index is not in 1,2,3
    void Set (const Standard_Integer Index, const Standard_Integer Node);
  
  //! Returns the node indices of this triangle in N1, N2 and N3.
  Standard_EXPORT void Get (Standard_Integer& N1, Standard_Integer& N2, Standard_Integer& N3) const;
  
  //! Get the node of given Index.
  //! Raises OutOfRange from Standard if Index is not in 1,2,3
    Standard_Integer Value (const Standard_Integer Index) const;
  Standard_Integer operator() (const Standard_Integer Index) const
{
  return Value(Index);
}
  
  //! Get the node of given Index.
  //! Raises OutOfRange if Index is not in 1,2,3
    Standard_Integer& ChangeValue (const Standard_Integer Index);
  Standard_Integer& operator() (const Standard_Integer Index)
{
  return ChangeValue(Index);
}




protected:





private:



  Standard_Integer myNodes[3];


};


#include <Poly_Triangle.lxx>





#endif // _Poly_Triangle_HeaderFile
