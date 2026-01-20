// Created on: 1993-07-19
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRep_Builder.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_PointRepresentation.hxx>
#include <NCollection_List.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <GeomTools.hxx>
#include <Message_ProgressScope.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

// Modified:    02 Nov 2000: BUC60769. JMB, PTV.  In order to be able to read BRep
//              files that came from a platform different from where CasCade
//              is run, we need the following modifications.
//
//              On NT platforms (in BRepTools_ShapeSet):
//              ----------------
//                In Visual C++ 5 (or higher) the std::fstream::tellg method is not
//                conform to Standard C++ because it modifies the file pointer
//                position and returns a wrong position. After that the next
//                readings are shifted and the reading process stop with errors.
//
//                Workaround is following: Now we don`t use tellg for get position in stream.
//                Now able to read file (when reading TopAbs_FACE) without tellg.
//                We simple check the next string if there are value that equal 2
//               (It means a parameter for triangulation).

//=================================================================================================

BRepTools_ShapeSet::BRepTools_ShapeSet(const bool theWithTriangles,
                                       const bool theWithNormals)
    : myWithTriangles(theWithTriangles),
      myWithNormals(theWithNormals)
{
}

//=================================================================================================

BRepTools_ShapeSet::BRepTools_ShapeSet(const BRep_Builder&    theBuilder,
                                       const bool theWithTriangles,
                                       const bool theWithNormals)
    : myBuilder(theBuilder),
      myWithTriangles(theWithTriangles),
      myWithNormals(theWithNormals)
{
}

//=================================================================================================

BRepTools_ShapeSet::~BRepTools_ShapeSet()
{
  //
}

//=================================================================================================

void BRepTools_ShapeSet::Clear()
{
  mySurfaces.Clear();
  myCurves.Clear();
  myCurves2d.Clear();
  myPolygons3D.Clear();
  myPolygons2D.Clear();
  myNodes.Clear();
  myTriangulations.Clear();
  TopTools_ShapeSet::Clear();
}

//=================================================================================================

void BRepTools_ShapeSet::AddGeometry(const TopoDS_Shape& S)
{
  // Add the geometry

  if (S.ShapeType() == TopAbs_VERTEX)
  {

    occ::handle<BRep_TVertex>                         TV = occ::down_cast<BRep_TVertex>(S.TShape());
    NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itrp(TV->Points());

    while (itrp.More())
    {
      const occ::handle<BRep_PointRepresentation>& PR = itrp.Value();

      if (PR->IsPointOnCurve())
      {
        myCurves.Add(PR->Curve());
      }

      else if (PR->IsPointOnCurveOnSurface())
      {
        myCurves2d.Add(PR->PCurve());
        mySurfaces.Add(PR->Surface());
      }

      else if (PR->IsPointOnSurface())
      {
        mySurfaces.Add(PR->Surface());
      }

      ChangeLocations().Add(PR->Location());
      itrp.Next();
    }
  }
  else if (S.ShapeType() == TopAbs_EDGE)
  {

    // Add the curve geometry
    occ::handle<BRep_TEdge>                           TE = occ::down_cast<BRep_TEdge>(S.TShape());
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itrc(TE->Curves());

    while (itrc.More())
    {
      const occ::handle<BRep_CurveRepresentation>& CR = itrc.Value();
      if (CR->IsCurve3D())
      {
        if (!CR->Curve3D().IsNull())
        {
          myCurves.Add(CR->Curve3D());
          ChangeLocations().Add(CR->Location());
        }
      }
      else if (CR->IsCurveOnSurface())
      {
        mySurfaces.Add(CR->Surface());
        myCurves2d.Add(CR->PCurve());
        ChangeLocations().Add(CR->Location());
        if (CR->IsCurveOnClosedSurface())
          myCurves2d.Add(CR->PCurve2());
      }
      else if (CR->IsRegularity())
      {
        mySurfaces.Add(CR->Surface());
        ChangeLocations().Add(CR->Location());
        mySurfaces.Add(CR->Surface2());
        ChangeLocations().Add(CR->Location2());
      }
      else if (myWithTriangles)
      { // for XML Persistence
        if (CR->IsPolygon3D())
        {
          if (!CR->Polygon3D().IsNull())
          {
            myPolygons3D.Add(CR->Polygon3D());
            ChangeLocations().Add(CR->Location());
          }
        }
        else if (CR->IsPolygonOnTriangulation())
        {
          // NCollection_IndexedDataMap::Add() function use is correct because
          // Bin(Brep)Tools_ShapeSet::AddGeometry() is called from Bin(Brep)Tools_ShapeSet::Add()
          // that processes shapes recursively from complex to elementary ones.
          // As a result, the TopAbs_FACE's will be processed earlier than the TopAbs_EDGE's.
          // clang-format off
          myTriangulations.Add(CR->Triangulation(), false); // edge triangulation does not need normals
          // clang-format on
          myNodes.Add(CR->PolygonOnTriangulation());
          ChangeLocations().Add(CR->Location());
          if (CR->IsPolygonOnClosedTriangulation())
            myNodes.Add(CR->PolygonOnTriangulation2());
        }
        else if (CR->IsPolygonOnSurface())
        {
          mySurfaces.Add(CR->Surface());
          myPolygons2D.Add(CR->Polygon());
          ChangeLocations().Add(CR->Location());
          if (CR->IsPolygonOnClosedSurface())
            myPolygons2D.Add(CR->Polygon2());
        }
      }
      itrc.Next();
    }
  }

  else if (S.ShapeType() == TopAbs_FACE)
  {

    // Add the surface geometry
    bool   needNormals(myWithNormals);
    occ::handle<BRep_TFace> TF = occ::down_cast<BRep_TFace>(S.TShape());
    if (!TF->Surface().IsNull())
    {
      mySurfaces.Add(TF->Surface());
    }
    else
    {
      needNormals = true;
    }
    if (myWithTriangles || TF->Surface().IsNull())
    { // for XML Persistence
      occ::handle<Poly_Triangulation> Tr = TF->Triangulation();
      if (!Tr.IsNull())
        myTriangulations.Add(Tr, needNormals);
    }

    ChangeLocations().Add(TF->Location());
  }
}

//=================================================================================================

void BRepTools_ShapeSet::DumpGeometry(Standard_OStream& OS) const
{
  myCurves2d.Dump(OS);
  myCurves.Dump(OS);
  DumpPolygon3D(OS);
  DumpPolygonOnTriangulation(OS);
  mySurfaces.Dump(OS);
  DumpTriangulation(OS);
}

//=================================================================================================

void BRepTools_ShapeSet::WriteGeometry(Standard_OStream&            OS,
                                       const Message_ProgressRange& theProgress)
{
  // Make nested progress scope for processing geometry
  Message_ProgressScope aPS(theProgress, "Geometry", 100);

  myCurves2d.Write(OS, aPS.Next(20));
  if (aPS.UserBreak())
    return;

  myCurves.Write(OS, aPS.Next(20));
  if (aPS.UserBreak())
    return;

  WritePolygon3D(OS, true, aPS.Next(10));
  if (aPS.UserBreak())
    return;

  WritePolygonOnTriangulation(OS, true, aPS.Next(10));
  if (aPS.UserBreak())
    return;

  mySurfaces.Write(OS, aPS.Next(20));
  if (aPS.UserBreak())
    return;

  WriteTriangulation(OS, true, aPS.Next(20));
}

//=================================================================================================

void BRepTools_ShapeSet::ReadGeometry(Standard_IStream&            IS,
                                      const Message_ProgressRange& theProgress)
{
  // Make nested progress scope for processing geometry
  Message_ProgressScope aPS(theProgress, "Geometry", 100);

  myCurves2d.Read(IS, aPS.Next(20));
  if (aPS.UserBreak())
    return;

  myCurves.Read(IS, aPS.Next(20));
  if (aPS.UserBreak())
    return;

  ReadPolygon3D(IS, aPS.Next(15));
  if (aPS.UserBreak())
    return;

  ReadPolygonOnTriangulation(IS, aPS.Next(15));
  if (aPS.UserBreak())
    return;

  mySurfaces.Read(IS, aPS.Next(15));
  if (aPS.UserBreak())
    return;

  ReadTriangulation(IS, aPS.Next(15));
}

//=================================================================================================

static void PrintRegularity(const GeomAbs_Shape C, Standard_OStream& OS)
{
  switch (C)
  {

    case GeomAbs_C0:
      OS << "C0";
      break;

    case GeomAbs_G1:
      OS << "G1";
      break;

    case GeomAbs_C1:
      OS << "C1";
      break;

    case GeomAbs_G2:
      OS << "G2";
      break;

    case GeomAbs_C2:
      OS << "C2";
      break;

    case GeomAbs_C3:
      OS << "C3";
      break;

    case GeomAbs_CN:
      OS << "CN";
      break;
  }
}

//=================================================================================================

void BRepTools_ShapeSet::DumpGeometry(const TopoDS_Shape& S, Standard_OStream& OS) const
{
  // Dump the geometry

  if (S.ShapeType() == TopAbs_VERTEX)
  {

    // Dump the point geometry
    TopoDS_Vertex V = TopoDS::Vertex(S);
    OS << "    Tolerance : " << BRep_Tool::Tolerance(V) << "\n";
    gp_Pnt p = BRep_Tool::Pnt(V);
    OS << "    - Point 3D : " << p.X() << ", " << p.Y() << ", " << p.Z() << "\n";

    occ::handle<BRep_TVertex>                         TV = occ::down_cast<BRep_TVertex>(S.TShape());
    NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itrp(TV->Points());

    while (itrp.More())
    {
      const occ::handle<BRep_PointRepresentation>& PR = itrp.Value();

      OS << "    - Parameter : " << PR->Parameter();
      if (PR->IsPointOnCurve())
      {
        OS << " on curve " << myCurves.Index(PR->Curve());
      }

      else if (PR->IsPointOnCurveOnSurface())
      {
        OS << " on pcurve " << myCurves2d.Index(PR->PCurve());
        OS << " on surface " << mySurfaces.Index(PR->Surface());
      }

      else if (PR->IsPointOnSurface())
      {
        OS << ", " << PR->Parameter2() << " on surface ";
        OS << mySurfaces.Index(PR->Surface());
      }

      if (!PR->Location().IsIdentity())
        OS << " location " << Locations().Index(PR->Location());
      OS << "\n";

      itrp.Next();
    }
  }

  else if (S.ShapeType() == TopAbs_EDGE)
  {

    occ::handle<BRep_TEdge> TE = occ::down_cast<BRep_TEdge>(S.TShape());
    gp_Pnt2d           Pf, Pl;

    // Dump the curve geometry
    OS << "    Tolerance : " << TE->Tolerance() << "\n";
    if (TE->SameParameter())
      OS << "     same parametrisation of curves\n";
    if (TE->SameRange())
      OS << "     same range on curves\n";
    if (TE->Degenerated())
      OS << "     degenerated\n";

    double                                first, last;
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itrc = TE->Curves();
    while (itrc.More())
    {
      const occ::handle<BRep_CurveRepresentation>& CR = itrc.Value();
      if (CR->IsCurve3D())
      {
        occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itrc.Value());
        GC->Range(first, last);
        if (!CR->Curve3D().IsNull())
        {
          OS << "    - Curve 3D : " << myCurves.Index(CR->Curve3D());
          if (!CR->Location().IsIdentity())
            OS << " location " << Locations().Index(CR->Location());
          OS << ", range : " << first << " " << last << "\n";
        }
      }
      else if (CR->IsCurveOnSurface())
      {
        occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itrc.Value());
        GC->Range(first, last);
        OS << "    - PCurve : " << myCurves2d.Index(CR->PCurve());
        if (CR->IsCurveOnClosedSurface())
        {
          OS << ", " << myCurves2d.Index(CR->PCurve2());
          OS << " (";
          PrintRegularity(CR->Continuity(), OS);
          OS << ")";
        }
        OS << " on surface " << mySurfaces.Index(CR->Surface());
        if (!CR->Location().IsIdentity())
          OS << " location " << Locations().Index(CR->Location());
        OS << ", range : " << first << " " << last << "\n";

        occ::handle<BRep_CurveOnSurface> COS = occ::down_cast<BRep_CurveOnSurface>(CR);
        COS->UVPoints(Pf, Pl);
        OS << "  UV Points : " << Pf.X() << ", " << Pf.Y() << " ";
        OS << Pl.X() << ", " << Pl.Y() << "\n";
        if (CR->IsCurveOnClosedSurface())
        {
          occ::handle<BRep_CurveOnClosedSurface> COCS = occ::down_cast<BRep_CurveOnClosedSurface>(CR);
          COCS->UVPoints2(Pf, Pl);
          OS << "  UV Points : " << Pf.X() << ", " << Pf.Y() << " ";
          OS << Pl.X() << ", " << Pl.Y() << "\n";
        }
      }
      else if (CR->IsRegularity())
      {
        OS << "    - Regularity ";
        PrintRegularity(CR->Continuity(), OS);
        OS << "   on surfaces : " << mySurfaces.Index(CR->Surface());
        if (!CR->Location().IsIdentity())
          OS << " location " << Locations().Index(CR->Location());
        OS << ", " << mySurfaces.Index(CR->Surface2());
        if (!CR->Location2().IsIdentity())
          OS << " location " << Locations().Index(CR->Location2());
        OS << "\n";
      }
      else if (CR->IsPolygon3D())
      {
        occ::handle<BRep_Polygon3D> GC = occ::down_cast<BRep_Polygon3D>(itrc.Value());
        if (!GC->Polygon3D().IsNull())
        {
          OS << "    - Polygon 3D : " << myPolygons3D.FindIndex(CR->Polygon3D());
          if (!CR->Location().IsIdentity())
            OS << " location " << Locations().Index(CR->Location());
        }
      }
      else if (CR->IsPolygonOnTriangulation())
      {
        OS << "    - PolygonOnTriangulation " << myNodes.FindIndex(CR->PolygonOnTriangulation());
        if (CR->IsPolygonOnClosedTriangulation())
        {
          OS << " " << myNodes.FindIndex(CR->PolygonOnTriangulation2());
        }
        OS << " on triangulation " << myTriangulations.FindIndex(CR->Triangulation());
        if (!CR->Location().IsIdentity())
          OS << " location " << Locations().Index(CR->Location());
        OS << "\n";
      }
      itrc.Next();
    }
  }

  else if (S.ShapeType() == TopAbs_FACE)
  {

    const TopoDS_Face& F = TopoDS::Face(S);
    if (BRep_Tool::NaturalRestriction(F))
      OS << "NaturalRestriction\n";

    // Dump the surface geometry
    occ::handle<BRep_TFace> TF = occ::down_cast<BRep_TFace>(S.TShape());
    if (!TF->Surface().IsNull())
    {
      OS << "    Tolerance : " << TF->Tolerance() << "\n";
      OS << "    - Surface : " << mySurfaces.Index(TF->Surface());
      if (!S.Location().IsIdentity())
        OS << " location " << Locations().Index(S.Location());
      OS << "\n";
    }
    if (!(TF->Triangulation()).IsNull())
    {
      // Dump the triangulation
      OS << "    - Triangulation : " << myTriangulations.FindIndex(TF->Triangulation());
      if (!S.Location().IsIdentity())
        OS << " location " << Locations().Index(TF->Location());
      OS << "\n";
    }
  }

  OS << "\n";
}

//=================================================================================================

void BRepTools_ShapeSet::WriteGeometry(const TopoDS_Shape& S, Standard_OStream& OS) const
{
  // Write the geometry

  if (S.ShapeType() == TopAbs_VERTEX)
  {

    // Write the point geometry
    TopoDS_Vertex V = TopoDS::Vertex(S);
    OS << BRep_Tool::Tolerance(V) << "\n";
    gp_Pnt p = BRep_Tool::Pnt(V);
    OS << p.X() << " " << p.Y() << " " << p.Z() << "\n";

    occ::handle<BRep_TVertex>                         TV = occ::down_cast<BRep_TVertex>(S.TShape());
    NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itrp(TV->Points());

    while (itrp.More())
    {
      const occ::handle<BRep_PointRepresentation>& PR = itrp.Value();

      OS << PR->Parameter();
      if (PR->IsPointOnCurve())
      {
        OS << " 1 " << myCurves.Index(PR->Curve());
      }

      else if (PR->IsPointOnCurveOnSurface())
      {
        OS << " 2 " << myCurves2d.Index(PR->PCurve());
        OS << " " << mySurfaces.Index(PR->Surface());
      }

      else if (PR->IsPointOnSurface())
      {
        OS << " 3 " << PR->Parameter2() << " ";
        OS << mySurfaces.Index(PR->Surface());
      }

      OS << " " << Locations().Index(PR->Location());
      OS << "\n";

      itrp.Next();
    }

    OS << "0 0\n"; // end representations
  }

  else if (S.ShapeType() == TopAbs_EDGE)
  {

    // Write the curve geometry

    occ::handle<BRep_TEdge> TE = occ::down_cast<BRep_TEdge>(S.TShape());

    OS << " " << TE->Tolerance() << " ";
    OS << ((TE->SameParameter()) ? 1 : 0) << " ";
    OS << ((TE->SameRange()) ? 1 : 0) << " ";
    OS << ((TE->Degenerated()) ? 1 : 0) << "\n";

    double                                first, last;
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itrc = TE->Curves();
    while (itrc.More())
    {
      const occ::handle<BRep_CurveRepresentation>& CR = itrc.Value();
      if (CR->IsCurve3D())
      {
        if (!CR->Curve3D().IsNull())
        {
          occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itrc.Value());
          GC->Range(first, last);
          OS << "1 "; // -1- Curve 3D
          OS << " " << myCurves.Index(CR->Curve3D());
          OS << " " << Locations().Index(CR->Location());
          OS << " " << first << " " << last;
          OS << "\n";
        }
      }
      else if (CR->IsCurveOnSurface())
      {
        occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itrc.Value());
        GC->Range(first, last);
        if (!CR->IsCurveOnClosedSurface())
          OS << "2 "; // -2- Curve on surf
        else
          OS << "3 "; // -3- Curve on closed surf
        OS << " " << myCurves2d.Index(CR->PCurve());
        if (CR->IsCurveOnClosedSurface())
        {
          OS << " " << myCurves2d.Index(CR->PCurve2());
          PrintRegularity(CR->Continuity(), OS);
        }
        OS << " " << mySurfaces.Index(CR->Surface());
        OS << " " << Locations().Index(CR->Location());
        OS << " " << first << " " << last;
        OS << "\n";

        // Write UV Points // for XML Persistence higher performance
        if (FormatNb() == TopTools_FormatVersion_VERSION_2)
        {
          gp_Pnt2d Pf, Pl;
          if (CR->IsCurveOnClosedSurface())
          {
            occ::handle<BRep_CurveOnClosedSurface> COCS =
              occ::down_cast<BRep_CurveOnClosedSurface>(CR);
            COCS->UVPoints2(Pf, Pl);
          }
          else
          {
            occ::handle<BRep_CurveOnSurface> COS = occ::down_cast<BRep_CurveOnSurface>(CR);
            COS->UVPoints(Pf, Pl);
          }
          OS << Pf.X() << " " << Pf.Y() << " " << Pl.X() << " " << Pl.Y() << "\n";
        }
      }
      else if (CR->IsRegularity())
      {
        OS << "4 "; // -4- Regularity
        PrintRegularity(CR->Continuity(), OS);
        OS << " " << mySurfaces.Index(CR->Surface());
        OS << " " << Locations().Index(CR->Location());
        OS << " " << mySurfaces.Index(CR->Surface2());
        OS << " " << Locations().Index(CR->Location2());
        OS << "\n";
      }

      else if (myWithTriangles)
      { // for XML Persistence
        if (CR->IsPolygon3D())
        {
          occ::handle<BRep_Polygon3D> GC = occ::down_cast<BRep_Polygon3D>(itrc.Value());
          if (!GC->Polygon3D().IsNull())
          {
            OS << "5 "; // -5- Polygon3D
            OS << " " << myPolygons3D.FindIndex(CR->Polygon3D());
            OS << " " << Locations().Index(CR->Location());
            OS << "\n";
          }
        }
        else if (CR->IsPolygonOnTriangulation())
        {
          occ::handle<BRep_PolygonOnTriangulation> PT =
            occ::down_cast<BRep_PolygonOnTriangulation>(itrc.Value());
          if (!CR->IsPolygonOnClosedTriangulation())
            OS << "6 "; // -6- Polygon on triangulation
          else
            OS << "7 "; // -7- Polygon on closed triangulation
          OS << " " << myNodes.FindIndex(PT->PolygonOnTriangulation());
          if (CR->IsPolygonOnClosedTriangulation())
          {
            OS << " " << myNodes.FindIndex(PT->PolygonOnTriangulation2());
          }
          OS << " " << myTriangulations.FindIndex(PT->Triangulation());
          OS << " " << Locations().Index(CR->Location());
          OS << "\n";
        }
      }

      itrc.Next();
    }
    OS << "0\n"; // end of the list of representations
  }

  else if (S.ShapeType() == TopAbs_FACE)
  {

    occ::handle<BRep_TFace> TF = occ::down_cast<BRep_TFace>(S.TShape());
    const TopoDS_Face& F  = TopoDS::Face(S);

    if (!(TF->Surface()).IsNull())
    {
      OS << ((BRep_Tool::NaturalRestriction(F)) ? 1 : 0);
      OS << " ";
      // Write the surface geometry
      OS << " " << TF->Tolerance();
      OS << " " << mySurfaces.Index(TF->Surface());
      OS << " " << Locations().Index(TF->Location());
      OS << "\n";
    }
    else // For correct reading of null face
    {
      OS << 0;
      OS << " ";
      OS << " " << TF->Tolerance();
      OS << " " << 0;
      OS << " " << 0;
      OS << "\n";
    }
    if (myWithTriangles || TF->Surface().IsNull())
    { // for XML Persistence
      if (!(TF->Triangulation()).IsNull())
      {
        OS << 2;
        OS << " ";
        // Write the triangulation
        OS << " " << myTriangulations.FindIndex(TF->Triangulation());
      }
    }
  }
}

//=================================================================================================

static GeomAbs_Shape ReadRegularity(Standard_IStream& IS)
{
  char buffer[255];
  IS >> buffer;
  switch (buffer[0])
  {

    case 'C':
      switch (buffer[1])
      {

        case '0':
          return GeomAbs_C0;

        case '1':
          return GeomAbs_C1;

        case '2':
          return GeomAbs_C2;

        case '3':
          return GeomAbs_C3;

        case 'N':
          return GeomAbs_CN;
      }
      break;

    case 'G':
      switch (buffer[1])
      {

        case '1':
          return GeomAbs_G1;

        case '2':
          return GeomAbs_G2;
      }
      break;
  }
  return GeomAbs_C0;
}

//=================================================================================================

void BRepTools_ShapeSet::ReadGeometry(const TopAbs_ShapeEnum T,
                                      Standard_IStream&      IS,
                                      TopoDS_Shape&          S)
{
  // Read the geometry

  int val, c, pc, pc2 = 0, s, s2, l, l2, t, pt, pt2 = 0;
  double    tol, X, Y, Z, first, last, p1, p2;
  double    PfX, PfY, PlX, PlY;
  gp_Pnt2d         aPf, aPl;
  bool closed;
  GeomAbs_Shape    reg = GeomAbs_C0;
  switch (T)
  {

      //---------
      // vertex
      //---------

    case TopAbs_VERTEX: {
      TopoDS_Vertex& V = TopoDS::Vertex(S);

      // Read the point geometry
      GeomTools::GetReal(IS, tol);
      GeomTools::GetReal(IS, X);
      GeomTools::GetReal(IS, Y);
      GeomTools::GetReal(IS, Z);
      myBuilder.MakeVertex(V, gp_Pnt(X, Y, Z), tol);
      occ::handle<BRep_TVertex> TV = occ::down_cast<BRep_TVertex>(V.TShape());

      NCollection_List<occ::handle<BRep_PointRepresentation>>& lpr = TV->ChangePoints();
      TopLoc_Location                 L;

      do
      {
        GeomTools::GetReal(IS, p1);
        IS >> val;

        occ::handle<BRep_PointRepresentation> PR;
        switch (val)
        {

          case 1: {
            IS >> c;

            //  Modified by Sergey KHROMOV - Wed Apr 24 13:59:09 2002 Begin
            if (myCurves.Curve(c).IsNull())
              break;
            //  Modified by Sergey KHROMOV - Wed Apr 24 13:59:13 2002 End

            occ::handle<BRep_PointOnCurve> POC = new BRep_PointOnCurve(p1, myCurves.Curve(c), L);
            PR                            = POC;
          }
          break;

          case 2: {
            IS >> pc >> s;

            //  Modified by Sergey KHROMOV - Wed Apr 24 13:59:09 2002 Begin
            if (myCurves2d.Curve2d(pc).IsNull() || mySurfaces.Surface(s).IsNull())
              break;
            //  Modified by Sergey KHROMOV - Wed Apr 24 13:59:13 2002 End

            occ::handle<BRep_PointOnCurveOnSurface> POC =
              new BRep_PointOnCurveOnSurface(p1, myCurves2d.Curve2d(pc), mySurfaces.Surface(s), L);
            PR = POC;
          }
          break;

          case 3: {
            GeomTools::GetReal(IS, p2);
            IS >> s;

            //  Modified by Sergey KHROMOV - Wed Apr 24 13:59:09 2002 Begin
            if (mySurfaces.Surface(s).IsNull())
              break;
            //  Modified by Sergey KHROMOV - Wed Apr 24 13:59:13 2002 End

            occ::handle<BRep_PointOnSurface> POC =
              new BRep_PointOnSurface(p1, p2, mySurfaces.Surface(s), L);
            PR = POC;
          }
          break;
        }

        if (val > 0)
        {
          IS >> l;
          if (!PR.IsNull())
          {
            PR->Location(Locations().Location(l));
            lpr.Append(PR);
          }
        }
      } while (val > 0);
    }
    break;

      //---------
      // edge
      //---------

    case TopAbs_EDGE:

      // Create an edge
      {
        TopoDS_Edge& E = TopoDS::Edge(S);

        myBuilder.MakeEdge(E);

        // Read the curve geometry
        GeomTools::GetReal(IS, tol);
        IS >> val;
        myBuilder.SameParameter(E, (val == 1));
        IS >> val;
        myBuilder.SameRange(E, (val == 1));
        IS >> val;
        myBuilder.Degenerated(E, (val == 1));

        do
        {
          IS >> val;
          switch (val)
          {

            case 1: // -1- Curve 3D
              IS >> c >> l;
              if (!myCurves.Curve(c).IsNull())
              {
                myBuilder.UpdateEdge(E, myCurves.Curve(c), Locations().Location(l), tol);
              }
              GeomTools::GetReal(IS, first);
              GeomTools::GetReal(IS, last);
              if (!myCurves.Curve(c).IsNull())
              {
                bool Only3d = true;
                myBuilder.Range(E, first, last, Only3d);
              }
              break;

            case 2: // -2- Curve on surf
            case 3: // -3- Curve on closed surf
              closed = (val == 3);
              IS >> pc;
              if (closed)
              {
                IS >> pc2;
                reg = ReadRegularity(IS);
              }

              // surface, location
              IS >> s >> l;

              // range
              GeomTools::GetReal(IS, first);
              GeomTools::GetReal(IS, last);

              // read UV Points // for XML Persistence higher performance
              if (FormatNb() == TopTools_FormatVersion_VERSION_2)
              {
                GeomTools::GetReal(IS, PfX);
                GeomTools::GetReal(IS, PfY);
                GeomTools::GetReal(IS, PlX);
                GeomTools::GetReal(IS, PlY);
                aPf = gp_Pnt2d(PfX, PfY);
                aPl = gp_Pnt2d(PlX, PlY);
              }

              //  Modified by Sergey KHROMOV - Wed Apr 24 12:11:16 2002 Begin
              if (myCurves2d.Curve2d(pc).IsNull() || (closed && myCurves2d.Curve2d(pc2).IsNull())
                  || mySurfaces.Surface(s).IsNull())
                break;
              //  Modified by Sergey KHROMOV - Wed Apr 24 12:11:17 2002 End

              if (closed)
              {
                if (FormatNb() == TopTools_FormatVersion_VERSION_2)
                  myBuilder.UpdateEdge(E,
                                       myCurves2d.Curve2d(pc),
                                       myCurves2d.Curve2d(pc2),
                                       mySurfaces.Surface(s),
                                       Locations().Location(l),
                                       tol,
                                       aPf,
                                       aPl);
                else
                  myBuilder.UpdateEdge(E,
                                       myCurves2d.Curve2d(pc),
                                       myCurves2d.Curve2d(pc2),
                                       mySurfaces.Surface(s),
                                       Locations().Location(l),
                                       tol);

                myBuilder.Continuity(E,
                                     mySurfaces.Surface(s),
                                     mySurfaces.Surface(s),
                                     Locations().Location(l),
                                     Locations().Location(l),
                                     reg);
              }
              else
              {
                if (FormatNb() == TopTools_FormatVersion_VERSION_2)
                  myBuilder.UpdateEdge(E,
                                       myCurves2d.Curve2d(pc),
                                       mySurfaces.Surface(s),
                                       Locations().Location(l),
                                       tol,
                                       aPf,
                                       aPl);
                else
                  myBuilder.UpdateEdge(E,
                                       myCurves2d.Curve2d(pc),
                                       mySurfaces.Surface(s),
                                       Locations().Location(l),
                                       tol);
              }
              myBuilder.Range(E, mySurfaces.Surface(s), Locations().Location(l), first, last);
              break;

            case 4: // -4- Regularity
              reg = ReadRegularity(IS);
              IS >> s >> l >> s2 >> l2;
              //  Modified by Sergey KHROMOV - Wed Apr 24 12:39:13 2002 Begin
              if (mySurfaces.Surface(s).IsNull() || mySurfaces.Surface(s2).IsNull())
                break;
              //  Modified by Sergey KHROMOV - Wed Apr 24 12:39:14 2002 End
              myBuilder.Continuity(E,
                                   mySurfaces.Surface(s),
                                   mySurfaces.Surface(s2),
                                   Locations().Location(l),
                                   Locations().Location(l2),
                                   reg);
              break;

            case 5: // -5- Polygon3D
              IS >> c >> l;
              // szy-02.05.2004
              // myBuilder.UpdateEdge(E,occ::down_cast<Poly_Polygon3D>(myPolygons3D(c)));
              if (c > 0 && c <= myPolygons3D.Extent())
                myBuilder.UpdateEdge(E,
                                     occ::down_cast<Poly_Polygon3D>(myPolygons3D(c)),
                                     Locations().Location(l));
              break;

            case 6:
            case 7:
              closed = (val == 7);
              IS >> pt;
              if (closed)
              {
                IS >> pt2;
              }
              IS >> t >> l;
              if (closed)
              {
                if (t > 0 && t <= myTriangulations.Extent() && pt > 0 && pt <= myNodes.Extent()
                    && pt2 > 0 && pt2 <= myNodes.Extent())
                  myBuilder.UpdateEdge(E,
                                       occ::down_cast<Poly_PolygonOnTriangulation>(myNodes(pt)),
                                       occ::down_cast<Poly_PolygonOnTriangulation>(myNodes(pt2)),
                                       myTriangulations.FindKey(t),
                                       Locations().Location(l));
              }
              else
              {
                if (t > 0 && t <= myTriangulations.Extent() && pt > 0 && pt <= myNodes.Extent())
                  myBuilder.UpdateEdge(E,
                                       occ::down_cast<Poly_PolygonOnTriangulation>(myNodes(pt)),
                                       myTriangulations.FindKey(t),
                                       Locations().Location(l));
              }
              // range

              break;
          }
        } while (val > 0);
      }
      break;

      //---------
      // wire
      //---------

    case TopAbs_WIRE:
      myBuilder.MakeWire(TopoDS::Wire(S));
      break;

      //---------
      // face
      //---------

    case TopAbs_FACE: {
      // create a face :
      TopoDS_Face& F = TopoDS::Face(S);
      //    std::streampos pos;
      myBuilder.MakeFace(F);

      IS >> val; // natural restriction
      if (val == 0 || val == 1)
      {
        GeomTools::GetReal(IS, tol);
        IS >> s >> l;
        //  Modified by Sergey KHROMOV - Wed Apr 24 12:39:13 2002 Begin
        if (!mySurfaces.Surface(s).IsNull())
        {
          //  Modified by Sergey KHROMOV - Wed Apr 24 12:39:14 2002 End
          myBuilder.UpdateFace(TopoDS::Face(S),
                               mySurfaces.Surface(s),
                               Locations().Location(l),
                               tol);
          myBuilder.NaturalRestriction(TopoDS::Face(S), (val == 1));
        }
        //      pos = IS.tellg();
        //      IS >> val;
      }
      else if (val == 2)
      {
        // only triangulation
        IS >> s;
        myBuilder.UpdateFace(TopoDS::Face(S), myTriangulations.FindKey(s));
      }
      //    else pos = IS.tellg();

      // BUC60769

      if (val == 2)
        break;

      char string[260];
      IS.getline(string, 256, '\n');
      IS.getline(string, 256, '\n');

      if (string[0] == '2')
      {
        // cas triangulation
        s = atoi(&string[2]);
        if (s > 0 && s <= myTriangulations.Extent())
          myBuilder.UpdateFace(TopoDS::Face(S), myTriangulations.FindKey(s));
      }
      //    else IS.seekg(pos);
    }
    break;

      //---------
      // shell
      //---------

    case TopAbs_SHELL:
      myBuilder.MakeShell(TopoDS::Shell(S));
      break;

      //---------
      // solid
      //---------

    case TopAbs_SOLID:
      myBuilder.MakeSolid(TopoDS::Solid(S));
      break;

      //---------
      // compsolid
      //---------

    case TopAbs_COMPSOLID:
      myBuilder.MakeCompSolid(TopoDS::CompSolid(S));
      break;

      //---------
      // compound
      //---------

    case TopAbs_COMPOUND:
      myBuilder.MakeCompound(TopoDS::Compound(S));
      break;

    default:
      break;
  }
}

//=================================================================================================

void BRepTools_ShapeSet::AddShapes(TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  myBuilder.Add(S1, S2);
}

//=================================================================================================

void BRepTools_ShapeSet::Check(const TopAbs_ShapeEnum T, TopoDS_Shape& S)
{
  if (T == TopAbs_FACE)
  {
    const TopoDS_Face& F = TopoDS::Face(S);
    BRepTools::Update(F);
  }
}

//=================================================================================================

void BRepTools_ShapeSet::WritePolygonOnTriangulation(Standard_OStream&            OS,
                                                     const bool       Compact,
                                                     const Message_ProgressRange& theProgress) const
{
  int i, j, nbpOntri = myNodes.Extent();

  Message_ProgressScope aPS(theProgress, "Polygons On Triangulation", nbpOntri);
  if (Compact)
    OS << "PolygonOnTriangulations " << nbpOntri << "\n";
  else
  {
    OS << " -------\n";
    OS << "Dump of " << nbpOntri << " PolygonOnTriangulations\n";
    OS << " -------\n";
  }

  occ::handle<Poly_PolygonOnTriangulation> Poly;
  occ::handle<NCollection_HArray1<double>>       Param;
  for (i = 1; i <= nbpOntri && aPS.More(); i++, aPS.Next())
  {
    Poly = occ::down_cast<Poly_PolygonOnTriangulation>(myNodes(i));
    const NCollection_Array1<int>& Nodes = Poly->Nodes();
    if (!Compact)
    {
      OS << "  " << i << " : PolygonOnTriangulation with " << Nodes.Length() << " Nodes\n";
    }
    else
      OS << Nodes.Length() << " ";
    if (!Compact)
      OS << "  ";
    for (j = 1; j <= Nodes.Length(); j++)
      OS << Nodes.Value(j) << " ";
    OS << "\n";

    // writing parameters:
    Param = Poly->Parameters();
    if (Compact)
      OS << "p ";

    // write the deflection
    if (!Compact)
      OS << "  Deflection : ";
    OS << Poly->Deflection() << " ";
    if (!Compact)
      OS << "\n";

    if (!Param.IsNull())
    {
      if (!Compact)
      {
        OS << "  Parameters :";
      }
      else
        OS << "1 ";
      if (!Compact)
        OS << "  ";
      for (j = 1; j <= Param->Length(); j++)
        OS << Param->Value(j) << " ";
      OS << "\n";
    }
    else
      OS << "0 \n";
  }
}

//=================================================================================================

void BRepTools_ShapeSet::DumpPolygonOnTriangulation(Standard_OStream& OS) const
{
  WritePolygonOnTriangulation(OS, false);
}

//=================================================================================================

void BRepTools_ShapeSet::ReadPolygonOnTriangulation(Standard_IStream&            IS,
                                                    const Message_ProgressRange& theProgress)
{
  char buffer[255];
  IS >> buffer;
  if (strstr(buffer, "PolygonOnTriangulations") == NULL)
    return;
  int                    i, j, val, nbpol = 0, nbnodes = 0;
  int                    hasparameters;
  double                       par;
  occ::handle<NCollection_HArray1<double>>       Param;
  occ::handle<Poly_PolygonOnTriangulation> Poly;
  IS >> nbpol;
  // OCC19559
  Message_ProgressScope aPS(theProgress, "Polygons On Triangulation", nbpol);
  for (i = 1; i <= nbpol && aPS.More(); i++, aPS.Next())
  {
    IS >> nbnodes;
    NCollection_Array1<int> Nodes(1, nbnodes);
    for (j = 1; j <= nbnodes; j++)
    {
      IS >> val;
      Nodes(j) = val;
    }
    IS >> buffer;
    //      if (!strcasecmp(buffer, "p")) {
    double def;
    GeomTools::GetReal(IS, def);
    IS >> hasparameters;
    if (hasparameters)
    {
      NCollection_Array1<double> Param1(1, nbnodes);
      for (j = 1; j <= nbnodes; j++)
      {
        GeomTools::GetReal(IS, par);
        Param1(j) = par;
      }
      Poly = new Poly_PolygonOnTriangulation(Nodes, Param1);
    }
    else
      Poly = new Poly_PolygonOnTriangulation(Nodes);
    Poly->Deflection(def);
    //      }
    //      else {
    //      IS.seekg(ppp);
    //      Poly = new Poly_PolygonOnTriangulation(Nodes);
    //      }
    myNodes.Add(Poly);
  }
  //  }
  //  else IS.seekg(pos);
}

//=================================================================================================

void BRepTools_ShapeSet::WritePolygon3D(Standard_OStream&            OS,
                                        const bool       Compact,
                                        const Message_ProgressRange& theProgress) const
{
  int i, j, nbpol = myPolygons3D.Extent();

  Message_ProgressScope aPS(theProgress, "3D Polygons", nbpol);

  if (Compact)
    OS << "Polygon3D " << nbpol << "\n";
  else
  {
    OS << " -------\n";
    OS << "Dump of " << nbpol << " Polygon3Ds\n";
    OS << " -------\n";
  }

  occ::handle<Poly_Polygon3D> P;
  for (i = 1; i <= nbpol && aPS.More(); i++, aPS.Next())
  {
    P = occ::down_cast<Poly_Polygon3D>(myPolygons3D(i));
    if (Compact)
    {
      OS << P->NbNodes() << " ";
      OS << ((P->HasParameters()) ? "1" : "0") << "\n";
    }
    else
    {
      OS << "  " << i << " : Polygon3D with " << P->NbNodes() << " Nodes\n";
      OS << ((P->HasParameters()) ? "with" : "without") << " parameters\n";
    }

    // write the deflection
    if (!Compact)
      OS << "Deflection : ";
    OS << P->Deflection() << "\n";

    // write the nodes
    if (!Compact)
      OS << "\nNodes :\n";

    int          i1, nbNodes = P->NbNodes();
    const NCollection_Array1<gp_Pnt>& Nodes = P->Nodes();
    for (j = 1; j <= nbNodes; j++)
    {
      if (!Compact)
        OS << std::setw(10) << j << " : ";
      if (!Compact)
        OS << std::setw(17);
      OS << Nodes(j).X() << " ";
      if (!Compact)
        OS << std::setw(17);
      OS << Nodes(j).Y() << " ";
      if (!Compact)
        OS << std::setw(17);
      OS << Nodes(j).Z();
      if (!Compact)
        OS << "\n";
      else
        OS << " ";
    }
    OS << "\n";

    if (P->HasParameters())
    {
      if (!Compact)
        OS << "\nParameters :\n";
      const NCollection_Array1<double>& Param = P->Parameters();
      for (i1 = 1; i1 <= nbNodes; i1++)
      {
        OS << Param(i1) << " ";
      }
      OS << "\n";
    }
  }
}

//=================================================================================================

void BRepTools_ShapeSet::DumpPolygon3D(Standard_OStream& OS) const
{
  WritePolygon3D(OS, false);
}

//=================================================================================================

void BRepTools_ShapeSet::ReadPolygon3D(Standard_IStream&            IS,
                                       const Message_ProgressRange& theProgress)
{
  char buffer[255];
  //  int i, j, p, val, nbpol, nbnodes, hasparameters;
  int i, j, p, nbpol = 0, nbnodes = 0, hasparameters = false;
  double    d, x, y, z;

  IS >> buffer;
  if (strstr(buffer, "Polygon3D") == NULL)
    return;
  occ::handle<Poly_Polygon3D> P;
  IS >> nbpol;
  // OCC19559
  Message_ProgressScope aPS(theProgress, "3D Polygons", nbpol);
  for (i = 1; i <= nbpol && aPS.More(); i++, aPS.Next())
  {
    IS >> nbnodes;
    IS >> hasparameters;
    NCollection_Array1<gp_Pnt> Nodes(1, nbnodes);
    GeomTools::GetReal(IS, d);
    for (j = 1; j <= nbnodes; j++)
    {
      GeomTools::GetReal(IS, x);
      GeomTools::GetReal(IS, y);
      GeomTools::GetReal(IS, z);
      Nodes(j).SetCoord(x, y, z);
    }
    if (hasparameters)
    {
      NCollection_Array1<double> Param(1, nbnodes);
      for (p = 1; p <= nbnodes; p++)
      {
        GeomTools::GetReal(IS, Param(p));
      }
      P = new Poly_Polygon3D(Nodes, Param);
    }
    else
      P = new Poly_Polygon3D(Nodes);
    P->Deflection(d);
    myPolygons3D.Add(P);
  }
}

//=================================================================================================

void BRepTools_ShapeSet::WriteTriangulation(Standard_OStream&            OS,
                                            const bool       Compact,
                                            const Message_ProgressRange& theProgress) const
{
  int i, j, nbNodes, nbtri = myTriangulations.Extent();
  int nbTriangles = 0, n1, n2, n3;

  Message_ProgressScope aPS(theProgress, "Triangulations", nbtri);

  if (Compact)
    OS << "Triangulations " << nbtri << "\n";
  else
  {
    OS << " -------\n";
    OS << "Dump of " << nbtri << " Triangulations\n";
    OS << " -------\n";
  }

  occ::handle<Poly_Triangulation> T;
  for (i = 1; i <= nbtri && aPS.More(); i++, aPS.Next())
  {

    T                                     = myTriangulations.FindKey(i);
    const bool toWriteNormals = myTriangulations(i);
    if (Compact)
    {
      OS << T->NbNodes() << " " << T->NbTriangles() << " ";
      OS << ((T->HasUVNodes()) ? "1" : "0") << " ";
      if (FormatNb() >= TopTools_FormatVersion_VERSION_3)
      {
        OS << ((T->HasNormals() && toWriteNormals) ? "1" : "0") << " ";
      }
    }
    else
    {
      OS << "  " << i << " : Triangulation with " << T->NbNodes() << " Nodes and "
         << T->NbTriangles() << " Triangles\n";
      OS << "      " << ((T->HasUVNodes()) ? "with" : "without") << " UV nodes\n";
      if (FormatNb() >= TopTools_FormatVersion_VERSION_3)
      {
        OS << "      " << ((T->HasNormals() && toWriteNormals) ? "with" : "without")
           << " normals\n";
      }
    }

    // write the deflection

    if (!Compact)
      OS << "  Deflection : ";
    OS << T->Deflection() << "\n";

    // write the 3d nodes

    if (!Compact)
      OS << "\n3D Nodes :\n";

    nbNodes = T->NbNodes();
    for (j = 1; j <= nbNodes; j++)
    {
      const gp_Pnt aNode = T->Node(j);
      if (!Compact)
        OS << std::setw(10) << j << " : ";
      if (!Compact)
        OS << std::setw(17);
      OS << aNode.X() << " ";
      if (!Compact)
        OS << std::setw(17);
      OS << aNode.Y() << " ";
      if (!Compact)
        OS << std::setw(17);
      OS << aNode.Z();
      if (!Compact)
        OS << "\n";
      else
        OS << " ";
    }

    if (T->HasUVNodes())
    {
      if (!Compact)
        OS << "\nUV Nodes :\n";
      for (j = 1; j <= nbNodes; j++)
      {
        const gp_Pnt2d aNode2d = T->UVNode(j);
        if (!Compact)
          OS << std::setw(10) << j << " : ";
        if (!Compact)
          OS << std::setw(17);
        OS << aNode2d.X() << " ";
        if (!Compact)
          OS << std::setw(17);
        OS << aNode2d.Y();
        if (!Compact)
          OS << "\n";
        else
          OS << " ";
      }
    }

    if (!Compact)
      OS << "\nTriangles :\n";
    nbTriangles = T->NbTriangles();
    for (j = 1; j <= nbTriangles; j++)
    {
      if (!Compact)
        OS << std::setw(10) << j << " : ";
      T->Triangle(j).Get(n1, n2, n3);
      if (!Compact)
        OS << std::setw(10);
      OS << n1 << " ";
      if (!Compact)
        OS << std::setw(10);
      OS << n2 << " ";
      if (!Compact)
        OS << std::setw(10);
      OS << n3;
      if (!Compact)
        OS << "\n";
      else
        OS << " ";
    }

    if (FormatNb() >= TopTools_FormatVersion_VERSION_3)
    {
      if (T->HasNormals() && toWriteNormals)
      {
        if (!Compact)
          OS << "\nNormals :\n";
        for (j = 1; j <= nbNodes; j++)
        {
          if (!Compact)
          {
            OS << std::setw(10) << j << " : ";
            OS << std::setw(17);
          }
          NCollection_Vec3<float> aNorm;
          for (int k = 0; k < 3; ++k)
          {
            T->Normal(j, aNorm);
            OS << aNorm[k];
            OS << (!Compact ? "\n" : " ");
          }
        }
      }
    }
    OS << "\n";
  }
}

//=================================================================================================

void BRepTools_ShapeSet::DumpTriangulation(Standard_OStream& OS) const
{
  WriteTriangulation(OS, false);
}

//=================================================================================================

void BRepTools_ShapeSet::ReadTriangulation(Standard_IStream&            IS,
                                           const Message_ProgressRange& theProgress)
{
  char             buffer[255];
  int i, j, nbtri = 0;
  double    d, x, y, z;
  int nbNodes = 0, nbTriangles = 0;
  bool hasUV      = false;
  bool hasNormals = false;

  occ::handle<Poly_Triangulation> T;

  IS >> buffer;
  if (strstr(buffer, "Triangulations") == NULL)
    return;

  IS >> nbtri;
  // OCC19559
  Message_ProgressScope aPS(theProgress, "Triangulations", nbtri);
  for (i = 1; i <= nbtri && aPS.More(); i++, aPS.Next())
  {

    IS >> nbNodes >> nbTriangles >> hasUV;
    if (FormatNb() >= TopTools_FormatVersion_VERSION_3)
    {
      IS >> hasNormals;
    }
    GeomTools::GetReal(IS, d);

    T = new Poly_Triangulation(nbNodes, nbTriangles, hasUV, hasNormals);

    for (j = 1; j <= nbNodes; j++)
    {
      GeomTools::GetReal(IS, x);
      GeomTools::GetReal(IS, y);
      GeomTools::GetReal(IS, z);
      T->SetNode(j, gp_Pnt(x, y, z));
    }

    if (hasUV)
    {
      for (j = 1; j <= nbNodes; j++)
      {
        GeomTools::GetReal(IS, x);
        GeomTools::GetReal(IS, y);
        T->SetUVNode(j, gp_Pnt2d(x, y));
      }
    }

    // read the triangles
    int n1, n2, n3;
    for (j = 1; j <= nbTriangles; j++)
    {
      IS >> n1 >> n2 >> n3;
      T->SetTriangle(j, Poly_Triangle(n1, n2, n3));
    }

    if (hasNormals)
    {
      NCollection_Vec3<double> aNorm;
      for (j = 1; j <= nbNodes; j++)
      {
        GeomTools::GetReal(IS, aNorm.x());
        GeomTools::GetReal(IS, aNorm.y());
        GeomTools::GetReal(IS, aNorm.z());
        T->SetNormal(j, NCollection_Vec3<float>(aNorm));
      }
    }

    T->Deflection(d);
    myTriangulations.Add(T, hasNormals);
  }
}
