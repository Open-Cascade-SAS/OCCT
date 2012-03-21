// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

/*=====================================================================

     FUNCTION :
     ----------
        Class Graphic2d_ImageFile

     TEST :
     ------

        See TestG2D/TestG21


=====================================================================*/

#define S3593           //GG_100398

#define xTRACE

//#if defined(WNT) || defined(LIN)
#include <stdio.h>
//#endif

#include <Graphic2d_ImageFile.ixx>
#include <Aspect_WindowDriver.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <OSD_Protection.hxx>
#include <Standard_Boolean.hxx>
#include <Aspect_XWD.hxx>

#define SWAPTEST (*(char*)&swaptest)

static unsigned long swaptest = 1;

#ifdef S3593
static Standard_Boolean ImageSize(const Handle(Graphic2d_Drawer) aDrawer,
                                  OSD_File &aFile,
                                  const TCollection_AsciiString& aFileName,
                                  Standard_Integer &aWidth,
                                  Standard_Integer &aHeight) {
Standard_Boolean status = Standard_False;
        if( !aDrawer.IsNull() ) {
          status = aDrawer->GetImageSize(aFileName.ToCString(),aWidth,aHeight); 
        } 

        if( !status ) {
          Standard_Integer depth;
          status = Graphic2d_ImageFile::Values(aFile,aWidth,aHeight,depth);
        }

        return status;
}
#endif

Graphic2d_ImageFile::Graphic2d_ImageFile (const Handle(Graphic2d_GraphicObject)& aGraphicObject, OSD_File& aFile, const Quantity_Length X, const Quantity_Length Y, const Quantity_Length adx, const Quantity_Length ady, const Aspect_CardinalPoints aTypeOfPlacement, const Quantity_Factor ascale):
        Graphic2d_Primitive (aGraphicObject),
        myFile(aFile),
        myX(Standard_ShortReal(X)),
        myY(Standard_ShortReal(Y)),
        mydx(Standard_ShortReal(adx)), 
        mydy(Standard_ShortReal(ady)),
        myPlacement(aTypeOfPlacement),
        myIsZoomable(Standard_False),
        myScale (Standard_ShortReal(ascale))
#ifndef S3593
        ,myIsModified(Standard_True)
#endif
{

        SetFamily(Graphic2d_TOP_IMAGE);

        OSD_Path Path;
        myFile.Path (Path);
        Path.SystemName (myFileName);

#ifdef S3593
        Standard_ShortReal Xc,Yc,Width,Height;
        if( !ComputeCenterAndSize (Drawer(), Xc, Yc, Width, Height) ) {
#else
        myMinX = myX; myMinY = myY;
        myMaxX = myX; myMaxY = myY;

        Standard_Integer depth;
        if( !Values(aFile,myWidth,myHeight,depth) ) {
#endif
           char buff[512];
           sprintf (  buff, "Bad image file: %s\n", myFileName.ToCString ()  );
           Graphic2d_ImageDefinitionError::Raise (buff); 
        } 
}

void Graphic2d_ImageFile::Clear() {

#ifdef S3593
        myMinX = ShortRealLast(); myMinY = ShortRealLast();
        myMaxX = ShortRealFirst(); myMaxY = ShortRealFirst();
        Standard_ShortReal Xc,Yc,Width,Height;
        Handle(Graphic2d_Drawer) aDrawer = Drawer();
        if( aDrawer->IsWindowDriver() ) {
          aDrawer->ClearImageFile(myFileName.ToCString());
          ComputeCenterAndSize (aDrawer, Xc, Yc, Width, Height);
        }
#else
        myMinX = myX; myMinY = myY;
        myMaxX = myX; myMaxY = myY;
        myIsModified = Standard_True;
#endif
}

void Graphic2d_ImageFile::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

Standard_Boolean IsIn = Standard_False;
Standard_ShortReal Xc,Yc,Width,Height;

#ifdef S3593
  Width = myMaxX - myMinX; Height = myMaxY - myMinY;
  Xc = Standard_ShortReal((myMaxX + myMinX)/2.); Yc = Standard_ShortReal((myMaxY + myMinY)/2.);
#else
  if( !ComputeCenterAndSize (aDrawer, Xc, Yc, Width, Height) ) return;
#endif

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if (IsIn) {
static TShort_Array1OfShortReal TheX (1, 5);
static TShort_Array1OfShortReal TheY (1, 5);
    if (myGOPtr->IsTransformed ()) {
      Standard_Real A = Standard_Real (Xc);
      Standard_Real B = Standard_Real (Yc);
      (myGOPtr->Transform ()).Transforms (A, B);
      Xc = Standard_ShortReal (A);
      Yc = Standard_ShortReal (B);
    }
    Standard_ShortReal scale = (myIsZoomable) ? Standard_ShortReal(aDrawer->Scale()) : Standard_ShortReal(1.);
    aDrawer->DrawImageFile (myFileName.ToCString(), Xc, Yc, scale*myScale);

    // In case of GraphicObject Highlighted
    // the bounding box of the image is traced
    if (myGOPtr->IsHighlighted ()) {

#ifdef TRACE
        cout << " Scale : " << scale << "\n";
        cout << " Xc : " << Xc << "\n";
        cout << " Yc : " << Yc << "\n";
        cout << " Width : " << Width << "\n";
        cout << " Height : " << Height << "\n";
#endif

        TheX (1)        = Xc - Standard_ShortReal(Width / 2.);
        TheY (1)        = Yc - Standard_ShortReal(Height / 2.);
        TheX (2)        = Xc - Standard_ShortReal(Width / 2.);
        TheY (2)        = Yc + Standard_ShortReal(Height / 2.);
        TheX (3)        = Xc + Standard_ShortReal(Width / 2.);
        TheY (3)        = Yc + Standard_ShortReal(Height / 2.);
        TheX (4)        = Xc + Standard_ShortReal(Width / 2.);
        TheY (4)        = Yc - Standard_ShortReal(Height / 2.);
        TheX (5)        = Xc - Standard_ShortReal(Width / 2.);
        TheY (5)        = Yc - Standard_ShortReal(Height / 2.);

        aDrawer->SetLineAttrib (myColorIndex, 0, 1);
        aDrawer->MapPolylineFromTo (TheX, TheY);
    }
  } else {
#ifdef TRACE
    cout << " ImageFile " << this << " is out of the view" << endl;
#endif
  }
}

Standard_Boolean Graphic2d_ImageFile::Pick (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal aPrecision, const Handle(Graphic2d_Drawer)& aDrawer) {

Standard_ShortReal SRX = X, SRY = Y;
Standard_ShortReal Xc,Yc,Width,Height;
Standard_Boolean theStatus = Standard_False;

  if( ComputeCenterAndSize (aDrawer, Xc, Yc, Width, Height) ) {

    if (myGOPtr->IsTransformed ()) {
gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
      aTrsf.Transforms (RX, RY);
      SRX = Standard_ShortReal (RX);
      SRY = Standard_ShortReal (RY);
    }

Standard_ShortReal MINX = Xc - Standard_ShortReal(Width/2.);
Standard_ShortReal MINY = Yc - Standard_ShortReal(Height/2.);
Standard_ShortReal MAXX = Xc + Standard_ShortReal(Width/2.);
Standard_ShortReal MAXY = Yc + Standard_ShortReal(Height/2.);

    theStatus =
                (SRX >= MINX - aPrecision)
           &&   (SRX <= MAXX + aPrecision)
           &&   (SRY >= MINY - aPrecision)
           &&   (SRY <= MAXY + aPrecision)
         ;
  }

  return theStatus;

}

void Graphic2d_ImageFile::SetCenter (const Quantity_Length X, const Quantity_Length Y) {

        myX     = Standard_ShortReal(X);
        myY     = Standard_ShortReal(Y);
#ifdef S3593
        Standard_ShortReal Xc,Yc,Width,Height;
        ComputeCenterAndSize (Drawer(), Xc, Yc, Width, Height);
#endif

}

void Graphic2d_ImageFile::Translate (const Quantity_Length DX, const Quantity_Length DY) {

        myX     += Standard_ShortReal(DX);
        myY     += Standard_ShortReal(DY);
#ifdef S3593
        Standard_ShortReal Xc,Yc,Width,Height;
        ComputeCenterAndSize (Drawer(), Xc, Yc, Width, Height);
#endif

}

// Private methods

Standard_Boolean Graphic2d_ImageFile::ComputeCenterAndSize (const Handle(Graphic2d_Drawer)& aDrawer, Standard_ShortReal& cx, Standard_ShortReal& cy, Standard_ShortReal& Width, Standard_ShortReal& Height) {

Standard_ShortReal scale = (myIsZoomable) ? Standard_ShortReal(aDrawer->Scale()) : Standard_ShortReal(1.); 
Standard_ShortReal ox   = aDrawer->ConvertMapToFrom (mydx);
Standard_ShortReal oy   = aDrawer->ConvertMapToFrom (mydy);
Standard_ShortReal dx,dy;
//Standard_Integer depth;

#ifdef S3593
        if( !ImageSize (aDrawer, myFile, myFileName, myWidth, myHeight) )
                                                        return Standard_False;
#else
        if( myIsModified ) {
          if( !Values(myFile,myWidth,myHeight,depth) ) return Standard_False;
          myIsModified = Standard_False;
          aDrawer->ClearImageFile(myFileName.ToCString());
        }
#endif

        dx      = aDrawer->Convert (myWidth);
        dy      = aDrawer->Convert (myHeight);

        Width   = scale*myScale*aDrawer->ConvertMapToFrom (dx);
        Height  = scale*myScale*aDrawer->ConvertMapToFrom (dy);
 
        switch (myPlacement) {
                case Aspect_CP_North:
                        cx = myX + ox;
                        cy = myY + oy - Standard_ShortReal(Height/2.);
                break;
                case Aspect_CP_NorthEast:
                        cx = myX + ox - Standard_ShortReal(Width/2.);
                        cy = myY + oy - Standard_ShortReal(Height/2.);
                break;
                case Aspect_CP_East:
                        cx = myX + ox - Standard_ShortReal(Width/2.);
                        cy = myY + oy;
                break;
                case Aspect_CP_SouthEast:
                        cx = myX + ox - Standard_ShortReal(Width/2.);
                        cy = myY + oy + Standard_ShortReal(Height/2.);
                break;
                case Aspect_CP_South:
                        cx = myX + ox;
                        cy = myY + oy + Standard_ShortReal(Height/2.);
                break;
                case Aspect_CP_SouthWest:
                        cx = myX + ox + Standard_ShortReal(Width/2.);
                        cy = myY + oy + Standard_ShortReal(Height/2.);
                break;
                case Aspect_CP_West:
                        cx = myX + ox + Standard_ShortReal(Width/2.);
                        cy = myY + oy;
                break;
                case Aspect_CP_NorthWest:
                        cx = myX + ox + Standard_ShortReal(Width/2.);
                        cy = myY + oy - Standard_ShortReal(Height/2.);
                break;
                case Aspect_CP_Center:
                        cx = myX + ox;
                        cy = myY + oy;
                break;
        }

        myMinX = cx - Standard_ShortReal(Width/2.);
        myMinY = cy - Standard_ShortReal(Height/2.);
        myMaxX = cx + Standard_ShortReal(Width/2.);
        myMaxY = cy + Standard_ShortReal(Height/2.);

        return Standard_True;
}

void Graphic2d_ImageFile::SetOffset (const Quantity_Length dx, const Quantity_Length dy) {

        mydx    = Standard_ShortReal(dx);
        mydy    = Standard_ShortReal(dy);
        myMinX = myMaxX = myX;
        myMinY = myMaxY = myY;
}

void Graphic2d_ImageFile::SetPlacement (const Aspect_CardinalPoints aPlacement) {

        myPlacement     = aPlacement;
        myMinX = myMaxX = myX;
        myMinY = myMaxY = myY;

}

Standard_Boolean Graphic2d_ImageFile::IsZoomable () const {

        return myIsZoomable;

}

void Graphic2d_ImageFile::SetZoomable (const Standard_Boolean aFlag) {

        myIsZoomable    = aFlag;
#ifdef S3593
        Standard_ShortReal Xc,Yc,Width,Height;
        ComputeCenterAndSize (Drawer(), Xc, Yc, Width, Height);
#endif

}

void Graphic2d_ImageFile::SetScale (const Quantity_Factor aScale) {

        myScale = Standard_ShortReal(aScale);
#ifdef S3593
        Standard_ShortReal Xc,Yc,Width,Height;
        ComputeCenterAndSize (Drawer(), Xc, Yc, Width, Height);
#endif

}

Quantity_Factor Graphic2d_ImageFile::Scale () const {

        return Quantity_Factor (myScale);

}

Standard_Boolean Graphic2d_ImageFile::Values (OSD_File& aFile, Standard_Integer &aWidth, Standard_Integer &aHeight, Standard_Integer &aDepth) {

Standard_Boolean Result = Standard_False;
Standard_Boolean Reopen = Standard_False;
        aWidth = aHeight = aDepth = 0;
        if (aFile.IsOpen () && aFile.IsLocked()) {
                cout << "Can't analyse a locked file.\n" << flush;
                return Result;
        }

        if ( aFile.IsOpen () ) {
          Reopen =Standard_True;
          aFile.Seek (0,OSD_FromBeginning);
        } else {
OSD_Protection Protection (OSD_R, OSD_R, OSD_R, OSD_R);
          aFile.Open (OSD_ReadOnly, Protection);
          if (! aFile.IsOpen ()) {
                cout << "Error => Can't open input file as specified.\n"
                        << "No such file or directory.\n" << flush;
                return Result;
          }
        }

OSD_Path Path;
        aFile.Path (Path);
TCollection_AsciiString ext = Path.Extension ();

        Standard_Boolean XwdImage = (ext == TCollection_AsciiString (".xwd"));
        if (! XwdImage) {
                cout << "Error => File extension " << ext
                        << " is not available.\n" << flush;
                if( !Reopen ) aFile.Close ();
                return Result;
        }
XWDFileHeader *pheader;

        pheader = (XWDFileHeader *) malloc (sizeof (XWDFileHeader));
        if (pheader == NULL) {
                cout << "Can't malloc XWDFileHeader storage.\n" << flush;
                if( !Reopen ) aFile.Close ();
                return Result;
        }

Standard_Address addrheader     = Standard_Address (pheader);
Standard_Integer sizeheader     = Standard_Integer (sizeof (XWDFileHeader));
Standard_Integer readheader     = 0;

        aFile.Read (addrheader, sizeheader, readheader);
        if (readheader != sizeheader) {
                cout << "Unable to read dump file header.\n" << flush;
                if( !Reopen ) aFile.Close ();
                return Result;
        }

        if (SWAPTEST)
                Graphic2d_ImageFile::SwapLong (addrheader, sizeheader);

        aWidth  = Standard_Integer (pheader->pixmap_width);
        aHeight = Standard_Integer (pheader->pixmap_height);
        aDepth  = Standard_Integer (pheader->pixmap_depth);
        Result  = Standard_True;

        /* close the input file */
        if( !Reopen ) aFile.Close ();

#ifdef TRACE
TCollection_AsciiString aFileName;      
Path.SystemName (aFileName);
cout << "Graphic2d_Image_File::Values('" << aFileName << "'," << 
                aWidth << "," << aHeight << "," << aDepth << ")" << endl;
#endif
        return Result;

}

void Graphic2d_ImageFile::SwapLong (
        const Standard_Address anAddress,
        const Standard_Integer aNumOfBytes)
{
int n = (int) aNumOfBytes;
char *bp = (char *) anAddress;

char c;
char *ep = bp + n;
char *sp;

        while (bp < ep) {
                sp      = bp + 3;
                c       = *sp;
                *sp     = *bp;
                *bp++   = c;
                sp      = bp + 1;
                c       = *sp;
                *sp     = *bp;
                *bp++   = c;
                bp += 2;
        }
}

void Graphic2d_ImageFile::Position( Quantity_Length& X,Quantity_Length& Y ) const {
  
  X = Quantity_Length( myX );
  Y = Quantity_Length( myY );

}

void Graphic2d_ImageFile::Offset(Quantity_Length& aX,Quantity_Length& aY) const {
    
  aX = Quantity_Length( mydx );
  aY = Quantity_Length( mydy );
}

Aspect_CardinalPoints Graphic2d_ImageFile::Placement() const {
  return myPlacement;
}

OSD_File Graphic2d_ImageFile::ImageFile() const {
  return myFile;
}

void Graphic2d_ImageFile::Save(Aspect_FStream& aFStream) const
{
}
