// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _ShapePersistent_Poly_HeaderFile
#define _ShapePersistent_Poly_HeaderFile

#include <StdObjMgt_SharedObject.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdLPersistent_HArray1.hxx>
#include <ShapePersistent_HArray1.hxx>

class Poly_Polygon2D;
class Poly_Polygon3D;
class Poly_PolygonOnTriangulation;
class Poly_Triangulation;


class ShapePersistent_Poly : private StdObjMgt_SharedObject
{
  class pPolygon2D : public Standard_Transient
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myDeflection >> myNodes; }

    Handle(Poly_Polygon2D) Import() const;

  private:
    Standard_Real                          myDeflection;
    Handle(ShapePersistent_HArray1::Pnt2d) myNodes;
  };

  class pPolygon3D : public Standard_Transient
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myDeflection >> myNodes >> myParameters; }

    Handle(Poly_Polygon3D) Import() const;

  private:
    Standard_Real                        myDeflection;
    Handle(ShapePersistent_HArray1::Pnt) myNodes;
    Handle(StdLPersistent_HArray1::Real) myParameters;
  };

  class pPolygonOnTriangulation : public Standard_Transient
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myDeflection >> myNodes >> myParameters; }

    Handle(Poly_PolygonOnTriangulation) Import() const;

  private:
    Standard_Real                           myDeflection;
    Handle(StdLPersistent_HArray1::Integer) myNodes;
    Handle(StdLPersistent_HArray1::Real)    myParameters;
  };

  class pTriangulation : public Standard_Transient
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myDeflection >> myNodes >> myUVNodes >> myTriangles; }

    Handle(Poly_Triangulation) Import() const;

  private:
    Standard_Real                             myDeflection;
    Handle(ShapePersistent_HArray1::Pnt)      myNodes;
    Handle(ShapePersistent_HArray1::Pnt2d)    myUVNodes;
    Handle(ShapePersistent_HArray1::Triangle) myTriangles;
  };

  template <class Persistent, class Transient>
  struct instance
    : Delayed <DelayedBase<StdObjMgt_Persistent, Transient, Persistent> > {};

public:
  typedef instance <pPolygon2D, Poly_Polygon2D>         Polygon2D;
  typedef instance <pPolygon3D, Poly_Polygon3D>         Polygon3D;
  typedef instance <pPolygonOnTriangulation,
                    Poly_PolygonOnTriangulation>        PolygonOnTriangulation;
  typedef instance <pTriangulation, Poly_Triangulation> Triangulation;
};

#endif
