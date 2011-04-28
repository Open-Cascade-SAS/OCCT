// File:  Graphic2d_View.cxx
// Created: Mon Nov 22 18:50:16 1993
// Author:  Stephane CALLEGARI
//    <cal@sparc5>
// Modified     23/07/98 : DCB ; Remove WNT specific since WNT_DDriver
//                        inherits from PlotterDriver

#define xTRACE

#define CSR_PRO12324        //SYL_030798
//                      No plot if object not plottable                      
//
// Modified     17/12/1998 SYL pour S3604
//              Aspect_PlotterDriver devient PlotMgt_PlotterDriver

#define BUC60639        //GG_220300
//              the Remove() method must sets correctly
//              all GraphicObjects fields

#define G002		//GG_180400
//			-> Drawer become a TransientManager for
//			  open detection and dragging facilities

#define OCC154 // jfa 06/03/2002 // for correct erasing

#define OCC402  // SAV 14/11/01 Added PickByCircle().

#include <Standard_Type.hxx>
#include <Graphic2d_View.ixx>
#include <Aspect_WindowDriver.hxx>
#include <PlotMgt_PlotterDriver.hxx>
#include <Aspect_Window.hxx>
#include <Aspect_Units.hxx>
#include <Graphic2d_DetectionColorError.hxx>

Graphic2d_View::Graphic2d_View ():
        myGraphicObjects (),
        myGraphicAltitudes (),
        myBuffers (),
        myOverrideColorIsDef (Standard_False),
        myDeflection (0.001 METER),
        myDeflectionCoefficient (1./3000.),
        myTypeOfDeflection (Aspect_TOD_RELATIVE),
        myXCenter (0.),
        myYCenter (0.),
        mySize (1. METER), 
        myXPosition (0.),
        myYPosition (0.),
        myScale (1.),
        myTextPrecision (0.0015 METER) {
#ifdef G002
        myDrawer        = new Graphic2d_TransientManager(this);
#else
        myDrawer        = new Graphic2d_Drawer;
#endif
}

void Graphic2d_View::Add (const Handle(Graphic2d_Buffer)& aBuffer) { 
Standard_Integer i,l = myBuffers.Length ();

        for ( i=1; i<=l ; i++) {
          if (myBuffers.Value (i) == aBuffer) break;
        }

        if (i > l) myBuffers.Append (aBuffer);
}

Standard_Integer Graphic2d_View::Add (const Handle(Graphic2d_GraphicObject)& aGraphicObject,const Standard_Integer anAltitude) { 
Standard_Integer i,l = myGraphicAltitudes.Length();

        for( i=l ; i>0 ; --i ) {
          if( anAltitude >= myGraphicAltitudes.Value(i) ) break;
        }
        if( i < l ) {
          myGraphicObjects.InsertAfter (i, aGraphicObject);
          myGraphicAltitudes.InsertAfter (i, anAltitude);
          i++;
        } else {
          myGraphicObjects.Append (aGraphicObject);
          myGraphicAltitudes.Append (anAltitude);
          i = l+1;
        }
        return i;
}

Standard_Integer Graphic2d_View::Change (const Handle(Graphic2d_GraphicObject)& aGraphicObject,const Standard_Integer anAltitude) { 
Standard_Integer i,l = myGraphicObjects.Length ();

        for ( i=1; i<=l ; i++) {
          if (myGraphicObjects.Value (i) == aGraphicObject) break;
        }

        if ( i > l ) i = 0;
        else if( myGraphicAltitudes.Value (i) != anAltitude ) {
          myGraphicObjects.Remove (i);
          myGraphicAltitudes.Remove (i);
          i = 0;
        }

        if( i == 0 ) {
          i = Add(aGraphicObject,anAltitude);
        }

        return i;
}

void Graphic2d_View::Remove (const Handle(Graphic2d_Buffer)& aBuffer) { 
Standard_Integer i,l = myBuffers.Length ();

        for ( i=1; i<=l ; i++) {
          if (myBuffers.Value (i) == aBuffer) break;
        }

        if (i <= l) myBuffers.Remove (i);
}

Standard_Integer Graphic2d_View::Remove (const Handle(Graphic2d_GraphicObject)& aGraphicObject) { 
Standard_Integer i,l = myGraphicObjects.Length ();

        for ( i=1; i<=l ; i++) {
          if (myGraphicObjects.Value (i) == aGraphicObject) break;
        }

        if ( i > l ) i = 0;
        else {
          myGraphicObjects.Remove (i);
          myGraphicAltitudes.Remove (i);
        }

        return i;
}

void Graphic2d_View::Destroy () {
Standard_Integer Length = myGraphicObjects.Length();
Standard_Integer i;

  for (i=1; i<=Length; i++)
    myGraphicObjects.Value(i)->RemovePrimitives ();
  myGraphicObjects.Clear();
  myGraphicAltitudes.Clear();
}

void Graphic2d_View::SetDefaultOverrideColor (const Standard_Integer anIndex) {

        myOverrideColorIsDef    = Standard_True;
        myDefaultOverrideColor  = anIndex;

}

Standard_Integer Graphic2d_View::DefaultOverrideColor() const { 

        if (! myOverrideColorIsDef)
                Graphic2d_OverrideColorError::Raise
                        ("No defined override color index");

        return myDefaultOverrideColor;

}

Standard_Boolean Graphic2d_View::IsDefinedColor() const { 

        return myOverrideColorIsDef;

}

void Graphic2d_View::Update (const Handle(Aspect_Driver)& aDriver,
                             const Handle(Graphic2d_ViewMapping)& aViewMapping,
                             const Standard_Real aXPosition, 
                             const Standard_Real aYPosition, 
                             const Standard_Real aScale,
                             const Standard_Boolean ClearBefore) {

#ifdef TRACE
cout << "Graphic2d_View::Update (Aspect_Driver)\n" << flush;
#endif

Standard_Integer GOLength       = myGraphicObjects.Length();
Standard_Integer GBLength       = myBuffers.Length();
Standard_Integer i;
Standard_Real XCenter, YCenter, Size;

        aViewMapping->ViewMapping(XCenter, YCenter, Size);
        myXCenter       = Standard_ShortReal (XCenter);
        myYCenter       = Standard_ShortReal (YCenter);
        mySize  = Standard_ShortReal (Size);
        myXPosition     = Standard_ShortReal (aXPosition);
        myYPosition     = Standard_ShortReal (aYPosition);
        myScale = Standard_ShortReal (aScale);
        myZoom  = Standard_ShortReal (aViewMapping->Zoom());

        myDrawer->SetValues (XCenter, YCenter, Size, 
                                aXPosition, aYPosition, myScale, myZoom);
        myDrawer->SetDrawPrecision
                (myDeflection, myDeflectionCoefficient, myTypeOfDeflection);
        myDrawer->SetTextPrecision(myTextPrecision);
        myDrawer->SetDriver(aDriver);

        if( aDriver->IsKind(STANDARD_TYPE(Aspect_WindowDriver)) ) {
          Handle(Aspect_WindowDriver) theDriver =
                *(Handle(Aspect_WindowDriver) *) &aDriver;
          theDriver->BeginDraw(Standard_True);
          if( ClearBefore ) {
            theDriver->Window()->Clear();
          }
          for (i=1; i<=GOLength; i++)
                myGraphicObjects.Value(i)->Redraw (myDrawer);
          aDriver->EndDraw();
          for (i=1; i<=GBLength; i++) {
            Handle(Graphic2d_Buffer) theBuffer = myBuffers.Value(i);
            if( theBuffer->IsPosted(theDriver) ) {
              theBuffer->ReLoad (Standard_True);
            }
          }
        } else if( aDriver->IsKind(STANDARD_TYPE(PlotMgt_PlotterDriver)) ) {
          Handle(PlotMgt_PlotterDriver) theDriver =
                *(Handle(PlotMgt_PlotterDriver) *) &aDriver;
          theDriver->BeginDraw();
          for (i=1; i<=GOLength; i++)
#ifdef CSR_PRO12324     
            if( myGraphicObjects.Value(i)->IsPlottable())             
#endif
                myGraphicObjects.Value(i)->Redraw (myDrawer);
          aDriver->EndDraw();
        }
}

void Graphic2d_View::TinyUpdate (const Handle(Aspect_Driver)& aDriver,
                             const Handle(Graphic2d_ViewMapping)& aViewMapping,
                             const Standard_Real aXPosition, 
                             const Standard_Real aYPosition, 
                             const Standard_Real aScale,
                             const Standard_Boolean Reset) {

#ifdef TRACE
cout << "Graphic2d_View::TinyUpdate (Aspect_Driver)\n" << flush;
#endif

Standard_Integer i,Length       = myGraphicObjects.Length ();
Standard_Real XCenter, YCenter, Size;

        aViewMapping->ViewMapping(XCenter, YCenter, Size);
        myXCenter       = Standard_ShortReal (XCenter);
        myYCenter       = Standard_ShortReal (YCenter);
        mySize  = Standard_ShortReal (Size);
        myXPosition     = Standard_ShortReal (aXPosition);
        myYPosition     = Standard_ShortReal (aYPosition);
        myScale = Standard_ShortReal (aScale);
        myZoom  = Standard_ShortReal (aViewMapping->Zoom());

        myDrawer->SetValues (XCenter, YCenter, Size, 
                                aXPosition, aYPosition, myScale, myZoom);
        myDrawer->SetDrawPrecision
                (myDeflection, myDeflectionCoefficient, myTypeOfDeflection);
        myDrawer->SetTextPrecision(myTextPrecision);
        myDrawer->SetDriver(aDriver);

        if( aDriver->IsKind(STANDARD_TYPE(Aspect_WindowDriver)) ) {
          Handle(Aspect_WindowDriver) theDriver =
                *(Handle(Aspect_WindowDriver) *) &aDriver;
          theDriver->BeginDraw(Standard_True);
        } else if( aDriver->IsKind(STANDARD_TYPE(PlotMgt_PlotterDriver)) ) {
          Handle(PlotMgt_PlotterDriver) theDriver =
                *(Handle(PlotMgt_PlotterDriver) *) &aDriver;
          theDriver->BeginDraw();
        }
        for (i=1; i<=Length; i++)
#ifdef CSR_PRO12324     
          if( myGraphicObjects.Value(i)->IsPlottable())               
#endif
            myGraphicObjects.Value (i)->Draw (myDrawer,Reset);
        aDriver->EndDraw ();
}

void Graphic2d_View::Update (const Handle(Aspect_Driver)& aDriver, const Handle(Graphic2d_GraphicObject)& aGraphicObject, const Handle(Graphic2d_ViewMapping)& aViewMapping, const Standard_Real aXPosition, const Standard_Real aYPosition, const Standard_Real aScale) {

#ifdef TRACE
cout << "Graphic2d_View::Update (Aspect_Driver, Graphic2d_GraphicObject)\n" << flush;
#endif

      if( IsIn (aGraphicObject) ) {
        Standard_Integer i,Length = aGraphicObject->Length ();
        Handle(Graphic2d_Primitive) aPrimitive;
        Standard_Real XCenter, YCenter, Size;

        aViewMapping->ViewMapping(XCenter, YCenter, Size);
        myXCenter       = Standard_ShortReal (XCenter);
        myYCenter       = Standard_ShortReal (YCenter);
        mySize  = Standard_ShortReal (Size);
        myXPosition     = Standard_ShortReal (aXPosition);
        myYPosition     = Standard_ShortReal (aYPosition);
        myScale = Standard_ShortReal (aScale);
        myZoom  = Standard_ShortReal (aViewMapping->Zoom());

        myDrawer->SetValues (XCenter, YCenter, Size, 
                                aXPosition, aYPosition, myScale, myZoom);
        myDrawer->SetDrawPrecision
                (myDeflection, myDeflectionCoefficient, myTypeOfDeflection);
        myDrawer->SetTextPrecision(myTextPrecision);
        myDrawer->SetDriver(aDriver);

        if( aDriver->IsKind(STANDARD_TYPE(Aspect_WindowDriver)) ) {
          Handle(Aspect_WindowDriver) theDriver =
                *(Handle(Aspect_WindowDriver) *) &aDriver;
          theDriver->BeginDraw(Standard_True);
        } else if( aDriver->IsKind(STANDARD_TYPE(PlotMgt_PlotterDriver)) ) {
          Handle(PlotMgt_PlotterDriver) theDriver =
                *(Handle(PlotMgt_PlotterDriver) *) &aDriver;
          theDriver->BeginDraw();
        }
        for (i=1; i<=Length ; i++) {
          aPrimitive = aGraphicObject->Primitive(i);
#ifdef CSR_PRO12324  
          if( !aDriver->IsKind(STANDARD_TYPE(Aspect_WindowDriver))
             || aGraphicObject->IsPlottable() )     
#endif  
            aGraphicObject->Draw (myDrawer, aPrimitive);
        }
        aDriver->EndDraw ();
      }
}

void Graphic2d_View::Update (const Handle(Aspect_Driver)& aDriver, const Handle(Graphic2d_Primitive)& aPrimitive, const Handle(Graphic2d_ViewMapping)& aViewMapping, const Standard_Real aXPosition, const Standard_Real aYPosition, const Standard_Real aScale) {

#ifdef TRACE
cout << "Graphic2d_View::Update (Aspect_Driver, Graphic2d_Primitive)\n" << flush;
#endif

Standard_Integer i,Length       = myGraphicObjects.Length ();
Standard_Boolean found  = Standard_False;
Standard_Real XCenter, YCenter, Size;

        aViewMapping->ViewMapping(XCenter, YCenter, Size);
        myXCenter       = Standard_ShortReal (XCenter);
        myYCenter       = Standard_ShortReal (YCenter);
        mySize  = Standard_ShortReal (Size);
        myXPosition     = Standard_ShortReal (aXPosition);
        myYPosition     = Standard_ShortReal (aYPosition);
        myScale = Standard_ShortReal (aScale);
        myZoom  = Standard_ShortReal (aViewMapping->Zoom());

        myDrawer->SetValues (XCenter, YCenter, Size, 
                                aXPosition, aYPosition, myScale, myZoom);
        myDrawer->SetDrawPrecision
                (myDeflection, myDeflectionCoefficient, myTypeOfDeflection);
        myDrawer->SetTextPrecision(myTextPrecision);
        myDrawer->SetDriver(aDriver);

        if( aDriver->IsKind(STANDARD_TYPE(Aspect_WindowDriver)) ) {
          Handle(Aspect_WindowDriver) theDriver =
                *(Handle(Aspect_WindowDriver) *) &aDriver;
          theDriver->BeginDraw(Standard_True);
        } else if( aDriver->IsKind(STANDARD_TYPE(PlotMgt_PlotterDriver)) ) {
          Handle(PlotMgt_PlotterDriver) theDriver =
                *(Handle(PlotMgt_PlotterDriver) *) &aDriver;
          theDriver->BeginDraw();
        }
        for (i=1; i<=Length && !found; i++) {
                found   = myGraphicObjects.Value (i)->IsIn (aPrimitive);
                if (found)
#ifdef CSR_PRO12324  
                  if( !aDriver->IsKind(STANDARD_TYPE(Aspect_WindowDriver)) 
                     || myGraphicObjects.Value (i)->IsPlottable() ) 
#endif  
                    myGraphicObjects.Value (i)->Draw (myDrawer, aPrimitive);
        }
        aDriver->EndDraw ();

}

Handle(Graphic2d_DisplayList) Graphic2d_View::DisplayList () const {
Standard_Integer Index;
Standard_Integer Length = myGraphicObjects.Length ();
Handle(Graphic2d_DisplayList) DL = new Graphic2d_DisplayList;

         for (Index=1; Index<=Length; Index++)
             DL->Append(myGraphicObjects.Value(Index));
         return DL;
}

Standard_Boolean Graphic2d_View::IsIn (const Handle(Graphic2d_GraphicObject)& aGraphicObject) const {
Standard_Integer Index = 0;
Standard_Integer Length = myGraphicObjects.Length ();

        for (Standard_Integer i=1; i<=Length && Index==0; i++)
                if (myGraphicObjects.Value (i) == aGraphicObject) Index = i;

        return (Index != 0);
}

void Graphic2d_View::MinMax (Quantity_Length& Minx, Quantity_Length& Maxx, Quantity_Length& Miny, Quantity_Length& Maxy) const {

Standard_Integer i,Length = myGraphicObjects.Length ();
Quantity_Length x, y, X, Y;
Standard_Real RL, RF;

        RL = Minx = Miny = ShortRealLast ();
        RF = Maxx = Maxy  = ShortRealFirst ();

        for (i=1; i<=Length; i++) {
                myGraphicObjects.Value (i)->MinMax (x, X, y, Y);
                // GraphicObject can be empty
                if (x > RF) Minx = (Minx < x ? Minx : x); 
                if (X < RL) Maxx = (Maxx > X ? Maxx : X);
                if (y > RF) Miny = (Miny < y ? Miny : y);
                if (Y < RL) Maxy = (Maxy > Y ? Maxy : Y);
        }

        // Attention, it is possible that :
        // Minx = Miny = ShortRealLast (); and
        // Maxx = Maxy = ShortRealFirst();
        if (Minx > Maxx) { Minx = RealFirst (); Maxx = RealLast (); }
        if (Miny > Maxy) { Miny = RealFirst (); Maxy = RealLast (); }
}

void Graphic2d_View::MinMax (const Handle(Graphic2d_DisplayList)& aList, Quantity_Length& Minx, Quantity_Length& Maxx, Quantity_Length& Miny, Quantity_Length& Maxy) const {

Standard_Integer i,Length = aList->Length ();
Quantity_Length x, y, X, Y;
Standard_Real RL, RF;

        RL = Minx = Miny = ShortRealLast ();
        RF = Maxx = Maxy = ShortRealFirst ();

        for (i=1; i<=Length; i++) {
                if (IsIn (aList->Value(i))) {
                        (aList->Value(i))->MinMax (x, X, y, Y);
                        //  GraphicObject can be empty
                        if (x > RF) Minx = (Minx < x ? Minx : x); 
                        if (X < RL) Maxx = (Maxx > X ? Maxx : X);
                        if (y > RF) Miny = (Miny < y ? Miny : y);
                        if (Y < RL) Maxy = (Maxy > Y ? Maxy : Y);
                }
        }

        // Attention, it is possible that :
        // Minx = Miny = ShortRealLast (); and
        // Maxx = Maxy = ShortRealFirst();
        if (Minx > Maxx) { Minx = RealFirst (); Maxx = RealLast (); }
        if (Miny > Maxy) { Miny = RealFirst (); Maxy = RealLast (); }
}

void Graphic2d_View::MarkerMinMax (Quantity_Length& Minx, Quantity_Length& Maxx, Quantity_Length& Miny, Quantity_Length& Maxy) const {

Standard_Integer i,Length = myGraphicObjects.Length ();
Quantity_Length x, y, X, Y;
Standard_Real RL, RF;

        RL = Minx = Miny = ShortRealLast ();
        RF = Maxx = Maxy = ShortRealFirst ();

        for (i=1; i<=Length; i++) {
                myGraphicObjects.Value (i)->MarkerMinMax (x, X, y, Y);
                // GraphicObject can be empty 
                if (x > RF) Minx = (Minx < x ? Minx : x); 
                if (X < RL) Maxx = (Maxx > X ? Maxx : X);
                if (y > RF) Miny = (Miny < y ? Miny : y);
                if (Y < RL) Maxy = (Maxy > Y ? Maxy : Y);
        }

        // Attention, it is possible that :
        // Minx = Miny = ShortRealLast (); and
        // Maxx = Maxy = ShortRealFirst();
        if (Minx > Maxx) { Minx = RealFirst (); Maxx = RealLast (); }
        if (Miny > Maxy) { Miny = RealFirst (); Maxy = RealLast (); }
}

void Graphic2d_View::MarkerMinMax (const Handle(Graphic2d_DisplayList)& aList, Quantity_Length& Minx, Quantity_Length& Maxx, Quantity_Length& Miny, Quantity_Length& Maxy) const {

Standard_Integer i,Length = aList->Length ();
Quantity_Length x, y, X, Y;
Standard_Real RL, RF;

        RL = Minx = Miny = ShortRealLast ();
        RF = Maxx = Maxy = ShortRealFirst ();

        for (i=1; i<=Length; i++) {
                if (IsIn (aList->Value(i))) {
                        (aList->Value(i))->MarkerMinMax (x, X, y, Y);
                        // GraphicObject can be empty
                        if (x > RF) Minx = (Minx < x ? Minx : x); 
                        if (X < RL) Maxx = (Maxx > X ? Maxx : X);
                        if (y > RF) Miny = (Miny < y ? Miny : y);
                        if (Y < RL) Maxy = (Maxy > Y ? Maxy : Y);
                }
        }

        // Attention, it is possible that :
        // Minx = Miny = ShortRealLast (); and
        // Maxx = Maxy = ShortRealFirst();
        if (Minx > Maxx) { Minx = RealFirst (); Maxx = RealLast (); }
        if (Miny > Maxy) { Miny = RealFirst (); Maxy = RealLast (); }
}

Standard_Integer Graphic2d_View::Position(const Handle(Graphic2d_GraphicObject)& aGraphicObject) const {
Standard_Integer i,l = myGraphicObjects.Length ();

        for ( i=1; i<=l ; i++) {
          if (myGraphicObjects.Value (i) == aGraphicObject) break;
        }

        return (i > l) ? 0 : i;
}

Standard_Integer Graphic2d_View::Altitude(const Handle(Graphic2d_GraphicObject)& aGraphicObject) const {
Standard_Integer i,l = myGraphicObjects.Length ();

        for ( i=1; i<=l ; i++) {
          if (myGraphicObjects.Value (i) == aGraphicObject) break;
        }

        return (i > l) ? -1 : myGraphicAltitudes.Value(i);
}

Handle(Graphic2d_DisplayList) Graphic2d_View::Pick
       (const Handle(Graphic2d_ViewMapping)& aViewMapping,
        const Standard_Real X,
        const Standard_Real Y,
        const Standard_Real aPrecision,
        const Standard_Real aXPosition, 
        const Standard_Real aYPosition, 
        const Standard_Real aScale) {

#ifdef G002
	if( myPickList.IsNull() )
		myPickList = new Graphic2d_DisplayList;
	else myPickList->Clear();
#else
Handle(Graphic2d_DisplayList) DL = new Graphic2d_DisplayList;
#endif
Standard_Integer Length = myGraphicObjects.Length ();
Standard_Integer Index;
Standard_Real XCenter,YCenter,Size,Zoom = aViewMapping->Zoom();

         aViewMapping->ViewMapping(XCenter,YCenter,Size);

         myDrawer->SetDrawPrecision(myDeflection*Zoom,
                                    myDeflectionCoefficient,
                                    myTypeOfDeflection);         
         myDrawer->SetValues (XCenter,YCenter,Size,
                                        aXPosition,aYPosition,aScale,Zoom);
         for (Index=Length; Index>0; --Index) {
           if (myGraphicObjects.Value(Index)->Pick(X,Y,aPrecision,myDrawer))
#ifdef G002
             myPickList->Append(myGraphicObjects.Value(Index));
#else
             DL->Append(myGraphicObjects.Value(Index));
#endif
         }

#ifdef G002
	 return myPickList;
#else
         return DL;
#endif
}

Handle(Graphic2d_DisplayList) Graphic2d_View
::PickByCircle( const Handle(Graphic2d_ViewMapping)& aViewMapping,
	        const Standard_Real X,
	        const Standard_Real Y,
	        const Standard_Real Radius,
	        const Standard_Real aXPosition, 
	        const Standard_Real aYPosition, 
	        const Standard_Real aScale)
{
  if( myPickList.IsNull() )
    myPickList = new Graphic2d_DisplayList;
  else myPickList->Clear();

#ifdef OCC402
  Standard_Integer Length = myGraphicObjects.Length();
  Standard_Integer Index;
  Standard_Real XCenter,YCenter,Size,Zoom = aViewMapping->Zoom();

  aViewMapping->ViewMapping( XCenter, YCenter, Size );

  myDrawer->SetDrawPrecision( myDeflection*Zoom,
			      myDeflectionCoefficient,
			      myTypeOfDeflection );         
  myDrawer->SetValues( XCenter, YCenter, Size,
		       aXPosition, aYPosition, aScale, Zoom );
  for ( Index = Length; Index > 0; --Index )
    if ( myGraphicObjects.Value( Index )->PickByCircle( X, Y, Radius, myDrawer ) )
      myPickList->Append(myGraphicObjects.Value(Index));
#endif
  return myPickList;
}


Handle(Graphic2d_DisplayList) Graphic2d_View::Pick
                  ( const Handle(Graphic2d_ViewMapping)& aViewMapping,
                    const Standard_Real Xmin,
                    const Standard_Real Ymin,
                    const Standard_Real Xmax,
                    const Standard_Real Ymax,
                    const Standard_Real aXPosition, 
                    const Standard_Real aYPosition, 
                    const Standard_Real aScale,
                    const Graphic2d_PickMode aPickMode ) {

#ifdef G002
	if ( myPickList.IsNull() )
		myPickList = new Graphic2d_DisplayList;
	else myPickList->Clear();
#else
    Handle(Graphic2d_DisplayList) DL = new Graphic2d_DisplayList;
#endif

    Standard_Integer Length = myGraphicObjects.Length ();
    Standard_Integer Index;
    Standard_Real XCenter, YCenter, Size, Zoom = aViewMapping->Zoom();

    aViewMapping->ViewMapping( XCenter, YCenter, Size );

    myDrawer->SetDrawPrecision( myDeflection * Zoom,
                                myDeflectionCoefficient,
                                myTypeOfDeflection );
    myDrawer->SetValues( XCenter, YCenter, Size,
                         aXPosition, aYPosition, aScale, Zoom );
    for ( Index = Length; Index > 0; --Index ) {
       if ( myGraphicObjects.Value(Index)->Pick( Xmin, Ymin, Xmax, Ymax, 
                                   myDrawer, aPickMode ) )
#ifdef G002
          myPickList->Append(myGraphicObjects.Value(Index));
#else
          DL->Append(myGraphicObjects.Value(Index));
#endif
    }

#ifdef G002
	 return myPickList;
#else
     return DL;
#endif
}

void Graphic2d_View::Erase() {
Standard_Integer Length = myGraphicObjects.Length ();
Standard_Integer Index;

  for (Index=1; Index<=Length; Index++)
    //myGraphicObjects.Value(Index)->Erase ();
    // CAL le 17/11/97.
    // 1 is set as it is required to remove N times the first from the list
    // Acytually this list is updated by method below
    // called by Graphic2d_GraphicObject::Erase.
#ifdef OCC154
    myGraphicObjects.Value(Index)->Erase();
#else
    myGraphicObjects.Value(1)->Erase ();
#endif
}

void Graphic2d_View::Erase (const Handle(Graphic2d_GraphicObject)& aGraphicObject) {
Standard_Integer Index = 0;
Standard_Integer Length = myGraphicObjects.Length ();

        for (Standard_Integer i=1; i<=Length && Index==0; i++)
                if (myGraphicObjects.Value (i) == aGraphicObject) Index = i;

        if (Index != 0) {
          myGraphicObjects.Remove (Index);
          myGraphicAltitudes.Remove (Index);
        }
}

void Graphic2d_View::Remove () {
#ifdef BUC60639
  Standard_Integer Length = myGraphicObjects.Length ();
  for (Standard_Integer i=1; i<=Length ; i++)
                myGraphicObjects.Value (1)->Remove();
#else
  myGraphicObjects.Clear();
  myGraphicAltitudes.Clear();
#endif
}

void Graphic2d_View::SetTypeOfDeflection(const Aspect_TypeOfDeflection aType) {
  myTypeOfDeflection = aType;
}

Aspect_TypeOfDeflection Graphic2d_View::TypeOfDeflection() const {
  return myTypeOfDeflection;
}


void Graphic2d_View::SetDeflection (const Quantity_Length aDeflection) {

  myDeflection = aDeflection;

}

Quantity_Length Graphic2d_View::Deflection() const { return myDeflection; }

void Graphic2d_View::SetDeflectionCoefficient(const Standard_Real aCoefficient) {
  myDeflectionCoefficient = aCoefficient;
}

Standard_Real Graphic2d_View::DeflectionCoefficient () const {
  return myDeflectionCoefficient;
}

void Graphic2d_View::SetTextPrecision (const Standard_Real aPrecision) {

  myTextPrecision = Standard_ShortReal(aPrecision);

}

Standard_Real Graphic2d_View::TextPrecision() const { return myTextPrecision;
}

void Graphic2d_View::Convert(const Handle(Graphic2d_ViewMapping)& aViewMapping,
                             const Standard_Real aDrawPositionX,
                             const Standard_Real aDrawPositionY,
                             const Standard_Real aXPosition,
                             const Standard_Real aYPosition,
                             const Standard_Real aScale,
                             Standard_Real& aX,
                             Standard_Real& aY) const {

          Standard_Real XCenter,YCenter,Size,Zoom = aViewMapping->Zoom();
          Standard_ShortReal xdraw = Standard_ShortReal(aDrawPositionX);
          Standard_ShortReal ydraw = Standard_ShortReal(aDrawPositionY);
          Standard_ShortReal xpos,ypos ;
          aViewMapping->ViewMapping(XCenter,YCenter,Size);
          myDrawer->SetValues (XCenter,YCenter,Size,
                                        aXPosition,aYPosition,aScale,Zoom);
          myDrawer->UnMapFromTo(xdraw,ydraw,xpos,ypos);
          aX = xpos ;
          aY = ypos ;

}

Standard_Real Graphic2d_View::Convert( const Handle(Graphic2d_ViewMapping)& aViewMapping,
                                      const Standard_Real aScalar,
                                      const Standard_Real aScale) const {

          Standard_Real XCenter,YCenter,Size;
          aViewMapping->ViewMapping(XCenter,YCenter,Size);
          return (aScalar / aScale) * Size;


}

Handle(Graphic2d_Drawer) Graphic2d_View::Drawer() const {

        return myDrawer ;
}
