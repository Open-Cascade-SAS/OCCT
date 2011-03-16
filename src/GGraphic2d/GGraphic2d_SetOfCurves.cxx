#include <GGraphic2d_SetOfCurves.ixx>
#include <BndLib_Add2dCurve.hxx>
#include <Bnd_Box2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <GCPnts_UniformDeflection.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>

#include <gp_Pnt.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Standard_ErrorHandler.hxx>

#define G002
#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0

GGraphic2d_SetOfCurves::GGraphic2d_SetOfCurves (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject)

  : Graphic2d_Line (aGraphicObject) {

}

void GGraphic2d_SetOfCurves::Add (const Handle(Geom2d_Curve) &aCurve)
{
    	Bnd_Box2d Box;
    	BndLib_Add2dCurve::Add(Geom2dAdaptor_Curve(aCurve),0.,Box);
    	Standard_Real Xmin, Ymin, Xmax, Ymax;
    	Box.Get( Xmin, Ymin, Xmax, Ymax);
    	Standard_ShortReal minX = Standard_ShortReal(Xmin);
    	Standard_ShortReal minY = Standard_ShortReal(Ymin);
    	Standard_ShortReal maxX = Standard_ShortReal(Xmax);
    	Standard_ShortReal maxY = Standard_ShortReal(Ymax);
	myMinX	= Min(myMinX,minX);
	myMinY	= Min(myMinY,minY);
	myMaxX	= Max(myMaxX,maxX);
	myMaxY	= Max(myMaxY,maxY);

	myCurves.Append(aCurve);
}

Standard_Integer GGraphic2d_SetOfCurves::Length () const {
   return myCurves.Length();
}

Handle(Geom2d_Curve) GGraphic2d_SetOfCurves::Values(const Standard_Integer aRank) const {

	if( aRank < 1 || aRank > myCurves.Length() ) 
                Standard_OutOfRange::Raise
			("the curve rank is out of bounds in the set");

	return myCurves.Value(aRank);
}

void GGraphic2d_SetOfCurves::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {
//Standard_Integer i,length = myCurves.Length(),bufferize = 1;
 Standard_Integer length = myCurves.Length(), bufferize = 1;
 Standard_Boolean IsIn = Standard_False;
 Standard_ShortReal minX,minY,maxX,maxY;
 Standard_Real Xmin,Ymin,Xmax,Ymax;

 if( length <= 0 ) return ;

 gp_GTrsf2d aTrsf = myGOPtr->Transform ();
 
  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    MinMax(minX,maxX,minY,maxY);
    IsIn = aDrawer->IsIn (minX,maxX,minY,maxY);
  }

  if ( IsIn ) {
    Standard_ShortReal Def;
    Standard_ShortReal Coeff;
    Aspect_TypeOfDeflection Type;
    Standard_Boolean Controll = Standard_True;
    Handle(Geom2d_Curve) aCurve;
    Standard_Integer i,typeindex = TypeIndex();

    aDrawer->DrawPrecision(Def,Coeff,Type);
    DrawLineAttrib(aDrawer);
    for( i=1 ; i<=length ; i++ ) {
      aCurve = myCurves.Value(i);

      if (Type == Aspect_TOD_RELATIVE) {
        Bnd_Box2d Box;
        BndLib_Add2dCurve::Add(Geom2dAdaptor_Curve(aCurve),0.,Box);
        Box.Get( Xmin,Ymin,Xmax,Ymax );
      	Def = Standard_ShortReal( (Abs(Xmax-Xmin) + Abs(Ymax-Ymin) ) * Coeff);
      }

      Geom2dAdaptor_Curve GAC(aCurve);
      GCPnts_UniformDeflection UD(GAC,Def,Controll);
      if(UD.IsDone()) {
        Standard_Integer j,nbp = UD.NbPoints();
        gp_Pnt p2;
        Standard_Real x,y,z;
    	Standard_ShortReal x1=0,y1=0,x2,y2;
        for (j=1; j<=nbp; j++) {
          p2 = UD.Value(j);
          p2.Coord(x,y,z);
          if (myGOPtr->IsTransformed ()) {
            aTrsf.Transforms(x, y);
          }
	  if( typeindex > 0 ) {
	    if( j > 1 ) {
              if( j < nbp ) bufferize = 0;
	      else          bufferize = -1;
	    } else {
	      bufferize = nbp;
	    }
          aDrawer->MapPolylineFromTo( Standard_ShortReal(x), Standard_ShortReal(y), bufferize);
	  } else {
	    if( j > 1 ) {
	      if( (i == length) && (j == nbp) ) bufferize = -1;
	      x2 = Standard_ShortReal(x); y2 = Standard_ShortReal(y);
          aDrawer->MapSegmentFromTo(x1,y1,x2,y2,bufferize);
	      x1 = x2; y1 = y2;
	      bufferize = 0; 
	    } else {
	      x1 = Standard_ShortReal(x); y1 = Standard_ShortReal(y);
	    }
	  }
        }
      }
    }
  }
}

#ifdef G002

void GGraphic2d_SetOfCurves::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                          const Standard_Integer anIndex ) {

 Standard_Integer length = myCurves.Length(), bufferize = 1;
 Standard_Boolean IsIn = Standard_False;
 Standard_ShortReal minX,minY,maxX,maxY;
 Standard_Real Xmin,Ymin,Xmax,Ymax;

 if ( length <= 0 ) return ;

 gp_GTrsf2d aTrsf = myGOPtr->Transform ();
 
 if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX,myMaxX,myMinY,myMaxY );
 else {
    MinMax(minX,maxX,minY,maxY);
    IsIn = aDrawer->IsIn (minX,maxX,minY,maxY);
  }

  if ( IsIn ) {

   if ( anIndex > 0 && anIndex <= length ) { 

    Standard_ShortReal Def, Coeff;
    Aspect_TypeOfDeflection Type;
    Standard_Boolean Controll = Standard_True;
    Handle(Geom2d_Curve) aCurve;
    Standard_Integer typeindex = TypeIndex();

    aDrawer->DrawPrecision( Def, Coeff, Type );
    DrawLineAttrib( aDrawer );
    aCurve = myCurves.Value( anIndex );

    if ( Type == Aspect_TOD_RELATIVE ) {
      Bnd_Box2d Box;
      BndLib_Add2dCurve::Add(Geom2dAdaptor_Curve(aCurve),0.,Box);
      Box.Get( Xmin,Ymin,Xmax,Ymax  );
      Def = Standard_ShortReal( (Abs(Xmax-Xmin) + Abs(Ymax-Ymin) ) * Coeff);
    }

    Geom2dAdaptor_Curve GAC(aCurve);
    GCPnts_UniformDeflection UD(GAC,Def,Controll);
    if ( UD.IsDone() ) {
      Standard_Integer j,nbp = UD.NbPoints();
      gp_Pnt p2;
      Standard_Real x,y,z;
      Standard_ShortReal x1=0,y1=0,x2,y2;
      for ( j = 1; j <= nbp; j++ ) {
          p2 = UD.Value( j );
          p2.Coord( x, y, z );
          if ( myGOPtr->IsTransformed() ) {
            aTrsf.Transforms (x, y);
          }
	      if ( typeindex > 0 ) {
	        if ( j > 1 ) {
              bufferize = ( j < nbp ) ? 0 : -1;
            } else {
	           bufferize = nbp;
            }
            aDrawer->MapPolylineFromTo( Standard_ShortReal(x), Standard_ShortReal(y), bufferize );
          } else {
	        if ( j > 1 ) {
	         if ( ( anIndex == length ) && (j == nbp) ) 
                 bufferize = -1;
	         x2 = Standard_ShortReal(x); y2 = Standard_ShortReal(y);
             aDrawer->MapSegmentFromTo( x1, y1, x2, y2, bufferize );
	         x1 = x2; y1 = y2;
	         bufferize = 0; 
            } else {
	         x1 = Standard_ShortReal(x); y1 = Standard_ShortReal(y);
            }
          }
      } // end for
    }
  }
 
 } // end if IsIn
}

void GGraphic2d_SetOfCurves::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                                         const Standard_Integer anIndex ) {

 Standard_Integer length = myCurves.Length();
 Standard_Boolean IsIn = Standard_False;
 Standard_ShortReal minX,minY,maxX,maxY;

 if ( length <= 0 ) return ;

 if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX,myMaxX,myMinY,myMaxY );
 else {
    MinMax(minX,maxX,minY,maxY);
    IsIn = aDrawer->IsIn (minX,maxX,minY,maxY);
  }

  if ( IsIn ) {

   if ( anIndex > 0 && anIndex <= 2*length ) { 
      
      DrawMarkerAttrib (aDrawer);
      Handle(Geom2d_Curve) aCurve; 
      Standard_Real theParam; 
      if ( anIndex <= length ) { 
       aCurve = myCurves.Value( anIndex );
       theParam = aCurve->FirstParameter();
      } else {
       aCurve = myCurves.Value( anIndex - length);
       theParam = aCurve->LastParameter();
      }
      gp_Pnt2d thePnt = aCurve->Value(theParam);
      Standard_ShortReal a = Standard_ShortReal(thePnt.X()),
                         b = Standard_ShortReal(thePnt.Y());
      if ( myGOPtr->IsTransformed() ) {
        gp_GTrsf2d aTrsf = myGOPtr->Transform ();
        Standard_Real a1, b1;
        a1 = Standard_Real(a);
        b1 = Standard_Real(b);
        aTrsf.Transforms( a1, b1 );
        a = Standard_ShortReal( a1 );
        b = Standard_ShortReal( b1 );
      } 
      aDrawer->MapMarkerFromTo( VERTEXMARKER, a, b,
			DEFAULTMARKERSIZE,DEFAULTMARKERSIZE,0.0);
   } 
  } // end if IsIn
}

#endif

Standard_Boolean GGraphic2d_SetOfCurves::Pick( 
                      const Standard_ShortReal X,
		      const Standard_ShortReal Y,
		      const Standard_ShortReal aPrecision,
		      const Handle(Graphic2d_Drawer)& /*aDrawer*/) {

 Standard_Integer i,length = myCurves.Length();
 //Standard_ShortReal SRX = X, SRY = Y;
 static Standard_ShortReal SRX , SRY ;
 SRX = X;
 SRY = Y;
 //Standard_Boolean isPicked = Standard_False;
  
 if ( (length > 0) && IsInMinMax (X, Y, aPrecision)) {
  if (myGOPtr->IsTransformed ()) {
    gp_GTrsf2d aTrsf = (myGOPtr->Transform()).Inverted();
    Standard_Real RX = Standard_Real( SRX ), RY = Standard_Real( SRY );
    aTrsf.Transforms( RX, RY );
    SRX = Standard_ShortReal( RX ); SRY = Standard_ShortReal( RY );
   }
   try {
     OCC_CATCH_SIGNALS
      Handle(Geom2d_Curve) aCurve;
      for ( i = 1 ; i <= length; i++ ) {
        aCurve = myCurves.Value(i);
        Standard_Real FParam = aCurve->FirstParameter(), 
                      LParam = aCurve->LastParameter();
        gp_Pnt2d FP = aCurve->Value(FParam), 
                 LP = aCurve->Value(LParam);
        Geom2dAPI_ProjectPointOnCurve PC(gp_Pnt2d(SRX, SRY),
                                     aCurve,
                                     FParam,
                                     LParam );
        if ( PC.NbPoints() > 0 ) {
          gp_Pnt2d P = PC.NearestPoint();

#ifdef G002

          if ( ( Abs( SRX - P.X() ) +  Abs( SRY - P.Y() ) ) < aPrecision ) {
            SetPickedIndex( i );
            return Standard_True;
          } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, 
              Standard_ShortReal(FP.X()), Standard_ShortReal(FP.Y()), aPrecision) ) {
            SetPickedIndex( -i );
            return Standard_True;
          } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, 
              Standard_ShortReal(LP.X()), Standard_ShortReal(LP.Y()), aPrecision) ) {
            SetPickedIndex( -i - length );
            return Standard_True;
          }
#else
          if ( ( Abs( SRX - P.X() ) +  Abs( SRY - P.Y() ) ) < aPrecision ) {
	        myPickedIndex = i;
            return Standard_True;
          }
#endif
        } // end if NbPoints() > 0
      } // end for
    } catch( Standard_Failure )  {
      return Standard_False;
    }
  }

  return Standard_False;
 
}

void GGraphic2d_SetOfCurves::Save(Aspect_FStream& aFStream) const
{
	int i,lngth = Length();
	if ( ! lngth ) return;

	*aFStream << "GGraphic2d_SetOfCurves" << endl;
	*aFStream << lngth << endl;	


	Handle(Geom2d_Curve) myCurve;

	for ( i = 1; i <= lngth; i++ ) {

	myCurve = myCurves.Value( i );

	if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Line)) ) {

	Handle(Geom2d_Line) theLine = Handle(Geom2d_Line)::DownCast( myCurve );
	gp_Dir2d tDir = theLine->Direction();
	gp_Pnt2d tPnt = theLine->Location();

	*aFStream << "Geom2d_Line" << endl;
	*aFStream << tDir.X() << ' ' << tDir.Y() << endl;
	*aFStream << tPnt.X() << ' ' << tPnt.Y() << endl;

	} else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Circle)) ) {

	Handle(Geom2d_Circle) theCirc = Handle(Geom2d_Circle)::DownCast( myCurve );
	gp_Circ2d tCirc  = theCirc->Circ2d();
	gp_Ax22d tAx22d = tCirc.Axis();
	gp_Pnt2d tLoc  = tAx22d.Location();
	gp_Dir2d tDirX = tAx22d.XDirection(),
	       tDirY = tAx22d.YDirection();

	Standard_Real tRad = tCirc.Radius();

	*aFStream << "Geom2d_Circle" << endl;
	*aFStream << tLoc.X() << ' ' << tLoc.Y() << endl;
	*aFStream << tDirX.X() << ' ' << tDirX.Y() << endl;
	*aFStream << tDirY.X() << ' ' << tDirY.Y() << endl;
	*aFStream << tRad << endl;

	} else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Parabola)) ) {

	Handle(Geom2d_Parabola) theParab = Handle(Geom2d_Parabola)::DownCast( myCurve );
	gp_Parab2d tParab = theParab->Parab2d();
	gp_Ax22d tAx22d = tParab.Axis();
	gp_Pnt2d tLoc  = tAx22d.Location();
	gp_Dir2d tDirX = tAx22d.XDirection(),
		tDirY = tAx22d.YDirection();

	Standard_Real tFocal = theParab->Focal();

	*aFStream << "Geom2d_Parabola" << endl;
	*aFStream << tLoc.X() << ' ' << tLoc.Y() << endl;
	*aFStream << tDirX.X() << ' ' << tDirX.Y() << endl;
	*aFStream << tDirY.X() << ' ' << tDirY.Y() << endl;
	*aFStream << tFocal << endl;

	} else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Ellipse)) ) {
	Handle(Geom2d_Ellipse) theEllipse = Handle(Geom2d_Ellipse)::DownCast(myCurve);
	gp_Elips2d tElips = theEllipse->Elips2d();
	Standard_Real tMin = tElips.MinorRadius(),
		     tMaj = tElips.MajorRadius();
	gp_Ax22d tAx22d = tElips.Axis();
	gp_Pnt2d tLoc  = tAx22d.Location();
	gp_Dir2d tDirX = tAx22d.XDirection(),
		tDirY = tAx22d.YDirection();

	*aFStream << "Geom2d_Ellipse" << endl;

	*aFStream << tMin << ' ' << tMaj << endl;
	*aFStream << tLoc.X() << ' ' << tLoc.Y() << endl;
	*aFStream << tDirX.X() << ' ' << tDirX.Y() << endl;
	*aFStream << tDirY.X() << ' ' << tDirY.Y() << endl;

	} else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Hyperbola)) ) {

	Handle(Geom2d_Hyperbola) theHypr = Handle(Geom2d_Hyperbola)::DownCast(myCurve);
	gp_Hypr2d tHypr = theHypr->Hypr2d();
	Standard_Real tMin = tHypr.MinorRadius(),
		      tMaj = tHypr.MajorRadius();
	gp_Ax22d tAx22d = tHypr.Axis();
	gp_Pnt2d tLoc  = tAx22d.Location();
	gp_Dir2d tDirX = tAx22d.XDirection(),
		 tDirY = tAx22d.YDirection();

	*aFStream << "Geom2d_Hyperbola" << endl;

	*aFStream << tMin << ' ' << tMaj << endl;
	*aFStream << tLoc.X() << ' ' << tLoc.Y() << endl;
	*aFStream << tDirX.X() << ' ' << tDirX.Y() << endl;
	*aFStream << tDirY.X() << ' ' << tDirY.Y() << endl;

	} else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)) ) {

	Handle(Geom2d_BezierCurve) theBC = Handle(Geom2d_BezierCurve)::DownCast(myCurve);
	Standard_Integer nbPoles = theBC->NbPoles();
	TColgp_Array1OfPnt2d tpoles( 1, nbPoles );
	theBC->Poles( tpoles );
	*aFStream << "Geom2d_BezierCurve" << endl;

	for ( i = 1; i <= nbPoles; i++ ) 
	  *aFStream << tpoles(i).X() << ' ' << tpoles(i).Y() << endl;

	} else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) ) {
	Handle(Geom2d_BSplineCurve) theBSC = Handle(Geom2d_BSplineCurve)::DownCast(myCurve);

	} else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)) ) {
	Handle(Geom2d_OffsetCurve) theOC = Handle(Geom2d_OffsetCurve)::DownCast(myCurve);

	} else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)) ) {    
	Handle(Geom2d_TrimmedCurve) theTC = Handle(Geom2d_TrimmedCurve)::DownCast(myCurve);
	}

	} // end for 
	Graphic2d_Line::Save(aFStream);
}
