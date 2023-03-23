// Created on: 2004-05-11
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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


#include <BinTools.hxx>
#include <BinTools_Curve2dSet.hxx>
#include <BinTools_ShapeSet.hxx>
#include <BinTools_SurfaceSet.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepTools.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <Message_ProgressRange.hxx>

#include <string.h>

//=======================================================================
//function : BinTools_ShapeSet
//purpose  :
//=======================================================================
BinTools_ShapeSet::BinTools_ShapeSet ()
  : BinTools_ShapeSetBase ()
{}

//=======================================================================
//function : ~BinTools_ShapeSet
//purpose  : 
//=======================================================================

BinTools_ShapeSet::~BinTools_ShapeSet()
{}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::Clear()
{
  mySurfaces.Clear();
  myCurves.Clear();
  myCurves2d.Clear();
  myPolygons3D.Clear();
  myPolygons2D.Clear();
  myNodes.Clear();
  myTriangulations.Clear();
  myShapes.Clear();
  myLocations.Clear();
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Integer BinTools_ShapeSet::Add (const TopoDS_Shape& theShape)
{
  if (theShape.IsNull()) return 0;
  myLocations.Add(theShape.Location());
  TopoDS_Shape aS2 = theShape;
  aS2.Location (TopLoc_Location());
  Standard_Integer anIndex = myShapes.FindIndex (aS2);
  if (anIndex == 0) {
    AddShape  (aS2);
    for (TopoDS_Iterator its (aS2, Standard_False, Standard_False); its.More(); its.Next())
      Add (its.Value());
    anIndex = myShapes.Add (aS2);
  }
  return anIndex;
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BinTools_ShapeSet::Shape (const Standard_Integer theIndx)
{
  return myShapes (theIndx);
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer BinTools_ShapeSet::Index (const TopoDS_Shape& theShape) const
{
  return myShapes.FindIndex (theShape);
}

//=======================================================================
//function : Locations
//purpose  : 
//=======================================================================

const BinTools_LocationSet&  BinTools_ShapeSet::Locations() const 
{
  return myLocations;
}


//=======================================================================
//function : ChangeLocations
//purpose  : 
//=======================================================================

BinTools_LocationSet&  BinTools_ShapeSet::ChangeLocations()
{
  return myLocations;
}

//=======================================================================
//function : AddGeometry
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::AddShape (const TopoDS_Shape& S)
{
  // Add the geometry
  
  if (S.ShapeType() == TopAbs_VERTEX) {
    
    Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(S.TShape());
    BRep_ListIteratorOfListOfPointRepresentation itrp(TV->Points());
    
    while (itrp.More()) {
      const Handle(BRep_PointRepresentation)& PR = itrp.Value();

      if (PR->IsPointOnCurve()) {
        myCurves.Add(PR->Curve());
      }

      else if (PR->IsPointOnCurveOnSurface()) {
        myCurves2d.Add(PR->PCurve());
        mySurfaces.Add(PR->Surface());
      }
      
      else if (PR->IsPointOnSurface()) {
        mySurfaces.Add(PR->Surface());
      }

      ChangeLocations().Add(PR->Location());
      itrp.Next();
    }

  }
  else if (S.ShapeType() == TopAbs_EDGE) {

    // Add the curve geometry
    Handle(BRep_TEdge) TE = Handle(BRep_TEdge)::DownCast(S.TShape());
    BRep_ListIteratorOfListOfCurveRepresentation itrc(TE->Curves());

    while (itrc.More()) {
      const Handle(BRep_CurveRepresentation)& CR = itrc.Value();
      if (CR->IsCurve3D()) {
        if (!CR->Curve3D().IsNull()) {
          myCurves.Add(CR->Curve3D());
          ChangeLocations().Add(CR->Location());
        }
      }
      else if (CR->IsCurveOnSurface()) {
        mySurfaces.Add(CR->Surface());
        myCurves2d.Add(CR->PCurve());
        ChangeLocations().Add(CR->Location());
        if (CR->IsCurveOnClosedSurface())
          myCurves2d.Add(CR->PCurve2());
      }
      else if (CR->IsRegularity()) {
        mySurfaces.Add(CR->Surface());
        ChangeLocations().Add(CR->Location());
        mySurfaces.Add(CR->Surface2());
        ChangeLocations().Add(CR->Location2());
      }
      else if (IsWithTriangles()) { 
        if (CR->IsPolygon3D()) {
          if (!CR->Polygon3D().IsNull()) {
            myPolygons3D.Add(CR->Polygon3D());
            ChangeLocations().Add(CR->Location());
          }
        }
        else if (CR->IsPolygonOnTriangulation()) {
          // NCollection_IndexedDataMap::Add() function use is correct because
          // Bin(Brep)Tools_ShapeSet::AddGeometry() is called from Bin(Brep)Tools_ShapeSet::Add()
          // that processes shapes recursively from complex to elementary ones.
          // As a result, the TopAbs_FACE's will be processed earlier than the TopAbs_EDGE's.
          myTriangulations.Add(CR->Triangulation(), Standard_False); // edge triangulation does not need normals
          myNodes.Add(CR->PolygonOnTriangulation());
          ChangeLocations().Add(CR->Location());
          if (CR->IsPolygonOnClosedTriangulation())
            myNodes.Add(CR->PolygonOnTriangulation2());
        }
        else if (CR->IsPolygonOnSurface()) {
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

  else if (S.ShapeType() == TopAbs_FACE) {

    // Add the surface geometry
    Standard_Boolean needNormals (IsWithNormals());
    Handle(BRep_TFace) TF = Handle(BRep_TFace)::DownCast(S.TShape());
    if (!TF->Surface().IsNull())
    {
      mySurfaces.Add(TF->Surface());
    }
    else
    {
      needNormals = Standard_True;
    }
    if (IsWithTriangles() || TF->Surface().IsNull())
    {
      Handle(Poly_Triangulation) Tr = TF->Triangulation();
      if (!Tr.IsNull()) myTriangulations.Add(Tr, needNormals);
    }

    ChangeLocations().Add(TF->Location());
  }
}

//=======================================================================
//function : WriteGeometry
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::WriteGeometry (Standard_OStream& OS,
                                        const Message_ProgressRange& theRange)const
{
  Message_ProgressScope aPS(theRange, "Writing geometry", 6);
  myCurves2d.Write(OS, aPS.Next());
  if (!aPS.More())
    return;
  myCurves.Write(OS, aPS.Next());
  if (!aPS.More())
    return;
  WritePolygon3D(OS, aPS.Next());
  if (!aPS.More())
    return;
  WritePolygonOnTriangulation(OS, aPS.Next());
  if (!aPS.More())
    return;
  mySurfaces.Write(OS, aPS.Next());
  if (!aPS.More())
    return;
  WriteTriangulation(OS, aPS.Next());
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::Write (Standard_OStream& OS,
                                const Message_ProgressRange& theRange)
{
  // write the copyright
  OS << "\n" << THE_ASCII_VERSIONS[FormatNb()] << "\n";

  //-----------------------------------------
  // write the locations
  //-----------------------------------------

  myLocations.Write(OS);

  //-----------------------------------------
  // write the geometry
  //-----------------------------------------

  Message_ProgressScope aPS(theRange, "Writing geometry", 2);

  WriteGeometry(OS, aPS.Next());
  if (!aPS.More())
    return;

  //-----------------------------------------
  // write the shapes
  //-----------------------------------------

  Standard_Integer i, nbShapes = myShapes.Extent();
  Message_ProgressScope aPSinner(aPS.Next(), "Writing shapes", nbShapes);
  OS << "\nTShapes " << nbShapes << "\n";
  
  // subshapes are written first
  for (i = 1; i <= nbShapes && aPSinner.More(); i++, aPSinner.Next()) {

    const TopoDS_Shape& S = myShapes (i);
    
    // Type
    OS << (Standard_Byte)S.ShapeType();

    // Geometry
    WriteShape (S, OS);

    // Flags
    BinTools::PutBool(OS, S.Free()? 1:0);
    BinTools::PutBool(OS, S.Modified()? 1:0);
    BinTools::PutBool(OS, S.Checked()? 1:0);
    BinTools::PutBool(OS, S.Orientable()? 1:0);
    BinTools::PutBool(OS, S.Closed()? 1:0);
    BinTools::PutBool(OS, S.Infinite()? 1:0);
    BinTools::PutBool(OS, S.Convex()? 1:0);

    // sub-shapes

    TopoDS_Iterator its(S,Standard_False,Standard_False);
    while (its.More()) {
      Write(its.Value(),OS);
      its.Next();
    }
    Write(TopoDS_Shape(),OS); // Null shape to end the list
  }
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================
void  BinTools_ShapeSet::Read (Standard_IStream& IS,
                               const Message_ProgressRange& theRange)
{
  Clear();

  // Check the version
  char vers[101];
  Standard_Boolean anIsSetFormat = Standard_False;
  do {
    IS.getline(vers,100,'\n');
    // BUC60769 PTV 18.10.2000: remove possible '\r' at the end of the line
    
    Standard_Size lv = strlen(vers);
    if (lv > 0) {
      for (lv--; lv > 0 && (vers[lv] == '\r' || vers[lv] == '\n'); lv--)
        vers[lv] = '\0';
    }

    for (Standard_Integer i = BinTools_FormatVersion_LOWER;
         i <= BinTools_FormatVersion_UPPER; ++i)
    {
      if (!strcmp(vers, THE_ASCII_VERSIONS[i]))
      {
        SetFormatNb(i);
        anIsSetFormat = Standard_True;
        break;
      }
    }
    if (anIsSetFormat)
    {
      break;
    }
  } 
  while ( ! IS.fail());
  if (IS.fail()) {
    std::cout << "BinTools_ShapeSet::Read: File was not written with this version of the topology" << std::endl;
    return;
  }

  //-----------------------------------------
  // read the locations
  //-----------------------------------------
  myLocations.Read(IS);
  //-----------------------------------------
  // read the geometry
  //-----------------------------------------
  Message_ProgressScope aPSouter(theRange, "Reading", 2);
  ReadGeometry(IS, aPSouter.Next());
  if (!aPSouter.More())
    return;
  //-----------------------------------------
  // read the shapes
  //-----------------------------------------

  char buffer[255];
  IS >> buffer;
  if (IS.fail() || strcmp(buffer,"TShapes")) {
    Standard_SStream aMsg;
    aMsg << "BinTools_ShapeSet::Read: Not a TShape table"<<std::endl;
    throw Standard_Failure(aMsg.str().c_str());
    return;
  }
  Standard_Integer nbShapes = 0;
  IS >> nbShapes;
  IS.get();//remove lf 
  Message_ProgressScope aPSinner(aPSouter.Next(), "Reading Shapes", nbShapes);
  for (int i = 1; i <= nbShapes && aPSinner.More(); i++, aPSinner.Next())
  {
    TopoDS_Shape S;
    TopAbs_ShapeEnum T = (TopAbs_ShapeEnum)IS.get();
    ReadShape (T, IS, S);
    ReadFlagsAndSubs (S, T, IS, nbShapes);
    myShapes.Add (S);
  }
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::Write (const TopoDS_Shape& S, 
                                Standard_OStream& OS)
{
  if (S.IsNull()) 

    OS << '*';
  else {    
// {TopAbs_FORWARD, TopAbs_REVERSED, TopAbs_INTERNAL, TopAbs_EXTERNAL} 
    OS << (Standard_Byte) S.Orientation();
    BinTools::PutInteger (OS, myShapes.Extent() - myShapes.FindIndex (S.Located (TopLoc_Location())) + 1);
    BinTools::PutInteger (OS, Locations().Index (S.Location()));
  }    
}

//=======================================================================
//function : ReadFlagsAndSubs
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::ReadFlagsAndSubs(TopoDS_Shape& S, const TopAbs_ShapeEnum T,
  Standard_IStream& IS, const Standard_Integer nbShapes)
{
  // Set the flags
  Standard_Boolean aFree, aMod, aChecked, anOrient, aClosed, anInf, aConv;
  BinTools::GetBool(IS, aFree);
  BinTools::GetBool(IS, aMod);
  BinTools::GetBool(IS, aChecked);
  BinTools::GetBool(IS, anOrient);
  BinTools::GetBool(IS, aClosed);
  BinTools::GetBool(IS, anInf);
  BinTools::GetBool(IS, aConv);

  // sub-shapes
  TopoDS_Shape SS;
  do {
    ReadSubs(SS, IS, nbShapes);
    if (!SS.IsNull())
      AddShapes(S, SS);
  } while (!SS.IsNull());

  S.Free(aFree);
  S.Modified(aMod);
  if (FormatNb() == BinTools_FormatVersion_VERSION_1)
  {
    aChecked = false; // force check at reading
  }
  S.Checked (aChecked);
  S.Orientable (anOrient);
  S.Closed (aClosed);
  S.Infinite (anInf);
  S.Convex (aConv);
  // check

  if (FormatNb() == BinTools_FormatVersion_VERSION_1)
    if (T == TopAbs_FACE) {
      const TopoDS_Face& F = TopoDS::Face(S);
      BRepTools::Update(F);
    }
}

//=======================================================================
//function : ReadSubs
//purpose  : 
//=======================================================================
void BinTools_ShapeSet::ReadSubs(TopoDS_Shape& S, Standard_IStream& IS,
                                 const Standard_Integer nbshapes)
{
  Standard_Character aChar = '\0';
  IS >> aChar;
  if (aChar == '*')
    S = TopoDS_Shape();
  else {
    TopAbs_Orientation anOrient;
    anOrient = (TopAbs_Orientation)aChar;
    Standard_Integer anIndx;
    BinTools::GetInteger(IS, anIndx);
    S = Shape (nbshapes - anIndx + 1);
    S.Orientation(anOrient);

    Standard_Integer l;
    BinTools::GetInteger(IS, l);
    S.Location(myLocations.Location(l), Standard_False);
  }
}

//=======================================================================
//function : ReadGeometry
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::ReadGeometry (Standard_IStream& IS,
                                      const Message_ProgressRange& theRange)
{

  Message_ProgressScope aPS(theRange, "Reading geometry", 6);
  myCurves2d.Read(IS, aPS.Next());
  if (!aPS.More())
    return;

  myCurves.Read(IS, aPS.Next());
  if (!aPS.More())
    return;

  ReadPolygon3D(IS, aPS.Next());
  if (!aPS.More())
    return;

  ReadPolygonOnTriangulation(IS, aPS.Next());
  if (!aPS.More())
    return;

  mySurfaces.Read(IS, aPS.Next());
  if (!aPS.More())
    return;

  ReadTriangulation(IS, aPS.Next());
}

//=======================================================================
//function : WriteGeometry
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::WriteShape (const TopoDS_Shape& S, 
                                    Standard_OStream&   OS) const 
{
// Write the geometry
  try {
    OCC_CATCH_SIGNALS
    if (S.ShapeType() == TopAbs_VERTEX) {
    
// Write the point geometry
      TopoDS_Vertex V = TopoDS::Vertex(S);
      BinTools::PutReal(OS, BRep_Tool::Tolerance(V));
      gp_Pnt p = BRep_Tool::Pnt(V);
      OS << p;
#ifdef OCCT_DEBUG_POS
      std::streamoff aPos;
#endif
      Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(S.TShape());
      BRep_ListIteratorOfListOfPointRepresentation itrp(TV->Points());
      while (itrp.More()) {
	const Handle(BRep_PointRepresentation)& PR = itrp.Value();
//	BinTools::PutReal(OS, PR->Parameter());
	if (PR->IsPointOnCurve()) {
#ifdef OCCT_DEBUG_POS
	  aPos = OS.tellp();
#endif
	  OS << (Standard_Byte)1; // 1
	  BinTools::PutReal(OS, PR->Parameter());
	  BinTools::PutInteger(OS, myCurves.Index(PR->Curve()));
	}

	else if (PR->IsPointOnCurveOnSurface()) {
#ifdef OCCT_DEBUG_POS
	  aPos = OS.tellp();
#endif
	  OS << (Standard_Byte)2;// 2
	  BinTools::PutReal(OS, PR->Parameter());
	  BinTools::PutInteger(OS, myCurves2d.Index(PR->PCurve()));
	  BinTools::PutInteger(OS, mySurfaces.Index(PR->Surface()));
	}

	else if (PR->IsPointOnSurface()) {
#ifdef OCCT_DEBUG_POS
	  aPos = OS.tellp();
#endif
	  OS << (Standard_Byte)3;// 3
	  BinTools::PutReal(OS, PR->Parameter2());
	  BinTools::PutReal(OS, PR->Parameter());
	  BinTools::PutInteger(OS, mySurfaces.Index(PR->Surface()));
	}
	BinTools::PutInteger(OS, Locations().Index(PR->Location()));
	itrp.Next();
      }
    
//    OS << "0 0\n"; // end representations
      OS.put((Standard_Byte)0);
    }

    else if (S.ShapeType() == TopAbs_EDGE) {

    // Write the curve geometry 

      Handle(BRep_TEdge) TE = Handle(BRep_TEdge)::DownCast(S.TShape());

      BinTools::PutReal(OS, TE->Tolerance());

      Standard_Boolean aVal = (TE->SameParameter()) ? Standard_True : Standard_False;
      BinTools::PutBool(OS, aVal);   
      aVal = (TE->SameRange()) ? Standard_True : Standard_False;
      BinTools::PutBool(OS, aVal);
      aVal = (TE->Degenerated())  ? Standard_True : Standard_False;
      BinTools::PutBool(OS, aVal);
      
      Standard_Real first, last;
      BRep_ListIteratorOfListOfCurveRepresentation itrc = TE->Curves();
      while (itrc.More()) {
	const Handle(BRep_CurveRepresentation)& CR = itrc.Value();
	if (CR->IsCurve3D()) {
	  if (!CR->Curve3D().IsNull()) {
	    Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itrc.Value());
	    GC->Range(first, last);
	    OS << (Standard_Byte)1;//CURVE_3D;
	    BinTools::PutInteger(OS, myCurves.Index(CR->Curve3D()));
	    BinTools::PutInteger(OS, Locations().Index(CR->Location()));
	    BinTools::PutReal(OS, first);
	    BinTools::PutReal(OS, last);
	  }
	}
	else if (CR->IsCurveOnSurface()) {
	  Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itrc.Value());
	  GC->Range(first, last);
	  if (!CR->IsCurveOnClosedSurface())
// -2- Curve on surf
	    OS << (Standard_Byte)2;
	  else
// -3- Curve on closed surf
	    OS << (Standard_Byte)3;
	  BinTools::PutInteger(OS, myCurves2d.Index(CR->PCurve()));
	  if (CR->IsCurveOnClosedSurface()) {//+ int|char
	    BinTools::PutInteger(OS, myCurves2d.Index(CR->PCurve2()));
	    OS << (Standard_Byte)CR->Continuity();
	  }
	  BinTools::PutInteger(OS, mySurfaces.Index(CR->Surface()));
	  BinTools::PutInteger(OS, Locations().Index(CR->Location()));
	  BinTools::PutReal(OS, first);
	  BinTools::PutReal(OS, last);

        // Write UV Points for higher performance
	  if (FormatNb() == BinTools_FormatVersion_VERSION_2
	   || FormatNb() == BinTools_FormatVersion_VERSION_3)
	    {
	      gp_Pnt2d Pf,Pl;
	      if (CR->IsCurveOnClosedSurface()) {
		Handle(BRep_CurveOnClosedSurface) COCS = Handle(BRep_CurveOnClosedSurface)::DownCast(CR);
		COCS->UVPoints2(Pf,Pl);
	      }
	      else {
		Handle(BRep_CurveOnSurface) COS = Handle(BRep_CurveOnSurface)::DownCast(CR);
		COS->UVPoints(Pf,Pl);
	      }
	      BinTools::PutReal(OS, Pf.X());
	      BinTools::PutReal(OS, Pf.Y());
	      BinTools::PutReal(OS, Pl.X());
	      BinTools::PutReal(OS, Pl.Y());
	    }
	}
	else if (CR->IsRegularity()) {
// -4- Regularity
	  OS << (Standard_Byte)4;
	  OS << (Standard_Byte)CR->Continuity();
	  BinTools::PutInteger(OS, mySurfaces.Index(CR->Surface()));
	  BinTools::PutInteger(OS, Locations().Index(CR->Location()));
	  BinTools::PutInteger(OS, mySurfaces.Index(CR->Surface2()));
	  BinTools::PutInteger(OS, Locations().Index(CR->Location2()));
	  
	}

	else if (IsWithTriangles()) { 
	  if (CR->IsPolygon3D()) {
	    Handle(BRep_Polygon3D) GC = Handle(BRep_Polygon3D)::DownCast(itrc.Value());
	    if (!GC->Polygon3D().IsNull()) {
// -5- Polygon3D
	      OS << (Standard_Byte)5;
	      BinTools::PutInteger(OS, myPolygons3D.FindIndex(CR->Polygon3D()));
	      BinTools::PutInteger(OS, Locations().Index(CR->Location())); 
	    }
	  }
	  else if (CR->IsPolygonOnTriangulation()) {
	    Handle(BRep_PolygonOnTriangulation) PT = 
	      Handle(BRep_PolygonOnTriangulation)::DownCast(itrc.Value());
	    if (!CR->IsPolygonOnClosedTriangulation())
// -6- Polygon on triangulation
	      OS << (Standard_Byte)6;
	    else
// -7- Polygon on closed triangulation
	      OS << (Standard_Byte)7;
	    BinTools::PutInteger(OS, myNodes.FindIndex(PT->PolygonOnTriangulation()));
	    
	    if (CR->IsPolygonOnClosedTriangulation()) {
	      BinTools::PutInteger(OS, myNodes.FindIndex(PT->PolygonOnTriangulation2()));
	    }
	    BinTools::PutInteger(OS, myTriangulations.FindIndex(PT->Triangulation()));
	    BinTools::PutInteger(OS, Locations().Index(CR->Location()));
	  }
	}
	
	itrc.Next();
      }
//   OS << "0\n"; // end of the list of representations

      OS << (Standard_Byte)0;
    }
  
    else if (S.ShapeType() == TopAbs_FACE) {

      Handle(BRep_TFace) TF = Handle(BRep_TFace)::DownCast(S.TShape());
      const TopoDS_Face& F = TopoDS::Face(S);

      // Write the surface geometry
      Standard_Boolean aNatRes = BRep_Tool::NaturalRestriction(F);
      BinTools::PutBool (OS, aNatRes);
      BinTools::PutReal (OS, TF->Tolerance());
      BinTools::PutInteger (OS, !TF->Surface().IsNull()
                               ? mySurfaces.Index (TF->Surface())
                               : 0);
      BinTools::PutInteger (OS, Locations().Index (TF->Location()));

      if (IsWithTriangles() || TF->Surface().IsNull())
      {
	if (!(TF->Triangulation()).IsNull()) {
	  OS << (Standard_Byte) 2;
        // Write the triangulation
	  BinTools::PutInteger(OS, myTriangulations.FindIndex(TF->Triangulation())); 
	} else
	  OS << (Standard_Byte) 1;
      } else
	OS << (Standard_Byte) 0;//without triangulation
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::WriteGeometry(S,OS)" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : ReadShape
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::ReadShape (const TopAbs_ShapeEnum T,
                                    Standard_IStream&      IS,
                                    TopoDS_Shape&          S)
{
  // Read the geometry

  Standard_Integer val, c, pc, pc2 = 0, s, s2, l, l2, t, pt, pt2 = 0;
  Standard_Real tol, X, Y, Z, first, last, p1 = 0., p2;
  Standard_Real PfX, PfY, PlX, PlY;
  gp_Pnt2d aPf, aPl;
  Standard_Boolean closed, bval;
  GeomAbs_Shape reg = GeomAbs_C0;
  try {
    OCC_CATCH_SIGNALS
      switch (T) {


        //---------
        // vertex
        //---------

      case TopAbs_VERTEX:
      {
        TopoDS_Vertex& V = TopoDS::Vertex(S);

        // Read the point geometry
        BinTools::GetReal(IS, tol);
        BinTools::GetReal(IS, X);
        BinTools::GetReal(IS, Y);
        BinTools::GetReal(IS, Z);
        gp_Pnt aPnt(X, Y, Z);
        myBuilder.MakeVertex(V, aPnt, tol);
        Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(V.TShape());

        BRep_ListOfPointRepresentation& lpr = TV->ChangePoints();
        TopLoc_Location L;
        do {
          if (FormatNb() == BinTools_FormatVersion_VERSION_3) {
            val = (Standard_Integer)IS.get();//case {0|1|2|3}
            if (val > 0 && val <= 3)
              BinTools::GetReal(IS, p1);
          }
          else {
            std::streampos aPos = IS.tellg();
            BinTools::GetReal(IS, p1);
            val = (Standard_Integer)IS.get();//case {0|1|2|3}
#ifdef OCCT_DEBUG
            std::cout << "\nVal = " << val << std::endl;
#endif	  
            if (val != 1 && val != 2 && val != 3) {
              IS.seekg(aPos);
              val = (Standard_Integer)IS.get();
              if (val > 0 && val <= 3)
                BinTools::GetReal(IS, p1);
            }
          }
          Handle(BRep_PointRepresentation) PR;
          switch (val) {
          case 0:
            break;

          case 1:
          {
            BinTools::GetInteger(IS, c);
            if (myCurves.Curve(c).IsNull())
              break;
            Handle(BRep_PointOnCurve) POC =
              new BRep_PointOnCurve(p1,
                myCurves.Curve(c),
                L);
            PR = POC;
          }
          break;

          case 2:
          {
            BinTools::GetInteger(IS, pc);
            BinTools::GetInteger(IS, s);
            if (myCurves2d.Curve2d(pc).IsNull() ||
              mySurfaces.Surface(s).IsNull())
              break;

            Handle(BRep_PointOnCurveOnSurface) POC =
              new BRep_PointOnCurveOnSurface(p1,
                myCurves2d.Curve2d(pc),
                mySurfaces.Surface(s),
                L);
            PR = POC;
          }
          break;

          case 3:
          {
            BinTools::GetReal(IS, p2);
            BinTools::GetInteger(IS, s);
            if (mySurfaces.Surface(s).IsNull())
              break;

            Handle(BRep_PointOnSurface) POC =
              new BRep_PointOnSurface(p1, p2,
                mySurfaces.Surface(s),
                L);
            PR = POC;
          }
          break;

          default:
          {
            Standard_SStream aMsg;
            aMsg << "BinTools_SurfaceSet::ReadGeometry: UnExpected BRep_PointRepresentation = " << val << std::endl;
            throw Standard_Failure(aMsg.str().c_str());
          }
          }

          if (val > 0) {
            BinTools::GetInteger(IS, l);//Locations index

            if (!PR.IsNull()) {
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
        BinTools::GetReal(IS, tol);
        BinTools::GetBool(IS, bval);
        myBuilder.SameParameter(E, bval);

        BinTools::GetBool(IS, bval);
        myBuilder.SameRange(E, bval);

        BinTools::GetBool(IS, bval);
        myBuilder.Degenerated(E, bval);

        do {
          val = (Standard_Integer)IS.get();//{0|1|2|3|4|5|6|7}
          // -0- no representation
          // -1- Curve 3D
          // -2- Curve on surf
          // -3- Curve on closed surf
          // -4- Regularity
          // -5- Polygon3D
          // -6- Polygon on triangulation
          // -7- Polygon on closed triangulation

          switch (val) {
          case 0:
            break;

          case 1:                               // -1- Curve 3D
            BinTools::GetInteger(IS, c);
            BinTools::GetInteger(IS, l);
            if (!myCurves.Curve(c).IsNull()) {
              myBuilder.UpdateEdge(E, myCurves.Curve(c),
                Locations().Location(l), tol);
            }
            BinTools::GetReal(IS, first);
            BinTools::GetReal(IS, last);
            if (!myCurves.Curve(c).IsNull()) {
              Standard_Boolean Only3d = Standard_True;
              myBuilder.Range(E, first, last, Only3d);
            }
            break;


          case 2: // -2- Curve on surf
          case 3: // -3- Curve on closed surf
            closed = (val == 3);
            BinTools::GetInteger(IS, pc);
            if (closed) {
              BinTools::GetInteger(IS, pc2);
              reg = (GeomAbs_Shape)IS.get();
            }

            // surface, location
            BinTools::GetInteger(IS, s);
            BinTools::GetInteger(IS, l);

            // range
            BinTools::GetReal(IS, first);
            BinTools::GetReal(IS, last);

            // read UV Points // for XML Persistence higher performance
            if (FormatNb() == BinTools_FormatVersion_VERSION_2
             || FormatNb() == BinTools_FormatVersion_VERSION_3)
            {
              BinTools::GetReal(IS, PfX);
              BinTools::GetReal(IS, PfY);
              BinTools::GetReal(IS, PlX);
              BinTools::GetReal(IS, PlY);
              aPf = gp_Pnt2d(PfX, PfY);
              aPl = gp_Pnt2d(PlX, PlY);
            }

            if (myCurves2d.Curve2d(pc).IsNull() ||
              (closed && myCurves2d.Curve2d(pc2).IsNull()) ||
              mySurfaces.Surface(s).IsNull())
              break;

            if (closed) {
              if (FormatNb() == BinTools_FormatVersion_VERSION_2
               || FormatNb() == BinTools_FormatVersion_VERSION_3)
              {
                myBuilder.UpdateEdge(E, myCurves2d.Curve2d(pc),
                  myCurves2d.Curve2d(pc2),
                  mySurfaces.Surface(s),
                  Locations().Location(l), tol,
                  aPf, aPl);
              }
              else
              {
                myBuilder.UpdateEdge(E, myCurves2d.Curve2d(pc),
                  myCurves2d.Curve2d(pc2),
                  mySurfaces.Surface(s),
                  Locations().Location(l), tol);
              }

              myBuilder.Continuity(E,
                mySurfaces.Surface(s),
                mySurfaces.Surface(s),
                Locations().Location(l),
                Locations().Location(l),
                reg);
            }
            else
            {
              if (FormatNb() == BinTools_FormatVersion_VERSION_2
               || FormatNb() == BinTools_FormatVersion_VERSION_3)
              {
                myBuilder.UpdateEdge(E, myCurves2d.Curve2d(pc),
                  mySurfaces.Surface(s),
                  Locations().Location(l), tol,
                  aPf, aPl);
              }
              else
              {
                myBuilder.UpdateEdge(E, myCurves2d.Curve2d(pc),
                  mySurfaces.Surface(s),
                  Locations().Location(l), tol);
              }
            }
            myBuilder.Range(E,
              mySurfaces.Surface(s),
              Locations().Location(l),
              first, last);
            break;

          case 4: // -4- Regularity
            reg = (GeomAbs_Shape)IS.get();
            BinTools::GetInteger(IS, s);
            BinTools::GetInteger(IS, l);
            BinTools::GetInteger(IS, s2);
            BinTools::GetInteger(IS, l2);
            if (mySurfaces.Surface(s).IsNull() ||
              mySurfaces.Surface(s2).IsNull())
              break;
            myBuilder.Continuity(E,
              mySurfaces.Surface(s),
              mySurfaces.Surface(s2),
              Locations().Location(l),
              Locations().Location(l2),
              reg);
            break;

          case 5: // -5- Polygon3D                     
            BinTools::GetInteger(IS, c);
            BinTools::GetInteger(IS, l);
            //??? Bug?  myBuilder.UpdateEdge(E,myPolygons3D(c));
            myBuilder.UpdateEdge(E, myPolygons3D(c), Locations().Location(l));
            break;

          case 6: // -6- Polygon on triangulation
          case 7: // -7- Polygon on closed triangulation
            closed = (val == 7);
            BinTools::GetInteger(IS, pt);
            if (closed)
              BinTools::GetInteger(IS, pt2);

            BinTools::GetInteger(IS, t);
            BinTools::GetInteger(IS, l);
            if (closed)
            {
              myBuilder.UpdateEdge (E, myNodes(pt), myNodes(pt2), myTriangulations.FindKey(t), Locations().Location(l));
            }
            else
            {
              myBuilder.UpdateEdge (E, myNodes(pt), myTriangulations.FindKey(t), Locations().Location(l));
            }
            // range            
            break;
          default:
          {
            Standard_SStream aMsg;
            aMsg << "Unexpected Curve Representation =" << val << std::endl;
            throw Standard_Failure(aMsg.str().c_str());
          }

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

      case TopAbs_FACE:
      {
        // create a face :
        TopoDS_Face& F = TopoDS::Face(S);
        myBuilder.MakeFace(F);
        BinTools::GetBool(IS, bval); //NaturalRestriction flag
        BinTools::GetReal(IS, tol);
        BinTools::GetInteger(IS, s); //surface indx
        BinTools::GetInteger(IS, l); //location indx
        myBuilder.UpdateFace(F,
          s > 0 ? mySurfaces.Surface(s) : Handle(Geom_Surface)(),
          Locations().Location(l),
          tol);
        myBuilder.NaturalRestriction(F, bval);

        Standard_Byte aByte = (Standard_Byte)IS.get();
        // cas triangulation
        if (aByte == 2) {
          BinTools::GetInteger(IS, s);
	  myBuilder.UpdateFace(TopoDS::Face(S), myTriangulations.FindKey(s));
        }
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
      {
        Standard_SStream aMsg;
        aMsg << "Unexpected topology type = " << T << std::endl;
        throw Standard_Failure(aMsg.str().c_str());
        break;
      }
      }
  }
  catch (Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::ReadGeometry(S,OS)" << std::endl;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : AddShapes
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::AddShapes(TopoDS_Shape&       S1, 
                                  const TopoDS_Shape& S2)
{
  myBuilder.Add(S1,S2);
}

//=======================================================================
//function : WritePolygonOnTriangulation
//purpose  :
//=======================================================================
void BinTools_ShapeSet::WritePolygonOnTriangulation
  (Standard_OStream& OS,
   const Message_ProgressRange& theRange) const
{
  const Standard_Integer aNbPol = myNodes.Extent();
  OS << "PolygonOnTriangulations " << aNbPol << "\n";
  try
  {
    OCC_CATCH_SIGNALS
    Message_ProgressScope aPS(theRange, "Writing polygons on triangulation", aNbPol);
    for (Standard_Integer aPolIter = 1; aPolIter <= aNbPol && aPS.More(); ++aPolIter, aPS.Next())
    {
      const Handle(Poly_PolygonOnTriangulation)& aPoly = myNodes.FindKey (aPolIter);
      const TColStd_Array1OfInteger& aNodes = aPoly->Nodes();
      BinTools::PutInteger(OS, aNodes.Length());
      for (Standard_Integer aNodeIter = 1; aNodeIter <= aNodes.Length(); ++aNodeIter)
      {
        BinTools::PutInteger(OS, aNodes.Value (aNodeIter));
      }

      // write the deflection
      BinTools::PutReal(OS, aPoly->Deflection());

      // writing parameters
      if (const Handle(TColStd_HArray1OfReal)& aParam = aPoly->Parameters())
      {
        BinTools::PutBool(OS, Standard_True);
        for (Standard_Integer aNodeIter = 1; aNodeIter <= aParam->Length(); ++aNodeIter)
        {
          BinTools::PutReal(OS, aParam->Value (aNodeIter));
        }
      }
      else
      {
        BinTools::PutBool(OS, Standard_False);
      }
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::WritePolygonOnTriangulation(..)\n" << anException << "\n";
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : ReadPolygonOnTriangulation
//purpose  :
//=======================================================================
void BinTools_ShapeSet::ReadPolygonOnTriangulation
  (Standard_IStream& IS,
   const Message_ProgressRange& theRange)
{
  char aHeader[255];
  IS >> aHeader;
  if (IS.fail() || (strstr(aHeader,"PolygonOnTriangulations") == NULL))
  {
    throw Standard_Failure("BinTools_ShapeSet::ReadPolygonOnTriangulation: Not a PolygonOnTriangulation section");
  }

  Standard_Integer aNbPol = 0;
  IS >> aNbPol;
  IS.get();//remove LF
  try
  {
    OCC_CATCH_SIGNALS
    Message_ProgressScope aPS(theRange, "Reading Polygones on triangulation", aNbPol);
    for (Standard_Integer aPolIter = 1; aPolIter <= aNbPol && aPS.More(); ++aPolIter, aPS.Next())
    {
      Standard_Integer aNbNodes = 0;
      BinTools::GetInteger(IS, aNbNodes);
      Handle(Poly_PolygonOnTriangulation) aPoly = new Poly_PolygonOnTriangulation (aNbNodes, Standard_False);
      for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      {
        Standard_Integer aNode = 0;
        BinTools::GetInteger (IS, aNode);
        aPoly->SetNode (aNodeIter, aNode);
      }

      Standard_Real aDefl = 0.0;
      BinTools::GetReal(IS, aDefl);
      aPoly->Deflection (aDefl);

      Standard_Boolean hasParameters = Standard_False;
      BinTools::GetBool(IS, hasParameters);
      if (hasParameters)
      {
        Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal (1, aNbNodes);
        for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
        {
          BinTools::GetReal(IS, aParams->ChangeValue (aNodeIter));
        }
        aPoly->SetParameters (aParams);
      }
      myNodes.Add (aPoly);
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::ReadPolygonOnTriangulation(..)\n" << anException << "\n";
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : WritePolygon3D
//purpose  :
//=======================================================================
void BinTools_ShapeSet::WritePolygon3D (Standard_OStream& OS,
                                        const Message_ProgressRange& theRange)const
{
  const Standard_Integer aNbPol = myPolygons3D.Extent();
  OS << "Polygon3D " << aNbPol << "\n";
  try
  {
    OCC_CATCH_SIGNALS
    Message_ProgressScope aPS(theRange, "Writing polygons 3D", aNbPol);
    for (Standard_Integer aPolIter = 1; aPolIter <= aNbPol && aPS.More(); ++aPolIter, aPS.Next())
    {
      const Handle(Poly_Polygon3D)& aPoly = myPolygons3D.FindKey (aPolIter);
      BinTools::PutInteger(OS, aPoly->NbNodes());
      BinTools::PutBool(OS, aPoly->HasParameters());

      // write the deflection
      BinTools::PutReal(OS, aPoly->Deflection());

      // write the nodes
      const Standard_Integer  aNbNodes = aPoly->NbNodes();
      const TColgp_Array1OfPnt& aNodes = aPoly->Nodes();
      for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      {
        const gp_Pnt& aPnt = aNodes.Value (aNodeIter);
        BinTools::PutReal(OS, aPnt.X());
        BinTools::PutReal(OS, aPnt.Y());
        BinTools::PutReal(OS, aPnt.Z());
      }
      if (aPoly->HasParameters())
      {
        const TColStd_Array1OfReal& aParam = aPoly->Parameters();
        for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
        {
          BinTools::PutReal(OS, aParam.Value (aNodeIter));
        }
      }
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::WritePolygon3D(..)\n" << anException << "\n";
    throw Standard_Failure(aMsg.str().c_str());
  }
}
//=======================================================================
//function : ReadPolygon3D
//purpose  :
//=======================================================================
void BinTools_ShapeSet::ReadPolygon3D (Standard_IStream& IS,
                                       const Message_ProgressRange& theRange)
{
  char aHeader[255];
  IS >> aHeader;

  if (IS.fail() || strstr(aHeader,"Polygon3D") == NULL)
  {
#ifdef OCCT_DEBUG
    std::cout <<"Buffer: " << aHeader << std::endl;
#endif
    throw Standard_Failure("BinTools_ShapeSet::ReadPolygon3D: Not a Polygon3D section");
  }

  Standard_Integer aNbPol = 0;
  IS >> aNbPol;
  IS.get();//remove LF
  try
  {
    OCC_CATCH_SIGNALS
    Message_ProgressScope aPS(theRange, "Reading polygones 3D", aNbPol);
    for (Standard_Integer aPolIter = 1; aPolIter <= aNbPol && aPS.More(); ++aPolIter, aPS.Next())
    {
      Standard_Integer aNbNodes = 0;
      Standard_Boolean hasParameters = Standard_False;
      Standard_Real aDefl = 0.0;
      BinTools::GetInteger(IS, aNbNodes);
      BinTools::GetBool(IS, hasParameters);
      BinTools::GetReal(IS, aDefl);

      Handle(Poly_Polygon3D) aPoly = new Poly_Polygon3D (aNbNodes, hasParameters);
      aPoly->Deflection (aDefl);

      TColgp_Array1OfPnt& aNodes = aPoly->ChangeNodes();
      for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      {
        gp_XYZ& aPnt = aNodes.ChangeValue (aNodeIter).ChangeCoord();
        BinTools::GetReal(IS, aPnt.ChangeCoord (1));
        BinTools::GetReal(IS, aPnt.ChangeCoord (2));
        BinTools::GetReal(IS, aPnt.ChangeCoord (3));
      }
      if (hasParameters)
      {
        TColStd_Array1OfReal& aParam = aPoly->ChangeParameters();
        for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
        {
          BinTools::GetReal(IS, aParam.ChangeValue (aNodeIter));
        }
      }

      myPolygons3D.Add (aPoly);
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::ReadPolygon3D(..)\n" << anException << "\n";
    throw Standard_Failure(aMsg.str().c_str());
  }
}


//=======================================================================
//function : WriteTriangulation
//purpose  :
//=======================================================================
void BinTools_ShapeSet::WriteTriangulation (Standard_OStream& OS,
                                            const Message_ProgressRange& theRange) const
{
  const Standard_Integer aNbTriangulations = myTriangulations.Extent();
  OS << "Triangulations " << aNbTriangulations << "\n";

  try
  {
    OCC_CATCH_SIGNALS
    Message_ProgressScope aPS(theRange, "Writing triangulation", aNbTriangulations);
    for (Standard_Integer aTriangulationIter = 1; aTriangulationIter <= aNbTriangulations && aPS.More(); ++aTriangulationIter, aPS.Next())
    {
      const Handle(Poly_Triangulation)& aTriangulation = myTriangulations.FindKey (aTriangulationIter);
      Standard_Boolean NeedToWriteNormals = myTriangulations.FindFromIndex(aTriangulationIter);
      const Standard_Integer aNbNodes     = aTriangulation->NbNodes();
      const Standard_Integer aNbTriangles = aTriangulation->NbTriangles();
      BinTools::PutInteger(OS, aNbNodes);
      BinTools::PutInteger(OS, aNbTriangles);
      BinTools::PutBool(OS, aTriangulation->HasUVNodes() ? 1 : 0);
      if (FormatNb() >= BinTools_FormatVersion_VERSION_4)
      {
        BinTools::PutBool(OS, (aTriangulation->HasNormals() && NeedToWriteNormals) ? 1 : 0);
      }
      BinTools::PutReal(OS, aTriangulation->Deflection());

      // write the 3d nodes
      for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      {
        const gp_Pnt aPnt = aTriangulation->Node (aNodeIter);
        BinTools::PutReal(OS, aPnt.X());
        BinTools::PutReal(OS, aPnt.Y());
        BinTools::PutReal(OS, aPnt.Z());
      }

      if (aTriangulation->HasUVNodes())
      {
        for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
        {
          const gp_Pnt2d aUV = aTriangulation->UVNode (aNodeIter);
          BinTools::PutReal(OS, aUV.X());
          BinTools::PutReal(OS, aUV.Y());
        }
      }

      for (Standard_Integer aTriIter = 1; aTriIter <= aNbTriangles; ++aTriIter)
      {
        const Poly_Triangle aTri = aTriangulation->Triangle (aTriIter);
        BinTools::PutInteger(OS, aTri.Value (1));
        BinTools::PutInteger(OS, aTri.Value (2));
        BinTools::PutInteger(OS, aTri.Value (3));
      }

      // write the normals
      if (FormatNb() >= BinTools_FormatVersion_VERSION_4)
      {
        if (aTriangulation->HasNormals() && NeedToWriteNormals)
        {
          gp_Vec3f aNormal;
          for (Standard_Integer aNormalIter = 1; aNormalIter <= aNbNodes; ++aNormalIter)
          {
            aTriangulation->Normal (aNormalIter, aNormal);
            BinTools::PutShortReal (OS, aNormal.x());
            BinTools::PutShortReal (OS, aNormal.y());
            BinTools::PutShortReal (OS, aNormal.z());
          }
        }
      }
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::WriteTriangulation(..)\n" << anException << "\n";
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : ReadTriangulation
//purpose  :
//=======================================================================
void BinTools_ShapeSet::ReadTriangulation (Standard_IStream& IS,
                                           const Message_ProgressRange& theRange)
{
  char aHeader[255];
  IS >> aHeader;
  if (IS.fail() || (strstr(aHeader, "Triangulations") == NULL))
  {
    throw Standard_Failure("BinTools_ShapeSet::Triangulation: Not a Triangulation section");
  }

  Standard_Integer aNbTriangulations = 0;
  IS >> aNbTriangulations;
  IS.get();// remove LF 

  try
  {
    OCC_CATCH_SIGNALS
    Message_ProgressScope aPS(theRange, "Reading triangulation", aNbTriangulations);
    for (Standard_Integer aTriangulationIter = 1; aTriangulationIter <= aNbTriangulations && aPS.More(); ++aTriangulationIter, aPS.Next())
    {
      Standard_Integer aNbNodes = 0, aNbTriangles = 0;
      Standard_Boolean hasUV = Standard_False;
      Standard_Boolean hasNormals = Standard_False;
      Standard_Real aDefl = 0.0;
      BinTools::GetInteger(IS, aNbNodes);
      BinTools::GetInteger(IS, aNbTriangles);
      BinTools::GetBool(IS, hasUV);
      if (FormatNb() >= BinTools_FormatVersion_VERSION_4)
      {
        BinTools::GetBool(IS, hasNormals);
      }
      BinTools::GetReal(IS, aDefl); //deflection
      Handle(Poly_Triangulation) aTriangulation = new Poly_Triangulation (aNbNodes, aNbTriangles, hasUV, hasNormals);
      aTriangulation->Deflection (aDefl);

      gp_Pnt aNode;
      for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
      {
        BinTools::GetReal(IS, aNode.ChangeCoord().ChangeCoord (1));
        BinTools::GetReal(IS, aNode.ChangeCoord().ChangeCoord (2));
        BinTools::GetReal(IS, aNode.ChangeCoord().ChangeCoord (3));
        aTriangulation->SetNode (aNodeIter, aNode);
      }

      if (hasUV)
      {
        gp_Pnt2d aNode2d;
        for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
        {
          BinTools::GetReal(IS, aNode2d.ChangeCoord().ChangeCoord (1));
          BinTools::GetReal(IS, aNode2d.ChangeCoord().ChangeCoord (2));
          aTriangulation->SetUVNode (aNodeIter, aNode2d);
        }
      }

      // read the triangles
      Standard_Integer aTriNodes[3] = {};
      for (Standard_Integer aTriIter = 1; aTriIter <= aNbTriangles; ++aTriIter)
      {
        BinTools::GetInteger(IS, aTriNodes[0]);
        BinTools::GetInteger(IS, aTriNodes[1]);
        BinTools::GetInteger(IS, aTriNodes[2]);
        aTriangulation->SetTriangle (aTriIter, Poly_Triangle (aTriNodes[0], aTriNodes[1], aTriNodes[2]));
      }
      //IS.ignore(sizeof(Standard_Real) * (hasUV ? 5 : 3) * aNbNodes + sizeof(Standard_Integer) * 3 * aNbTriangles);

      if (hasNormals)
      {
        gp_Vec3f aNormal;
        for (Standard_Integer aNormalIter = 1; aNormalIter <= aNbNodes; ++aNormalIter)
        {
          BinTools::GetShortReal(IS, aNormal.x());
          BinTools::GetShortReal(IS, aNormal.y());
          BinTools::GetShortReal(IS, aNormal.z());
          aTriangulation->SetNormal (aNormalIter, aNormal);
        }
      }

      myTriangulations.Add (aTriangulation, hasNormals);
    }
  }
  catch (Standard_Failure const& anException)
  {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::ReadTriangulation(..)\n" << anException << "\n";
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================

Standard_Integer  BinTools_ShapeSet::NbShapes() const
{
  return myShapes.Extent();
}
