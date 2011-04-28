/*=====================================================================

     FONCTION :
     ----------
        Classe Graphic2d_Paragraph

     TEST :
     ------

        Voir TestG2D/TestG21

=====================================================================*/

#define PRO12982  //GG_230498
//            Take into account the underlining in calculation of 
//            paragraph size.

#define PRO11004  //GG_051197
//            The contour of the paragraph is not rectangle

#define PRO11005  //GG_131197
//            Text can't be seen in mode Highlight.

#define PRO11041 //GG_201197
//            Modification of SetCurrentFont(...,aHScale,aWScale)
//            to parameterize the size of the text.

#define PRO12320//GG_120297
//            Compose texts depending on the summary height of the
//            chain of characters.

#define PERF    //GG_200898
//              The MinMax are now computed at the right time and no more
//              soon an attribute has change
//              (see the redefined method ComputeMinMax()).

#include <Graphic2d_Paragraph.ixx>
#include <Aspect_WindowDriver.hxx>
#include <TShort_Array1OfShortReal.hxx>

#include <Graphic2d_Primitive.pxx>

#define MAXCOLUMN 16
#define MAXROW 256
#define MAXCOLOR 64
#define MAXFONT 256
#define MAXALIGNMENT 16
#define MAXUNDERLINE 2
#define TEXT_DESCRIPTOR(row,column,color,font,align,underline) \
        ((column) | (row)*MAXCOLUMN | (color)*MAXCOLUMN*MAXROW | \
        (font)*MAXCOLUMN*MAXROW*MAXCOLOR | \
        (align)*MAXCOLUMN*MAXROW*MAXCOLOR*MAXFONT | \
        (underline)*MAXCOLUMN*MAXROW*MAXCOLOR*MAXFONT*MAXALIGNMENT)
#define TEXT_POSITION(descriptor) ((descriptor)&(MAXCOLUMN*MAXROW-1))
#define TEXT_COLUMN(descriptor) ((descriptor)&(MAXCOLUMN-1)) 
#define TEXT_ROW(descriptor) (((descriptor)/MAXCOLUMN)&(MAXROW-1)) 
#define TEXT_COLOR(descriptor) (((descriptor)/(MAXCOLUMN*MAXROW))&(MAXCOLOR-1)) 
#define TEXT_FONT(descriptor) (((descriptor)/(MAXCOLUMN*MAXROW*MAXCOLOR))&(MAXFONT-1)) 
#define TEXT_ALIGNMENT(descriptor) (((descriptor)/(MAXCOLUMN*MAXROW*MAXCOLOR*MAXFONT))&(MAXALIGNMENT-1))
#define TEXT_UNDERLINE(descriptor) (((descriptor)/(MAXCOLUMN*MAXROW*MAXCOLOR*MAXFONT*MAXALIGNMENT))&(MAXUNDERLINE-1))

Graphic2d_Paragraph::Graphic2d_Paragraph
   (const Handle(Graphic2d_GraphicObject)& aGraphicObject,
    const Standard_Real X, const Standard_Real Y,
    const Quantity_PlaneAngle anAngle,
    const Aspect_CardinalPoints anOffset,
    const Quantity_Factor aScale)
    : Graphic2d_Primitive (aGraphicObject),
      myX( Standard_ShortReal( X ) ), 
      myY( Standard_ShortReal( Y ) ),
      myXoffset(0.),
      myYoffset(0.),
      myOffset(anOffset),
      myWidth(0.),
      myHeight(0.),
      myAngle(Standard_ShortReal( anAngle )),
      myScale(Standard_ShortReal( aScale )),
      myTextStringList(),
      myTextDescriptorList(),
      myTextXpositionList(),
      myTextYpositionList(),
      myTextFheightList(),
      myTextFoffsetList(),
      myTextHScaleList(),
      myTextWScaleList() {

      while (myAngle < 0.) myAngle += Standard_ShortReal( 2.*Standard_PI );
      while (myAngle >= 2.*Standard_PI) myAngle -= Standard_ShortReal( 2.*Standard_PI );

      SetZoomable() ;
      SetSlant() ;
      SetSpacing() ;
      SetMargin() ;
      SetFrameColorIndex() ;
      SetFrameWidthIndex() ;
      SetHidingColorIndex() ;
      SetCurrentColorIndex() ;
      SetCurrentFontIndex() ;
      SetCurrentAlignment() ;
      SetCurrentUnderline() ;
}

void Graphic2d_Paragraph::SetHidingColorIndex (const Standard_Integer anIndex)
{

        myHidingColorIndex = anIndex;

}

void Graphic2d_Paragraph::SetFrameColorIndex (const Standard_Integer anIndex) {

        myFrameColorIndex = anIndex;

}

void Graphic2d_Paragraph::SetFrameWidthIndex (const Standard_Integer anIndex) {

        myFrameWidthIndex = anIndex;

}

void Graphic2d_Paragraph::SetCurrentColorIndex (const Standard_Integer anIndex) {

        myCurrentColorIndex = anIndex;

}

void Graphic2d_Paragraph::SetCurrentFontIndex (const Standard_Integer anIndex,const Quantity_Length aHScale,const Quantity_Length aWScale) {

        myCurrentFontIndex = anIndex;
        myCurrentFontHScale = (aHScale > 0.) ? 
            Standard_ShortReal( aHScale ) : Standard_ShortReal( 1. );
        myCurrentFontWScale = 
                        (aWScale > 0.) ? Standard_ShortReal( aWScale ) : myCurrentFontHScale;

}

void Graphic2d_Paragraph::SetCurrentAlignment (const Graphic2d_TypeOfAlignment anAlignment) {

        myCurrentAlignment = anAlignment;
}

void Graphic2d_Paragraph::SetCurrentUnderline (const Standard_Boolean isUnderlined) {

        myCurrentUnderline = isUnderlined;
}

void Graphic2d_Paragraph::SetSlant (const Quantity_PlaneAngle aSlant) {

        mySlant = Standard_ShortReal( aSlant );
        while (mySlant < 0.) mySlant += Standard_ShortReal( 2.*Standard_PI );
        while (mySlant >= 2.*Standard_PI) mySlant -= Standard_ShortReal( 2.*Standard_PI );
}

void Graphic2d_Paragraph::SetSpacing (const Quantity_Ratio aSpacing) {

        mySpacing = Standard_ShortReal( aSpacing );
#ifdef PERF
        myMinX = myMinY = ShortRealLast ();
        myMaxX = myMaxY = ShortRealFirst ();
#else
        Update();     
#endif
}

void Graphic2d_Paragraph::SetMargin (const Quantity_Length aMargin) {

#ifdef PRO12982
        myMargin = Standard_ShortReal( aMargin * myScale );
#else
        myMargin = Standard_ShortReal( aMargin );
#endif
#ifdef PERF
        myMinX = myMinY = ShortRealLast ();
        myMaxX = myMaxY = ShortRealFirst ();
#else
        Update();     
#endif
}

Standard_Boolean Graphic2d_Paragraph::IsZoomable () const {

        return myIsZoomable;

}

void Graphic2d_Paragraph::SetZoomable (const Standard_Boolean aFlag) {

        myIsZoomable    = aFlag;
#ifdef PERF
        myMinX = myMinY = ShortRealLast ();
        myMaxX = myMaxY = ShortRealFirst ();
#else
        Update();     
#endif
}

void Graphic2d_Paragraph::AddText (const TCollection_ExtendedString &aText,
                                   const Standard_Integer aRow,
                                   const Standard_Integer aColumn) {
Standard_Integer i,length = myTextDescriptorList.Length() ;
Standard_Integer row = (aRow > 0) ? Min(aRow,MAXROW-1) : MAXROW-1 ;
Standard_Integer column = (aColumn > 0) ? Min(aColumn,MAXCOLUMN-1) : MAXCOLUMN-1 ;
Standard_Integer descriptor = TEXT_DESCRIPTOR(row,column,myCurrentColorIndex,
                myCurrentFontIndex,myCurrentAlignment,myCurrentUnderline) ;
Standard_Integer the_position = TEXT_POSITION(descriptor) ;

        for( i=1 ; i<=length ; i++ ) {
          Standard_Integer cur_position = 
                        TEXT_POSITION(myTextDescriptorList.Value(i)) ;
          if( the_position == cur_position ) {
            myTextDescriptorList.SetValue(i,descriptor) ;
            myTextStringList.SetValue(i,aText) ;
            myTextHScaleList.SetValue(i,myCurrentFontHScale);
            myTextWScaleList.SetValue(i,myCurrentFontWScale);
            return ;
          }
          if( the_position < cur_position ) break ;
        }

        if( i > length ) {
          if( aRow <= 0 || aColumn <= 0 ) {
            if( aRow <= 0 ) {
              if( length > 0 ) {
                row = TEXT_ROW(myTextDescriptorList.Value(length))+1 ;
                row = Min(row,MAXROW-1) ;
              } else {
                row = 1 ;
              }
            }
            if( aColumn <= 0 ) {
              if( length > 0 ) {
                if( row == TEXT_ROW(myTextDescriptorList.Value(length)) ) {
                  column = TEXT_COLUMN(myTextDescriptorList.Value(length))+1 ;
                  column = Min(column,MAXCOLUMN-1) ;
                } else {
                  column = 1;
                }
              } else {
                column = 1 ;
              }
            }
            descriptor = TEXT_DESCRIPTOR(row,column,myCurrentColorIndex,
                myCurrentFontIndex,myCurrentAlignment,myCurrentUnderline) ;
          }
          myTextDescriptorList.Append(descriptor) ;
          myTextStringList.Append(aText) ;
          myTextXpositionList.Append(0.) ;
          myTextYpositionList.Append(0.) ;
          myTextFheightList.Append(0.) ;
          myTextFoffsetList.Append(0.) ;
          myTextHScaleList.Append(myCurrentFontHScale) ;
          myTextWScaleList.Append(myCurrentFontWScale) ;
        } else {
          if( aRow <= 0 || aColumn <= 0 ) {
            if( aRow <= 0 ) {
              if( i > 1 ) {
                row = TEXT_ROW(myTextDescriptorList.Value(i-1))+1 ;
                row = Min(row,MAXROW-1) ;
              } else {
                row = 1 ;
              }
            }
            if( aColumn <= 0 ) {
              if( i > 1 ) {
                if( row == TEXT_ROW(myTextDescriptorList.Value(i-1)) ) {
                  column = TEXT_COLUMN(myTextDescriptorList.Value(i-1))+1 ;
                  column = Min(column,MAXCOLUMN-1) ;
                } else {
                  column = 1;
                }
              } else {
                column = 1 ;
              }
            }
            descriptor = TEXT_DESCRIPTOR(row,column,myCurrentColorIndex,
                myCurrentFontIndex,myCurrentAlignment,myCurrentUnderline) ;
          }
          myTextDescriptorList.InsertBefore(i,descriptor) ;
          myTextStringList.InsertBefore(i,aText) ;
          myTextXpositionList.InsertBefore(i,0.) ;
          myTextYpositionList.InsertBefore(i,0.) ;
          myTextFheightList.InsertBefore(i,0.) ;
          myTextFoffsetList.InsertBefore(i,0.) ;
          myTextHScaleList.InsertBefore(i,myCurrentFontHScale);
          myTextWScaleList.InsertBefore(i,myCurrentFontWScale);
        }
#ifdef PERF
        myMinX = myMinY = ShortRealLast ();
        myMaxX = myMaxY = ShortRealFirst ();
#else
        Update();     
#endif

}

void Graphic2d_Paragraph::ChangeText (const TCollection_ExtendedString &aText,
                                   const Standard_Integer aRow,
                                   const Standard_Integer aColumn) {
Standard_Integer i,length = myTextDescriptorList.Length() ;
#ifdef DEB
Standard_Integer row = (aRow > 0) ? Min(aRow,MAXROW-1) : MAXROW-1 ;
Standard_Integer column = (aColumn > 0) ? Min(aColumn,MAXCOLUMN-1) : MAXCOLUMN-1 ;
#endif
Standard_Integer descriptor = TEXT_DESCRIPTOR(aRow,aColumn,0,0,0,0) ;
Standard_Integer the_position = TEXT_POSITION(descriptor) ;

        for( i=1 ; i<=length ; i++ ) {
          Standard_Integer cur_position = 
                        TEXT_POSITION(myTextDescriptorList.Value(i)) ;
          if( the_position == cur_position ) {
            myTextStringList.SetValue(i,aText) ;
#ifdef PERF
            myMinX = myMinY = ShortRealLast ();
            myMaxX = myMaxY = ShortRealFirst ();
#else
            Update() ;
#endif
            return ;
          }
        }

}

void Graphic2d_Paragraph::Clear() {

        myTextDescriptorList.Clear() ;
        myTextStringList.Clear() ;
        myTextXpositionList.Clear() ;
        myTextYpositionList.Clear() ;
        myTextFheightList.Clear() ;
        myTextFoffsetList.Clear() ;
        myTextHScaleList.Clear() ;
        myTextWScaleList.Clear() ;
#ifdef PERF
        myMinX = myMinY = ShortRealLast ();
        myMaxX = myMaxY = ShortRealFirst ();
#else
        Update();
#endif
}

void Graphic2d_Paragraph::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {
//Standard_Integer descriptor,row = 0 ;
Standard_Integer descriptor ;
TCollection_ExtendedString atext ;
Standard_ShortReal x = 0, y = 0, dx, dy, 
                   sina = Standard_ShortReal( Sin(myAngle) ),
                   cosa = Standard_ShortReal( Cos(myAngle) );
//Standard_Integer i,j,lrow,length = myTextDescriptorList.Length() ;
Standard_Integer i,length = myTextDescriptorList.Length() ;
Standard_Boolean IsIn = Standard_False;
Standard_ShortReal hscale,wscale,scale = (myIsZoomable) ? myScale*aDrawer->Scale() : myScale;
#ifdef PRO12982
Standard_ShortReal rscale = 1.;
#else
Standard_ShortReal rscale = 1./myScale;
#endif

#ifdef PERF
        if( (myMaxX < myMinX) || (myMaxY < myMinY) ) {
          if( !ComputeMinMax() ) return;
        }
#else
        if( myWidth <= 0. ) {
          Update();
        }
#endif

static TShort_Array1OfShortReal theXarray(1,5) ;
static TShort_Array1OfShortReal theYarray(1,5) ;

        // We have to draw frame without drawer's scaling
        // because drawer's scale will betaken into consideration
        // in MapPolylineFromTo(), MapPolygonFromTo().
        // The same concerns test's position.
        if (!myIsZoomable)
          rscale = aDrawer->Scale();
        if (! myGOPtr->IsTransformed ()) {
          IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
          if (IsIn) {
            if( myFrameColorIndex > 0  || myHidingColorIndex >= 0 ) {
              theXarray.SetValue(1,myX+XROTATE(myXoffset,myYoffset)/rscale) ;
              theYarray.SetValue(1,myY+YROTATE(myXoffset,myYoffset)/rscale) ;
              theXarray.SetValue(2,myX+XROTATE(myXoffset+myWidth,myYoffset)/rscale) ;
              theYarray.SetValue(2,myY+YROTATE(myXoffset+myWidth,myYoffset)/rscale) ;
              theXarray.SetValue(3,myX+XROTATE(myXoffset+myWidth,myYoffset-myHeight)/rscale) ;
#ifdef PRO11004
              theYarray.SetValue(3,myY+YROTATE(myXoffset+myWidth,myYoffset-myHeight)/rscale) ;
#else
              theYarray.SetValue(3,YROTATE(myXoffset+myWidth,myYoffset-myHeight)/rscale) ;
#endif
              theXarray.SetValue(4,myX+XROTATE(myXoffset,myYoffset-myHeight)/rscale) ;
              theYarray.SetValue(4,myY+YROTATE(myXoffset,myYoffset-myHeight)/rscale) ;
              theXarray.SetValue(5,myX+XROTATE(myXoffset,myYoffset)/rscale) ;
              theYarray.SetValue(5,myY+YROTATE(myXoffset,myYoffset)/rscale) ;
#ifdef PRO11005
              if( myHidingColorIndex >= 0 ) {
                aDrawer->SetHidingTextAttrib(-1,myHidingColorIndex,
                        myFrameColorIndex,myFrameWidthIndex,
                                        -1,0.,1.,1.,Standard_False);
                aDrawer->MapPolygonFromTo(theXarray, theYarray, 4);
              } else {
                aDrawer->SetFramedTextAttrib(-1,myFrameColorIndex,
                        myFrameWidthIndex,-1,0.,1.,1.,Standard_False);
                aDrawer->MapPolylineFromTo(theXarray, theYarray, 5);
              }
#else
              if( myHidingColorIndex >= 0 ) {
                aDrawer->SetPolyAttrib(myHidingColorIndex,0,Standard_False) ;
                aDrawer->MapPolygonFromTo(theXarray, theYarray, 4);
              }
              if( myFrameColorIndex > 0 ) {
                aDrawer->SetLineAttrib(myFrameColorIndex,0,myFrameWidthIndex) ;
                aDrawer->MapPolylineFromTo(theXarray, theYarray, 5);
              }
#endif
            }
            for( i=1 ; i<=length ; i++ ) {      
              descriptor = myTextDescriptorList.Value(i) ;
              atext = myTextStringList.Value(i) ;
              dx = myXoffset + myTextXpositionList.Value(i);
              dy = myYoffset + myTextYpositionList.Value(i);
              x = XROTATE(dx,dy)/rscale ; y = YROTATE(dx,dy)/rscale;
              hscale = scale*myTextHScaleList.Value(i);
              wscale = scale*myTextWScaleList.Value(i);
              aDrawer->SetTextAttrib(TEXT_COLOR(descriptor),
                                TEXT_FONT(descriptor),mySlant,hscale,wscale,
                                                TEXT_UNDERLINE(descriptor));
              aDrawer->MapTextFromTo(atext,myX+x,myY+y,
                                        myAngle,0.,0.,Aspect_TOT_SOLID);
            }
          }
        } else {
          gp_GTrsf2d aTrsf = myGOPtr->Transform ();
          Standard_ShortReal minx,miny,maxx,maxy;
          MinMax(minx,maxx,miny,maxy);
          IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
          if (IsIn) {
            Standard_Real A = Standard_Real (myX); 
            Standard_Real B = Standard_Real (myY);
            Standard_Real C = Standard_Real (myAngle);
            Standard_Real cosa = Cos (C);
            Standard_Real sina = Sin (C);

            aTrsf.Transforms (A, B);

            // To calculate new aperture angles
            // the calculation is done on trigonometric circle
            // and in this case translation is not taken into account.
            aTrsf.SetValue (1, 3, 0.0);
            aTrsf.SetValue (2, 3, 0.0);
            aTrsf.Transforms (cosa, sina);

            Standard_ShortReal tscale = 1.;
            Standard_ShortReal x0 = myXoffset;
            Standard_ShortReal y0 = myYoffset;
            Standard_ShortReal w = myWidth;
            Standard_ShortReal h = myHeight;
            C = atan2(sina,cosa);
            Standard_ShortReal angle = Standard_ShortReal (C);
            if ( myIsZoomable ) {
              tscale = Standard_ShortReal( Sqrt(cosa*cosa + sina*sina) );
              scale *= Standard_ShortReal( tscale );
              x0 *= tscale; y0 *= tscale;
              w *= tscale; h *= tscale;
            }
            cosa = Cos(C) ; sina = Sin(C) ;
            if( myFrameColorIndex > 0  || myHidingColorIndex >= 0 ) {
              dx = Standard_ShortReal( XROTATE(x0,y0)/rscale );
              dy = Standard_ShortReal( YROTATE(x0,y0)/rscale );
              theXarray.SetValue( 1, x + dx ); 
              theYarray.SetValue( 1, y + dy );
              dx = Standard_ShortReal( XROTATE(x0+w,y0)/rscale );
              dy = Standard_ShortReal( YROTATE(x0+w,y0)/rscale );
              theXarray.SetValue(2,x+dx) ; theYarray.SetValue(2,y+dy) ;
              dx = Standard_ShortReal( XROTATE(x0+w,y0-h)/rscale );
              dy = Standard_ShortReal( YROTATE(x0+w,y0-h)/rscale );
              theXarray.SetValue(3,x+dx) ; 
              theYarray.SetValue(3,y+dy) ;
              dx = Standard_ShortReal( XROTATE(x0,y0-h)/rscale );
              dy = Standard_ShortReal( YROTATE(x0,y0-h)/rscale );
              theXarray.SetValue(4,x+dx) ; theYarray.SetValue(4,y+dy) ;
              dx = Standard_ShortReal( XROTATE(x0,y0)/rscale );
              dy = Standard_ShortReal( YROTATE(x0,y0)/rscale );
              theXarray.SetValue(5,x+dx) ; theYarray.SetValue(5,y+dy) ;
#ifdef PRO11005
              if( myHidingColorIndex >= 0 ) {
                aDrawer->SetHidingTextAttrib(-1,myHidingColorIndex,
                        myFrameColorIndex,myFrameWidthIndex,
                                        -1,0.,1.,1.,Standard_False);
                aDrawer->MapPolygonFromTo(theXarray, theYarray, 4);
              } else {
                aDrawer->SetFramedTextAttrib(-1,myFrameColorIndex,
                        myFrameWidthIndex,-1,0.,1.,1.,Standard_False);
                aDrawer->MapPolylineFromTo(theXarray, theYarray, 5);
              }
#else
              if( myHidingColorIndex >= 0 ) {
                aDrawer->SetPolyAttrib(myHidingColorIndex,0,Standard_False) ;
                aDrawer->MapPolygonFromTo(theXarray, theYarray, 4);
              }  
               if( myFrameColorIndex > 0 ) {
                aDrawer->SetLineAttrib(myFrameColorIndex,0,myFrameWidthIndex) ;
                aDrawer->MapPolylineFromTo(theXarray, theYarray, 5);
              }
#endif
            }
            Standard_ShortReal tx,ty;
            for( i=1 ; i<=length ; i++ ) {       
              descriptor = myTextDescriptorList.Value(i) ;
              atext = myTextStringList.Value(i) ;
              dx = x0 + myTextXpositionList.Value(i)*tscale ;
              dy = y0 + myTextYpositionList.Value(i)*tscale ;
              tx = Standard_ShortReal( XROTATE(dx,dy)/rscale ); 
              ty = Standard_ShortReal( YROTATE(dx,dy)/rscale );
              hscale = scale*myTextHScaleList.Value(i) ;
              wscale = scale*myTextWScaleList.Value(i) ;
              aDrawer->SetTextAttrib(TEXT_COLOR(descriptor),
                        TEXT_FONT(descriptor),mySlant,hscale,wscale,
                                        TEXT_UNDERLINE(descriptor));
              aDrawer->MapTextFromTo(atext,x+tx,y+ty,
                                        angle,0.,0.,Aspect_TOT_SOLID);
            }
          }
        }
}


Standard_Boolean Graphic2d_Paragraph::Pick (const Standard_ShortReal X,
                                       const Standard_ShortReal Y,
                                       const Standard_ShortReal aPrecision,
                                       const Handle(Graphic2d_Drawer)& aDrawer) 
{
Standard_Boolean theStatus = Standard_False;
#ifdef DEB
Standard_ShortReal scale = 
#endif
  (myIsZoomable) ? myScale*aDrawer->Scale() : myScale;
Standard_ShortReal TX = X, TY = Y;

  if (IsInMinMax (X, Y, aPrecision)) {
    if (myGOPtr->IsTransformed ()) {
        gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
        Standard_Real RX = Standard_Real (X), RY = Standard_Real (Y);
        aTrsf.Transforms (RX, RY); 
        TX = Standard_ShortReal (RX); TY = Standard_ShortReal (RY);
    }
  
    Standard_ShortReal cosa = Standard_ShortReal( Cos(-myAngle) );
    Standard_ShortReal sina = Standard_ShortReal( Sin(-myAngle) );
    Standard_ShortReal dx = TX-myX;
    Standard_ShortReal dy = TY-myY;
    Standard_ShortReal x = XROTATE(dx,dy);
    Standard_ShortReal y = YROTATE(dx,dy);

    theStatus =  x >= myXoffset - aPrecision            
            &&   x <= myXoffset + myWidth + aPrecision
            &&   y >= -myYoffset - aPrecision 
            &&   y <=  myHeight -myYoffset + aPrecision;
  }

  return theStatus;
}

void Graphic2d_Paragraph::Size ( Quantity_Length &aWidth,
                                            Quantity_Length &aHeight) const {
    aWidth = myWidth;
    aHeight = myHeight;

}

void Graphic2d_Paragraph::Position( Quantity_Length &X,
                               Quantity_Length &Y) const {
 
    X = myX;
    Y = myY;
}

Aspect_CardinalPoints Graphic2d_Paragraph::Offset( Quantity_Length &X,
                             Quantity_Length &Y) const {

        X = myXoffset;
        Y = myYoffset;

        return myOffset;
}

Quantity_PlaneAngle Graphic2d_Paragraph::Angle() const {

        return myAngle;
}

Quantity_PlaneAngle Graphic2d_Paragraph::Slant() const {

        return mySlant;
}

Quantity_PlaneAngle Graphic2d_Paragraph::Spacing() const {

        return mySpacing;
}

Quantity_PlaneAngle Graphic2d_Paragraph::Margin() const {

        return myMargin;
}

Standard_Integer Graphic2d_Paragraph::HidingColorIndex() const {
 
        return myHidingColorIndex;
}
 
Standard_Integer Graphic2d_Paragraph::FrameColorIndex() const {
 
        return myFrameColorIndex;
}
 
Standard_Integer Graphic2d_Paragraph::FrameWidthIndex() const {
 
        return myFrameWidthIndex;
}

TCollection_ExtendedString Graphic2d_Paragraph::Text(
                                const Standard_Integer aRank,
                                Standard_Integer &aRow,
                                Standard_Integer &aColumn,
                                Standard_Integer &aColorIndex,
                                Standard_Integer &aFontIndex,
                                Graphic2d_TypeOfAlignment &anAlignment) const {

        if( aRank <= 0 || aRank > myTextDescriptorList.Length() ) {
                Standard_OutOfRange::Raise ("Bad Text rank");
        }

        Standard_Integer descriptor = myTextDescriptorList.Value(aRank) ;
        aRow = TEXT_ROW(descriptor) ;
        aColumn = TEXT_COLUMN(descriptor) ;
        aColorIndex = TEXT_COLOR(descriptor) ;
        aFontIndex = TEXT_FONT(descriptor) ;
        anAlignment = Graphic2d_TypeOfAlignment(TEXT_ALIGNMENT(descriptor)) ;

        return myTextStringList.Value(aRank) ;
}

Standard_Boolean Graphic2d_Paragraph::TextSize ( const Standard_Integer aRank,
        Quantity_Length &aWidth, Quantity_Length &aHeight,
        Quantity_Length &anXoffset, Quantity_Length &anYoffset) const {
        Handle(Graphic2d_Drawer) aDrawer = Drawer();

        if( aRank <= 0 || aRank > myTextDescriptorList.Length() ) {
                Standard_OutOfRange::Raise ("Bad Text rank");
        }

    if( !aDrawer.IsNull() && aDrawer->IsWindowDriver() ) {
        Standard_Integer descriptor = myTextDescriptorList.Value(aRank) ;
        Standard_ShortReal wscale,hscale,scale = (myIsZoomable) ?
                                myScale * aDrawer->Scale() : myScale;
        Standard_ShortReal width,height,xoffset,yoffset;
        TCollection_ExtendedString atext = myTextStringList.Value(aRank) ;
        hscale = scale*myTextHScaleList.Value(aRank);
        wscale = scale*myTextWScaleList.Value(aRank);
        aDrawer->SetTextAttrib(TEXT_COLOR(descriptor),TEXT_FONT(descriptor),
                            mySlant,hscale,wscale,TEXT_UNDERLINE(descriptor));

        aDrawer->GetTextSize(atext,width,height,xoffset,yoffset);
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

Standard_Integer Graphic2d_Paragraph::Length() const {

        return myTextStringList.Length();
}

Standard_Integer Graphic2d_Paragraph::MaxRow() const {
Standard_Integer descriptor,i,n;

        for( i=1,n=0 ; i<=myTextDescriptorList.Length() ; i++ ) {
          descriptor = myTextDescriptorList.Value(i) ;
          n = Max(n,TEXT_ROW(descriptor)) ;
        }

        return n;
}

Standard_Integer Graphic2d_Paragraph::MaxColumn() const {
Standard_Integer descriptor,i,n;

        for( i=1,n=0 ; i<=myTextDescriptorList.Length() ; i++ ) {
          descriptor = myTextDescriptorList.Value(i) ;
          n = Max(n,TEXT_COLUMN(descriptor)) ;
        }

        return n;
}

#ifdef PERF
Standard_Boolean Graphic2d_Paragraph::ComputeMinMax() {
#else
void Graphic2d_Paragraph::Update() {
#endif
Handle(Graphic2d_Drawer) aDrawer = Drawer();
Standard_Boolean status = Standard_False;

    if( !aDrawer.IsNull() && aDrawer->IsWindowDriver() ) {
          Standard_ShortReal rscale = 1.;
          if (!myIsZoomable)
            rscale = aDrawer->Scale();

          Standard_Integer descriptor,row = 0 ;
          TCollection_ExtendedString atext ;
          Standard_ShortReal x, y, dx, dy, 
              sina = Standard_ShortReal( Sin(myAngle) ),
              cosa = Standard_ShortReal( Cos(myAngle) );
          Standard_Integer i,j,lrow,length = myTextDescriptorList.Length() ;
          Standard_ShortReal hscale,wscale,scale = 
                        (myIsZoomable) ? myScale*aDrawer->Scale() : myScale;
          Standard_ShortReal twidth = 0.,theight = 0.,txoffset = 0.,tyoffset = 0.;
          Standard_ShortReal lwidth = 0.,lheight = 0.,loffset = 0.;
          myWidth = myHeight = 0. ;
          for( i=1,j=lrow=0 ; i<=length ; i++ ) {       // Passe 1, gets texts minmax 
            descriptor = myTextDescriptorList.Value(i) ;
            atext = myTextStringList.Value(i) ;
            hscale = scale*myTextHScaleList.Value(i) ;
            wscale = scale*myTextWScaleList.Value(i) ;
            aDrawer->SetTextAttrib(TEXT_COLOR(descriptor),
                                TEXT_FONT(descriptor),mySlant,hscale,wscale,
                                                TEXT_UNDERLINE(descriptor));
            if( aDrawer->GetTextSize(atext,twidth,theight,txoffset,tyoffset) ) {
#ifdef PRO12320
              static TCollection_ExtendedString hletter("Hp");
              Standard_ShortReal hwidth,hheight,hxoffset,hyoffset;
              if(( status = aDrawer->GetTextSize(hletter,hwidth,hheight,
                                                        hxoffset,hyoffset) )) {
                theight = Max(hheight-hyoffset,theight-tyoffset);
                tyoffset = Max(hyoffset,tyoffset);
              }
#else
              Standard_ShortReal fheight,fbheight,fslant;
              aDrawer->GetFontSize(fheight,fbheight,fslant);
              theight = fheight + fbheight; tyoffset = 0.;
#endif  //PRO12320
              twidth += txoffset;
              myTextXpositionList.SetValue(i,twidth) ;
              if( row == TEXT_ROW(descriptor) ) {
                lwidth += twidth ; lheight = Max(lheight,theight) ;
                loffset = Max(loffset,tyoffset);
              } else {
                if( j > 0 ) {
                  if( j > 1 ) lheight += mySpacing*lheight*(row-lrow) ;
#ifndef PRO12982
                  lheight -= loffset;
#endif
                  myTextYpositionList.SetValue(j,lheight) ;
                  myWidth = Max(myWidth,lwidth) ; 
                  myHeight += lheight ;
                }
                lwidth = twidth ; lheight = theight ; loffset = tyoffset;
                lrow = row ; row = TEXT_ROW(descriptor) ; j = i ;
              }
            }
          }

          if( j > 0 ) {
            if( j > 1 ) lheight += mySpacing*lheight*(row-lrow) ;
#ifndef PRO12982
            lheight -= loffset;
#endif
            myTextYpositionList.SetValue(j,lheight) ;
            myWidth = Max(myWidth,lwidth) ; 
            myHeight += lheight ;
#ifdef PRO12982
            myHeight += loffset;
#endif
          }
          myWidth += Standard_ShortReal( 2.*myMargin );
          myHeight += Standard_ShortReal( 2.*myMargin );

          x = myMargin ;
          y = -myMargin ; 
          row = 0 ;
          for( i=1 ; i<=length ; i++ ) {        // Passe 2,sets texts position
            descriptor = myTextDescriptorList.Value(i) ;
            twidth = myTextXpositionList.Value(i) ;
            if( row != TEXT_ROW(descriptor) ) {
              theight = myTextYpositionList.Value(i) ;
              x = myMargin ; y -= theight ;     
              row = TEXT_ROW(descriptor) ;
            }
            switch (TEXT_ALIGNMENT(descriptor)) {
              case Graphic2d_TOA_TOPLEFT:
              case Graphic2d_TOA_MEDIUMLEFT:
              case Graphic2d_TOA_BOTTOMLEFT:
              case Graphic2d_TOA_LEFT:
                break ;
              case Graphic2d_TOA_TOPCENTER:
              case Graphic2d_TOA_MEDIUMCENTER:
              case Graphic2d_TOA_BOTTOMCENTER:
              case Graphic2d_TOA_CENTER:
                x = Standard_ShortReal( (myWidth-twidth)/2. );
                break ;
              case Graphic2d_TOA_TOPRIGHT:
              case Graphic2d_TOA_MEDIUMRIGHT:
              case Graphic2d_TOA_BOTTOMRIGHT:
              case Graphic2d_TOA_RIGHT:
                x = myWidth-twidth-myMargin ;
                break ;
            }
            myTextXpositionList.SetValue(i,x*rscale) ;
            myTextYpositionList.SetValue(i,y*rscale) ;
            x += twidth ;
          }

          switch (myOffset) {
            case Aspect_CP_North:
              myXoffset = Standard_ShortReal( -myWidth/2. ); myYoffset = 0. ;
              break ;
            case Aspect_CP_NorthEast:
              myXoffset = -myWidth ; myYoffset = 0. ;
              break ;
            case Aspect_CP_East:
              myXoffset = -myWidth ; myYoffset = Standard_ShortReal( myHeight/2. );
              break ;
            case Aspect_CP_SouthEast:
              myXoffset = -myWidth ; myYoffset = myHeight ;
              break ;
            case Aspect_CP_South:
              myXoffset = Standard_ShortReal( -myWidth/2. ); myYoffset = myHeight ;
              break ;
            case Aspect_CP_SouthWest:
              myXoffset = 0. ; myYoffset = myHeight ;
              break ;
            case Aspect_CP_West:
              myXoffset = 0.; myYoffset = Standard_ShortReal( myHeight/2. );
              break ;
            case Aspect_CP_NorthWest:
              myXoffset = 0. ; myYoffset = 0. ;
              break ;
            case Aspect_CP_Center:
              myXoffset = Standard_ShortReal( -myWidth/2. ); 
              myYoffset = Standard_ShortReal( myHeight/2. );
          }
  
          myMinX = myMinY = ShortRealLast();
          myMaxX = myMaxY = ShortRealFirst();

          dx = XROTATE(myXoffset,myYoffset) ;
          dy = YROTATE(myXoffset,myYoffset) ;
          myMinX = Min(myMinX,myX+dx) ;
          myMinY = Min(myMinY,myY+dy) ;
          myMaxX = Max(myMaxX,myX+dx) ;
          myMaxY = Max(myMaxY,myY+dy) ;

          dx = XROTATE(myWidth+myXoffset,myYoffset) ;
          dy = YROTATE(myWidth+myXoffset,myYoffset) ;
          myMinX = Min(myMinX,myX+dx) ;
          myMinY = Min(myMinY,myY+dy) ;
          myMaxX = Max(myMaxX,myX+dx) ;
          myMaxY = Max(myMaxY,myY+dy) ;

          dx = XROTATE(myWidth+myXoffset,-myHeight+myYoffset) ;
          dy = YROTATE(myWidth+myXoffset,-myHeight+myYoffset) ;
          myMinX = Min(myMinX,myX+dx) ;
          myMinY = Min(myMinY,myY+dy) ;
          myMaxX = Max(myMaxX,myX+dx) ;
          myMaxY = Max(myMaxY,myY+dy) ;

          dx = XROTATE(myXoffset,-myHeight+myYoffset) ;
          dy = YROTATE(myXoffset,-myHeight+myYoffset) ;
          myMinX = Min(myMinX,myX+dx) ;
          myMinY = Min(myMinY,myY+dy) ;
          myMaxX = Max(myMaxX,myX+dx) ;
          myMaxY = Max(myMaxY,myY+dy) ;

          myXoffset *= rscale;
          myYoffset *= rscale;
          myWidth   *= rscale;
          myHeight  *= rscale;
    }
#ifdef PERF
      else {
        cout << "*Graphic2d_Paragraph::ComputeMinMax() returns wrong values*" << endl;
    }

    return status;
#endif


}

void Graphic2d_Paragraph::Save(Aspect_FStream& aFStream) const
{
}
