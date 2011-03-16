#define WTO0001         //GG_140596
//                      Calcul des min-max faux apres transformation.
// G002 TCL : Drawing of descriptor of BSpline and Bezier curves in display mode 1

#include <GGraphic2d_Curve.ixx>
#include <BndLib_Add2dCurve.hxx>
#include <Bnd_Box2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
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

#define MAXPOINTS 1023
#define VERTEXMARKER 4
#define DEFAULTMARKERSIZE 3.0

static  TShort_Array1OfShortReal Xpoint(1,MAXPOINTS);
static  TShort_Array1OfShortReal Ypoint(1,MAXPOINTS);

GGraphic2d_Curve::GGraphic2d_Curve
  (const Handle(Graphic2d_GraphicObject)& aGraphicObject,
   const Handle(Geom2d_Curve)& aCurve)
    : Graphic2d_Line(aGraphicObject) , myCurve(aCurve) {

    Bnd_Box2d Box;
    BndLib_Add2dCurve::Add(Geom2dAdaptor_Curve(aCurve),0.,Box);
    Standard_Real aXmin, aYmin, aXmax, aYmax;
    Box.Get( aXmin, aYmin, aXmax, aYmax);
    myMinX = Standard_ShortReal(aXmin);
    myMinY = Standard_ShortReal(aYmin);
    myMaxX = Standard_ShortReal(aXmax);
    myMaxY = Standard_ShortReal(aYmax);
}

Handle(Geom2d_Curve) GGraphic2d_Curve::GeomCurve() const {
   return myCurve;
}

void GGraphic2d_Curve::Draw (const Handle(Graphic2d_Drawer)& aDrawer) { 

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

  if (IsIn) {
   switch ( myDisplayMode ) {
   case 0:
   default: {
        
    gp_GTrsf2d aTrsf = myGOPtr->Transform ();

    Standard_ShortReal Def;
    Standard_ShortReal Coeff;
    Aspect_TypeOfDeflection Type;
    aDrawer->DrawPrecision(Def,Coeff,Type);
    Standard_Boolean Controll = Standard_True;
    if (Type == Aspect_TOD_RELATIVE)
      Def = ( Abs(myMaxX-myMinX) + Abs(myMaxY-myMinY) ) * Coeff;
    Geom2dAdaptor_Curve GAC(myCurve);
    GCPnts_UniformDeflection UD(GAC,Def,Controll);
    if (UD.IsDone() ) {
      Standard_Integer i,j = 1,nbp = UD.NbPoints();
      gp_Pnt p2;
      Standard_Real x,y,z;
      
      while (nbp > 0) {
	    Standard_Integer n = Min(MAXPOINTS,nbp);
        for (i=1; i<=n; i++) {
	      p2 = UD.Value(j); j++;
	      p2.Coord(x,y,z);
	      Xpoint(i) = Standard_ShortReal( x );
	      Ypoint(i) = Standard_ShortReal( y );
        }

        if (myGOPtr->IsTransformed ()) {
          Standard_Real A, B;
          for (i=1; i<=n; i++) {
		    A = Standard_Real (Xpoint(i));
		    B = Standard_Real (Ypoint(i));
		    aTrsf.Transforms (A, B);
		    Xpoint(i) = Standard_ShortReal (A);
		    Ypoint(i) = Standard_ShortReal (B);
          }
        }
        DrawLineAttrib(aDrawer);
        aDrawer->MapPolylineFromTo(Xpoint,Ypoint,n);
        nbp -= MAXPOINTS; --j;
      }  // end while
    } // end UD.IsDone
   } // end case 0
   break;
   case 1: {
    if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve) ) ) {    
       Handle(Geom2d_BezierCurve) theBC = Handle(Geom2d_BezierCurve)::DownCast(myCurve); 
       TColgp_Array1OfPnt2d thePnts(1, theBC->NbPoles());
       theBC->Poles(thePnts);
       TShort_Array1OfShortReal XDpoint(thePnts.Lower(), thePnts.Upper());
       TShort_Array1OfShortReal YDpoint(thePnts.Lower(), thePnts.Upper());
       int i;
       for ( i = thePnts.Lower(); i <= thePnts.Upper(); ++i ) {
             XDpoint.SetValue( i, Standard_ShortReal( thePnts(i).X() ) );
             YDpoint.SetValue( i, Standard_ShortReal( thePnts(i).Y() ) );
       }
       
       if ( myGOPtr->IsTransformed() ) {
         gp_GTrsf2d aTrsf = myGOPtr->Transform();
         Standard_Real A, B;
         for ( i = XDpoint.Lower(); i <= XDpoint.Upper(); ++i ) {
            A = Standard_Real(XDpoint(i));
		    B = Standard_Real(YDpoint(i));
		    aTrsf.Transforms( A, B );
		    XDpoint(i) = Standard_ShortReal( A );
		    YDpoint(i) = Standard_ShortReal( B );
         }
       }
       DrawLineAttrib(aDrawer);
       aDrawer->MapPolylineFromTo(XDpoint,YDpoint,thePnts.Length());
       
       DrawMarkerAttrib(aDrawer);
       for ( i = XDpoint.Lower(); i <= XDpoint.Upper(); ++i ) 
         aDrawer->MapMarkerFromTo(VERTEXMARKER,XDpoint(i),YDpoint(i), DEFAULTMARKERSIZE,DEFAULTMARKERSIZE,0.0);

    } else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve) ) ) {                     
       Handle(Geom2d_BSplineCurve) theBC = Handle(Geom2d_BSplineCurve)::DownCast(myCurve); 
       TColgp_Array1OfPnt2d thePnts(1, theBC->NbPoles());
       theBC->Poles(thePnts);
       TShort_Array1OfShortReal XDpoint(thePnts.Lower(), thePnts.Upper());
       TShort_Array1OfShortReal YDpoint(thePnts.Lower(), thePnts.Upper());
       int i;
       for ( i = thePnts.Lower(); i <= thePnts.Upper(); ++i ) {
             XDpoint.SetValue( i, Standard_ShortReal( thePnts(i).X() ) );
             YDpoint.SetValue( i, Standard_ShortReal( thePnts(i).Y() ) );
       }
       
       if ( myGOPtr->IsTransformed() ) {
         gp_GTrsf2d aTrsf = myGOPtr->Transform();
         Standard_Real A, B;
         for ( i = XDpoint.Lower(); i <= XDpoint.Upper(); ++i ) {
            A = Standard_Real(XDpoint(i));
		    B = Standard_Real(YDpoint(i));
		    aTrsf.Transforms( A, B );
		    XDpoint(i) = Standard_ShortReal( A );
		    YDpoint(i) = Standard_ShortReal( B );
         }
       }
       DrawLineAttrib(aDrawer);
       aDrawer->MapPolylineFromTo(XDpoint,YDpoint,thePnts.Length());
       DrawMarkerAttrib(aDrawer);
       for ( i = XDpoint.Lower(); i <= XDpoint.Upper(); ++i ) 
         aDrawer->MapMarkerFromTo( VERTEXMARKER, XDpoint(i), YDpoint(i), DEFAULTMARKERSIZE, DEFAULTMARKERSIZE, 0.0 );
    }
       
   } //end case 1
   
   }  //end switch
  } // end IsIn
}

Standard_Boolean GGraphic2d_Curve::Pick (const Standard_ShortReal X,
					 const Standard_ShortReal Y,
					 const Standard_ShortReal aPrecision,
					 const Handle(Graphic2d_Drawer)& /*aDrawer*/) 

{
//Standard_ShortReal SRX = X, SRY = Y;
static Standard_ShortReal SRX , SRY ;
SRX = X;
SRY = Y;

if (IsInMinMax (X, Y, aPrecision)) {
  if (myGOPtr->IsTransformed ()) {
gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
	aTrsf.Transforms (RX, RY);
	SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
  }
  try { 
    OCC_CATCH_SIGNALS
    Geom2dAPI_ProjectPointOnCurve PC(gp_Pnt2d(SRX, SRY),
				     myCurve, 
				     myCurve->FirstParameter(),
				     myCurve->LastParameter());
    if(PC.NbPoints() == 0) {
      return Standard_False;
    }
    else {
      gp_Pnt2d P = PC.NearestPoint();
      return (( Abs ( SRX - P.X() ) +  Abs ( SRY - P.Y() ) ) < aPrecision);    
    }
    
  }
  catch(Standard_Failure)
    {
      return Standard_False;          
    }
}
return Standard_False;          //WNT
}

void GGraphic2d_Curve::Save(Aspect_FStream& aFStream) const
{
	*aFStream << "GGraphic2d_Curve" << endl;

	if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Line)) )
	{
		Handle(Geom2d_Line) theLine = Handle(Geom2d_Line)::DownCast( myCurve );
		gp_Dir2d tDir = theLine->Direction();
		gp_Pnt2d tPnt = theLine->Location();

		*aFStream << "Geom2d_Line" << endl;
		*aFStream << tPnt.X() << ' ' << tPnt.Y() << endl;
		*aFStream << tDir.X() << ' ' << tDir.Y() << endl;
	}
	else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Circle)) )
	{
		Handle(Geom2d_Circle) theCirc = Handle(Geom2d_Circle)::DownCast( myCurve );
		gp_Circ2d tCirc  = theCirc->Circ2d();
		gp_Ax22d tAx22d = tCirc.Axis();
		gp_Pnt2d tLoc  = tAx22d.Location();
		gp_Dir2d tDirX = tAx22d.XDirection(),
		       tDirY = tAx22d.YDirection();

		Standard_Real tRad = tCirc.Radius();

		*aFStream << "Geom2d_Circle" << endl;
		*aFStream << tRad << endl;
		*aFStream << tLoc.X() << ' ' << tLoc.Y() << endl;
		*aFStream << tDirX.X() << ' ' << tDirX.Y() << endl;
		*aFStream << tDirY.X() << ' ' << tDirY.Y() << endl;
	}
	else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Parabola)) )
	{
		Handle(Geom2d_Parabola) theParab = Handle(Geom2d_Parabola)::DownCast( myCurve );
		gp_Parab2d tParab = theParab->Parab2d();
		gp_Ax22d tAx22d = tParab.Axis();
		gp_Pnt2d tLoc  = tAx22d.Location();
		gp_Dir2d tDirX = tAx22d.XDirection(),
			tDirY = tAx22d.YDirection();

		Standard_Real tFocal = theParab->Focal();

		*aFStream << "Geom2d_Parabola" << endl;
		*aFStream << tFocal << endl;
		*aFStream << tLoc.X() << ' ' << tLoc.Y() << endl;
		*aFStream << tDirX.X() << ' ' << tDirX.Y() << endl;
		*aFStream << tDirY.X() << ' ' << tDirY.Y() << endl;
	}
	else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Ellipse)) )
	{
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
	}
	else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_Hyperbola)) )
	{
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
	}
	else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)) )
	{
		Handle(Geom2d_BezierCurve) theBC = Handle(Geom2d_BezierCurve)::DownCast(myCurve);
		Standard_Integer nbPoles = theBC->NbPoles();
		TColgp_Array1OfPnt2d tpoles( 1, nbPoles );
		theBC->Poles( tpoles );
		*aFStream << "Geom2d_BezierCurve" << endl;
		*aFStream << nbPoles << endl;
		for ( int i = 1; i <= nbPoles; i++ ) 
			*aFStream << tpoles(i).X() << ' ' << tpoles(i).Y() << endl;

	}
	else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) )
	{
//		Handle(Geom2d_BSplineCurve) theBSC = Handle(Geom2d_BSplineCurve)::DownCast(myCurve);
	}
	else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)) )
	{
//		Handle(Geom2d_OffsetCurve) theOC = Handle(Geom2d_OffsetCurve)::DownCast(myCurve);
	}
	else if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)) )
	{
//		Handle(Geom2d_TrimmedCurve) theTC = Handle(Geom2d_TrimmedCurve)::DownCast(myCurve);
	}
	Graphic2d_Line::Save(aFStream);
}

void GGraphic2d_Curve::Retrieve(Aspect_IFStream& anIFStream,
			const Handle(Graphic2d_GraphicObject)& aGraphicObject)
{
	char buf[100];

	anIFStream->getline(buf, 100);
	if (! strcmp(buf, "Geom2d_Line"))
	{
		Standard_Real X, Y, dX, dY;
		
		*anIFStream >> X >> Y;
		*anIFStream >> dX >> dY;
		gp_Pnt2d tPnt(X, Y);
		gp_Dir2d tDir(dX, dY);
		Handle(Geom2d_Line)
			theLine = new Geom2d_Line(tPnt, tDir);
		Handle(GGraphic2d_Curve)
			theGGC = new GGraphic2d_Curve(aGraphicObject, theLine);
		((Handle (Graphic2d_Line))theGGC)->Retrieve(anIFStream);
	}
	else if (! strcmp(buf, "Geom2d_Circle"))
	{
		Standard_Real tRad, X, Y, dXX, dXY, dYX, dYY;
		
		*anIFStream >> tRad;
		*anIFStream >> X >> Y;
		*anIFStream >> dXX >> dXY;
		*anIFStream >> dYX >> dYY;
		gp_Pnt2d tPnt(X, Y);
		gp_Dir2d tDirX(dXX, dXY);
		gp_Dir2d tDirY(dYX, dYY);
		gp_Ax22d tAx22d(tPnt, tDirX, tDirY);
		Handle(Geom2d_Circle)
			tCirc = new Geom2d_Circle(tAx22d, tRad);
		Handle(GGraphic2d_Curve)
			theGGC = new GGraphic2d_Curve(aGraphicObject, tCirc);
		((Handle (Graphic2d_Line))theGGC)->Retrieve(anIFStream);
	}
	else if (! strcmp(buf, "Geom2d_Parabola"))
	{
		Standard_Real tFocal, X, Y, dXX, dXY, dYX, dYY;
		
		*anIFStream >> tFocal;
		*anIFStream >> X >> Y;
		*anIFStream >> dXX >> dXY;
		*anIFStream >> dYX >> dYY;
		gp_Pnt2d tPnt(X, Y);
		gp_Dir2d tDirX(dXX, dXY);
		gp_Dir2d tDirY(dYX, dYY);
		gp_Ax22d tAx22d(tPnt, tDirX, tDirY);
		Handle(Geom2d_Parabola)
			gp_Parab2d = new Geom2d_Parabola(tAx22d, tFocal);
		Handle(GGraphic2d_Curve)
			theGGC = new GGraphic2d_Curve(aGraphicObject, gp_Parab2d);
		((Handle (Graphic2d_Line))theGGC)->Retrieve(anIFStream);
	}
	else if (! strcmp(buf, "Geom2d_Ellipse"))
	{
		Standard_Real tMin, tMaj, X, Y, dXX, dXY, dYX, dYY;
		
		*anIFStream >> tMin >> tMaj;
		*anIFStream >> X >> Y;
		*anIFStream >> dXX >> dXY;
		*anIFStream >> dYX >> dYY;
		gp_Pnt2d tPnt(X, Y);
		gp_Dir2d tDirX(dXX, dXY);
		gp_Dir2d tDirY(dYX, dYY);
		gp_Ax22d tAx22d(tPnt, tDirX, tDirY);
		Handle(Geom2d_Ellipse)
			theEllipse = new Geom2d_Ellipse(tAx22d, tMaj, tMin);
		Handle(GGraphic2d_Curve)
			theGGC = new GGraphic2d_Curve(aGraphicObject, theEllipse);
		((Handle (Graphic2d_Line))theGGC)->Retrieve(anIFStream);
	}
	else if (! strcmp(buf, "Geom2d_Hyperbola"))
	{
		Standard_Real tMin, tMaj, X, Y, dXX, dXY, dYX, dYY;
		
		*anIFStream >> tMin >> tMaj;
		*anIFStream >> X >> Y;
		*anIFStream >> dXX >> dXY;
		*anIFStream >> dYX >> dYY;
		gp_Pnt2d tPnt(X, Y);
		gp_Dir2d tDirX(dXX, dXY);
		gp_Dir2d tDirY(dYX, dYY);
		gp_Ax22d tAx22d(tPnt, tDirX, tDirY);
		Handle(Geom2d_Hyperbola)
			theHypr = new Geom2d_Hyperbola(tAx22d, tMaj, tMin);
		Handle(GGraphic2d_Curve)
			theGGC = new GGraphic2d_Curve(aGraphicObject, theHypr);
		((Handle (Graphic2d_Line))theGGC)->Retrieve(anIFStream);
	}
	else if (! strcmp(buf, "Geom2d_BezierCurve"))
	{
		Standard_Integer numPoles;
		Standard_Real X, Y;
		
		*anIFStream >> numPoles;
		TColgp_Array1OfPnt2d listPoles(1, numPoles);
		for (Standard_Integer i=1; i<=numPoles; i++)
		{
			*anIFStream >> X >> Y;
			listPoles.SetValue(i, gp_Pnt2d(X, Y));
		}
		Handle(Geom2d_BezierCurve)
			theBC = new Geom2d_BezierCurve(listPoles);
		Handle(GGraphic2d_Curve)
			theGGC = new GGraphic2d_Curve(aGraphicObject, theBC);
		((Handle (Graphic2d_Line))theGGC)->Retrieve(anIFStream);
	}
	else if (! strcmp(buf, "Geom2d_BSplineCurve"))
		;
	else if (! strcmp(buf, "Geom2d_OffsetCurve"))
		;
	else if (! strcmp(buf, "Geom2d_TrimmedCurve"))
		;
	else 
	  {}
}
