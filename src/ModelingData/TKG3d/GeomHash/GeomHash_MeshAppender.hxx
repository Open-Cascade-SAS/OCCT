// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _GeomHash_MeshAppender_HeaderFile
#define _GeomHash_MeshAppender_HeaderFile

#include <GeomHash_Accumulator.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_TriangulationParameters.hxx>

template <class TheSinkType = GeomHash_Accumulator<uint64_t>>
class GeomHash_MeshAppender
{
public:
  using result_type = typename TheSinkType::result_type;

  result_type operator()(const occ::handle<Poly_Polygon3D>& thePolygon) const
  {
    return hash(0x50334450u, thePolygon, &GeomHash_MeshAppender::AppendPolygon);
  }

  result_type operator()(const occ::handle<Poly_Polygon2D>& thePolygon) const
  {
    return hash(0x50324450u, thePolygon, &GeomHash_MeshAppender::AppendPolygon);
  }

  result_type operator()(const occ::handle<Poly_PolygonOnTriangulation>& thePolygon) const
  {
    return hash(0x504f5452u, thePolygon, &GeomHash_MeshAppender::AppendPolygon);
  }

  result_type operator()(const occ::handle<Poly_Triangulation>& theTriangulation) const
  {
    return hash(0x50545249u, theTriangulation, &GeomHash_MeshAppender::AppendTriangulation);
  }

  static void AppendPolygon(TheSinkType& theSink, const occ::handle<Poly_Polygon3D>& thePolygon)
  {
    theSink.AppendDouble(thePolygon->Deflection());
    theSink.AppendUInt64(static_cast<uint64_t>(thePolygon->NbNodes()));
    for (const gp_Pnt& aNode : thePolygon->Nodes())
    {
      theSink.AppendPoint(aNode);
    }
    theSink.AppendBool(thePolygon->HasParameters());
    if (thePolygon->HasParameters())
    {
      const NCollection_Array1<double>& aParameters = thePolygon->Parameters();
      theSink.AppendUInt64(static_cast<uint64_t>(aParameters.Size()));
      for (const double aParameter : aParameters)
      {
        theSink.AppendDouble(aParameter);
      }
    }
  }

  static void AppendPolygon(TheSinkType& theSink, const occ::handle<Poly_Polygon2D>& thePolygon)
  {
    theSink.AppendDouble(thePolygon->Deflection());
    theSink.AppendUInt64(static_cast<uint64_t>(thePolygon->NbNodes()));
    for (const gp_Pnt2d& aNode : thePolygon->Nodes())
    {
      theSink.AppendPoint2d(aNode);
    }
  }

  static void AppendPolygon(TheSinkType&                                    theSink,
                            const occ::handle<Poly_PolygonOnTriangulation>& thePolygon)
  {
    theSink.AppendDouble(thePolygon->Deflection());
    theSink.AppendUInt64(static_cast<uint64_t>(thePolygon->NbNodes()));
    for (const int aNode : thePolygon->Nodes())
    {
      theSink.AppendInt(aNode);
    }
    theSink.AppendBool(thePolygon->HasParameters());
    if (thePolygon->HasParameters())
    {
      theSink.AppendUInt64(static_cast<uint64_t>(thePolygon->Parameters()->Size()));
      for (const double aParameter : thePolygon->Parameters()->Array1())
      {
        theSink.AppendDouble(aParameter);
      }
    }
  }

  static void AppendTriangulation(TheSinkType&                           theSink,
                                  const occ::handle<Poly_Triangulation>& theTriangulation)
  {
    const int aNbNodes     = theTriangulation->NbNodes();
    const int aNbTriangles = theTriangulation->NbTriangles();
    theSink.AppendDouble(theTriangulation->Deflection());
    theSink.AppendUInt32(static_cast<uint32_t>(theTriangulation->MeshPurpose()));
    theSink.AppendUInt64(static_cast<uint64_t>(aNbNodes));
    for (int aNodeIndex = 1; aNodeIndex <= aNbNodes; ++aNodeIndex)
    {
      theSink.AppendPoint(theTriangulation->Node(aNodeIndex));
    }

    theSink.AppendBool(theTriangulation->HasUVNodes());
    if (theTriangulation->HasUVNodes())
    {
      for (int aNodeIndex = 1; aNodeIndex <= aNbNodes; ++aNodeIndex)
      {
        theSink.AppendPoint2d(theTriangulation->UVNode(aNodeIndex));
      }
    }

    theSink.AppendBool(theTriangulation->HasNormals());
    if (theTriangulation->HasNormals())
    {
      for (int aNodeIndex = 1; aNodeIndex <= aNbNodes; ++aNodeIndex)
      {
        NCollection_Vec3<float> aNormal;
        theTriangulation->Normal(aNodeIndex, aNormal);
        theSink.AppendFloat(aNormal.x());
        theSink.AppendFloat(aNormal.y());
        theSink.AppendFloat(aNormal.z());
      }
    }

    theSink.AppendUInt64(static_cast<uint64_t>(aNbTriangles));
    for (int aTriangleIndex = 1; aTriangleIndex <= aNbTriangles; ++aTriangleIndex)
    {
      int aNode1 = 0;
      int aNode2 = 0;
      int aNode3 = 0;
      theTriangulation->Triangle(aTriangleIndex).Get(aNode1, aNode2, aNode3);
      theSink.AppendInt(aNode1);
      theSink.AppendInt(aNode2);
      theSink.AppendInt(aNode3);
    }

    const occ::handle<Poly_TriangulationParameters>& aParameters = theTriangulation->Parameters();
    theSink.AppendBool(!aParameters.IsNull());
    if (!aParameters.IsNull())
    {
      theSink.AppendBool(aParameters->HasDeflection());
      if (aParameters->HasDeflection())
      {
        theSink.AppendDouble(aParameters->Deflection());
      }
      theSink.AppendBool(aParameters->HasAngle());
      if (aParameters->HasAngle())
      {
        theSink.AppendDouble(aParameters->Angle());
      }
      theSink.AppendBool(aParameters->HasMinSize());
      if (aParameters->HasMinSize())
      {
        theSink.AppendDouble(aParameters->MinSize());
      }
    }
  }

private:
  template <class TheMeshType>
  static result_type hash(const uint32_t                  theDomain,
                          const occ::handle<TheMeshType>& theMesh,
                          void (*theAppend)(TheSinkType&, const occ::handle<TheMeshType>&))
  {
    TheSinkType aSink;
    aSink.AppendUInt32(theDomain);
    aSink.AppendBool(!theMesh.IsNull());
    if (!theMesh.IsNull())
    {
      theAppend(aSink, theMesh);
    }
    return aSink.Finish();
  }
};

#endif // _GeomHash_MeshAppender_HeaderFile
