// Created on: 1997-07-30
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <DDataStd.hxx>
#include <DDF.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>

#include <DBRep.hxx>
#include <DrawTrSurf.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>

#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>

#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Plane.hxx>

#include <TNaming_Builder.hxx>

#include <DDataStd_DrawPresentation.hxx>

// Unused :
#ifdef BLABLA_DEB
static void Location (Standard_Integer nb, const char** arg, gp_Ax2& Loc) {

  if (nb == 9) {
    Standard_Real X  = Draw::Atof(arg[3]);
    Standard_Real Y  = Draw::Atof(arg[4]);
    Standard_Real Z  = Draw::Atof(arg[5]);
    Standard_Real DX = Draw::Atof(arg[6]);
    Standard_Real DY = Draw::Atof(arg[7]);
    Standard_Real DZ = Draw::Atof(arg[8]);
    Loc = gp_Ax2 (gp_Pnt(X,Y,Z), gp_Dir(DX,DY,DZ));
  }  
}
#endif


//=======================================================================
//function : DDataStd_SetPoint
//purpose  : SetPoint (DF, entry, [drawpoint])
//=======================================================================

static Standard_Integer DDataStd_SetPoint (Draw_Interpretor& di,
                                           Standard_Integer nb, 
                                           const char** arg) 
{ 
  if (nb < 3) return 1;
  TDF_Label L;
  Handle(TDF_Data) DF;
  if (!DDF::GetDF (arg[1], DF)) return 1;
  DDF::AddLabel (DF, arg[2], L);
  if (nb == 3) {
    TDataXtd_Point::Set (L);  
  }
  else if (nb == 4) { 
    gp_Pnt p;
    if (DrawTrSurf::GetPoint (arg[3],p)) {
      TDataXtd_Point::Set (L,p);
    }
    else {
      di << "DDataStd_SetPoint : not a point" << "\n";
      return 1;
    }
  }  
  DDataStd_DrawPresentation::Display (L);
  return 0;
}


//=======================================================================
//function : DDataStd_SetAxis
//purpose  : SetAxis (DF, entry, drawline])
//=======================================================================

static Standard_Integer DDataStd_SetAxis (Draw_Interpretor& di,
                                          Standard_Integer nb, 
                                          const char** arg) 
{
  if (nb < 3) return 1;
  TDF_Label L;
  Handle(TDF_Data) DF;
  if (!DDF::GetDF (arg[1], DF)) return 1;
  DDF::AddLabel (DF, arg[2], L);
  if (nb == 3) {
    TDataXtd_Axis::Set (L);  
  }
  else if (nb == 4) { 
    Handle(Geom_Line) LINE =  Handle(Geom_Line)::DownCast(DrawTrSurf::Get (arg[3]));
    if (LINE.IsNull()) {
      di << "DDataStd_SetAxis : not a line" << "\n";
      return 1;
    }
    TDataXtd_Axis::Set (L,LINE->Lin()); 
  }  
  DDataStd_DrawPresentation::Display (L);    
  return 0;
}



//=======================================================================
//function : DDataStd_SetPlane
//purpose  : SetPlane (DF, entry, [drawplane])
//=======================================================================

static Standard_Integer DDataStd_SetPlane (Draw_Interpretor& di,
                                           Standard_Integer nb, 
                                           const char** arg) 
{   
  if (nb < 3) return 1;
  TDF_Label L;
  Handle(TDF_Data) DF;
  if (!DDF::GetDF (arg[1], DF)) return 1;
  DDF::AddLabel (DF, arg[2], L);
  if (nb == 3) {
    TDataXtd_Plane::Set(L);  
  }
  else if (nb == 4) {
    Handle(Geom_Plane) PLANE =  Handle(Geom_Plane)::DownCast(DrawTrSurf::Get (arg[3]));
    if (PLANE.IsNull()) {
      di << "DDataStd_SetPlane : not a plane" << "\n";
      return 1;
    }
    TDataXtd_Plane::Set (L,PLANE->Pln()); 
  }  
  DDataStd_DrawPresentation::Display (L);
  return 0;
}


//=======================================================================
//function : DDataStd_SetMove
//purpose  : 
//=======================================================================

// static Standard_Integer DDataStd_SetMove (Draw_Interpretor&,
//                                        Standard_Integer nb, 
//                                        const char** arg) 
// {  
//   if (nb < 3) return 1;
//   TDF_Label L;
//   Handle(TDF_Data) DF;
//   if (!DDF::GetDF (arg[1], DF)) return 1;
//   if (!DDF::FindLabel(DF, arg[2], L)) return 1;

//   TopLoc_Location Loc;
//   if (nb == 4) {
//     TopoDS_Shape S = DBRep::Get(arg[3]);
//     if (S.IsNull()) return 1;
//     Loc = S.Location();
//   }
  
//   else {
//     if (nb < 6) return 1;
    
//     gp_Trsf T;
//     Standard_Real x = Draw::Atof(arg[3]);
//     Standard_Real y = Draw::Atof(arg[4]);
//     Standard_Real z = Draw::Atof(arg[5]);
    
//     if (nb == 6) {
//       T.SetTranslation(gp_Vec(x,y,z));
//     }
//     else if (nb < 10)
//       return 1;
//     else {
//       Standard_Real dx = Draw::Atof(arg[6]);
//       Standard_Real dy = Draw::Atof(arg[7]);
//       Standard_Real dz = Draw::Atof(arg[8]);
//       Standard_Real ang = Draw::Atof(arg[9]);
//       T.SetRotation(gp_Ax1(gp_Pnt(x,y,z),
//                         gp_Vec(dx,dy,dz)),
//                  ang * (M_PI / 180.0));
//     }
//     Loc = T;
//   }
//   TDataStd_CoordSys::Move(L,Loc);
//   return 0;
// }



//=======================================================================
//function : DDataStd_GetPoint
//purpose  : GetPoint (DF, entry, [drawname])
//=======================================================================

static Standard_Integer DDataStd_GetPoint (Draw_Interpretor&,
                                           Standard_Integer nb, 
                                           const char** arg) 
{ 
  if (nb < 3) return 1;
  Handle(TDF_Data) DF;
  if (!DDF::GetDF (arg[1], DF)) return 1;
  Handle(TDataXtd_Point) A;
  gp_Pnt P;
  if (!DDF::Find (DF, arg[2], TDataXtd_Point::GetID(), A)) return 1;
  if (TDataXtd_Geometry::Point(A->Label(), P)) {
    if (nb == 4) DrawTrSurf::Set(arg[3], P);
    else         DrawTrSurf::Set(arg[2], P);
    return 0;
  }
  return 1;
}


//=======================================================================
//function : DDataStd_GetAxis
//purpose  : GetAxis (DF, entry, [drawname])
//=======================================================================

static Standard_Integer DDataStd_GetAxis (Draw_Interpretor&,
                                          Standard_Integer nb, 
                                          const char** arg) 
{  
  if (nb < 3) return 1;
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(arg[1],DF)) return 1;
  Handle(TDataXtd_Axis) A;
  if (!DDF::Find (DF, arg[2], TDataXtd_Axis::GetID(), A)) return 1;
  gp_Lin l;
  if (TDataXtd_Geometry::Line(A->Label(),l)) {
    Handle(Geom_Line) L = new Geom_Line (l);  
    if (nb == 4) DrawTrSurf::Set (arg[3], L);
    else         DrawTrSurf::Set (arg[2], L);
    return 0;
  }
  return 1;
}

//=======================================================================
//function : DDataStd_GetPlane
//purpose  : GetPlane (DF, entry, [drawname])
//=======================================================================

static Standard_Integer DDataStd_GetPlane (Draw_Interpretor&,
                                           Standard_Integer nb, 
                                           const char** arg) 
{  
  if (nb < 3) return 1;
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(arg[1],DF)) return 1;
  Handle(TDataXtd_Plane) A;
  if (!DDF::Find (DF, arg[2], TDataXtd_Plane::GetID(), A)) return 1;
  gp_Pln p;  
  if (TDataXtd_Geometry::Plane(A->Label(),p)) {  
    Handle(Geom_Plane) P = new Geom_Plane (p);  
    if (nb == 4) DrawTrSurf::Set (arg[3], P);
    else         DrawTrSurf::Set (arg[2], P);
    return 0;
  }
  return 1;
}

//=======================================================================
//function : DDataStd_SetGeometry
//purpose  : SetGeometry (DF, entry, [type], [shape])
//=======================================================================
static Standard_Integer DDataStd_SetGeometry (Draw_Interpretor& di,
                                              Standard_Integer nb, 
                                              const char** arg) 
{   
  if (nb < 3) return 1;

  Handle(TDF_Data) DF;
  if (!DDF::GetDF (arg[1], DF)) return 1;

  TDF_Label L;
  if (!DDF::FindLabel(DF, arg[2], L)) DDF::AddLabel(DF, arg[2], L);

  if (nb == 5)
  {
    // set NS attribute
    TopoDS_Shape s = DBRep::Get(arg[4]);  
    if (s.IsNull()) { di <<"shape not found"<< "\n"; return 1;}  
    TNaming_Builder SI (L);
    SI.Generated(s);
  }

  // set geometry attribute
  Handle(TDataXtd_Geometry) aGA = TDataXtd_Geometry::Set(L);

  // set type
  TDataXtd_GeometryEnum aType;
  if (nb > 3)
  {
    const char* aT = arg[3];
    if (strcmp(aT,"any") == 0)      aType = TDataXtd_ANY_GEOM;
    else if (strcmp(aT,"pnt") == 0) aType = TDataXtd_POINT;
    else if (strcmp(aT,"lin") == 0) aType = TDataXtd_LINE;
    else if (strcmp(aT,"cir") == 0) aType = TDataXtd_CIRCLE;
    else if (strcmp(aT,"ell") == 0) aType = TDataXtd_ELLIPSE;
    else if (strcmp(aT,"spl") == 0) aType = TDataXtd_SPLINE;
    else if (strcmp(aT,"pln") == 0) aType = TDataXtd_PLANE;
    else if (strcmp(aT,"cyl") == 0) aType = TDataXtd_CYLINDER;
    else
    {
      di << "DDataStd_SetGeometry : unknown type, must be one of:" << "\n";
      di << "any/pnt/lin/cir/ell/spl/pln/cyl" << "\n";
      return 1;
    }
  }
  else
  {
    aType = TDataXtd_Geometry::Type(L);
  }
  aGA->SetType(aType);

//  DDataStd_DrawPresentation::Display (L);
  return 0;
}

//=======================================================================
//function : DDataStd_GetGeometryType
//purpose  : GetGeometryType (DF, entry)
//=======================================================================
static Standard_Integer DDataStd_GetGeometryType (Draw_Interpretor& di,
                                              Standard_Integer nb, 
                                              const char** arg) 
{   
  if (nb != 3) return 1;

  Handle(TDF_Data) DF;
  if (!DDF::GetDF (arg[1], DF)) return 1;

  TDF_Label L;
  if (!DDF::FindLabel(DF, arg[2], L)) DDF::AddLabel(DF, arg[2], L);

  // get geometry attribute
  Handle(TDataXtd_Geometry) aGA;
  if (!L.FindAttribute(TDataXtd_Geometry::GetID(),aGA))
  {
    di << "TDataStd_Geometry : attribute not found" << "\n";
    return 1;
  }

  // get type
  TDataXtd_GeometryEnum aType = aGA->GetType();
  switch (aType)
  {
  case TDataXtd_ANY_GEOM:  di << "any"; break;
  case TDataXtd_POINT:     di << "pnt"; break;
  case TDataXtd_LINE:      di << "lin"; break;
  case TDataXtd_CIRCLE:    di << "cir"; break;
  case TDataXtd_ELLIPSE:   di << "ell"; break;
  case TDataXtd_SPLINE:    di << "spl"; break;
  case TDataXtd_PLANE:     di << "pln"; break;
  case TDataXtd_CYLINDER:  di <<"cyl"; break;
  default:
    {
      di << "DDataStd_GetGeometry : unknown type" << "\n";
      return 1;
    }
  }

  return 0;
}

//=======================================================================
//function : DatumCommands
//purpose  : 
//=======================================================================

void DDataStd::DatumCommands (Draw_Interpretor& theCommands)

{  
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "DData : Standard Attribute Commands";
  
  theCommands.Add ("SetPoint", 
                   "SetPoint (DF, entry, [drawpoint])",
                   __FILE__, DDataStd_SetPoint, g);
  
  theCommands.Add ("SetAxis", 
                   "SetAxis (DF, entry, [drawline])",
                   __FILE__, DDataStd_SetAxis, g);

  theCommands.Add ("SetPlane", 
                   "SetPlane (DF, entry, [drawplane])",
                   __FILE__, DDataStd_SetPlane, g);

//  theCommands.Add ("SetMove", 
 //                  "SetMove (DF, entry, Shape | [x, y, z, [dx, dy, dz, angle]])",
        //         __FILE__, DDataStd_SetMove, g);  

  theCommands.Add ("GetPoint", 
                   "GetPoint (DF, entry, [drawname])",
                   __FILE__, DDataStd_GetPoint, g);

  theCommands.Add ("GetAxis", 
                   "GetAxis (DF, entry, [drawname])",
                   __FILE__, DDataStd_GetAxis, g);

  theCommands.Add ("GetPlane", 
                   "GetPlane (DF, entry, [drawname])",
                   __FILE__, DDataStd_GetPlane, g);

  theCommands.Add ("SetGeometry", 
                   "SetGeometry (DF, entry, [type], [shape])",
                   __FILE__, DDataStd_SetGeometry, g);
  
  theCommands.Add ("GetGeometryType", 
                   "GetGeometryType (DF, entry)",
                   __FILE__, DDataStd_GetGeometryType, g);
}
