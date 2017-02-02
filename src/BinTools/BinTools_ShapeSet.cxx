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
#include <BinTools_CurveSet.hxx>
#include <BinTools_LocationSet.hxx>
#include <BinTools_ShapeSet.hxx>
#include <BinTools_SurfaceSet.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
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
#include <gp_Trsf.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <string.h>
//#define MDTV_DEB 1
const char* Version_1  = "Open CASCADE Topology V1 (c)";
const char* Version_2  = "Open CASCADE Topology V2 (c)";
const char* Version_3  = "Open CASCADE Topology V3 (c)";
//=======================================================================
//function : operator << (gp_Pnt)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const gp_Pnt P)
{
  BinTools::PutReal(OS, P.X());
  BinTools::PutReal(OS, P.Y());
  BinTools::PutReal(OS, P.Z());
  return OS;
}
//=======================================================================
//function : BinTools_ShapeSet
//purpose  : 
//=======================================================================

BinTools_ShapeSet::BinTools_ShapeSet(const Standard_Boolean isWithTriangles)
     :myFormatNb(3), myWithTriangles(isWithTriangles)
{}

//=======================================================================
//function : ~BinTools_ShapeSet
//purpose  : 
//=======================================================================

BinTools_ShapeSet::~BinTools_ShapeSet()
{}

//=======================================================================
//function : SetFormatNb
//purpose  : 
//=======================================================================
void BinTools_ShapeSet::SetFormatNb(const Standard_Integer theFormatNb)
{
  myFormatNb = theFormatNb;
}

//=======================================================================
//function : FormatNb
//purpose  : 
//=======================================================================
Standard_Integer BinTools_ShapeSet::FormatNb() const
{
  return myFormatNb;
}

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

Standard_Integer  BinTools_ShapeSet::Add(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull()) return 0;
  myLocations.Add(theShape.Location());
  TopoDS_Shape aS2 = theShape;
  aS2.Location(TopLoc_Location());
  Standard_Integer anIndex = myShapes.FindIndex(aS2);
  if (anIndex == 0) {
    AddGeometry(aS2);
    for (TopoDS_Iterator its(aS2,Standard_False,Standard_False);its.More(); its.Next())
      Add(its.Value());
    anIndex = myShapes.Add(aS2);
  }
  return anIndex;
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BinTools_ShapeSet::Shape(const Standard_Integer theIndx)const 
{
  return myShapes(theIndx);
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer BinTools_ShapeSet::Index(const TopoDS_Shape& theShape) const
{
  return myShapes.FindIndex(theShape);
}

//=======================================================================
//function : Locations
//purpose  : 
//=======================================================================

const BinTools_LocationSet&  BinTools_ShapeSet::Locations()const 
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

void BinTools_ShapeSet::AddGeometry(const TopoDS_Shape& S)
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
      else if (myWithTriangles) { 
        if (CR->IsPolygon3D()) {
          if (!CR->Polygon3D().IsNull()) {
            myPolygons3D.Add(CR->Polygon3D());
            ChangeLocations().Add(CR->Location());
          }
        }
        else if (CR->IsPolygonOnTriangulation()) {
          myTriangulations.Add(CR->Triangulation());
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
    Handle(BRep_TFace) TF = Handle(BRep_TFace)::DownCast(S.TShape());
    if (!TF->Surface().IsNull())  mySurfaces.Add(TF->Surface());

    if (myWithTriangles
     || TF->Surface().IsNull())
    {
      Handle(Poly_Triangulation) Tr = TF->Triangulation();
      if (!Tr.IsNull()) myTriangulations.Add(Tr);
    }

    ChangeLocations().Add(TF->Location());
  }
}

//=======================================================================
//function : WriteGeometry
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::WriteGeometry(Standard_OStream& OS)const 
{
  myCurves2d.Write(OS); 
  myCurves.Write(OS);
  WritePolygon3D(OS);
  WritePolygonOnTriangulation(OS);
  mySurfaces.Write(OS);
  WriteTriangulation(OS);
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::Write(Standard_OStream& OS)const 
{

  // write the copyright
  if (myFormatNb == 3)
    OS << "\n" << Version_3 << "\n";
  else if (myFormatNb == 2)
    OS << "\n" << Version_2 << "\n";
  else
    OS << "\n" << Version_1 << "\n";

  //-----------------------------------------
  // write the locations
  //-----------------------------------------

  myLocations.Write(OS);

  //-----------------------------------------
  // write the geometry
  //-----------------------------------------

  WriteGeometry(OS);

  //-----------------------------------------
  // write the shapes
  //-----------------------------------------

  Standard_Integer i, nbShapes = myShapes.Extent();
  
  OS << "\nTShapes " << nbShapes << "\n";
  
  // subshapes are written first
  for (i = 1; i <= nbShapes; i++) {

    const TopoDS_Shape& S = myShapes(i);
    
    // Type
    OS << (Standard_Byte)S.ShapeType();

    // Geometry
    WriteGeometry(S,OS);

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

void  BinTools_ShapeSet::Read(Standard_IStream& IS)
{

  Clear();

  // Check the version
  char vers[101];
  do {
    IS.getline(vers,100,'\n');
    // BUC60769 PTV 18.10.2000: remove possible '\r' at the end of the line
    
    Standard_Size lv = strlen(vers);
    if (lv > 0) {
      for (lv--; lv > 0 && (vers[lv] == '\r' || vers[lv] == '\n'); lv--)
        vers[lv] = '\0';
    }
    
  } while ( ! IS.fail() && strcmp(vers,Version_1) && strcmp(vers,Version_2) &&
	   strcmp(vers,Version_3));
  if (IS.fail()) {
    cout << "BinTools_ShapeSet::Read: File was not written with this version of the topology"<<endl;
     return;
  }

  if (strcmp(vers,Version_3) == 0) SetFormatNb(3);
  else  if (strcmp(vers,Version_2) == 0) SetFormatNb(2);    
  else SetFormatNb(1);

  //-----------------------------------------
  // read the locations
  //-----------------------------------------

  myLocations.Read(IS);
  //-----------------------------------------
  // read the geometry
  //-----------------------------------------

  ReadGeometry(IS);

  //-----------------------------------------
  // read the shapes
  //-----------------------------------------

  char buffer[255];
  IS >> buffer;
  if (IS.fail() || strcmp(buffer,"TShapes")) {
    Standard_SStream aMsg;
    aMsg << "BinTools_ShapeSet::Read: Not a TShape table"<<endl;
    throw Standard_Failure(aMsg.str().c_str());
    return;
  }

  Standard_Integer nbShapes = 0;
  IS >> nbShapes;
  IS.get();//remove lf 

  for (int i = 1; i <= nbShapes; i++) {

    TopoDS_Shape S;
    
    //Read type and create empty shape.

    TopAbs_ShapeEnum T = (TopAbs_ShapeEnum) IS.get();

    ReadGeometry(T,IS,S);
    
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
      Read(SS,IS,nbShapes);
      if (!SS.IsNull())
	AddShapes(S,SS);
    } while(!SS.IsNull());

    S.Free(aFree);
    S.Modified(aMod);
     if (myFormatNb >= 2)
       S.Checked(aChecked);
     else
       S.Checked   (Standard_False);     // force check at reading.. 
    S.Orientable(anOrient);
    S.Closed    (aClosed);
    S.Infinite  (anInf);
    S.Convex    (aConv);
    // check

    if (myFormatNb == 1)
      if(T == TopAbs_FACE) {
	const TopoDS_Face& F = TopoDS::Face(S);
	BRepTools::Update(F);
      }
    myShapes.Add(S);
  }
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::Write(const TopoDS_Shape& S, Standard_OStream& OS)const 
{
  if (S.IsNull()) 

    OS << '*';
  else {    
// {TopAbs_FORWARD, TopAbs_REVERSED, TopAbs_INTERNAL, TopAbs_EXTERNAL} 
    OS << (Standard_Byte) S.Orientation();
    BinTools::PutInteger(OS, myShapes.Extent() - myShapes.FindIndex(S.Located(TopLoc_Location())) + 1);
    BinTools::PutInteger(OS, Locations().Index(S.Location()));
  }    
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::Read(TopoDS_Shape& S, Standard_IStream& IS,
                              const Standard_Integer nbshapes)const 
{
  Standard_Character aChar = '\0';
  IS >> aChar;
  if(aChar == '*')
    S = TopoDS_Shape();
  else {
    TopAbs_Orientation anOrient;
    anOrient = (TopAbs_Orientation)aChar;
    Standard_Integer anIndx;
    BinTools::GetInteger(IS, anIndx);
    S = myShapes(nbshapes - anIndx + 1);
    S.Orientation(anOrient);

    Standard_Integer l;
    BinTools::GetInteger(IS, l);
    S.Location(myLocations.Location(l));
  }
}

//=======================================================================
//function : ReadGeometry
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::ReadGeometry(Standard_IStream& IS)
{
  myCurves2d.Read(IS);
  myCurves.Read(IS);
  ReadPolygon3D(IS);
  ReadPolygonOnTriangulation(IS);
  mySurfaces.Read(IS);
  ReadTriangulation(IS);
}

//=======================================================================
//function : WriteGeometry
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::WriteGeometry(const TopoDS_Shape& S, 
                                        Standard_OStream&   OS)const 
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
	  if (FormatNb() >= 2)
	    {
	      gp_Pnt2d Pf,Pl;
	      if (CR->IsCurveOnClosedSurface()) {
		Handle(BRep_CurveOnClosedSurface) COCS = 
		  Handle(BRep_CurveOnClosedSurface)::DownCast(CR);
		COCS->UVPoints2(Pf,Pl);
	      }
	      else {
		Handle(BRep_CurveOnSurface) COS = 
		  Handle(BRep_CurveOnSurface)::DownCast(CR);
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

	else if (myWithTriangles) { 
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
      Standard_Boolean aNatRes = (BRep_Tool::NaturalRestriction(F)) ? Standard_True : Standard_False;
      BinTools::PutBool (OS, aNatRes);
      BinTools::PutReal (OS, TF->Tolerance());
      BinTools::PutInteger (OS, !TF->Surface().IsNull()
                               ? mySurfaces.Index (TF->Surface())
                               : 0);
      BinTools::PutInteger (OS, Locations().Index (TF->Location()));

      if (myWithTriangles
       || TF->Surface().IsNull())
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
    aMsg << "EXCEPTION in BinTools_ShapeSet::WriteGeometry(S,OS)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : ReadGeometry
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::ReadGeometry(const TopAbs_ShapeEnum T, 
                                       Standard_IStream&      IS, 
                                       TopoDS_Shape&          S)
{
  // Read the geometry

  Standard_Integer val, c,pc,pc2 = 0,s,s2,l,l2,t, pt, pt2 = 0;
  Standard_Real tol,X,Y,Z,first,last,p1 = 0.,p2;
  Standard_Real PfX,PfY,PlX,PlY;
  gp_Pnt2d aPf, aPl;
  Standard_Boolean closed, bval;
  GeomAbs_Shape reg = GeomAbs_C0;
  try {
    OCC_CATCH_SIGNALS
    switch (T) {


    //---------
    // vertex
    //---------

    case TopAbs_VERTEX :
      {
//       Standard_Integer aPos = IS.tellg();      
//       cout << "\nPOS = " << aPos << endl;
	TopoDS_Vertex& V = TopoDS::Vertex(S);

      // Read the point geometry
	BinTools::GetReal(IS, tol);
	BinTools::GetReal(IS, X);
	BinTools::GetReal(IS, Y);
	BinTools::GetReal(IS, Z);
	gp_Pnt aPnt (X, Y, Z);
	myBuilder.MakeVertex (V, aPnt, tol);
	Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(V.TShape());

	BRep_ListOfPointRepresentation& lpr = TV->ChangePoints();
	TopLoc_Location L;
	Standard_Boolean aNewF = (myFormatNb > 2) ? Standard_True : Standard_False;
	do {
	  if(aNewF) {
	    val = (Standard_Integer)IS.get();//case {0|1|2|3}
	    if (val > 0 && val <= 3) 
	      BinTools::GetReal(IS, p1); 
	  } else {
        streampos aPos = IS.tellg();
	    BinTools::GetReal(IS, p1); 	    
	    val = (Standard_Integer)IS.get();//case {0|1|2|3}
#ifdef OCCT_DEBUG
	    cout << "\nVal = " << val <<endl;   
#endif	  
	    if(val != 1 && val !=2 && val !=3){
	      IS.seekg(aPos);
	      val = (Standard_Integer)IS.get();
	      if (val > 0 && val <= 3) 
		BinTools::GetReal(IS, p1);
	    }
	  }
	  Handle(BRep_PointRepresentation) PR;
	  switch (val) {
	  case 0 :
	    break;
	    
	  case 1 :
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
	    
	  case 2 :
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
	    
	  case 3 :
	    {
	      BinTools::GetReal(IS, p2);
	      BinTools::GetInteger(IS, s);
	      if (mySurfaces.Surface(s).IsNull())
		break;
	      
	      Handle(BRep_PointOnSurface) POC =
		new BRep_PointOnSurface(p1,p2,
					mySurfaces.Surface(s),
					L);
	      PR = POC;
	    }
	    break;
	    
	  default:
	    {
              Standard_SStream aMsg;
	      aMsg << "BinTools_SurfaceSet::ReadGeometry: UnExpected BRep_PointRepresentation = "<< val <<endl;
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


    case TopAbs_EDGE :

      // Create an edge
      {
        TopoDS_Edge& E = TopoDS::Edge(S);
        
        myBuilder.MakeEdge(E);
        
        // Read the curve geometry 
	BinTools::GetReal(IS, tol);
	BinTools::GetBool(IS, bval);
        myBuilder.SameParameter(E, bval);

	BinTools::GetBool(IS, bval);
        myBuilder.SameRange(E,bval);

	BinTools::GetBool(IS, bval);
        myBuilder.Degenerated(E,bval);
        
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

          case 1 :                               // -1- Curve 3D
	    BinTools::GetInteger(IS, c);
	    BinTools::GetInteger(IS, l);
	    if (!myCurves.Curve(c).IsNull()) {
	      myBuilder.UpdateEdge(E,myCurves.Curve(c),
				   Locations().Location(l),tol);
	    }
	    BinTools::GetReal(IS, first);
	    BinTools::GetReal(IS, last);
	    if (!myCurves.Curve(c).IsNull()) {
	      Standard_Boolean Only3d = Standard_True;
	      myBuilder.Range(E,first,last,Only3d);
	    }
            break;
            
            
          case 2 : // -2- Curve on surf
          case 3 : // -3- Curve on closed surf
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
            if (FormatNb() >= 2)
            {
	      BinTools::GetReal(IS, PfX);
	      BinTools::GetReal(IS, PfY);
	      BinTools::GetReal(IS, PlX);
	      BinTools::GetReal(IS, PlY);
              aPf = gp_Pnt2d(PfX,PfY);
              aPl = gp_Pnt2d(PlX,PlY);
            }

	    if (myCurves2d.Curve2d(pc).IsNull() ||
		(closed && myCurves2d.Curve2d(pc2).IsNull()) ||
		mySurfaces.Surface(s).IsNull())
	      break;
	    
            if (closed) {
              if (FormatNb() >= 2)
                myBuilder.UpdateEdge(E,myCurves2d.Curve2d(pc),
                                     myCurves2d.Curve2d(pc2),
                                     mySurfaces.Surface(s),
                                     Locations().Location(l),tol,
                                     aPf, aPl);
              else
                myBuilder.UpdateEdge(E,myCurves2d.Curve2d(pc),
                                     myCurves2d.Curve2d(pc2),
                                     mySurfaces.Surface(s),
                                     Locations().Location(l),tol);

              myBuilder.Continuity(E,
                                   mySurfaces.Surface(s),
                                   mySurfaces.Surface(s),
                                   Locations().Location(l),
                                   Locations().Location(l),
                                   reg);
            }
            else
            {
              if (FormatNb() >= 2)
                myBuilder.UpdateEdge(E,myCurves2d.Curve2d(pc),
                                     mySurfaces.Surface(s),
                                     Locations().Location(l),tol,
                                     aPf, aPl);
              else
                myBuilder.UpdateEdge(E,myCurves2d.Curve2d(pc),
                                     mySurfaces.Surface(s),
                                     Locations().Location(l),tol);
            }
            myBuilder.Range(E,
                            mySurfaces.Surface(s),
                            Locations().Location(l),
                            first,last);
            break;
            
          case 4 : // -4- Regularity
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
	    
          case 5 : // -5- Polygon3D                     
	    BinTools::GetInteger(IS, c);
	    BinTools::GetInteger(IS, l);
//??? Bug?  myBuilder.UpdateEdge(E,Handle(Poly_Polygon3D)::DownCast(myPolygons3D(c)));
	    myBuilder.UpdateEdge(E,Handle(Poly_Polygon3D)::DownCast(myPolygons3D(c)), Locations().Location(l));
            break;

          case 6 : // -6- Polygon on triangulation
          case 7 : // -7- Polygon on closed triangulation
            closed = (val == 7);
	    BinTools::GetInteger(IS, pt);
            if (closed) 
	      BinTools::GetInteger(IS, pt2);

	    BinTools::GetInteger(IS, t);
	    BinTools::GetInteger(IS, l);
            if (closed) {
              myBuilder.UpdateEdge
                (E, Handle(Poly_PolygonOnTriangulation)::DownCast(myNodes(pt)),
                 Handle(Poly_PolygonOnTriangulation)::DownCast(myNodes(pt2)),
                 Handle(Poly_Triangulation)::DownCast(myTriangulations(t)),
                 Locations().Location(l));
            }
            else {
              myBuilder.UpdateEdge
                (E,Handle(Poly_PolygonOnTriangulation)::DownCast(myNodes(pt)),
                 Handle(Poly_Triangulation)::DownCast(myTriangulations(t)),
                 Locations().Location(l));
            }
            // range            
            break;
	  default:
	    {
              Standard_SStream aMsg;
	      aMsg <<"Unexpected Curve Representation ="<< val << endl;
	      throw Standard_Failure(aMsg.str().c_str());
	    }
            
          }
        } while (val > 0);
      }
      break;


    //---------
    // wire
    //---------

    case TopAbs_WIRE :
      myBuilder.MakeWire(TopoDS::Wire(S));
      break;


    //---------
    // face
    //---------

    case TopAbs_FACE :
      {
    // create a face :
	TopoDS_Face& F = TopoDS::Face(S);
	myBuilder.MakeFace(F);
	BinTools::GetBool(IS, bval); //NaturalRestriction flag
	BinTools::GetReal(IS, tol);
	BinTools::GetInteger(IS, s); //surface indx
	BinTools::GetInteger(IS, l); //location indx
	myBuilder.UpdateFace (F,
                        s > 0 ? mySurfaces.Surface(s) : Handle(Geom_Surface)(),
			                  Locations().Location(l),
                        tol);
	myBuilder.NaturalRestriction (F, bval);
    
	Standard_Byte aByte = (Standard_Byte)IS.get();
      // cas triangulation
	if(aByte == 2) {
	  BinTools::GetInteger(IS, s);
	  myBuilder.UpdateFace(TopoDS::Face(S),
			       Handle(Poly_Triangulation)::DownCast(myTriangulations(s)));
	}
      }
      break;


    //---------
    // shell
    //---------

    case TopAbs_SHELL :
      myBuilder.MakeShell(TopoDS::Shell(S));
      break;


    //---------
    // solid
    //---------

    case TopAbs_SOLID :
      myBuilder.MakeSolid(TopoDS::Solid(S));
      break;


    //---------
    // compsolid
    //---------

    case TopAbs_COMPSOLID :
      myBuilder.MakeCompSolid(TopoDS::CompSolid(S));
      break;


    //---------
    // compound
    //---------

    case TopAbs_COMPOUND :
      myBuilder.MakeCompound(TopoDS::Compound(S));
      break;

    default:
      {
        Standard_SStream aMsg;
        aMsg << "Unexpected topology type = "<< T <<endl;
        throw Standard_Failure(aMsg.str().c_str());
        break;
      }
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::ReadGeometry(S,OS)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}



//=======================================================================
//function : AddShapes
//purpose  : 
//=======================================================================

void  BinTools_ShapeSet::AddShapes(TopoDS_Shape&       S1, 
                                    const TopoDS_Shape& S2)
{
  myBuilder.Add(S1,S2);
}


//=======================================================================
//function : WritePolygonOnTriangulation
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::WritePolygonOnTriangulation(Standard_OStream& OS) const
{
  Standard_Integer i, j, nbpOntri = myNodes.Extent();

  OS << "PolygonOnTriangulations " << nbpOntri << "\n";
  Handle(Poly_PolygonOnTriangulation) Poly;
  Handle(TColStd_HArray1OfReal) Param;
  try {
    OCC_CATCH_SIGNALS
    for (i=1; i<=nbpOntri; i++) {
      Poly = Handle(Poly_PolygonOnTriangulation)::DownCast(myNodes(i));
      const TColStd_Array1OfInteger& Nodes = Poly->Nodes();
      BinTools::PutInteger(OS, Nodes.Length());
      for (j=1; j <= Nodes.Length(); j++) 
	BinTools::PutInteger(OS,  Nodes.Value(j));

    // writing parameters:
      Param = Poly->Parameters();

    // write the deflection
      BinTools::PutReal(OS, Poly->Deflection());
      if (!Param.IsNull()) {
	BinTools::PutBool(OS, Standard_True);
	for (j=1; j <= Param->Length(); j++) 
	  BinTools::PutReal(OS, Param->Value(j)); 
      }
      else 
	BinTools::PutBool(OS, Standard_False);
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::WritePolygonOnTriangulation(..)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : ReadPolygonOnTriangulation
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::ReadPolygonOnTriangulation(Standard_IStream& IS)
{
  char buffer[255];
  IS >> buffer;
  if (IS.fail() || (strstr(buffer,"PolygonOnTriangulations") == NULL)) {
    throw Standard_Failure("BinTools_ShapeSet::ReadPolygonOnTriangulation: Not a PolygonOnTriangulation section");
  }
  Standard_Integer i, j, val, nbpol = 0, nbnodes =0;
  Standard_Boolean hasparameters;
  Standard_Real par;
  Handle(TColStd_HArray1OfReal) Param;
  Handle(Poly_PolygonOnTriangulation) Poly;
  IS >> nbpol;
  IS.get();//remove LF 
  try {
    OCC_CATCH_SIGNALS
    for (i=1; i<=nbpol; i++) {
      BinTools::GetInteger(IS, nbnodes);

      TColStd_Array1OfInteger Nodes(1, nbnodes);
      for (j = 1; j <= nbnodes; j++) {
	BinTools::GetInteger(IS, val);
	Nodes(j) = val;
      }
      Standard_Real def;
      BinTools::GetReal(IS, def);
      BinTools::GetBool(IS, hasparameters);
      if (hasparameters) {
	TColStd_Array1OfReal Param1(1, nbnodes);
	for (j = 1; j <= nbnodes; j++) {
	  BinTools::GetReal(IS, par);
	  Param1(j) = par;
	}
	Poly = new Poly_PolygonOnTriangulation(Nodes, Param1);
      }
      else Poly = new Poly_PolygonOnTriangulation(Nodes);
      Poly->Deflection(def);
      myNodes.Add(Poly);
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::ReadPolygonOnTriangulation(..)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}



//=======================================================================
//function : WritePolygon3D
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::WritePolygon3D(Standard_OStream& OS)const
{
  Standard_Integer i, j, nbpol = myPolygons3D.Extent();
  OS << "Polygon3D " << nbpol << "\n";
  Handle(Poly_Polygon3D) P;
  try {
    OCC_CATCH_SIGNALS
    for (i = 1; i <= nbpol; i++) {
      P = Handle(Poly_Polygon3D)::DownCast(myPolygons3D(i));
      BinTools::PutInteger(OS, P->NbNodes());
      BinTools::PutBool(OS, P->HasParameters()? 1:0);

    // write the deflection
      BinTools::PutReal(OS, P->Deflection());

    // write the nodes
      Standard_Integer i1, nbNodes = P->NbNodes();
      const TColgp_Array1OfPnt& Nodes = P->Nodes();
      for (j = 1; j <= nbNodes; j++) {
	BinTools::PutReal(OS, Nodes(j).X());
	BinTools::PutReal(OS, Nodes(j).Y());
	BinTools::PutReal(OS, Nodes(j).Z());
      }
      if (P->HasParameters()) {
	const TColStd_Array1OfReal& Param = P->Parameters();
	for ( i1 = 1; i1 <= nbNodes; i1++ ) {
	  BinTools::PutReal(OS, Param(i1));
	}
      }
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::WritePolygon3D(..)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}
//=======================================================================
//function : ReadPolygon3D
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::ReadPolygon3D(Standard_IStream& IS)
{
  char buffer[255];
  Standard_Integer i, j, p, nbpol=0, nbnodes =0;
  Standard_Boolean hasparameters = Standard_False;
  Standard_Real d, x, y, z;
  IS >> buffer;

  if (IS.fail() || strstr(buffer,"Polygon3D") == NULL) {
#ifdef OCCT_DEBUG
    cout <<"Buffer: " << buffer << endl;
#endif
    throw Standard_Failure("BinTools_ShapeSet::ReadPolygon3D: Not a Polygon3D section");
  }
  Handle(Poly_Polygon3D) P;
  IS >> nbpol;
  IS.get();//remove LF 

  try {
    OCC_CATCH_SIGNALS
    for (i=1; i<=nbpol; i++) {
      BinTools::GetInteger(IS, nbnodes);
      BinTools::GetBool(IS, hasparameters);
      TColgp_Array1OfPnt Nodes(1, nbnodes);
      BinTools::GetReal(IS, d);
      for (j = 1; j <= nbnodes; j++) {
	BinTools::GetReal(IS, x);
	BinTools::GetReal(IS, y);
	BinTools::GetReal(IS, z);
	Nodes(j).SetCoord(x,y,z);
      }
      if (hasparameters) {
	TColStd_Array1OfReal Param(1,nbnodes);
	for (p = 1; p <= nbnodes; p++) 
	  BinTools::GetReal(IS, Param(p));

	P = new Poly_Polygon3D(Nodes, Param);
      }
      else P = new Poly_Polygon3D(Nodes);
      P->Deflection(d);
      myPolygons3D.Add(P);
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::ReadPolygon3D(..)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}


//=======================================================================
//function : WriteTriangulation
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::WriteTriangulation(Standard_OStream& OS) const
{
  Standard_Integer i, j, nbNodes, nbtri = myTriangulations.Extent();
  Standard_Integer nbTriangles = 0, n1, n2, n3;
    OS << "Triangulations " << nbtri << "\n";
  Handle(Poly_Triangulation) T;
  try {
    OCC_CATCH_SIGNALS
    for (i = 1; i <= nbtri; i++) {
      T = Handle(Poly_Triangulation)::DownCast(myTriangulations(i));
      BinTools::PutInteger(OS, T->NbNodes());
      BinTools::PutInteger(OS, T->NbTriangles());
      BinTools::PutBool(OS, T->HasUVNodes()? 1:0);
    // write the deflection
      BinTools::PutReal(OS, T->Deflection());

    // write the 3d nodes
      nbNodes = T->NbNodes();
      const TColgp_Array1OfPnt& Nodes = T->Nodes();
      for (j = 1; j <= nbNodes; j++) {
	BinTools::PutReal(OS, Nodes(j).X());
	BinTools::PutReal(OS, Nodes(j).Y());
	BinTools::PutReal(OS, Nodes(j).Z());
      }
    
      if (T->HasUVNodes()) {
	const TColgp_Array1OfPnt2d& UVNodes = T->UVNodes();
	for (j = 1; j <= nbNodes; j++) {
	  BinTools::PutReal(OS, UVNodes(j).X());
	  BinTools::PutReal(OS, UVNodes(j).Y());
	}
      }
      nbTriangles = T->NbTriangles();
      const Poly_Array1OfTriangle& Triangles = T->Triangles();
      for (j = 1; j <= nbTriangles; j++) {
	Triangles(j).Get(n1, n2, n3);
	BinTools::PutInteger(OS, n1);
	BinTools::PutInteger(OS, n2);
	BinTools::PutInteger(OS, n3);
      }
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::WriteTriangulation(..)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : ReadTriangulation
//purpose  : 
//=======================================================================

void BinTools_ShapeSet::ReadTriangulation(Standard_IStream& IS)
{
  char buffer[255];
  Standard_Integer i, j, nbtri =0;
  Standard_Real d, x, y, z;
  Standard_Integer nbNodes =0, nbTriangles=0;
  Standard_Boolean hasUV = Standard_False;

  Handle(Poly_Triangulation) T;
  IS >> buffer;

  if (IS.fail() || (strstr(buffer,"Triangulations") == NULL)) {
    throw Standard_Failure("BinTools_ShapeSet::Triangulation: Not a Triangulation section");
  }
  IS >> nbtri;
  IS.get();// remove LF 

  try {
    OCC_CATCH_SIGNALS
    for (i=1; i<=nbtri; i++) {
      BinTools::GetInteger(IS, nbNodes);
      BinTools::GetInteger(IS, nbTriangles);
      TColgp_Array1OfPnt Nodes(1, nbNodes);
      BinTools::GetBool(IS, hasUV);
      TColgp_Array1OfPnt2d UVNodes(1, nbNodes);
      BinTools::GetReal(IS, d); //deflection
      for (j = 1; j <= nbNodes; j++) {
	BinTools::GetReal(IS, x);
	BinTools::GetReal(IS, y);
	BinTools::GetReal(IS, z);
	Nodes(j).SetCoord(x,y,z);
      }
      
      if (hasUV) {
	for (j = 1; j <= nbNodes; j++) {
	  BinTools::GetReal(IS, x);
	  BinTools::GetReal(IS, y);
	  UVNodes(j).SetCoord(x,y);
	}
      }
      
      // read the triangles
      Standard_Integer n1,n2,n3;
      Poly_Array1OfTriangle Triangles(1, nbTriangles);
      for (j = 1; j <= nbTriangles; j++) {
	BinTools::GetInteger(IS, n1);
	BinTools::GetInteger(IS, n2);
	BinTools::GetInteger(IS, n3);
	Triangles(j).Set(n1,n2,n3);
      }
      
      if (hasUV) T =  new Poly_Triangulation(Nodes,UVNodes,Triangles);
      else T = new Poly_Triangulation(Nodes,Triangles);      
      T->Deflection(d);      
      myTriangulations.Add(T);
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_ShapeSet::ReadTriangulation(..)" << endl;
    aMsg << anException << endl;
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
