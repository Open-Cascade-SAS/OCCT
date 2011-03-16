#include <AIS2D.hxx>
#include <Graphic2d_Primitive.hxx>
#include <Graphic2d_DisplayList.hxx>
#include <Graphic2d_Circle.hxx>
#include <Graphic2d_CircleMarker.hxx>
#include <Graphic2d_Ellips.hxx>
#include <Graphic2d_EllipsMarker.hxx>
#include <Graphic2d_InfiniteLine.hxx>
#include <Graphic2d_Marker.hxx>
#include <Graphic2d_VectorialMarker.hxx>
#include <Graphic2d_PolylineMarker.hxx>
#include <Graphic2d_SetOfMarkers.hxx>
#include <Graphic2d_SetOfSegments.hxx>
#include <Graphic2d_Segment.hxx>
#include <Graphic2d_Polyline.hxx>
#include <Graphic2d_SetOfPolylines.hxx>
#include <Graphic2d_Image.hxx>
#include <Graphic2d_ImageFile.hxx>
#include <Graphic2d_Text.hxx>
#include <Graphic2d_FramedText.hxx>
#include <Graphic2d_HidingText.hxx>
#include <GGraphic2d_Curve.hxx>
#include <GGraphic2d_SetOfCurves.hxx>
#include <Graphic2d_View.hxx>
#include <Graphic2d_Paragraph.hxx>

#include <V2d_Viewer.hxx>

#include <Quantity_Color.hxx>

#include <Prs2d_AspectRoot.hxx>
#include <Prs2d_AspectLine.hxx>
#include <Prs2d_AspectText.hxx>
#include <Prs2d_Angle.hxx>

#include <AIS2D_InteractiveContext.hxx>
#include <AIS2D_InteractiveObject.hxx>
#include <AIS2D_KindOfPrimitive.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Standard_Stream.hxx>
Standard_Boolean AIS2D::Save( const Handle(AIS2D_InteractiveContext)& aCntx,
                              const Standard_CString aFile ) {


 if ( !aCntx.IsNull() ) {

  Handle( Graphic2d_DisplayList ) theDL = aCntx->CurrentViewer()->View()->DisplayList();
 
  if ( !theDL->IsEmpty() ) {
   
   Aspect_FStream ofs = new ofstream;
   ofs->open( aFile, ios::out );

   int LnOfDL = theDL->Length();
   TCollection_AsciiString thestr;
   for ( int i = 1; i <= LnOfDL; ++i )
   {
      Handle(AIS2D_InteractiveObject) theIO = 
	      Handle(AIS2D_InteractiveObject)::DownCast( theDL->Value(i) ); 
      theIO->Save( ofs );

  /*   int lnOfPr = theIO->Length();

     for ( int j = 1; j <= lnOfPr; j++ ) {

       Handle(Graphic2d_Primitive) thePrm = theIO->Primitive( j );
       Handle( Prs2d_AspectRoot ) theAsp = theIO->GetAspect( thePrm );

       if ( thePrm->IsKind( STANDARD_TYPE( Graphic2d_Line ) ) ) {
         Handle(Prs2d_AspectLine) theAL = Handle(Prs2d_AspectLine)::DownCast( theAsp );
         Quantity_Color theColor;
         Aspect_TypeOfLine theTOL;
         Aspect_WidthOfLine theWOL;
         theAL->ValuesOfLine( theColor, theTOL, theWOL );
         Standard_Real R1, R2, R3;
         theColor.Values( R1, R2, R3, Quantity_TOC_RGB );
         
       } else if ( thePrm->IsKind( STANDARD_TYPE( Graphic2d_Paragraph ) ) ) {
       
       } else if ( thePrm->IsKind( STANDARD_TYPE( Graphic2d_Image ) ) ) {

       } else if ( thePrm->IsKind( STANDARD_TYPE( Graphic2d_ImageFile ) ) ) {
       
       } else if ( thePrm->IsKind( STANDARD_TYPE( Graphic2d_Text ) ) ) {     

         Handle(Prs2d_AspectText) theAT = Handle(Prs2d_AspectText)::DownCast( theAsp );
         Quantity_Color aColor;
         Aspect_FontStyle aFont;
         Quantity_PlaneAngle aSlant;
         Aspect_TypeOfText aType; 
         Standard_Boolean isUnderlined;
         theAT->Values( aColor, aFont, aSlant, aType, isUnderlined );
       }    
    }*/
   }
     ofs->close();
  } //if sequence of displyed objects isn't empty
 } // if context isn't NULL
    return Standard_False;
}

 Handle (AIS2D_InteractiveObject)
 AIS2D::Retrieve( const Handle(AIS2D_InteractiveContext)& /*aCntx*/,
                  const Standard_CString aFile )
{
	char buf[100];
	Aspect_IFStream	ifs = new ifstream;
	Handle(AIS2D_InteractiveObject) theIO;
	ifs->open(aFile, ios::in);
	ifs->getline(buf, 100);
	while (! ifs->eof())
	{
		if (! strcmp(buf, "AIS2D_InteractiveObject"))
		{
			theIO = new AIS2D_InteractiveObject();
			theIO->Retrieve(ifs);
		}
		ifs->getline(buf, 100);
	}
	ifs->close();

	return theIO;
}
