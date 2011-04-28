#define PERF    //GG_270298


#ifdef WNT
#define PRO19042        //GG_261199     Avoid to have black buffer because the ClearBuffer() method
//                      clear also the current buffer attributes !
//                      Under WNT the buffer contains also the attributes.
#endif

#define NO_TRACE 1

#include <Graphic2d_Buffer.ixx>

#include <Aspect_WindowDriver.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_WidthMapEntry.hxx>

#include <Graphic2d_Line.hxx>
#include <Graphic2d_Text.hxx>
#include <Graphic2d_Drawer.hxx>

Graphic2d_Buffer::Graphic2d_Buffer (const Handle(Graphic2d_View)& aView, const Quantity_Length aPivotX, const Quantity_Length aPivotY, const Standard_Integer aWidthIndex, const Standard_Integer aColorIndex, const Standard_Integer aFontIndex, const Aspect_TypeOfDrawMode aDrawMode):
	myBufferId(0),
    	myBufferIsPosted (Standard_False),
	myPivotX (Standard_ShortReal (aPivotX)),
	myPivotY (Standard_ShortReal (aPivotY)),
	myWidthIndex (aWidthIndex),
	myColorIndex (aColorIndex),
	myFontIndex (aFontIndex),
	myDrawMode(aDrawMode),
	myPView(aView.operator->()) {
}

void Graphic2d_Buffer::Destroy () {

	if( !myDriver.IsNull() ) {
	  myDriver->CloseBuffer(myBufferId) ;
	}
}

void Graphic2d_Buffer::Add (const Handle(Graphic2d_GraphicObject)& anObject) {
Standard_Integer i ;

  	myObjects.Append(anObject) ;
	for( i=1 ; i<=anObject->Length() ; i++ ) {
	  myPrimitives.Append(anObject->Primitive(i)) ;
	}
	if( myBufferIsPosted ) {
	  this->ReLoad(Standard_False) ;
	}
}

void Graphic2d_Buffer::Add (const Handle(Graphic2d_Primitive)& aPrimitive) {

  	myPrimitives.Append(aPrimitive) ;
	if( myBufferIsPosted ) {
	  this->ReLoad(Standard_False) ;
	}
}

void Graphic2d_Buffer::Remove (const Handle(Graphic2d_GraphicObject)& anObject) {
Standard_Integer i,j ;

  	for( i=1 ; i<=myObjects.Length() ; i++ ) {
    	  if( anObject == myObjects.Value(i) ) break ;
  	}

  	if( i <= myObjects.Length() ) {
    	  myObjects.Remove(i) ;
	  // not very clever, but who will use Remove (anObject) ?
	  for( j=1 ; j<=anObject->Length() ; j++ ) {
	    Remove(anObject->Primitive(j)) ;
	  }
	  if( myBufferIsPosted ) {
	    this->ReLoad(Standard_False) ;
	  }
  	}
}

void Graphic2d_Buffer::Remove (const Handle(Graphic2d_Primitive)& aPrimitive) {
Standard_Integer i ;

  	for( i=1 ; i<=myPrimitives.Length() ; i++ ) {
    	  if( aPrimitive == myPrimitives.Value(i) ) break ;
  	}

  	if( i <= myPrimitives.Length() ) {
    	  myPrimitives.Remove(i) ;
	  if( myBufferIsPosted ) {
	    this->ReLoad(Standard_False) ;
	  }
  	}
}

void Graphic2d_Buffer::Clear () {
#ifdef TRACE
printf(" Graphic2d_Buffer::Clear()%d,%d\n",myBufferId,myBufferIsPosted);
#endif

	myPrimitives.Clear() ;
	myObjects.Clear() ;
	if( myBufferIsPosted ) {
	  myDriver->ClearBuffer(myBufferId);
	}
}

void Graphic2d_Buffer::SetAttrib (const Standard_Integer aWidthIndex,
				  const Standard_Integer aColorIndex,
				  const Standard_Integer aFontIndex,
				  const Aspect_TypeOfDrawMode aDrawMode) {

	if( myColorIndex != aColorIndex || myFontIndex != aFontIndex ||
	    myWidthIndex != aWidthIndex || myDrawMode != aDrawMode ) {

  	  myWidthIndex = aWidthIndex ;	
  	  myColorIndex = aColorIndex ;	
  	  myFontIndex = aFontIndex ;	
  	  myDrawMode = aDrawMode ;	

	  if( myBufferIsPosted ) {
	    this->ReLoad(Standard_False) ;
	  }
	}
}

void Graphic2d_Buffer::SetPivot () {

	if( !myDriver.IsNull() ) {
	  Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer() ;
	  Standard_ShortReal xpivot,ypivot;

          myDriver->PositionOfBuffer(myBufferId,xpivot,ypivot) ;
	  theDrawer->UnMapFromTo(xpivot,ypivot,myPivotX,myPivotY) ;
	}
}

void Graphic2d_Buffer::SetPivot ( const Quantity_Length aPivotX, const Quantity_Length aPivotY) {

	myPivotX = Standard_ShortReal( aPivotX );
	myPivotY = Standard_ShortReal( aPivotY );

  	if( myBufferIsPosted ) {
	  this->ReLoad(Standard_False) ;
	}
}

void Graphic2d_Buffer::Move (const Quantity_Length aDeltaX, const Quantity_Length aDeltaY) {

	if( myBufferIsPosted ) {
	  Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer();
          Standard_ShortReal xpivot, ypivot;
          theDrawer->GetMapFromTo(Standard_ShortReal(aDeltaX),
              Standard_ShortReal(aDeltaY),xpivot,ypivot);
          myDriver->MoveBuffer(myBufferId, xpivot, ypivot);
	}
}

void Graphic2d_Buffer::Rotate (const Quantity_PlaneAngle anAngle) {

	if( myBufferIsPosted ) {
	  myDriver->RotateBuffer(myBufferId,anAngle) ;
	}
}

void Graphic2d_Buffer::Scale (const Quantity_Factor aFactor) {

	if( myBufferIsPosted ) {
	  myDriver->ScaleBuffer(myBufferId,aFactor,aFactor) ;
	}
}

Standard_Boolean Graphic2d_Buffer::IsEmpty () const {

	if( !myDriver.IsNull() ) {
	  return myDriver->BufferIsEmpty(myBufferId) ;
	} else {
	  return Standard_True ;
	}
}

Standard_Boolean Graphic2d_Buffer::IsIn (const Handle(Graphic2d_GraphicObject)& anObject) const {
Standard_Integer i,length = myObjects.Length() ;

  	for( i=1 ; i<=length ; i++ ) {
    	  if( anObject == myObjects.Value(i) ) break ;
  	}

	if( i <= length ) return Standard_True ;
	else return Standard_False;
}

Standard_Boolean Graphic2d_Buffer::IsIn (const Handle(Graphic2d_Primitive)& aPrimitive) const {
Standard_Integer i,length = myPrimitives.Length() ;

  	for( i=1 ; i<=length ; i++ ) {
    	  if( aPrimitive == myPrimitives.Value(i) ) break ;
  	}

	if( i <= length ) return Standard_True ;
	else return Standard_False;
}

void Graphic2d_Buffer::Post () {
Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer() ;
Standard_Boolean reset = Standard_True;
#ifdef TRACE
printf(" Graphic2d_Buffer::Post()%d\n",myBufferIsPosted);
#endif

	if( theDrawer->IsWindowDriver() ) {
          if( myBufferIsPosted ) {
	    reset = Standard_False;
	    UnPost() ;
	  }
	  myDriver = theDrawer->WindowDriver() ;
	  myBufferIsPosted = Standard_True ;
	  this->ReLoad(reset) ;
	  myPView->Add(this);
	}
}

void Graphic2d_Buffer::Post ( const Handle(Aspect_WindowDriver)& aDriver,
                              const Handle(Graphic2d_ViewMapping)& aViewMapping,
                              const Standard_Real aXPosition,
                              const Standard_Real aYPosition,
                              const Standard_Real aScale) {
 
Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer() ;
Standard_Boolean reset = Standard_True;
#ifdef TRACE
printf(" Graphic2d_Buffer::Post(%lx,%lx,%f,%f,%f)%d\n",
aDriver,aViewMapping,aXPosition,aYPosition,aScale,myBufferIsPosted);
#endif

	Standard_Real XCenter, YCenter, Size;

        if( myBufferIsPosted && (aDriver == myDriver) ) {
	  reset = Standard_False;
	  UnPost() ;
	}
	myDriver = aDriver;
        aViewMapping->ViewMapping(XCenter, YCenter, Size);
	theDrawer->SetDriver(aDriver);
        theDrawer->SetValues (XCenter, YCenter, Size,
                  	aXPosition, aYPosition, aScale, aViewMapping->Zoom());

	myBufferIsPosted = Standard_True ;
	this->ReLoad(reset) ;
	myPView->Add(this);
}

void Graphic2d_Buffer::UnPost () {
#ifdef TRACE
printf(" Graphic2d_Buffer::UnPost()%d\n",myBufferIsPosted);
#endif

        if( myBufferIsPosted ) {
  	  this->Erase() ;
	  myBufferIsPosted = Standard_False ;
	  myPView->Remove(this);
	}
}

Standard_Boolean Graphic2d_Buffer::IsPosted () const {

	return myBufferIsPosted ;
}

Standard_Boolean Graphic2d_Buffer::IsPosted ( const Handle(Aspect_WindowDriver)& aDriver) const {

	return (myBufferIsPosted && (aDriver == myDriver)) ;
}

void Graphic2d_Buffer::Erase () {
#ifdef TRACE
printf(" Graphic2d_Buffer::Erase()%d\n",myBufferIsPosted);
#endif

	if( myBufferIsPosted ) {
	  myDriver->EraseBuffer(myBufferId) ;
	}
}

void Graphic2d_Buffer::Draw () {
#ifdef TRACE
printf(" Graphic2d_Buffer::Draw()%d\n",myBufferIsPosted);
#endif

	if( myBufferIsPosted ) {
	  Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer() ;
	  Standard_Integer i,length = myPrimitives.Length() ;
	  theDrawer->SetRejection(Standard_False);
          myDriver->BeginDraw(Standard_False,myBufferId);
          for( i=1 ; i<=length ; i++ ) {
            myPrimitives.Value(i)->Draw(theDrawer);
	  }
	  myDriver->EndDraw() ;
	  theDrawer->SetRejection(Standard_True);
	}
}

void Graphic2d_Buffer::ReLoad (const Standard_Boolean ResetPosition) {

Standard_Boolean theStatus = Standard_False; 
Standard_Integer theColorIndex = myColorIndex;
Standard_Integer theWidthIndex = myWidthIndex;
Standard_Integer theFontIndex = myFontIndex;

#ifdef TRACE
printf(" Graphic2d_Buffer::ReLoad(%d)\n",ResetPosition);
#endif
	if( !myDriver.IsNull() ) {

	    myBufferId = this->HashCode(IntegerLast());

	    // Maximum depth of primitive lines
	    // contained in the buffer is required
	    if (theWidthIndex < 0) {
		Standard_Real theWidthMax;
		// There are no primitive lines => thickness by default
		if (! MaxWidth (theWidthMax, theWidthIndex))
		    theWidthIndex = 0;
	    }

	    // The font of the 1st primitive text from the buffer is required
	    if( theFontIndex < 0) {
	        Standard_Integer i;
		Handle(Graphic2d_Primitive) agp;
                for (i=1; i<=myPrimitives.Length(); i++) {
		  agp = myPrimitives.Value (i);
#ifdef PERF
                  if (agp->Family() == Graphic2d_TOP_TEXT) {
#else
                  if (agp->IsKind (STANDARD_TYPE (Graphic2d_Text))) {
#endif
		     theFontIndex =
			(*(Handle(Graphic2d_Text ) *) &agp)->FontIndex ();
		    break;
 
		  }
		}
		if( theFontIndex < 0 ) theFontIndex = 0;
	    }

	    // The color of the 1st primitive from the buffer is required
	    if( theColorIndex < 0) {
	        Standard_Integer i;
                for (i=1; i<=myPrimitives.Length(); i++) {
		  theColorIndex = myPrimitives.Value (i)->ColorIndex ();
		    break;
 
		}
		if( theColorIndex < 0 ) theColorIndex = 0;
	    }

	    Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer() ;
	    Standard_ShortReal xpivot,ypivot;

	    theDrawer->GetMapFromTo(myPivotX,myPivotY,xpivot,ypivot) ;
	    theStatus = myDriver->OpenBuffer(myBufferId,xpivot,ypivot,
			theWidthIndex,theColorIndex,theFontIndex,myDrawMode) ;

	    if( theStatus ) {
	      Standard_ShortReal xpos,ypos;

              myDriver->PositionOfBuffer(myBufferId,xpos,ypos) ;
#ifndef PRO19042        // Under WNT the buffer is cleared at open time
              myDriver->ClearBuffer(myBufferId) ;
#endif
	      this->Draw() ;
              if( ResetPosition ) {
	      	myDriver->DrawBuffer(myBufferId);
	      } else {
		myDriver->MoveBuffer(myBufferId,xpos,ypos) ;
	      } 
	    }
	}

	if( !theStatus ) {
	    myBufferIsPosted = Standard_False ;
	}
}

Standard_Boolean
Graphic2d_Buffer::MaxWidth (
	Quantity_Length& theWidth, Standard_Integer& theIndex) const {
Standard_Boolean Result = Standard_False;

      if( !myDriver.IsNull() ) {
	Handle(Aspect_WidthMap) theWidthMap = myDriver->WidthMap ();
	Standard_Integer i,size = theWidthMap->Size(),
				length = myPrimitives.Length();

	Quantity_Length WidthCur = 0.;
	Standard_Integer IndexCur = 0;

	theWidth = 0.;
	theIndex = 0;

	Handle(Graphic2d_Primitive) agp;
	for (i=1; i<=length; i++) {
	    agp = myPrimitives.Value (i);
#ifdef PERF
	    if (agp->Family() == Graphic2d_TOP_LINE ) {
#else
	    if (agp->IsKind (STANDARD_TYPE (Graphic2d_Line))) {
#endif
		// There are lines in the Buffer
		// and not by fracture on the line
		Result = Standard_True;
		IndexCur =
		    (*(Handle(Graphic2d_Line) *) &agp)->WidthIndex ();
		if ((IndexCur <= 0) || (IndexCur > size)) {
		    // Case when thicknesses are not precised, IndexCur == 0
		    // or there is a really huge problem !
		}
		else {
		    // +1 because in the table there is 1 default entry
		    // which defines default thickness
		    WidthCur = (theWidthMap->Entry (IndexCur+1)).Width ();
		    if (theWidth < WidthCur) {
			theWidth = WidthCur;
			theIndex = IndexCur;
		    }
		}
	    }
        }
      }
      return Result;
}

Handle(Graphic2d_View) Graphic2d_Buffer::View () const {

	return ((Graphic2d_View*) myPView);

}

Quantity_PlaneAngle Graphic2d_Buffer::Angle () const {
Quantity_PlaneAngle angle = 0. ;

	if( !myDriver.IsNull() ) {
	  myDriver->AngleOfBuffer(myBufferId,angle) ;
	}

	return angle;
}

Quantity_Factor Graphic2d_Buffer::Scale () const {
Quantity_Factor xscale = 1. ;
Quantity_Factor yscale = 1. ;
	
	if( !myDriver.IsNull() ) {
	  myDriver->ScaleOfBuffer(myBufferId,xscale,yscale) ;
	}

	return (xscale+yscale)/2. ;
}

Quantity_Length Graphic2d_Buffer::Xpivot () const {
Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer() ;
Standard_ShortReal pivotx = myPivotX ;
Standard_ShortReal pivoty = myPivotY ;
	
	if( !myDriver.IsNull() ) {
	  Standard_ShortReal xpivot,ypivot ;
	  myDriver->PositionOfBuffer(myBufferId,xpivot,ypivot) ;
	  theDrawer->UnMapFromTo(xpivot,ypivot,pivotx,pivoty) ;
	}

	return Quantity_Length(pivotx);
}

Quantity_Length Graphic2d_Buffer::Ypivot() const {
Handle(Graphic2d_Drawer) theDrawer = myPView->Drawer() ;
Standard_ShortReal pivotx = myPivotX ;
Standard_ShortReal pivoty = myPivotY ;
	
	if( !myDriver.IsNull() ) {
	  Standard_ShortReal xpivot,ypivot ;
	  myDriver->PositionOfBuffer(myBufferId,xpivot,ypivot) ;
	  theDrawer->UnMapFromTo(xpivot,ypivot,pivotx,pivoty) ;
	}

	return Quantity_Length(pivoty);
}
