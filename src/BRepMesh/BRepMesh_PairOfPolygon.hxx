// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_PairOfPolygon_HeaderFile
#define _BRepMesh_PairOfPolygon_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

class Poly_PolygonOnTriangulation;

class BRepMesh_PairOfPolygon
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor. Creates empty pair with null fileds.
  BRepMesh_PairOfPolygon()
  {
  }

  //! Clears pair handles.
  inline void Clear()
  {
    myFirst.Nullify();
    myLast.Nullify();
  }
  
  //! Sets the first element of the pair.
  //! If last element is empty, also assignes the given polygon to it.
  //! @param thePolygon plygon to be set.
  inline void Prepend(const Handle(Poly_PolygonOnTriangulation)& thePolygon)
  {
    myFirst = thePolygon;

    if (myLast.IsNull())
      myLast = thePolygon;
  }

  //! Sets the last element of the pair.
  //! If first element is empty, also assignes the given polygon to it.
  //! @param thePolygon plygon to be set.
  inline void Append(const Handle(Poly_PolygonOnTriangulation)& thePolygon)
  {
    if (myFirst.IsNull())
      myFirst = thePolygon;

    myLast = thePolygon;
  }

  //! Returns first polygon on triangulation.
  inline const Handle(Poly_PolygonOnTriangulation)& First() const
  {
    return myFirst;
  }

  //! Returns last polygon on triangulation.
  inline const Handle(Poly_PolygonOnTriangulation)& Last() const
  {
    return myLast;
  }

private:

  Handle(Poly_PolygonOnTriangulation) myFirst;
  Handle(Poly_PolygonOnTriangulation) myLast;
};

#endif
