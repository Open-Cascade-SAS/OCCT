#define xTRACE

// File:        Graphic2d_Drawer.cxx
// Created:     Mon Nov 22 18:50:16 1993
// Author:      Stephane CALLEGARI
//              <cal@sparc5>
// Modified     23/02/98 : FMN ; Remplacement PI par Standard_PI
// Modified     16/03/98 GG
//              OPTIMISATION
//              Ajouter un champs myWDriver et myPDriver initialise correctement        
//              lorsque le driver est du type "WindowDriver" ou "plotterdriver".
//              Eviter l'appel des contructeurs vides.

#define CTS17857//GG_200897
//              Permettre d'inhiber le clipping sur les primitives.

#define CSR_PRO12324        //SYL_030798
//              le Plot d'un objet en echo doit se faire en normal

#define DCB_220798 // DCB
//    Using pointers to drivers instead of handles
//    Reason : to avoid undesirable incrementation
//    of handle's links.

// Modified     17/12/1998 SYL pour S3604
//              Aspect_PlotterDriver devient PlotMgt_PlotterDriver

#define BUC60650	//GG 010300 Add protection on scale parameters
//			in SetxxxxTextAttrib() methods

#define OCC402         //SAV added method to map merkers operating with arrays

#include <Graphic2d_Drawer.ixx>
#include <Graphic2d_Primitive.pxx>
#include <Standard_Type.hxx>
#include <Aspect_Units.hxx>
#include <Precision.hxx>

#define MAXPOINTS 1023

#ifdef WNT
# include <WNT_DDriver.hxx>
#endif  // WNT

Graphic2d_Drawer::Graphic2d_Drawer () {

        myOverride              = Standard_False;
#ifdef DCB_220798
        myDriver = NULL;
        myWDriver = NULL;
        myPDriver = NULL;
#else // DCB_220798
        myDriver.Nullify();
        myWDriver.Nullify();
        myPDriver.Nullify();
#endif  // DCB_220798
        myDriverIsDefined       = Standard_False;
        myMinMaxIsActivated     = Standard_False;
        myClippingIsActivated     = Standard_True;
        myMinX = myMinY = ShortRealLast();
        myMaxX = myMaxY = ShortRealFirst();

        myDrawPrecision         = 0.;
        myDeflectionCoefficient = 0.;
        myTypeOfDeflection      = Aspect_TOD_RELATIVE;
        myTextPrecision         = 0.;
 
        myOverrideColor         = 0;
 
        myOffSet                = 0;
 
        mySpaceWidth            = 1. METER;
        mySpaceHeight           = 1. METER;

        SetValues (0., 0., 1. METER, 0., 0., 1., 1.);

}

void Graphic2d_Drawer::SetDrawPrecision(const Standard_Real aPrecision,
                                        const Standard_Real aCoefficient,
                                        const Aspect_TypeOfDeflection aType) {

        myDrawPrecision = Standard_ShortReal(aPrecision);
        myDeflectionCoefficient = aCoefficient;
        myTypeOfDeflection      = aType;

}

void Graphic2d_Drawer::DrawPrecision(Standard_ShortReal& aPrecision,
                                     Standard_ShortReal& aCoefficient,
                                     Aspect_TypeOfDeflection& aType) const {

        aPrecision      = myDrawPrecision;
        aCoefficient    = Standard_ShortReal(myDeflectionCoefficient);
        aType           = myTypeOfDeflection;

}

void Graphic2d_Drawer::SetTextPrecision(const Standard_ShortReal aPrecision) {

        myTextPrecision         = aPrecision;

}

Standard_ShortReal Graphic2d_Drawer::TextPrecision() const {

        return myTextPrecision;

}

void Graphic2d_Drawer::SetDriver(const Handle(Aspect_Driver)& aDriver) {

Standard_Real w,h;

#ifdef DCB_220798
        myDriver = aDriver.operator->();
        if( aDriver->IsKind(STANDARD_TYPE(Aspect_WindowDriver)) ) {
          myWDriver = (Handle(Aspect_WindowDriver)::DownCast(aDriver)).operator->();
          myPDriver = NULL;
        } else {
          myWDriver = NULL;
          myPDriver = (Handle(PlotMgt_PlotterDriver)::DownCast(aDriver)).operator->();
        }
#else // DCB_220798
        myDriver  = aDriver;
        myWDriver = Handle(Aspect_WindowDriver)::DownCast(aDriver);
        myPDriver = Handle(PlotMgt_PlotterDriver)::DownCast(aDriver);
#endif // DCB_220798
        myDriverIsDefined       = Standard_True;
        myDriver->WorkSpace(w,h);
        mySpaceWidth    = Standard_ShortReal(w);
        mySpaceHeight   = Standard_ShortReal(h);
#ifdef TRACE
        cout << "Graphic2d_Drawer::SetDriver() -> IsWindowDriver ? "
       << (myWDriver ? "TRUE " : "FALSE ") << flush;
        cout << "Graphic2d_Drawer::SetDriver() -> myDriverIsDefined: "
       << (myDriverIsDefined  ? "TRUE " : "FALSE ")
       << (int)myDriverIsDefined << endl << flush;
        cout << "Graphic2d_Drawer::SetDriver() -> Space: "
       << mySpaceWidth << " x " << mySpaceHeight << endl << flush;
#endif

}

Handle(Aspect_Driver) Graphic2d_Drawer::Driver() const {

//      if (! myDriverIsDefined )
//              Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        return myDriver;

}

Standard_Boolean Graphic2d_Drawer::IsWindowDriver() const {

#ifdef DCB_220798
        return (myWDriver) ? Standard_True : Standard_False;
#else
        return (myWDriver.IsNull()) ? Standard_False : Standard_True;
#endif
}

Standard_Boolean Graphic2d_Drawer::IsPlotterDriver() const {

#ifdef DCB_220798
        return (myPDriver) ? Standard_True : Standard_False;
#else
        return (myPDriver.IsNull()) ? Standard_False : Standard_True;
#endif
}

Handle(Aspect_WindowDriver) Graphic2d_Drawer::WindowDriver() const {

        return myWDriver;
}

Handle(PlotMgt_PlotterDriver) Graphic2d_Drawer::PlotterDriver() const {

        return myPDriver;
}

void Graphic2d_Drawer::SetValues (const Standard_Real XF,
                                  const Standard_Real YF,
                                  const Standard_Real SF,
                                  const Standard_Real XT,
                                  const Standard_Real YT,
                                  const Standard_Real ST,
                                  const Standard_Real ZF){

        myXF    = Standard_ShortReal(XF);
        myYF    = Standard_ShortReal(YF);
        mySF    = Standard_ShortReal(SF>Precision::Confusion()?SF:Precision::Confusion());
        myXT    = Standard_ShortReal(XT);
        myYT    = Standard_ShortReal(YT);
        myST    = Standard_ShortReal(ST>Precision::Confusion()?ST:Precision::Confusion());
        myZF    = Standard_ShortReal(ZF);

}

void Graphic2d_Drawer::Values(Standard_ShortReal& XF,
                              Standard_ShortReal& YF,
                              Standard_ShortReal& SF,
                              Standard_ShortReal& XT,
                              Standard_ShortReal& YT,
                              Standard_ShortReal& ST,
                              Standard_ShortReal& ZF) const {

        XF      = myXF;
        YF      = myYF;
        SF      = mySF;
        XT      = myXT;
        YT      = myYT;
        ST      = myST;
        ZF      = myZF;

}

void Graphic2d_Drawer::SetOverride(const Standard_Boolean aValue) {

        myOverride      = aValue;

}

void Graphic2d_Drawer::SetOverrideColor(const Standard_Integer anIndex) {

        myOverrideColor = anIndex;

}

void Graphic2d_Drawer::SetOffSet(const Standard_Integer anOffSet) {

        myOffSet        = anOffSet;

}

void Graphic2d_Drawer::GetMapFrom(const Standard_ShortReal x1,
                                  const Standard_ShortReal y1,
                                  Standard_ShortReal& x2,
                                  Standard_ShortReal& y2) const {

        x2      = (x1-myXF)/mySF;
        y2      = (y1-myYF)/mySF;

}

void Graphic2d_Drawer::GetMapFromTo(const Standard_ShortReal x1,
                                 const Standard_ShortReal y1,
                                 Standard_ShortReal& x2,
                                 Standard_ShortReal& y2) const {

        x2      = myXT + ((x1-myXF)/mySF) *myST;
        y2      = myYT + ((y1-myYF)/mySF) *myST;

}

Standard_Boolean Graphic2d_Drawer::GetTextSize(
                                  const TCollection_ExtendedString &aText,
                                  Standard_ShortReal &aWidth,
                                  Standard_ShortReal &aHeight) const {

        if( IsWindowDriver() ) {
          myWDriver->TextSize(aText,aWidth,aHeight);
          aWidth /= Scale();
          aHeight /= Scale();
          return Standard_True;
#ifdef WNT
        } else 
          if (   myDriver -> IsKind (  STANDARD_TYPE( WNT_DDriver )  )   ) {
      
          Handle( WNT_DDriver ) :: DownCast ( myDriver ) -> TextSize ( aText, aWidth, aHeight );
          aWidth  /= Scale ();
          aHeight /= Scale ();
          return Standard_True;
#endif  // WNT
        } else {
          aWidth = aHeight = 0.;
          return Standard_False;
        }  // end else
}

Standard_Boolean Graphic2d_Drawer::GetTextSize(
                                  const TCollection_ExtendedString &aText,
                                  Standard_ShortReal &aWidth,
                                  Standard_ShortReal &aHeight,
                                  Standard_ShortReal &anXoffset,
                                  Standard_ShortReal &anYoffset) const {

        if( IsWindowDriver() ) {
          myWDriver->TextSize(aText,aWidth,aHeight,anXoffset,anYoffset);
          aWidth /= Scale();
          aHeight /= Scale();
          anXoffset /= Scale();
          anYoffset /= Scale();
          return Standard_True;
#ifdef WNT
        } else
          if (   myDriver -> IsKind (  STANDARD_TYPE( WNT_DDriver )  )   ) {
      
          Handle( WNT_DDriver ) :: DownCast ( myDriver ) -> TextSize ( aText, aWidth, aHeight );
          aWidth  /= Scale ();
          aHeight /= Scale ();
          return Standard_True;
#endif  // WNT
        } else {
          aWidth = aHeight = anXoffset = anYoffset = 0.;
          return Standard_False;
        }  // end else
}

Standard_Boolean Graphic2d_Drawer::GetFontSize(
                                  Standard_ShortReal &aHeight,
                                  Standard_ShortReal &aBheight,
                                  Standard_ShortReal &aSlant) const {

        if( IsWindowDriver() ) {
          Quantity_PlaneAngle slant;
          myWDriver->FontSize(slant,aHeight,aBheight);
          aHeight /= Scale();
          aBheight /= Scale();
          aSlant = Standard_ShortReal(slant);
          return Standard_True;
        } else {
          aHeight = aBheight = aSlant = 0.;
          return Standard_False;
        }
}

Standard_Boolean Graphic2d_Drawer::GetImageSize(
                                  const Standard_CString aFileName,
                                  Standard_ShortReal &aWidth,
                                  Standard_ShortReal &aHeight) const {

    Standard_Boolean theStatus = Standard_False;
    Standard_Integer width,height;

        if( myDriverIsDefined ) {
          if(( theStatus = myDriver->SizeOfImageFile(aFileName,width,height))){
            aWidth = Standard_ShortReal(myDriver->Convert(width)/Scale());
            aHeight = Standard_ShortReal(myDriver->Convert(height)/Scale());
          }
        }

        if( !theStatus ) {
          aWidth = aHeight = 0.;
        }
        return theStatus;
}

Standard_Boolean Graphic2d_Drawer::GetImageSize(
                                  const Standard_CString aFileName,
                                  Standard_Integer &aWidth,
                                  Standard_Integer &aHeight) const {
Standard_Boolean theStatus = Standard_False;

        if( myDriverIsDefined ) {
          theStatus = myDriver->SizeOfImageFile(aFileName,aWidth,aHeight);
        } 

        if( !theStatus ) {
          aWidth = aHeight = 0;
        }
        return theStatus;
}

void Graphic2d_Drawer::MapSegmentFromTo (
                               const Standard_ShortReal x1,
                               const Standard_ShortReal y1,
                               const Standard_ShortReal x2,
                               const Standard_ShortReal y2,
                               const Standard_Integer aMode) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx1,my1,mx2,my2;
        mx1 = myXT + ((x1-myXF)/mySF) *myST;
        my1 = myYT + ((y1-myYF)/mySF) *myST;
        mx2 = myXT + ((x2-myXF)/mySF) *myST;
        my2 = myYT + ((y2-myYF)/mySF) *myST;

        if( aMode > 0 ) myDriver->BeginSegments();
        myDriver->DrawSegment(mx1,my1,mx2,my2);
        if( aMode < 0 ) myDriver->ClosePrimitive();
        if( myMinMaxIsActivated ) {
          myMinX = Min(mx1,myMinX); myMinX = Min(mx2,myMinX);
          myMinY = Min(my1,myMinY); myMinY = Min(my2,myMinY);
          myMaxX = Max(mx1,myMaxX); myMaxX = Max(mx2,myMaxX);
          myMaxY = Max(my1,myMaxY); myMaxY = Max(my2,myMaxY);
        }

}

void Graphic2d_Drawer::DrawSegment (
                               const Standard_ShortReal x1,
                               const Standard_ShortReal y1,
                               const Standard_ShortReal x2,
                               const Standard_ShortReal y2) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        myDriver->DrawSegment(x1,y1,x2,y2);
        if( myMinMaxIsActivated ) {
          myMinX = Min(x1,myMinX); myMinX = Min(x2,myMinX);
          myMinY = Min(y1,myMinY); myMinY = Min(y2,myMinY);
          myMaxX = Max(x1,myMaxX); myMaxX = Max(x2,myMaxX);
          myMaxY = Max(y1,myMaxY); myMaxY = Max(y2,myMaxY);
        }
}

void Graphic2d_Drawer::DrawText(
                                     const TCollection_ExtendedString& aText,
                                     const Standard_ShortReal x,
                                     const Standard_ShortReal y,
                                     const Standard_ShortReal anAngle,
                                     const Aspect_TypeOfText aType) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx = x,my = y;

        myDriver->DrawText(aText,mx,my,anAngle,aType);

        if( myMinMaxIsActivated && IsWindowDriver() ) {
          Standard_ShortReal width,height,xoffset,yoffset;
          myWDriver->TextSize(aText,width,height,xoffset,yoffset);
          if( width > 0. && height > 0. ) {
            Standard_ShortReal x1,y1,x2,y2,x3,y3,x4,y4;

            if ( !anAngle ) {
              Standard_ShortReal cosa = Standard_ShortReal(Cos(anAngle));
              Standard_ShortReal sina = Standard_ShortReal(Sin(anAngle));

              x1 = mx + XROTATE(xoffset,-yoffset) ;
              y1 = my + YROTATE(xoffset,-yoffset) ;

              x2 = mx + XROTATE(xoffset,height-yoffset) ;
              y2 = my + YROTATE(xoffset,height-yoffset) ;

              x3 = mx + XROTATE(width+xoffset,height-yoffset) ;
              y3 = my + YROTATE(width+xoffset,height-yoffset) ;

              x4 = mx + XROTATE(width+xoffset,-yoffset) ;
              y4 = my + YROTATE(width+xoffset,-yoffset) ;

            } else {   
              x1 = mx+xoffset; y1 = my-yoffset;
              x2 = mx+xoffset; y2 = my+height-yoffset;
              x3 = mx+width+xoffset; y3 = my+height-yoffset;
              x4 = mx+width+xoffset; y4 = my-yoffset;
            }
            myMinX = Min(x1,myMinX); myMinY = Min(y1,myMinY);
            myMaxX = Max(x1,myMaxX); myMaxY = Max(y1,myMaxY);
            myMinX = Min(x2,myMinX); myMinY = Min(y2,myMinY);
            myMaxX = Max(x2,myMaxX); myMaxY = Max(y2,myMaxY);
            myMinX = Min(x3,myMinX); myMinY = Min(y3,myMinY);
            myMaxX = Max(x3,myMaxX); myMaxY = Max(y3,myMaxY);
            myMinX = Min(x4,myMinX); myMinY = Min(y4,myMinY);
            myMaxX = Max(x4,myMaxX); myMaxY = Max(y4,myMaxY);
          }
        }
}

void Graphic2d_Drawer::MapTextFromTo(
                                     const TCollection_ExtendedString& aText,
                                     const Standard_ShortReal x,
                                     const Standard_ShortReal y,
                                     const Standard_ShortReal anAngle,
                                     const Standard_ShortReal aDeltax,
                                     const Standard_ShortReal aDeltay,
                                     const Aspect_TypeOfText aType) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx,my;
        mx = myXT + aDeltax + ((x-myXF)/mySF) *myST;
        my = myYT + aDeltay + ((y-myYF)/mySF) *myST;
        DrawText(aText,mx,my,anAngle,aType);
}

void Graphic2d_Drawer::DrawPolyText(
                                     const TCollection_ExtendedString& aText,
                                     const Standard_ShortReal x,
                                     const Standard_ShortReal y,
                                     const Standard_ShortReal anAngle,
                                     const Standard_ShortReal aMargin,
                                     const Aspect_TypeOfText aType) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx = x,my = y;
        myDriver->DrawPolyText(aText,mx,my,aMargin,anAngle,aType);

        if( myMinMaxIsActivated && IsWindowDriver() ) {
          Standard_ShortReal width,height,xoffset,yoffset;
          myWDriver->TextSize(aText,width,height,xoffset,yoffset);
          if( width > 0. && height > 0. ) {
            Standard_ShortReal x1,y1,x2,y2,x3,y3,x4,y4,dxm,dym;
            dxm = dym = height*aMargin;

            if( !anAngle ) {
              Standard_ShortReal cosa = Standard_ShortReal(Cos(anAngle));
              Standard_ShortReal sina = Standard_ShortReal(Sin(anAngle));

              x1 = mx + XROTATE(-dxm+xoffset,-dym-yoffset) ;
              y1 = my + YROTATE(-dxm+xoffset,-dym-yoffset) ;

              x2 = mx + XROTATE(-dxm+xoffset,height+dym-yoffset) ;
              y2 = my + YROTATE(-dxm+xoffset,height+dym-yoffset) ;

              x3 = mx + XROTATE(width+dxm+xoffset,height+dym-yoffset) ;
              y3 = my + YROTATE(width+dxm+xoffset,height+dym-yoffset) ;

              x4 = mx + XROTATE(width+dxm+xoffset,-dym-yoffset) ;
              y4 = my + YROTATE(width+dxm+xoffset,-dym-yoffset) ;

            } else {
              x1 = mx-dxm+xoffset; y1 = my-dym-yoffset;
              x2 = mx-dxm+xoffset; y2 = my+height+dym-yoffset;
              x3 = mx+width+dxm+xoffset; y3 = my+height+dym-yoffset;
              x4 = mx+width+dxm+xoffset; y4 = my-dym-yoffset;
            }
            myMinX = Min(x1,myMinX); myMinY = Min(y1,myMinY);
            myMaxX = Max(x1,myMaxX); myMaxY = Max(y1,myMaxY);
            myMinX = Min(x2,myMinX); myMinY = Min(y2,myMinY);
            myMaxX = Max(x2,myMaxX); myMaxY = Max(y2,myMaxY);
            myMinX = Min(x3,myMinX); myMinY = Min(y3,myMinY);
            myMaxX = Max(x3,myMaxX); myMaxY = Max(y3,myMaxY);
            myMinX = Min(x4,myMinX); myMinY = Min(y4,myMinY);
            myMaxX = Max(x4,myMaxX); myMaxY = Max(y4,myMaxY);
          }
        }
}

void Graphic2d_Drawer::MapPolyTextFromTo(
                                     const TCollection_ExtendedString& aText,
                                     const Standard_ShortReal x,
                                     const Standard_ShortReal y,
                                     const Standard_ShortReal anAngle,
                                     const Standard_ShortReal aMargin,
                                     const Standard_ShortReal aDeltax,
                                     const Standard_ShortReal aDeltay,
                                     const Aspect_TypeOfText aType) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx,my;
        mx = myXT + aDeltax + ((x-myXF)/mySF) *myST;
        my = myYT + aDeltay + ((y-myYF)/mySF) *myST;
        DrawPolyText(aText,mx,my,anAngle,aMargin,aType);
}

void Graphic2d_Drawer::DrawFramedText(
                                     const TCollection_ExtendedString& aText,
                                     const Standard_ShortReal x,
                                     const Standard_ShortReal y,
                                     const Standard_ShortReal anAngle,
                                     const Standard_ShortReal aMargin,
                                     const Aspect_TypeOfText aType) {

        DrawPolyText(aText,x,y,anAngle,aMargin,aType);
}


void Graphic2d_Drawer::MapFramedTextFromTo(
                                     const TCollection_ExtendedString& aText,
                                     const Standard_ShortReal x,
                                     const Standard_ShortReal y,
                                     const Standard_ShortReal anAngle,
                                     const Standard_ShortReal aMargin,
                                     const Standard_ShortReal aDeltax,
                                     const Standard_ShortReal aDeltay,
                                     const Aspect_TypeOfText aType) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx,my;
        mx = myXT + aDeltax + ((x-myXF)/mySF) *myST;
        my = myYT + aDeltay + ((y-myYF)/mySF) *myST;
        DrawFramedText(aText,mx,my,anAngle,aMargin,aType);
}

void Graphic2d_Drawer::UnMapFromTo (const Standard_ShortReal x1,
                                 const Standard_ShortReal y1,
                                 Standard_ShortReal& x2,
                                 Standard_ShortReal& y2) const {

        x2 = ((x1-myXT) /myST) *mySF + myXF;
        y2 = ((y1-myYT) /myST) *mySF + myYF;

}

void Graphic2d_Drawer::DrawPolyline (
                               const Standard_Real aDeltaX,
                               const Standard_Real aDeltaY,
                               const TShort_Array1OfShortReal& aListX,
                               const TShort_Array1OfShortReal& aListY) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

 Standard_Integer n1 = aListX.Lower();
 Standard_Integer n2 = aListX.Upper();
 Standard_ShortReal X,Y;

 myDriver->BeginPolyline(n2-n1+1);
 for (Standard_Integer i=n1 ; i<=n2; i++) {
   X =  Standard_ShortReal(aDeltaX) + Standard_ShortReal(aListX(i));
   Y =  Standard_ShortReal(aDeltaY) + Standard_ShortReal(aListY(i));
   myDriver->DrawPoint(X,Y);
   if( myMinMaxIsActivated ) {
     myMinX = Min(X,myMinX);
     myMinY = Min(Y,myMinY);
     myMaxX = Max(X,myMaxX);
     myMaxY = Max(Y,myMaxY);
   }
 }
 myDriver->ClosePrimitive();

}

void Graphic2d_Drawer::MapPolylineFromTo (
                               const TShort_Array1OfShortReal& aListX,
                               const TShort_Array1OfShortReal& aListY,
                               const Standard_Integer Npoint) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

 Standard_Integer n1 = aListX.Lower();
 Standard_Integer n2 = (Npoint > 0) ? n1+Npoint-1 : aListX.Upper();
 Standard_ShortReal X,Y;

 myDriver->BeginPolyline(n2-n1+1);
 for (Standard_Integer i=n1 ; i<=n2; i++) {
   X = myXT + ((aListX(i)-myXF)/mySF) *myST;
   Y = myYT + ((aListY(i)-myYF)/mySF) *myST;
   myDriver->DrawPoint(X,Y);
   if( myMinMaxIsActivated ) {
     myMinX = Min(X,myMinX);
     myMinY = Min(Y,myMinY);
     myMaxX = Max(X,myMaxX);
     myMaxY = Max(Y,myMaxY);
   }
 }
 myDriver->ClosePrimitive();

}

void Graphic2d_Drawer::MapPolylineFromTo (
                               const Standard_ShortReal x,
                               const Standard_ShortReal y,
                               const Standard_Integer aMode) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal X,Y;
        X = myXT + ((x-myXF)/mySF) *myST;
        Y = myYT + ((y-myYF)/mySF) *myST;

        if( aMode > 0 ) myDriver->BeginPolyline(aMode);
        myDriver->DrawPoint(X,Y);
        if( myMinMaxIsActivated ) {
          myMinX = Min(X,myMinX);
          myMinY = Min(Y,myMinY);
          myMaxX = Max(X,myMaxX);
          myMaxY = Max(Y,myMaxY);
        }
        if( aMode < 0 ) myDriver->ClosePrimitive();
}

void Graphic2d_Drawer::DrawMarker(
                        const Standard_Integer anindex,
                        const Standard_ShortReal x,
                        const Standard_ShortReal y,
                        const Standard_ShortReal awidth,
                        const Standard_ShortReal anheight,
                        const Standard_ShortReal anAngle) {
        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx = x,my = y;

        if( anindex > 0 && awidth > 0. && anheight > 0. ) {
          myDriver->DrawMarker(anindex,mx,my,awidth,anheight,anAngle);
          if( myMinMaxIsActivated ) {
            Standard_ShortReal d = Standard_ShortReal(Sqrt(awidth*awidth + anheight*anheight));
            myMinX = Min(mx-d,myMinX);
            myMinY = Min(my-d,myMinY);
            myMaxX = Max(mx+d,myMaxX);
            myMaxY = Max(my+d,myMaxY);
          }
        } else {
          myDriver->DrawPoint(mx,my);
          if( myMinMaxIsActivated ) {
            myMinX = Min(mx,myMinX);
            myMinY = Min(my,myMinY);
            myMaxX = Max(mx,myMaxX);
            myMaxY = Max(my,myMaxY);
          }
        }
}

void Graphic2d_Drawer::MapMarkerFromTo(
                        const Standard_Integer anindex,
                        const Standard_ShortReal x,
                        const Standard_ShortReal y,
                        const Standard_ShortReal awidth,
                        const Standard_ShortReal anheight,
                        const Standard_ShortReal anAngle,
                        const Standard_Integer aMode) {
        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx,my;
        mx = myXT + ((x-myXF)/mySF) *myST;
        my = myYT + ((y-myYF)/mySF) *myST;

        if( anindex > 0 && awidth > 0. && anheight > 0. ) {
          if( aMode > 0 ) myDriver->BeginMarkers();
          myDriver->DrawMarker(anindex,mx,my,awidth,anheight,anAngle);
          if( myMinMaxIsActivated ) {
            Standard_ShortReal d = Standard_ShortReal(Sqrt(awidth*awidth + anheight*anheight));
            myMinX = Min(mx-d,myMinX);
            myMinY = Min(my-d,myMinY);
            myMaxX = Max(mx+d,myMaxX);
            myMaxY = Max(my+d,myMaxY);
          }
        } else {
          if( aMode > 0 ) myDriver->BeginPoints();
          myDriver->DrawPoint(mx,my);
          if( myMinMaxIsActivated ) {
            myMinX = Min(mx,myMinX);
            myMinY = Min(my,myMinY);
            myMaxX = Max(mx,myMaxX);
            myMaxY = Max(my,myMaxY);
          }
        }
        if( aMode < 0 ) myDriver->ClosePrimitive();

}

void Graphic2d_Drawer
::MapMarkersFromTo( const Standard_Integer anindex,
		    const Handle(TShort_HArray1OfShortReal)& x,
		    const Handle(TShort_HArray1OfShortReal)& y,
		    const Standard_ShortReal awidth,
		    const Standard_ShortReal anheight,
		    const Standard_ShortReal anAngle,
		    const Standard_Integer aMode)
{
#ifdef OCC402

  if (! myDriverIsDefined)
    Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

  if( anindex > 0 && awidth > 0. && anheight > 0. ) {

    Standard_ShortReal mx,my;
    myDriver->BeginMarkers();

    for ( Standard_Integer i = x->Lower(); i <= x->Upper(); i++ ) {
      mx = myXT + ( ( x->Value( i ) - myXF ) / mySF ) * myST;
      my = myYT + ( ( y->Value( i ) - myYF ) / mySF ) * myST;
      myDriver->DrawMarker(anindex,mx,my,awidth,anheight,anAngle);
    }
    myDriver->ClosePrimitive();
  }
#endif
}

void Graphic2d_Drawer::DrawPolygon (
                               const Standard_Real aDeltaX,
                               const Standard_Real aDeltaY,
                               const TShort_Array1OfShortReal& aListX,
                               const TShort_Array1OfShortReal& aListY) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

 Standard_Integer n1 = aListX.Lower();
 Standard_Integer n2 = aListX.Upper();
 Standard_ShortReal X,Y;

 myDriver->BeginPolygon(n2-n1+1);
 for (Standard_Integer i=n1 ; i<=n2; i++) {
   X = Standard_ShortReal(aDeltaX) + Standard_ShortReal(aListX(i));
   Y = Standard_ShortReal(aDeltaY) + Standard_ShortReal(aListY(i));
   myDriver->DrawPoint(X,Y);
   if( myMinMaxIsActivated ) {
     myMinX = Min(X,myMinX);
     myMinY = Min(Y,myMinY);
     myMaxX = Max(X,myMaxX);
     myMaxY = Max(Y,myMaxY);
   }
 }
 myDriver->ClosePrimitive();

}

void Graphic2d_Drawer::MapPolygonFromTo (
                               const TShort_Array1OfShortReal& aListX,
                               const TShort_Array1OfShortReal& aListY,
                               const Standard_Integer Npoint) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

 Standard_Integer n1 = aListX.Lower();
 Standard_Integer n2 = (Npoint > 0) ? n1+Npoint-1 : aListX.Upper();
 Standard_ShortReal X,Y;

 myDriver->BeginPolygon(n2-n1+1);
 for (Standard_Integer i=n1 ; i<=n2; i++) {
   X = myXT + ((aListX(i)-myXF)/mySF) *myST;
   Y = myYT + ((aListY(i)-myYF)/mySF) *myST;
   myDriver->DrawPoint(X,Y);
   if( myMinMaxIsActivated ) {
     myMinX = Min(X,myMinX);
     myMinY = Min(Y,myMinY);
     myMaxX = Max(X,myMaxX);
     myMaxY = Max(Y,myMaxY);
   }
 }
 myDriver->ClosePrimitive();

}

void Graphic2d_Drawer::MapPolygonFromTo (
                               const Standard_ShortReal x,
                               const Standard_ShortReal y,
                               const Standard_Integer aMode) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal X,Y;
        X = myXT + ((x-myXF)/mySF) *myST;
        Y = myYT + ((y-myYF)/mySF) *myST;

        if( aMode > 0 ) myDriver->BeginPolygon(aMode);
        myDriver->DrawPoint(X,Y);
        if( myMinMaxIsActivated ) {
          myMinX = Min(X,myMinX);
          myMinY = Min(Y,myMinY);
          myMaxX = Max(X,myMaxX);
          myMaxY = Max(Y,myMaxY);
        }
        if( aMode < 0 ) myDriver->ClosePrimitive();

}

void Graphic2d_Drawer::DrawArc(
                        const Standard_Real aDeltaX,
                        const Standard_Real aDeltaY,
                        const Standard_ShortReal aRadius,
                        const Standard_ShortReal Angle1,
                        const Standard_ShortReal Angle2) {
        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx,my,a1,a2,da;
        mx = Standard_ShortReal(aDeltaX);
        my = Standard_ShortReal(aDeltaY);
        a1 = (Angle1 >= 0.) ? Angle1 : Angle1 + Standard_ShortReal(2.*Standard_PI);
        a2 = (Angle2 >= 0.) ? Angle2 : Angle2 + Standard_ShortReal(2.*Standard_PI);
        if( a2 > a1 ) {
          da = a2 - a1;
        } else if( a2 < a1 ) {
          da = Standard_ShortReal(2.*Standard_PI + a2 - a1);
        } else {
          a1 = 0.; da = Standard_ShortReal(2.*Standard_PI);
        }
        
        if( aRadius > 0. ) {    // Try to use hardware first
          Standard_Boolean status =
                   myDriver->DrawArc(mx,my,aRadius,aRadius,a1,da);
          if( !status ) {                       // or Emulate
            Standard_ShortReal precis = myDrawPrecision;
            Standard_ShortReal value;
            if( myTypeOfDeflection == Aspect_TOD_RELATIVE) {
              precis = Standard_ShortReal(aRadius * myDeflectionCoefficient);
            }
            if( aRadius > precis )
              value = Standard_ShortReal(Max( 0.0044, Min (0.7854 , 2. * ACos(1.-precis/aRadius))));
            else
              value = Standard_ShortReal(0.7854);  // = PI/4.
              Standard_Integer nbpoints = Min(MAXPOINTS,
                                Standard_Integer(Abs(da)/value)+2);
              Standard_ShortReal teta = da/nbpoints;
              Standard_ShortReal cosin = Standard_ShortReal(Cos(teta));
              Standard_ShortReal x1 = Standard_ShortReal(aRadius * Cos(a1));
              Standard_ShortReal y1 = Standard_ShortReal(aRadius * Sin(a1));
              Standard_ShortReal x2 = Standard_ShortReal(aRadius * Cos(a1+teta));
              Standard_ShortReal y2 = Standard_ShortReal(aRadius * Sin(a1+teta));
              Standard_ShortReal x,y;

            nbpoints++;
            myDriver->BeginPolyline(nbpoints);
            myDriver->DrawPoint(mx+x1,my+y1);
            myDriver->DrawPoint(mx+x2,my+y2);
            for (Standard_Integer i=3; i<= nbpoints; i++) {
              x = 2*x2*cosin - x1; x1 = x2; x2 = x;
              y = 2*y2*cosin - y1; y1 = y2; y2 = y;
              myDriver->DrawPoint(mx+x,my+y);
            }
            myDriver->ClosePrimitive();
          }
          if( myMinMaxIsActivated ) {
            myMinX = Min(mx-aRadius,myMinX);
            myMinY = Min(my-aRadius,myMinY);
            myMaxX = Max(mx+aRadius,myMaxX);
            myMaxY = Max(my+aRadius,myMaxY);
          }
        }
}

void Graphic2d_Drawer::MapArcFromTo(
                        const Standard_ShortReal x,
                        const Standard_ShortReal y,
                        const Standard_ShortReal aRadius,
                        const Standard_ShortReal Angle1,
                        const Standard_ShortReal Angle2,
                        const Standard_Integer aMode) {
        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx,my,r;
        mx = myXT + ((x-myXF)/mySF) *myST;
        my = myYT + ((y-myYF)/mySF) *myST;
        r = aRadius/mySF*myST;
        
        if( aRadius > 0. ) {
          DrawArc(mx,my,r,Angle1,Angle2);
          if( myMinMaxIsActivated ) {
            myMinX = Min(mx-aRadius,myMinX);
            myMinY = Min(my-aRadius,myMinY);
            myMaxX = Max(mx+aRadius,myMaxX);
            myMaxY = Max(my+aRadius,myMaxY);
          }
        }
}

void Graphic2d_Drawer::DrawPolyArc(
                        const Standard_Real aDeltaX,
                        const Standard_Real aDeltaY,
                        const Standard_ShortReal aRadius,
                        const Standard_ShortReal Angle1,
                        const Standard_ShortReal Angle2) {
        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx,my,a1,a2,da;
        mx = Standard_ShortReal(aDeltaX);
        my = Standard_ShortReal(aDeltaY);
        a1 = (Angle1 >= 0.) ? Angle1 : Angle1 + Standard_ShortReal(2.*Standard_PI);
        a2 = (Angle2 >= 0.) ? Angle2 : Angle2 + Standard_ShortReal(2.*Standard_PI);
        if( a2 > a1 ) {
          da = a2 - a1;
        } else if( a2 < a1 ) {
          da = Standard_ShortReal(2.*Standard_PI + a2 - a1);
        } else {
          a1 = 0.; da = Standard_ShortReal(2.*Standard_PI);
        }
        
        if( aRadius > 0. ) {    // Try to use hardware first
          Standard_Boolean status =
                   myDriver->DrawPolyArc(mx,my,aRadius,aRadius,a1,da);
          if( !status ) {                       // or Emulate
            Standard_ShortReal precis = myDrawPrecision;
            Standard_ShortReal value;
            if( myTypeOfDeflection == Aspect_TOD_RELATIVE) {
              precis = Standard_ShortReal(aRadius * myDeflectionCoefficient);
            }
            if( aRadius > precis )
              value = Standard_ShortReal(Max( 0.0044, Min (0.7854 , 2. * ACos(1.-precis/aRadius))));
            else
              value = Standard_ShortReal(0.7854);  // = PI/4.
              Standard_Integer nbpoints = Min(MAXPOINTS,
                                Standard_Integer(Abs(da)/value)+2);
              Standard_ShortReal teta = da/nbpoints;
            Standard_ShortReal cosin = Standard_ShortReal(Cos(teta));
            Standard_ShortReal x1 = Standard_ShortReal(aRadius * Cos(a1));
            Standard_ShortReal y1 = Standard_ShortReal(aRadius * Sin(a1));
            Standard_ShortReal x2 = Standard_ShortReal(aRadius * Cos(a1+teta));
            Standard_ShortReal y2 = Standard_ShortReal(aRadius * Sin(a1+teta));
            Standard_ShortReal x,y;

            nbpoints++;
            myDriver->BeginPolygon(nbpoints);
            myDriver->DrawPoint(mx+x1,my+y1);
            myDriver->DrawPoint(mx+x2,my+y2);
            for (Standard_Integer i=3; i<= nbpoints; i++) {
              x = 2*x2*cosin - x1; x1 = x2; x2 = x;
              y = 2*y2*cosin - y1; y1 = y2; y2 = y;
              myDriver->DrawPoint(mx+x,my+y);
            }
            myDriver->ClosePrimitive();
          }
          if( myMinMaxIsActivated ) {
            myMinX = Min(mx-aRadius,myMinX);
            myMinY = Min(my-aRadius,myMinY);
            myMaxX = Max(mx+aRadius,myMaxX);
            myMaxY = Max(my+aRadius,myMaxY);
          }
        }
}

void Graphic2d_Drawer::MapPolyArcFromTo(
                        const Standard_ShortReal x,
                        const Standard_ShortReal y,
                        const Standard_ShortReal aRadius,
                        const Standard_ShortReal Angle1,
                        const Standard_ShortReal Angle2,
                        const Standard_Integer aMode) {
        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal mx,my,r;
        mx = myXT + ((x-myXF)/mySF) *myST;
        my = myYT + ((y-myYF)/mySF) *myST;
        r = aRadius/mySF*myST;
        
        if( aRadius > 0. ) {
          DrawPolyArc(mx,my,r,Angle1,Angle2);
          if( myMinMaxIsActivated ) {
            myMinX = Min(mx-aRadius,myMinX);
            myMinY = Min(my-aRadius,myMinY);
            myMaxX = Max(mx+aRadius,myMaxX);
            myMaxY = Max(my+aRadius,myMaxY);
          }
        }
}

void Graphic2d_Drawer::SetRejection( const Standard_Boolean aClippingFlag) {
        myClippingIsActivated     = aClippingFlag;
}

Standard_Boolean Graphic2d_Drawer::IsIn(
                                        const Standard_ShortReal aMinX,
                                        const Standard_ShortReal aMaxX,
                                        const Standard_ShortReal aMinY,
                                        const Standard_ShortReal aMaxY)const
{
#ifdef CTS17857
  if( !myClippingIsActivated ) return Standard_True;
#endif

  Standard_ShortReal x,X,y,Y;

  x = myXT + ((aMinX-myXF)/mySF) *myST;
  y = myYT + ((aMinY-myYF)/mySF) *myST;
  X = myXT + ((aMaxX-myXF)/mySF) *myST;
  Y = myYT + ((aMaxY-myYF)/mySF) *myST;

  Standard_Boolean clip = ((x < 0. && X < 0.) ||
            (x > mySpaceWidth && X > mySpaceWidth) ||
            (y < 0. && Y < 0.) ||
            (y > mySpaceHeight && Y > mySpaceHeight));

#ifdef TRACE
  if( clip ) printf(" *Rejected primitive %f,%f,%f,%f\n",
                aMinX,aMinY,aMaxX,aMaxY);
#endif

  return !clip;
}

void Graphic2d_Drawer::DrawInfiniteLine (
                               const Standard_ShortReal x,
                               const Standard_ShortReal y,
                               const Standard_ShortReal dx,
                               const Standard_ShortReal dy) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

 Standard_ShortReal mx = x,my = y,x1,y1,x2=0,y2=0;
 Standard_Boolean status = Standard_True;

 if ( dx == 0. ) {
   // Bug de rejection des lignes infinies
   // if ( my >= 0. && my <= mySpaceHeight )
   x1 = mx; y1 = 0.; x2 = mx; y2 = mySpaceHeight;
 }
 else if ( dy == 0.) {
   // Bug de rejection des lignes infinies
   // if ( mx >= 0. && mx <= mySpaceWidth )
   x1 = 0.; y1 = my; x2 = mySpaceWidth; y2 = my;
 }
 else {
// equation of the line:   X = mx + lambda * dx
//                         Y = my + lambda * dy
   Standard_ShortReal lambda, lmin=ShortRealLast(),lmax= ShortRealFirst();

   lambda = -mx/dx; y1 = my + lambda *dy;
   if ( y1 >= 0. && y1 <= mySpaceHeight) {
     lmin = lmax = lambda;}

   lambda = (mySpaceWidth-mx)/dx; y1 = my + lambda *dy;
   if ( y1 >= 0. && y1 <= mySpaceHeight) {
     lmin = lambda < lmin ? lambda : lmin;
     lmax = lambda > lmax ? lambda : lmax;}

   lambda = -my/dy; x1 = mx + lambda *dx;
   if ( x1 >= 0. && x1 <= mySpaceWidth) {
     lmin = lambda < lmin ? lambda : lmin;
     lmax = lambda > lmax ? lambda : lmax;}

   lambda = (mySpaceHeight-my)/dy; x1 = mx + lambda *dx;
   if ( x1 >= 0. && x1 <= mySpaceWidth) {
     lmin = lambda < lmin ? lambda : lmin;
     lmax = lambda > lmax ? lambda : lmax;}

   if (lmin != ShortRealLast() && lmax != ShortRealFirst() &&
       lmin < lmax ) {
     x1 = mx + lmin *dx; y1 = my + lmin *dy;
     x2 = mx + lmax *dx; y2 = my + lmax *dy;
   } else status = Standard_False;
 }
 if( status ) {
   myDriver->DrawSegment(x1,y1,x2,y2);
   if( myMinMaxIsActivated ) {
     myMinX = Min(x1,myMinX); myMinX = Min(x2,myMinX);
     myMinY = Min(y1,myMinY); myMinY = Min(y2,myMinY);
     myMaxX = Max(x1,myMaxX); myMaxX = Max(x2,myMaxX);
     myMaxY = Max(y1,myMaxY); myMaxY = Max(y2,myMaxY);
   }
 }

}

void Graphic2d_Drawer::MapInfiniteLineFromTo (
                               const Standard_ShortReal x,
                               const Standard_ShortReal y,
                               const Standard_ShortReal dx,
                               const Standard_ShortReal dy) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

// Standard_ShortReal mx,my,x1,y1,x2,y2;
 Standard_ShortReal mx,my;
// Standard_Boolean status = Standard_True;

 mx = myXT + ((x-myXF)/mySF) *myST;
 my = myYT + ((y-myYF)/mySF) *myST;
 DrawInfiniteLine(mx,my,dx,dy);

}

void Graphic2d_Drawer::SetLineAttrib(const Standard_Integer ColorIndex,
                                     const Standard_Integer TypeIndex,
                                     const Standard_Integer WidthIndex) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

#ifdef CSR_PRO12324
        if(myOverride && !IsPlotterDriver())
#else
        if(myOverride)
#endif
                myDriver->SetLineAttrib(myOverrideColor,
                                        TypeIndex,WidthIndex);
        else if( ColorIndex > 0 )
                myDriver->SetLineAttrib(ColorIndex+myOffSet,
                                        TypeIndex,WidthIndex);
        else myDriver->SetLineAttrib(ColorIndex,TypeIndex,WidthIndex);

}

void Graphic2d_Drawer::SetMarkerAttrib(const Standard_Integer ColorIndex,
                                       const Standard_Integer WidthIndex,
                                       const Standard_Boolean FillMarker) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

#ifdef CSR_PRO12324
        if(myOverride && !IsPlotterDriver())
#else
        if(myOverride)
#endif  
                myDriver->SetMarkerAttrib(myOverrideColor,
                                        WidthIndex,FillMarker);
        else if( ColorIndex > 0 )
                myDriver->SetMarkerAttrib(ColorIndex+myOffSet,
                                        WidthIndex,FillMarker);
        else myDriver->SetMarkerAttrib(ColorIndex,
                                        WidthIndex,FillMarker);

}

void Graphic2d_Drawer::SetPolyAttrib(const Standard_Integer ColorIndex,
                                     const Standard_Integer TileIndex,
                                     const Standard_Boolean DrawEdge) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

#ifdef CSR_PRO12324
        if(myOverride && !IsPlotterDriver())
#else
        if(myOverride)
#endif 
                myDriver->SetPolyAttrib(myOverrideColor,TileIndex,DrawEdge);
        else if( ColorIndex > 0 )
                myDriver->SetPolyAttrib(ColorIndex+myOffSet,TileIndex,DrawEdge);
        else myDriver->SetPolyAttrib(ColorIndex,TileIndex,DrawEdge);

}

void Graphic2d_Drawer::SetHidingPolyAttrib(
                        const Standard_Integer HidingColorIndex,
                        const Standard_Integer FrameColorIndex,
                        const Standard_Integer FrameTypeIndex,
                        const Standard_Integer FrameWidthIndex) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");
#ifdef CSR_PRO12324
        if(myOverride && !IsPlotterDriver()) {
#else
        if(myOverride) {
#endif
          myDriver->SetLineAttrib(myOverrideColor,FrameTypeIndex,FrameWidthIndex);
          if( HidingColorIndex > 0 ) {
            myDriver->SetPolyAttrib(HidingColorIndex+myOffSet,0,Standard_True);
          } else {
            myDriver->SetPolyAttrib(HidingColorIndex,0,Standard_True);
          }
        } else {
          Standard_Boolean DrawEdge = Standard_False;
          if( (FrameColorIndex > 0) && (FrameColorIndex != HidingColorIndex) ) {
            DrawEdge = Standard_True;
            myDriver->SetLineAttrib(FrameColorIndex+myOffSet,FrameTypeIndex,FrameWidthIndex);
          }
          if( HidingColorIndex > 0 ) {
            myDriver->SetPolyAttrib(HidingColorIndex+myOffSet,0,DrawEdge);
          } else {
            myDriver->SetPolyAttrib(HidingColorIndex,0,DrawEdge);
          }
        }

}

void Graphic2d_Drawer::SetTextAttrib(const Standard_Integer ColorIndex,
                                     const Standard_Integer FontIndex,
                                     const Standard_ShortReal aSlant,
                                     const Standard_ShortReal aHScale,
                                     const Standard_ShortReal aWScale,
                                     const Standard_Boolean isUnderlined) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

#ifdef BUC60650
	Standard_ShortReal hscale = (aHScale > 0.) ? aHScale : 
					(aWScale > 0.) ? aWScale : Standard_ShortReal(1.0);
	Standard_ShortReal wscale = (aWScale > 0.) ? aWScale : hscale;
#else
	Standard_ShortReal hscale = aHScale;
	Standard_ShortReal wscale = aWScale;
#endif
	

#ifdef CSR_PRO12324
        if(myOverride && !IsPlotterDriver())
#else
        if(myOverride)
#endif
                myDriver->SetTextAttrib(myOverrideColor,FontIndex,
                                aSlant,hscale,wscale,isUnderlined);
        else if( ColorIndex > 0 )
                myDriver->SetTextAttrib(ColorIndex+myOffSet,FontIndex,
                                aSlant,hscale,wscale,isUnderlined);
        else myDriver->SetTextAttrib(ColorIndex,FontIndex,
                                aSlant,hscale,wscale,isUnderlined);

}

void Graphic2d_Drawer::SetHidingTextAttrib(const Standard_Integer ColorIndex,
                                     const Standard_Integer HidingColorIndex,
                                     const Standard_Integer FrameColorIndex,
                                     const Standard_Integer FrameWidthIndex,
                                     const Standard_Integer FontIndex,
                                     const Standard_ShortReal aSlant,
                                     const Standard_ShortReal aHScale,
                                     const Standard_ShortReal aWScale,
                                     const Standard_Boolean isUnderlined) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

#ifdef BUC60650
	Standard_ShortReal hscale = (aHScale > 0.) ? aHScale : 
					(aWScale > 0.) ? aWScale : Standard_ShortReal(1.0);
	Standard_ShortReal wscale = (aWScale > 0.) ? aWScale : hscale;
#else
	Standard_ShortReal hscale = aHScale;
	Standard_ShortReal wscale = aWScale;
#endif

#ifdef CSR_PRO12324
        if(myOverride && !IsPlotterDriver()) {
#else
        if(myOverride) {
#endif  
          if( ColorIndex >= 0 ) {
            myDriver->SetTextAttrib(myOverrideColor,FontIndex,
                                aSlant,hscale,wscale,isUnderlined);
          }
          myDriver->SetLineAttrib(myOverrideColor,0,FrameWidthIndex);
          if( HidingColorIndex > 0 ) {
            myDriver->SetPolyAttrib(HidingColorIndex+myOffSet,0,Standard_True);
          } else {
            myDriver->SetPolyAttrib(HidingColorIndex,0,Standard_True);
          }
        } else {
          Standard_Boolean DrawEdge = Standard_False;
          if( ColorIndex >= 0 ) {
            if( (ColorIndex > 0) && (ColorIndex != HidingColorIndex) ) {
                myDriver->SetTextAttrib(ColorIndex+myOffSet,FontIndex,
                                aSlant,hscale,wscale,isUnderlined);
            } else {
                myDriver->SetTextAttrib(0,FontIndex,
                                aSlant,hscale,wscale,isUnderlined);
            }
          }
          if( (FrameColorIndex > 0) && (FrameColorIndex != HidingColorIndex) ) {
            DrawEdge = Standard_True;
            myDriver->SetLineAttrib(FrameColorIndex+myOffSet,0,FrameWidthIndex);
          }
          if( HidingColorIndex > 0 ) {
            myDriver->SetPolyAttrib(HidingColorIndex+myOffSet,0,DrawEdge);
          } else {
            myDriver->SetPolyAttrib(HidingColorIndex,0,DrawEdge);
          }
        }
}

void Graphic2d_Drawer::SetFramedTextAttrib(const Standard_Integer ColorIndex,
                                     const Standard_Integer FrameColorIndex,
                                     const Standard_Integer FrameWidthIndex,
                                     const Standard_Integer FontIndex,
                                     const Standard_ShortReal aSlant,
                                     const Standard_ShortReal aHScale,
                                     const Standard_ShortReal aWScale,
                                     const Standard_Boolean isUnderlined) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

#ifdef BUC60650
	Standard_ShortReal hscale = (aHScale > 0.) ? aHScale : 
					(aWScale > 0.) ? aWScale : Standard_ShortReal(1.0);
	Standard_ShortReal wscale = (aWScale > 0.) ? aWScale : hscale;
#else
	Standard_ShortReal hscale = aHScale;
	Standard_ShortReal wscale = aWScale;
#endif

#ifdef CSR_PRO12324
        if(myOverride && !IsPlotterDriver()) {
#else
        if(myOverride) {
#endif  
          if( ColorIndex >= 0 ) {
            myDriver->SetTextAttrib(myOverrideColor,FontIndex,
                                aSlant,hscale,wscale,isUnderlined);
          }
          myDriver->SetLineAttrib(myOverrideColor,0,FrameWidthIndex);
        } else {
          if( ColorIndex >= 0 ) {
            if( ColorIndex > 0 ) {
                myDriver->SetTextAttrib(ColorIndex+myOffSet,FontIndex,
                                aSlant,hscale,wscale,isUnderlined);
            } else {
                myDriver->SetTextAttrib(ColorIndex,FontIndex,
                                aSlant,hscale,wscale,isUnderlined);
            }
          }
          if( FrameColorIndex > 0 ) {
              myDriver->SetLineAttrib(FrameColorIndex+myOffSet,0,FrameWidthIndex);
          } else if( ColorIndex > 0 ) {
              myDriver->SetLineAttrib(ColorIndex+myOffSet,0,FrameWidthIndex);
          } else {
              myDriver->SetLineAttrib(ColorIndex,0,FrameWidthIndex);
          }
        }
        myDriver->SetPolyAttrib(ColorIndex,-1,Standard_True);
}

Standard_Boolean Graphic2d_Drawer::IsKnownImage(const Handle(Standard_Transient)& anImageId) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        return myDriver->IsKnownImage(anImageId);

}

void Graphic2d_Drawer::ClearImage(const Handle(Standard_Transient)& anImageId) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        myDriver->ClearImage(anImageId);

}

void Graphic2d_Drawer::DrawImage(const Handle(Standard_Transient)& anImageId,
                                 const Standard_ShortReal aX,
                                 const Standard_ShortReal aY) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal X,Y;
        X = myXT + ((aX-myXF)/mySF) *myST;
        Y = myYT + ((aY-myYF)/mySF) *myST;
        myDriver->DrawImage(anImageId,X,Y);
}

void Graphic2d_Drawer::ClearImageFile(const Standard_CString aName) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        myDriver->ClearImageFile(aName);

}

void Graphic2d_Drawer::DrawImageFile (const Standard_CString aName,
                                 const Standard_ShortReal aX,
                                 const Standard_ShortReal aY,
                                 const Quantity_Factor aScale) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal X,Y;
        X = myXT + ((aX-myXF)/mySF) *myST;
        Y = myYT + ((aY-myYF)/mySF) *myST;
        myDriver->DrawImageFile (aName, X, Y, aScale);
        if( myMinMaxIsActivated ) {
          Standard_ShortReal width,height;
          if( GetImageSize(aName,width,height) ) {
            Standard_ShortReal x1 = Standard_ShortReal(X-width/2.);
            Standard_ShortReal y1 = Standard_ShortReal(Y-height/2.);
            Standard_ShortReal x2 = Standard_ShortReal(X+width/2.);
            Standard_ShortReal y2 = Standard_ShortReal(Y+height/2.);
            myMinX = Min(x1,myMinX);
            myMinY = Min(y1,myMinY);
            myMaxX = Max(x2,myMaxX);
            myMaxY = Max(y2,myMaxY);
          }
        }

}

void Graphic2d_Drawer::FillAndDrawImage(const Handle(Standard_Transient)& anImageId,
                                        const Standard_ShortReal aX,
                                        const Standard_ShortReal aY,
                                        const Standard_Integer aWidth,
                                        const Standard_Integer aHeight,
                                        const Standard_Address anArrayOfPixels) {

        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal X,Y;
        X = myXT + ((aX-myXF)/mySF) *myST;
        Y = myYT + ((aY-myYF)/mySF) *myST;
        myDriver->FillAndDrawImage
                (anImageId,X,Y,aWidth,aHeight,anArrayOfPixels);
        if( myMinMaxIsActivated ) {
          Standard_ShortReal width = Convert(aWidth);
          Standard_ShortReal height = Convert(aHeight);
          Standard_ShortReal x1 = Standard_ShortReal(X-width/2.);
          Standard_ShortReal y1 = Standard_ShortReal(Y-height/2.);
          Standard_ShortReal x2 = Standard_ShortReal(X+width/2.);
          Standard_ShortReal y2 = Standard_ShortReal(Y+height/2.);
          myMinX = Min(x1,myMinX);
          myMinY = Min(y1,myMinY);
          myMaxX = Max(x2,myMaxX);
          myMaxY = Max(y2,myMaxY);
        }

}

void Graphic2d_Drawer::FillAndDrawImage(const Handle(Standard_Transient)& anImageId,
                                        const Standard_ShortReal aX,
                                        const Standard_ShortReal aY,
                                        const Standard_Integer anIndexOfLine,
                                        const Standard_Integer aWidth,
                                        const Standard_Integer aHeight,
                                        const Standard_Address anArrayOfPixels) {
        if (! myDriverIsDefined)
                Graphic2d_DrawerDefinitionError::Raise ("No defined driver");

        Standard_ShortReal X,Y;
        X = myXT + ((aX-myXF)/mySF) *myST;
        Y = myYT + ((aY-myYF)/mySF) *myST;
        myDriver->FillAndDrawImage
                (anImageId,X,Y,anIndexOfLine,aWidth,aHeight,anArrayOfPixels);
        if( myMinMaxIsActivated ) {
          Standard_ShortReal width = Convert(aWidth);
          Standard_ShortReal height = Convert(aHeight);
          Standard_ShortReal x1 = Standard_ShortReal(X-width/2.);
          Standard_ShortReal y1 = Standard_ShortReal(Y-height/2.);
          Standard_ShortReal x2 = Standard_ShortReal(X+width/2.);
          Standard_ShortReal y2 = Standard_ShortReal(Y+height/2.);
          myMinX = Min(x1,myMinX);
          myMinY = Min(y1,myMinY);
          myMaxX = Max(x2,myMaxX);
          myMaxY = Max(y2,myMaxY);
        }

}

Standard_ShortReal Graphic2d_Drawer::Convert(const Standard_ShortReal aValue) const {

        return aValue/mySF * myST;

}

Standard_ShortReal Graphic2d_Drawer::Convert(const Standard_Integer aValue) const {
Standard_ShortReal dValue = 0.;

        if( myDriverIsDefined ) {
          dValue = Standard_ShortReal(myDriver->Convert(aValue));
        }
        return  dValue;
}

Standard_ShortReal Graphic2d_Drawer::Scale() const {

        return myST/mySF;

}

Standard_ShortReal Graphic2d_Drawer::ConvertMapToFrom(const Standard_ShortReal x) const {

        return x/myST * mySF;

}
