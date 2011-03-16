#define WTO0001         //GG_140596
//                      Calcul des min-max faux apres transformation.

#define PRO5663         //GG_291196
//                      Renseigner les MinMax de l'image !.

#include <Graphic2d_Image.ixx>
#include <Graphic2d_Image.pxx>
#include <Aspect_WindowDriver.hxx>
#include <Aspect_RGBPixel.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <Quantity_Array1OfColor.hxx>

Graphic2d_Image::Graphic2d_Image (const Handle(Graphic2d_GraphicObject)& aGraphicObject, const Handle(Image_Image)& anImage, const Quantity_Length X, const Quantity_Length Y, const Quantity_Length adx, const Quantity_Length ady, const Aspect_CardinalPoints aTypeOfPlacement)
  : Graphic2d_Primitive (aGraphicObject) , 
    myImage(anImage), 
    mydx(Standard_ShortReal(adx)), 
    mydy(Standard_ShortReal(ady)), 
    myPlacement(aTypeOfPlacement), 
    myIsModified(Standard_False) {

        SetFamily(Graphic2d_TOP_IMAGE);
        SetCenter(X,Y);
#ifndef PRO5663
        myMinX = myX; myMinY = myY;
        myMaxX = myX; myMaxY = myY;
#endif
}

void Graphic2d_Image::Clear () {

    myIsModified = Standard_True;
}

void Graphic2d_Image::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

#ifdef OK
Standard_Boolean IsIn = Standard_False;
  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
#ifdef WTO0001
    MinMax(minx,maxx,miny,maxy);
#else
gp_GTrsf2d aTrsf = myGOPtr->Transform ();
Standard_Real MINX, MINY, MAXX, MAXY;

    MINX = Standard_Real (myMinX);
    MINY = Standard_Real (myMinY);
    MAXX = Standard_Real (myMaxX);
    MAXY = Standard_Real (myMaxY);
    aTrsf.Transforms (MINX, MINY);
    aTrsf.Transforms (MAXX, MAXY);
    minx = Standard_ShortReal (MINX);
    miny = Standard_ShortReal (MINY);
    maxx = Standard_ShortReal (MAXX);
    maxy = Standard_ShortReal (MAXY);
#endif
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }
#else
Standard_Boolean IsIn = Standard_True;
#endif

  if (IsIn) {

Standard_ShortReal a, b;
Standard_Real A, B;

        ComputeCenter (aDrawer, a, b);
#ifdef PRO5663
        if( myMinX == myMaxX ) { 
          Standard_ShortReal dx = aDrawer->Convert(myImage->Width());
          Standard_ShortReal dy = aDrawer->Convert(myImage->Height());
          Standard_ShortReal width  = aDrawer->ConvertMapToFrom(dx);
          Standard_ShortReal height = aDrawer->ConvertMapToFrom(dy);
          myMinX = a - Standard_ShortReal(width/2.); myMaxX = a + Standard_ShortReal(width/2.);
          myMinY = b - Standard_ShortReal(height/2.); myMaxY = b + Standard_ShortReal(height/2.);
        }
#endif
        if (myGOPtr->IsTransformed ()) {
                A = Standard_Real (a);
                B = Standard_Real (b);
                (myGOPtr->Transform ()).Transforms (A, B);
                a = Standard_ShortReal (A);
                b = Standard_ShortReal (B);
        }
        if (myIsModified) {
                myIsModified = Standard_False;
                aDrawer->ClearImage (myImage);
        }
        if (aDrawer->IsKnownImage (myImage))
                aDrawer->DrawImage (myImage, a, b);
        else
                FillAndDraw (aDrawer);

        // Dans le cas d'un GraphicObject Highlighted
        // on trace le rectangle englobant de l'image
        if (myGOPtr->IsHighlighted ()) {

Standard_Integer Width  = myImage->Width ();
Standard_Integer Height = myImage->Height ();
                //
                // - 2 sur Width et Height pour eviter que
                // le bord soit en dehors de l'image.
                //
                if (Width > 2) Width -= 2;
                if (Height > 2) Height -= 2;
Standard_ShortReal dx = aDrawer->Convert (Width);
Standard_ShortReal dy = aDrawer->Convert (Height);

Standard_ShortReal width  = aDrawer->ConvertMapToFrom (dx);
Standard_ShortReal height = aDrawer->ConvertMapToFrom (dy);

TShort_Array1OfShortReal TheX (1, 5);
TShort_Array1OfShortReal TheY (1, 5);

                TheX (1)        = a - Standard_ShortReal(width / 2.);
                TheY (1)        = b - Standard_ShortReal(height / 2.);
                TheX (2)        = a - Standard_ShortReal(width / 2.);
                TheY (2)        = b + Standard_ShortReal(height / 2.);
                TheX (3)        = a + Standard_ShortReal(width / 2.);
                TheY (3)        = b + Standard_ShortReal(height / 2.);
                TheX (4)        = a + Standard_ShortReal(width / 2.);
                TheY (4)        = b - Standard_ShortReal(height / 2.);
                TheX (5)        = a - Standard_ShortReal(width / 2.);
                TheY (5)        = b - Standard_ShortReal(height / 2.);

                aDrawer->SetLineAttrib (myColorIndex, 0, 0);
                aDrawer->MapPolylineFromTo (TheX, TheY);
        }

  }
}

Standard_Boolean Graphic2d_Image::Pick (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal aPrecision,const Handle(Graphic2d_Drawer)& aDrawer) {

Standard_ShortReal dx = aDrawer->Convert(myImage->Width());
Standard_ShortReal dy = aDrawer->Convert(myImage->Height());

Standard_ShortReal width  = aDrawer->ConvertMapToFrom(dx);
Standard_ShortReal height = aDrawer->ConvertMapToFrom(dy);

  Standard_ShortReal a,b;
  ComputeCenter(aDrawer,a,b);

Standard_ShortReal SRX = X, SRY = Y;

  if (myGOPtr->IsTransformed ()) {
gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
        aTrsf.Transforms (RX, RY);
        SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
  }

  return
      (       SRX >= a - width  / 2. - aPrecision
         &&   SRX <= a + width  / 2. + aPrecision

         &&   SRY >= b - height / 2. - aPrecision
         &&   SRY <= b + height / 2. + aPrecision  );

}

void Graphic2d_Image::SetCenter (const Quantity_Length X, const Quantity_Length Y) {

        myX     = Standard_ShortReal(X);
        myY     = Standard_ShortReal(Y);
#ifdef PRO5663
        myMinX = myMaxX = myX;
        myMinY = myMaxY = myY;
#endif

}

void Graphic2d_Image::Translate (const Quantity_Length DX, const Quantity_Length DY) {

        myX     = myX + Standard_ShortReal(DX);
        myY     = myY + Standard_ShortReal(DY);
#ifdef PRO5663
        myMinX = myMaxX = myX;
        myMinY = myMaxY = myY;
#endif

}

void Graphic2d_Image::SetSmallSize (const Standard_Integer aSize) {

        TheSmallSize    = aSize;

}

Standard_Integer Graphic2d_Image::SmallSize () {

        return (TheSmallSize);

}

// Private methods

void Graphic2d_Image::FillAndDraw (const Handle(Graphic2d_Drawer)& aDrawer) const {

Standard_ShortReal a,b;

Standard_Integer LowerX, LowerY;
Standard_Integer UpperX, UpperY;
Standard_Integer Width, Height;
Aspect_RGBPixel *pixels;

Standard_Integer indice;
Standard_Integer i, ii;
Standard_Integer j;

Standard_Real R, G, B;

        ComputeCenter(aDrawer,a,b);

        Width   = myImage->Width ();
        Height  = myImage->Height ();

        LowerX  = myImage->LowerX ();
        LowerY  = myImage->LowerY ();

        UpperX  = myImage->UpperX ();
        UpperY  = myImage->UpperY ();

        if (Height*Width <= Graphic2d_Image::SmallSize ()) {
                // Les petites images sont remplies d'un coup
                pixels  = new Aspect_RGBPixel [Height*Width];

                Quantity_Array1OfColor Array (0, Width-1);

                // Parcours de l'image Lignes-Colonnes (Height-Width)
                for (i=0, ii=LowerY; ii<=UpperY; i++, ii++) {
                        myImage->RowColor (ii, Array);
                        for (j=0; j<Width; j++) {
                                Array (j).Values (R, G, B, Quantity_TOC_RGB);
                                indice  = i * Width + j;
                                pixels[indice].red      = float (R);
                                pixels[indice].green    = float (G);
                                pixels[indice].blue     = float (B);
                        }
                }

                aDrawer->FillAndDrawImage
                        (myImage, a, b, Width, Height,
                        (Standard_Address*) pixels);

                delete [] pixels;
        }
        else {
                // Les grandes images sont remplies ligne a ligne
                pixels  = new Aspect_RGBPixel [Width];

                Quantity_Array1OfColor Array (0, Width-1);

                // Parcours de l'image Lignes-Colonnes (Height-Width)
                for (i=0, ii=LowerY; ii<=UpperY; i++, ii++) {
                        myImage->RowColor (ii, Array);
                        for (j=0; j<Width; j++) {
                                Array (j).Values (R, G, B, Quantity_TOC_RGB);
                                pixels[j].red   = float (R);
                                pixels[j].green = float (G);
                                pixels[j].blue  = float (B);
                        }

                        aDrawer->FillAndDrawImage
                                (myImage, a, b, i, Width, Height,
                                (Standard_Address*)  pixels);
                }

                delete [] pixels;
        }

}
void Graphic2d_Image::ComputeCenter(const Handle(Graphic2d_Drawer)& aDrawer,
                                    Standard_ShortReal& cx,
                                    Standard_ShortReal& cy) const {
     Standard_Integer Width, Height;
     Width = myImage->Width ();
     Height = myImage->Height ();
     Standard_ShortReal dx = aDrawer->Convert(myImage->Width());
     Standard_ShortReal dy = aDrawer->Convert(myImage->Height());

     Standard_ShortReal width  = aDrawer->ConvertMapToFrom(dx);
     Standard_ShortReal height = aDrawer->ConvertMapToFrom(dy);

     Standard_ShortReal ox = aDrawer->ConvertMapToFrom(mydx);
     Standard_ShortReal oy = aDrawer->ConvertMapToFrom(mydy);

     switch (myPlacement) {
        case Aspect_CP_North:     { cx = myX + ox           ; 
                                    cy = myY + oy - Standard_ShortReal(height/2.) ; break; };
        case Aspect_CP_NorthEast: { cx = myX + ox - Standard_ShortReal(width/2.); 
                                    cy = myY + oy - Standard_ShortReal(height/2.) ; break; };
        case Aspect_CP_East:      { cx = myX + ox - Standard_ShortReal(width/2.); 
                                    cy = myY + oy ; break; };
        case Aspect_CP_SouthEast: { cx = myX + ox - Standard_ShortReal(width/2.); 
                                    cy = myY + oy + Standard_ShortReal(height/2.) ; break; };
        case Aspect_CP_South:     { cx = myX + ox           ; 
                                    cy = myY + oy + Standard_ShortReal(height/2.) ; break; };
        case Aspect_CP_SouthWest: { cx = myX + ox + Standard_ShortReal(width/2.); 
                                    cy = myY + oy + Standard_ShortReal(height/2.) ; break; };
        case Aspect_CP_West:      { cx = myX + ox + Standard_ShortReal(width/2.); 
                                    cy = myY + oy ; break; };
        case Aspect_CP_NorthWest: { cx = myX + ox + Standard_ShortReal(width/2.); 
                                    cy = myY + oy - Standard_ShortReal(height/2.) ; break; };
        case Aspect_CP_Center:    { cx = myX + ox; 
                                    cy = myY + oy ; break; };
     }
}

void Graphic2d_Image::SetOffset(const Quantity_Length dx, const Quantity_Length dy) {

        mydx    = Standard_ShortReal(dx);
        mydy    = Standard_ShortReal(dy);
#ifdef PRO5663
        myMinX = myMaxX = myX;
        myMinY = myMaxY = myY;
#endif
}

void Graphic2d_Image::SetPlacement(const Aspect_CardinalPoints aPlacement) {

        myPlacement     = aPlacement;
#ifdef PRO5663
        myMinX = myMaxX = myX;
        myMinY = myMaxY = myY;
#endif

}

void Graphic2d_Image::Position( Quantity_Length& X,Quantity_Length& Y ) const {
  
  X = Quantity_Length( myX );
  Y = Quantity_Length( myY );

}

void Graphic2d_Image::Offset(Quantity_Length& aX,Quantity_Length& aY) const {
    
  aX = Quantity_Length( mydx );
  aY = Quantity_Length( mydy );
}

Aspect_CardinalPoints Graphic2d_Image::Placement() const {
  return myPlacement;
}

Handle(Image_Image) Graphic2d_Image::Image() const {
  return myImage;
}

void Graphic2d_Image::Save(Aspect_FStream& aFStream) const
{
}
