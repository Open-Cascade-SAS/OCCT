#define xTRACE

// S3569	//Gestion des zones cachantes

#define G002        //GG_140400
//              Manage highlighting.

#include <Graphic2d_HidingGraphicObject.ixx>
#include <Graphic2d_Vertex.hxx>
#include <Aspect.hxx>

#define BASE_PRIORITY 1000
#define MAX_PRIORITY 999
#define MAX_POINTS 1024

Graphic2d_HidingGraphicObject::Graphic2d_HidingGraphicObject
       (const Handle(Graphic2d_View)& aView,
	const Graphic2d_Array1OfVertex& aFrame) :
Graphic2d_GraphicObject(aView) ,
myXmin(RealLast()),myYmin(RealLast()),myXmax(RealFirst()),myYmax(RealFirst())
{

  SetFrame(aFrame);
  SetHidingColorIndex();
  SetFrameColorIndex();
  SetFrameTypeIndex();
  SetFrameWidthIndex();

}

Graphic2d_HidingGraphicObject::Graphic2d_HidingGraphicObject
       (const Handle(Graphic2d_View)& aView,
	const Graphic2d_TypeOfFrame aType,
	const Quantity_Length aMargin1,
	const Quantity_Length aMargin2) :
Graphic2d_GraphicObject(aView), 
myXmin(RealLast()),myYmin(RealLast()),myXmax(RealFirst()),myYmax(RealFirst())
{

  SetFrame(aType,aMargin1,aMargin2);
  SetHidingColorIndex();
  SetFrameColorIndex();
  SetFrameTypeIndex();
  SetFrameWidthIndex();

}

void Graphic2d_HidingGraphicObject::SetFrame(const Graphic2d_Array1OfVertex& aFrame) {
static Graphic2d_Vertex v;
Standard_Integer l = aFrame.Length();
Standard_Real x,y;

  	myFrame = new Graphic2d_HArray1OfVertex(1,l);
  	for( Standard_Integer i=1,j=aFrame.Lower() ; i<=l ; i++,j++) {
    	  v = aFrame.Value(j); v.Coord(x,y);
    	  myXmin = Min(myXmin,x); myYmin = Min(myYmin,y);
    	  myXmax = Max(myXmax,x); myYmax = Max(myYmax,y);
    	  myFrame->SetValue(i,v);
  	}
  	myTypeOfFrame = Graphic2d_TOF_UNKNOWN;
  	myIsComputed = Standard_True;
}

void Graphic2d_HidingGraphicObject::SetFrame(
			const Graphic2d_TypeOfFrame aType,
			const Quantity_Length aMargin1,
			const Quantity_Length aMargin2) {
	myFrame.Nullify();
	myTypeOfFrame = aType;
	myFrameMargin1 = aMargin1;
	myFrameMargin2 = aMargin2;
	if( myTypeOfFrame != Graphic2d_TOF_UNKNOWN )
				myIsComputed = Standard_False;
	else myIsComputed = Standard_True;
}

void Graphic2d_HidingGraphicObject::SetHidingColorIndex(const Standard_Integer anIndex) {
	myHidingColorIndex = anIndex;
}

void Graphic2d_HidingGraphicObject::SetFrameColorIndex(const Standard_Integer anIndex) {
	myFrameColorIndex = anIndex;
}

void Graphic2d_HidingGraphicObject::SetFrameTypeIndex(const Standard_Integer anIndex) {
	myFrameTypeIndex = anIndex;
}

void Graphic2d_HidingGraphicObject::SetFrameWidthIndex(const Standard_Integer anIndex) {
	myFrameWidthIndex = anIndex;
}

Standard_Integer Graphic2d_HidingGraphicObject::BasePriority () const {

	return BASE_PRIORITY;

}

Standard_Integer Graphic2d_HidingGraphicObject::MaxPriority () const {

	return MAX_PRIORITY;

}

void Graphic2d_HidingGraphicObject::Draw (const Handle(Graphic2d_Drawer)& aDrawer, const Standard_Boolean Reset) {

	if( !IsDrawable() ) return;

	if( !IsDisplayed() && !IsHighlighted() ) return;

	if( !myIsUpToDate || !myIsComputed  ) {
	  static Graphic2d_Vertex v;
	  myIsComputed = 
		Graphic2d_GraphicObject::MinMax(myXmin,myXmax,myYmin,myYmax);

	  if( myIsComputed ) switch (myTypeOfFrame) {
	    case Graphic2d_TOF_RECTANGULAR :
	      myXmin -= myFrameMargin1; myYmin -= myFrameMargin2;
	      myXmax += myFrameMargin1; myYmax += myFrameMargin2;
	      if( myFrame.IsNull() || (myFrame->Length() != 4) ) {
	      	myFrame = new Graphic2d_HArray1OfVertex(1,4);
	      }
	      v.SetCoord(myXmin,myYmin);
	      myFrame->SetValue(1,v);
	      v.SetCoord(myXmin,myYmax);
	      myFrame->SetValue(2,v);
	      v.SetCoord(myXmax,myYmax);
	      myFrame->SetValue(3,v);
	      v.SetCoord(myXmax,myYmin);
	      myFrame->SetValue(4,v);
	      break;
	    case Graphic2d_TOF_CIRCULAR :
	      {
	        Standard_ShortReal precis,coef,radius;
                radius = Standard_ShortReal(Sqrt( (myXmax - myXmin) * (myXmax - myXmin) +
                             (myYmax - myYmin) * (myYmax - myYmin) ) / 2.);
		radius += Standard_ShortReal(myFrameMargin1);
		Standard_ShortReal x0 = Standard_ShortReal((myXmax + myXmin)/2.);
		Standard_ShortReal y0 = Standard_ShortReal((myYmax + myYmin)/2.);
	        myXmin = x0 - radius; 
	        myYmin = y0 - radius; 
	        myXmax = x0 + radius; 
	        myYmax = y0 + radius; 
	        Aspect_TypeOfDeflection type;
	        aDrawer->DrawPrecision(precis,coef,type);
                if( type == Aspect_TOD_RELATIVE) {
                  precis = radius * coef;
                }
                Standard_ShortReal value = Standard_ShortReal(Standard_PI/4.);
                if( radius > precis )
                  value = Standard_ShortReal(Max( 0.0044 ,
                                Min (0.7854 , 2. * ACos(1.-precis/radius))));
                Standard_Integer n = Min(MAX_POINTS,
                                Standard_Integer(2.*Standard_PI/value)+2);
                Standard_ShortReal da = Standard_ShortReal(2.*Standard_PI/n);
                Standard_ShortReal cosin = Standard_ShortReal(Cos(da));
                Standard_ShortReal x1 = radius;
                Standard_ShortReal y1 = 0.;
                Standard_ShortReal x2 = radius * cosin;
                Standard_ShortReal y2 = Standard_ShortReal(radius * Sin(da));
                Standard_ShortReal x,y;

	        if( myFrame.IsNull() || (myFrame->Length() != n) ) {
	      	  myFrame = new Graphic2d_HArray1OfVertex(1,n);
	        }
		v.SetCoord(x0+x1,y0+y1);
		myFrame->SetValue(1,v);
		v.SetCoord(x0+x2,y0+y2);
		myFrame->SetValue(2,v);
                for (Standard_Integer i=3; i<=n; i++) {
                  x = Standard_ShortReal(2.*x2*cosin - x1); x1 = x2; x2 = x;
                  y = Standard_ShortReal(2.*y2*cosin - y1); y1 = y2; y2 = y;
		  v.SetCoord(x0+x,y0+y);
		  myFrame->SetValue(i,v);
                }
	        break;
	      }
#ifndef DEB
	    default:
	      break;
#endif
	  } else {
	    myXmin = myYmin = RealLast();
	    myXmax = myYmax = RealFirst();
	  }
	}

	if( !myFrame.IsNull() && (myXmax > myXmin) && (myYmax > myYmin) ) {
	  Standard_Real xmin,ymin,xmax,ymax;
	  Standard_Boolean transform = Standard_False; 
	  gp_GTrsf2d theTrsf;
	  xmin = myXmin; ymin = myYmin;
	  xmax = myXmax; ymax = myYmax;
	  if( IsTransformed() ) {
	    transform = Standard_True;
	    theTrsf = Transform();
	    TransformMinMax(theTrsf,xmin,xmax,ymin,ymax);
	  }

	  if( aDrawer->IsIn (
		Standard_ShortReal(xmin), Standard_ShortReal(xmax),
			Standard_ShortReal(ymin),Standard_ShortReal(ymax)) ) { 
	    if( IsHighlighted() ) {
              aDrawer->SetOverride ( Standard_True );
              aDrawer->SetOverrideColor ( myOverrideColor );
            } else {
              aDrawer->SetOffSet ( myOffSet );
            }

	    aDrawer->SetHidingPolyAttrib (myHidingColorIndex,
		myFrameColorIndex,myFrameTypeIndex,myFrameWidthIndex);

	    Standard_Real x,y;
	    Standard_Integer i,l = myFrame->Length(),mode = l;
	    for( i=1 ; i<=l ; i++ ) {
	      (myFrame->Value(i)).Coord(x,y);
	      if( transform ) {
		theTrsf.Transforms(x,y);
	      }
	      if( i >= l ) mode = -1;
#ifdef TRACE
cout << " Graphic2d_HidingGraphicObject::Draw(" << x << "," << y << "," << mode << endl;
#endif
	      aDrawer->MapPolygonFromTo(Standard_ShortReal(x),
					Standard_ShortReal(y),mode);
	      mode = 0;
	    }
	  }
	}

	Graphic2d_GraphicObject::Draw(aDrawer,Reset);
}

void Graphic2d_HidingGraphicObject::Draw(const Handle_Graphic2d_Drawer& aDrawer, const Handle_Graphic2d_Primitive& aPrim)
{
 Graphic2d_GraphicObject::Draw(aDrawer,aPrim);
}

Standard_Boolean Graphic2d_HidingGraphicObject::Pick (const Standard_Real X, const Standard_Real Y, const Standard_Real aPrecision, const Handle(Graphic2d_Drawer)& aDrawer) {
Standard_Boolean status = Standard_False;

#ifndef G002
  myPickedIndex = 0;
#endif
  if( IsPickable() && (IsDisplayed() || IsHighlighted()) ) {

    if( myIsComputed ) {
      Standard_Real xx = X, yy = Y;
      Standard_Real xmin = myXmin - aPrecision;
      Standard_Real ymin = myYmin - aPrecision;
      Standard_Real xmax = myXmax + aPrecision;
      Standard_Real ymax = myYmax + aPrecision;
      if( IsTransformed() ) {
	gp_GTrsf2d aTrsf = (Transform ()).Inverted ();
        aTrsf.Transforms (xx, yy);
      }
      if( (xx > xmin) && (xx < xmax) &&
		(yy > ymin) && (yy < ymax) ) {
	switch (myTypeOfFrame) {
	  case Graphic2d_TOF_UNKNOWN :
	    status = (myFrame.IsNull()) ? Standard_False : Standard_True;
	    break;

	  case Graphic2d_TOF_RECTANGULAR :
	    status = Standard_True;
	    break;

	  case Graphic2d_TOF_CIRCULAR :
	    {
	      Standard_Real x = (xmin + xmax)/2.;
	      Standard_Real y = (ymin + ymax)/2.;
	      Standard_Real r = xmax - x;
	      if( Sqrt ((x - xx)*(x - xx) + (y - yy)*(y - yy)) < r )
			status = Standard_True;
	    }
	    break;
	}
      } 
    }

    if( !status )
    	status = Graphic2d_GraphicObject::Pick(X,Y,aPrecision,aDrawer);
#ifdef G002
    else SetPickedIndex(0);
#endif
  }

  return status;
}

Standard_Boolean Graphic2d_HidingGraphicObject::FrameMinMax (Quantity_Length & Minx, Quantity_Length &Maxx, Quantity_Length & Miny, Quantity_Length & Maxy) const {
Standard_Boolean status = Standard_False;

	if( !myFrame.IsNull() && (myXmax > myXmin) && (myYmax > myYmin) ) {
	  Standard_Real xmin,ymin,xmax,ymax;
	  xmin = myXmin; ymin = myYmin;
	  xmax = myXmax; ymax = myYmax;
	  if( IsTransformed() ) {
	    gp_GTrsf2d theTrsf = Transform();
	    TransformMinMax(theTrsf,xmin,xmax,ymin,ymax);
	  }
	  status = Standard_True;
	  Minx = xmin;
	  Miny = ymin;
	  Maxx = xmax;
	  Maxy = ymax;
	} else {
	  Minx = RealFirst();
	  Miny = RealFirst();
	  Maxx = RealLast();
	  Maxy = RealLast();
	}

	return status;
}

Standard_Boolean Graphic2d_HidingGraphicObject::MinMax (Quantity_Length & Minx, Quantity_Length &Maxx, Quantity_Length & Miny, Quantity_Length & Maxy) const {

	Standard_Boolean status = 
		Graphic2d_GraphicObject::MinMax(Minx,Maxx,Miny,Maxy);

	Standard_Real xmin,ymin,xmax,ymax;
	if( this->FrameMinMax(xmin,ymin,xmax,ymax) ) {
	  if( status ) {
	    Minx = Min(Minx,xmin);
	    Miny = Min(Miny,ymin);
	    Maxx = Min(Maxx,xmax);
	    Maxy = Min(Maxy,ymax);
	  } else {
	    status = Standard_True;
	    Minx = xmin;
	    Miny = ymin;
	    Maxx = xmax;
	    Maxy = ymax;
	  }
	}

	return status;
}

Graphic2d_TypeOfFrame Graphic2d_HidingGraphicObject::Frame( Graphic2d_Array1OfVertex& aFrame) const {
//Graphic2d_Vertex v;
//Standard_Real x,y;

	aFrame = myFrame->Array1();
	return myTypeOfFrame;
}

Standard_Integer Graphic2d_HidingGraphicObject::HidingColorIndex() const {

	return myHidingColorIndex;
}

Standard_Integer Graphic2d_HidingGraphicObject::FrameColorIndex() const {

	return myFrameColorIndex;
}

Standard_Integer Graphic2d_HidingGraphicObject::FrameTypeIndex() const {

	return myFrameTypeIndex;
}

Standard_Integer Graphic2d_HidingGraphicObject::FrameWidthIndex() const {

	return myFrameWidthIndex;
}

void Graphic2d_HidingGraphicObject::TransformMinMax (const gp_GTrsf2d& aTrsf,Standard_Real & Minx, Standard_Real &Maxx, Standard_Real & Miny, Standard_Real & Maxy) const {

	  Standard_Real xx,yy,xmin,ymin,xmax,ymax;
	  xmin = ymin = RealLast(); 
	  xmax = ymax = RealFirst();

	  xx = Minx; yy = Miny; 
	  aTrsf.Transforms (xx,yy);
	  xmin = Min(xmin,xx) ; 
	  ymin = Min(ymin,yy);
	  xmax = Max(xmax,xx) ; 
	  ymax = Max(ymax,yy);

	  xx = Minx; yy = Maxy; 
	  aTrsf.Transforms (xx,yy);
	  xmin = Min(xmin,xx) ; 
	  ymin = Min(ymin,yy);
	  xmax = Max(xmax,xx) ; 
	  ymax = Max(ymax,yy);

	  xx = Maxx; yy = Maxy; 
	  aTrsf.Transforms (xx,yy);
	  xmin = Min(xmin,xx) ; 
	  ymin = Min(ymin,yy);
	  xmax = Max(xmax,xx) ; 
	  ymax = Max(ymax,yy);

	  xx = Maxx; yy = Miny; 
	  aTrsf.Transforms (xx,yy);
	  Minx = Min(xmin,xx) ; 
	  Miny = Min(ymin,yy);
	  Maxx = Max(xmax,xx) ; 
	  Maxy = Max(ymax,yy);
}
