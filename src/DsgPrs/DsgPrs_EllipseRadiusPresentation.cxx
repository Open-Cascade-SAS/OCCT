// Created on: 1998-01-26
// Created by: Sergey ZARITCHNY
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

#include <DsgPrs_EllipseRadiusPresentation.hxx>

#include <DsgPrs.hxx>
#include <ElCLib.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void DsgPrs_EllipseRadiusPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					    const Handle(Prs3d_Drawer)& aDrawer,
					    const Standard_Real theval,
					    const TCollection_ExtendedString & aText,
					    const gp_Pnt & aPosition,
					    const gp_Pnt & anEndOfArrow,
					    const gp_Pnt & aCenter,
					    const Standard_Boolean IsMaxRadius,
					    const DsgPrs_ArrowSide ArrowPrs)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
 
  const Standard_Real dist = aCenter.Distance( aPosition );
  const Standard_Boolean inside = ( dist <= theval );
  gp_Pnt EndPoint(inside? anEndOfArrow : aPosition);

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aCenter);
  aPrims->AddVertex(EndPoint);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // value
  TCollection_ExtendedString Text(IsMaxRadius? "a = " : "b = ");
  Text += aText;
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), Text, aPosition);

  // arrows
  gp_Dir arrdir( gp_Vec( aCenter, anEndOfArrow));
  if (!inside) arrdir.Reverse();

  DsgPrs::ComputeSymbol(aPresentation, LA, anEndOfArrow, anEndOfArrow, arrdir, arrdir, ArrowPrs );
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void DsgPrs_EllipseRadiusPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					    const Handle(Prs3d_Drawer)& aDrawer,
					    const Standard_Real theval,
					    const TCollection_ExtendedString & aText,
					    const gp_Elips & anEllipse,
					    const gp_Pnt & aPosition,
					    const gp_Pnt & anEndOfArrow,
					    const gp_Pnt & aCenter,
					    const Standard_Real uFirst,
					    const Standard_Boolean IsInDomain,
					    const Standard_Boolean IsMaxRadius,
					    const DsgPrs_ArrowSide ArrowPrs)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  if(!IsInDomain)
  {
    const Standard_Real uLast = ElCLib::Parameter ( anEllipse, anEndOfArrow );
    const Standard_Real Alpha = DsgPrs::DistanceFromApex(anEllipse, anEndOfArrow, uFirst);//length of ellipse arc
    gp_Vec Vapex(aCenter, ElCLib::Value( uLast, anEllipse )) ;
    gp_Vec Vpnt(aCenter,  ElCLib::Value( uFirst, anEllipse )) ;
    gp_Dir dir(Vpnt ^ Vapex);
	Standard_Real parFirst = anEllipse.Position().Direction().IsOpposite( dir, Precision::Angular())? uLast : uFirst;
    const Standard_Integer NodeNumber = Max (4 , Standard_Integer (50. * Alpha / M_PI));
    const Standard_Real delta = Alpha / ( NodeNumber - 1 );

    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(NodeNumber);
    for (Standard_Integer i = 0 ; i < NodeNumber; i++, parFirst += delta)
	  aPrims->AddVertex(ElCLib::Value( parFirst, anEllipse ));
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }
  DsgPrs_EllipseRadiusPresentation::Add(aPresentation, aDrawer, theval, aText,
                                        aPosition, anEndOfArrow, aCenter, IsMaxRadius, ArrowPrs);
}


//=======================================================================
//function : Add
//purpose  : // for offset curve
//=======================================================================

void DsgPrs_EllipseRadiusPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					    const Handle(Prs3d_Drawer)& aDrawer,
					    const Standard_Real theval,
					    const TCollection_ExtendedString & aText,
					    const Handle(Geom_OffsetCurve) & aCurve,
					    const gp_Pnt & aPosition,
					    const gp_Pnt & anEndOfArrow,
					    const gp_Pnt & aCenter,
					    const Standard_Real uFirst,
					    const Standard_Boolean IsInDomain,
					    const Standard_Boolean IsMaxRadius,
					    const DsgPrs_ArrowSide ArrowPrs)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  if(!IsInDomain)
  {
    if(!aCurve->IsCN(1)) return ;
    gp_Elips aBEllipse = Handle(Geom_Ellipse)::DownCast(aCurve->BasisCurve ())->Elips();
    const Standard_Real Offset = aCurve->Offset();
    aBEllipse.SetMajorRadius(aBEllipse.MajorRadius() + Offset);
    aBEllipse.SetMinorRadius(aBEllipse.MinorRadius() + Offset);
    const Standard_Real uLast = ElCLib::Parameter ( aBEllipse, anEndOfArrow );
    const Standard_Real Alpha = DsgPrs::DistanceFromApex(aBEllipse, anEndOfArrow, uFirst);//length of ellipse arc
    gp_Pnt p1;
    aCurve->D0(uFirst, p1);
    gp_Vec Vapex(aCenter, anEndOfArrow) ;
    gp_Vec Vpnt (aCenter, p1) ;
    gp_Dir dir(Vpnt ^ Vapex);
	Standard_Real parFirst = aCurve->Direction().IsOpposite( dir, Precision::Angular())? uLast : uFirst;
    const Standard_Integer NodeNumber = Max (4 , Standard_Integer (50. * Alpha / M_PI));
    const Standard_Real delta = Alpha / ( NodeNumber - 1 );

    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(NodeNumber);
    for (Standard_Integer i = 0 ; i < NodeNumber; i++, parFirst += delta)
	{
	  aCurve->D0( parFirst, p1 );
	  aPrims->AddVertex(p1);
	}
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }
  DsgPrs_EllipseRadiusPresentation::Add(aPresentation, aDrawer, theval, aText,
                                        aPosition, anEndOfArrow, aCenter, IsMaxRadius, ArrowPrs);
}
