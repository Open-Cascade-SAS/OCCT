// Modified     23/02/98 : FMN ; Remplacement PI par Standard_PI
// S3824	10/06/98 : GG ; 
//      	1) La methode "Fit" comporte un argum optionnel supplementaire
//		   "Expand" permettant de ne pas ajuster la largeur du texte
//		    lorsque celle ci est < a la taille du FIT.
//		2) La methode "Trunc" permet de tronquer le texte a la
//		   taille precisee.

#define PERF	//GG_200898
//		The MinMax are now computed at the right time and no more
//		soon an attribute has change 
//		(see the redefined method ComputeMinMax()).

#define BUC60583        //GG_300999 Enable to compute correctly a
//                      Mirror transformation

#include <Graphic2d_Text.ixx>
#include <Aspect_WindowDriver.hxx>

#include <Graphic2d_Primitive.pxx>

Graphic2d_Text::Graphic2d_Text
   (const Handle(Graphic2d_GraphicObject)& aGraphicObject,
    const TCollection_ExtendedString& aText,
    const Standard_Real X, const Standard_Real Y,
    const Quantity_PlaneAngle anAngle,
    const Aspect_TypeOfText aType,
    const Quantity_Factor aScale)
    : Graphic2d_Primitive (aGraphicObject),
      myFontIndex (0),
      myX(Standard_ShortReal(X)),
      myDx(Standard_ShortReal(0.)),
      myY(Standard_ShortReal(Y)),
      myDy(Standard_ShortReal(0.)),
      myAngle(Standard_ShortReal(anAngle)),
      myType(aType),
      myText(aText),
      myAdjustFlag(Standard_False), 
      myDeltax(Standard_ShortReal(0.)), myDeltay(Standard_ShortReal(0.)),
      mySlant(Standard_ShortReal(0.)),
      myIsZoomable(Standard_True),
      myIsUnderlined(Standard_False),
      myHScale(Standard_ShortReal(aScale)),
      myWScale(Standard_ShortReal(aScale)),
      myAlignment(Graphic2d_TOA_LEFT)
#ifndef PERF
      ,myResetFlag(Standard_True),
#endif
{

      SetFamily(Graphic2d_TOP_TEXT);

      while (myAngle < 0.) myAngle += Standard_ShortReal(2.*Standard_PI);
      while (myAngle >= 2.*Standard_PI) myAngle -= Standard_ShortReal(2.*Standard_PI);

#ifndef PERF
      this->ComputeMinMax();
#endif

}

void Graphic2d_Text::SetFontIndex (const Standard_Integer anIndex) {

	myFontIndex = anIndex;
#ifdef PERF
	myMinX = myMinY = ShortRealLast ();
	myMaxX = myMaxY = ShortRealFirst ();
#else
	myResetFlag = Standard_True;
        this->ComputeMinMax();
#endif

}

void Graphic2d_Text::SetSlant (const Quantity_PlaneAngle aSlant) {

	mySlant = Standard_ShortReal(aSlant);
        while (mySlant < 0.) mySlant += Standard_ShortReal(2.*Standard_PI);
        while (mySlant >= 2.*Standard_PI) mySlant -= Standard_ShortReal(2.*Standard_PI);
#ifdef PERF
	myMinX = myMinY = ShortRealLast ();
	myMaxX = myMaxY = ShortRealFirst ();
#else
	myResetFlag = Standard_True;
        this->ComputeMinMax();
#endif
}

Standard_Boolean Graphic2d_Text::IsZoomable () const {

        return myIsZoomable;

}

Standard_Boolean Graphic2d_Text::IsUnderlined () const {

        return myIsUnderlined;

}

void Graphic2d_Text::SetZoomable (const Standard_Boolean aFlag) {

        myIsZoomable    = aFlag;
#ifdef PERF
	myMinX = myMinY = ShortRealLast ();
	myMaxX = myMaxY = ShortRealFirst ();
#else
	myResetFlag = Standard_True;
        this->ComputeMinMax();
#endif

}

void Graphic2d_Text::SetUnderline (const Standard_Boolean aFlag) {

        myIsUnderlined   = aFlag;
#ifdef PERF
	myMinX = myMinY = ShortRealLast ();
	myMaxX = myMaxY = ShortRealFirst ();
#else
	myResetFlag = Standard_True;
        this->ComputeMinMax();
#endif

}

void Graphic2d_Text::SetOffset (const Standard_Real aDx, const Standard_Real aDy) {

	myDeltax = Standard_ShortReal(aDx);
	myDeltay = Standard_ShortReal(aDy);
#ifdef PERF
	myMinX = myMinY = ShortRealLast ();
	myMaxX = myMaxY = ShortRealFirst ();
#else
	myResetFlag = Standard_True;
        this->ComputeMinMax();
#endif
}

void Graphic2d_Text::SetAlignment (const Graphic2d_TypeOfAlignment anAlignment) {

        myAlignment   = anAlignment;
#ifdef PERF
	myMinX = myMinY = ShortRealLast ();
	myMaxX = myMaxY = ShortRealFirst ();
#else
	myResetFlag = Standard_True;
        this->ComputeMinMax();
#endif
}

Graphic2d_TypeOfAlignment Graphic2d_Text::Alignment () const {

        return myAlignment;

}

void Graphic2d_Text::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {
Standard_Boolean IsIn = Standard_False;
Standard_ShortReal hscale = (myIsZoomable) ? Standard_ShortReal(myHScale * aDrawer->Scale()) : Standard_ShortReal(myHScale);
Standard_ShortReal wscale = (myIsZoomable) ? Standard_ShortReal(myWScale * aDrawer->Scale()) : Standard_ShortReal(myWScale);
Standard_ShortReal ox = aDrawer->ConvertMapToFrom(myDeltax);
Standard_ShortReal oy = aDrawer->ConvertMapToFrom(myDeltay);

#ifdef PERF
	if( (myMaxX < myMinX) || (myMaxY < myMinY) ) {
	  if( !ComputeMinMax() ) return;
	}
#else
	if( myResetFlag ) {
	  ComputeMinMax();
	}
#endif
 
  	if (! myGOPtr->IsTransformed ()) {
    	  IsIn = aDrawer->IsIn (myMinX+ox,myMaxX+ox,myMinY+oy,myMaxY+oy);
  	  if (IsIn) {
            aDrawer->SetTextAttrib(myColorIndex,myFontIndex,
					mySlant,hscale,wscale,myIsUnderlined);
	    Standard_ShortReal dx = myDx,dy = myDy;
	    if( myAngle != 0. ) {
	      Standard_ShortReal cosa = Standard_ShortReal(Cos(myAngle));
	      Standard_ShortReal sina = Standard_ShortReal(Sin(myAngle));

              dx = XROTATE(myDx,myDy) ;
              dy = YROTATE(myDx,myDy) ;
	    }
            aDrawer->MapTextFromTo(myText,myX+dx,myY+dy,myAngle,
						myDeltax,myDeltay,myType);
	  }
  	} else {
	  gp_GTrsf2d aTrsf = myGOPtr->Transform ();
	  Standard_ShortReal minx,miny,maxx,maxy;
	  MinMax(minx,maxx,miny,maxy);
    	  IsIn = aDrawer->IsIn (minx+ox,maxx+ox,miny+oy,maxy+oy);
  	  if (IsIn) {
	    Standard_Real A = Standard_Real (myX); 
	    Standard_Real B = Standard_Real (myY);
	    Standard_Real C = Standard_Real (myAngle);
	    Standard_Real cosa = Cos (C);
	    Standard_Real sina = Sin (C);
	    aTrsf.Transforms (A, B);
	    Standard_ShortReal a = Standard_ShortReal (A);   
	    Standard_ShortReal b = Standard_ShortReal (B);
#ifdef BUC60583
            aTrsf.Transforms (cosa, sina);
            A = B = 0.;
            aTrsf.Transforms (A, B);
            cosa -= A; sina -= B;
            if( aTrsf.IsNegative() ) hscale = - hscale;
#else
	    // Pour calculer les nouveaux angles d'ouverture
	    // le calcul est fait sur le cercle trigonometrique
	    // et l'on tient pas compte dans ce cas de la translation.
	    aTrsf.SetValue (1, 3, 0.0);
	    aTrsf.SetValue (2, 3, 0.0);
            aTrsf.Transforms (cosa, sina);
#endif
            Standard_ShortReal angle = Standard_ShortReal (atan2(sina,cosa));
            if( myIsZoomable ) {
              hscale *= Standard_ShortReal (Sqrt(cosa*cosa + sina*sina));
              wscale *= Standard_ShortReal (Sqrt(cosa*cosa + sina*sina));
	    }
 
            aDrawer->SetTextAttrib(myColorIndex,myFontIndex,
					mySlant,hscale,wscale,myIsUnderlined);
	    Standard_ShortReal dx = myDx,dy = myDy;
	    if( angle != 0. ) {
              dx = Standard_ShortReal(XROTATE(myDx,myDy));
              dy = Standard_ShortReal(YROTATE(myDx,myDy));
	    }
	    a += dx; b += dy;
            aDrawer->MapTextFromTo(myText,a,b,angle,myDeltax,myDeltay,myType);
	  }
	}
}


Standard_Boolean Graphic2d_Text::Pick (const Standard_ShortReal X,
				       const Standard_ShortReal Y,
				       const Standard_ShortReal aPrecision,
				       const Handle(Graphic2d_Drawer)& aDrawer) 
{
Standard_Boolean theStatus = Standard_False;
Standard_ShortReal width,height,xoffset,yoffset;
Standard_ShortReal hscale = (myIsZoomable) ? Standard_ShortReal(myHScale * aDrawer->Scale()) : Standard_ShortReal(myHScale);
Standard_ShortReal wscale = (myIsZoomable) ? Standard_ShortReal(myWScale * aDrawer->Scale()) : Standard_ShortReal(myWScale);
Standard_ShortReal TX = X, TY = Y;
Standard_ShortReal ox = aDrawer->ConvertMapToFrom(myDeltax);
Standard_ShortReal oy = aDrawer->ConvertMapToFrom(myDeltay);

  if (IsInMinMax (X-ox, Y-oy, aPrecision)) {
    if (myGOPtr->IsTransformed ()) {
        gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
        Standard_Real RX = Standard_Real (X), RY = Standard_Real (Y);
        aTrsf.Transforms (RX, RY); 
        TX = Standard_ShortReal (RX); TY = Standard_ShortReal (RY);
    }
  
    aDrawer->SetTextAttrib(myColorIndex,myFontIndex,
					mySlant,hscale,wscale,myIsUnderlined);
    if( !aDrawer->GetTextSize(myText,width,height,xoffset,yoffset) ) {
      width = height = xoffset = yoffset = 0.;
    }
    Standard_ShortReal cosa = Standard_ShortReal(Cos(-myAngle));
    Standard_ShortReal sina = Standard_ShortReal(Sin(-myAngle));
    Standard_ShortReal dx = TX-(myX+ox+myDx);
    Standard_ShortReal dy = TY-(myY+oy+myDy);
    Standard_ShortReal x = XROTATE(dx,dy);
    Standard_ShortReal y = YROTATE(dx,dy);

    theStatus =  (x >= -aPrecision + xoffset)            
            &&   (x <= width + xoffset + aPrecision)
            &&   (y >= -yoffset - aPrecision) 
            &&   (y <= height - yoffset + aPrecision);
  }

  return theStatus;
}

Standard_Boolean Graphic2d_Text::TextSize ( Quantity_Length &aWidth,
				   	    Quantity_Length &aHeight) const {
Handle(Graphic2d_Drawer) aDrawer = Drawer(); 

    if( !aDrawer.IsNull() && aDrawer->IsWindowDriver() ) {
  	Standard_ShortReal hscale = (myIsZoomable) ? Standard_ShortReal(myHScale * aDrawer->Scale()) : Standard_ShortReal(myHScale);
  	Standard_ShortReal wscale = (myIsZoomable) ? Standard_ShortReal(myWScale * aDrawer->Scale()) : Standard_ShortReal(myWScale);
	Standard_ShortReal width,height;
        aDrawer->SetTextAttrib(myColorIndex,myFontIndex,
					mySlant,hscale,wscale,myIsUnderlined);

	aDrawer->GetTextSize(myText,width,height);
	aWidth = width;
	aHeight = height;
        return Standard_True;
    } else {
	aWidth = aHeight = 0.;
        return Standard_False;
    }

}

Standard_Boolean Graphic2d_Text::TextSize ( Quantity_Length &aWidth,
				   	    Quantity_Length &aHeight,
					    Quantity_Length &anXoffset,
					    Quantity_Length &anYoffset) const {
Handle(Graphic2d_Drawer) aDrawer = Drawer(); 

    if( !aDrawer.IsNull() && aDrawer->IsWindowDriver() ) {
  	Standard_ShortReal hscale = (myIsZoomable) ? Standard_ShortReal(myHScale * aDrawer->Scale()) : Standard_ShortReal(myHScale);
  	Standard_ShortReal wscale = (myIsZoomable) ? Standard_ShortReal(myWScale * aDrawer->Scale()) : Standard_ShortReal(myWScale);
	Standard_ShortReal width,height,xoffset,yoffset;
        aDrawer->SetTextAttrib(myColorIndex,myFontIndex,
					mySlant,hscale,wscale,myIsUnderlined);

	aDrawer->GetTextSize(myText,width,height,xoffset,yoffset);
	aWidth = width;
	aHeight = height;
	anXoffset = xoffset;
	anYoffset = yoffset;
        return Standard_True;
    } else {
	aWidth = aHeight = anXoffset = anYoffset = 0.;
        return Standard_False;
    }

}

void Graphic2d_Text::Position( Quantity_Length &X,
			       Quantity_Length &Y) const {
Handle(Graphic2d_Drawer) aDrawer = Drawer();
 
    X = myX;
    Y = myY;
    if( !aDrawer.IsNull() ) {
        X += aDrawer->ConvertMapToFrom(myDeltax);
        Y += aDrawer->ConvertMapToFrom(myDeltay);
    }    
}

void Graphic2d_Text::Offset( Standard_Real &X,
			     Standard_Real &Y) const {

	X = myDeltax;
	Y = myDeltay;
}

Quantity_PlaneAngle Graphic2d_Text::Slant() const {

	return mySlant;
}

Quantity_PlaneAngle Graphic2d_Text::Angle() const {

	return myAngle;
}

Standard_Integer Graphic2d_Text::FontIndex() const {

	return myFontIndex;
}

Quantity_Factor Graphic2d_Text::Scale() const {

	return myHScale;
}

Standard_Boolean Graphic2d_Text::Fit(const Quantity_Length aWidth,const Quantity_Length aHeight,const Standard_Boolean Adjust,const Standard_Boolean Expand) {
Quantity_Length twidth,theight,txoffset,tyoffset;
Standard_ShortReal wscale,hscale;
Standard_Boolean status;

    myAdjustFlag = Adjust;
    if(( status = TextSize(twidth,theight,txoffset,tyoffset) )) {
      wscale = Standard_ShortReal(aWidth/twidth);
      if( wscale > 0. ) {
	if( Expand || (twidth > aWidth) ) myWScale *= wscale;
      }
      hscale = Standard_ShortReal(aHeight/theight);
      if( hscale > 0. ) myHScale *= hscale;
#ifdef PERF
      myMinX = myMinY = ShortRealLast ();
      myMaxX = myMaxY = ShortRealFirst ();
#else
      myResetFlag = Standard_True;
      this->ComputeMinMax();
#endif
    }

    return status;
}

Standard_Boolean Graphic2d_Text::Trunc(const Quantity_Length aWidth) {
Quantity_Length twidth,theight,txoffset,tyoffset;
//Standard_ShortReal wscale,hscale;
Standard_Boolean status;

    if(( status = TextSize(twidth,theight,txoffset,tyoffset) )) {
      Standard_Integer l = myText.Length();
      while( (l > 1) && (twidth > aWidth) ) {
        --l;
	myText.Split(l); 
        TextSize(twidth,theight,txoffset,tyoffset);
      } 
#ifdef PERF
      myMinX = myMinY = ShortRealLast ();
      myMaxX = myMaxY = ShortRealFirst ();
#else
      myResetFlag = Standard_True;
      this->ComputeMinMax();
#endif
    }

    return status;
}

Standard_Boolean Graphic2d_Text::ComputeMinMax() {
Handle(Graphic2d_Drawer) aDrawer = Drawer(); 
Standard_Boolean status = Standard_False;

    if( !aDrawer.IsNull() && aDrawer->IsWindowDriver() ) {
	  Standard_ShortReal hscale = 
		(myIsZoomable) ? Standard_ShortReal(myHScale * aDrawer->Scale()) : Standard_ShortReal(myHScale);
	  Standard_ShortReal wscale = 
		(myIsZoomable) ? Standard_ShortReal(myWScale * aDrawer->Scale()) : Standard_ShortReal(myWScale);
          Standard_ShortReal width,height,xoffset,yoffset;
          aDrawer->SetTextAttrib(myColorIndex,myFontIndex,
					mySlant,hscale,wscale,myIsUnderlined);
          if(( status = 
		aDrawer->GetTextSize(myText,width,height,xoffset,yoffset) )) {
#ifndef PERF
	    myResetFlag = Standard_False;
#endif
	    switch( myAlignment ) {
	      case Graphic2d_TOA_LEFT:
		myDx = 0.; myDy = 0.;
		break;
	      case Graphic2d_TOA_RIGHT:
		myDx = -width; myDy = 0.;
		break;
	      case Graphic2d_TOA_CENTER:
		myDx = Standard_ShortReal(-width/2.); myDy = Standard_ShortReal(0.);
		break;
              case Graphic2d_TOA_TOPLEFT:
                myDx = 0.; myDy = yoffset-height;
                break;
              case Graphic2d_TOA_TOPRIGHT:
                myDx = -width; myDy = yoffset-height;
                break;
              case Graphic2d_TOA_TOPCENTER:
                myDx = Standard_ShortReal(-width/2.); myDy = yoffset-height;
                break;
              case Graphic2d_TOA_MEDIUMLEFT:
                myDx = 0.; myDy = Standard_ShortReal((yoffset-height)/2.);
                break;
              case Graphic2d_TOA_MEDIUMRIGHT:
                myDx = -width; myDy = Standard_ShortReal((yoffset-height)/2.);
                break;
              case Graphic2d_TOA_MEDIUMCENTER:
                myDx = Standard_ShortReal(-width/2.); myDy = Standard_ShortReal((yoffset-height)/2.);
                break;
              case Graphic2d_TOA_BOTTOMLEFT:
                myDx = 0.; myDy = yoffset;
                break;
              case Graphic2d_TOA_BOTTOMRIGHT:
                myDx = -width; myDy = yoffset;
                break;
              case Graphic2d_TOA_BOTTOMCENTER:
                myDx = Standard_ShortReal(-width/2.); myDy = yoffset;
                break;
	    }
	    if( myAdjustFlag ) {
	      myDx -= xoffset; myDy += yoffset;
	    }
	    Standard_ShortReal xmin = myDx + xoffset;
	    Standard_ShortReal ymin = myDy - yoffset;
	    Standard_ShortReal xmax = xmin + width;
	    Standard_ShortReal ymax = ymin + height;

    	    myMinX = myMinY = ShortRealLast();
    	    myMaxX = myMaxY = ShortRealFirst();
	    if( myAngle != 0. ) {
	      Standard_ShortReal cosa = Standard_ShortReal(Cos(myAngle));
	      Standard_ShortReal sina = Standard_ShortReal(Sin(myAngle));
	      Standard_ShortReal dx,dy ;

              dx = XROTATE(xmin,ymin) ;
              dy = YROTATE(xmin,ymin) ;
              myMinX = Min(myMinX,myX+dx) ;
              myMinY = Min(myMinY,myY+dy) ;
              myMaxX = Max(myMaxX,myX+dx) ;
              myMaxY = Max(myMaxY,myY+dy) ;

              dx = XROTATE(xmax,ymin) ;
              dy = YROTATE(xmax,ymin) ;
              myMinX = Min(myMinX,myX+dx) ;
              myMinY = Min(myMinY,myY+dy) ;
              myMaxX = Max(myMaxX,myX+dx) ;
              myMaxY = Max(myMaxY,myY+dy) ;

              dx = XROTATE(xmax,ymax) ;
              dy = YROTATE(xmax,ymax) ;
              myMinX = Min(myMinX,myX+dx) ;
              myMinY = Min(myMinY,myY+dy) ;
              myMaxX = Max(myMaxX,myX+dx) ;
              myMaxY = Max(myMaxY,myY+dy) ;

              dx = XROTATE(xmin,ymax) ;
              dy = YROTATE(xmin,ymax) ;
              myMinX = Min(myMinX,myX+dx) ;
              myMinY = Min(myMinY,myY+dy) ;
              myMaxX = Max(myMaxX,myX+dx) ;
              myMaxY = Max(myMaxY,myY+dy) ;
            } else {
              myMinX = Min(myMinX,myX+xmin) ;
              myMinY = Min(myMinY,myY+ymin) ;
              myMaxX = Max(myMaxX,myX+xmax) ;
              myMaxY = Max(myMaxY,myY+ymax) ;
	    }
	  }
	}
#ifdef PERF
	 else {
	  cout << "*Graphic2d_Text::ComputeMinMax() returns wrong values*" << endl;
	}
#endif

	return status;
}


TCollection_ExtendedString Graphic2d_Text::GetText() const {

    return myText;
}

Aspect_TypeOfText Graphic2d_Text::GetType() const {
    
    return myType;
}

void Graphic2d_Text::Save(Aspect_FStream& aFStream) const
{
}
 

