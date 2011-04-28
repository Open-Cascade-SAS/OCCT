//
// S3593    //GG_270298
//              Avoid calling IsKind() which is expensive.
//		Add field myIsUpToDate informing if the object is updated.
//		Add field myIsTransformed informing if the object is transformed.
//

#define G002	    //GG_140400
//		Add empty constructor and SetView() method.
//		Manage highlighting.
//      TCL : New method Pick(xmin, ymin, xmax, ymax,...)

#define OCC197 // jfa 01.03.2002

#define OCC154 // jfa 06.03.2002 // to erase instead of removing

#define OCC402   //SAV  14/11/01 Added PickByCircle(). Draw() was modified to be able
                 //              to draw selected elements of SetOfMarker.

#define OCC627   //SAV 16/08/02 new flag added to control visual object state

#include <Graphic2d_GraphicObject.ixx>
#include <Graphic2d_GraphicObject.pxx>

#include <Standard_ErrorHandler.hxx>

#define BASE_PRIORITY 1
#define HIGH_PRIORITY 0x7FFFFFFF
#define MAX_PRIORITY 999

Graphic2d_GraphicObject::Graphic2d_GraphicObject
       (const Handle(Graphic2d_View)& aView)
	:myOverrideColor (0),
	myCurrentIndex (1),
	myOffSet (0),
	myIsUpToDate (Standard_False),
	myIsTransformed (Standard_False),
	myPrimitives (),
    	myLayer (0),
	myTrsf (),
	myPickedIndex (0),
	myPriority(0) ,
    	myPickIndices(new TColStd_HSequenceOfInteger) {

	Plottable	= Standard_True;
	Drawable	= Standard_True;
	Pickable	= Standard_True;
	myIsDisplayed	= Standard_False;
	myIsHighlighted	= Standard_False;
	myViewPtr	= aView.operator->();
#ifdef OCC627
	myDisplayStatus = Graphic2d_DS_NONE;
#endif
}

Graphic2d_GraphicObject::Graphic2d_GraphicObject()
	:myOverrideColor (0),
	myCurrentIndex (1),
	myOffSet (0),
	myIsUpToDate (Standard_False),
	myIsTransformed (Standard_False),
	myPrimitives (),
        myLayer (0),
	myTrsf (),
	myPickedIndex (0),
	myPriority(0) ,
        myPickIndices(new TColStd_HSequenceOfInteger) {

	Plottable	= Standard_True;
	Drawable	= Standard_True;
	Pickable	= Standard_True;
	myIsDisplayed	= Standard_False;
	myIsHighlighted	= Standard_False;
	myViewPtr	= NULL;
#ifdef OCC627
	myDisplayStatus = Graphic2d_DS_NONE;
#endif
}

void Graphic2d_GraphicObject::SetView( const Handle(Graphic2d_View)& aView ) {

    myViewPtr   = aView.operator->();
}

void Graphic2d_GraphicObject::SetTransform (const gp_GTrsf2d& aTrsf,
			const Graphic2d_TypeOfComposition aType) {

        if( aType == Graphic2d_TOC_REPLACE ) {
          myTrsf = aTrsf;
        } else {
          myTrsf = aTrsf.Multiplied(myTrsf);
        }
	myIsTransformed = (aTrsf.Form() == gp_Identity) ? 
					Standard_False : Standard_True;
}

const gp_GTrsf2d& Graphic2d_GraphicObject::Transform () const {

	return myTrsf;

}

Standard_Boolean Graphic2d_GraphicObject::IsTransformed () const {
	return myIsTransformed;
}

void Graphic2d_GraphicObject::SetLayer (const Standard_Integer aLayer) {

	myLayer	= aLayer;

}

Standard_Integer Graphic2d_GraphicObject::Layer () const {

	return myLayer;

}

void Graphic2d_GraphicObject::SetPriority (const Standard_Integer aPriority) {

	if( aPriority < 0 || aPriority > MaxPriority() ) {
            Standard_OutOfRange::Raise (" Bad priority");
	}
	myPriority	= aPriority;
}

Standard_Integer Graphic2d_GraphicObject::Priority () const {

	return myPriority;

}

Standard_Integer Graphic2d_GraphicObject::BasePriority () const {

	return BASE_PRIORITY;

}

Standard_Integer Graphic2d_GraphicObject::MaxPriority () const {

	return MAX_PRIORITY;

}

void Graphic2d_GraphicObject::EnablePlot () {

	Plottable	= Standard_True;

}

void Graphic2d_GraphicObject::DisablePlot () {

	Plottable	= Standard_False;

}

Standard_Boolean Graphic2d_GraphicObject::IsPlottable () const {

	return Plottable;

}

void Graphic2d_GraphicObject::EnableDraw () {

	Drawable	= Standard_True;

}

void Graphic2d_GraphicObject::DisableDraw () {

	Drawable	= Standard_False;

}

Standard_Boolean Graphic2d_GraphicObject::IsDrawable () const {

	return Drawable;

}

Standard_Boolean Graphic2d_GraphicObject::IsIn (const Handle(Graphic2d_Primitive) & aPrimitive) const {
  return myPrimitives.Contains(aPrimitive);
}

void Graphic2d_GraphicObject::EnablePick () {

	Pickable	= Standard_True;

}

void Graphic2d_GraphicObject::DisablePick () {

	Pickable	= Standard_False;

}

Standard_Boolean Graphic2d_GraphicObject::IsPickable () const {

	return Pickable;

}

void Graphic2d_GraphicObject::SetPickedIndex (const Standard_Integer anIndex) {
      myPickedIndex = anIndex;
}

Standard_Integer Graphic2d_GraphicObject::PickedIndex () const {

	return myPickedIndex;
}

void Graphic2d_GraphicObject::Redraw (const Handle(Graphic2d_Drawer)& aDrawer) {

	myCurrentIndex	= 1;
	Draw (aDrawer,Standard_True);

}

void Graphic2d_GraphicObject::Draw (const Handle(Graphic2d_Drawer)& aDrawer, const Standard_Boolean Reset) {

	if (! Drawable) return;

	if (myIsDisplayed || myIsHighlighted) {
#ifdef G002
	  	aDrawer->SetOverrideColor (myOverrideColor);
#endif
		if (myIsHighlighted) {
          aDrawer->SetOverride (Standard_True);
#ifndef G002
          aDrawer->SetOverrideColor (myOverrideColor);
#endif
		} else {
		  aDrawer->SetOffSet (myOffSet);
		}

		Standard_Integer Length = myPrimitives.Extent ();
		Handle(Graphic2d_Primitive) aPrimitive;

		for (Standard_Integer i=myCurrentIndex; i<=Length; i++) {
		  aPrimitive = Primitive (i); 
		  // We draw the boundary rectangle of an image
		  // in a highlighted Graphic2d_GraphicObject
		  Standard_Boolean TheFlag = myIsHighlighted &&
	 			(aPrimitive->Family() == Graphic2d_TOP_IMAGE);
		  if (TheFlag) {
		    Standard_Integer ColorIndexSave = aPrimitive->ColorIndex ();
		    aPrimitive->SetColorIndex (myOverrideColor);
		    aPrimitive->Draw (aDrawer);
		    aPrimitive->SetColorIndex (ColorIndexSave);
		  } else {
#ifdef G002
		    if( aPrimitive->IsHighlighted() && !myIsHighlighted )
                    {
                      Handle(TColStd_HSequenceOfInteger) theHSeq = aPrimitive->HighlightIndices();
#ifdef OCC197
                      aPrimitive->Draw (aDrawer);
#endif
		      for ( Standard_Integer j = 1; j <= theHSeq->Length(); ++j )
                      {
                        Standard_Integer hindex = theHSeq->Value(j);
                        if( hindex != 0 ) {	// Highlight a part of primitive
#ifndef OCC197
// if we draw a primitive in cycle, it hides its own highlighted parts except the last one
		           aPrimitive->Draw (aDrawer);
#endif
		           aDrawer->SetOverride (Standard_True);
		           if( hindex > 0 )
		       	     aPrimitive->DrawElement (aDrawer,hindex);
		           else
				     aPrimitive->DrawVertex (aDrawer,-hindex);
		           aDrawer->SetOverride (Standard_False);
                         } else {	// Highlight the full primitive
		           aDrawer->SetOverride (Standard_True);
		           aPrimitive->Draw (aDrawer);
		           aDrawer->SetOverride (Standard_False);
                         }
                      } // end for 
	} 
#ifdef OCC402
	else
	  if ( aPrimitive->HasSelectedElements() ) {
	    aPrimitive->Draw (aDrawer);
	    aDrawer->SetOverride( Standard_True );
	    aPrimitive->DrawSelectedElements( aDrawer );
	    aDrawer->SetOverride( Standard_False );
	  }
#endif
	  else 
#endif
		    aPrimitive->Draw (aDrawer);
	      }
		}

		if( Reset ) myCurrentIndex = Length + 1;
		myIsUpToDate = Standard_True;
		if (myIsHighlighted) {
			aDrawer->SetOverride (Standard_False);
		}
	}

}

void Graphic2d_GraphicObject::Draw (
	const Handle(Graphic2d_Drawer)& aDrawer,
	const Handle(Graphic2d_Primitive)& aPrimitive) {

	//
	// Test deja effectue dans l'appelant
	//
	// if (! IsIn (aPrimitive)) return;

	if (! Drawable) return;

	if (myIsDisplayed || myIsHighlighted) {
#ifdef G002
		aDrawer->SetOverrideColor (myOverrideColor);
#endif
		if (myIsHighlighted) {
	  	  aDrawer->SetOverride (Standard_True);
#ifndef G002
		  aDrawer->SetOverrideColor (myOverrideColor);
#endif
		} else {
		  aDrawer->SetOffSet (myOffSet);
		}

		// We draw the boundary rectangle of an image
		// in a highlighted Graphic2d_GraphicObject
		Standard_Boolean TheFlag = myIsHighlighted &&
			(aPrimitive->Family() == Graphic2d_TOP_IMAGE);
		if (TheFlag) {
			Standard_Integer ColorIndexSave =
				aPrimitive->ColorIndex ();
			aPrimitive->SetColorIndex (myOverrideColor);
			aPrimitive->Draw (aDrawer);
			aPrimitive->SetColorIndex (ColorIndexSave);
		} else {
#ifdef G002
		    if( aPrimitive->IsHighlighted() && !myIsHighlighted ) {
              Handle(TColStd_HSequenceOfInteger) theHSeq = aPrimitive->HighlightIndices();
		      for ( int i = 1; i <= theHSeq->Length(); ++i ) {
                 Standard_Integer hindex = theHSeq->Value(i);
                 if( hindex != 0 ) {	// Highlight a part of primitive
		           aPrimitive->Draw (aDrawer);
		           aDrawer->SetOverride (Standard_True);
		           if( hindex > 0 )
		       	     aPrimitive->DrawElement (aDrawer,hindex);
		           else
				     aPrimitive->DrawVertex (aDrawer,-hindex);
		           aDrawer->SetOverride (Standard_False);
                 } else {	// Highlight the full primitive
		           aDrawer->SetOverride (Standard_True);
		           aPrimitive->Draw (aDrawer);
		           aDrawer->SetOverride (Standard_False);
                 } 
              } // end for 
		    } else 
#endif
		      aPrimitive->Draw (aDrawer);
		}
		if (myIsHighlighted) {
			aDrawer->SetOverride (Standard_False);
		}
	}
}

void Graphic2d_GraphicObject::Display () {

	if (myIsHighlighted) Unhighlight();
#ifdef OCC627
	if (! myIsDisplayed && ( myDisplayStatus != Graphic2d_DS_ERASED ) ) 
#else
	if (! myIsDisplayed )
#endif
	  myViewPtr->Add (this,myPriority + BasePriority() );
	myIsDisplayed	= Standard_True;
#ifdef OCC627
	myDisplayStatus = Graphic2d_DS_DISPLAYED;
#endif
}

void Graphic2d_GraphicObject::Erase () {

	if (myIsHighlighted) Unhighlight();
#ifndef OCC154
	if (myIsDisplayed) myViewPtr->Remove (this);
#endif
#ifdef OCC627
        if(myIsDisplayed)
          myDisplayStatus = Graphic2d_DS_ERASED;
#endif
	myIsDisplayed	= Standard_False;
	myCurrentIndex	= 1;
}

Standard_Boolean Graphic2d_GraphicObject::IsDisplayed () const {

	return myIsDisplayed;

}


void Graphic2d_GraphicObject::RemovePrimitive (const Handle(Graphic2d_Primitive)& aPrimitive) {

  Standard_Integer Index = myPrimitives.FindIndex(aPrimitive);
  if (Index == 0) return;
  Standard_Integer Length = myPrimitives.Extent ();

  for (Standard_Integer i=Index+1; i<=Length; i++)
    ((Handle(Standard_Transient)&) myPrimitives.FindKey(i-1)) = myPrimitives.FindKey(i);
  
  myPrimitives.RemoveLast ();

  myCurrentIndex = (myCurrentIndex > Index ? Index : myCurrentIndex);

#ifdef OK
	//
	// To retrace whatever is necessary
	// Parse the primitives from 1 to Index
	// and return l'Index of the first primitive
	// with bounding box intersecting the bounding
        // box of the removed primitive.
	//
Standard_Boolean stop = Standard_False;
Standard_ShortReal sMinx, sMiny, sMaxx, sMaxy;
Standard_ShortReal x, y, X, Y;
Standard_ShortReal RF	= ShortRealFirst ();
Standard_ShortReal RL	= ShortRealLast ();

	aPrimitive->MinMax (sMinx, sMaxx, sMiny, sMaxy);
	for (Standard_Integer i=1; i<Index && ! stop; i++) {
		(Primitive (i))->MinMax (x, X, y, Y);
		//
		// Infinite lines
		//
		if ((x == RF) || (X == RL) || (y == RF) || (Y == RL)) {
			// infinite horizontal
			if (y == Y) stop = (sMiny <= y) && (y <= sMaxy);
			// infinite vertical
			if (x == X) stop = (sMinx <= x) && (x <= sMaxx);
			// infinite diagonal
			if ((y != Y) && (x != X)) {
				// no interrogation possible
				// about the content of the primitive.
				stop = Standard_True;
			}
		}
		else {
			if ((x<=sMinx) && (sMinx<=X) &&
			    (y<=sMaxy) && (sMaxy<=Y)) stop = Standard_True;
			if ((x<=sMinx) && (sMinx<=X) &&
			    (y<=sMiny) && (sMiny<=Y)) stop = Standard_True;
			if ((x<=sMaxx) && (sMaxx<=X) &&
			    (y<=sMiny) && (sMiny<=Y)) stop = Standard_True;
			if ((x<=sMaxx) && (sMaxx<=X) &&
			    (y<=sMaxy) && (sMaxy<=Y)) stop = Standard_True;
		}
		if (stop) myCurrentIndex = i;
	}
#endif

}

void Graphic2d_GraphicObject::RemovePrimitives () {

	myPrimitives.Clear ();
	myCurrentIndex	= 1;

}

void Graphic2d_GraphicObject::Remove () {

#ifdef OCC627
	if ( myIsDisplayed || myIsHighlighted || 
	     ( myDisplayStatus == Graphic2d_DS_ERASED ) )
#else
	if (myIsDisplayed || myIsHighlighted) 
#endif
	  myViewPtr->Remove (this);
	myIsHighlighted	= Standard_False;
	myIsDisplayed	= Standard_False;
#ifdef OCC627
	myDisplayStatus = Graphic2d_DS_REMOVED;
#endif
	myCurrentIndex	= 1;

}

void Graphic2d_GraphicObject::Highlight () {

	if (! myViewPtr->IsDefinedColor ())
                Graphic2d_OverrideColorError::Raise
			("Undefined highlight color index");

	if (! myIsHighlighted) {
	  if (! myIsDisplayed ) 
#ifdef OCC627
	    if ( myDisplayStatus != Graphic2d_DS_ERASED )
#endif
		myViewPtr->Add (this,HIGH_PRIORITY);
#ifdef TEST
	  else 
		myViewPtr->Change (this,HIGH_PRIORITY);
#endif
	  myIsHighlighted	= Standard_True;
	}
	myOverrideColor	= myViewPtr->DefaultOverrideColor();
	myCurrentIndex = 1;

}

void Graphic2d_GraphicObject::Highlight (const Standard_Integer aColorIndex) {

	if (! myIsHighlighted) {
	  if (! myIsDisplayed ) 
#ifdef OCC627
	    if ( myDisplayStatus != Graphic2d_DS_ERASED )
#endif
		myViewPtr->Add (this,HIGH_PRIORITY);
#ifdef TEST
	  else 
		myViewPtr->Change (this,HIGH_PRIORITY);
#endif
	  myIsHighlighted	= Standard_True;
	}
	myOverrideColor	= aColorIndex;
	myCurrentIndex = 1;

}

void Graphic2d_GraphicObject::SetOffSet (const Standard_Integer anOffSet) {

	myOffSet	= anOffSet;
	myCurrentIndex	= 1;

}

Standard_Integer Graphic2d_GraphicObject::OffSet () const {

	return myOffSet;

}

Standard_Integer Graphic2d_GraphicObject::OverrideColor () const {

	return myOverrideColor;

}

void Graphic2d_GraphicObject::SetOverrideColor( const Standard_Integer indColor ) {

	 myOverrideColor = indColor;

}

void Graphic2d_GraphicObject::Unhighlight () {

	if( myIsHighlighted ) {
#ifdef TEST
	  if ( myIsDisplayed ) 
		myViewPtr->Change (this,myPriority + BasePriority() );
	  else 
#else
	  if ( !myIsDisplayed )
#endif
		myViewPtr->Remove (this); 
	  myIsHighlighted	= Standard_False;
	}
	myCurrentIndex	= 1;

}

Standard_Boolean Graphic2d_GraphicObject::IsHighlighted () const {

	return myIsHighlighted;

}

Handle(Graphic2d_View) Graphic2d_GraphicObject::View () const {

	return ((Graphic2d_View*) myViewPtr);

}

Standard_Boolean Graphic2d_GraphicObject::Pick (const Standard_Real X, const Standard_Real Y, const Standard_Real aPrecision, const Handle(Graphic2d_Drawer)& aDrawer) {

#ifndef G002
  myPickedIndex = 0;
#else
  myPickIndices->Clear();
#endif
  if (Pickable && (myIsDisplayed || myIsHighlighted))
 {
    Standard_ShortReal XX	= Standard_ShortReal (X);
    Standard_ShortReal YY	= Standard_ShortReal (Y);
    Standard_ShortReal P	= Standard_ShortReal (aPrecision);
    Standard_Integer i;
    Handle(Graphic2d_Primitive) thePrimitive;
    Standard_ShortReal Minx, Maxx, Miny, Maxy;

    for( i=1 ; i<=myPrimitives.Extent () ; i++ ) {
      thePrimitive = Primitive(i);
      thePrimitive->MinMax(Minx, Maxx, Miny, Maxy);
      if(XX < Minx || Maxx < XX || YY < Miny || Maxy < YY)
	continue;
      if( thePrimitive->Pick(XX,YY,P,aDrawer) ) {
#ifdef G002
        SetPickedIndex(i);
#else
        myPickedIndex = i;
#endif
	return Standard_True;
      }
    }
    return Standard_False;
  }
  else
    return Standard_False;
}

Standard_Boolean Graphic2d_GraphicObject
::PickByCircle( const Standard_Real X,
	        const Standard_Real Y,
	        const Standard_Real Radius,
	        const Handle(Graphic2d_Drawer)& aDrawer )
{
#ifdef OCC402

  myPickIndices->Clear();

  if ( Pickable && ( myIsDisplayed || myIsHighlighted ) ) {
    Standard_ShortReal XX	= Standard_ShortReal( X );
    Standard_ShortReal YY	= Standard_ShortReal( Y );
    Standard_ShortReal radius	= Standard_ShortReal( Radius );
    Standard_Integer i;
    Handle(Graphic2d_Primitive) thePrimitive;
    Standard_ShortReal Minx, Maxx, Miny, Maxy;
    
    for( i=1 ; i<=myPrimitives.Extent() ; i++ ) {
      thePrimitive = Primitive( i );
      thePrimitive->MinMax(Minx, Maxx, Miny, Maxy);
      if((XX+radius) < Minx || Maxx < (XX-radius) || (YY+radius) < Miny || Maxy < (YY-radius))
	continue;
      if( thePrimitive->PickByCircle( XX, YY, radius, aDrawer ) )
	myPickIndices->Append(i);
    }
    if ( myPickIndices->Length() > 0 ) 
      return Standard_True;
    return Standard_False;
  }
  else
    return Standard_False;
#endif
  return Standard_False;
}

Standard_Boolean Graphic2d_GraphicObject::Pick( const Standard_Real Xmin,
                                         const Standard_Real Ymin,
                                         const Standard_Real Xmax,
                                         const Standard_Real Ymax,
                                         const Handle(Graphic2d_Drawer)& aDrawer,
                                         const Graphic2d_PickMode aPickMode ) {
  myPickIndices->Clear();

  if ( Pickable && (myIsDisplayed || myIsHighlighted) ) {
    Standard_ShortReal X1 = Standard_ShortReal( (Xmin<Xmax)?Xmin:Xmax );
    Standard_ShortReal Y1 = Standard_ShortReal( (Ymin<Ymax)?Ymin:Ymax );
    Standard_ShortReal X2 = Standard_ShortReal( (Xmin<Xmax)?Xmax:Xmin );
    Standard_ShortReal Y2 = Standard_ShortReal( (Ymin<Ymax)?Ymax:Ymin );
    
    Standard_Integer i;
    
    Handle(Graphic2d_Primitive) thePrimitive;
    Standard_ShortReal Minx, Maxx, Miny, Maxy;

    for ( i = 1; i <= myPrimitives.Extent(); i++ ) {
      thePrimitive = Primitive(i);
      thePrimitive->MinMax(Minx, Maxx, Miny, Maxy);
      if(X2 < Minx || Maxx < X1 || Y2 < Miny || Maxy < Y1)
	continue;
      if ( thePrimitive->Pick( X1,Y1,X2, Y2, aDrawer, aPickMode ) ) 
        myPickIndices->Append(i);
    }
    if ( myPickIndices->Length() > 0 ) 
	       return Standard_True;

    return Standard_False;
 }
  else
    return Standard_False;

}

Handle(TColStd_HSequenceOfInteger) Graphic2d_GraphicObject::PickList() const {

 return myPickIndices;

}

void Graphic2d_GraphicObject::AddPrimitive (const Handle(Graphic2d_Primitive)& aPrimitive) {
  Standard_Integer ColorIndex = aPrimitive->ColorIndex ();
  aPrimitive->SetColorIndex (ColorIndex + myOffSet);
  myPrimitives.Add (aPrimitive);
  myIsUpToDate = Standard_False;
}

Standard_Integer Graphic2d_GraphicObject::Length() const {

	return myPrimitives.Extent() ;
}

Handle(Graphic2d_Primitive) Graphic2d_GraphicObject::Primitive(const Standard_Integer aRank) const {
  // modif : EAV , 23-09-01 
  static Handle(Graphic2d_Primitive) null;
  try {
    OCC_CATCH_SIGNALS
      return Handle(Graphic2d_Primitive)::DownCast(myPrimitives.FindKey(aRank)) ;
  }
  catch( Standard_OutOfRange ) {
    return null;
  }
  // modif end 
}

void Graphic2d_GraphicObject::SetIndex (const Handle(Graphic2d_Primitive)& aPrimitive) {

  Standard_Integer Index = myPrimitives.FindIndex(aPrimitive);

  if (Index == 0) return;
  myCurrentIndex = (myCurrentIndex > Index ? Index : myCurrentIndex);

}

Standard_Boolean Graphic2d_GraphicObject::MinMax (Quantity_Length & Minx, Quantity_Length & Maxx, Quantity_Length & Miny, Quantity_Length & Maxy) const {

  Standard_ShortReal RL, RF, sMinx, sMiny, sMaxx, sMaxy;
  Standard_Boolean status = Standard_True;
  Handle(Graphic2d_Primitive) thePrimitive = NULL;

	RL = sMinx = sMiny = ShortRealLast ();
	RF = sMaxx = sMaxy = ShortRealFirst();

	if (myIsDisplayed || myIsHighlighted) {

		Standard_Integer Length = myPrimitives.Extent ();
		Standard_ShortReal x, y, X, Y;
		Standard_Boolean TheFlag;

		for (Standard_Integer i=1; i<=Length; i++) {
		  thePrimitive = Primitive (i);
		  TheFlag = 
		    (thePrimitive->Family() == Graphic2d_TOP_MARKER);
		    if (! TheFlag) {
		      if( thePrimitive->MinMax (x, X, y, Y) ) {
			// Tests for cause of infinite lines
			if (x != RF) sMinx = (sMinx < x ? sMinx : x);
			if (X != RL) sMaxx = (sMaxx > X ? sMaxx : X);
			if (y != RF) sMiny = (sMiny < y ? sMiny : y);
			if (Y != RL) sMaxy = (sMaxy > Y ? sMaxy : Y);
		      }
		    }
		}
	}

	// Attention, it is possible that :
	// sMinx = sMiny = ShortRealLast (); and
	// sMaxx = sMaxy = ShortRealFirst();
	if (sMinx > sMaxx) {
	  status = Standard_False;
	  sMinx = ShortRealFirst (); sMaxx = ShortRealLast ();
	}
	if (sMiny > sMaxy) {
	  status = Standard_False;
	  sMiny = ShortRealFirst (); sMaxy = ShortRealLast (); 
	}
	Minx	= sMinx;
	Miny	= sMiny;
	Maxx	= sMaxx;
	Maxy	= sMaxy;

	return status;

}

Standard_Boolean Graphic2d_GraphicObject::MarkerMinMax (Quantity_Length & Minx, Quantity_Length & Maxx, Quantity_Length & Miny, Quantity_Length & Maxy) const {

  Standard_ShortReal RL, RF, sMinx, sMiny, sMaxx, sMaxy;
  Standard_Boolean status = Standard_True;
  Handle(Graphic2d_Primitive) thePrimitive = NULL;

	RL = sMinx = sMiny = ShortRealLast ();
	RF = sMaxx = sMaxy = ShortRealFirst();

	if (myIsDisplayed || myIsHighlighted) {

		Standard_Integer Length = myPrimitives.Extent ();
		Standard_ShortReal x, y, X, Y;
		Standard_Boolean TheFlag;

		for (Standard_Integer i=1; i<=Length; i++) {
		  thePrimitive = Primitive(i);
		  TheFlag = 
		    (thePrimitive->Family() == Graphic2d_TOP_MARKER);
		    if (TheFlag) {
		      if( thePrimitive->MinMax (x, X, y, Y) ) {
			// Tests for cause of infinite lines
			if (x != RF) sMinx = (sMinx < x ? sMinx : x);
			if (X != RL) sMaxx = (sMaxx > X ? sMaxx : X);
			if (y != RF) sMiny = (sMiny < y ? sMiny : y);
			if (Y != RL) sMaxy = (sMaxy > Y ? sMaxy : Y);
		      }
		    }
		}
	}

	// Attention, it is possible that :
	// sMinx = sMiny = ShortRealLast (); and
	// sMaxx = sMaxy = ShortRealFirst();
	if (sMinx > sMaxx) {
	  status = Standard_False;
	  sMinx = ShortRealFirst (); sMaxx = ShortRealLast (); 
	}
	if (sMiny > sMaxy) {
	  status = Standard_False;
	  sMiny = ShortRealFirst (); sMaxy = ShortRealLast (); 
	}
	Minx	= sMinx;
	Miny	= sMiny;
	Maxx	= sMaxx;
	Maxy	= sMaxy;

	return status;
}

Standard_Boolean Graphic2d_GraphicObject::IsUpToDate() const {
	
	return myIsUpToDate;
}
