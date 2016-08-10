// Created on: 1996-08-21
// Created by: Jacques MINOT
// Copyright (c) 1996-1999 Matra Datavision
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

#include <DsgPrs_DiameterPresentation.hxx>

#include <DsgPrs.hxx>
#include <ElCLib.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <TCollection_ExtendedString.hxx>

//==========================================================================
// function : DsgPrs_DiameterPresentation::Add
// purpose  : it is possible to choose the symbol of extremities of the face (arrow, point ...)
//==========================================================================
void DsgPrs_DiameterPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				       const Handle(Prs3d_Drawer)& aDrawer,
				       const TCollection_ExtendedString& aText,
				       const gp_Pnt& AttachmentPoint,
				       const gp_Circ& aCircle,
   				       const DsgPrs_ArrowSide ArrowPrs,
				       const Standard_Boolean IsDiamSymbol )
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Standard_Real parat    = ElCLib::Parameter(aCircle, AttachmentPoint);
  gp_Pnt        ptoncirc = ElCLib::Value    (parat, aCircle);

  // sideline
  gp_Pnt        center  = aCircle.Location();
  gp_Vec        vecrap  (ptoncirc,center);

  Standard_Real dist    = center.Distance(AttachmentPoint);
  Standard_Real aRadius = aCircle.Radius();
  Standard_Boolean inside = (dist < aRadius);

  gp_Pnt pt1 = AttachmentPoint;
  if (inside) {
    pt1 = ptoncirc;
    dist = aRadius;
  }
  vecrap.Normalize();
  vecrap *= (dist+aRadius);
  gp_Pnt OppositePoint = pt1.Translated(vecrap);
  
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(pt1);
  aPrims->AddVertex(OppositePoint);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // value
  TCollection_ExtendedString Text = aText;
  if(IsDiamSymbol) 
    Text = TCollection_ExtendedString("\330  ") +  aText; // VRO (2007-05-17) inserted a blank.
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), Text, AttachmentPoint);

  // arrows
  gp_Dir arrdir (vecrap);
  if (inside) arrdir.Reverse();

  gp_Vec vecrap2 = vecrap; 
  gp_Pnt ptoncirc2 = ptoncirc;
  gp_Dir arrdir2 = arrdir;
  vecrap2.Normalize();
  vecrap2 *= (aCircle.Radius() * 2.);
  ptoncirc2.Translate (vecrap2);
  arrdir2.Reverse();

  DsgPrs::ComputeSymbol(aPresentation,LA,ptoncirc,ptoncirc2,arrdir,arrdir2,ArrowPrs);
}


static Standard_Boolean DsgPrs_InDomain(const Standard_Real fpar,
					const Standard_Real lpar,
					const Standard_Real para) 
{
 if (fpar >= 0.) {
    if(lpar > fpar)
      return ((para >= fpar) && (para <= lpar));
    else { // fpar > lpar
      Standard_Real delta = 2.*M_PI-fpar;
      Standard_Real lp, par, fp;
      lp = lpar + delta;
      par = para + delta;
      while(lp > 2*M_PI) lp-=2*M_PI;
      while(par > 2*M_PI) par-=2*M_PI;
      fp = 0.;
      return ((par >= fp) && (par <= lp));
    }
  }
  if (para >= (fpar+2*M_PI)) return Standard_True;
  if (para <= lpar) return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : DsgPrs_DiameterPresentation::Add
//purpose  : SZY 12-february-98
//=======================================================================

void DsgPrs_DiameterPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				       const Handle(Prs3d_Drawer)& aDrawer,
				       const TCollection_ExtendedString& aText,
				       const gp_Pnt& AttachmentPoint,
				       const gp_Circ& aCircle,
				       const Standard_Real uFirst,
				       const Standard_Real uLast,
				       const DsgPrs_ArrowSide ArrowPrs,//ArrowSide
				       const Standard_Boolean IsDiamSymbol )
{
  Standard_Real fpara = uFirst;
  Standard_Real lpara = uLast;
  while (lpara > 2.*M_PI) {
    fpara -= 2.*M_PI;
    lpara -= 2.*M_PI;
  }

  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Standard_Real parEndOfArrow = ElCLib::Parameter(aCircle,AttachmentPoint);
  gp_Pnt EndOfArrow;
  gp_Pnt DrawPosition = AttachmentPoint;// point of attachment

  gp_Pnt Center = aCircle.Location();
  gp_Pnt FirstPoint = ElCLib::Value(uFirst, aCircle);
  gp_Pnt SecondPoint = ElCLib::Value(uLast, aCircle);

  if ( !DsgPrs_InDomain(fpara,lpara,parEndOfArrow)) {
    Standard_Real otherpar = parEndOfArrow + M_PI;// not in domain
    if (otherpar > 2*M_PI) otherpar -= 2*M_PI;
    if (DsgPrs_InDomain(fpara,lpara,otherpar)) {
      parEndOfArrow = otherpar; // parameter on circle
      EndOfArrow = ElCLib::Value(parEndOfArrow, aCircle);
    }
    else {
      gp_Dir dir1(gp_Vec(Center, FirstPoint));
      gp_Dir dir2(gp_Vec(Center, SecondPoint));
      gp_Lin L1( Center, dir1 );
      gp_Lin L2( Center, dir2 );
      if(L1.Distance(AttachmentPoint) < L2.Distance(AttachmentPoint))
      {
        EndOfArrow = FirstPoint; //***
        DrawPosition = ElCLib::Value(ElCLib::Parameter( L1, AttachmentPoint ), L1);	
      }
      else
      {
        EndOfArrow = SecondPoint; //***
        DrawPosition = ElCLib::Value(ElCLib::Parameter( L2, AttachmentPoint ), L2);
      }      
    }
  } 
  else {
    EndOfArrow   = ElCLib::Value(parEndOfArrow, aCircle);
    DrawPosition = AttachmentPoint;
  }

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(DrawPosition);
  aPrims->AddVertex(EndOfArrow);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // text
  TCollection_ExtendedString Text = aText;
  if(IsDiamSymbol)
    Text = TCollection_ExtendedString("\330 ") +  Text;//  => \330 | \370?
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), Text, DrawPosition);

  // Add presentation of arrow 
  gp_Dir DirOfArrow(gp_Vec(DrawPosition, EndOfArrow).XYZ()); 
  DsgPrs::ComputeSymbol(aPresentation, LA, EndOfArrow, EndOfArrow, DirOfArrow, DirOfArrow, ArrowPrs);
}
