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

// Modifed:     Portage NT 7-5-97 DPF (strcasecmp)

#include <BRep_Builder.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnClosedSurface.hxx>
#include <BRep_PolygonOnClosedTriangulation.hxx>
#include <BRep_PolygonOnSurface.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <GeomTools.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>
#include <Poly.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_Stream.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#ifdef MacOS
#define strcasecmp(p,q) strcmp(p,q)
#elseif _WIN32
#define strcasecmp strcmp
#elseif AIX
#include <string.h>
#endif

// Modified:    02 Nov 2000: BUC60769. JMB, PTV.  In order to be able to read BRep 
//              files that came from a platform different from where CasCade 
//              is run, we need the following modifications.
//
//              On NT platforms (in BRepTools_ShapeSet):
//              ----------------
//                In Visual C++ 5 (or higher) the fstream::tellg method is not 
//                conform to Standard C++ because it modifies the file pointer
//                position and returns a wrong position. After that the next
//                readings are shifted and the reading process stop with errors.
//                
//                Workaround is following: Now we don`t use tellg for get position in stream.
//                Now able to read file (when reading TopAbs_FACE) without tellg. 
//                We simple check the next string if there are value that equal 2 
//               (It means a parametr for triangulation).


//=======================================================================
//function : BRepTools_ShapeSet
//purpose  : 
//=======================================================================

BRepTools_ShapeSet::BRepTools_ShapeSet(const Standard_Boolean isWithTriangles)
     :myWithTriangles(isWithTriangles)
{
}

//=======================================================================
//function : BRepTools_ShapeSet
//purpose  : 
//=======================================================================

BRepTools_ShapeSet::BRepTools_ShapeSet (const BRep_Builder& B,
                                        const Standard_Boolean isWithTriangles) :
       myBuilder(B), myWithTriangles(isWithTriangles)
{
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  BRepTools_ShapeSet::Clear()
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


//=======================================================================
//function : AddGeometry
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::AddGeometry(const TopoDS_Shape& S)
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
      else if (myWithTriangles) { // for XML Persistence
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

    if (myWithTriangles) { // for XML Persistence
      Handle(Poly_Triangulation) Tr = TF->Triangulation();
      if (!Tr.IsNull()) myTriangulations.Add(Tr);
    }

    ChangeLocations().Add(TF->Location());
  }
}


//=======================================================================
//function : DumpGeometry
//purpose  : 
//=======================================================================

void  BRepTools_ShapeSet::DumpGeometry(Standard_OStream& OS)const 
{
  myCurves2d.Dump(OS);
  myCurves.Dump(OS);
  DumpPolygon3D(OS);
  DumpPolygonOnTriangulation(OS);
  mySurfaces.Dump(OS);
  DumpTriangulation(OS);
}


//=======================================================================
//function : WriteGeometry
//purpose  : 
//=======================================================================

void  BRepTools_ShapeSet::WriteGeometry(Standard_OStream& OS)
{
  //OCC19559
  myCurves2d.SetProgress(GetProgress());
  myCurves.SetProgress(GetProgress());
  mySurfaces.SetProgress(GetProgress());

  if ( !GetProgress().IsNull()) {
    if(GetProgress()->UserBreak() ) return;
    GetProgress()->NewScope ( 15, "2D Curves" );
  }
  myCurves2d.Write(OS);
  
  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();
    
    GetProgress()->NewScope ( 15, "3D Curves" );
  }
  myCurves.Write(OS);
  
  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();
  
    GetProgress()->NewScope ( 10, "3D Polygons" );
  }
  WritePolygon3D(OS);
  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();

    GetProgress()->NewScope ( 10, "Polygons On Triangulation" );
  }
  WritePolygonOnTriangulation(OS);
  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();
    
    GetProgress()->NewScope ( 10, "Surfaces" );
  }
  mySurfaces.Write(OS);
  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();
    
    GetProgress()->NewScope ( 15, "Triangulations" );
  }
  WriteTriangulation(OS);
  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();
  }
}


//=======================================================================
//function : ReadGeometry
//purpose  : 
//=======================================================================

void  BRepTools_ShapeSet::ReadGeometry(Standard_IStream& IS)
{
  //OCC19559
  myCurves2d.SetProgress(GetProgress());
  myCurves.SetProgress(GetProgress());
  mySurfaces.SetProgress(GetProgress());

  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->NewScope ( 15, "2D Curves" );
  }
  myCurves2d.Read(IS);

  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();
    
    GetProgress()->NewScope ( 15, "3D Curves" );
  }
  myCurves.Read(IS);

  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();
    
    GetProgress()->NewScope ( 10, "3D Polygons" );
  }
  ReadPolygon3D(IS);
  if ( !GetProgress().IsNull() ) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();

    GetProgress()->NewScope ( 10, "Polygons On Triangulation" );
  }
  ReadPolygonOnTriangulation(IS);
  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();
    
    GetProgress()->NewScope ( 10, "Surfaces" );
  }
  mySurfaces.Read(IS);
  if ( !GetProgress().IsNull() ) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();

    GetProgress()->NewScope ( 15, "Triangulations" );
  }
  ReadTriangulation(IS);
  if ( !GetProgress().IsNull()) {
    if( GetProgress()->UserBreak() ) return;
    GetProgress()->EndScope();
    GetProgress()->Show();
  }
}

//=======================================================================
//function : PrintRegularity
//purpose  : 
//=======================================================================

static void PrintRegularity(const GeomAbs_Shape C,
                            Standard_OStream& OS)
{
  switch (C) {

  case GeomAbs_C0 :
    OS << "C0";
    break;

  case GeomAbs_G1 :
    OS << "G1";
    break;

  case GeomAbs_C1 :
    OS << "C1";
    break;

  case GeomAbs_G2 :
    OS << "G2";
    break;

  case GeomAbs_C2 :
    OS << "C2";
    break;

  case GeomAbs_C3 :
    OS << "C3";
    break;

  case GeomAbs_CN :
    OS << "CN";
    break;

  }
}

//=======================================================================
//function : DumpGeometry
//purpose  : 
//=======================================================================

void  BRepTools_ShapeSet::DumpGeometry(const TopoDS_Shape& S, 
                                       Standard_OStream&   OS)const 
{
  // Dump the geometry
  
  if (S.ShapeType() == TopAbs_VERTEX) {

    // Dump the point geometry
    TopoDS_Vertex V = TopoDS::Vertex(S);
    OS << "    Tolerance : " << BRep_Tool::Tolerance(V) << "\n";
    gp_Pnt p = BRep_Tool::Pnt(V);
    OS << "    - Point 3D : "<<p.X()<<", "<<p.Y()<<", "<<p.Z()<<"\n";

    Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(S.TShape());
    BRep_ListIteratorOfListOfPointRepresentation itrp(TV->Points());
    
    while (itrp.More()) {
      const Handle(BRep_PointRepresentation)& PR = itrp.Value();

      OS << "    - Parameter : "<< PR->Parameter();
      if (PR->IsPointOnCurve()) {
        OS << " on curve " << myCurves.Index(PR->Curve());
      }

      else if (PR->IsPointOnCurveOnSurface()) {
        OS << " on pcurve " <<  myCurves2d.Index(PR->PCurve());
        OS << " on surface " << mySurfaces.Index(PR->Surface());
      }

      else if (PR->IsPointOnSurface()) {
        OS << ", " << PR->Parameter2() << " on surface ";
        OS << mySurfaces.Index(PR->Surface());
      }

      if (!PR->Location().IsIdentity())
        OS << " location " << Locations().Index(PR->Location());
      OS << "\n";

      itrp.Next();
    }

  }

  else if (S.ShapeType() == TopAbs_EDGE) {

    Handle(BRep_TEdge) TE = Handle(BRep_TEdge)::DownCast(S.TShape());
    gp_Pnt2d Pf,Pl;

    // Dump the curve geometry 
    OS << "    Tolerance : " << TE->Tolerance() << "\n";
    if (TE->SameParameter()) OS << "     same parametrisation of curves\n";
    if (TE->SameRange())     OS << "     same range on curves\n";
    if (TE->Degenerated())   OS << "     degenerated\n";
    
    Standard_Real first, last;
    BRep_ListIteratorOfListOfCurveRepresentation itrc = TE->Curves();
    while (itrc.More()) {
      const Handle(BRep_CurveRepresentation)& CR = itrc.Value();
      if (CR->IsCurve3D()) {
        Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itrc.Value());
        GC->Range(first, last);
        if (!CR->Curve3D().IsNull()) {
          OS << "    - Curve 3D : "<<myCurves.Index(CR->Curve3D());
          if (!CR->Location().IsIdentity())
            OS << " location "<<Locations().Index(CR->Location());
          OS <<", range : " << first << " " << last <<"\n";
        }
      }
      else if (CR->IsCurveOnSurface()) {
        Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itrc.Value());
        GC->Range(first, last);
        OS <<"    - PCurve : "<<myCurves2d.Index(CR->PCurve());
        if (CR->IsCurveOnClosedSurface()) {
          OS <<", " << myCurves2d.Index(CR->PCurve2());
          OS << " (";
          PrintRegularity(CR->Continuity(),OS);
          OS <<")";
        }
        OS << " on surface "<<mySurfaces.Index(CR->Surface());
        if (!CR->Location().IsIdentity())
          OS << " location "<<Locations().Index(CR->Location());
        OS <<", range : " << first << " " << last <<"\n";
        
        Handle(BRep_CurveOnSurface) COS = 
          Handle(BRep_CurveOnSurface)::DownCast(CR);
        COS->UVPoints(Pf,Pl);
        OS << "  UV Points : " <<Pf.X()<<", "<<Pf.Y()<<" ";
        OS << Pl.X()<<", "<<Pl.Y()<<"\n";
        if (CR->IsCurveOnClosedSurface()) {
          Handle(BRep_CurveOnClosedSurface) COCS = 
            Handle(BRep_CurveOnClosedSurface)::DownCast(CR);
          COCS->UVPoints2(Pf,Pl);
          OS << "  UV Points : " <<Pf.X()<<", "<<Pf.Y()<<" ";
          OS << Pl.X()<<", "<<Pl.Y()<<"\n";
        }
      }
      else if (CR->IsRegularity()) {
        OS << "    - Regularity ";
        PrintRegularity(CR->Continuity(),OS);
        OS << "   on surfaces : "<<mySurfaces.Index(CR->Surface());
        if (!CR->Location().IsIdentity())
          OS << " location "<<Locations().Index(CR->Location());
        OS << ", "<<mySurfaces.Index(CR->Surface2());
        if (!CR->Location2().IsIdentity())
          OS << " location "<<Locations().Index(CR->Location2());
        OS << "\n";
      }
      else if (CR->IsPolygon3D()) {
        Handle(BRep_Polygon3D) GC = Handle(BRep_Polygon3D)::DownCast(itrc.Value());
        if (!GC->Polygon3D().IsNull()) {
          OS << "    - Polygon 3D : "<<myPolygons3D.FindIndex(CR->Polygon3D());
          if (!CR->Location().IsIdentity())
            OS << " location "<<Locations().Index(CR->Location());
        }
      }
      else if (CR->IsPolygonOnTriangulation()) {
        OS << "    - PolygonOnTriangulation " << myNodes.FindIndex(CR->PolygonOnTriangulation());
        if (CR->IsPolygonOnClosedTriangulation()) {
          OS << " " << myNodes.FindIndex(CR->PolygonOnTriangulation2());
        }
        OS << " on triangulation " << myTriangulations.FindIndex(CR->Triangulation());
        if (!CR->Location().IsIdentity())
          OS << " location "<<Locations().Index(CR->Location());
        OS << "\n";
      }
      itrc.Next();
    }
  }

  else if (S.ShapeType() == TopAbs_FACE) {

    const TopoDS_Face& F = TopoDS::Face(S);
    if (BRep_Tool::NaturalRestriction(F))
      OS << "NaturalRestriction\n";

    // Dump the surface geometry
    Handle(BRep_TFace) TF = Handle(BRep_TFace)::DownCast(S.TShape());
    if (!TF->Surface().IsNull()) {
      OS << "    Tolerance : " <<TF->Tolerance() << "\n";
      OS << "    - Surface : "<<mySurfaces.Index(TF->Surface());
      if (!S.Location().IsIdentity())
        OS << " location "<<Locations().Index(S.Location());
      OS << "\n";
    }
    if (!(TF->Triangulation()).IsNull()) {
      // Dump the triangulation
      OS << "    - Triangulation : " <<myTriangulations.FindIndex(TF->Triangulation());
      if (!S.Location().IsIdentity())
        OS << " location " <<Locations().Index(TF->Location());
      OS << "\n";
    }
  }
 
  OS << "\n";
}


//=======================================================================
//function : WriteGeometry
//purpose  : 
//=======================================================================

void  BRepTools_ShapeSet::WriteGeometry(const TopoDS_Shape& S, 
                                        Standard_OStream&   OS)const 
{
  // Write the geometry
  
  if (S.ShapeType() == TopAbs_VERTEX) {

    // Write the point geometry
    TopoDS_Vertex V = TopoDS::Vertex(S);
    OS << BRep_Tool::Tolerance(V) << "\n";
    gp_Pnt p = BRep_Tool::Pnt(V);
    OS<<p.X()<<" "<<p.Y()<<" "<<p.Z()<<"\n";

    Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(S.TShape());
    BRep_ListIteratorOfListOfPointRepresentation itrp(TV->Points());
    
    while (itrp.More()) {
      const Handle(BRep_PointRepresentation)& PR = itrp.Value();

      OS << PR->Parameter();
      if (PR->IsPointOnCurve()) {
        OS << " 1 " << myCurves.Index(PR->Curve());
      }

      else if (PR->IsPointOnCurveOnSurface()) {
        OS << " 2 " <<  myCurves2d.Index(PR->PCurve());
        OS << " " << mySurfaces.Index(PR->Surface());
      }

      else if (PR->IsPointOnSurface()) {
        OS << " 3 " << PR->Parameter2() << " ";
        OS << mySurfaces.Index(PR->Surface());
      }

      OS << " " << Locations().Index(PR->Location());
      OS << "\n";
      
      itrp.Next();
    }
    
    OS << "0 0\n"; // end representations

  }

  else if (S.ShapeType() == TopAbs_EDGE) {

    // Write the curve geometry 

    Handle(BRep_TEdge) TE = Handle(BRep_TEdge)::DownCast(S.TShape());

    OS << " " << TE->Tolerance() << " ";
    OS << ((TE->SameParameter()) ? 1 : 0) << " ";
    OS << ((TE->SameRange())     ? 1 : 0) << " ";
    OS << ((TE->Degenerated())   ? 1 : 0) << "\n";
    
    Standard_Real first, last;
    BRep_ListIteratorOfListOfCurveRepresentation itrc = TE->Curves();
    while (itrc.More()) {
      const Handle(BRep_CurveRepresentation)& CR = itrc.Value();
      if (CR->IsCurve3D()) {
        if (!CR->Curve3D().IsNull()) {
          Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itrc.Value());
          GC->Range(first, last);
          OS << "1 ";                               // -1- Curve 3D
          OS << " "<<myCurves.Index(CR->Curve3D());
          OS << " "<<Locations().Index(CR->Location());
          OS << " "<<first<<" "<<last;
          OS << "\n";
        }
      }
      else if (CR->IsCurveOnSurface()) {
        Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itrc.Value());
        GC->Range(first, last);
        if (!CR->IsCurveOnClosedSurface())
          OS << "2 ";                             // -2- Curve on surf
        else
          OS << "3 ";                             // -3- Curve on closed surf
        OS <<" "<<myCurves2d.Index(CR->PCurve());
        if (CR->IsCurveOnClosedSurface()) {
          OS <<" " << myCurves2d.Index(CR->PCurve2());
          PrintRegularity(CR->Continuity(),OS);
        }
        OS << " " << mySurfaces.Index(CR->Surface());
        OS << " " << Locations().Index(CR->Location());
        OS << " "<<first<<" "<<last;
        OS << "\n";

        // Write UV Points // for XML Persistence higher performance
        if (FormatNb() == 2)
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
          OS << Pf.X() << " " << Pf.Y() << " " << Pl.X() << " " << Pl.Y() << "\n";
        }
      }
      else if (CR->IsRegularity()) {
        OS << "4 ";                              // -4- Regularity
        PrintRegularity(CR->Continuity(),OS);
        OS << " "<<mySurfaces.Index(CR->Surface());
        OS << " "<<Locations().Index(CR->Location());
        OS << " "<<mySurfaces.Index(CR->Surface2());
        OS << " "<<Locations().Index(CR->Location2());
        OS << "\n";
      }

      else if (myWithTriangles) { // for XML Persistence
        if (CR->IsPolygon3D()) {
          Handle(BRep_Polygon3D) GC = Handle(BRep_Polygon3D)::DownCast(itrc.Value());
          if (!GC->Polygon3D().IsNull()) {
            OS << "5 ";                            // -5- Polygon3D
            OS << " "<<myPolygons3D.FindIndex(CR->Polygon3D());
            OS << " "<<Locations().Index(CR->Location());
            OS << "\n";
          }
        }
        else if (CR->IsPolygonOnTriangulation()) {
          Handle(BRep_PolygonOnTriangulation) PT = 
            Handle(BRep_PolygonOnTriangulation)::DownCast(itrc.Value());
          if (!CR->IsPolygonOnClosedTriangulation())
            OS << "6 ";                            // -6- Polygon on triangulation
          else
            OS << "7 ";                            // -7- Polygon on closed triangulation
          OS << " " <<  myNodes.FindIndex(PT->PolygonOnTriangulation());
          if (CR->IsPolygonOnClosedTriangulation()) {
            OS << " " << myNodes.FindIndex(PT->PolygonOnTriangulation2());
          }
          OS << " " << myTriangulations.FindIndex(PT->Triangulation());
          OS << " "<<Locations().Index(CR->Location());
          OS << "\n";
        }
      }
      
      itrc.Next();
    }
    OS << "0\n"; // end of the list of representations
  }
  
  else if (S.ShapeType() == TopAbs_FACE) {

    Handle(BRep_TFace) TF = Handle(BRep_TFace)::DownCast(S.TShape());
    const TopoDS_Face& F = TopoDS::Face(S);

    if (!(TF->Surface()).IsNull()) {
      OS << ((BRep_Tool::NaturalRestriction(F)) ? 1 : 0);
      OS << " ";
      // Write the surface geometry
      OS << " " <<TF->Tolerance();
      OS << " " <<mySurfaces.Index(TF->Surface());
      OS << " " <<Locations().Index(TF->Location());
      OS << "\n";
    }
    else //For correct reading of null face
      {
	OS << 0;
	OS << " ";
	OS << " " <<TF->Tolerance();
	OS << " " << 0;
	OS << " " << 0;
	OS << "\n";
      }
    if (myWithTriangles) { // for XML Persistence
      if (!(TF->Triangulation()).IsNull()) {
        OS << 2;
        OS << " ";
        // Write the triangulation
        OS << " " <<myTriangulations.FindIndex(TF->Triangulation());
      }
    }
  }
  
}

//=======================================================================
//function : ReadRegularity
//purpose  : 
//=======================================================================

static GeomAbs_Shape ReadRegularity(Standard_IStream& IS)
{
  char buffer[255];
  IS >> buffer;
  switch (buffer[0]) {

  case 'C' :
    switch (buffer[1]) {
      
    case '0' :
      return GeomAbs_C0;
      
    case '1' :
      return GeomAbs_C1;
      
    case '2' :
      return GeomAbs_C2;
      
    case '3' :
      return GeomAbs_C3;
      
    case 'N' :
      return GeomAbs_CN;
    }
    break;

  case 'G' :
    switch (buffer[1]) {
      
    case '1' :
      return GeomAbs_G1;
      
    case '2' :
      return GeomAbs_G2;
      
    }
    break;
  }
  return GeomAbs_C0;
}

//=======================================================================
//function : ReadGeometry
//purpose  : 
//=======================================================================

void  BRepTools_ShapeSet::ReadGeometry(const TopAbs_ShapeEnum T, 
                                       Standard_IStream&      IS, 
                                       TopoDS_Shape&          S)
{
  // Read the geometry

  Standard_Integer val,c,pc,pc2 = 0,s,s2,l,l2,t, pt, pt2 = 0;
  Standard_Real tol,X,Y,Z,first,last,p1,p2;
  Standard_Real PfX,PfY,PlX,PlY;
  gp_Pnt2d aPf, aPl;
  Standard_Boolean closed;
  GeomAbs_Shape reg = GeomAbs_C0;
  switch (T) {


    //---------
    // vertex
    //---------

  case TopAbs_VERTEX :
    {
      TopoDS_Vertex& V = TopoDS::Vertex(S);
      
      // Read the point geometry
      GeomTools::GetReal(IS, tol);
      GeomTools::GetReal(IS, X);
      GeomTools::GetReal(IS, Y);
      GeomTools::GetReal(IS, Z);
      myBuilder.MakeVertex(V,gp_Pnt(X,Y,Z),tol);
      Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(V.TShape());

      BRep_ListOfPointRepresentation& lpr = TV->ChangePoints();
      TopLoc_Location L;

      do {
        GeomTools::GetReal(IS, p1);
        IS >> val;
        
        Handle(BRep_PointRepresentation) PR;
        switch (val) {

        case 1 :
          {
            IS >> c;

//  Modified by Sergey KHROMOV - Wed Apr 24 13:59:09 2002 Begin
	    if (myCurves.Curve(c).IsNull())
	      break;
//  Modified by Sergey KHROMOV - Wed Apr 24 13:59:13 2002 End

            Handle(BRep_PointOnCurve) POC =
              new BRep_PointOnCurve(p1,
                                    myCurves.Curve(c),
                                    L);
            PR = POC;
          }
          break;

        case 2 :
          {
            IS >> pc >> s;

//  Modified by Sergey KHROMOV - Wed Apr 24 13:59:09 2002 Begin
	    if (myCurves2d.Curve2d(pc).IsNull() ||
		mySurfaces.Surface(s).IsNull())
	      break;
//  Modified by Sergey KHROMOV - Wed Apr 24 13:59:13 2002 End

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
            GeomTools::GetReal(IS, p2);
            IS >> s;

//  Modified by Sergey KHROMOV - Wed Apr 24 13:59:09 2002 Begin
	    if (mySurfaces.Surface(s).IsNull())
	      break;
//  Modified by Sergey KHROMOV - Wed Apr 24 13:59:13 2002 End

            Handle(BRep_PointOnSurface) POC =
              new BRep_PointOnSurface(p1,p2,
                                      mySurfaces.Surface(s),
                                      L);
            PR = POC;
          }
          break;
        }

        if (val > 0) {
          IS >> l;
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
        GeomTools::GetReal(IS, tol);
        IS >> val;
        myBuilder.SameParameter(E,(val == 1));
        IS >> val;
        myBuilder.SameRange(E,(val == 1));
        IS >> val;
        myBuilder.Degenerated(E,(val == 1));
        
        do {
          IS >> val;
          switch (val) {
            
          case 1 :                               // -1- Curve 3D
            IS >> c >> l;
	    if (!myCurves.Curve(c).IsNull()) {
	      myBuilder.UpdateEdge(E,myCurves.Curve(c),
				   Locations().Location(l),tol);
	    }
            GeomTools::GetReal(IS, first);
            GeomTools::GetReal(IS, last);
	    if (!myCurves.Curve(c).IsNull()) {
	      Standard_Boolean Only3d = Standard_True;
	      myBuilder.Range(E,first,last,Only3d);
	    }
            break;
            
            
          case 2 :                               // -2- Curve on surf
          case 3 :                               // -3- Curve on closed surf
            closed = (val == 3);
            IS >> pc;
            if (closed) {
              IS >> pc2;
              reg = ReadRegularity(IS);
            }

            // surface, location
            IS >> s >> l;

            // range
            GeomTools::GetReal(IS, first);
            GeomTools::GetReal(IS, last);

            // read UV Points // for XML Persistence higher performance
            if (FormatNb() == 2)
            {
              GeomTools::GetReal(IS, PfX);
              GeomTools::GetReal(IS, PfY);
              GeomTools::GetReal(IS, PlX);
              GeomTools::GetReal(IS, PlY);
              aPf = gp_Pnt2d(PfX,PfY);
              aPl = gp_Pnt2d(PlX,PlY);
            }

//  Modified by Sergey KHROMOV - Wed Apr 24 12:11:16 2002 Begin
	    if (myCurves2d.Curve2d(pc).IsNull() ||
		(closed && myCurves2d.Curve2d(pc2).IsNull()) ||
		mySurfaces.Surface(s).IsNull())
	      break;
//  Modified by Sergey KHROMOV - Wed Apr 24 12:11:17 2002 End

            if (closed) {
              if (FormatNb() == 2)
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
              if (FormatNb() == 2)
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
            
          case 4 :                               // -4- Regularity
            reg = ReadRegularity(IS);
            IS >> s >> l >> s2 >> l2;
//  Modified by Sergey KHROMOV - Wed Apr 24 12:39:13 2002 Begin
	    if (mySurfaces.Surface(s).IsNull() ||
		mySurfaces.Surface(s2).IsNull())
	      break;
//  Modified by Sergey KHROMOV - Wed Apr 24 12:39:14 2002 End
            myBuilder.Continuity(E,
                                 mySurfaces.Surface(s),
                                 mySurfaces.Surface(s2),
                                 Locations().Location(l),
                                 Locations().Location(l2),
                                 reg);
            break;

          case 5 :   // -5- Polygon3D                            
            IS >> c >> l;
//szy-02.05.2004            myBuilder.UpdateEdge(E,Handle(Poly_Polygon3D)::DownCast(myPolygons3D(c)));
			if (c > 0 && c <= myPolygons3D.Extent())
	          myBuilder.UpdateEdge(E,Handle(Poly_Polygon3D)::DownCast(myPolygons3D(c)), Locations().Location(l));
            break;

          case 6 :
          case 7 :
            closed = (val == 7);
            IS >> pt;
            if (closed) {
              IS >> pt2;
            }
            IS >> t >> l;
            if (closed) {
		      if (t > 0 && t <= myTriangulations.Extent() &&
				  pt > 0 && pt <= myNodes.Extent() &&
				  pt2 > 0 && pt2 <= myNodes.Extent())
                myBuilder.UpdateEdge
                  (E, Handle(Poly_PolygonOnTriangulation)::DownCast(myNodes(pt)),
                   Handle(Poly_PolygonOnTriangulation)::DownCast(myNodes(pt2)),
                   Handle(Poly_Triangulation)::DownCast(myTriangulations(t)),
                   Locations().Location(l));
            }
            else {
		      if (t > 0 && t <= myTriangulations.Extent() &&
				  pt > 0 && pt <= myNodes.Extent())
                myBuilder.UpdateEdge
                  (E,Handle(Poly_PolygonOnTriangulation)::DownCast(myNodes(pt)),
                   Handle(Poly_Triangulation)::DownCast(myTriangulations(t)),
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
//    streampos pos;
    myBuilder.MakeFace(F);

    IS >> val; // natural restriction
    if (val == 0 || val == 1) {
      GeomTools::GetReal(IS, tol);
      IS >> s >> l;
//  Modified by Sergey KHROMOV - Wed Apr 24 12:39:13 2002 Begin
      if (!mySurfaces.Surface(s).IsNull()) {
//  Modified by Sergey KHROMOV - Wed Apr 24 12:39:14 2002 End
	myBuilder.UpdateFace(TopoDS::Face(S),
			     mySurfaces.Surface(s),
			     Locations().Location(l),tol);
	myBuilder.NaturalRestriction(TopoDS::Face(S),(val == 1));
      }
//      pos = IS.tellg();
//      IS >> val;
    }
    else if(val == 2) {
      //only triangulation
      IS >> s;
      myBuilder.UpdateFace(TopoDS::Face(S),
                           Handle(Poly_Triangulation)::DownCast(myTriangulations(s)));
    }
//    else pos = IS.tellg();
    
    // BUC60769

    if(val == 2) break;

    char string[260];
    IS.getline ( string, 256, '\n' );
    IS.getline ( string, 256, '\n' );

    if (string[0] == '2') {
      // cas triangulation
      s = atoi ( &string[2] );
	  if (s > 0 && s <= myTriangulations.Extent())
        myBuilder.UpdateFace(TopoDS::Face(S),
                             Handle(Poly_Triangulation)::DownCast(myTriangulations(s)));
    }
//    else IS.seekg(pos);
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
    break;
  }
  
}



//=======================================================================
//function : AddShapes
//purpose  : 
//=======================================================================

void  BRepTools_ShapeSet::AddShapes(TopoDS_Shape&       S1, 
                                    const TopoDS_Shape& S2)
{
  myBuilder.Add(S1,S2);
}

//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::Check(const TopAbs_ShapeEnum T,
                               TopoDS_Shape&          S)
{
  if (T == TopAbs_FACE) {
    const TopoDS_Face& F = TopoDS::Face(S);
    BRepTools::Update(F);
  }
}



//=======================================================================
//function : WritePolygonOnTriangulation
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::WritePolygonOnTriangulation(Standard_OStream&      OS,
                                                     const Standard_Boolean Compact)const
{
  Standard_Integer i, j, nbpOntri = myNodes.Extent();

  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "Polygons On Triangulation", 0, nbpOntri, 1);
  if (Compact)
    OS << "PolygonOnTriangulations " << nbpOntri << "\n";
  else {
    OS << " -------\n";
    OS <<"Dump of " << nbpOntri << " PolygonOnTriangulations\n";
    OS << " -------\n";
  }

  Handle(Poly_PolygonOnTriangulation) Poly;
  Handle(TColStd_HArray1OfReal) Param;
  for (i=1; i<=nbpOntri && PS.More(); i++, PS.Next()) {
    Poly = Handle(Poly_PolygonOnTriangulation)::DownCast(myNodes(i));
    const TColStd_Array1OfInteger& Nodes = Poly->Nodes();
    if (!Compact) {
      OS << "  "<< i << " : PolygonOnTriangulation with " << Nodes.Length() << " Nodes\n";
    }
    else OS << Nodes.Length()<<" ";
    if (!Compact) OS <<"  ";
    for (j=1; j <= Nodes.Length(); j++) OS << Nodes.Value(j) << " ";
    OS << "\n";

    // writing parameters:
    Param = Poly->Parameters();
    if (Compact) OS <<"p ";

    // write the deflection
    if (!Compact) OS << "  Deflection : ";
    OS <<Poly->Deflection() << " ";
    if (!Compact) OS << "\n";
    
    if (!Param.IsNull()) {
      if (!Compact) {
        OS << "  Parameters :";
      }
      else OS << "1 " ;
      if (!Compact) OS <<"  ";
      for (j=1; j <= Param->Length(); j++) OS << Param->Value(j) << " ";
      OS << "\n";
    }
    else OS <<"0 \n";
  }
}

//=======================================================================
//function : DumpPolygonOnTriangulation
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::DumpPolygonOnTriangulation(Standard_OStream& OS)const
{
  WritePolygonOnTriangulation(OS, Standard_False);
}

//=======================================================================
//function : ReadPolygonOnTriangulation
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::ReadPolygonOnTriangulation(Standard_IStream& IS)
{
  char buffer[255];
  IS >> buffer;
  if (strstr(buffer,"PolygonOnTriangulations") == NULL) return;
  Standard_Integer i, j, val, nbpol = 0, nbnodes =0;
  Standard_Integer hasparameters;
  Standard_Real par;
  Handle(TColStd_HArray1OfReal) Param;
  Handle(Poly_PolygonOnTriangulation) Poly;
  IS >> nbpol;
  //OCC19559
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "Polygons On Triangulation", 0, nbpol, 1);
  for (i=1; i<=nbpol&& PS.More(); i++, PS.Next()) {
    IS >> nbnodes;
    TColStd_Array1OfInteger Nodes(1, nbnodes);
    for (j = 1; j <= nbnodes; j++) {
      IS >> val;
      Nodes(j) = val;
    }
    IS >> buffer;
//      if (!strcasecmp(buffer, "p")) {
      Standard_Real def;
      GeomTools::GetReal(IS, def);
      IS >> hasparameters;
      if (hasparameters) {
        TColStd_Array1OfReal Param1(1, nbnodes);
        for (j = 1; j <= nbnodes; j++) {
          GeomTools::GetReal(IS, par);
          Param1(j) = par;
        }
        Poly = new Poly_PolygonOnTriangulation(Nodes, Param1);
      }
      else Poly = new Poly_PolygonOnTriangulation(Nodes);
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



//=======================================================================
//function : WritePolygon3D
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::WritePolygon3D(Standard_OStream&      OS,
                                        const Standard_Boolean Compact)const
{
  Standard_Integer i, j, nbpol = myPolygons3D.Extent();
  
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "3D Poligons", 0, nbpol, 1);

  if (Compact)
    OS << "Polygon3D " << nbpol << "\n";
  else {
    OS << " -------\n";
    OS <<"Dump of " << nbpol << " Polygon3Ds\n";
    OS << " -------\n";
  }
  
  Handle(Poly_Polygon3D) P;
  for (i = 1; i <= nbpol && PS.More(); i++, PS.Next()) {
    P = Handle(Poly_Polygon3D)::DownCast(myPolygons3D(i));
    if (Compact) {
      OS << P->NbNodes() << " ";
      OS << ((P->HasParameters()) ? "1" : "0") << "\n";
    }
    else {
      OS << "  "<< i << " : Polygon3D with " << P->NbNodes() << " Nodes\n";
      OS << ((P->HasParameters()) ? "with" : "without") << " parameters\n";
    }
    

    // write the deflection
    if (!Compact) OS << "Deflection : ";
    OS << P->Deflection() << "\n";
    
    // write the nodes
    if (!Compact) OS << "\nNodes :\n";
    
    Standard_Integer i1, nbNodes = P->NbNodes();
    const TColgp_Array1OfPnt& Nodes = P->Nodes();
    for (j = 1; j <= nbNodes; j++) {
      if (!Compact) OS << setw(10) << j << " : ";
      if (!Compact) OS << setw(17);
      OS << Nodes(j).X() << " ";
      if (!Compact) OS << setw(17);
      OS << Nodes(j).Y() << " ";
      if (!Compact) OS << setw(17);
      OS << Nodes(j).Z();
      if (!Compact) OS << "\n";
      else OS << " ";
    }
    OS <<"\n";
  
    if (P->HasParameters()) {
      if (!Compact) OS << "\nParameters :\n";
      const TColStd_Array1OfReal& Param = P->Parameters();
      for ( i1 = 1; i1 <= nbNodes; i1++ ) {
        OS << Param(i1) << " ";
      }
      OS <<"\n";
    }
  }
}

//=======================================================================
//function : DumpPolygon3D
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::DumpPolygon3D(Standard_OStream& OS)const
{
  WritePolygon3D(OS, Standard_False);
}


//=======================================================================
//function : ReadPolygon3D
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::ReadPolygon3D(Standard_IStream& IS)
{
  char buffer[255];
  //  Standard_Integer i, j, p, val, nbpol, nbnodes, hasparameters;
  Standard_Integer i, j, p, nbpol=0, nbnodes =0, hasparameters = Standard_False;
  Standard_Real d, x, y, z;

  IS >> buffer;
  if (strstr(buffer,"Polygon3D") == NULL) return;
  Handle(Poly_Polygon3D) P;
  IS >> nbpol;
  //OCC19559
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "3D Polygons", 0, nbpol, 1);
  for (i=1; i<=nbpol && PS.More(); i++, PS.Next()) {
    IS >> nbnodes;
    IS >> hasparameters;
    TColgp_Array1OfPnt Nodes(1, nbnodes);
    GeomTools::GetReal(IS, d);
    for (j = 1; j <= nbnodes; j++) {
      GeomTools::GetReal(IS, x);
      GeomTools::GetReal(IS, y);
      GeomTools::GetReal(IS, z);
      Nodes(j).SetCoord(x,y,z);
    }
    if (hasparameters) {
      TColStd_Array1OfReal Param(1,nbnodes);
      for (p = 1; p <= nbnodes; p++) {
        GeomTools::GetReal(IS, Param(p));
      }
      P = new Poly_Polygon3D(Nodes, Param);
    }
    else P = new Poly_Polygon3D(Nodes);
    P->Deflection(d);
    myPolygons3D.Add(P);
  }
}



//=======================================================================
//function : WriteTriangulation
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::WriteTriangulation(Standard_OStream&      OS,
                                        const Standard_Boolean Compact)const
{
  Standard_Integer i, j, nbNodes, nbtri = myTriangulations.Extent();
  Standard_Integer nbTriangles = 0, n1, n2, n3;
  
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "Triangulations", 0, nbtri, 1);

  if (Compact)
    OS << "Triangulations " << nbtri << "\n";
  else {
    OS << " -------\n";
    OS <<"Dump of " << nbtri << " Triangulations\n";
    OS << " -------\n";
  }

  Handle(Poly_Triangulation) T;
  for (i = 1; i <= nbtri && PS.More(); i++, PS.Next()) {

    T = Handle(Poly_Triangulation)::DownCast(myTriangulations(i));
    if (Compact) {
      OS << T->NbNodes() << " " << T->NbTriangles() << " ";
      OS << ((T->HasUVNodes()) ? "1" : "0") << " ";
    }
    else {
      OS << "  "<< i << " : Triangulation with " << T->NbNodes() << " Nodes and "
         << T->NbTriangles() <<" Triangles\n";
      OS << "      "<<((T->HasUVNodes()) ? "with" : "without") << " UV nodes\n";

    }
    
    // write the deflection
    
    if (!Compact) OS << "  Deflection : ";
    OS <<T->Deflection() << "\n";
    
    // write the 3d nodes
    
    if (!Compact) OS << "\n3D Nodes :\n";
    
    nbNodes = T->NbNodes();
    const TColgp_Array1OfPnt& Nodes = T->Nodes();
    for (j = 1; j <= nbNodes; j++) {
      if (!Compact) OS << setw(10) << j << " : ";
      if (!Compact) OS << setw(17);
      OS << Nodes(j).X() << " ";
      if (!Compact) OS << setw(17);
      OS << Nodes(j).Y() << " ";
      if (!Compact) OS << setw(17);
      OS << Nodes(j).Z();
      if (!Compact) OS << "\n";
      else OS << " ";
    }
    
    if (T->HasUVNodes()) {
      if (!Compact) OS << "\nUV Nodes :\n";
      const TColgp_Array1OfPnt2d& UVNodes = T->UVNodes();
      for (j = 1; j <= nbNodes; j++) {
        if (!Compact) OS << setw(10) << j << " : ";
        if (!Compact) OS << setw(17);
        OS << UVNodes(j).X() << " ";
        if (!Compact) OS << setw(17);
        OS << UVNodes(j).Y();
        if (!Compact) OS << "\n";
        else OS << " ";
      }
    }
    
    if (!Compact) OS << "\nTriangles :\n";
    nbTriangles = T->NbTriangles();
    const Poly_Array1OfTriangle& Triangles = T->Triangles();
    for (j = 1; j <= nbTriangles; j++) {
      if (!Compact) OS << setw(10) << j << " : ";
      Triangles(j).Get(n1, n2, n3);
      if (!Compact) OS << setw(10);
      OS << n1 << " ";
      if (!Compact) OS << setw(10);
      OS << n2 << " ";
      if (!Compact) OS << setw(10);
      OS << n3;
      if (!Compact) OS << "\n";
      else OS << " ";
    }
    OS << "\n";
  }
}

//=======================================================================
//function : DumpTriangulation
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::DumpTriangulation(Standard_OStream& OS)const
{
  WriteTriangulation(OS, Standard_False);
}


//=======================================================================
//function : ReadTriangulation
//purpose  : 
//=======================================================================

void BRepTools_ShapeSet::ReadTriangulation(Standard_IStream& IS)
{
  char buffer[255];
  //  Standard_Integer i, j, val, nbtri;
  Standard_Integer i, j, nbtri =0;
  Standard_Real d, x, y, z;
  Standard_Integer nbNodes =0, nbTriangles=0;
  Standard_Boolean hasUV= Standard_False;

  Handle(Poly_Triangulation) T;

  IS >> buffer;
  if (strstr(buffer,"Triangulations") == NULL) return;

  IS >> nbtri;
  //OCC19559
  Handle(Message_ProgressIndicator) progress = GetProgress();
  Message_ProgressSentry PS(progress, "Triangulations", 0, nbtri, 1);
  for (i=1; i<=nbtri && PS.More();i++, PS.Next()) {

    IS >> nbNodes >> nbTriangles >> hasUV;
    GeomTools::GetReal(IS, d);

    TColgp_Array1OfPnt Nodes(1, nbNodes);
    TColgp_Array1OfPnt2d UVNodes(1, nbNodes);

    for (j = 1; j <= nbNodes; j++) {
      GeomTools::GetReal(IS, x);
      GeomTools::GetReal(IS, y);
      GeomTools::GetReal(IS, z);
      Nodes(j).SetCoord(x,y,z);
    }

    if (hasUV) {
      for (j = 1; j <= nbNodes; j++) {
        GeomTools::GetReal(IS, x);
        GeomTools::GetReal(IS, y);
        UVNodes(j).SetCoord(x,y);
      }
    }
      
    // read the triangles
    Standard_Integer n1,n2,n3;
    Poly_Array1OfTriangle Triangles(1, nbTriangles);
    for (j = 1; j <= nbTriangles; j++) {
      IS >> n1 >> n2 >> n3;
      Triangles(j).Set(n1,n2,n3);
    }
      
    if (hasUV) T =  new Poly_Triangulation(Nodes,UVNodes,Triangles);
    else T = new Poly_Triangulation(Nodes,Triangles);
      
    T->Deflection(d);
      
    myTriangulations.Add(T);
  }
}
