// Created on: 2016-10-13
// Created by: Alexander MALYSHEV
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2016 OPEN CASCADE SAS
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

// Include self.
#include <BRepOffset_MakeSimpleOffset.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTools_Quilt.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepOffset_SimpleOffset.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_Line.hxx>
#include <GeomFill_Generator.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <NCollection_List.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeFix_Edge.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>


//=============================================================================
//function : BRepOffset_MakeSimpleOffset
//purpose  : Constructor
//=============================================================================
BRepOffset_MakeSimpleOffset::BRepOffset_MakeSimpleOffset()
: myOffsetValue(0.),
  myTolerance(Precision::Confusion()),
  myIsBuildSolid(Standard_False),
  myMaxAngle(0.0),
  myError(BRepOffsetSimple_OK),
  myIsDone(Standard_False)
{
  myReShape = new ShapeBuild_ReShape();
}

//=============================================================================
//function : BRepOffset_MakeSimpleOffset
//purpose  : Constructor
//=============================================================================
BRepOffset_MakeSimpleOffset::BRepOffset_MakeSimpleOffset(const TopoDS_Shape& theInputShape,
                                                         const Standard_Real theOffsetValue)
: myInputShape(theInputShape),
  myOffsetValue(theOffsetValue),
  myTolerance(Precision::Confusion()),
  myIsBuildSolid(Standard_False),
  myMaxAngle(0.0),
  myError(BRepOffsetSimple_OK),
  myIsDone(Standard_False)
{
  myReShape = new ShapeBuild_ReShape();
}

//=============================================================================
//function : Initialize
//purpose  :
//=============================================================================
void BRepOffset_MakeSimpleOffset::Initialize(const TopoDS_Shape& theInputShape,
                                             const Standard_Real theOffsetValue)
{
  myInputShape = theInputShape;
  myOffsetValue = theOffsetValue;
  Clear();
}

//=============================================================================
//function : GetErrorMessage
//purpose  :
//=============================================================================
TCollection_AsciiString BRepOffset_MakeSimpleOffset::GetErrorMessage() const
{
  TCollection_AsciiString anError = "";

  if (myError == BRepOffsetSimple_NullInputShape)
  {
    anError = "Null input shape";
    return anError;
  }
  else if (myError == BRepOffsetSimple_ErrorOffsetComputation)
  {
    anError = "Error during offset construction";
    return anError;
  }
  else if (myError == BRepOffsetSimple_ErrorWallFaceComputation)
  {
    anError = "Error during building wall face";
    return anError;
  }
  else if (myError == BRepOffsetSimple_ErrorInvalidNbShells)
  {
    anError = "Result contains two or more shells";
    return anError;
  }
  else if (myError == BRepOffsetSimple_ErrorNonClosedShell)
  {
    anError = "Result shell is not closed";
    return anError;
  }

  return anError;
}

//=============================================================================
//function : Clear
//purpose  : 
//=============================================================================
void BRepOffset_MakeSimpleOffset::Clear()
{
  myIsDone = Standard_False;
  myError = BRepOffsetSimple_OK;
  myMaxAngle = 0.0;
  myMapVE.Clear();
  myReShape->Clear(); // Clear possible stored modifications.
}

//=============================================================================
//function : GetSafeOffset
//purpose  : 
//=============================================================================
Standard_Real BRepOffset_MakeSimpleOffset::GetSafeOffset(const Standard_Real theExpectedToler)
{
  if (myInputShape.IsNull())
    return 0.0; // Input shape is null.

  // Compute max angle in faces junctions.
  if (myMaxAngle == 0.0) // Non-initialized.
    ComputeMaxAngle();

  Standard_Real aMaxTol = 0.0;
  aMaxTol = BRep_Tool::MaxTolerance(myInputShape, TopAbs_VERTEX);

  const Standard_Real anExpOffset = Max((theExpectedToler - aMaxTol) / (2.0 * myMaxAngle),
                                        0.0); // Minimal distance can't be lower than 0.0.
  return anExpOffset;
}

//=============================================================================
//function : Perform
//purpose  : 
//=============================================================================
void BRepOffset_MakeSimpleOffset::Perform()
{
  // Clear result of previous computations.
  Clear();

  // Check shape existence.
  if (myInputShape.IsNull())
  {
    myError = BRepOffsetSimple_NullInputShape;
    return;
  }

  if (myMaxAngle == 0.0) // Non-initialized.
    ComputeMaxAngle();

  myBuilder.Init(myInputShape);
  Handle(BRepOffset_SimpleOffset) aMapper = new BRepOffset_SimpleOffset(myInputShape, myOffsetValue, myTolerance);
  myBuilder.Perform(aMapper);

  if (!myBuilder.IsDone())
  {
    myError = BRepOffsetSimple_ErrorOffsetComputation;
    return;
  }

  myResShape = myBuilder.ModifiedShape(myInputShape);

  // Fix degeneracy. Degenerated edge should be mapped to the degenerated.
  BRep_Builder aBB;
  TopExp_Explorer anExpSE(myInputShape, TopAbs_EDGE);
  for(; anExpSE.More(); anExpSE.Next())
  {
    const TopoDS_Edge & aCurrEdge = TopoDS::Edge(anExpSE.Current());

    if (!BRep_Tool::Degenerated(aCurrEdge))
      continue;

    const TopoDS_Edge & anEdge = TopoDS::Edge(myBuilder.ModifiedShape(aCurrEdge));
    aBB.Degenerated(anEdge, Standard_True);
  }

  // Restore walls for solid.
  if (myIsBuildSolid && !BuildMissingWalls())
    return;

  myIsDone = Standard_True;
}

//=============================================================================
//function : tgtfaces
//purpose  : check the angle at the border between two squares.
//           Two shares should have a shared front edge.
//=============================================================================
static void tgtfaces(const TopoDS_Edge& Ed,
                     const TopoDS_Face& F1,
                     const TopoDS_Face& F2,
                     const Standard_Boolean couture,
                     Standard_Real& theResAngle)
{
 // Check that pcurves exist on both faces of edge.
  Standard_Real aFirst,aLast;
  Handle(Geom2d_Curve) aCurve;
  aCurve = BRep_Tool::CurveOnSurface(Ed,F1,aFirst,aLast);
  if(aCurve.IsNull())
    return;
  aCurve = BRep_Tool::CurveOnSurface(Ed,F2,aFirst,aLast);
  if(aCurve.IsNull())
    return;

  Standard_Real u;
  TopoDS_Edge E = Ed;
  BRepAdaptor_Surface aBAS1(F1,Standard_False);
  BRepAdaptor_Surface aBAS2(F2,Standard_False);

  Handle(BRepAdaptor_HSurface) HS1 = new BRepAdaptor_HSurface (aBAS1);
  Handle(BRepAdaptor_HSurface) HS2;
  if(couture) HS2 = HS1;
  else HS2 = new BRepAdaptor_HSurface(aBAS2);
  //case when edge lies on the one face
  
  E.Orientation(TopAbs_FORWARD);
  BRepAdaptor_Curve2d C2d1(E, F1);
  if(couture) E.Orientation(TopAbs_REVERSED);
  BRepAdaptor_Curve2d C2d2(E,F2);

  Standard_Boolean rev1 = (F1.Orientation() == TopAbs_REVERSED);
  Standard_Boolean rev2 = (F2.Orientation() == TopAbs_REVERSED);
  Standard_Real f,l,eps;
  BRep_Tool::Range(E,f,l);
  Extrema_LocateExtPC ext;

  eps = (l - f) / 100.0;
  f += eps; // to avoid calculations on  
  l -= eps; // points of pointed squares.
  gp_Pnt2d p;
  gp_Pnt pp1,pp2;//,PP;
  gp_Vec du1, dv1;
  gp_Vec du2, dv2;
  gp_Vec d1,d2;
  Standard_Real norm;

  const Standard_Integer NBPNT = 23;
  for(Standard_Integer i = 0; i <= NBPNT; i++)
  {
    // First suppose that this is sameParameter
    u = f + (l - f) * i / NBPNT;

    // take derivatives of surfaces at the same u, and compute normals
    C2d1.D0(u,p);
    HS1->D1 (p.X(), p.Y(), pp1, du1, dv1);
    d1 = (du1.Crossed(dv1));
    norm = d1.Magnitude(); 
    if (norm > 1.e-12) d1 /= norm;
    else continue; // skip degenerated point
    if(rev1) d1.Reverse();

    C2d2.D0(u,p);
    HS2->D1 (p.X(), p.Y(), pp2, du2, dv2);
    d2 = (du2.Crossed(dv2));
    norm = d2.Magnitude();
    if (norm > 1.e-12) d2 /= norm;
    else continue; // skip degenerated point
    if(rev2) d2.Reverse();

    // Compute angle.
    Standard_Real aCurrentAng = d1.Angle(d2);

    theResAngle = Max(theResAngle, aCurrentAng);
  }
}

//=============================================================================
// function : ComputeMaxAngleOnShape
// purpose  : Code the regularities on all edges of the shape, boundary of 
//            two faces that do not have it.
//=============================================================================
static void ComputeMaxAngleOnShape(const TopoDS_Shape& S,
                                   Standard_Real& theResAngle)
{
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(S,TopAbs_EDGE,TopAbs_FACE,M);
  TopTools_ListIteratorOfListOfShape It;
  TopExp_Explorer Ex;
  TopoDS_Face F1,F2;
  Standard_Boolean found, couture;
  for(Standard_Integer i = 1; i <= M.Extent(); i++)
  {
    TopoDS_Edge E = TopoDS::Edge(M.FindKey(i));
    found = Standard_False; couture = Standard_False;
    F1.Nullify();
    for(It.Initialize(M.FindFromIndex(i));It.More() && !found;It.Next())
    {
      if(F1.IsNull()) { F1 = TopoDS::Face(It.Value()); }
      else
      {
        if(!F1.IsSame(TopoDS::Face(It.Value())))
        {
          found = Standard_True;
          F2 = TopoDS::Face(It.Value());
        }
      }
    }
    if (!found && !F1.IsNull()){//is it a sewing edge?
      TopAbs_Orientation orE = E.Orientation();
      TopoDS_Edge curE;
      for(Ex.Init(F1,TopAbs_EDGE);Ex.More() && !found;Ex.Next()){
        curE= TopoDS::Edge(Ex.Current());
        if(E.IsSame(curE) && orE != curE.Orientation()) 
        {
          found = Standard_True;
          couture = Standard_True;
          F2 = F1;
        }
      }
    }
    if(found)
    {
      if(BRep_Tool::Continuity(E,F1,F2)<=GeomAbs_C0)
      {
        try
        {
          tgtfaces(E, F1, F2, couture, theResAngle);
        }
        catch(Standard_Failure)
        {
        }
      }
    }
  }
}

//=============================================================================
//function : ComputeMaxAngle
//purpose  : Computes max angle in faces junction
//=============================================================================
void BRepOffset_MakeSimpleOffset::ComputeMaxAngle()
{
  ComputeMaxAngleOnShape(myInputShape, myMaxAngle);
}

//=============================================================================
//function : BuildMissingWalls
//purpose  : Builds walls to the result solid.
//=============================================================================
Standard_Boolean BRepOffset_MakeSimpleOffset::BuildMissingWalls()
{
  // Internal list of new faces.
  TopoDS_Compound aNewFaces;
  BRep_Builder aBB;
  aBB.MakeCompound(aNewFaces);

  // Compute outer bounds of original shape.
  ShapeAnalysis_FreeBounds aFB(myInputShape);
  const TopoDS_Compound& aFreeWires = aFB.GetClosedWires();

  // Build linear faces on each edge and its image.
  TopExp_Explorer anExpCW(aFreeWires,TopAbs_WIRE);
  for(; anExpCW.More() ; anExpCW.Next())
  {
    const TopoDS_Wire& aCurWire = TopoDS::Wire(anExpCW.Current());

    // Iterate over outer edges in outer wires.
    TopExp_Explorer anExpWE(aCurWire, TopAbs_EDGE);
    for(; anExpWE.More() ; anExpWE.Next())
    {
      const TopoDS_Edge& aCurEdge = TopoDS::Edge(anExpWE.Current());

      TopoDS_Face aNewFace = BuildWallFace(aCurEdge);

      if (aNewFace.IsNull())
      {
        myError = BRepOffsetSimple_ErrorWallFaceComputation;
        return Standard_False;
      }

      aBB.Add(aNewFaces, aNewFace);
    }
  }

  // Update edges from wall faces.
  ShapeFix_Edge aSFE;
  aSFE.SetContext(myReShape);
  TopExp_Explorer anExpCE(aNewFaces, TopAbs_EDGE);
  for ( ; anExpCE.More() ; anExpCE.Next())
  {
    // Fix same parameter and same range flags.
    const TopoDS_Edge& aCurrEdge = TopoDS::Edge(anExpCE.Current());
    aSFE.FixSameParameter(aCurrEdge);
  }

  // Update result to be compound.
  TopoDS_Compound aResCompound;
  aBB.MakeCompound(aResCompound);

  // Add old faces the result.
  TopExp_Explorer anExpSF(myInputShape, TopAbs_FACE);
  for ( ; anExpSF.More() ; anExpSF.Next())
    aBB.Add(aResCompound, anExpSF.Current());

  // Add new faces the result.
  anExpSF.Init(myResShape, TopAbs_FACE);
  for ( ; anExpSF.More() ; anExpSF.Next())
    aBB.Add(aResCompound, anExpSF.Current());

  // Add wall faces to the result.
  TopExp_Explorer anExpCF(aNewFaces, TopAbs_FACE);
  for ( ; anExpCF.More() ; anExpCF.Next())
  {
    const TopoDS_Face& aF = TopoDS::Face(anExpCF.Current());
    aBB.Add(aResCompound, aF);
  }

  // Apply stored modifications.
  aResCompound = TopoDS::Compound(myReShape->Apply(aResCompound));

  // Create result shell.
  BRepTools_Quilt aQuilt;
  aQuilt.Add(aResCompound);
  TopoDS_Shape aShells = aQuilt.Shells();

  TopExp_Explorer anExpSSh(aShells, TopAbs_SHELL);
  TopoDS_Shell aResShell;
  for ( ; anExpSSh.More() ; anExpSSh.Next() )
  {
    if (!aResShell.IsNull())
    {
      // Shell is not null -> explorer contains two or more shells.
      myError = BRepOffsetSimple_ErrorInvalidNbShells;
      return Standard_False;
    }
    aResShell = TopoDS::Shell(anExpSSh.Current());
  }

  if (!BRep_Tool::IsClosed(aResShell))
  {
    myError = BRepOffsetSimple_ErrorNonClosedShell;
    return Standard_False;
  }

  // Create result solid.
  TopoDS_Solid aResSolid;
  aBB.MakeSolid(aResSolid);
  aBB.Add(aResSolid, aResShell);
  myResShape = aResSolid;

  return Standard_True;
}

//=============================================================================
//function : BuildWallFace
//purpose  :
//=============================================================================
TopoDS_Face BRepOffset_MakeSimpleOffset::BuildWallFace(const TopoDS_Edge& theOrigEdge)
{
  TopoDS_Face aResFace;

  // Get offset edge. offset edge is revered to create correct wire.
  TopoDS_Edge aNewEdge = TopoDS::Edge(myBuilder.ModifiedShape(theOrigEdge));
  aNewEdge.Orientation(TopAbs_REVERSED);

  TopoDS_Vertex aNewV1, aNewV2;
  TopExp::Vertices(aNewEdge, aNewV1, aNewV2);

  // Wire contour is:
  // theOrigEdge (forcible forward) -> wall1 -> aNewEdge (forcible reversed) -> wall2
  // Firstly it is necessary to create copy of original shape with forward direction.
  // This simplifies walls creation.
  TopoDS_Edge anOrigCopy = theOrigEdge;
  anOrigCopy.Orientation(TopAbs_FORWARD);
  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(anOrigCopy, aV1, aV2);

  // To simplify work with map.
  TopoDS_Vertex aForwardV1 = TopoDS::Vertex(aV1.Oriented(TopAbs_FORWARD));
  TopoDS_Vertex aForwardV2 = TopoDS::Vertex(aV2.Oriented(TopAbs_FORWARD));

  // Check existence of edges in stored map: Edge1
  TopoDS_Edge aWall1;
  if (myMapVE.IsBound(aForwardV2))
  {
    // Edge exists - get it from map.
    aWall1 = myMapVE(aForwardV2);
  }
  else
  {
    // Edge does not exist - create it and add to the map.
    BRepLib_MakeEdge aME1(TopoDS::Vertex(aV2.Oriented(TopAbs_FORWARD)), 
                          TopoDS::Vertex(aNewV2.Oriented(TopAbs_REVERSED)));
    if (!aME1.IsDone())
      return aResFace;
    aWall1 = aME1.Edge();

    myMapVE.Bind(aForwardV2, aWall1);
  }

  // Check existence of edges in stored map: Edge2
  TopoDS_Edge aWall2;
  if (myMapVE.IsBound(aForwardV1))
  {
    // Edge exists - get it from map.
    aWall2 = TopoDS::Edge(myMapVE(aForwardV1).Oriented(TopAbs_REVERSED));
  }
  else
  {
    // Edge does not exist - create it and add to the map.
    BRepLib_MakeEdge aME2(TopoDS::Vertex(aV1.Oriented(TopAbs_FORWARD)), 
                          TopoDS::Vertex(aNewV1.Oriented(TopAbs_REVERSED)));
    if (!aME2.IsDone())
      return aResFace;
    aWall2 = aME2.Edge();

    myMapVE.Bind(aForwardV1, aWall2);

    // Orient it in reversed direction.
    aWall2.Orientation(TopAbs_REVERSED);
  }

  BRep_Builder aBB;

  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, anOrigCopy);
  aBB.Add(aWire, aWall1);
  aBB.Add(aWire, aNewEdge);
  aBB.Add(aWire, aWall2);

  // Build 3d curves on wire
  BRepLib::BuildCurves3d( aWire );

  // Try to build using simple planar approach.
  TopoDS_Face aF;
  try
  {
    // Call of face maker is wrapped by try/catch since it generates exceptions sometimes.
    BRepLib_MakeFace aFM(aWire, Standard_True);
    if (aFM.IsDone())
      aF = aFM.Face();
  }
  catch(Standard_Failure)
  {
  }

  if (aF.IsNull()) // Exception in face maker or result is not computed.
  {
    // Build using thrusections.
    Standard_Boolean ToReverse = Standard_False;
    Standard_Real fpar, lpar, fparOE, lparOE;
    Handle(Geom_Curve) EdgeCurve = BRep_Tool::Curve(theOrigEdge, fpar, lpar);
    Handle(Geom_TrimmedCurve) TrEdgeCurve = new Geom_TrimmedCurve( EdgeCurve, fpar, lpar );
    Handle(Geom_Curve) OffsetCurve = BRep_Tool::Curve(aNewEdge, fparOE, lparOE);
    Handle(Geom_TrimmedCurve) TrOffsetCurve = new Geom_TrimmedCurve( OffsetCurve, fparOE, lparOE );

    GeomFill_Generator ThrusecGenerator;
    ThrusecGenerator.AddCurve( TrEdgeCurve );
    ThrusecGenerator.AddCurve( TrOffsetCurve );
    ThrusecGenerator.Perform( Precision::PConfusion() );
    Handle(Geom_Surface) theSurf = ThrusecGenerator.Surface();
    //theSurf = new Geom_SurfaceOfLinearExtrusion( TrOffsetCurve, OffsetDir );
    Standard_Real Uf, Ul, Vf, Vl;
    theSurf->Bounds(Uf, Ul, Vf, Vl);
    TopLoc_Location Loc;
    Handle(Geom2d_Line) EdgeLine2d, OELine2d, aLine2d, aLine2d2;
    EdgeLine2d = new Geom2d_Line(gp_Pnt2d(0., Vf), gp_Dir2d(1., 0.));
    aBB.UpdateEdge(theOrigEdge, EdgeLine2d, theSurf, Loc, Precision::Confusion());
    OELine2d = new Geom2d_Line(gp_Pnt2d(0., Vl), gp_Dir2d(1., 0.));
    aBB.UpdateEdge(aNewEdge, OELine2d, theSurf, Loc, Precision::Confusion());
    Standard_Real UonV1 = (ToReverse)? Ul : Uf;
    Standard_Real UonV2 = (ToReverse)? Uf : Ul;
    aLine2d  = new Geom2d_Line(gp_Pnt2d(UonV2, 0.), gp_Dir2d(0., 1.));
    aLine2d2 = new Geom2d_Line(gp_Pnt2d(UonV1, 0.), gp_Dir2d(0., 1.));
    if (aWall1.IsSame(aWall2))
    {
      aBB.UpdateEdge(aWall1, aLine2d, aLine2d2, theSurf, Loc, Precision::Confusion());
      Handle(Geom_Curve) BSplC34 = theSurf->UIso( Uf );
      aBB.UpdateEdge(aWall1, BSplC34, Precision::Confusion());
      aBB.Range(aWall1, Vf, Vl);
    }
    else
    {
      aBB.SameParameter(aWall1, Standard_False);
      aBB.SameRange(aWall1, Standard_False);
      aBB.SameParameter(aWall2, Standard_False);
      aBB.SameRange(aWall2, Standard_False);
      aBB.UpdateEdge(aWall1, aLine2d,  theSurf, Loc, Precision::Confusion());
      aBB.Range(aWall1, theSurf, Loc, Vf, Vl);
      aBB.UpdateEdge(aWall2, aLine2d2, theSurf, Loc, Precision::Confusion());
      aBB.Range(aWall2, theSurf, Loc, Vf, Vl);
      Handle(Geom_Curve) BSplC3 = theSurf->UIso( UonV2 );
      aBB.UpdateEdge(aWall1, BSplC3, Precision::Confusion());
      aBB.Range(aWall1, Vf, Vl, Standard_True); //only for 3d curve
      Handle(Geom_Curve) BSplC4 = theSurf->UIso( UonV1 );
      aBB.UpdateEdge(aWall2, BSplC4, Precision::Confusion());
      aBB.Range(aWall2, Vf, Vl, Standard_True); //only for 3d curve
    }

    aF = BRepLib_MakeFace(theSurf, aWire);

  }

  return aF;
}

//=============================================================================
//function : Generated
//purpose  :
//=============================================================================
const TopoDS_Shape BRepOffset_MakeSimpleOffset::Generated(const TopoDS_Shape& theShape) const
{
  // Shape generated by modification.
  TopoDS_Shape aRes;
  aRes = myBuilder.ModifiedShape(theShape);

  if (aRes.IsNull())
    return aRes;

  // Shape modifications obtained in scope of shape healing.
  aRes = myReShape->Apply(aRes);

  return aRes;
}

//=============================================================================
//function : Modified
//purpose  :
//=============================================================================
const TopoDS_Shape BRepOffset_MakeSimpleOffset::Modified(const TopoDS_Shape& theShape) const
{
  TopoDS_Shape aRes, anEmptyShape;

  // Get modification status and new shape.
  Standard_Integer aModStatus = myReShape->Status(theShape, aRes);

  if (aModStatus == 0)
    return anEmptyShape; // No modifications are applied to the shape or its sub-shapes.

  return aRes;
}

