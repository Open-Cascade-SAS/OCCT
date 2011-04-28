// File:	DsgPrs_AnglePresentation.cxx
// Created:	Tue Feb  7 12:18:14 1995


#include <DsgPrs_AnglePresentation.ixx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <ElCLib.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Prs3d_AngleAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <ElCLib.hxx>

#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_AspectMarker.hxx>
#include <Quantity_Color.hxx>
#include <DsgPrs.hxx>
#include <Precision.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GC_MakeCircle.hxx>
#include <gce_MakePln.hxx>

#include <UnitsAPI.hxx>

//pop pour NT
//#if WNT
#include <stdio.h>
//#endif


static Standard_Integer AboveInBelowCone(gp_Circ CMax, gp_Circ CMin, gp_Circ C);


//==========================================================================
// function : DsgPrs_AnglePresentation::Add
// purpose  : draws the presentation of the cone's angle;
//==========================================================================
void DsgPrs_AnglePresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				    const Handle(Prs3d_Drawer)& aDrawer,
				    const Standard_Real aVal,
				    const TCollection_ExtendedString& aText,
				    const gp_Circ& aCircle,
                                    const gp_Pnt& aPosition,
                                    const gp_Pnt& Apex,
				    const gp_Circ& VminCircle,
				    const gp_Circ& VmaxCircle,
				    const Standard_Real aArrowSize) 
{
  

  
  Handle(Prs3d_AngleAspect) anAngleAspect = aDrawer->AngleAspect();
  Handle(Prs3d_LengthAspect) aLengthAspect = aDrawer->LengthAspect();
  Standard_Real myArrowSize;

  TCollection_ExtendedString txt = aText;
  if( aArrowSize == 0.0 ) myArrowSize =  aCircle.Radius()/ 10.;
  else myArrowSize = aArrowSize;

  anAngleAspect->ArrowAspect()->SetLength(myArrowSize);
  aDrawer->ArrowAspect()->SetLength(myArrowSize);

  Standard_Boolean IsArrowOut = Standard_True;    //Is arrows inside or outside of the cone
  Standard_Boolean IsConeTrimmed = Standard_False; 
  gp_Circ myCircle = aCircle;


  if( VminCircle.Radius() > 0.01 ) {
    IsConeTrimmed = Standard_True;
    if( AboveInBelowCone( VmaxCircle, VminCircle, myCircle ) == 1 ) myCircle =  VminCircle;
  }
 
  gp_Pnt AttachmentPnt;
  gp_Pnt OppositePnt;
  gp_Pnt aPnt, tmpPnt;


  Quantity_Length X,Y,Z;

  Standard_Real param = 0.; //ElCLib::Parameter(myCircle, aPosition);
 
  aPnt = Apex;
  gp_Pnt P1 = ElCLib::Value(0., myCircle);
  gp_Pnt P2 = ElCLib::Value(Standard_PI, myCircle);

  gce_MakePln mkPln(P1, P2,  aPnt);   // create a plane whitch defines plane for projection aPosition on it

  gp_Vec aVector( mkPln.Value().Location(), aPosition );     //project aPosition on a plane
  gp_Vec Normal = mkPln.Value().Axis().Direction(); 
  Normal = (aVector * Normal) * Normal;
  aPnt = aPosition;  

  aPnt =  aPnt.Translated( -Normal );
  
  tmpPnt = aPnt;

  if( aPnt.Distance(P1) <  aPnt.Distance(P2) ){
    AttachmentPnt = P1; 
    OppositePnt = P2; 
  }
  else {
    AttachmentPnt = P2; 
    OppositePnt = P1;
  }
  
  aPnt = AttachmentPnt ;                          // Creating of circle whitch defines a plane for a dimension arc
  gp_Vec Vec(AttachmentPnt, Apex);                // Dimension arc is a part of the circle 
  Vec.Scale(2);
  aPnt.Translate(Vec);
  GC_MakeCircle mkCirc(AttachmentPnt, OppositePnt,  aPnt); 
  gp_Circ  aCircle2 = mkCirc.Value()->Circ();

  Standard_Integer i;
  Standard_Real AttParam = ElCLib::Parameter(aCircle2, AttachmentPnt);  //must be equal to zero (look circle construction)
  Standard_Real OppParam = ElCLib::Parameter(aCircle2, OppositePnt);    
  gp_Dir aDir, aDir2;
  
  while ( AttParam >= 2*Standard_PI ) AttParam -= 2*Standard_PI;
  while ( OppParam >= 2*Standard_PI ) OppParam -= 2*Standard_PI;

  //-------------------------- Compute angle ------------------------
   if( txt.Length() == 0 ) {
     Standard_Real angle = UnitsAPI::CurrentFromLS( Abs( OppParam ),"PLANE ANGLE");
     char res[80]; 
     sprintf(res, "%g", angle );
     txt = TCollection_ExtendedString(res);
   }
  //-----------------------------------------------------------------

   

  if( ElCLib::Parameter(aCircle2, tmpPnt) < OppParam )
 // if( aPosition.Distance( myCircle.Location() ) <= myCircle.Radius() ) 
    if( 2 * myCircle.Radius() > 4 * myArrowSize ) IsArrowOut = Standard_False;  //four times more than an arrow size
    
  
  Graphic3d_Array1OfVertex V(1, 12);
  
  Standard_Real angle;
//  param = ElCLib::Parameter(aCircle2, tmpPnt);
  angle = OppParam - AttParam;
  param = AttParam;

  if(IsArrowOut) {
    aDir = gp_Dir( ( gp_Vec( ElCLib::Value( AttParam - Standard_PI/12, aCircle2 ), AttachmentPnt) ) );
    aDir2 = gp_Dir( ( gp_Vec( ElCLib::Value( OppParam + Standard_PI/12, aCircle2 ), OppositePnt) ) );
  }
  else {
    aDir = gp_Dir( ( gp_Vec( ElCLib::Value( AttParam + Standard_PI/12, aCircle2 ), AttachmentPnt ) ) );
    aDir2 = gp_Dir( ( gp_Vec( ElCLib::Value( OppParam - Standard_PI/12, aCircle2 ),  OppositePnt ) ) );
  }
  
  while ( angle > 2*Standard_PI ) angle -= 2*Standard_PI;
  for( i = 0; i <= 11; i++ ) {                                            //calculating of arc             
    ( ElCLib::Value(param + angle/11 * i, aCircle2) ).Coord(X, Y, Z);
    V(i+1).SetCoord(X, Y, Z);
  }

  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);   //add the arc  

  DsgPrs::ComputeSymbol(aPresentation, anAngleAspect, AttachmentPnt,
			  AttachmentPnt, aDir, aDir, DsgPrs_AS_LASTAR);
  DsgPrs::ComputeSymbol(aPresentation, anAngleAspect, OppositePnt, 
			  OppositePnt, aDir2, aDir2, DsgPrs_AS_LASTAR);

  param = ElCLib::Parameter(aCircle2, tmpPnt);
  tmpPnt = ElCLib::Value(param, aCircle2);
  tmpPnt =  tmpPnt.Translated(gp_Vec(0, 0, -1)*2);
  Prs3d_Text::Draw(aPresentation,aLengthAspect->TextAspect(), txt, tmpPnt);   //add the TCollection_ExtendedString

  angle = 2*Standard_PI - param ; 
  if( param > OppParam ) {
    while ( angle > 2*Standard_PI ) angle -= 2*Standard_PI;
    for( i = 11; i >= 0; i-- ) {       //calculating of arc             
      ( ElCLib::Value(-angle/11 * i, aCircle2) ).Coord(X, Y, Z);
      V(i+1).SetCoord(X, Y, Z);
    }

    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);   //add additional line 
  }
 
  if( AboveInBelowCone( VmaxCircle, VminCircle, myCircle ) == 1 && !IsConeTrimmed ) {         //above
    Graphic3d_Array1OfVertex V2(1,3);    
    AttachmentPnt.Coord(X, Y, Z);
    V2(1).SetCoord(X, Y, Z);
    Apex.Coord(X, Y, Z);
    V2(2).SetCoord(X, Y, Z); 
    OppositePnt.Coord(X, Y, Z);
    V2(3).SetCoord(X, Y, Z); 

    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V2);   //add the additional lines
  }
  else {  
    aPnt = OppositePnt ;
    if ( AboveInBelowCone( VmaxCircle, VminCircle, myCircle ) == 0 ) return;
    Graphic3d_Array1OfVertex V3(1,2);
    gp_Pnt P11 = ElCLib::Value( 0., VmaxCircle );
    gp_Pnt P12 = ElCLib::Value( Standard_PI, VmaxCircle );
  
    AttachmentPnt.Coord(X, Y, Z);
    V3(1).SetCoord(X, Y, Z);
    if( aPnt.Distance(P1) <  aPnt.Distance(P2) )  P12.Coord(X, Y, Z);
    else P11.Coord(X, Y, Z);
    V3(2).SetCoord(X, Y, Z);
      
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V3);
      
    OppositePnt.Coord(X, Y, Z);
    V3(1).SetCoord(X, Y, Z);
    if( aPnt.Distance(P1) <  aPnt.Distance(P2) )  P11.Coord(X, Y, Z);
    else P12.Coord(X, Y, Z);
    V3(2).SetCoord(X, Y, Z);
    
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V3);
  }
}

//------------------------------------------------------------------------------------------------------------------
// Returns 1 if C is above of CMin; 0 if C is bitween CMin and CMax; -1 if C is Below CMax   
//-----------------------------------------------------------------------------------------------------------------
static Standard_Integer AboveInBelowCone(gp_Circ CMax, gp_Circ CMin, gp_Circ C)
{
  Standard_Real D, D1, D2;
  
  D = CMax.Location().Distance( CMin.Location() );
  D1 = CMax.Location().Distance( C.Location() );
  D2 = CMin.Location().Distance( C.Location() );


  if ( D >= D1 && D >= D2 ) return 0;
  if ( D < D2 && D1 < D2 ) return -1;
  if ( D < D1 && D2 < D1 ) return 1;

  return 0;
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
				    const gp_Pnt& OffsetPoint) {
  char valcar[80];
  sprintf(valcar,"%5.2f",theval);
  
  Handle(Prs3d_AngleAspect) LA = aDrawer->AngleAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Ax2 ax(CenterPoint,axisdir,dir1);
  gp_Circ cer(ax,CenterPoint.Distance(OffsetPoint));
  gp_Vec vec1(dir1);
  vec1 *= cer.Radius();
#ifdef DEB
  gp_Pnt p1 =
#endif
              CenterPoint.Translated(vec1);
  gp_Vec vec2(dir2);
  vec2 *= cer.Radius();
  gp_Pnt p2 = CenterPoint.Translated(vec2);

  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,p2);
  Standard_Real uco = ElCLib::Parameter(cer,OffsetPoint);

  Standard_Real udeb = uc1;
  Standard_Real ufin = uc2;

  if (uco > ufin) {
    if (Abs(theval)<PI) {
      // test if uco is in the opposite sector 
      if (uco > udeb+PI && uco < ufin+PI){
	udeb = udeb + PI;
	ufin = ufin + PI;
	uc1  = udeb;
	uc2  = ufin;
      }
    }
  }

  if (uco > ufin) {
    if ((uco-uc2) < (uc1-uco+(2*PI))) {
      ufin = uco;
    }
    else {
      udeb = uco - 2*PI;
    }
  }

  Standard_Real alpha = Abs(ufin-udeb);
  Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / PI));
  Graphic3d_Array1OfVertex V(1,nbp);
  Standard_Real dteta = alpha/(nbp-1);
  gp_Pnt ptcur;
  for (Standard_Integer i = 1; i<=nbp; i++) {
    ptcur =  ElCLib::Value(udeb+ dteta*(i-1),cer);
    V(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
  }
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,OffsetPoint);
  
  gp_Vec vecarr;
  gp_Pnt ptarr;
  ElCLib::D1(uc1,cer,ptarr,vecarr);
  gp_Ax1 ax1(ptarr, axisdir);
  gp_Dir dirarr(-vecarr);
  //calculate angle of rotation
  Standard_Real beta(0.);
  Standard_Real length = LA->ArrowAspect()->Length();
  if (length <  Precision::Confusion()) length = 1.e-04;
  gp_Pnt ptarr2(ptarr.XYZ() + length*dirarr.XYZ());
  Standard_Real parcir = ElCLib::Parameter(cer, ptarr2);
  gp_Pnt ptarr3 = ElCLib::Value(parcir, cer);
  gp_Vec v1(ptarr,ptarr2 );
  gp_Vec v2(ptarr, ptarr3);
  beta = v1.Angle(v2);
  dirarr.Rotate(ax1, beta);
  Prs3d_Arrow::Draw(aPresentation,
		    ptarr,
		    dirarr,
		    LA->ArrowAspect()->Angle(),
		    length);  

  Graphic3d_Array1OfVertex Vrap(1,2);
  Vrap(1).SetCoord(AttachmentPoint1.X(),
		   AttachmentPoint1.Y(),
		   AttachmentPoint1.Z());
  Vrap(2).SetCoord(ptarr.X(),ptarr.Y(),ptarr.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrap);
  
  ElCLib::D1(uc2,cer,ptarr,vecarr);
  ax1.SetLocation(ptarr);
  gp_Dir dirarr2(vecarr);
  dirarr2.Rotate(ax1,-beta);
  Prs3d_Arrow::Draw(aPresentation,
		    ptarr,
		    dirarr2,
		    LA->ArrowAspect()->Angle(),
		    length);  

  Vrap(1).SetCoord(AttachmentPoint2.X(),
		   AttachmentPoint2.Y(),
		   AttachmentPoint2.Z());
  Vrap(2).SetCoord(ptarr.X(),ptarr.Y(),ptarr.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrap);
}


//==========================================================================
// function : DsgPrs_AnglePresentation::Add
// purpose  : Adds prezentation of angle between two faces
//==========================================================================

void DsgPrs_AnglePresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
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
  sprintf( valcar, "%5.2f", theval );
  
  Handle( Prs3d_AngleAspect ) LA = aDrawer->AngleAspect();
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
				      
  Graphic3d_Array1OfVertex Vrap(1,2);

  // Creating the angle's arc or line if null angle
  if (theval > Precision::Angular() && Abs( PI-theval ) > Precision::Angular())
    {
      Standard_Real Alpha  = Abs( LastParAngleCirc - FirstParAngleCirc );
      Standard_Integer NodeNumber = Max (4 , Standard_Integer (50. * Alpha / PI));
      Graphic3d_Array1OfVertex ApproxArc( 0, NodeNumber-1 );
      Standard_Real delta = Alpha / (Standard_Real)( NodeNumber - 1 );
      gp_Pnt CurPnt;
      for (Standard_Integer i = 0 ; i < NodeNumber; i++)
	{
	  CurPnt =  ElCLib::Value( FirstParAngleCirc, AngleCirc );
	  ApproxArc(i).SetCoord( CurPnt.X(), CurPnt.Y(), CurPnt.Z() );
	  FirstParAngleCirc += delta ;
	}
      Prs3d_Root::CurrentGroup( aPresentation )->Polyline( ApproxArc );
    }
  else // null angle
    {
      Vrap(1).SetCoord( OffsetPoint.X(),
			OffsetPoint.Y(),
			OffsetPoint.Z());
      Vrap(2).SetCoord( EndOfArrow1.X(), EndOfArrow1.Y(), EndOfArrow1.Z() );
      Prs3d_Root::CurrentGroup( aPresentation )->Polyline( Vrap );
    }

  // Add presentation of arrows
  DsgPrs::ComputeSymbol( aPresentation, LA, EndOfArrow1, EndOfArrow2, DirOfArrow1, DirOfArrow2, ArrowPrs );
  
  // Drawing the text
  Prs3d_Text::Draw( aPresentation, LA->TextAspect(), aText, OffsetPoint );
  
  // Line from AttachmentPoint1 to end of Arrow1
  Vrap(1).SetCoord(AttachmentPoint1.X(),
		   AttachmentPoint1.Y(),
		   AttachmentPoint1.Z());
  Vrap(2).SetCoord( EndOfArrow1.X(), EndOfArrow1.Y(), EndOfArrow1.Z() );
  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( Vrap );
  
  // Line or arc from AttachmentPoint2 to its "projection"
  if (AttachmentPoint2.Distance( ProjAttachPoint2 ) > Precision::Confusion())
    {
      if (isPlane)
	{
	  // Creating the line from AttachmentPoint2 to its projection
	  Vrap(1).SetCoord( AttachmentPoint2.X(),
			    AttachmentPoint2.Y(),
			    AttachmentPoint2.Z() );
	  Vrap(2).SetCoord( ProjAttachPoint2.X(),
			    ProjAttachPoint2.Y(),
			    ProjAttachPoint2.Z() );
	  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( Vrap );
	}      
      else
	{
	  // Creating the arc from AttachmentPoint2 to its projection
	  Standard_Real Alpha  = Abs( LastParAttachCirc - FirstParAttachCirc );
	  Standard_Integer NodeNumber = Max (4 , Standard_Integer (50. * Alpha / PI));
	  Graphic3d_Array1OfVertex ApproxArc( 0, NodeNumber-1 );
	  Standard_Real delta = Alpha / (Standard_Real)( NodeNumber - 1 );
	  gp_Pnt CurPnt;
	  for (Standard_Integer i = 0 ; i < NodeNumber; i++)
	    {
	      CurPnt =  ElCLib::Value( FirstParAttachCirc, AttachCirc );
	      ApproxArc(i).SetCoord( CurPnt.X(), CurPnt.Y(), CurPnt.Z() );
	      FirstParAttachCirc += delta ;
	    }
	  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( ApproxArc );
	}
    }
  // Line from "projection" of AttachmentPoint2 to end of Arrow2
  Vrap(1).SetCoord( ProjAttachPoint2.X(),
		    ProjAttachPoint2.Y(),
		    ProjAttachPoint2.Z() );
  Vrap(2).SetCoord( EndOfArrow2.X(), EndOfArrow2.Y(), EndOfArrow2.Z());
  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( Vrap );
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
				    const gp_Pnt& OffsetPoint) {
  char valcar[80];
  sprintf(valcar,"%5.2f",theval);
  
  Handle(Prs3d_AngleAspect) LA = aDrawer->AngleAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  gp_Dir Norm;
  if (!dir1.IsParallel(dir2, Precision::Angular())) {
    Norm = dir1.Crossed(dir2);
  }
  else {
    gp_Dir dir2B = gp_Dir(gp_Vec(CenterPoint, OffsetPoint));
    Norm = dir1.Crossed(dir2B);
  }

  if (Abs(theval) > PI) Norm.Reverse();

  gp_Ax2 ax(CenterPoint,Norm,dir1);
  gp_Circ cer(ax,CenterPoint.Distance(OffsetPoint));
  gp_Vec vec1(dir1);
  vec1 *= cer.Radius();
#ifdef DEB
  gp_Pnt p1 =
#endif
              CenterPoint.Translated(vec1);
  gp_Vec vec2(dir2);
  vec2 *= cer.Radius();
  gp_Pnt p2 = CenterPoint.Translated(vec2);

  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,p2);
  Standard_Real uco = ElCLib::Parameter(cer,OffsetPoint);

  Standard_Real udeb = uc1;
  Standard_Real ufin = uc2;

  if (uco > ufin) {
    if (Abs(theval)<PI) {
      // test if uco is in the opposite sector 
      if (uco > udeb+PI && uco < ufin+PI){
	udeb = udeb + PI;
	ufin = ufin + PI;
	uc1  = udeb;
	uc2  = ufin;
      }
    }
  }

  if (uco > ufin) {
    if ((uco-uc2) < (uc1-uco+(2*PI))) {
      ufin = uco;
    }
    else {
      udeb = uco - 2*PI;
    }
  }

  Standard_Real alpha = Abs(ufin-udeb);
  Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / PI));
  Graphic3d_Array1OfVertex V(1,nbp);
  Standard_Real dteta = alpha/(nbp-1);
  gp_Pnt ptcur;
  for (Standard_Integer i = 1; i<=nbp; i++) {
    ptcur =  ElCLib::Value(udeb+ dteta*(i-1),cer);
    V(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
  }
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,OffsetPoint);
  
  gp_Vec vecarr;
  gp_Pnt ptarr;
  ElCLib::D1(uc1,cer,ptarr,vecarr);
  gp_Ax1 ax1(ptarr, Norm);
  gp_Dir dirarr(-vecarr);
  //calculate the angle of rotation
  Standard_Real beta;
  Standard_Real length = LA->ArrowAspect()->Length();
  if (length <  Precision::Confusion()) length = 1.e-04;
  gp_Pnt ptarr2(ptarr.XYZ() + length*dirarr.XYZ());
  Standard_Real parcir = ElCLib::Parameter(cer, ptarr2);
  gp_Pnt ptarr3 = ElCLib::Value(parcir, cer);
  gp_Vec v1(ptarr,ptarr2 );
  gp_Vec v2(ptarr, ptarr3);
  beta = v1.Angle(v2);
  dirarr.Rotate(ax1, beta);
  Prs3d_Arrow::Draw(aPresentation,
		    ptarr,
		    dirarr,
		    LA->ArrowAspect()->Angle(),
		    length);  
  Graphic3d_Array1OfVertex Vrap(1,2);
  Vrap(1).SetCoord(AttachmentPoint1.X(),
		   AttachmentPoint1.Y(),
		   AttachmentPoint1.Z());
  Vrap(2).SetCoord(ptarr.X(),ptarr.Y(),ptarr.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrap);
  
  ElCLib::D1(uc2,cer,ptarr,vecarr);
  ax1.SetLocation(ptarr);
  gp_Dir dirarr2(vecarr);
  dirarr2.Rotate(ax1,  - beta);
  Prs3d_Arrow::Draw(aPresentation,
		    ptarr,
		    dirarr2,
		    LA->ArrowAspect()->Angle(),
		    length);  
  
  Vrap(1).SetCoord(AttachmentPoint2.X(),
		   AttachmentPoint2.Y(),
		   AttachmentPoint2.Z());
  Vrap(2).SetCoord(ptarr.X(),ptarr.Y(),ptarr.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrap);
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
  
  Handle(Prs3d_AngleAspect) LA = aDrawer->AngleAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  gp_Dir Norm = dir1.Crossed(dir2);

  if (Abs(theval) > PI) Norm.Reverse();

  gp_Ax2 ax(CenterPoint,Norm,dir1);
  gp_Circ cer(ax,CenterPoint.Distance(OffsetPoint));
  gp_Vec vec1(dir1);
  vec1 *= cer.Radius();
#ifdef DEB
  gp_Pnt p1 =
#endif
              CenterPoint.Translated(vec1);
  gp_Vec vec2(dir2);
  vec2 *= cer.Radius();
  gp_Pnt p2 = CenterPoint.Translated(vec2);

  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,p2);
  Standard_Real uco = ElCLib::Parameter(cer,OffsetPoint);

  Standard_Real udeb = uc1;
  Standard_Real ufin = uc2;

  if (uco > ufin) {
    if (Abs(theval)<PI) {
      // test if uco is in the opposite sector 
      if (uco > udeb+PI && uco < ufin+PI){
	udeb = udeb + PI;
	ufin = ufin + PI;
	uc1  = udeb;
	uc2  = ufin;
      }
    }
  }

  if (uco > ufin) {
    if ((uco-uc2) < (uc1-uco+(2*PI))) {
      ufin = uco;
    }
    else {
      udeb = uco - 2*PI;
    }
  }

  Standard_Real alpha = Abs(ufin-udeb);
  Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / PI));
  Graphic3d_Array1OfVertex V(1,nbp);
  Standard_Real dteta = alpha/(nbp-1);
  gp_Pnt ptcur;
  for (Standard_Integer i = 1; i<=nbp; i++) {
    ptcur =  ElCLib::Value(udeb+ dteta*(i-1),cer);
    V(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
  }
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,OffsetPoint);
  
// Lines of recall
  gp_Vec vecarr;
  gp_Pnt ptarr;
  ElCLib::D1(uc1,cer,ptarr,vecarr);
  gp_Ax1 ax1(ptarr, Norm);
  gp_Dir dirarr(-vecarr);
  //calculate angle of rotation
  Standard_Real beta(0.);
  Standard_Real length = LA->ArrowAspect()->Length();
  if (length <  Precision::Confusion()) length = 1.e-04;
  gp_Pnt ptarr2(ptarr.XYZ() + length*dirarr.XYZ());
  Standard_Real parcir = ElCLib::Parameter(cer, ptarr2);
  gp_Pnt ptarr3 = ElCLib::Value(parcir, cer);
  gp_Vec v1(ptarr,ptarr2 );
  gp_Vec v2(ptarr, ptarr3);
  beta = v1.Angle(v2);
  dirarr.Rotate(ax1, beta);

  Graphic3d_Array1OfVertex Vrap(1,2);
  Vrap(1).SetCoord(AttachmentPoint1.X(),
		   AttachmentPoint1.Y(),
		   AttachmentPoint1.Z());
  Vrap(2).SetCoord(ptarr.X(),ptarr.Y(),ptarr.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrap);
  
  gp_Vec vecarr1;
  gp_Pnt ptarr1;
  ElCLib::D1(uc2,cer,ptarr1,vecarr1);
  ax1.SetLocation(ptarr1);
  gp_Dir dirarr2(vecarr1);
  dirarr2.Rotate(ax1,  - beta);

  
  Vrap(1).SetCoord(AttachmentPoint2.X(),
		   AttachmentPoint2.Y(),
		   AttachmentPoint2.Z());
  Vrap(2).SetCoord(ptarr1.X(),ptarr1.Y(),ptarr1.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrap);


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
				    const gp_Pnt& OffsetPoint) {


  char valcar[80];
  sprintf(valcar,"%5.2f",theval);
  TCollection_AsciiString valas(valcar);
  TCollection_ExtendedString aText(valas);

  Handle(Prs3d_AngleAspect) LA = aDrawer->AngleAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  gp_Dir Norm = dir1.Crossed(dir2);

  if (Abs(theval) > PI) Norm.Reverse();

  gp_Ax2 ax(CenterPoint,Norm,dir1);
  gp_Circ cer(ax,CenterPoint.Distance(OffsetPoint));
  gp_Vec vec1(dir1);
  vec1 *= cer.Radius();
#ifdef DEB
  gp_Pnt p1 =
#endif
              CenterPoint.Translated(vec1);
  gp_Vec vec2(dir2);
  vec2 *= cer.Radius();
  gp_Pnt p2 = CenterPoint.Translated(vec2);

  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,p2);
  Standard_Real uco = ElCLib::Parameter(cer,OffsetPoint);

  Standard_Real udeb = uc1;
  Standard_Real ufin = uc2;

  if (uco > ufin) {
    if (Abs(theval)<PI) {
      // test if uco is in the opposite sector 
      if (uco > udeb+PI && uco < ufin+PI){
	udeb = udeb + PI;
	ufin = ufin + PI;
	uc1  = udeb;
	uc2  = ufin;
      }
    }
  }

  if (uco > ufin) {
    if ((uco-uc2) < (uc1-uco+(2*PI))) {
      ufin = uco;
    }
    else {
      udeb = uco - 2*PI;
    }
  }

  Standard_Real alpha = Abs(ufin-udeb);
  Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / PI));
  Graphic3d_Array1OfVertex V(1,nbp);
  Standard_Real dteta = alpha/(nbp-1);
  gp_Pnt ptcur;
  for (Standard_Integer i = 1; i<=nbp; i++) {
    ptcur =  ElCLib::Value(udeb+ dteta*(i-1),cer);
    V(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
  }
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,OffsetPoint);

  gp_Vec vecarr;
  gp_Pnt ptarr;
  ElCLib::D1(uc1,cer,ptarr,vecarr);
  gp_Ax1 ax1(ptarr, Norm);
  gp_Dir dirarr(-vecarr);
  //calculate the angle of rotation
  Standard_Real beta;
  Standard_Real length = LA->ArrowAspect()->Length();
  if (length <  Precision::Confusion()) length = 1.e-04;
  gp_Pnt ptarr2(ptarr.XYZ() + length*dirarr.XYZ());
  Standard_Real parcir = ElCLib::Parameter(cer, ptarr2);
  gp_Pnt ptarr3 = ElCLib::Value(parcir, cer);
  gp_Vec v1(ptarr,ptarr2 );
  gp_Vec v2(ptarr, ptarr3);
  beta = v1.Angle(v2);
  dirarr.Rotate(ax1, beta);
  Prs3d_Arrow::Draw(aPresentation,
		    ptarr,
		    dirarr,
		    LA->ArrowAspect()->Angle(),
		    length);  

  Graphic3d_Array1OfVertex Vrap(1,2);
  Vrap(1).SetCoord(AttachmentPoint1.X(),
		   AttachmentPoint1.Y(),
		   AttachmentPoint1.Z());
  Vrap(2).SetCoord(ptarr.X(),ptarr.Y(),ptarr.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrap);
  
  ElCLib::D1(uc2,cer,ptarr,vecarr);
  ax1.SetLocation(ptarr);
  gp_Dir dirarr2(vecarr);
  dirarr2.Rotate(ax1,  - beta);
  Prs3d_Arrow::Draw(aPresentation,
		    ptarr,
		    dirarr2,
		    LA->ArrowAspect()->Angle(),
		    length); 
  
  Vrap(1).SetCoord(AttachmentPoint2.X(),
		   AttachmentPoint2.Y(),
		   AttachmentPoint2.Z());
  Vrap(2).SetCoord(ptarr.X(),ptarr.Y(),ptarr.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrap);
}

void DsgPrs_AnglePresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				    const Handle(Prs3d_Drawer)& aDrawer,
				    const Standard_Real theval,
				    const gp_Pnt& CenterPoint,
				    const gp_Pnt& AttachmentPoint1,
				    const gp_Ax1& theAxe,
				    const DsgPrs_ArrowSide ArrowSide) 
{
  Handle(Prs3d_AngleAspect) LA = aDrawer->AngleAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  gp_Dir dir1(gp_Vec(CenterPoint, AttachmentPoint1));
  gp_Ax2 ax(CenterPoint,theAxe.Direction(),dir1);
  gp_Circ cer(ax,CenterPoint.Distance(AttachmentPoint1));

  Standard_Integer nbp = Max (4 , Standard_Integer (50. * theval / PI));
  Graphic3d_Array1OfVertex V(1,nbp);
  Standard_Real dteta = theval/(nbp-1);
  gp_Pnt ptcur;
  for (Standard_Integer i = 1; i<=nbp; i++) {
    ptcur =  ElCLib::Value(dteta*(i-1),cer);
    V(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
  }
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,AttachmentPoint1.Rotated(theAxe,theval));

  gp_Vec vecarr;
  gp_Pnt ptarr;
  Standard_Real length = LA->ArrowAspect()->Length();
  if (length <  Precision::Confusion()) length = 1.e-04;

  switch(ArrowSide) {
  case DsgPrs_AS_NONE:
    {
      break;
    }
  case DsgPrs_AS_FIRSTAR:
    {
      ElCLib::D1(uc1,cer,ptarr,vecarr);
      Prs3d_Arrow::Draw(aPresentation,
			ptarr,
			gp_Dir(-vecarr),
			LA->ArrowAspect()->Angle(),
			length);
      break;
    }
  case DsgPrs_AS_LASTAR:
    {
      ElCLib::D1(uc2,cer,ptarr,vecarr);
      Prs3d_Arrow::Draw(aPresentation,
			ptarr,
			gp_Dir(vecarr),
			LA->ArrowAspect()->Angle(),
			length);
      break;
    }
  case DsgPrs_AS_BOTHAR:
    {
      ElCLib::D1(uc1,cer,ptarr,vecarr);
      Prs3d_Arrow::Draw(aPresentation,
			ptarr,
			gp_Dir(-vecarr),
			LA->ArrowAspect()->Angle(),
			length);
      ElCLib::D1(uc2,cer,ptarr,vecarr);
      Prs3d_Arrow::Draw(aPresentation,
			ptarr,
			gp_Dir(vecarr),
			LA->ArrowAspect()->Angle(),
			length);
      break;
    }
#ifndef DEB
  default:
      break;
#endif
  }
}

