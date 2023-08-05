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

#include <BinTools_ShapeReader.hxx>

#include <BinTools_Curve2dSet.hxx>
#include <BinTools_CurveSet.hxx>
#include <BinTools_SurfaceSet.hxx>
#include <BRep_Builder.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : BinTools_ShapeReader
//purpose  : 
//=======================================================================
BinTools_ShapeReader::BinTools_ShapeReader()
{}
  
//=======================================================================
//function : ~BinTools_ShapeReader
//purpose  : 
//=======================================================================
BinTools_ShapeReader::~BinTools_ShapeReader()
{}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BinTools_ShapeReader::Clear()
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

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================
void BinTools_ShapeReader::Read (Standard_IStream& theStream, TopoDS_Shape& theShape)
{
  BinTools_IStream aStream(theStream);
  theShape = ReadShape(aStream);
}

//=======================================================================
//function : ReadShape
//purpose  : 
//=======================================================================
TopoDS_Shape BinTools_ShapeReader::ReadShape (BinTools_IStream& theStream)
{
  TopoDS_Shape aResult;
  uint64_t aPosition = theStream.Position();
  const BinTools_ObjectType& aType = theStream.ReadType();
  if (aType == BinTools_ObjectType_EmptyShape || aType == BinTools_ObjectType_EndShape)
    return aResult;

  if (theStream.IsReference())
  {
    uint64_t aRef = theStream.ReadReference();
    const TopoDS_Shape* aFound = myShapePos.Seek(aRef);
    if (aFound) // the shape is already retrieved, just add location
    {
      aResult = *aFound;
    }
    else
    {
      uint64_t aCurrent = theStream.Position();
      theStream.GoTo (aRef); // go to the referenced position
      aResult = ReadShape (theStream);
      theStream.GoTo (aCurrent); // returns to the current position
    }
    aResult.Location (*ReadLocation (theStream), Standard_False);
    aResult.Orientation (TopAbs_Orientation (theStream.ReadByte()));
    return aResult;
  }
  // read the shape
  TopAbs_ShapeEnum aShapeType = theStream.ShapeType();
  TopAbs_Orientation aShapeOrientation = theStream.ShapeOrientation();
  const TopLoc_Location* aShapeLocation = ReadLocation (theStream);
  Standard_Real aTol;
  static BRep_Builder aBuilder;
  try {
    OCC_CATCH_SIGNALS
      switch (aShapeType) {
      case TopAbs_VERTEX:
      {
        TopoDS_Vertex& aV = TopoDS::Vertex (aResult);
        // Read the point geometry
        theStream >> aTol;
        gp_Pnt aPnt = theStream.ReadPnt();
        aBuilder.MakeVertex (aV, aPnt, aTol);
        Handle(BRep_TVertex) aTV = Handle(BRep_TVertex)::DownCast (aV.TShape());
        BRep_ListOfPointRepresentation& aLpr = aTV->ChangePoints();
        static TopLoc_Location anEmptyLoc;
        while (theStream) {
          Standard_Byte aPrsType = theStream.ReadByte();
          if (aPrsType == 0) // end of the cycle
            break;
          Standard_Real aParam = theStream.ReadReal();
          Handle(BRep_PointRepresentation) aPR;
          switch (aPrsType) {
          case 1:
          {
            Handle(Geom_Curve) aCurve = ReadCurve (theStream);
            if (!aCurve.IsNull())
              aPR = new BRep_PointOnCurve (aParam, aCurve, anEmptyLoc);
            break;
          }
          case 2:
          {
            Handle(Geom2d_Curve) aCurve2d = ReadCurve2d (theStream);
            Handle(Geom_Surface) aSurface = ReadSurface (theStream);
            if (!aCurve2d.IsNull() && aSurface.IsNull())
              aPR = new BRep_PointOnCurveOnSurface (aParam, aCurve2d, aSurface, anEmptyLoc);
            break;
          }
          case 3:
          {
            Standard_Real aParam2 = theStream.ReadReal();
            Handle(Geom_Surface) aSurface = ReadSurface (theStream);
            if (!aSurface.IsNull())
              aPR = new BRep_PointOnSurface (aParam, aParam2, aSurface, anEmptyLoc);
            break;
          }
          default:
          {
            Standard_SStream aMsg;
            aMsg << "BinTools_ShapeReader::Read: UnExpected BRep_PointRepresentation = " << aPrsType << std::endl;
            throw Standard_Failure (aMsg.str().c_str());
          }
          }
          const TopLoc_Location* aPRLoc = ReadLocation (theStream);
          if (!aPR.IsNull())
          {
            aPR->Location (*aPRLoc);
            aLpr.Append (aPR);
          }
        }
        break;
      }
      case TopAbs_EDGE:
      {
        TopoDS_Edge& aE = TopoDS::Edge (aResult);
        aBuilder.MakeEdge(aE);
        // Read the curve geometry 
        theStream >> aTol;
        Standard_Boolean aSameParameter, aSameRange, aDegenerated;
        theStream.ReadBools (aSameParameter, aSameRange, aDegenerated);
        aBuilder.SameParameter (aE, aSameParameter);
        aBuilder.SameRange (aE, aSameRange);
        aBuilder.Degenerated (aE, aDegenerated);
        Standard_Real aFirst, aLast;
        while (theStream) {
          Standard_Byte aPrsType = theStream.ReadByte(); //{0|1|2|3|4|5|6|7}
          if (aPrsType == 0)
            break;
          switch (aPrsType)
          {
          case 1: // -1- Curve 3D
          {
            Handle(Geom_Curve) aCurve = ReadCurve (theStream);
            const TopLoc_Location* aLoc = ReadLocation (theStream);
            theStream >> aFirst;
            theStream >> aLast;
            if (!aCurve.IsNull())
            {
              aBuilder.UpdateEdge (aE, aCurve, *aLoc, aTol);
              aBuilder.Range (aE, aFirst, aLast, Standard_True);
            }
            break;
          }
          case 2: // -2- Curve on surf
          case 3: // -3- Curve on closed surf
          {
            Standard_Boolean aClosed = (aPrsType == 3);
            Handle(Geom2d_Curve) aCurve2d_2, aCurve2d_1 = ReadCurve2d (theStream);
            GeomAbs_Shape aReg = GeomAbs_C0;
            if (aClosed) {
              aCurve2d_2 = ReadCurve2d (theStream);
              aReg = (GeomAbs_Shape)theStream.ReadByte();
            }
            Handle(Geom_Surface) aSurface = ReadSurface (theStream);
            const TopLoc_Location* aLoc = ReadLocation (theStream);
            // range
            theStream >> aFirst;
            theStream >> aLast;
            if (!aCurve2d_1.IsNull() && (!aClosed || !aCurve2d_2.IsNull()) && !aSurface.IsNull())
            {
              if (aClosed)
              {
                aBuilder.UpdateEdge (aE, aCurve2d_1, aCurve2d_2, aSurface, *aLoc, aTol);
                aBuilder.Continuity (aE, aSurface, aSurface, *aLoc, *aLoc, aReg);
              }
              else
                aBuilder.UpdateEdge (aE, aCurve2d_1, aSurface, *aLoc, aTol);
              aBuilder.Range (aE, aSurface, *aLoc, aFirst, aLast);
            }
            break;
          }
          case 4: // -4- Regularity
          {
            GeomAbs_Shape aReg = (GeomAbs_Shape)theStream.ReadByte();
            Handle(Geom_Surface) aSurface1 = ReadSurface (theStream);
            const TopLoc_Location* aLoc1 = ReadLocation (theStream);
            Handle(Geom_Surface) aSurface2 = ReadSurface (theStream);
            const TopLoc_Location* aLoc2 = ReadLocation (theStream);
            if (!aSurface1.IsNull() && !aSurface2.IsNull())
              aBuilder.Continuity (aE, aSurface1, aSurface2, *aLoc1, *aLoc2, aReg);
            break;
          }
          case 5: // -5- Polygon3D                     
          {
            Handle(Poly_Polygon3D) aPolygon = ReadPolygon3d (theStream);
            const TopLoc_Location* aLoc = ReadLocation (theStream);
            aBuilder.UpdateEdge (aE, aPolygon, *aLoc);
            break;
          }
          case 6: // -6- Polygon on triangulation
          case 7: // -7- Polygon on closed triangulation
          {
            Standard_Boolean aClosed = (aPrsType == 7);
            Handle(Poly_PolygonOnTriangulation) aPoly2, aPoly1 = ReadPolygon (theStream);
            if (aClosed)
              aPoly2 = ReadPolygon (theStream);
            Handle(Poly_Triangulation) aTriangulation = ReadTriangulation (theStream);
            const TopLoc_Location* aLoc = ReadLocation (theStream);
            if (aClosed)
              aBuilder.UpdateEdge (aE, aPoly1, aPoly2, aTriangulation, *aLoc);
            else
              aBuilder.UpdateEdge (aE, aPoly1, aTriangulation, *aLoc);
            // range            
            break;
          }
          default:
          {
            Standard_SStream aMsg;
            aMsg << "Unexpected Curve Representation =" << aPrsType << std::endl;
            throw Standard_Failure (aMsg.str().c_str());
          }

          }
        }
        break;
      }
      case TopAbs_WIRE:
        aBuilder.MakeWire (TopoDS::Wire (aResult));
        break;
      case TopAbs_FACE:
      {
        TopoDS_Face& aF = TopoDS::Face (aResult);
        aBuilder.MakeFace (aF);
        Standard_Boolean aNatRes = theStream.ReadBool();
        theStream >> aTol;
        Handle(Geom_Surface) aSurface = ReadSurface (theStream);
        const TopLoc_Location* aLoc = ReadLocation (theStream);
        aBuilder.UpdateFace (aF, aSurface, *aLoc, aTol);
        aBuilder.NaturalRestriction (aF, aNatRes);
        if (theStream.ReadByte() == 2) // triangulation
          aBuilder.UpdateFace (aF, ReadTriangulation (theStream));
        break;
      }
      case TopAbs_SHELL:
        aBuilder.MakeShell (TopoDS::Shell (aResult));
        break;
      case TopAbs_SOLID:
        aBuilder.MakeSolid (TopoDS::Solid (aResult));
        break;
      case TopAbs_COMPSOLID:
        aBuilder.MakeCompSolid (TopoDS::CompSolid (aResult));
        break;
      case TopAbs_COMPOUND:
        aBuilder.MakeCompound (TopoDS::Compound (aResult));
        break;
      default:
      {
        Standard_SStream aMsg;
        aMsg << "Unexpected topology type = " << aShapeType << std::endl;
        throw Standard_Failure (aMsg.str().c_str());
        break;
      }
      }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeReader::Read" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure (aMsg.str().c_str());
  }
  // read flags and subs
  Standard_Boolean aFree, aMod, aChecked, anOrient, aClosed, anInf, aConv;
  theStream.ReadBools (aFree, aMod, aChecked, anOrient, aClosed, anInf, aConv);
  // sub-shapes
  for(TopoDS_Shape aSub = ReadShape (theStream); !aSub.IsNull(); aSub = ReadShape (theStream))
    aBuilder.Add (aResult, aSub);
  aResult.Free (aFree);
  aResult.Modified (aMod);
  aResult.Checked (aChecked);
  aResult.Orientable (anOrient);
  aResult.Closed (aClosed);
  aResult.Infinite (anInf);
  aResult.Convex (aConv);
  myShapePos.Bind (aPosition, aResult);
  aResult.Orientation (aShapeOrientation);
  aResult.Location (*aShapeLocation, Standard_False);
  return aResult;
}

//=======================================================================
//function : ReadLocation
//purpose  : 
//=======================================================================
const TopLoc_Location* BinTools_ShapeReader::ReadLocation (BinTools_IStream& theStream)
{
  static const TopLoc_Location* anEmptyLoc = new TopLoc_Location;

  uint64_t aPosition = theStream.Position();
  const BinTools_ObjectType& aType = theStream.ReadType();
  if (aType == BinTools_ObjectType_EmptyLocation || aType == BinTools_ObjectType_LocationEnd)
    return anEmptyLoc;
  if (theStream.IsReference())
  {
    uint64_t aRef = theStream.ReadReference();
    const TopLoc_Location* aFound = myLocationPos.Seek (aRef);
    if (aFound) // the location is already retrieved
      return aFound;
    uint64_t aCurrent = theStream.Position();
    theStream.GoTo (aRef); // go to the referenced position
    const TopLoc_Location* aResult = ReadLocation (theStream);
    theStream.GoTo (aCurrent); // returns to the current position
    return aResult;
  }
  // read the location directly from the stream
  TopLoc_Location aLoc;
  if (aType == BinTools_ObjectType_SimpleLocation)
  {
    gp_Trsf aTrsf;
    theStream >> aTrsf;
    aLoc = aTrsf;
  }
  else if (aType == BinTools_ObjectType_Location)
  {
    for(const TopLoc_Location* aNextLoc = ReadLocation (theStream); !aNextLoc->IsIdentity();
                               aNextLoc = ReadLocation (theStream))
      aLoc = aNextLoc->Powered (theStream.ReadInteger()) * aLoc;
  }
  myLocationPos.Bind (aPosition, aLoc);
  return myLocationPos.Seek (aPosition);
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================
Handle(Geom_Curve) BinTools_ShapeReader::ReadCurve (BinTools_IStream& theStream)
{
  Handle(Geom_Curve) aResult;
  uint64_t aPosition = theStream.Position();
  theStream.ReadType();
  if (theStream.IsReference())
  { // get by reference
    uint64_t aRef = theStream.ReadReference();
    const Handle(Geom_Curve)* aFound = myCurvePos.Seek (aRef);
    if (aFound) // the location is already retrieved
      return *aFound;
    uint64_t aCurrent = theStream.Position();
    theStream.GoTo (aRef); // go to the referenced position
    aResult = ReadCurve (theStream);
    theStream.GoTo (aCurrent); // returns to the current position
  }
  else if (theStream.LastType() == BinTools_ObjectType_Curve)
  { // read from the stream
    BinTools_CurveSet::ReadCurve (theStream.Stream(), aResult);
    theStream.UpdatePosition();
    myCurvePos.Bind (aPosition, aResult);
  }
  return aResult;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) BinTools_ShapeReader::ReadCurve2d (BinTools_IStream& theStream)
{
  Handle(Geom2d_Curve) aResult;
  uint64_t aPosition = theStream.Position();
  theStream.ReadType();
  if (theStream.IsReference())
  { // get by reference
    uint64_t aRef = theStream.ReadReference();
    const Handle(Geom2d_Curve)* aFound = myCurve2dPos.Seek (aRef);
    if (aFound) // the location is already retrieved
      return *aFound;
    uint64_t aCurrent = theStream.Position();
    theStream.GoTo (aRef); // go to the referenced position
    aResult = ReadCurve2d (theStream);
    theStream.GoTo (aCurrent); // returns to the current position
  }
  else if (theStream.LastType() == BinTools_ObjectType_Curve2d)
  { // read from the stream
    BinTools_Curve2dSet::ReadCurve2d (theStream.Stream(), aResult);
    theStream.UpdatePosition();
    myCurve2dPos.Bind (aPosition, aResult);
  }
  return aResult;
}

//=======================================================================
//function : ReadSurface
//purpose  : 
//=======================================================================
Handle(Geom_Surface) BinTools_ShapeReader::ReadSurface (BinTools_IStream& theStream)
{
  Handle(Geom_Surface) aResult;
  uint64_t aPosition = theStream.Position();
  theStream.ReadType();
  if (theStream.IsReference())
  { // get by reference
    uint64_t aRef = theStream.ReadReference();
    const Handle(Geom_Surface)* aFound = mySurfacePos.Seek (aRef);
    if (aFound) // the location is already retrieved
      return *aFound;
    uint64_t aCurrent = theStream.Position();
    theStream.GoTo (aRef); // go to the referenced position
    aResult = ReadSurface (theStream);
    theStream.GoTo (aCurrent); // returns to the current position
  }
  else if (theStream.LastType() == BinTools_ObjectType_Surface)
  { // read from the stream
    BinTools_SurfaceSet::ReadSurface (theStream.Stream(), aResult);
    theStream.UpdatePosition();
    mySurfacePos.Bind (aPosition, aResult);
  }
  return aResult;
}

//=======================================================================
//function : ReadPolygon3d
//purpose  : 
//=======================================================================
Handle(Poly_Polygon3D) BinTools_ShapeReader::ReadPolygon3d (BinTools_IStream& theStream)
{
  Handle(Poly_Polygon3D) aResult;
  uint64_t aPosition = theStream.Position();
  theStream.ReadType();
  if (theStream.IsReference())
  { // get by reference
    uint64_t aRef = theStream.ReadReference();
    const Handle(Poly_Polygon3D)* aFound = myPolygon3dPos.Seek (aRef);
    if (aFound) // the location is already retrieved
      return *aFound;
    uint64_t aCurrent = theStream.Position();
    theStream.GoTo (aRef); // go to the referenced position
    aResult = ReadPolygon3d (theStream);
    theStream.GoTo (aCurrent); // returns to the current position
  }
  else if (theStream.LastType() == BinTools_ObjectType_Polygon3d)
  { // read from the stream
    Standard_Integer aNbNodes = theStream.ReadInteger();
    Standard_Boolean aHasParameters = theStream.ReadBool();
    aResult = new Poly_Polygon3D (aNbNodes, aHasParameters);
    aResult->Deflection (theStream.ReadReal());
    TColgp_Array1OfPnt& aNodes = aResult->ChangeNodes();
    for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      theStream >> aNodes.ChangeValue (aNodeIter);
    if (aHasParameters)
    {
      TColStd_Array1OfReal& aParam = aResult->ChangeParameters();
      for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
        theStream >> aParam.ChangeValue (aNodeIter);
    }
    myPolygon3dPos.Bind (aPosition, aResult);
  }
  return aResult;
}

//=======================================================================
//function : ReadPolygon
//purpose  : 
//=======================================================================
Handle(Poly_PolygonOnTriangulation) BinTools_ShapeReader::ReadPolygon (BinTools_IStream& theStream)
{
  Handle(Poly_PolygonOnTriangulation) aResult;
  uint64_t aPosition = theStream.Position();
  theStream.ReadType();
  if (theStream.IsReference())
  { // get by reference
    uint64_t aRef = theStream.ReadReference();
    const Handle(Poly_PolygonOnTriangulation)* aFound = myPolygonPos.Seek (aRef);
    if (aFound) // the location is already retrieved
      return *aFound;
    uint64_t aCurrent = theStream.Position();
    theStream.GoTo (aRef); // go to the referenced position
    aResult = ReadPolygon (theStream);
    theStream.GoTo (aCurrent); // returns to the current position
  }
  else if (theStream.LastType() == BinTools_ObjectType_PolygonOnTriangulation)
  { // read from the stream
    Standard_Integer aNbNodes = theStream.ReadInteger();
    aResult = new Poly_PolygonOnTriangulation (aNbNodes, Standard_False);
    for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      aResult->SetNode(aNodeIter, theStream.ReadInteger());
    aResult->Deflection (theStream.ReadReal());
    if (theStream.ReadBool())
    {
      Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal (1, aNbNodes);
      for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
        theStream >> aParams->ChangeValue (aNodeIter);
      aResult->SetParameters (aParams);
    }
    myPolygonPos.Bind (aPosition, aResult);
  }
  return aResult;
}

//=======================================================================
//function : ReadTriangulation
//purpose  : 
//=======================================================================
Handle(Poly_Triangulation) BinTools_ShapeReader::ReadTriangulation (BinTools_IStream& theStream)
{
  Handle(Poly_Triangulation) aResult;
  uint64_t aPosition = theStream.Position();
  const BinTools_ObjectType& aType = theStream.ReadType();
  if (theStream.IsReference())
  { // get by reference
    uint64_t aRef = theStream.ReadReference();
    const Handle(Poly_Triangulation)* aFound = myTriangulationPos.Seek (aRef);
    if (aFound) // the location is already retrieved
      return *aFound;
    uint64_t aCurrent = theStream.Position();
    theStream.GoTo (aRef); // go to the referenced position
    aResult = ReadTriangulation (theStream);
    theStream.GoTo (aCurrent); // returns to the current position
  }
  else if (aType == BinTools_ObjectType_Triangulation)
  { // read from the stream
    Standard_Integer aNbNodes = theStream.ReadInteger();
    Standard_Integer aNbTriangles = theStream.ReadInteger();
    Standard_Boolean aHasUV = theStream.ReadBool();
    Standard_Boolean aHasNormals = theStream.ReadBool();
    aResult = new Poly_Triangulation (aNbNodes, aNbTriangles, aHasUV, aHasNormals);
    aResult->Deflection (theStream.ReadReal());
    for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      aResult->SetNode(aNodeIter, theStream.ReadPnt());
    if (aHasUV)
    {
      gp_Pnt2d anUV;
      for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      {
        theStream >> anUV.ChangeCoord().ChangeCoord (1);
        theStream >> anUV.ChangeCoord().ChangeCoord (2);
        aResult->SetUVNode(aNodeIter, anUV);
      }
    }
    // read the triangles
    Poly_Triangle aTriangle;
    for (Standard_Integer aTriIter = 1; aTriIter <= aNbTriangles; ++aTriIter)
    {
      theStream >> aTriangle.ChangeValue (1);
      theStream >> aTriangle.ChangeValue (2);
      theStream >> aTriangle.ChangeValue (3);
      aResult->SetTriangle(aTriIter, aTriangle);
    }
    if (aHasNormals)
    {
      gp_Vec3f aNormal;
      for (Standard_Integer aNormalIter = 1; aNormalIter <= aNbNodes; ++aNormalIter)
      {
        theStream >> aNormal.x();
        theStream >> aNormal.y();
        theStream >> aNormal.z();
        aResult->SetNormal (aNormalIter, aNormal);
      }
    }

    myTriangulationPos.Bind (aPosition, aResult);
  }
  return aResult;
}
