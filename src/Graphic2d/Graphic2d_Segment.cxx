#define WTO0001         //GG_140596
//                      Calcul des min-max faux apres transformation.

#define G002	//Use SetPickedIndex() method
//              Add new DrawElement(), DrawVertex() methods

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0

#include <Graphic2d_Segment.ixx>

Graphic2d_Segment::Graphic2d_Segment (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Standard_Real X1, const Standard_Real Y1,
	const Standard_Real X2, const Standard_Real Y2)
  : Graphic2d_Line (aGraphicObject)
{
    myX1	= Standard_ShortReal (X1); 
    myY1	= Standard_ShortReal (Y1); 
    myX2	= Standard_ShortReal (X2); 
    myY2	= Standard_ShortReal (Y2);

    myMinX	= (myX1 < myX2 ? myX1 : myX2);
    myMinY	= (myY1 < myY2 ? myY1 : myY2);
    myMaxX	= (myX1 > myX2 ? myX1 : myX2);
    myMaxY	= (myY1 > myY2 ? myY1 : myY2);
    myNumOfElem = 1;
    myNumOfVert = 2;
}

void Graphic2d_Segment :: FirstPoint ( Quantity_Length& X,Quantity_Length& Y ) const
{
  X = Standard_Real(myX1);
  Y = Standard_Real(myY1);
}

void Graphic2d_Segment :: SecondPoint ( Quantity_Length& X,Quantity_Length& Y ) const
{
  X = Standard_Real(myX2);
  Y = Standard_Real(myY2);
}

void Graphic2d_Segment::Draw (const Handle(Graphic2d_Drawer)& aDrawer)
{

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

  if ( IsIn ) {
   DrawLineAttrib(aDrawer);
   switch ( myDisplayMode ) {
   case 0 :
   default: {
    Standard_ShortReal a = myX1, b = myY1, c = myX2, d = myY2;

    if ( myGOPtr->IsTransformed() ) {
     gp_GTrsf2d aTrsf = myGOPtr->Transform ();
     Standard_Real A, B, C, D;
	 A = Standard_Real (a);
	 B = Standard_Real (b);
	 C = Standard_Real (c);
	 D = Standard_Real (d);
	 aTrsf.Transforms (A, B);
	 aTrsf.Transforms (C, D);
	 a = Standard_ShortReal (A);
	 b = Standard_ShortReal (B);
	 c = Standard_ShortReal (C);
	 d = Standard_ShortReal (D);
    }
    aDrawer->MapSegmentFromTo(a, b, c, d);         
    }
    break;
   case 1 : {
     Standard_ShortReal minX = myMinX, minY = myMinY, maxX = myMaxX, maxY = myMaxY;
     if ( myGOPtr->IsTransformed() ) {
      gp_GTrsf2d aTrsf = myGOPtr->Transform ();
      Standard_Real A, B, C, D;
	  A = Standard_Real(minX);
	  B = Standard_Real(minY);
	  C = Standard_Real(maxX);
	  D = Standard_Real(maxY);
	  aTrsf.Transforms(A, B);
	  aTrsf.Transforms(C, D);
	  minX = Standard_ShortReal(A);
	  minY = Standard_ShortReal(B);
	  maxX = Standard_ShortReal(C);
	  maxY = Standard_ShortReal(D);
     }
     aDrawer->MapSegmentFromTo(minX, minY, minX, maxY);
     aDrawer->MapSegmentFromTo(minX, maxY, maxX, maxY );
     aDrawer->MapSegmentFromTo(maxX, maxY, maxX, minY );
     aDrawer->MapSegmentFromTo(maxX, minY, minX, minY );
    }
   } // end switch
  } // end if IsIn
}

#ifdef G002
void Graphic2d_Segment::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                     const Standard_Integer anIndex)
{
  if ( anIndex == 1 )
    Draw( aDrawer );
}

void Graphic2d_Segment::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                                    const Standard_Integer anIndex)
{
  Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }
  if ( IsIn ) {
   if ( anIndex == 1 || anIndex == 2 ) {
    Standard_ShortReal X,Y;
    DrawMarkerAttrib (aDrawer);
    if ( myGOPtr->IsTransformed() ) {
      gp_GTrsf2d aTrsf = myGOPtr->Transform ();
      Standard_Real A, B;
      if ( anIndex == 1 ) { 
       A = Standard_Real(myX1);
       B = Standard_Real(myY1);
      } else {
       A = Standard_Real(myX2);
       B = Standard_Real(myY2);
      }
      aTrsf.Transforms (A, B);
      X = Standard_ShortReal(A);
      Y = Standard_ShortReal(B);
    } else {
      if ( anIndex == 1 ) { 
        X = myX1;
        Y = myY1;
      } else {
        X = myX2;
        Y = myY2;
      }
    }
     aDrawer->MapMarkerFromTo(VERTEXMARKER,X,Y, DEFAULTMARKERSIZE,DEFAULTMARKERSIZE,0.0);
   }
  }
}
#endif

Standard_Boolean Graphic2d_Segment::Pick(
             const Standard_ShortReal X,
			 const Standard_ShortReal Y,
			 const Standard_ShortReal aPrecision,
			 const Handle(Graphic2d_Drawer)& aDrawer)
{
  Standard_ShortReal SRX = X, SRY = Y;
  Standard_Boolean Result = Standard_False;

  if ( IsInMinMax( X, Y, aPrecision ) ) {
	if ( myGOPtr->IsTransformed() ) {
      gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
      Standard_Real RX = Standard_Real( SRX ), RY = Standard_Real( SRY );
	  aTrsf.Transforms( RX, RY );
	  SRX = Standard_ShortReal( RX );
	  SRY = Standard_ShortReal( RY );
	}
    #ifdef G002
     if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX1, myY1, aPrecision) ) {
        SetPickedIndex(-1);
        Result = Standard_True;
      } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX2, myY2, aPrecision) ) {
        SetPickedIndex(-2);
        Result = Standard_True;
      } else if ( IsOn( SRX, SRY, myX1, myY1, myX2, myY2, aPrecision ) ) {
        SetPickedIndex( 1 );
        Result = Standard_True;
      }
    #else	 
	 Result = IsOn( SRX, SRY, myX1, myY1, myX2, myY2, aPrecision );
    #endif
  }
  return Result;
}

void Graphic2d_Segment::Save(Aspect_FStream& aFStream) const
{
	*aFStream << "Graphic2d_Segment" << endl;
	*aFStream << myX2 << ' ' << myY2 << endl;
	*aFStream << myX1 << ' ' << myY1 << endl;
	Graphic2d_Line::Save(aFStream);
}

void Graphic2d_Segment::Retrieve(Aspect_IFStream& anIFStream,
			const Handle(Graphic2d_GraphicObject)& aGraphicObject)
{
	Quantity_Length X1, Y1, X2, Y2;
	
	*anIFStream >> X2 >> Y2;
	*anIFStream >> X1 >> Y1;
	Handle(Graphic2d_Segment)
		theSeg = new Graphic2d_Segment(aGraphicObject, X1, Y1, X2, Y2);
	((Handle (Graphic2d_Line))theSeg)->Retrieve(anIFStream);
}

