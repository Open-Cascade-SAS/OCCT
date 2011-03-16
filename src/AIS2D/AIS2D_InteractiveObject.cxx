#include <AIS2D_InteractiveObject.ixx>
#include <Prs2d_AspectLine.hxx>
#include <Prs2d_Point.hxx>
#include <V2d_Viewer.hxx>
#include <AIS2D_ProjShape.hxx>
#include <AIS2D_PrimitiveArchit.hxx>
#include <Graphic2d_Segment.hxx>
#include <Graphic2d_InfiniteLine.hxx>
#include <Graphic2d_Polyline.hxx>
#include <Graphic2d_PolylineMarker.hxx>
#include <Graphic2d_Circle.hxx>
#include <Graphic2d_CircleMarker.hxx>
#include <Graphic2d_Ellips.hxx>
#include <Graphic2d_EllipsMarker.hxx>
#include <GGraphic2d_Curve.hxx>
#include <stdio.h>

#define NUM_ASPECTROOT 29


AIS2D_InteractiveObject::AIS2D_InteractiveObject() 

 : Graphic2d_GraphicObject(),
   myDrawer( new Prs2d_Drawer() ),
   myHighlightMode( AIS2D_TOD_PRIMITIVE ),
   myICTX(),
   myDisplayMode( 0 ),
   mySelectionMode( 0 ),
   myAspects(),
   myState( 0 ),
   myDetectedPrimSeq( new AIS2D_HSequenceOfPrimArchit() ),
   mySelectedPrimSeq( new AIS2D_HSequenceOfPrimArchit() ) 
   
{
   AIS2D_DataMapOfPrimAspects theAspect(NUM_ASPECTROOT);
   myAspects = theAspect;
printf(" $$$ Creates AIS2D_InteractiveObject()\n");
} 

void AIS2D_InteractiveObject::SetAttributes( const Handle(Prs2d_Drawer)& aDrawer ) {
	 myDrawer = aDrawer;
}

void AIS2D_InteractiveObject::UnsetAttributes() {
   myDrawer = new Prs2d_Drawer();
}

void AIS2D_InteractiveObject::SetAspect( const Handle(Prs2d_AspectRoot)& anAspect ) {

  if ( ! PrimitiveExist(anAspect->GetAspectName()) ) return;

  switch ( anAspect->GetAspectName() ) {
   
    case Prs2d_AN_LINE: {

    if ( GetContext().IsNull() ) {
        myDrawer->SetAspect( anAspect, Prs2d_AN_LINE );
        return;
    }
    Handle(Prs2d_AspectLine) theAspLine = 
         Handle(Prs2d_AspectLine)::DownCast( anAspect );

    Quantity_Color     aColor;
    Aspect_TypeOfLine  aType;
    Aspect_WidthOfLine aWidth;
  
    Quantity_Color aIntColor;
    Graphic2d_TypeOfPolygonFilling aTypeFill;
    Standard_Integer aTile;
    Standard_Boolean aDrawEdge;
  
    theAspLine->ValuesOfLine( aColor, aType, aWidth );
    theAspLine->ValuesOfPoly( aIntColor, aTypeFill, aTile, aDrawEdge );
   
    Standard_Integer colorInd = theAspLine->ColorIndex();
    Standard_Integer typeInd = theAspLine->TypeIndex();
    Standard_Integer widthInd = theAspLine->WidthIndex();
    Standard_Integer colorIntInd = theAspLine->InterColorIndex();

    if ( !colorInd ) {
       colorInd = GetContext()->InitializeColor( aColor );
       theAspLine->SetColorIndex( colorInd );
    }

    if ( !typeInd ) {
       typeInd = GetContext()->InitializeStyle( aType );
       theAspLine->SetTypeIndex( typeInd );
    }

    if ( !widthInd ) {
       widthInd = GetContext()->InitializeWidth( aWidth );
       theAspLine->SetWidthIndex( widthInd );
    }
    
    if ( !colorIntInd ) {
       colorIntInd = GetContext()->InitializeColor( aIntColor );
       theAspLine->SetIntColorInd( colorIntInd );
    }

    if ( this->IsKind(STANDARD_TYPE(AIS2D_ProjShape) ) ) {

     Handle(AIS2D_ProjShape) thePS = Handle(AIS2D_ProjShape)::DownCast(this);
     Handle(Graphic2d_Line) theLines = thePS->GetPrimitives();
     theLines->SetColorIndex( colorInd );
     theLines->SetTypeIndex( typeInd );
     theLines->SetWidthIndex( widthInd );

     if ( thePS->IsHLMode() ) {
       Handle(Graphic2d_Line) theHLines = thePS->GetHLPrimitives();
       theHLines->SetColorIndex( colorInd );
       theHLines->SetWidthIndex( widthInd );
       Standard_Integer theDashInd = GetContext()->InitializeStyle( Aspect_TOL_DASH );
       theHLines->SetTypeIndex( theDashInd );          
     }


    } else 

     for ( Standard_Integer i = 1; i <= Length(); i++ ) {
             
       if ( Primitive(i)->IsKind(STANDARD_TYPE(Graphic2d_Line)) ) {
       
         Handle(Graphic2d_Line) theLine = 
             Handle(Graphic2d_Line)::DownCast( Primitive(i) );
                          
         theLine->SetColorIndex( colorInd );
         theLine->SetTypeIndex( typeInd );
         theLine->SetWidthIndex( widthInd );

         theLine->SetInteriorColorIndex( colorIntInd );
         theLine->SetTypeOfPolygonFilling( aTypeFill );
         theLine->SetInteriorPattern( aTile );
         theLine->SetDrawEdge( aDrawEdge );

         if ( !myAspects.IsBound(theLine) ) 
             myAspects.Bind( theLine, theAspLine );
         else
             myAspects( theLine ) = theAspLine;
  
       } // end if STANDARD_TYPE is Graphic2d_Line
     
      } // end for
     
     break;  

   } // end case Prs2d_AN_LINE

   case Prs2d_AN_HIDDENLINE: {
       }
   case Prs2d_AN_TEXT: {
       }
   case Prs2d_AN_HIDINGPOLY: {
       }
   case Prs2d_AN_HIDINGTEXT: {
       }
   case Prs2d_AN_FRAMEDTEXT: {
       }
   case Prs2d_AN_LAST: {
       }
   case Prs2d_AN_UNKNOWN: {
       }
  }
  myDrawer->SetAspect( anAspect, anAspect->GetAspectName() );
  if ( !GetContext().IsNull() ) 
     GetContext()->CurrentViewer()->Update();

}

void AIS2D_InteractiveObject::SetAspect( const Handle(Prs2d_AspectRoot)& anAspect,
                                         const Handle(Graphic2d_Primitive)& aPrim ) {
               
  if ( ( GetContext().IsNull() ) || ( !IsIn(aPrim) ) ) return;

   switch ( anAspect->GetAspectName() ) {
   
   case Prs2d_AN_LINE: {

    if ( aPrim->IsKind(STANDARD_TYPE(Graphic2d_Line)) ) {
   
      Handle(Prs2d_AspectLine) theAspLine = 
         Handle(Prs2d_AspectLine)::DownCast( anAspect );

      Quantity_Color     aColor;
      Aspect_TypeOfLine  aType;
      Aspect_WidthOfLine aWidth;
   
      Quantity_Color aIntColor;
      Graphic2d_TypeOfPolygonFilling aTypeFill;
      Standard_Integer aTile;
      Standard_Boolean aDrawEdge;
  
      theAspLine->ValuesOfLine( aColor, aType, aWidth );
      theAspLine->ValuesOfPoly( aIntColor, aTypeFill, aTile, aDrawEdge );
   
      Standard_Integer colorInd = theAspLine->ColorIndex();
      Standard_Integer typeInd = theAspLine->TypeIndex();
      Standard_Integer widthInd = theAspLine->WidthIndex();
      Standard_Integer colorIntInd = theAspLine->InterColorIndex();

      if ( !colorInd ) {
       colorInd = GetContext()->InitializeColor( aColor );
       theAspLine->SetColorIndex( colorInd );
      }

      if ( !typeInd ) {
       typeInd = GetContext()->InitializeStyle( aType );
       theAspLine->SetTypeIndex( typeInd );
      }

      if ( !widthInd ) {
       widthInd = GetContext()->InitializeWidth( aWidth );
       theAspLine->SetWidthIndex( widthInd );
      }

      if ( !colorIntInd ) {
       colorIntInd = GetContext()->InitializeColor( aIntColor );
       theAspLine->SetIntColorInd( colorIntInd );
      }

      Handle(Graphic2d_Line) theLine = Handle(Graphic2d_Line)::DownCast( aPrim );
                
      theLine->SetColorIndex( colorInd );
      theLine->SetTypeIndex( typeInd );
      theLine->SetWidthIndex( widthInd );

      theLine->SetInteriorColorIndex( colorIntInd );
      theLine->SetTypeOfPolygonFilling( aTypeFill );
      theLine->SetInteriorPattern( aTile );
      theLine->SetDrawEdge( aDrawEdge );

      if ( !myAspects.IsBound(aPrim) )
         myAspects.Bind( theLine, theAspLine );
      else
         myAspects( theLine ) = theAspLine;

    } // end if STANDARD_TYPE is Graphic2d_Line
    break;
    
   } // end case Prs2d_AN_LINE

   case Prs2d_AN_HIDDENLINE: {
       break;
       }
   case Prs2d_AN_TEXT: {
       break;
       }
   case Prs2d_AN_HIDINGPOLY: {
       break;
       }
   case Prs2d_AN_HIDINGTEXT: {
       break;
       }
   case Prs2d_AN_FRAMEDTEXT: {
       break;
       }
   case Prs2d_AN_LAST: {
       break;
       }
   case Prs2d_AN_UNKNOWN: {
       break;
       }
   
   } // end switch

  myDrawer->SetAspect( anAspect, anAspect->GetAspectName() );
  GetContext()->CurrentViewer()->Update();

}

Handle(Prs2d_AspectRoot) AIS2D_InteractiveObject::GetAspect
                       ( const Handle(Graphic2d_Primitive)& aPrim ) const {
    
   return myAspects( aPrim );
}

Standard_Boolean AIS2D_InteractiveObject::HasAspect
            ( const Handle(Graphic2d_Primitive)& aPrim ) const {

  return myAspects.IsBound( aPrim );
}

Standard_Boolean AIS2D_InteractiveObject::HasInteractiveContext() const {
  return ( ! myICTX.IsNull() );	
}

Handle(AIS2D_InteractiveContext) AIS2D_InteractiveObject::GetContext() const {
  	return myICTX;
}

void AIS2D_InteractiveObject::SetContext( const Handle(AIS2D_InteractiveContext)& aCtx) {
  
  myICTX = aCtx;
  if ( myDrawer.IsNull() ) 
    myDrawer = new Prs2d_Drawer;
   
}
 
void AIS2D_InteractiveObject::SelectPrimitive (const Standard_Integer anIndex, const Standard_Boolean append ) 
{
  if ( !append ) 
  { // unselect all 
    for ( Standard_Integer i = 1; i <= mySelectedPrimSeq->Length(); i++ )
      mySelectedPrimSeq->Value( i )->GetPrimitive()->Unhighlight();
    mySelectedPrimSeq->Clear();
    SetState( 0 );
  }
  // select new primitive 
  SetPickedIndex( anIndex );
}

Standard_Boolean AIS2D_InteractiveObject::PrimitiveExist( const Prs2d_AspectName aName) const {

   Standard_Boolean isFound = Standard_False;
   
   for ( Standard_Integer i = 1; i <= Length(); i++ )

     switch ( aName ) {
     case Prs2d_AN_LINE: {
       if ( Primitive(i)->IsKind(STANDARD_TYPE(Graphic2d_Line)) )
           isFound = Standard_True;
       break;
     }
     case Prs2d_AN_HIDDENLINE: {
         break;
     }
     case Prs2d_AN_TEXT: {
         break;
     }
     case Prs2d_AN_HIDINGPOLY: {
         break;
     }
     case Prs2d_AN_HIDINGTEXT: {
         break;
     }
     case Prs2d_AN_FRAMEDTEXT: {
         break;
     }
     case Prs2d_AN_LAST: {
         break;
     }
     case Prs2d_AN_UNKNOWN: {
         break;
     }
    } // end switch

      
   return isFound;
}

void AIS2D_InteractiveObject::AddDetectPrim ( const Handle(Graphic2d_Primitive)& aPrim,
                                              const Standard_Integer anInd) {

  Handle(AIS2D_PrimitiveArchit) thePA = new AIS2D_PrimitiveArchit( aPrim, anInd );
  myDetectedPrimSeq->Append( thePA );

}

void AIS2D_InteractiveObject::AddSelectPrim ( const Handle(Graphic2d_Primitive)& aPrim,
                                              const Standard_Integer anInd) {

  Handle(AIS2D_PrimitiveArchit) thePA = new AIS2D_PrimitiveArchit( aPrim, anInd );
  mySelectedPrimSeq->Append( thePA );

}

void AIS2D_InteractiveObject::RemoveSelectPrim ( const Handle(Graphic2d_Primitive)& aPrim,
                                                 const Standard_Integer anInd) {

    if ( mySelectedPrimSeq->IsEmpty() ) return;
    for ( int i = 1; i <= mySelectedPrimSeq->Length(); ++i )
      if ( mySelectedPrimSeq->Value(i)->GetPrimitive() == aPrim && 
          mySelectedPrimSeq->Value(i)->GetIndex() == anInd ) {
          mySelectedPrimSeq->Remove(i);
          return;
      }   
}

void AIS2D_InteractiveObject::SetSelSeqPrim( const Handle(AIS2D_HSequenceOfPrimArchit)& aSeq ) {

   mySelectedPrimSeq = aSeq;

}

void AIS2D_InteractiveObject::ClearSeqDetPrim() {

   myDetectedPrimSeq->Clear();

}

void AIS2D_InteractiveObject::ClearSeqSelPrim() {

   mySelectedPrimSeq->Clear();

}

Handle(AIS2D_HSequenceOfPrimArchit) AIS2D_InteractiveObject::GetDetectSeqPrim() const {

   return myDetectedPrimSeq;

}

Handle(AIS2D_HSequenceOfPrimArchit) AIS2D_InteractiveObject::GetSelectedSeqPrim() const {

  return mySelectedPrimSeq;

}

void AIS2D_InteractiveObject::Save(Aspect_FStream& aFStream) const
{
	int lnOfPr = Length();

	*aFStream << "AIS2D_InteractiveObject" << endl;
	for (int j = 1; j <= lnOfPr; j++)
	{
		Handle(Graphic2d_Primitive) thePrm = this->Primitive(j);
		thePrm->Save(aFStream);
	}
}

void AIS2D_InteractiveObject::Retrieve(Aspect_IFStream& anIFStream) const
{
	char buf[100];
	anIFStream->getline(buf, 100);
	while (! anIFStream->eof())
	{
		if (! strcmp(buf, "Graphic2d_Segment"))
		{
			Graphic2d_Segment::Retrieve(anIFStream, this);
		}
		else if (! strcmp(buf, "Graphic2d_InfiniteLine"))
		{
			Graphic2d_InfiniteLine::Retrieve(anIFStream, this);
		}
		else if (! strcmp(buf, "Graphic2d_Polyline"))
		{
			Graphic2d_Polyline::Retrieve(anIFStream, this);
		}
		else if (! strcmp(buf, "Graphic2d_PolylineMarker"))
		{
			Graphic2d_PolylineMarker::Retrieve(anIFStream, this);
		}
		else if (! strcmp(buf, "Graphic2d_Circle"))
		{
			Graphic2d_Circle::Retrieve(anIFStream, this);
		}
		else if (! strcmp(buf, "Graphic2d_CircleMarker"))
		{
			Graphic2d_CircleMarker::Retrieve(anIFStream, this);
		}
		else if (! strcmp(buf, "Graphic2d_Ellips"))
		{
			Graphic2d_Ellips::Retrieve(anIFStream, this);
		}
		else if (! strcmp(buf, "Graphic2d_EllipsMarker"))
		{
			Graphic2d_EllipsMarker::Retrieve(anIFStream, this);
		}
		else if (! strcmp(buf, "GGraphic2d_Curve"))
		{
			GGraphic2d_Curve::Retrieve(anIFStream, this);
		}
		anIFStream->getline(buf, 100);
	}
}

