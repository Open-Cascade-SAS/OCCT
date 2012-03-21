// Created on: 2000-10-20
// Created by: Julia DOROVSKIKH
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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


#include <DsgPrs_MidPointPresentation.ixx>

#include <Precision.hxx>

#include <TCollection_ExtendedString.hxx>

#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>

#include <ElCLib.hxx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_AspectLine3d.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Text.hxx>

//===================================================================
//Function:Add
//Purpose: draws the representation of a radial symmetry between two vertices.
//===================================================================
void DsgPrs_MidPointPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				       const Handle(Prs3d_Drawer)& aDrawer,	
				       const gp_Ax2&  theAxe,
				       const gp_Pnt&  MidPoint,
				       const gp_Pnt&  Position,
				       const gp_Pnt&  AttachPoint,
				       const Standard_Boolean first)
{ 
  Standard_Real rad = AttachPoint.Distance(MidPoint)/20.0;

  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Graphic3d_Array1OfVertex V(1,2);

  gp_Pnt Ptmp;

  gp_Ax2 ax = theAxe;
  ax.SetLocation(MidPoint);
  gp_Circ aCircleM (ax,rad);

  if ( first )
    {
      // center of the symmetry - circle around the MidPoint
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      Standard_Real alpha = 2 * M_PI;
      Standard_Integer nbp = 100;
      Graphic3d_Array1OfVertex VC(1,nbp);
      Standard_Real dteta = alpha/(nbp-1);
      gp_Pnt ptcur;
      for (Standard_Integer i = 1; i <= nbp; i++)
        {
          ptcur =  ElCLib::Value(dteta*(i-1),aCircleM);
          VC(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
        }
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VC);

      // segment from mid point to the text position
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      // mid point
      if ( Position.IsEqual(MidPoint,rad) )
        Ptmp = MidPoint;
      else
        Ptmp = ElCLib::Value(ElCLib::Parameter(aCircleM,Position),aCircleM);
      V(1).SetCoord(Ptmp.X(),Ptmp.Y(),Ptmp.Z());
      // text position
      V(2).SetCoord(Position.X(),Position.Y(),Position.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

      // texte 
      TCollection_ExtendedString aText (" (+)");
      Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,Position);
    }

  if ( !AttachPoint.IsEqual(MidPoint, Precision::Confusion()) )
    {
      // segment from mid point to the geometry
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      // mid point
      Ptmp = ElCLib::Value(ElCLib::Parameter(aCircleM,AttachPoint),aCircleM);
      V(1).SetCoord(Ptmp.X(),Ptmp.Y(),Ptmp.Z());
      // attach point to the geometry
      V(2).SetCoord(AttachPoint.X(),AttachPoint.Y(),AttachPoint.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    }
}
  
//===================================================================
//Function:Add
//Purpose: draws the representation of a radial symmetry between two linear segments.
//===================================================================
void DsgPrs_MidPointPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				       const Handle(Prs3d_Drawer)& aDrawer,	
				       const gp_Ax2&  theAxe,
				       const gp_Pnt&  MidPoint,
				       const gp_Pnt&  Position,
				       const gp_Pnt&  AttachPoint,
				       const gp_Pnt&  Point1,
				       const gp_Pnt&  Point2,
				       const Standard_Boolean first)
{
  Standard_Real rad = AttachPoint.Distance(MidPoint)/20.0;
  if ( rad <= Precision::Confusion() ) rad = Point1.Distance(Point2)/20.0;

  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Graphic3d_Array1OfVertex V(1,2);

  gp_Pnt Ptmp;

  gp_Ax2 ax = theAxe;
  ax.SetLocation(MidPoint);
  gp_Circ aCircleM (ax,rad);

  if ( first )
    {
      // center of the symmetry - circle around the MidPoint
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      Standard_Real alpha = 2 * M_PI;
      Standard_Integer nbp = 100;
      Graphic3d_Array1OfVertex VC(1,nbp);
      Standard_Real dteta = alpha/(nbp-1);
      gp_Pnt ptcur;
      for (Standard_Integer i = 1; i <= nbp; i++)
        {
          ptcur =  ElCLib::Value(dteta*(i-1),aCircleM);
          VC(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
        }
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VC);

      // segment from mid point to the text position
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      // mid point
      if ( Position.IsEqual(MidPoint,rad) )
        Ptmp = MidPoint;
      else
        Ptmp = ElCLib::Value(ElCLib::Parameter(aCircleM,Position),aCircleM);
      V(1).SetCoord(Ptmp.X(),Ptmp.Y(),Ptmp.Z());
      // text position
      V(2).SetCoord(Position.X(),Position.Y(),Position.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

      // texte 
      TCollection_ExtendedString aText (" (+)");
      Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,Position);
    }

  if ( !AttachPoint.IsEqual(MidPoint, Precision::Confusion()) )
    {
      // segment from mid point to the geometry
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      // mid point
      Ptmp = ElCLib::Value(ElCLib::Parameter(aCircleM,AttachPoint),aCircleM);
      V(1).SetCoord(Ptmp.X(),Ptmp.Y(),Ptmp.Z());
      // attach point to the geometry
      V(2).SetCoord(AttachPoint.X(),AttachPoint.Y(),AttachPoint.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    }

  // segment on line
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  V(1).SetCoord(Point1.X(),Point1.Y(),Point1.Z());
  V(2).SetCoord(Point2.X(),Point2.Y(),Point2.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
}
  
//===================================================================
//Function:Add
//Purpose: draws the representation of a radial symmetry between two circular arcs.
//===================================================================
void DsgPrs_MidPointPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				       const Handle(Prs3d_Drawer)& aDrawer,	
				       const gp_Circ& aCircle,
				       const gp_Pnt&  MidPoint,
				       const gp_Pnt&  Position,
				       const gp_Pnt&  AttachPoint,
				       const gp_Pnt&  Point1,
				       const gp_Pnt&  Point2,
				       const Standard_Boolean first)
{
  Standard_Real rad = AttachPoint.Distance(MidPoint)/20.0;
  if ( rad <= Precision::Confusion() ) rad = Point1.Distance(Point2)/20.0;

  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Graphic3d_Array1OfVertex V(1,2);

  gp_Pnt Ptmp,ptcur;

  gp_Ax2 ax = aCircle.Position();
  ax.SetLocation(MidPoint);
  gp_Circ aCircleM (ax,rad);

  if ( first )
    {
      // center of the symmetry - circle around the MidPoint
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      Standard_Real alpha = 2 * M_PI;
      Standard_Integer nbp = 100;
      Graphic3d_Array1OfVertex VC(1,nbp);
      Standard_Real dteta = alpha/(nbp-1);
      for (Standard_Integer i = 1; i <= nbp; i++)
        {
          ptcur =  ElCLib::Value(dteta*(i-1),aCircleM);
          VC(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
        }
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VC);

      // segment from mid point to the text position
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      // mid point
      if ( Position.IsEqual(MidPoint,rad) )
        Ptmp = MidPoint;
      else
        Ptmp = ElCLib::Value(ElCLib::Parameter(aCircleM,Position),aCircleM);
      V(1).SetCoord(Ptmp.X(),Ptmp.Y(),Ptmp.Z());
      // text position
      V(2).SetCoord(Position.X(),Position.Y(),Position.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

      // texte 
      TCollection_ExtendedString aText (" (+)");
      Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,Position);
    }

  if ( !AttachPoint.IsEqual(MidPoint, Precision::Confusion()) )
    {
      // segment from mid point to the geometry
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      // mid point
      Ptmp = ElCLib::Value(ElCLib::Parameter(aCircleM,AttachPoint),aCircleM);
      V(1).SetCoord(Ptmp.X(),Ptmp.Y(),Ptmp.Z());
      // attach point to the geometry
      V(2).SetCoord(AttachPoint.X(),AttachPoint.Y(),AttachPoint.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    }

  // segment on circle
  Standard_Real pf = ElCLib::Parameter(aCircle,Point1);
  Standard_Real pl = ElCLib::Parameter(aCircle,Point2);
  Standard_Real alpha = pl - pf;
  if ( alpha < 0 ) alpha += 2 * M_PI;
  Standard_Integer nb = (Standard_Integer)(50.0*alpha/M_PI);
  Standard_Integer nbp = Max(4,nb);
  Graphic3d_Array1OfVertex VC1(1,nbp);
  Standard_Real dteta = alpha/(nbp-1);

  for (Standard_Integer i = 1; i <= nbp; i++)
    {
      ptcur = ElCLib::Value(pf + dteta*(i-1),aCircle);
      VC1(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
    }
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VC1);
}
  
//===================================================================
//Function:Add
//Purpose: draws the representation of a radial symmetry between two elliptic arcs.
//===================================================================
void DsgPrs_MidPointPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				       const Handle(Prs3d_Drawer)& aDrawer,	
				       const gp_Elips& aCircle,
				       const gp_Pnt&   MidPoint,
				       const gp_Pnt&   Position,
				       const gp_Pnt&   AttachPoint,
				       const gp_Pnt&   Point1,
				       const gp_Pnt&   Point2,
				       const Standard_Boolean first)
{
  Standard_Real rad = AttachPoint.Distance(MidPoint)/20.0;
  if ( rad <= Precision::Confusion() ) rad = Point1.Distance(Point2)/20.0;

  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Graphic3d_Array1OfVertex V(1,2);

  gp_Pnt Ptmp,ptcur;

  gp_Ax2 ax = aCircle.Position();
  ax.SetLocation(MidPoint);
  gp_Circ aCircleM (ax,rad);

  if ( first )
    {
      // center of the symmetry - circle around the MidPoint
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      Standard_Real alpha = 2 * M_PI;
      Standard_Integer nbp = 100;
      Graphic3d_Array1OfVertex VC(1,nbp);
      Standard_Real dteta = alpha/(nbp-1);
      for (Standard_Integer i = 1; i <= nbp; i++)
        {
          ptcur =  ElCLib::Value(dteta*(i-1),aCircleM);
          VC(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
        }
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VC);

      // segment from mid point to the text position
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      // mid point
      if ( Position.IsEqual(MidPoint,rad) )
        Ptmp = MidPoint;
      else
        Ptmp = ElCLib::Value(ElCLib::Parameter(aCircleM,Position),aCircleM);
      V(1).SetCoord(Ptmp.X(),Ptmp.Y(),Ptmp.Z());
      // text position
      V(2).SetCoord(Position.X(),Position.Y(),Position.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

      // texte 
      TCollection_ExtendedString aText (" (+)");
      Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,Position);
    }

  if ( !AttachPoint.IsEqual(MidPoint, Precision::Confusion()) )
    {
      // segment from mid point to the geometry
      Prs3d_Root::NewGroup(aPresentation);
      Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
      // mid point
      Ptmp = ElCLib::Value(ElCLib::Parameter(aCircleM,AttachPoint),aCircleM);
      V(1).SetCoord(Ptmp.X(),Ptmp.Y(),Ptmp.Z());
      // attach point to the geometry
      V(2).SetCoord(AttachPoint.X(),AttachPoint.Y(),AttachPoint.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    }

  // segment on ellipse
  Standard_Real pf = ElCLib::Parameter(aCircle,Point1);
  Standard_Real pl = ElCLib::Parameter(aCircle,Point2);
  Standard_Real alpha = pl - pf;
  if ( alpha < 0 ) alpha += 2 * M_PI;
  Standard_Integer nb = (Standard_Integer)(50.0*alpha/M_PI);
  Standard_Integer nbp = Max(4,nb);
  Graphic3d_Array1OfVertex VC1(1,nbp);
  Standard_Real dteta = alpha/(nbp-1);

  for (Standard_Integer i = 1; i <= nbp; i++)
    {
      ptcur = ElCLib::Value(pf + dteta*(i-1),aCircle);
      VC1(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
    }
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VC1);
}
