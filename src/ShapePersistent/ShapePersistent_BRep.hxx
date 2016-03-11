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


#ifndef _ShapePersistent_BRep_HeaderFile
#define _ShapePersistent_BRep_HeaderFile

#include <ShapePersistent_TopoDS.hxx>
#include <ShapePersistent_Geom.hxx>
#include <ShapePersistent_Geom2d.hxx>
#include <ShapePersistent_Poly.hxx>
#include <StdObject_Location.hxx>
#include <StdObject_gp_Vectors.hxx>

#include <BRep_ListOfPointRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

class BRep_PointRepresentation;
class BRep_CurveRepresentation;


class ShapePersistent_BRep : public ShapePersistent_TopoDS
{
public:
  class PointRepresentation : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

    //! Import transient object from the persistent data.
    Standard_EXPORT void Import (BRep_ListOfPointRepresentation& thePoints)
      const;

  protected:
    virtual Handle(BRep_PointRepresentation) import() const;

  protected:
    StdObject_Location myLocation;
    Standard_Real      myParameter;

  private:
    Handle(PointRepresentation) myNext;
  };

  class PointOnCurve : public PointRepresentation
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_PointRepresentation) import() const;

  private:
    Handle(ShapePersistent_Geom::Curve) myCurve;
  };

  class PointsOnSurface : public PointRepresentation
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);

  protected:
    Handle(ShapePersistent_Geom::Surface) mySurface;
  };

  class PointOnCurveOnSurface : public PointsOnSurface
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_PointRepresentation) import() const;

  private:
    Handle(ShapePersistent_Geom2d::Curve) myPCurve;
  };

  class PointOnSurface : public PointsOnSurface
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_PointRepresentation) import() const;

  private:
    Standard_Real myParameter2;
  };

  class CurveRepresentation : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

    //! Import transient object from the persistent data.
    Standard_EXPORT void Import (BRep_ListOfCurveRepresentation& theCurves)
      const;

  protected:
    virtual Handle(BRep_CurveRepresentation) import() const;

  protected:
    StdObject_Location myLocation;

  private:
    Handle(CurveRepresentation) myNext;
  };

  class GCurve : public CurveRepresentation
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);

  protected:
    Standard_Real myFirst;
    Standard_Real myLast;
  };

  class Curve3D : public GCurve
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Geom::Curve) myCurve3D;
  };

  class CurveOnSurface : public GCurve
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_CurveRepresentation) import() const;

  protected:
    Handle(ShapePersistent_Geom2d::Curve) myPCurve;
    Handle(ShapePersistent_Geom::Surface) mySurface;
    gp_Pnt2d                              myUV1;
    gp_Pnt2d                              myUV2;
  };

  class CurveOnClosedSurface : public CurveOnSurface
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Geom2d::Curve) myPCurve2;
    Standard_Integer                      myContinuity;
    gp_Pnt2d                              myUV21;
    gp_Pnt2d                              myUV22;
  };

  class Polygon3D : public CurveRepresentation
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Poly::Polygon3D) myPolygon3D;
  };

  class PolygonOnTriangulation : public CurveRepresentation
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_CurveRepresentation) import() const;

  protected:
    Handle(ShapePersistent_Poly::PolygonOnTriangulation) myPolygon;
    Handle(ShapePersistent_Poly::Triangulation)          myTriangulation;
  };

  class PolygonOnClosedTriangulation : public PolygonOnTriangulation
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Poly::PolygonOnTriangulation) myPolygon2;
  };

  class PolygonOnSurface : public CurveRepresentation
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_CurveRepresentation) import() const;

  protected:
    Handle(ShapePersistent_Poly::Polygon2D) myPolygon2D;
    Handle(ShapePersistent_Geom::Surface)   mySurface;
  };

  class PolygonOnClosedSurface : public PolygonOnSurface
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Poly::Polygon2D) myPolygon2;
  };

  class CurveOn2Surfaces : public CurveRepresentation
  {
  public:
    virtual void Read (StdObjMgt_ReadData& theReadData);
    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Geom::Surface) mySurface;
    Handle(ShapePersistent_Geom::Surface) mySurface2;
    StdObject_Location                    myLocation2;
    Standard_Integer                      myContinuity;
  };

private:
  class pTVertex : public pTBase
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      pTBase::Read (theReadData);
      theReadData >> myTolerance >> myPnt >> myPoints;
    }

  private:
    virtual Handle(TopoDS_TShape) createTShape() const;

  private:
    Standard_Real               myTolerance;
    gp_Pnt                      myPnt;
    Handle(PointRepresentation) myPoints;
  };

  class pTEdge : public pTBase
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      pTBase::Read (theReadData);
      theReadData >> myTolerance >> myFlags >> myCurves;
    }

  private:
    virtual Handle(TopoDS_TShape) createTShape() const;

  private:
    Standard_Real               myTolerance;
    Standard_Integer            myFlags;
    Handle(CurveRepresentation) myCurves;
  };

  class pTFace : public pTBase
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      pTBase::Read (theReadData);
      theReadData >> mySurface >> myTriangulation >> myLocation;
      theReadData >> myTolerance >> myNaturalRestriction;
    }

  private:
    virtual Handle(TopoDS_TShape) createTShape() const;

  private:
    Handle(ShapePersistent_Geom::Surface)       mySurface;
    Handle(ShapePersistent_Poly::Triangulation) myTriangulation;
    StdObject_Location                          myLocation;
    Standard_Real                               myTolerance;
    Standard_Boolean                            myNaturalRestriction;
  };

public:
  typedef tObject  <pTVertex> TVertex;
  typedef tObject  <pTEdge>   TEdge;
  typedef tObject  <pTFace>   TFace;

  typedef tObject1 <pTVertex> TVertex1;
  typedef tObject1 <pTEdge>   TEdge1;
  typedef tObject1 <pTFace>   TFace1;
};

#endif
