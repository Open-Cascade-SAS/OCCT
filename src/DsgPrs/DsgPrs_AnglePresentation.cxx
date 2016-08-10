// Created on: 1995-02-07
// Copyright (c) 1995-1999 Matra Datavision
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

#include <DsgPrs_AnglePresentation.hxx>

#include <DsgPrs.hxx>
#include <ElCLib.hxx>
#include <GC_MakeCircle.hxx>
#include <gce_MakePln.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
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
#include <UnitsAPI.hxx>

#include <stdio.h>
//------------------------------------------------------------------------------------------------------------------
// Returns 1 if C is above of CMin; 0 if C is bitween CMin and CMax; -1 if C is Below CMax   
//-----------------------------------------------------------------------------------------------------------------
static Standard_Integer AboveInBelowCone(const gp_Circ &CMax, const gp_Circ &CMin, const gp_Circ &C)
{
  const Standard_Real D = CMax.Location().Distance( CMin.Location() );
  const Standard_Real D1 = CMax.Location().Distance( C.Location() );
  const Standard_Real D2 = CMin.Location().Distance( C.Location() );

  if ( D >= D1 && D >= D2 ) return 0;
  if ( D < D2 && D1 < D2 ) return -1;
  if ( D < D1 && D2 < D1 ) return 1;

  return 0;
}


//==========================================================================
// function : DsgPrs_AnglePresentation::Add
// purpose  : draws the presentation of the cone's angle;
//==========================================================================
void DsgPrs_AnglePresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
                                    const Handle(Prs3d_Drawer)& aDrawer,
                                    const Standard_Real /*aVal*/,
                                    const TCollection_ExtendedString& aText,
                                    const gp_Circ& aCircle,
                                    const gp_Pnt& aPosition,
                                    const gp_Pnt& Apex,
                                    const gp_Circ& VminCircle,
                                    const gp_Circ& VmaxCircle,
                                    const Standard_Real aArrowSize) 
{
  Handle(Prs3d_DimensionAspect) aDimensionAspect = aDrawer->DimensionAspect();

  TCollection_ExtendedString txt(aText);

  const Standard_Real myArrowSize = ( aArrowSize == 0.0 )? (0.1 * aCircle.Radius()) : aArrowSize;

  aDimensionAspect->ArrowAspect()->SetLength(myArrowSize);
  aDrawer->ArrowAspect()->SetLength(myArrowSize);

  Standard_Boolean IsConeTrimmed = Standard_False; 
  gp_Circ myCircle = aCircle;
  if( VminCircle.Radius() > 0.01 ) {
    IsConeTrimmed = Standard_True;
    if( AboveInBelowCone( VmaxCircle, VminCircle, myCircle ) == 1 ) myCircle = VminCircle;
  }
 
  gp_Pnt P1 = ElCLib::Value(0., myCircle);
  gp_Pnt P2 = ElCLib::Value(M_PI, myCircle);

  gce_MakePln mkPln(P1, P2, Apex); // create a plane whitch defines plane for projection aPosition on it

  gp_Vec aVector( mkPln.Value().Location(), aPosition );     //project aPosition on a plane
  gp_Vec Normal = mkPln.Value().Axis().Direction(); 
  Normal = (aVector * Normal) * Normal;

  gp_Pnt aPnt = aPosition;
  aPnt = aPnt.Translated( -Normal );
  
  gp_Pnt tmpPnt = aPnt;

  gp_Pnt AttachmentPnt, OppositePnt;
  if( aPnt.Distance(P1) <  aPnt.Distance(P2) ) {
    AttachmentPnt = P1; 
    OppositePnt = P2; 
  }
  else {
    AttachmentPnt = P2; 
    OppositePnt = P1;
  }

  aPnt = AttachmentPnt ;                          // Creating of circle whitch defines a plane for a dimension arc
  gp_Vec Vec(AttachmentPnt, Apex);                // Dimension arc is a part of the circle 
  Vec.Scale(2.);
  aPnt.Translate(Vec);
  GC_MakeCircle mkCirc(AttachmentPnt, OppositePnt, aPnt); 
  gp_Circ aCircle2 = mkCirc.Value()->Circ();

  Standard_Integer i;
  Standard_Real AttParam = ElCLib::Parameter(aCircle2, AttachmentPnt);  //must be equal to zero (look circle construction)
  Standard_Real OppParam = ElCLib::Parameter(aCircle2, OppositePnt);    
  
  while ( AttParam >= 2. * M_PI ) AttParam -= 2. * M_PI;
  while ( OppParam >= 2. * M_PI ) OppParam -= 2. * M_PI;

  //-------------------------- Compute angle ------------------------
  if( txt.Length() == 0 ) {
    Standard_Real angle = UnitsAPI::CurrentFromLS( Abs( OppParam ),"PLANE ANGLE");
    char res[80]; 
    sprintf(res, "%g", angle );
    txt = TCollection_ExtendedString(res);
  }
  //-----------------------------------------------------------------

  Standard_Boolean IsArrowOut = Standard_True;    //Is arrows inside or outside of the cone
  if( ElCLib::Parameter(aCircle2, tmpPnt) < OppParam )
    if( 2. * myCircle.Radius() > 4. * myArrowSize ) IsArrowOut = Standard_False;  //four times more than an arrow size

  Standard_Real angle = OppParam - AttParam;
  Standard_Real param = AttParam;

  gp_Dir aDir, aDir2;
  if(IsArrowOut) {
    aDir = gp_Dir( ( gp_Vec( ElCLib::Value( AttParam - M_PI / 12., aCircle2 ), AttachmentPnt) ) );
    aDir2 = gp_Dir( ( gp_Vec( ElCLib::Value( OppParam + M_PI / 12., aCircle2 ), OppositePnt) ) );
  }
  else {
    aDir = gp_Dir( ( gp_Vec( ElCLib::Value( AttParam + M_PI / 12., aCircle2 ), AttachmentPnt ) ) );
    aDir2 = gp_Dir( ( gp_Vec( ElCLib::Value( OppParam - M_PI / 12., aCircle2 ), OppositePnt ) ) );
  }

  while ( angle > 2. * M_PI ) angle -= 2. * M_PI;

  Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPolylines(12);
  for( i = 0; i <= 11; i++ )
    aPrims->AddVertex(ElCLib::Value(param + angle/11 * i, aCircle2));
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  DsgPrs::ComputeSymbol(aPresentation, aDimensionAspect, AttachmentPnt,
                        AttachmentPnt, aDir, aDir, DsgPrs_AS_LASTAR);
  DsgPrs::ComputeSymbol(aPresentation, aDimensionAspect, OppositePnt, 
                        OppositePnt, aDir2, aDir2, DsgPrs_AS_LASTAR);

  param = ElCLib::Parameter(aCircle2, tmpPnt);
  tmpPnt = ElCLib::Value(param, aCircle2);
  tmpPnt = tmpPnt.Translated(gp_Vec(0, 0, -2));
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), aDimensionAspect->TextAspect(), txt, tmpPnt);

  angle = 2. * M_PI - param ; 
  if( param > OppParam )
  {
    while ( angle > 2. * M_PI ) angle -= 2. * M_PI;
    aPrims = new Graphic3d_ArrayOfPolylines(12);
    for( i = 11; i >= 0; i-- )
      aPrims->AddVertex(ElCLib::Value(-angle/11 * i, aCircle2));
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }

  if( AboveInBelowCone( VmaxCircle, VminCircle, myCircle ) == 1 && !IsConeTrimmed )         //above
  {
    aPrims = new Graphic3d_ArrayOfPolylines(3);
    aPrims->AddVertex(AttachmentPnt);
    aPrims->AddVertex(Apex);
    aPrims->AddVertex(OppositePnt);
  }
  else
  {
    aPnt = OppositePnt ;
    if ( AboveInBelowCone( VmaxCircle, VminCircle, myCircle ) == 0 ) return;

    gp_Pnt P11 = ElCLib::Value( 0., VmaxCircle );
    gp_Pnt P12 = ElCLib::Value( M_PI, VmaxCircle );

    aPrims = new Graphic3d_ArrayOfSegments(4);
    aPrims->AddVertex(AttachmentPnt);
    aPrims->AddVertex(( aPnt.Distance(P1) < aPnt.Distance(P2) )? P12 : P11);
    aPrims->AddVertex(OppositePnt);
    aPrims->AddVertex(( aPnt.Distance(P1) < aPnt.Distance(P2) )? P11 : P12);
  }
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
}


//==========================================================================
// function : DsgPrs_AnglePresentation::Add
// purpose  : 
//            
//==========================================================================

void DsgPrs_AnglePresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
                                    const Handle(Prs3d_Drawer)& aDrawer,
                                    const Standard_Real theval,
                                    const TCollection_ExtendedString& aText,
                                    const gp_Pnt& CenterPoint,
                                    const gp_Pnt& AttachmentPoint1,
                                    const gp_Pnt& AttachmentPoint2,
                                    const gp_Dir& dir1,
                                    const gp_Dir& dir2,
                                    const gp_Dir& axisdir,
                                    const gp_Pnt& OffsetPoint)
{
  char valcar[80];
  sprintf(valcar,"%5.2f",theval);

  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (aPresentation);
  aGroup->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Ax2 ax(CenterPoint,axisdir,dir1);
  gp_Circ cer(ax,CenterPoint.Distance(OffsetPoint));
  gp_Vec vec1(dir1);
  vec1 *= cer.Radius();
  gp_Vec vec2(dir2);
  vec2 *= cer.Radius();
  gp_Pnt p2 = CenterPoint.Translated(vec2);

  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,p2);
  Standard_Real uco = ElCLib::Parameter(cer,OffsetPoint);

  Standard_Real udeb = uc1;
  Standard_Real ufin = uc2;

  if (uco > ufin) {
    if (Abs(theval)<M_PI) {
      // test if uco is in the opposite sector 
      if (uco > udeb+M_PI && uco < ufin+M_PI) {
        udeb += M_PI;
        ufin += M_PI;
        uc1 = udeb;
        uc2 = ufin;
      }
    }
  }

  if (uco > ufin) {
    if ((uco-uc2) < (uc1-uco+(2.*M_PI))) {
      ufin = uco;
    }
    else {
      udeb = uco - 2.*M_PI;
    }
  }

  const Standard_Real alpha = Abs(ufin-udeb);
  const Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / M_PI));
  const Standard_Real dteta = alpha/(nbp-1);

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(nbp+4,3);
  aPrims->AddBound(nbp);
  for (Standard_Integer i = 1; i<=nbp; i++)
    aPrims->AddVertex(ElCLib::Value(udeb+ dteta*(i-1),cer));

  Prs3d_Text::Draw (aGroup, LA->TextAspect(), aText, OffsetPoint);
  
  Standard_Real length = LA->ArrowAspect()->Length();
  if (length <  Precision::Confusion()) length = 1.e-04;

  gp_Vec vecarr;
  gp_Pnt ptarr;
  ElCLib::D1(uc1,cer,ptarr,vecarr);

  gp_Ax1 ax1(ptarr, axisdir);
  gp_Dir dirarr(-vecarr);

  //calculate angle of rotation
  gp_Pnt ptarr2(ptarr.XYZ() + length*dirarr.XYZ());
  const Standard_Real parcir = ElCLib::Parameter(cer, ptarr2);
  gp_Pnt ptarr3 = ElCLib::Value(parcir, cer);
  gp_Vec v1(ptarr,ptarr2);
  gp_Vec v2(ptarr,ptarr3);
  const Standard_Real beta = v1.Angle(v2);
  dirarr.Rotate(ax1, beta);
  Prs3d_Arrow::Draw (aGroup, ptarr, dirarr, LA->ArrowAspect()->Angle(), length);

  aPrims->AddBound(2);
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(ptarr);

  ElCLib::D1(uc2,cer,ptarr,vecarr);

  ax1.SetLocation(ptarr);
  gp_Dir dirarr2(vecarr);
  dirarr2.Rotate(ax1,-beta);
  Prs3d_Arrow::Draw (aGroup, ptarr, dirarr2, LA->ArrowAspect()->Angle(), length);

  aPrims->AddBound(2);
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(ptarr);

  aGroup->AddPrimitiveArray (aPrims);
}


//==========================================================================
// function : DsgPrs_AnglePresentation::Add
// purpose  : Adds prezentation of angle between two faces
//==========================================================================

void DsgPrs_AnglePresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
                                    const Handle(Prs3d_Drawer)& aDrawer,
                                    const Standard_Real theval,
                                    const TCollection_ExtendedString& aText,
                                    const gp_Pnt& CenterPoint,
                                    const gp_Pnt& AttachmentPoint1,
                                    const gp_Pnt& AttachmentPoint2,
                                    const gp_Dir& dir1,
                                    const gp_Dir& dir2,
                                    const gp_Dir& axisdir,
                                    const Standard_Boolean isPlane,
                                    const gp_Ax1& AxisOfSurf,
                                    const gp_Pnt& OffsetPoint,
                                    const DsgPrs_ArrowSide ArrowPrs )
{
  char valcar[80];
  sprintf(valcar,"%5.2f",theval);
  
  Handle( Prs3d_DimensionAspect ) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup( aPresentation )->SetPrimitivesAspect( LA->LineAspect()->Aspect() );

  gp_Circ AngleCirc, AttachCirc;
  Standard_Real FirstParAngleCirc, LastParAngleCirc, FirstParAttachCirc, LastParAttachCirc;
  gp_Pnt EndOfArrow1, EndOfArrow2, ProjAttachPoint2;
  gp_Dir DirOfArrow1, DirOfArrow2;
  DsgPrs::ComputeFacesAnglePresentation( LA->ArrowAspect()->Length(),
					 theval,
					 CenterPoint,
					 AttachmentPoint1,
					 AttachmentPoint2,
					 dir1,
					 dir2,
					 axisdir,
					 isPlane,
					 AxisOfSurf,
					 OffsetPoint,
					 AngleCirc,
					 FirstParAngleCirc,
					 LastParAngleCirc,
					 EndOfArrow1,
					 EndOfArrow2,
					 DirOfArrow1,
					 DirOfArrow2,
					 ProjAttachPoint2,
					 AttachCirc,
					 FirstParAttachCirc,
					 LastParAttachCirc );
				      
  // Creating the angle's arc or line if null angle
  Handle(Graphic3d_ArrayOfPrimitives) aPrims;
  if (theval > Precision::Angular() && Abs( M_PI-theval ) > Precision::Angular())
  {
    const Standard_Real Alpha  = Abs( LastParAngleCirc - FirstParAngleCirc );
    const Standard_Integer NodeNumber = Max (4 , Standard_Integer (50. * Alpha / M_PI));
    const Standard_Real delta = Alpha / (Standard_Real)( NodeNumber - 1 );

    aPrims = new Graphic3d_ArrayOfPolylines(NodeNumber+4,3);
    aPrims->AddBound(NodeNumber);
    for (Standard_Integer i = 0; i < NodeNumber; i++, FirstParAngleCirc += delta)
      aPrims->AddVertex(ElCLib::Value( FirstParAngleCirc, AngleCirc ));

    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
    aPrims = new Graphic3d_ArrayOfSegments(4);
  }
  else // null angle
  {
    aPrims = new Graphic3d_ArrayOfSegments(6);
    aPrims->AddVertex(OffsetPoint);
    aPrims->AddVertex(EndOfArrow1);
  }

  // Add presentation of arrows
  DsgPrs::ComputeSymbol( aPresentation, LA, EndOfArrow1, EndOfArrow2, DirOfArrow1, DirOfArrow2, ArrowPrs );
  
  // Drawing the text
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, OffsetPoint);
  
  // Line from AttachmentPoint1 to end of Arrow1
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(EndOfArrow1);
  // Line from "projection" of AttachmentPoint2 to end of Arrow2
  aPrims->AddVertex(ProjAttachPoint2);
  aPrims->AddVertex(EndOfArrow2);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // Line or arc from AttachmentPoint2 to its "projection"
  if (AttachmentPoint2.Distance( ProjAttachPoint2 ) > Precision::Confusion())
  {
    if (isPlane)
	{
	  // Creating the line from AttachmentPoint2 to its projection
      aPrims = new Graphic3d_ArrayOfSegments(2);
      aPrims->AddVertex(AttachmentPoint2);
      aPrims->AddVertex(ProjAttachPoint2);
	}      
    else
	{
	  // Creating the arc from AttachmentPoint2 to its projection
      const Standard_Real Alpha = Abs( LastParAttachCirc - FirstParAttachCirc );
	  const Standard_Integer NodeNumber = Max (4 , Standard_Integer (50. * Alpha / M_PI));
      const Standard_Real delta = Alpha / (Standard_Real)( NodeNumber - 1 );

      aPrims = new Graphic3d_ArrayOfPolylines(NodeNumber);
	  for (Standard_Integer i = 0; i < NodeNumber; i++, FirstParAttachCirc += delta)
        aPrims->AddVertex(ElCLib::Value( FirstParAttachCirc, AttachCirc ));
	}
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }
}


//==========================================================================
// function : DsgPrs_AnglePresentation::Add
// purpose  : 
//            
//==========================================================================

void DsgPrs_AnglePresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
                                    const Handle(Prs3d_Drawer)& aDrawer,
                                    const Standard_Real theval,
                                    const TCollection_ExtendedString& aText,
                                    const gp_Pnt& CenterPoint,
                                    const gp_Pnt& AttachmentPoint1,
                                    const gp_Pnt& AttachmentPoint2,
                                    const gp_Dir& dir1,
                                    const gp_Dir& dir2,
                                    const gp_Pnt& OffsetPoint)
{
  char valcar[80];
  sprintf(valcar,"%5.2f",theval);
  
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Dir Norm;
  if (!dir1.IsParallel(dir2, Precision::Angular())) {
    Norm = dir1.Crossed(dir2);
  }
  else {
    gp_Dir dir2B = gp_Dir(gp_Vec(CenterPoint, OffsetPoint));
    Norm = dir1.Crossed(dir2B);
  }

  if (Abs(theval) > M_PI) Norm.Reverse();

  gp_Ax2 ax(CenterPoint,Norm,dir1);
  gp_Circ cer(ax,CenterPoint.Distance(OffsetPoint));
  gp_Vec vec1(dir1);
  vec1 *= cer.Radius();
  gp_Vec vec2(dir2);
  vec2 *= cer.Radius();
  gp_Pnt p2 = CenterPoint.Translated(vec2);

  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,p2);
  Standard_Real uco = ElCLib::Parameter(cer,OffsetPoint);

  Standard_Real udeb = uc1;
  Standard_Real ufin = uc2;

  if (uco > ufin) {
    if (Abs(theval)<M_PI) {
      // test if uco is in the opposite sector 
      if (uco > udeb+M_PI && uco < ufin+M_PI) {
        udeb += M_PI;
        ufin += M_PI;
        uc1 = udeb;
        uc2 = ufin;
      }
    }
  }

  if (uco > ufin) {
    if ((uco-uc2) < (uc1-uco+(2.*M_PI))) {
      ufin = uco;
    }
    else {
      udeb = uco - 2.*M_PI;
    }
  }

  const Standard_Real alpha = Abs(ufin-udeb);
  const Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / M_PI));
  const Standard_Real dteta = alpha/(nbp-1);

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(nbp+4,3);
  aPrims->AddBound(nbp);
  for (Standard_Integer i = 1; i<=nbp; i++)
    aPrims->AddVertex(ElCLib::Value(udeb+ dteta*(i-1),cer));
  
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText,OffsetPoint);
  
  Standard_Real length = LA->ArrowAspect()->Length();
  if (length <  Precision::Confusion()) length = 1.e-04;

  gp_Vec vecarr;
  gp_Pnt ptarr;
  ElCLib::D1(uc1,cer,ptarr,vecarr);

  gp_Ax1 ax1(ptarr, Norm);
  gp_Dir dirarr(-vecarr);
  //calculate the angle of rotation
  gp_Pnt ptarr2(ptarr.XYZ() + length*dirarr.XYZ());
  const Standard_Real parcir = ElCLib::Parameter(cer, ptarr2);
  gp_Pnt ptarr3 = ElCLib::Value(parcir, cer);
  gp_Vec v1(ptarr,ptarr2);
  gp_Vec v2(ptarr,ptarr3);
  const Standard_Real beta = v1.Angle(v2);
  dirarr.Rotate(ax1, beta);
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), ptarr, dirarr, LA->ArrowAspect()->Angle(), length);

  aPrims->AddBound(2);
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(ptarr);

  ElCLib::D1(uc2,cer,ptarr,vecarr);

  ax1.SetLocation(ptarr);
  gp_Dir dirarr2(vecarr);
  dirarr2.Rotate(ax1,  - beta);
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), ptarr, dirarr2, LA->ArrowAspect()->Angle(), length);
  
  aPrims->AddBound(2);
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(ptarr);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
}


//==========================================================================
// function : DsgPrs_AnglePresentation::Add
// purpose  : It is possible to choose the symbol of extremities of the face (arrow, point...)
//==========================================================================

void DsgPrs_AnglePresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
                                    const Handle(Prs3d_Drawer)& aDrawer,
                                    const Standard_Real theval,
                                    const TCollection_ExtendedString& aText,
                                    const gp_Pnt& CenterPoint,
                                    const gp_Pnt& AttachmentPoint1,
                                    const gp_Pnt& AttachmentPoint2,
                                    const gp_Dir& dir1,
                                    const gp_Dir& dir2,
                                    const gp_Pnt& OffsetPoint,
                                    const DsgPrs_ArrowSide ArrowPrs)
{
  char valcar[80];
  sprintf(valcar,"%5.2f",theval);
  
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Dir Norm = dir1.Crossed(dir2);
  if (Abs(theval) > M_PI) Norm.Reverse();

  gp_Ax2 ax(CenterPoint,Norm,dir1);
  gp_Circ cer(ax,CenterPoint.Distance(OffsetPoint));
  gp_Vec vec1(dir1);
  vec1 *= cer.Radius();
  gp_Vec vec2(dir2);
  vec2 *= cer.Radius();
  gp_Pnt p2 = CenterPoint.Translated(vec2);

  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,p2);
  Standard_Real uco = ElCLib::Parameter(cer,OffsetPoint);

  Standard_Real udeb = uc1;
  Standard_Real ufin = uc2;

  if (uco > ufin) {
    if (Abs(theval)<M_PI) {
      // test if uco is in the opposite sector 
      if (uco > udeb+M_PI && uco < ufin+M_PI) {
        udeb += M_PI;
        ufin += M_PI;
        uc1 = udeb;
        uc2 = ufin;
      }
    }
  }

  if (uco > ufin) {
    if ((uco-uc2) < (uc1-uco+(2.*M_PI))) {
      ufin = uco;
    }
    else {
      udeb = uco - 2.*M_PI;
    }
  }

  const Standard_Real alpha = Abs(ufin-udeb);
  const Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / M_PI));
  const Standard_Real dteta = alpha/(nbp-1);

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(nbp+4,3);
  aPrims->AddBound(nbp);
  for (Standard_Integer i = 1; i<=nbp; i++)
    aPrims->AddVertex(ElCLib::Value(udeb+ dteta*(i-1),cer));

  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, OffsetPoint);
  
  Standard_Real length = LA->ArrowAspect()->Length();
  if (length <  Precision::Confusion()) length = 1.e-04;

  // Lines of recall
  gp_Vec vecarr;
  gp_Pnt ptarr;
  ElCLib::D1(uc1,cer,ptarr,vecarr);

  gp_Ax1 ax1(ptarr, Norm);
  gp_Dir dirarr(-vecarr);
  //calculate angle of rotation
  gp_Pnt ptarr2(ptarr.XYZ() + length*dirarr.XYZ());
  const Standard_Real parcir = ElCLib::Parameter(cer, ptarr2);
  gp_Pnt ptarr3 = ElCLib::Value(parcir, cer);
  gp_Vec v1(ptarr,ptarr2 );
  gp_Vec v2(ptarr, ptarr3);
  const Standard_Real beta = v1.Angle(v2);
  dirarr.Rotate(ax1, beta);

  aPrims->AddBound(2);
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(ptarr);
  
  gp_Vec vecarr1;
  gp_Pnt ptarr1;
  ElCLib::D1(uc2,cer,ptarr1,vecarr1);
  ax1.SetLocation(ptarr1);
  gp_Dir dirarr2(vecarr1);
  dirarr2.Rotate(ax1,  - beta);

  aPrims->AddBound(2);
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(ptarr1);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // One traces the arrows
  DsgPrs::ComputeSymbol(aPresentation,LA,ptarr,ptarr1,dirarr,dirarr2,ArrowPrs);
}


//==========================================================================
// function : DsgPrs_AnglePresentation::Add
// purpose  : 
//            
//==========================================================================

void DsgPrs_AnglePresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
                                    const Handle(Prs3d_Drawer)& aDrawer,
                                    const Standard_Real theval,
                                    const gp_Pnt& CenterPoint,
                                    const gp_Pnt& AttachmentPoint1,
                                    const gp_Pnt& AttachmentPoint2,
                                    const gp_Dir& dir1,
                                    const gp_Dir& dir2,
                                    const gp_Pnt& OffsetPoint)
{
  char valcar[80];
  sprintf(valcar,"%5.2f",theval);

  TCollection_AsciiString valas(valcar);
  TCollection_ExtendedString aText(valas);

  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Dir Norm = dir1.Crossed(dir2);
  if (Abs(theval) > M_PI) Norm.Reverse();

  gp_Ax2 ax(CenterPoint,Norm,dir1);
  gp_Circ cer(ax,CenterPoint.Distance(OffsetPoint));
  gp_Vec vec1(dir1);
  vec1 *= cer.Radius();
  gp_Vec vec2(dir2);
  vec2 *= cer.Radius();
  gp_Pnt p2 = CenterPoint.Translated(vec2);

  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,p2);
  Standard_Real uco = ElCLib::Parameter(cer,OffsetPoint);

  Standard_Real udeb = uc1;
  Standard_Real ufin = uc2;

  if (uco > ufin) {
    if (Abs(theval)<M_PI) {
      // test if uco is in the opposite sector 
      if (uco > udeb+M_PI && uco < ufin+M_PI) {
        udeb += M_PI;
        ufin += M_PI;
        uc1 = udeb;
        uc2 = ufin;
      }
    }
  }

  if (uco > ufin) {
    if ((uco-uc2) < (uc1-uco+(2.*M_PI))) {
      ufin = uco;
    }
    else {
      udeb = uco - 2.*M_PI;
    }
  }

  const Standard_Real alpha = Abs(ufin-udeb);
  const Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / M_PI));
  const Standard_Real dteta = alpha/(nbp-1);

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(nbp+4,3);
  aPrims->AddBound(nbp);
  for (Standard_Integer i = 1; i<=nbp; i++)
    aPrims->AddVertex(ElCLib::Value(udeb+ dteta*(i-1),cer));

  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, OffsetPoint);

  Standard_Real length = LA->ArrowAspect()->Length();
  if (length <  Precision::Confusion()) length = 1.e-04;

  gp_Vec vecarr;
  gp_Pnt ptarr;
  ElCLib::D1(uc1,cer,ptarr,vecarr);

  gp_Ax1 ax1(ptarr, Norm);
  gp_Dir dirarr(-vecarr);
  //calculate the angle of rotation
  gp_Pnt ptarr2(ptarr.XYZ() + length*dirarr.XYZ());
  const Standard_Real parcir = ElCLib::Parameter(cer, ptarr2);
  gp_Pnt ptarr3 = ElCLib::Value(parcir, cer);
  gp_Vec v1(ptarr,ptarr2 );
  gp_Vec v2(ptarr, ptarr3);
  const Standard_Real beta = v1.Angle(v2);
  dirarr.Rotate(ax1, beta);

  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), ptarr, dirarr, LA->ArrowAspect()->Angle(), length);

  aPrims->AddBound(2);
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(ptarr);

  ElCLib::D1(uc2,cer,ptarr,vecarr);
  ax1.SetLocation(ptarr);
  gp_Dir dirarr2(vecarr);
  dirarr2.Rotate(ax1, -beta);

  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), ptarr, dirarr2, LA->ArrowAspect()->Angle(), length);
  
  aPrims->AddBound(2);
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(ptarr);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
}

void DsgPrs_AnglePresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
                                    const Handle(Prs3d_Drawer)& aDrawer,
                                    const Standard_Real theval,
                                    const gp_Pnt& CenterPoint,
                                    const gp_Pnt& AttachmentPoint1,
                                    const gp_Ax1& theAxe,
                                    const DsgPrs_ArrowSide ArrowSide)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  gp_Dir dir1(gp_Vec(CenterPoint, AttachmentPoint1));
  gp_Ax2 ax(CenterPoint,theAxe.Direction(),dir1);
  gp_Circ cer(ax,CenterPoint.Distance(AttachmentPoint1));

  const Standard_Integer nbp = Max (4 , Standard_Integer (50. * theval / M_PI));
  const Standard_Real dteta = theval/(nbp-1);

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(nbp);
  for (Standard_Integer i = 1; i<=nbp; i++)
    aPrims->AddVertex(ElCLib::Value(dteta*(i-1),cer));
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,AttachmentPoint1.Rotated(theAxe,theval));

  Standard_Real length = LA->ArrowAspect()->Length();
  if (length < Precision::Confusion()) length = 1.e-04;

  gp_Vec vecarr;
  gp_Pnt ptarr;
  switch(ArrowSide)
  {
    case DsgPrs_AS_FIRSTAR:
    {
      ElCLib::D1(uc1,cer,ptarr,vecarr);
      Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), ptarr, gp_Dir(-vecarr), LA->ArrowAspect()->Angle(), length);
      break;
    }
    case DsgPrs_AS_LASTAR:
    {
      ElCLib::D1(uc2,cer,ptarr,vecarr);
      Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), ptarr, gp_Dir(vecarr), LA->ArrowAspect()->Angle(), length);
      break;
    }
    case DsgPrs_AS_BOTHAR:
    {
      ElCLib::D1(uc1,cer,ptarr,vecarr);
      Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), ptarr, gp_Dir(-vecarr), LA->ArrowAspect()->Angle(), length);
      ElCLib::D1(uc2,cer,ptarr,vecarr);
      Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), ptarr, gp_Dir(vecarr), LA->ArrowAspect()->Angle(), length);
      break;
    }
    default: break;
  }
}
