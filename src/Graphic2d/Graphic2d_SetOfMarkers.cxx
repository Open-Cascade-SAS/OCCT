#define G002    //GG_140400 Use SetPickedIndex method 
                // Add new DrawElement(), DrawVertex() methods

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0

#include <Graphic2d_SetOfMarkers.ixx>


//SAV
#include <TColStd_HSequenceOfInteger.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <Aspect_WindowDriver.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <gp_Circ2d.hxx> 
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <Aspect_Units.hxx>

#define OCC402

Graphic2d_SetOfMarkers
::Graphic2d_SetOfMarkers( const Handle(Graphic2d_GraphicObject)& aGraphicObject )
: Graphic2d_Line (aGraphicObject), myHLimit( -1 ), mySuppressHigh( Standard_False )
{

  	SetFamily(Graphic2d_TOP_MARKER);
    
}

void Graphic2d_SetOfMarkers::Add ( const Standard_Real X, const Standard_Real Y) {

  Standard_ShortReal x = Standard_ShortReal( X );
  Standard_ShortReal y = Standard_ShortReal( Y );

  myMinX = Min(myMinX,x);
  myMinY = Min(myMinY,y);
  myMaxX = Max(myMaxX,x);
  myMaxY = Max(myMaxY,y);

  myType.Append(0);
  myX.Append(x); 
  myY.Append(y); 
  myWidth.Append(0.);
  myHeight.Append(0.);
  myAngle.Append(0.);
  myScaledWidth = 0.;

}

void Graphic2d_SetOfMarkers::Add (
        const Standard_Integer anIndex,
        const Standard_Real X, const Standard_Real Y,
        const Standard_Real aWidth, const Standard_Real anHeight,
        const Quantity_PlaneAngle anAngle)
{ 
Standard_ShortReal x  = Standard_ShortReal( X );
Standard_ShortReal y  = Standard_ShortReal( Y );
Standard_ShortReal w  = Standard_ShortReal( aWidth );
Standard_ShortReal h  = Standard_ShortReal( anHeight );
Standard_ShortReal a  = Standard_ShortReal( anAngle );
Standard_ShortReal x1 = Standard_ShortReal( x - w/2. );
Standard_ShortReal x2 = Standard_ShortReal( x + w/2. );
Standard_ShortReal y1 = Standard_ShortReal( y - h/2. );
Standard_ShortReal y2 = Standard_ShortReal( y + h/2. );

        if( anIndex <= 0 )
                Graphic2d_MarkerDefinitionError::Raise
                        ("The marker index must be > 0");
 
        if (aWidth <= 0.0)
                Graphic2d_MarkerDefinitionError::Raise ("The width = 0.");
 
        if (anHeight <= 0.0)
                Graphic2d_MarkerDefinitionError::Raise ("The height = 0.");
 
	myMinX = Min(myMinX,x1);
	myMinY = Min(myMinY,y1);
	myMaxX = Max(myMaxX,x2);
	myMaxY = Max(myMaxY,y2);

	myType.Append(anIndex);
	myX.Append(x); 
        myY.Append(y); 
	myWidth.Append(w);
	myHeight.Append(h);
	myAngle.Append(a);
        myScaledWidth = w;
}

Standard_Integer Graphic2d_SetOfMarkers::Length () const {
   return myX.Length();
}

void Graphic2d_SetOfMarkers::Values(
                      const Standard_Integer aRank,
				      Standard_Integer    &anIndex,
				      Quantity_Length     & X,
                      Quantity_Length     & Y,
                      Quantity_Length     & aW, 
                      Quantity_Length     & aH,
                      Quantity_PlaneAngle & anAngle ) const
{
	if( aRank < 1 || aRank > myX.Length() ) 
                Standard_OutOfRange::Raise
			("the marker rank is out of bounds in the set");

	anIndex = myType( aRank );
	X       = Quantity_Length( myX( aRank ) );
	Y       = Quantity_Length( myY( aRank ) );
    aW      = Quantity_Length( myWidth( aRank ) );
    aH      = Quantity_Length( myHeight( aRank ) );
    anAngle = Quantity_PlaneAngle( myAngle( aRank ) );
}

void Graphic2d_SetOfMarkers::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {
 
  myNumOfElem = Length();
  myNumOfVert = Length();
  Standard_Integer i,l = Length(),bufferize = 0;

  if ( (l > 0) && aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY)){
    DrawMarkerAttrib(aDrawer);

    if( l > 2 ) bufferize = 1; 
    aDrawer->MapMarkerFromTo
	(myType(1), myX(1), myY(1), myWidth(1), myHeight(1), 
						myAngle(1),bufferize);
    for( i=2 ; i<l ; i++ ) {
      aDrawer->MapMarkerFromTo
	(myType(i), myX(i), myY(i), myWidth(i), myHeight(i), myAngle(i),0);
    }
    aDrawer->MapMarkerFromTo
    	(myType(l), myX(l), myY(l), myWidth(l), myHeight(l), 
						myAngle(l),-bufferize);
  }
}

#ifdef G002

void Graphic2d_SetOfMarkers::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                      const Standard_Integer anIndex) {
  Standard_Integer l = Length(), bufferize = 0;

  if ( (l > 0) && aDrawer->IsIn( myMinX,myMaxX,myMinY,myMaxY) ) {
   if ( anIndex > 0 && anIndex <= l ) { //Draw element
    DrawMarkerAttrib(aDrawer);
    if ( l > 2 ) bufferize = 1; 
    if ( anIndex == 1 )     
     aDrawer->MapMarkerFromTo
	 ( myType(1), myX(1), myY(1), myWidth(1), myHeight(1), 
						myAngle(1),bufferize );
    else if ( anIndex == l )
      aDrawer->MapMarkerFromTo
      (myType(l), myX(l), myY(l), myWidth(l), myHeight(l), 
						myAngle(l),-bufferize);
    else 
      aDrawer->MapMarkerFromTo
	  ( myType(anIndex), myX(anIndex), myY(anIndex), myWidth(anIndex), 
        myHeight(anIndex), myAngle(anIndex),0);
    }    
  }

}

void Graphic2d_SetOfMarkers::DrawVertex( const Handle(Graphic2d_Drawer)& /*aDrawer*/,
                                         const Standard_Integer /*anIndex*/) {
}
#endif

Standard_Boolean Graphic2d_SetOfMarkers::Pick (const Standard_ShortReal X,
					  const Standard_ShortReal Y,
					  const Standard_ShortReal aPrecision,
					  const Handle(Graphic2d_Drawer)& /*aDrawer*/) {


#ifdef OCC402
  myPickedIndices.Clear();
#endif

  Standard_Integer i,l = Length();
  Standard_ShortReal SRX = X, SRY = Y;
 
  if ( (l > 0) && IsInMinMax (X, Y, aPrecision)) {

	if (myGOPtr->IsTransformed ()) {
      gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
      Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
	  aTrsf.Transforms (RX, RY);
	  SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
	}

#ifdef G002
    for ( i = 1; i <= myX.Length(); i++ ) {
       if ( Graphic2d_Primitive::IsOn ( SRX, SRY, myX(i), myY(i), aPrecision) ) {
           SetPickedIndex(-i);
           return Standard_True;
       } else if ( (i < myX.Length() ) &&  
                   SRX >= myX(i)-aPrecision &&
	               SRX <= myX(i)+aPrecision &&
	               SRY >= myY(i)-aPrecision &&
	               SRY <= myY(i)+aPrecision ) {
            SetPickedIndex(i);
            return Standard_True;
       }
    }
#else

    for ( i=1; i<=myX.Length(); i++) {
	  if (SRX >= myX(i)-aPrecision &&
	      SRX <= myX(i)+aPrecision &&
	      SRY >= myY(i)-aPrecision &&
	      SRY <= myY(i)+aPrecision) {
        myPickedIndex = i;
		return Standard_True;
      }
    }

#endif

   TShort_Array1OfShortReal arr1X( 1, myX.Length() );
   TShort_Array1OfShortReal arr1Y( 1, myY.Length() );
   for ( i = 1; i < myX.Length(); i++ ) {
      arr1X.SetValue( i, myX(i) );
      arr1Y.SetValue( i, myY(i) );
   }

   if ( IsIn( SRX, SRY,  arr1X, arr1Y, aPrecision ) ) {
#ifdef G002
	      SetPickedIndex(0);
#else
	      myPickedIndex = 0;
#endif
	      return Standard_True;
   }

    return Standard_False;
  } //end if it's in MinMax
  return Standard_False;
}

Standard_Boolean Graphic2d_SetOfMarkers::Pick( const Standard_ShortReal X1,
					       const Standard_ShortReal Y1,
					       const Standard_ShortReal X2,
					       const Standard_ShortReal Y2,
					       const Handle(Graphic2d_Drawer)& drawer,
					       const Graphic2d_PickMode pMode )
{
#ifdef OCC402
  mySuppressHigh = Standard_False;
  Standard_ShortReal minX = Min( X2, X1 ),
                     minY = Min( Y2, Y1 ),
                     maxX = Max( X2, X1 ),
                     maxY = Max( Y2, Y1 );


  // no check for transformation...
  // pick mode - INCLUDE only.
  
  switch( pMode ) {

    case Graphic2d_PM_INCLUDE : {
	
      TColStd_MapOfInteger toHighlight; 

      for (Standard_Integer i=1; i<=myX.Length(); i++)
	if ( myX(i) >= minX && myX(i) <= maxX &&
	     myY(i) >= minY && myY(i) <= maxY ) {

	  if ( myMapOfSelected.Extent() && myMapOfSelected.Contains( i ) )
	    continue;
	  if ( myPickedIndices.Contains( i ) )
	    myPickedIndices.Remove( i );
	  toHighlight.Add( i );
	}
      
      if ( myPickedIndices.Extent() && drawer->IsWindowDriver() ) {
	Handle(Aspect_WindowDriver) theDriver = drawer->WindowDriver();
	theDriver->BeginDraw( Standard_True );
	drawer->SetOverride( Standard_True );
	drawer->SetOverrideColor( ColorIndex() );
	DrawPickedElements( drawer );
	drawer->SetOverride( Standard_False );
	theDriver->EndDraw();
      }
      
      myPickedIndices = toHighlight;

      if ( myPickedIndices.Extent() )
	return Standard_True;
      break;
    }
  }
#endif
  return Standard_False;
}


Standard_Boolean Graphic2d_SetOfMarkers
::PickByCircle( const Standard_ShortReal x,
	        const Standard_ShortReal y,
	        const Standard_ShortReal radius,
	        const Handle(Graphic2d_Drawer)& drawer,
	        const Graphic2d_PickMode pMode )
{
#ifdef OCC402

  mySuppressHigh = Standard_True;
  myPickedIndices.Clear();

  // no check for transformation...
  // pick mode - INCLUDE only.
  
  switch( pMode ) 
  {

    case Graphic2d_PM_INCLUDE : {
	
      Standard_Real xdist, ydist;
      Standard_Real rad;
      for (Standard_Integer i=1; i<=myX.Length(); i++) {
	xdist = Abs( myX(i) - x );
	ydist = Abs( myY(i) - y );
	rad = radius - myScaledWidth / 2;
	if ( xdist <= rad && ydist <= rad )
	  myPickedIndices.Add( i );
      }
      
      if ( myPickedIndices.Extent() )
	return Standard_True;
      break;
    }
  }
#endif
  return Standard_False;
}


void Graphic2d_SetOfMarkers::Save(Aspect_FStream& aFStream) const
{
}

//SAV
void Graphic2d_SetOfMarkers
::DrawPickedElements( const Handle(Graphic2d_Drawer)& aDrawer )
{
#ifdef OCC402
  if ( !mySuppressHigh ) {
    Standard_Integer length = myPickedIndices.Extent();
    if ( length ) {
      
      if ( myHLimit > 0 && length > myHLimit )
	length = myHLimit;
      
      Handle(TShort_HArray1OfShortReal) x = 
	new TShort_HArray1OfShortReal( 1, length );
      Handle(TShort_HArray1OfShortReal) y = 
	new TShort_HArray1OfShortReal( 1, length );
      
      TColStd_MapIteratorOfMapOfInteger it( myPickedIndices );
      for ( Standard_Integer i = 1; i <= length; it.Next(), i++ ) {
	x->SetValue( i, myX( it.Key() ) );
	y->SetValue( i, myY( it.Key() ) );
      }
      
      DrawElements( aDrawer, x, y );
    }
  }
#endif
}

//SAV
void Graphic2d_SetOfMarkers
::DrawSelectedElements( const Handle(Graphic2d_Drawer)& aDrawer )
{
#ifdef OCC402
  Standard_Integer length = myMapOfSelected.Extent();
  if ( length ) {
    Handle(TShort_HArray1OfShortReal) x = 
      new TShort_HArray1OfShortReal( 1, length );
    Handle(TShort_HArray1OfShortReal) y = 
      new TShort_HArray1OfShortReal( 1, length );
    TColStd_MapIteratorOfMapOfInteger it( myMapOfSelected );
    Standard_Integer index = 1;
    for (; it.More(); it.Next(), index++ ) {
      x->SetValue( index, myX( it.Key() ) );
      y->SetValue( index, myY( it.Key() ) );
    }
  
    DrawElements( aDrawer, x, y );
    //myPickedIndices.Clear();
  }
#endif
}

//SAV
void Graphic2d_SetOfMarkers
::DrawElements( const Handle(Graphic2d_Drawer)& aDrawer,
	        const Handle(TShort_HArray1OfShortReal)& x,
	        const Handle(TShort_HArray1OfShortReal)& y )
{
#ifdef OCC402
  DrawMarkerAttrib(aDrawer);

  Standard_Integer type = myType( 1 );
  Standard_ShortReal width = myWidth( 1 ), 
                     height = myHeight( 1 ),
                     angle = myAngle( 1 );
  aDrawer->MapMarkersFromTo( type, x, y, width, height, angle, 0);
#endif
}

//SAV
Standard_Boolean Graphic2d_SetOfMarkers::SetElementsSelected()
{
#ifdef OCC402
  if ( myPickedIndices.Extent() ) {
    myMapOfSelected.Clear();
    myMapOfSelected = myPickedIndices;
    //myPickedIndices.Clear();
  }
#endif
  return Standard_True;
}

//SAV
Standard_Boolean Graphic2d_SetOfMarkers::HasSelectedElements()
{
#ifdef OCC402
  return (myMapOfSelected.Extent() > 0);
#endif
  return Standard_False;
}

//SAV
void Graphic2d_SetOfMarkers::ClearSelectedElements()
{
#ifdef OCC402
  myMapOfSelected.Clear();
#endif
}


//SAV
const TColStd_MapOfInteger& Graphic2d_SetOfMarkers::GetSelectedElements() const
{
 return myMapOfSelected;
}

//SAV
void Graphic2d_SetOfMarkers::AddOrRemoveSelected(const Standard_Integer index)
{
#ifdef OCC402
  if ( myMapOfSelected.Contains( index ) )
    myMapOfSelected.Remove( index );
  else
    myMapOfSelected.Add( index );
#endif
}

//SAV
void Graphic2d_SetOfMarkers::SetHighlightedLimit(const Standard_Integer number)
{
#ifdef OCC402
  if ( number > 0 )
    myHLimit = number;
#endif
}

void Graphic2d_SetOfMarkers::SetScaledWidth( const Standard_ShortReal width )
{
#ifdef OCC402
 myScaledWidth = width;
#endif
}
