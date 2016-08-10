// Created on: 1997-01-03
// Created by: Flore Lantheaume
// Copyright (c) 1997-1999 Matra Datavision
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


#include <DsgPrs_IdenticPresentation.hxx>
#include <ElCLib.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

void DsgPrs_IdenticPresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
				      const Handle(Prs3d_Drawer)& aDrawer,
				      const TCollection_ExtendedString& aText,
				      const gp_Pnt& aPntAttach,
				      const gp_Pnt& aPntOffset)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPntAttach);
  aPrims->AddVertex(aPntOffset);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Quantity_Color aColor = LA->LineAspect()->Aspect()->Color();
  Handle(Graphic3d_AspectMarker3d) aMarkerAsp = new Graphic3d_AspectMarker3d (Aspect_TOM_O, aColor, 1.0);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect (aMarkerAsp);
  Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
  anArrayOfPoints->AddVertex (aPntAttach.X(), aPntAttach.Y(), aPntAttach.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);

  // texte 
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, aPntOffset);
}


void DsgPrs_IdenticPresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
				      const Handle(Prs3d_Drawer)& aDrawer,
				      const TCollection_ExtendedString& aText,
				      const gp_Pnt& aFAttach,
				      const gp_Pnt& aSAttach,
				      const gp_Pnt& aPntOffset)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
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
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, aPntOffset);
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
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
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
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, aPntOffset);
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
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
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
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, aPntOffset);
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
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
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
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, aPntOffset);
}
// jfa 10/10/2000 end
