// Created on: 1997-01-03
// Created by: Flore Lantheaume
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



#include <DsgPrs_IdenticPresentation.ixx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>

#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Text.hxx>

#include <TCollection_AsciiString.hxx>

#include <gp_Vec.hxx>
#include <gp_Dir.hxx>

#include <ElCLib.hxx>

#include <Precision.hxx>
#include <gp_Elips.hxx>

void DsgPrs_IdenticPresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
				      const Handle(Prs3d_Drawer)& aDrawer,
				      const TCollection_ExtendedString& aText,
				      const gp_Pnt& aPntAttach,
				      const gp_Pnt& aPntOffset)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPntAttach);
  aPrims->AddVertex(aPntOffset);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // On ajoute un rond au point d'attache
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Handle(Graphic3d_AspectMarker3d) MarkerAsp = new Graphic3d_AspectMarker3d();
  MarkerAsp->SetType(Aspect_TOM_BALL);
  MarkerAsp->SetScale(0.8);
  Quantity_Color acolor;
  Aspect_TypeOfLine atype;
  Standard_Real awidth;
  LA->LineAspect()->Aspect()->Values(acolor, atype, awidth);
  MarkerAsp->SetColor(acolor);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(MarkerAsp);
  Graphic3d_Vertex V3d(aPntAttach.X(), aPntAttach.Y(), aPntAttach.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Marker(V3d);

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}


void DsgPrs_IdenticPresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
				      const Handle(Prs3d_Drawer)& aDrawer,
				      const TCollection_ExtendedString& aText,
				      const gp_Pnt& aFAttach,
				      const gp_Pnt& aSAttach,
				      const gp_Pnt& aPntOffset)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(4);

  aPrims->AddVertex(aFAttach);
  aPrims->AddVertex(aSAttach);

  // trait joignant aPntOffset
  gp_Vec v1(aFAttach, aSAttach);
  gp_Vec v2(aSAttach, aPntOffset);

  aPrims->AddVertex(aPntOffset);
  if ( !v1.IsParallel(v2, Precision::Angular()))
  {
    // on joint aPntOffset a son projete
    gp_Lin ll(aFAttach, gp_Dir(v1));
    aPrims->AddVertex(ElCLib::Value(ElCLib::Parameter(ll,aPntOffset ), ll));
  }
  else
    aPrims->AddVertex(aSAttach);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}


void DsgPrs_IdenticPresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Ax2& theAxe,
				     const gp_Pnt& aCenter,
				     const gp_Pnt& aFAttach,
				     const gp_Pnt& aSAttach,
				     const gp_Pnt& aPntOffset)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Ax2 ax = theAxe;
  ax.SetLocation(aCenter);
  Standard_Real rad = aCenter.Distance(aFAttach);
  gp_Circ CC(ax,rad );
  Standard_Real pFAttach =  ElCLib::Parameter(CC, aFAttach);
  Standard_Real pSAttach =  ElCLib::Parameter(CC, aSAttach);
  Standard_Real alpha = pSAttach - pFAttach;
  if ( alpha < 0 ) alpha += 2. * M_PI;
  const Standard_Integer nb = (Standard_Integer )( 50. * alpha / M_PI);
  const Standard_Integer nbp = Max (4, nb);
  const Standard_Real dteta = alpha/(nbp-1);

  Handle(Graphic3d_ArrayOfPolylines) aPrims;
  
  // trait joignant aPntOffset
  if ( Abs((aPntOffset.Distance(aCenter) - rad )) >= Precision::Confusion() )
  {
    aPrims = new Graphic3d_ArrayOfPolylines(nbp+2,2);
    aPrims->AddBound(2);
    aPrims->AddVertex(aPntOffset);
    aPrims->AddVertex(ElCLib::Value(ElCLib::Parameter(CC,aPntOffset ), CC));
    aPrims->AddBound(nbp);
  }
  else
    aPrims = new Graphic3d_ArrayOfPolylines(nbp);

  for (Standard_Integer i = 1; i<=nbp; i++)
    aPrims->AddVertex(ElCLib::Value(pFAttach + dteta*(i-1),CC));

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}

// jfa 16/10/2000
void DsgPrs_IdenticPresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Ax2& theAxe,
				     const gp_Pnt& aCenter,
				     const gp_Pnt& aFAttach,
				     const gp_Pnt& aSAttach,
				     const gp_Pnt& aPntOffset,
				     const gp_Pnt& aPntOnCirc)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Ax2 ax = theAxe;
  ax.SetLocation(aCenter);
  Standard_Real rad = aCenter.Distance(aFAttach);
  gp_Circ CC(ax,rad );
  Standard_Real pFAttach = ElCLib::Parameter(CC, aFAttach);
  Standard_Real pSAttach = ElCLib::Parameter(CC, aSAttach);
  Standard_Real alpha = pSAttach - pFAttach;
  if ( alpha < 0 ) alpha += 2. * M_PI;
  const Standard_Integer nb = (Standard_Integer)( 50. * alpha / M_PI);
  const Standard_Integer nbp = Max (4, nb);
  const Standard_Real dteta = alpha/(nbp-1);

  Handle(Graphic3d_ArrayOfPolylines) aPrims;

  // trait joignant aPntOffset
  if ( aPntOffset.Distance(aPntOnCirc) >= Precision::Confusion() )
  {
    aPrims = new Graphic3d_ArrayOfPolylines(nbp+2,2);
    aPrims->AddBound(2);
    aPrims->AddVertex(aPntOffset);
    aPrims->AddVertex(aPntOnCirc);
    aPrims->AddBound(nbp);
  }
  else
    aPrims = new Graphic3d_ArrayOfPolylines(nbp);

  for (Standard_Integer i = 1; i<=nbp; i++)
    aPrims->AddVertex(ElCLib::Value(pFAttach + dteta*(i-1),CC));

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}
// jfa 16/10/2000 end

// jfa 10/10/2000
void DsgPrs_IdenticPresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Elips& anEllipse,
				     const gp_Pnt& aFAttach,
				     const gp_Pnt& aSAttach,
				     const gp_Pnt& aPntOffset,
				     const gp_Pnt& aPntOnElli)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Standard_Real pFAttach =  ElCLib::Parameter(anEllipse, aFAttach);
  Standard_Real pSAttach =  ElCLib::Parameter(anEllipse, aSAttach);
  Standard_Real alpha = pSAttach - pFAttach;
  if ( alpha < 0 ) alpha += 2. * M_PI;
  const Standard_Integer nb = (Standard_Integer)(50.0*alpha/M_PI);
  const Standard_Integer nbp = Max (4, nb);
  const Standard_Real dteta = alpha/(nbp-1);

  Handle(Graphic3d_ArrayOfPolylines) aPrims;
  
  // trait joignant aPntOffset
  if ( ! aPntOnElli.IsEqual(aPntOffset, Precision::Confusion()) )
  {
    aPrims = new Graphic3d_ArrayOfPolylines(nbp+2,2);
    aPrims->AddBound(2);
    aPrims->AddVertex(aPntOffset);
    aPrims->AddVertex(aPntOnElli);
    aPrims->AddBound(nbp);
  }
  else
    aPrims = new Graphic3d_ArrayOfPolylines(nbp);

  for (Standard_Integer i = 1; i<=nbp; i++)
    aPrims->AddVertex(ElCLib::Value(pFAttach + dteta*(i-1),anEllipse));

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}
// jfa 10/10/2000 end
