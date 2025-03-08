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
#include <ShapePersistent_Geom2d.hxx>
#include <ShapePersistent_Poly.hxx>
#include <StdObjMgt_TransientPersistentMap.hxx>
#include <StdObject_Location.hxx>
#include <StdObject_gp_Vectors.hxx>

#include <BRep_ListOfPointRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

class BRep_PointRepresentation;
class BRep_CurveRepresentation;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Face;

class ShapePersistent_BRep : public ShapePersistent_TopoDS
{
public:
  class PointRepresentation : public StdObjMgt_Persistent
  {
    friend class ShapePersistent_BRep;

  public:
    //! Empty constructor.
    PointRepresentation()
        : myParameter(0.0)
    {
    }

    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read(StdObjMgt_ReadData& theReadData);
    //! Write persistent data to a file.
    Standard_EXPORT virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    //! Gets persistent child objects
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    //! Returns persistent type name
    virtual Standard_CString PName() const { return "PBRep_PointRepresentation"; }

    //! Import transient object from the persistent data.
    Standard_EXPORT void Import(BRep_ListOfPointRepresentation& thePoints) const;

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
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_PointOnCurve"; }

    virtual Handle(BRep_PointRepresentation) import() const;

  private:
    Handle(ShapePersistent_Geom::Curve) myCurve;
  };

  class PointsOnSurface : public PointRepresentation
  {
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_PointsOnSurface"; }

  protected:
    Handle(ShapePersistent_Geom::Surface) mySurface;
  };

  class PointOnCurveOnSurface : public PointsOnSurface
  {
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_PointOnCurveOnSurface"; }

    virtual Handle(BRep_PointRepresentation) import() const;

  private:
    Handle(ShapePersistent_Geom2d::Curve) myPCurve;
  };

  class PointOnSurface : public PointsOnSurface
  {
    friend class ShapePersistent_BRep;

  public:
    PointOnSurface()
        : myParameter2(0.0)
    {
    }

    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;

    virtual Standard_CString PName() const { return "PBRep_PointOnSurface"; }

    virtual Handle(BRep_PointRepresentation) import() const;

  private:
    Standard_Real myParameter2;
  };

  class CurveRepresentation : public StdObjMgt_Persistent
  {
    friend class ShapePersistent_BRep;

  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read(StdObjMgt_ReadData& theReadData);
    //! Write persistent data from a file.
    Standard_EXPORT virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    //! Gets persistent child objects
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    //! Returns persistent type name
    virtual Standard_CString PName() const { return "PBRep_CurveRepresentation"; }

    //! Import transient object from the persistent data.
    Standard_EXPORT void Import(BRep_ListOfCurveRepresentation& theCurves) const;

  protected:
    virtual Handle(BRep_CurveRepresentation) import() const;

  protected:
    StdObject_Location myLocation;

  private:
    Handle(CurveRepresentation) myNext;
  };

  class GCurve : public CurveRepresentation
  {
    friend class ShapePersistent_BRep;

  public:
    GCurve()
        : myFirst(0.0),
          myLast(0.0)
    {
    }

    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;

    virtual Standard_CString PName() const { return "PBRep_GCurve"; }

  protected:
    Standard_Real myFirst;
    Standard_Real myLast;
  };

  class Curve3D : public GCurve
  {
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_Curve3D"; }

    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Geom::Curve) myCurve3D;
  };

  class CurveOnSurface : public GCurve
  {
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_CurveOnSurface"; }

    virtual Handle(BRep_CurveRepresentation) import() const;

  protected:
    Handle(ShapePersistent_Geom2d::Curve) myPCurve;
    Handle(ShapePersistent_Geom::Surface) mySurface;
    gp_Pnt2d                              myUV1;
    gp_Pnt2d                              myUV2;
  };

  class CurveOnClosedSurface : public CurveOnSurface
  {
    friend class ShapePersistent_BRep;

  public:
    CurveOnClosedSurface()
        : myContinuity(0)
    {
    }

    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_CurveOnClosedSurface"; }

    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Geom2d::Curve) myPCurve2;
    Standard_Integer                      myContinuity;
    gp_Pnt2d                              myUV21;
    gp_Pnt2d                              myUV22;
  };

  class Polygon3D : public CurveRepresentation
  {
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_Polygon3D"; }

    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Poly::Polygon3D) myPolygon3D;
  };

  class PolygonOnTriangulation : public CurveRepresentation
  {
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_PolygonOnTriangulation"; }

    virtual Handle(BRep_CurveRepresentation) import() const;

  protected:
    Handle(ShapePersistent_Poly::PolygonOnTriangulation) myPolygon;
    Handle(ShapePersistent_Poly::Triangulation)          myTriangulation;
  };

  class PolygonOnClosedTriangulation : public PolygonOnTriangulation
  {
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_PolygonOnClosedTriangulation"; }

    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Poly::PolygonOnTriangulation) myPolygon2;
  };

  class PolygonOnSurface : public CurveRepresentation
  {
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_PolygonOnSurface"; }

    virtual Handle(BRep_CurveRepresentation) import() const;

  protected:
    Handle(ShapePersistent_Poly::Polygon2D) myPolygon2D;
    Handle(ShapePersistent_Geom::Surface)   mySurface;
  };

  class PolygonOnClosedSurface : public PolygonOnSurface
  {
    friend class ShapePersistent_BRep;

  public:
    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_PolygonOnClosedSurface"; }

    virtual Handle(BRep_CurveRepresentation) import() const;

  private:
    Handle(ShapePersistent_Poly::Polygon2D) myPolygon2;
  };

  class CurveOn2Surfaces : public CurveRepresentation
  {
    friend class ShapePersistent_BRep;

  public:
    CurveOn2Surfaces()
        : myContinuity(0)
    {
    }

    virtual void Read(StdObjMgt_ReadData& theReadData);
    virtual void Write(StdObjMgt_WriteData& theWriteData) const;
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

    virtual Standard_CString PName() const { return "PBRep_CurveOn2Surfaces"; }

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
    friend class ShapePersistent_BRep;

  public:
    pTVertex()
        : myTolerance(0.0)
    {
    }

    inline void Read(StdObjMgt_ReadData& theReadData)
    {
      pTBase::Read(theReadData);
      theReadData >> myTolerance >> myPnt >> myPoints;
    }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      pTBase::Write(theWriteData);
      theWriteData << myTolerance << myPnt << myPoints;
    }

    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      pTBase::PChildren(theChildren);
      theChildren.Append(myPoints);
    }

    inline Standard_CString PName() const { return "PBRep_TVertex"; }

  private:
    virtual Handle(TopoDS_TShape) createTShape() const;

  private:
    Standard_Real               myTolerance;
    gp_Pnt                      myPnt;
    Handle(PointRepresentation) myPoints;
  };

  class pTEdge : public pTBase
  {
    friend class ShapePersistent_BRep;

  public:
    pTEdge()
        : myTolerance(0.0),
          myFlags(0)
    {
    }

    inline void Read(StdObjMgt_ReadData& theReadData)
    {
      pTBase::Read(theReadData);
      theReadData >> myTolerance >> myFlags >> myCurves;
    }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      pTBase::Write(theWriteData);
      theWriteData << myTolerance << myFlags << myCurves;
    }

    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      pTBase::PChildren(theChildren);
      theChildren.Append(myCurves);
    }

    inline Standard_CString PName() const { return "PBRep_TEdge"; }

  private:
    virtual Handle(TopoDS_TShape) createTShape() const;

  private:
    Standard_Real               myTolerance;
    Standard_Integer            myFlags;
    Handle(CurveRepresentation) myCurves;
  };

  class pTFace : public pTBase
  {
    friend class ShapePersistent_BRep;

  public:
    pTFace()
        : myTolerance(0.0),
          myNaturalRestriction(Standard_False)
    {
    }

    inline void Read(StdObjMgt_ReadData& theReadData)
    {
      pTBase::Read(theReadData);
      theReadData >> mySurface >> myTriangulation >> myLocation;
      theReadData >> myTolerance >> myNaturalRestriction;
    }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      pTBase::Write(theWriteData);
      theWriteData << mySurface << myTriangulation << myLocation;
      theWriteData << myTolerance << myNaturalRestriction;
    }

    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      pTBase::PChildren(theChildren);
      theChildren.Append(mySurface);
      theChildren.Append(myTriangulation);
      myLocation.PChildren(theChildren);
    }

    inline Standard_CString PName() const { return "PBRep_TFace"; }

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
  typedef tObject<pTVertex> TVertex;
  typedef tObject<pTEdge>   TEdge;
  typedef tObject<pTFace>   TFace;

  typedef tObject1<pTVertex> TVertex1;
  typedef tObject1<pTEdge>   TEdge1;
  typedef tObject1<pTFace>   TFace1;

public:
  //! Create a persistent object for a vertex
  Standard_EXPORT static Handle(TVertex::pTObjectT) Translate(
    const TopoDS_Vertex&              theVertex,
    StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for an edge
  Standard_EXPORT static Handle(TEdge::pTObjectT) Translate(
    const TopoDS_Edge&                theEdge,
    StdObjMgt_TransientPersistentMap& theMap,
    ShapePersistent_TriangleMode      theTriangleMode);
  //! Create a persistent object for a face
  Standard_EXPORT static Handle(TFace::pTObjectT) Translate(
    const TopoDS_Face&                theFace,
    StdObjMgt_TransientPersistentMap& theMap,
    ShapePersistent_TriangleMode      theTriangleMode);
  //! Create a persistent object for a point on a 3D curve
  Standard_EXPORT static Handle(PointOnCurve) Translate(Standard_Real                     theParam,
                                                        const Handle(Geom_Curve)&         theCurve,
                                                        const TopLoc_Location&            theLoc,
                                                        StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a point on a 3D curve on a surface
  Standard_EXPORT static Handle(PointOnCurveOnSurface) Translate(
    Standard_Real                     theParam,
    const Handle(Geom2d_Curve)&       theCurve,
    const Handle(Geom_Surface)&       theSurf,
    const TopLoc_Location&            theLoc,
    StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a point on a surface
  Standard_EXPORT static Handle(PointOnSurface) Translate(Standard_Real               theParam,
                                                          Standard_Real               theParam2,
                                                          const Handle(Geom_Surface)& theSurf,
                                                          const TopLoc_Location&      theLoc,
                                                          StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a curve on a surface
  Standard_EXPORT static Handle(CurveOnSurface) Translate(const Handle(Geom2d_Curve)& theCurve,
                                                          const Standard_Real         theFirstParam,
                                                          const Standard_Real         theLastParam,
                                                          const Handle(Geom_Surface)& theSurf,
                                                          const TopLoc_Location&      theLoc,
                                                          StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a curve on a closed surface
  Standard_EXPORT static Handle(CurveOnClosedSurface) Translate(
    const Handle(Geom2d_Curve)&       theCurve,
    const Handle(Geom2d_Curve)&       theCurve2,
    const Standard_Real               theFirstParam,
    const Standard_Real               theLastParam,
    const Handle(Geom_Surface)&       theSurf,
    const TopLoc_Location&            theLoc,
    const GeomAbs_Shape               theContinuity,
    StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a curve on two surfaces
  Standard_EXPORT static Handle(CurveOn2Surfaces) Translate(
    const Handle(Geom_Surface)&       theSurf,
    const Handle(Geom_Surface)&       theSurf2,
    const TopLoc_Location&            theLoc,
    const TopLoc_Location&            theLoc2,
    const GeomAbs_Shape               theContinuity,
    StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a 3D curve
  Standard_EXPORT static Handle(Curve3D) Translate(const Handle(Geom_Curve)&         theCurve,
                                                   const Standard_Real               theFirstParam,
                                                   const Standard_Real               theLastParam,
                                                   const TopLoc_Location&            theLoc,
                                                   StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a 3D polygon
  Standard_EXPORT static Handle(Polygon3D) Translate(const Handle(Poly_Polygon3D)&     thePoly,
                                                     const TopLoc_Location&            theLoc,
                                                     StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a polygon on a closed surface
  Standard_EXPORT static Handle(PolygonOnClosedSurface) Translate(
    const Handle(Poly_Polygon2D)&     thePoly,
    const Handle(Poly_Polygon2D)&     thePoly2,
    const Handle(Geom_Surface)&       theSurf,
    const TopLoc_Location&            theLoc,
    StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a polygon on a surface
  Standard_EXPORT static Handle(PolygonOnSurface) Translate(
    const Handle(Poly_Polygon2D)&     thePoly,
    const Handle(Geom_Surface)&       theSurf,
    const TopLoc_Location&            theLoc,
    StdObjMgt_TransientPersistentMap& theMap);
  //! Create a persistent object for a polygon on a surface
  Standard_EXPORT static Handle(PolygonOnClosedTriangulation) Translate(
    const Handle(Poly_PolygonOnTriangulation)& thePolyOnTriang,
    const Handle(Poly_PolygonOnTriangulation)& thePolyOnTriang2,
    const Handle(Poly_Triangulation)&          thePolyTriang,
    const TopLoc_Location&                     theLoc,
    StdObjMgt_TransientPersistentMap&          theMap);
  //! Create a persistent object for a polygon on a surface
  Standard_EXPORT static Handle(PolygonOnTriangulation) Translate(
    const Handle(Poly_PolygonOnTriangulation)& thePolyOnTriang,
    const Handle(Poly_Triangulation)&          thePolyTriang,
    const TopLoc_Location&                     theLoc,
    StdObjMgt_TransientPersistentMap&          theMap);
};

#endif
