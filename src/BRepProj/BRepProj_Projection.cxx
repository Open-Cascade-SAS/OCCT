// Copyright (c) 1998-1999 Matra Datavision
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


#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBndLib.hxx>
#include <BRepFill_Generator.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepProj_Projection.hxx>
#include <BRepSweep_Prism.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : DistanceOut
//purpose  : Compute the minimum distance between input shapes 
//           (using Bounding Boxes of each Shape)
//=======================================================================
static Standard_Real DistanceOut (const TopoDS_Shape& S1, const TopoDS_Shape& S2) 
{
  Bnd_Box BBox1, BBox2;
  BRepBndLib::Add(S1,BBox1);
  BRepBndLib::Add(S2,BBox2);
  return BBox1.Distance(BBox2);
}
  
//=======================================================================
//function : DistanceIn
//purpose  : Compute the maximum distance between input Shapes
//           we compute the maximum dimension of each Bounding Box and then
//           add each other with the minimum distance of shapes.
//=======================================================================

static Standard_Real DistanceIn (const TopoDS_Shape& S1, const TopoDS_Shape& S2) 
{
  Bnd_Box LBBox,SBBox;
  BRepBndLib::Add(S1,SBBox);
  BRepBndLib::Add(S2,LBBox);

  Standard_Real LXmin, LYmin, LZmin, LXmax, LYmax, LZmax, 
                SXmin, SYmin, SZmin, SXmax, SYmax, SZmax; 
  SBBox.Get(SXmin, SYmin, SZmin, 
            SXmax, SYmax, SZmax);
  LBBox.Get(LXmin, LYmin, LZmin, 
            LXmax, LYmax, LZmax);

  //Compute the max distance between input shapes------------//
  gp_XYZ Lmin(LXmin, LYmin, LZmin), 
         Lmax(LXmax, LYmax, LZmax);
  gp_XYZ Smin(SXmin, SYmin, SZmin), 
         Smax(SXmax, SYmax, SZmax);
  Lmax.Subtract(Lmin);
  Smax.Subtract(Smin);
  return Lmax.Modulus() + Smax.Modulus() + DistanceOut(S1, S2);
}

//=======================================================================
//function : BuildSection
//purpose  : Cuts theShape by theTool using BRepAlgoAPI_Section and 
//           stores result as set of connected wires and compound
//=======================================================================

void BRepProj_Projection::BuildSection (const TopoDS_Shape& theShape,
                                        const TopoDS_Shape& theTool)
{
  myIsDone = Standard_False;
  mySection.Nullify();
  myShape.Nullify();
  myItr = 0;

  // if theShape is compound, extract only faces -- section algorithm 
  // may refuse to work if e.g. vertex is present
  TopoDS_Shape aShape;
  if (theShape.ShapeType() == TopAbs_FACE ||
      theShape.ShapeType() == TopAbs_SHELL ||
      theShape.ShapeType() == TopAbs_SOLID ||
      theShape.ShapeType() == TopAbs_COMPSOLID)
    aShape = theShape;
  else if (theShape.ShapeType() == TopAbs_COMPOUND)
  {
    TopoDS_Compound C;
    BRep_Builder B;
    TopExp_Explorer exp (theShape, TopAbs_FACE);
    for (; exp.More(); exp.Next())
    {
      if ( C.IsNull() )
        B.MakeCompound (C);
      B.Add (C, exp.Current());
    }
    aShape = C;
  }
  if ( aShape.IsNull() )
    throw Standard_ConstructionError(__FILE__": target shape has no faces");

  // build section computing p-curves on both shapes to get higher precision
  BRepAlgoAPI_Section aSectionTool(aShape, theTool, Standard_False);
  aSectionTool.Approximation(Standard_True);
  aSectionTool.ComputePCurveOn1(Standard_True);
  aSectionTool.ComputePCurveOn2(Standard_True);
  // Use Oriented Bounding Boxes inside Booleans to speed up calculation of the section
  aSectionTool.SetUseOBB(Standard_True);
  aSectionTool.Build();

  // check for successful work of the section tool
  if (!aSectionTool.IsDone())
    return;

  // get edges of the result
  Handle(TopTools_HSequenceOfShape) anEdges = new TopTools_HSequenceOfShape;
  TopExp_Explorer exp(aSectionTool.Shape(), TopAbs_EDGE);
  for (; exp.More(); exp.Next())
    anEdges->Append (exp.Current());

  // if no edges are found, this means that this section yields no result
  if (anEdges->Length() <= 0) 
    return;

  // connect edges to wires using ShapeAnalysis functionality
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires (anEdges, Precision::Confusion(), 
                                                 Standard_True, mySection);
  myIsDone = (! mySection.IsNull() && mySection->Length() > 0);

  // collect all resulting wires to compound
  if ( myIsDone )
  {
    BRep_Builder B;
    B.MakeCompound (myShape);
    for (Standard_Integer i=1; i <= mySection->Length(); i++)
      B.Add (myShape, mySection->Value(i));

    // initialize iteration (for compatibility with previous versions)
    myItr = 1;
  }
}

//=======================================================================
//function : BRepProj_Projection    
//purpose  : Cylindrical Projection
//=======================================================================

BRepProj_Projection::BRepProj_Projection(const TopoDS_Shape& Wire,
                                         const TopoDS_Shape& Shape,
                                         const gp_Dir& D)
: myIsDone(Standard_False), myItr(0) 
{
  // Check the input
  Standard_NullObject_Raise_if((Wire.IsNull() || Shape.IsNull()),__FILE__": null input shape");
  if (Wire.ShapeType() != TopAbs_EDGE && 
      Wire.ShapeType() != TopAbs_WIRE ) 
    throw Standard_ConstructionError(__FILE__": projected shape is neither wire nor edge");

  // compute the "length" of the cylindrical surface to build
  Standard_Real mdis = DistanceIn(Wire, Shape);
  gp_Vec Vsup (D.XYZ() * 2 * mdis);
  gp_Vec Vinf (D.XYZ() * -mdis);

  // move the base of the cylindrical surface by translating it by -mdis
  gp_Trsf T;
  T.SetTranslation(Vinf);
  // Note: it is necessary to create copy of wire to avoid adding new pcurves into it
  Handle(BRepTools_TrsfModification) Trsf = new BRepTools_TrsfModification(T);
  BRepTools_Modifier Modif (Wire, Trsf);
  TopoDS_Shape WireBase = Modif.ModifiedShape(Wire);

  // Creation of a cylindrical surface
  BRepSweep_Prism CylSurf (WireBase, Vsup, Standard_False);

  // Perform section
  BuildSection (Shape, CylSurf.Shape());
}

//=======================================================================
//function : BRepProj_Projection
//purpose  : Conical projection
//=======================================================================

BRepProj_Projection::BRepProj_Projection (const TopoDS_Shape& Wire,
                                          const TopoDS_Shape& Shape,
                                          const gp_Pnt& P)
: myIsDone(Standard_False), myItr(0)
{
  // Check the input
  Standard_NullObject_Raise_if((Wire.IsNull() || Shape.IsNull()),__FILE__": null input shape");
  if (Wire.ShapeType() != TopAbs_EDGE && 
      Wire.ShapeType() != TopAbs_WIRE ) 
    throw Standard_ConstructionError(__FILE__": projected shape is neither wire nor edge");

  // if Wire is only an edge, transform it into a Wire
  TopoDS_Wire aWire;
  if (Wire.ShapeType() == TopAbs_EDGE) 
  {
    BRep_Builder BB;
    BB.MakeWire(aWire);
    BB.Add(aWire, Wire);
  }
  else 
    aWire = TopoDS::Wire(Wire);
  
  // compute the "length" of the conical surface to build
  Standard_Real mdis = DistanceIn(Wire, Shape);

  // Initialize iterator to get first sub-shape of Wire
  TopExp_Explorer ExpWire; 
  ExpWire.Init (aWire, TopAbs_VERTEX);
  
  // get the first Point of the first sub-shape os the Wire
  gp_Pnt PC = BRep_Tool::Pnt(TopoDS::Vertex(ExpWire.Current()));
  
  // compute the ratio of the scale transformation
  Standard_Real Scale = PC.Distance(P);
  if ( Abs (Scale) < Precision::Confusion() ) 
    throw Standard_ConstructionError("Projection");
  Scale = 1. + mdis / Scale;
  
  // move the base of the conical surface by scaling it with ratio Scale
  gp_Trsf T;
  T.SetScale(P, Scale);
  Handle(BRepTools_TrsfModification) Tsca = new BRepTools_TrsfModification(T);
  BRepTools_Modifier ModifScale(aWire,Tsca);
  TopoDS_Shape ShapeGen1 = ModifScale.ModifiedShape(aWire);

  TopoDS_Vertex aVertex = BRepLib_MakeVertex(P);
  TopoDS_Edge DegEdge;
  BRep_Builder BB;
  BB.MakeEdge( DegEdge );
  BB.Add( DegEdge, aVertex.Oriented(TopAbs_FORWARD) );
  BB.Add( DegEdge, aVertex.Oriented(TopAbs_REVERSED) );
  BB.Degenerated( DegEdge, Standard_True );

  TopoDS_Wire DegWire;
  BB.MakeWire( DegWire );
  BB.Add( DegWire, DegEdge );
  DegWire.Closed( Standard_True );

  // Build the Ruled surface based shape
  BRepFill_Generator RuledSurf;
  RuledSurf.AddWire(DegWire);
  RuledSurf.AddWire(TopoDS::Wire(ShapeGen1));
  RuledSurf.Perform();
  TopoDS_Shell SurfShell = RuledSurf.Shell();

  // Perform section
  BuildSection (Shape, SurfShell);
}
