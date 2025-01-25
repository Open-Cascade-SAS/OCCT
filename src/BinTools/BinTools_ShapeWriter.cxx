// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <BinTools_ShapeWriter.hxx>
#include <BinTools_LocationSet.hxx>

#include <TopoDS.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TFace.hxx>
#include <BinTools_CurveSet.hxx>
#include <BinTools_Curve2dSet.hxx>
#include <BinTools_SurfaceSet.hxx>
#include <BinTools_OStream.hxx>

//=================================================================================================

BinTools_ShapeWriter::BinTools_ShapeWriter()
    : BinTools_ShapeSetBase()
{
}

//=================================================================================================

BinTools_ShapeWriter::~BinTools_ShapeWriter() {}

//=================================================================================================

void BinTools_ShapeWriter::Clear()
{
  BinTools_ShapeSetBase::Clear();
  myShapePos.Clear();
  myLocationPos.Clear();
  myCurvePos.Clear();
  myCurve2dPos.Clear();
  mySurfacePos.Clear();
  myPolygon3dPos.Clear();
  myPolygonPos.Clear();
  myTriangulationPos.Clear();
}

//=================================================================================================

void BinTools_ShapeWriter::Write(const TopoDS_Shape& theShape, Standard_OStream& theStream)
{
  BinTools_OStream anOStream(theStream);
  WriteShape(anOStream, theShape);
}

//=================================================================================================

void BinTools_ShapeWriter::WriteShape(BinTools_OStream& theStream, const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
  {
    theStream << BinTools_ObjectType_EmptyShape;
    return;
  }
  TopoDS_Shape    aShape     = theShape.Located(TopLoc_Location());
  const uint64_t* anExisting = myShapePos.Seek(aShape);
  if (anExisting) // shape is already there, so, write reference to it
  {
    theStream.WriteReference(*anExisting);
    WriteLocation(theStream, theShape.Location());
    theStream << Standard_Byte(theShape.Orientation());
    return;
  }
  uint64_t aNewPos = theStream.Position();
  myShapePos.Bind(aShape, aNewPos);
  theStream.WriteShape(aShape.ShapeType(), aShape.Orientation());
  WriteLocation(theStream, theShape.Location());

  try
  {
    OCC_CATCH_SIGNALS
    switch (aShape.ShapeType())
    {
      case TopAbs_VERTEX: {
        TopoDS_Vertex aV = TopoDS::Vertex(aShape);
        theStream << BRep_Tool::Tolerance(aV);
        gp_Pnt aP = BRep_Tool::Pnt(aV);
        theStream << aP;
        Handle(BRep_TVertex) aTV = Handle(BRep_TVertex)::DownCast(aShape.TShape());
        for (BRep_ListIteratorOfListOfPointRepresentation anIter(aTV->Points()); anIter.More();
             anIter.Next())
        {
          const Handle(BRep_PointRepresentation)& aPR = anIter.Value();
          if (aPR->IsPointOnCurve())
          {
            theStream << (Standard_Byte)1; // 1
            theStream << aPR->Parameter();
            WriteCurve(theStream, aPR->Curve());
          }
          else if (aPR->IsPointOnCurveOnSurface())
          {
            theStream << (Standard_Byte)2; // 2
            theStream << aPR->Parameter();
            WriteCurve(theStream, aPR->PCurve());
            WriteSurface(theStream, aPR->Surface());
          }
          else if (aPR->IsPointOnSurface())
          {
            theStream << (Standard_Byte)3; // 3
            theStream << aPR->Parameter2() << aPR->Parameter();
            WriteSurface(theStream, aPR->Surface());
          }
          WriteLocation(theStream, aPR->Location());
        }
        theStream << (Standard_Byte)0;
      }
      break;
      case TopAbs_EDGE: {
        Handle(BRep_TEdge) aTE = Handle(BRep_TEdge)::DownCast(aShape.TShape());
        theStream << aTE->Tolerance();
        theStream.PutBools(aTE->SameParameter(), aTE->SameRange(), aTE->Degenerated());
        Standard_Real aFirst, aLast;
        for (BRep_ListIteratorOfListOfCurveRepresentation anIter = aTE->Curves(); anIter.More();
             anIter.Next())
        {
          const Handle(BRep_CurveRepresentation)& aCR = anIter.Value();
          if (aCR->IsCurve3D())
          {
            if (!aCR->Curve3D().IsNull())
            {
              Handle(BRep_GCurve) aGC = Handle(BRep_GCurve)::DownCast(aCR);
              aGC->Range(aFirst, aLast);
              theStream << (Standard_Byte)1; // -1- CURVE_3D;
              WriteCurve(theStream, aCR->Curve3D());
              WriteLocation(theStream, aCR->Location());
              theStream << aFirst << aLast;
            }
          }
          else if (aCR->IsCurveOnSurface())
          {
            Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(aCR);
            GC->Range(aFirst, aLast);
            if (!aCR->IsCurveOnClosedSurface())
              theStream << (Standard_Byte)2; // -2- Curve on surf
            else
              theStream << (Standard_Byte)3; // -3- Curve on closed surf
            WriteCurve(theStream, aCR->PCurve());

            if (aCR->IsCurveOnClosedSurface())
            { //+ int|char
              WriteCurve(theStream, aCR->PCurve2());
              theStream << (Standard_Byte)aCR->Continuity();
            }
            WriteSurface(theStream, aCR->Surface());
            WriteLocation(theStream, aCR->Location());
            theStream << aFirst << aLast;
          }
          else if (aCR->IsRegularity())
          {
            theStream << (Standard_Byte)4; // -4- Regularity
            theStream << (Standard_Byte)aCR->Continuity();
            WriteSurface(theStream, aCR->Surface());
            WriteLocation(theStream, aCR->Location());
            WriteSurface(theStream, aCR->Surface2());
            WriteLocation(theStream, aCR->Location2());
          }
          else if (IsWithTriangles())
          {
            if (aCR->IsPolygon3D())
            {
              Handle(BRep_Polygon3D) aGC = Handle(BRep_Polygon3D)::DownCast(aCR);
              if (!aGC->Polygon3D().IsNull())
              {
                theStream << (Standard_Byte)5; // -5- Polygon3D
                WritePolygon(theStream, aCR->Polygon3D());
                WriteLocation(theStream, aCR->Location());
              }
            }
            else if (aCR->IsPolygonOnTriangulation())
            {
              Handle(BRep_PolygonOnTriangulation) aPT =
                Handle(BRep_PolygonOnTriangulation)::DownCast(aCR);
              if (!aCR->IsPolygonOnClosedTriangulation())
                theStream << (Standard_Byte)6; // -6- Polygon on triangulation
              else
                theStream << (Standard_Byte)7; // -7- Polygon on closed triangulation
              WritePolygon(theStream, aPT->PolygonOnTriangulation());

              if (aCR->IsPolygonOnClosedTriangulation())
                WritePolygon(theStream, aPT->PolygonOnTriangulation2());
              // edge triangulation does not need normals
              WriteTriangulation(theStream, aPT->Triangulation(), Standard_False);
              WriteLocation(theStream, aCR->Location());
            }
          }
        }
        theStream << (Standard_Byte)0;
      }
      break;
      case TopAbs_FACE: {

        Handle(BRep_TFace) aTF = Handle(BRep_TFace)::DownCast(aShape.TShape());
        const TopoDS_Face& aF  = TopoDS::Face(aShape);

        // Write the surface geometry
        theStream << BRep_Tool::NaturalRestriction(aF) << aTF->Tolerance();
        WriteSurface(theStream, aTF->Surface());
        WriteLocation(theStream, aTF->Location());

        if (IsWithTriangles() || aTF->Surface().IsNull())
        {
          if (!(aTF->Triangulation()).IsNull())
          {
            theStream << (Standard_Byte)2;
            WriteTriangulation(theStream,
                               aTF->Triangulation(),
                               aTF->Triangulation()->HasNormals()
                                 && (IsWithNormals() || aTF->Surface().IsNull()));
          }
          else
            theStream << (Standard_Byte)1;
        }
        else
          theStream << (Standard_Byte)0; // without triangulation
      }
      break;
      default: {
      }
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeWriter::WriteShape" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  theStream.PutBools(aShape.Free(),
                     aShape.Modified(),
                     aShape.Checked(),
                     aShape.Orientable(),
                     aShape.Closed(),
                     aShape.Infinite(),
                     aShape.Convex());
  // process sub-shapes
  for (TopoDS_Iterator aSub(aShape, Standard_False, Standard_False); aSub.More(); aSub.Next())
    WriteShape(theStream, aSub.Value());

  theStream << BinTools_ObjectType_EndShape;
}

//=================================================================================================

void BinTools_ShapeWriter::WriteLocation(BinTools_OStream&      theStream,
                                         const TopLoc_Location& theLocation)
{
  if (theLocation.IsIdentity())
  {
    theStream << BinTools_ObjectType_EmptyLocation;
    return;
  }
  const uint64_t* aLoc = myLocationPos.Seek(theLocation);
  if (aLoc)
  {
    theStream.WriteReference(*aLoc);
    return;
  }
  uint64_t aNewLoc = theStream.Position();
  try
  {
    OCC_CATCH_SIGNALS
    TopLoc_Location  aL2        = theLocation.NextLocation();
    Standard_Boolean isSimple   = aL2.IsIdentity();
    Standard_Integer aPower     = theLocation.FirstPower();
    TopLoc_Location  aL1        = theLocation.FirstDatum();
    Standard_Boolean elementary = (isSimple && aPower == 1);
    if (elementary)
    {
      theStream << BinTools_ObjectType_SimpleLocation << theLocation.Transformation();
    }
    else
    {
      theStream << BinTools_ObjectType_Location;
      WriteLocation(theStream, aL1);
      theStream << aPower;
      while (!aL2.IsIdentity())
      {
        aL1    = aL2.FirstDatum();
        aPower = aL2.FirstPower();
        aL2    = aL2.NextLocation();
        WriteLocation(theStream, aL1);
        theStream << aPower;
      }
      theStream << BinTools_ObjectType_LocationEnd;
    }
    myLocationPos.Bind(theLocation, aNewLoc);
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeWriter::WriteLocation" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=================================================================================================

void BinTools_ShapeWriter::WriteCurve(BinTools_OStream&         theStream,
                                      const Handle(Geom_Curve)& theCurve)
{
  if (theCurve.IsNull())
  {
    theStream << BinTools_ObjectType_EmptyCurve;
    return;
  }
  const uint64_t* aCurve = myCurvePos.Seek(theCurve);
  if (aCurve)
  {
    theStream.WriteReference(*aCurve);
    return;
  }
  myCurvePos.Bind(theCurve, theStream.Position());
  theStream << BinTools_ObjectType_Curve;
  BinTools_CurveSet::WriteCurve(theCurve, theStream);
}

//=================================================================================================

void BinTools_ShapeWriter::WriteCurve(BinTools_OStream&           theStream,
                                      const Handle(Geom2d_Curve)& theCurve)
{
  if (theCurve.IsNull())
  {
    theStream << BinTools_ObjectType_EmptyCurve2d;
    return;
  }
  const uint64_t* aCurve = myCurve2dPos.Seek(theCurve);
  if (aCurve)
  {
    theStream.WriteReference(*aCurve);
    return;
  }
  myCurve2dPos.Bind(theCurve, theStream.Position());
  theStream << BinTools_ObjectType_Curve2d;
  BinTools_Curve2dSet::WriteCurve2d(theCurve, theStream);
}

//=================================================================================================

void BinTools_ShapeWriter::WriteSurface(BinTools_OStream&           theStream,
                                        const Handle(Geom_Surface)& theSurface)
{
  if (theSurface.IsNull())
  {
    theStream << BinTools_ObjectType_EmptySurface;
    return;
  }
  const uint64_t* aSurface = mySurfacePos.Seek(theSurface);
  if (aSurface)
  {
    theStream.WriteReference(*aSurface);
    return;
  }
  mySurfacePos.Bind(theSurface, theStream.Position());
  theStream << BinTools_ObjectType_Surface;
  BinTools_SurfaceSet::WriteSurface(theSurface, theStream);
}

//=================================================================================================

void BinTools_ShapeWriter::WritePolygon(BinTools_OStream&             theStream,
                                        const Handle(Poly_Polygon3D)& thePolygon)
{
  if (thePolygon.IsNull())
  {
    theStream << BinTools_ObjectType_EmptyPolygon3d;
    return;
  }
  const uint64_t* aPolygon = myPolygon3dPos.Seek(thePolygon);
  if (aPolygon)
  {
    theStream.WriteReference(*aPolygon);
    return;
  }
  myPolygon3dPos.Bind(thePolygon, theStream.Position());
  theStream << BinTools_ObjectType_Polygon3d;

  const Standard_Integer aNbNodes = thePolygon->NbNodes();
  theStream << aNbNodes << thePolygon->HasParameters() << thePolygon->Deflection();
  const TColgp_Array1OfPnt& aNodes = thePolygon->Nodes();
  for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
    theStream << aNodes.Value(aNodeIter);
  if (thePolygon->HasParameters())
  {
    const TColStd_Array1OfReal& aParam = thePolygon->Parameters();
    for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      theStream << aParam.Value(aNodeIter);
  }
}

//=================================================================================================

void BinTools_ShapeWriter::WritePolygon(BinTools_OStream&                          theStream,
                                        const Handle(Poly_PolygonOnTriangulation)& thePolygon)
{
  if (thePolygon.IsNull())
  {
    theStream << BinTools_ObjectType_EmptyPolygonOnTriangulation;
    return;
  }
  const uint64_t* aPolygon = myPolygonPos.Seek(thePolygon);
  if (aPolygon)
  {
    theStream.WriteReference(*aPolygon);
    return;
  }
  myPolygonPos.Bind(thePolygon, theStream.Position());
  theStream << BinTools_ObjectType_PolygonOnTriangulation;

  const TColStd_Array1OfInteger& aNodes = thePolygon->Nodes();
  theStream << aNodes.Length();
  for (Standard_Integer aNodeIter = 1; aNodeIter <= aNodes.Length(); ++aNodeIter)
    theStream << aNodes.Value(aNodeIter);
  theStream << thePolygon->Deflection();
  if (const Handle(TColStd_HArray1OfReal)& aParam = thePolygon->Parameters())
  {
    theStream << Standard_True;
    for (Standard_Integer aNodeIter = 1; aNodeIter <= aParam->Length(); ++aNodeIter)
      theStream << aParam->Value(aNodeIter);
  }
  else
    theStream << Standard_False;
}

void BinTools_ShapeWriter::WriteTriangulation(BinTools_OStream&                 theStream,
                                              const Handle(Poly_Triangulation)& theTriangulation,
                                              const Standard_Boolean theNeedToWriteNormals)
{
  if (theTriangulation.IsNull())
  {
    theStream << BinTools_ObjectType_EmptyTriangulation;
    return;
  }
  const uint64_t* aTriangulation = myTriangulationPos.Seek(theTriangulation);
  if (aTriangulation)
  {
    theStream.WriteReference(*aTriangulation);
    return;
  }
  myTriangulationPos.Bind(theTriangulation, theStream.Position());
  theStream << BinTools_ObjectType_Triangulation;

  const Standard_Integer aNbNodes     = theTriangulation->NbNodes();
  const Standard_Integer aNbTriangles = theTriangulation->NbTriangles();
  theStream << aNbNodes << aNbTriangles << theTriangulation->HasUVNodes();
  theStream << theNeedToWriteNormals << theTriangulation->Deflection();
  // write the 3d nodes
  for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
    theStream << theTriangulation->Node(aNodeIter);
  // theStream.write ((char*)(theTriangulation->InternalNodes().value(0)) , sizeof (gp_Pnt) *
  // aNbNodes);

  if (theTriangulation->HasUVNodes())
  {
    for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      theStream << theTriangulation->UVNode(aNodeIter);
  }
  for (Standard_Integer aTriIter = 1; aTriIter <= aNbTriangles; ++aTriIter)
    theStream << theTriangulation->Triangle(aTriIter);

  if (theNeedToWriteNormals)
  {
    gp_Vec3f aNormal;
    for (Standard_Integer aNormalIter = 1; aNormalIter <= aNbNodes; ++aNormalIter)
    {
      theTriangulation->Normal(aNormalIter, aNormal);
      theStream << aNormal;
    }
  }
}
