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


#define PRO3730

#define WTO0006	//GG_221196
//		Ne pas transformer les cercles pleins en arc de cercles
//		presque pleins a EPSILON pres car dans ce cas les
//		JAP ne peuvent plus tracer leur drapeaux !!!

#define BUC50076//GG_231097/090298
//		Attention aux transformations de type MIRROR
//		car dans ce cas il faut inverser le parcours de l'arc et
//		surtout ne pas annuler la translation dans la matrice!


#define G002	//Add new DrawElement(), DrawVertex() methods

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0
#define MAXPOINTS 1023

#include <Graphic2d_Circle.ixx>
#include <Quantity_PlaneAngle.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <gp_Trsf2d.hxx>

#include <Graphic2d_Primitive.pxx>

Graphic2d_Circle::Graphic2d_Circle
  (const Handle(Graphic2d_GraphicObject)& aGraphicObject,
   const Quantity_Length X, const Quantity_Length Y, const Quantity_Length Radius)
    : Graphic2d_Line(aGraphicObject) {

	myX = Standard_ShortReal(X);
	myY = Standard_ShortReal(Y);
	myRadius = Standard_ShortReal(Radius);

	if ( myRadius <= ShortRealEpsilon ())
                Graphic2d_CircleDefinitionError::Raise ("The radius = 0.");

    DoMinMax();
    myFirstAngle = 0.;
    mySecondAngle = Standard_ShortReal(2. * M_PI);
    myisArc = Standard_False;
    myNumOfElem = MAXPOINTS + 1;
    myNumOfVert = 3;
}

Graphic2d_Circle::Graphic2d_Circle
  (const Handle(Graphic2d_GraphicObject)& aGraphicObject,
   const Quantity_Length X, const Quantity_Length Y,
   const Quantity_Length Radius, const Quantity_PlaneAngle Alpha,
   const Quantity_PlaneAngle Beta)
    : Graphic2d_Line(aGraphicObject) {

	myX = Standard_ShortReal(X);
	myY = Standard_ShortReal(Y);
	myRadius = Standard_ShortReal(Radius);
    myisArc = Standard_True;
	if (myRadius <= ShortRealEpsilon ())
                Graphic2d_CircleDefinitionError::Raise ("The radius = 0.");
    myNumOfElem = MAXPOINTS + 1;
    myNumOfVert = 3;
#ifdef PRO3730
	Standard_ShortReal TwoPI = Standard_ShortReal(2. * M_PI);
	myFirstAngle	= Standard_ShortReal (Alpha);
    mySecondAngle	= Standard_ShortReal (Beta);
	while( myFirstAngle < 0. ) myFirstAngle += TwoPI;
	while( myFirstAngle > 2. * M_PI ) myFirstAngle -= TwoPI;
	while( mySecondAngle < 0. ) mySecondAngle += TwoPI;
	while( mySecondAngle > 2. * M_PI ) mySecondAngle -= TwoPI;
	if( mySecondAngle < myFirstAngle ) mySecondAngle += TwoPI;
	if ( (mySecondAngle - myFirstAngle < ShortRealEpsilon()) || 
			(mySecondAngle - myFirstAngle >= TwoPI) ) {
	   myFirstAngle = 0.;
	   mySecondAngle = TwoPI;
  	   DoMinMax();
	} else {
	  Standard_ShortReal Acur,Xcur,Ycur,Xsav;
          myMinX = myMaxX = Standard_ShortReal(Cos(myFirstAngle));
          myMinY = myMaxY = Standard_ShortReal(Sin(myFirstAngle));
	      Xcur = Standard_ShortReal(Cos(mySecondAngle));
	      Ycur = Standard_ShortReal(Sin(mySecondAngle));
          myMinX = myMinX < Xcur ? myMinX : Xcur;
          myMaxX = myMaxX < Xcur ? Xcur : myMaxX;
          myMinY = myMinY < Ycur ? myMinY : Ycur;
          myMaxY = myMaxY < Ycur ? Ycur : myMaxY;

          for( Acur = 0.,Xcur = 1.,Ycur = 0.; 
		       Acur < mySecondAngle; Acur += Standard_ShortReal(M_PI / 2.) ) {
	        if( Acur > myFirstAngle ) {
              myMinX = ( myMinX < Xcur ? myMinX : Xcur );
              myMaxX = ( myMaxX < Xcur ? Xcur : myMaxX );
              myMinY = ( myMinY < Ycur ? myMinY : Ycur );
              myMaxY = ( myMaxY < Ycur ? Ycur : myMaxY );
	    }
            Xsav = Xcur ;Xcur = -Ycur; Ycur = Xsav;
          }
          myMinX = myX + myRadius * myMinX;
          myMaxX = myX + myRadius * myMaxX;
          myMinY = myY + myRadius * myMinY;
          myMaxY = myY + myRadius * myMaxY;
	}
#else
      DoMinMax();
#endif
}

void Graphic2d_Circle :: Center ( Quantity_Length& X,Quantity_Length& Y ) const {
     
 X = Quantity_Length(myX);
 Y = Quantity_Length(myY);
}

Quantity_Length Graphic2d_Circle :: Radius (  ) const {

 return Quantity_Length(myRadius);
}

Quantity_PlaneAngle Graphic2d_Circle :: FirstAngle (  ) const {
 
    return Quantity_PlaneAngle(myFirstAngle);

}

Quantity_PlaneAngle Graphic2d_Circle :: SecondAngle (  ) const {

    return Quantity_PlaneAngle(mySecondAngle);

}

void Graphic2d_Circle::SetCenter( const Quantity_Length X,
                                  const Quantity_Length Y ) {
    myX = Standard_ShortReal( X );
    myY = Standard_ShortReal( Y );
    DoMinMax();

}

void Graphic2d_Circle::SetRadius( const Quantity_Length theR ) {
    myRadius = Standard_ShortReal( theR );
    DoMinMax();
}

void Graphic2d_Circle::SetAngles( const Quantity_PlaneAngle Alpha,
                                  const Quantity_PlaneAngle Beta ) {

    myFirstAngle  = Standard_ShortReal( Alpha );
    mySecondAngle = Standard_ShortReal( Beta );
}


void Graphic2d_Circle::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

  Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if (IsIn) {

    DrawLineAttrib(aDrawer);

    Standard_ShortReal a, b, c, e, f;

    a = myX; b = myY; c = myRadius;
    e = myFirstAngle; f = mySecondAngle;
    if (myGOPtr->IsTransformed ()) {
       gp_GTrsf2d aTrsf = myGOPtr->Transform ();
       Standard_Real A, B;
       Standard_Real X0, Y0, X1, Y1, X2, Y2, E, F;
	   A = Standard_Real (a); B = Standard_Real (b);
	   E = Standard_Real (e); F = Standard_Real (f);
	   aTrsf.Transforms (A, B);
	   a = Standard_ShortReal (A); b = Standard_ShortReal (B);
	   if( Abs(f-e) < Standard_ShortReal(2. * M_PI) ) {
	  // To calculate new aperture angles 
	  // the calculation is done on the trigonometric circle
	  // and in this case the translation is not taken into account
	  // except for transformations of type Mirror
	  // with negative determinant.
#ifndef BUC50076
	  aTrsf.SetValue (1, 3, 0.0);
	  aTrsf.SetValue (2, 3, 0.0);
#endif
	  X1 = Cos (E); Y1 = Sin (E);
	  X2 = Cos (F); Y2 = Sin (F);
	  aTrsf.Transforms (X1, Y1); 
	  aTrsf.Transforms (X2, Y2); 
#ifdef BUC50076
	  X0 = Y0 = 0.;
	  aTrsf.Transforms (X0, Y0); 
	  X1 -= X0; Y1 -= Y0;
	  X2 -= X0; Y2 -= Y0;
#endif
          c = myRadius * Standard_ShortReal(Sqrt(X1*X1 + Y1*Y1));
          e = Standard_ShortReal(atan2(Y1,X1));
          f = Standard_ShortReal(atan2(Y2,X2));
#ifdef BUC50076
	  if( aTrsf.IsNegative() ) {
	    Standard_ShortReal t = e; e = f; f = t;
	  }
#endif
	}
    }

    if (myTypeOfPolygonFilling == Graphic2d_TOPF_EMPTY) {
      aDrawer->MapArcFromTo(a, b, c, e, f);
    } else {
      aDrawer->MapPolyArcFromTo(a, b, c, e, f);
    }

  }
}

#ifdef G002

void Graphic2d_Circle::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                    const Standard_Integer anIndex) {
  
   Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if ( IsIn ) {
    
    DrawLineAttrib(aDrawer);
    
    if ( anIndex > 0 && anIndex <= MAXPOINTS + 1 ) { 

    Standard_ShortReal teta = Abs( mySecondAngle - myFirstAngle ) / MAXPOINTS;
    Standard_ShortReal Xp = Standard_ShortReal(myX + myRadius * Cos(myFirstAngle + teta*(anIndex-1))),
                       Yp = Standard_ShortReal(myY + myRadius * Sin(myFirstAngle + teta*(anIndex-1)));
    Standard_Real A, B, C, D;
    Standard_ShortReal a = myX, b = myY, c = Xp, d = Yp;
    if ( myGOPtr->IsTransformed() ) {
     gp_GTrsf2d aTrsf = myGOPtr->Transform ();
	 A = Standard_Real(a);
	 B = Standard_Real(b);
     C = Standard_Real(c);
	 D = Standard_Real(d);
     aTrsf.Transforms (A, B);
     aTrsf.Transforms (C, D);
     a = Standard_ShortReal(A);
	 b = Standard_ShortReal(B);
     c = Standard_ShortReal(C);
	 d = Standard_ShortReal(D);
    }

    aDrawer->MapSegmentFromTo( a, b, c, d );
   }                                           
 }

}

void Graphic2d_Circle::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                                   const Standard_Integer anIndex) {

  Standard_Boolean IsIn = Standard_False;

  if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }
  
  if ( IsIn  ) {
   if ( anIndex > 0 && anIndex < 4) {
    DrawMarkerAttrib( aDrawer );
    Standard_ShortReal X=0.,Y=0.;
    if ( anIndex == 1 ) {
        X = myX; Y = myY; 
    } else if ( anIndex == 2 ) {
        X = Standard_ShortReal( myX + myRadius *Cos( myFirstAngle ) );
        Y = Standard_ShortReal( myY + myRadius *Sin( myFirstAngle ) );
    } else if ( anIndex == 3 ) {
        X = Standard_ShortReal( myX + myRadius *Cos( mySecondAngle ) );
        Y = Standard_ShortReal( myY + myRadius *Sin( mySecondAngle ) );
    }
    if ( myGOPtr->IsTransformed() ) {
        gp_GTrsf2d aTrsf = myGOPtr->Transform ();
        Standard_Real A, B;
        A = Standard_Real( X );
        B = Standard_Real( Y );
        aTrsf.Transforms (A, B);
        X = Standard_ShortReal( A );
        Y = Standard_ShortReal( B );
    } 
     aDrawer->MapMarkerFromTo( VERTEXMARKER, X, Y,
			DEFAULTMARKERSIZE,DEFAULTMARKERSIZE, 0.0 );
    }
  }  // end if IsIn is true         
}

#endif

Standard_Boolean Graphic2d_Circle::Pick (const Standard_ShortReal X,
					 const Standard_ShortReal Y,
					 const Standard_ShortReal aPrecision,
					 const Handle(Graphic2d_Drawer)& /*aDrawer*/) {

  Standard_Boolean found = Standard_False;
  Standard_ShortReal SRX = X, SRY = Y;

  if ( IsInMinMax( X, Y, aPrecision) ) {
    if ( myGOPtr->IsTransformed() ) {
       gp_GTrsf2d aTrsf = ( myGOPtr->Transform () ).Inverted ();
       Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
	   aTrsf.Transforms( RX, RY );
	   SRX = Standard_ShortReal( RX ); 
       SRY = Standard_ShortReal( RY );
    }
 #ifdef G002
     if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX, myY, aPrecision) ) {
        SetPickedIndex(-1);
        return Standard_True;
     } else {
       if ( myisArc ) {
        Standard_ShortReal x1 = Standard_ShortReal(myRadius *Cos( myFirstAngle ) + myX),
                           y1 = Standard_ShortReal(myRadius *Sin( myFirstAngle ) + myY),
                           x2 = Standard_ShortReal(myRadius *Cos( mySecondAngle ) + myX),
                           y2 = Standard_ShortReal(myRadius *Sin( mySecondAngle ) + myY);

        if ( Graphic2d_Primitive::IsOn( SRX, SRY, x1, y1, aPrecision) ) {
            SetPickedIndex(-2);
            return Standard_True;
        } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, x2, y2, aPrecision) ) {
            SetPickedIndex(-3);
            return Standard_True;
        }
       }
       
       Standard_ShortReal alpha = Abs( mySecondAngle - myFirstAngle );
       TShort_Array1OfShortReal Xpoint(1,MAXPOINTS+1); 
       TShort_Array1OfShortReal Ypoint(1,MAXPOINTS+1); 

       Standard_ShortReal teta = alpha / MAXPOINTS;
    
       for ( Standard_Integer i = 1; i<= MAXPOINTS+1; i++) {
	      Xpoint(i) = Standard_ShortReal(myX + myRadius * Cos(myFirstAngle + teta*(i-1)));
	      Ypoint(i) = Standard_ShortReal(myY + myRadius * Sin(myFirstAngle + teta*(i-1)));
          if ( Graphic2d_Primitive::IsOn( SRX, SRY, Xpoint(i), Ypoint(i), aPrecision) ) {
            SetPickedIndex(i);
            return Standard_True;
          }
       }
       if ( myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY ) 
           found = Abs( Sqrt((myX - SRX)*(myX - SRX) + (myY - SRY)*(myY - SRY))) 
	         < myRadius;
       if ( !found ) 
	       found = Abs( Sqrt( (myX - SRX)*(myX - SRX) + (myY - SRY)*(myY - SRY))
          - myRadius ) < aPrecision;
       if ( found ) 
            SetPickedIndex( 0 );
     }
 #else	 
	  if ( myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY )
        found = Abs( Sqrt((myX - SRX)*(myX - SRX) + (myY - SRY)*(myY - SRY))) 
	         < myRadius;
      
      if ( !found ) 
	    found = Abs( Sqrt( (myX - SRX)*(myX - SRX) + (myY - SRY)*(myY - SRY))
          - myRadius ) < aPrecision;   
 #endif
  }	 
 
  return found;
}

void Graphic2d_Circle::DoMinMax() {
    
 	myMinX = myX - myRadius; myMinY = myY - myRadius;
	myMaxX = myX + myRadius; myMaxY = myY + myRadius;
   
}

void Graphic2d_Circle::Save(Aspect_FStream& aFStream) const
{
	*aFStream << "Graphic2d_Circle" << endl;
	*aFStream << myX << ' ' << myY << endl;
	*aFStream << myRadius << endl;
	*aFStream << myisArc << endl;
	if (myisArc)
		*aFStream << myFirstAngle << ' ' << mySecondAngle << endl;
	Graphic2d_Line::Save(aFStream);
}

void Graphic2d_Circle::Retrieve(Aspect_IFStream& anIFStream,
			const Handle(Graphic2d_GraphicObject)& aGraphicObject)
{
	Quantity_Length X, Y, Rad;
	Quantity_PlaneAngle Ang1, Ang2;
	int isArc;
	Handle(Graphic2d_Circle) theCir;

	*anIFStream >> X >> Y;
	*anIFStream >> Rad;
	*anIFStream >> isArc;
	if (isArc)
	{
		*anIFStream >> Ang1 >> Ang2;
		theCir = new Graphic2d_Circle(aGraphicObject, X, Y, Rad, Ang1, Ang2);
	}
	else
		theCir = new Graphic2d_Circle(aGraphicObject, X, Y, Rad);
	((Handle (Graphic2d_Line))theCir)->Retrieve(anIFStream);
}

