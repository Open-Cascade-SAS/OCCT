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

#include <Prs2d_Axis.ixx>
#include <TCollection_ExtendedString.hxx>

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0
#define XROTATE(x,y) ((x)*cosa - (y)*sina)
#define YROTATE(x,y) ((y)*cosa + (x)*sina)
#define ADDPIX 17

Prs2d_Axis::Prs2d_Axis( const Handle(Graphic2d_GraphicObject)& aGO,
                        const gp_Ax22d& anAx,
                        const Standard_Real aLength,
                        const Standard_Real anArrAngle,
                        const Standard_Real anArrLength,
                        const Prs2d_TypeOfArrow anArrType,
                        const Standard_Real aTxtScale ) 
   :
       Graphic2d_Line( aGO ),
       myXVertX( 1, 3 ),
       myYVertX( 1, 3 ),
       myXVertY( 1, 3 ),
       myYVertY( 1, 3 ),
       myArrType( anArrType ),
       myisXY( Standard_True ),
       myTextScale( aTxtScale )
	
{
   Standard_Real X0, Y0, X1, Y1, X2, Y2;

   anAx.Location().Coord( X0, Y0 );
   anAx.XDirection().Coord( X1, Y1 );
   anAx.YDirection().Coord( X2, Y2 );

   X1 = X0 + X1 * aLength;
   Y1 = Y0 + Y1 * aLength;
   X2 = X0 + X2 * aLength;
   Y2 = Y0 + Y2 * aLength;

   myX0 = Standard_ShortReal( X0 );
   myY0 = Standard_ShortReal( Y0 );
   myX1 = Standard_ShortReal( X1 );
   myY1 = Standard_ShortReal( Y1 );
   myX2 = Standard_ShortReal( X2 );
   myY2 = Standard_ShortReal( Y2 );

   myMinX	= myX0;
   myMinY	= myY0;
   myMaxX	= myX0;
   myMaxY	= myY0;

   if ( myX1 < myMinX  ) myMinX = myX1;
   if ( myY1 < myMinY  ) myMinY = myY1;
   if ( myX1 > myMaxX  ) myMaxX = myX1;
   if ( myY1 > myMaxY  ) myMaxY = myY1;
   
   if ( myX2 < myMinX ) myMinX =  myX2;
   if ( myY2 < myMinY ) myMinY =  myY2;  
   if ( myX2 > myMaxX ) myMaxX =  myX2;
   if ( myY2 > myMaxY ) myMaxY =  myY2;  

   Standard_Real ArrAngle = M_PI/180.*anArrAngle, theAngle;
   gp_Pnt2d theOrigine( 0., 0.), P1, P2, P3;
   gp_Vec2d VX( 1., 0. ), VDir;
  
   P1 = gp_Pnt2d( X1, Y1 );
   P2 = gp_Pnt2d( anArrLength,  anArrLength*Tan( ArrAngle/2. ) );
   P3 = gp_Pnt2d( anArrLength, -anArrLength*Tan( ArrAngle/2. ) );
  
   VDir = gp_Vec2d( gp_Pnt2d( X1, Y1 ), gp_Pnt2d( X0, Y0 ) );
   theAngle = VX.Angle( VDir );

   P2.Rotate( theOrigine, theAngle );
   P3.Rotate( theOrigine, theAngle );
  
   P2.Translate( gp_Vec2d( theOrigine, P1 ) );
   P3.Translate( gp_Vec2d( theOrigine, P1 ) );
  
   myXVertX(1) = Standard_ShortReal( P2.X() );
   myYVertX(1) = Standard_ShortReal( P2.Y() );
   myXVertX(2) = Standard_ShortReal( P1.X() );
   myYVertX(2) = Standard_ShortReal( P1.Y() );
   myXVertX(3) = Standard_ShortReal( P3.X() );
   myYVertX(3) = Standard_ShortReal( P3.Y() );

   Standard_Integer i;
   for ( i = 1; i <= 3; i++ ) {
      if ( myXVertX(i) < myMinX ) myMinX = myXVertX(i);
      if ( myYVertX(i) < myMinY ) myMinY = myYVertX(i);
      if ( myXVertX(i) > myMaxX ) myMaxX = myXVertX(i);
	  if ( myYVertX(i) > myMaxY ) myMaxY = myYVertX(i);
   }

   P1 = gp_Pnt2d( X2, Y2 );;
   P2 = gp_Pnt2d( anArrLength,  anArrLength*Tan( ArrAngle/2. ) );
   P3 = gp_Pnt2d( anArrLength, -anArrLength*Tan( ArrAngle/2. ) );
  
   VDir = gp_Vec2d( gp_Pnt2d( X2, Y2 ), gp_Pnt2d( X0, Y0 ) );
   theAngle = VX.Angle( VDir );

   P2.Rotate( theOrigine, theAngle );
   P3.Rotate( theOrigine, theAngle );
  
   P2.Translate( gp_Vec2d( theOrigine, P1 ) );
   P3.Translate( gp_Vec2d( theOrigine, P1 ) );
  
   myXVertY(1) = Standard_ShortReal( P2.X() );
   myYVertY(1) = Standard_ShortReal( P2.Y() );
   myXVertY(2) = Standard_ShortReal( P1.X() );
   myYVertY(2) = Standard_ShortReal( P1.Y() );
   myXVertY(3) = Standard_ShortReal( P3.X() );
   myYVertY(3) = Standard_ShortReal( P3.Y() );

   for ( i = 1; i <= 3; i++ ) {
	  
	   if ( myXVertY(i) < myMinX ) myMinX = myXVertY(i);
       if ( myYVertY(i) < myMinY ) myMinY = myYVertY(i);
       if ( myXVertY(i) > myMaxX ) myMaxX = myXVertY(i);
	   if ( myYVertY(i) > myMaxY ) myMaxY = myYVertY(i);
	   
   }

  myNumOfElem = 6;
  myNumOfVert = 3;

}

Prs2d_Axis::Prs2d_Axis( const Handle(Graphic2d_GraphicObject)& aGO,
                        const gp_Ax2d& anAx,
                        const Standard_Real aLength,
                        const Standard_Real anArrAngle,
                        const Standard_Real anArrLength,
                        const Prs2d_TypeOfArrow anArrType,
                        const Standard_Real aTxtScale ) 
  :
    Graphic2d_Line( aGO ),
    
       myX2( 0. ),
       myY2( 0. ),
       myXVertX( 1, 3 ),
       myYVertX( 1, 3 ),
       myXVertY( 1, 3 ),
       myYVertY( 1, 3 ),
       myArrType( anArrType ),
       myisXY( Standard_False ),
       myTextScale( aTxtScale )
{
   Standard_Real X0, Y0, X1, Y1;

   anAx.Location().Coord( X0, Y0 );
   anAx.Direction().Coord( X1, Y1 );

   X1 = X0 + X1 * aLength;
   Y1 = Y0 + Y1 * aLength;

   myX0 = Standard_ShortReal( X0 );
   myY0 = Standard_ShortReal( Y0 );
   myX1 = Standard_ShortReal( X1 );
   myY1 = Standard_ShortReal( Y1 );

   myMinX	= myX0;
   myMinY	= myY0;
   myMaxX	= myX0;
   myMaxY	= myY0;

   if ( myX1 < myMinX  ) myMinX = myX1;
   if ( myY1 < myMinY  ) myMinY = myY1;
   if ( myX1 > myMaxX  ) myMaxX = myX1;
   if ( myY1 > myMaxY  ) myMaxY = myY1;

   Standard_Real ArrAngle = M_PI/180.*anArrAngle, theAngle;
   gp_Pnt2d theOrigine( 0., 0.), P1, P2, P3;
   gp_Vec2d VX( 1., 0. ), VDir;
  
   P1 = gp_Pnt2d( X0, Y0 );
   P2 = gp_Pnt2d( anArrLength,  anArrLength*Tan( ArrAngle/2. ) );
   P3 = gp_Pnt2d( anArrLength, -anArrLength*Tan( ArrAngle/2. ) );
  
   VDir = gp_Vec2d( gp_Pnt2d( X1, Y1 ), gp_Pnt2d( X0, Y0 ) );
   theAngle = VX.Angle( VDir );

   P2.Rotate( theOrigine, theAngle );
   P3.Rotate( theOrigine, theAngle );
  
   P2.Translate( gp_Vec2d( theOrigine, P1 ) );
   P3.Translate( gp_Vec2d( theOrigine, P1 ) );
  
   myXVertX(1) = Standard_ShortReal( P2.X() );
   myYVertX(1) = Standard_ShortReal( P2.Y() );
   myXVertX(2) = Standard_ShortReal( P1.X() );
   myYVertX(2) = Standard_ShortReal( P1.Y() );
   myXVertX(3) = Standard_ShortReal( P3.X() );
   myYVertX(3) = Standard_ShortReal( P3.Y() );

   for ( Standard_Integer i = 1; i <= 3; i++ ) {
	   if ( myXVertX(i) < myMinX ) myMinX = myXVertX(i);
       if ( myYVertX(i) < myMinY ) myMinY = myYVertX(i);
       if ( myXVertX(i) > myMaxX ) myMaxX = myXVertX(i);
	   if ( myYVertX(i) > myMaxY ) myMaxY = myYVertX(i);
   }
  myNumOfElem = 6;
  myNumOfVert = 3;

}

Prs2d_Axis::Prs2d_Axis( const Handle(Graphic2d_GraphicObject)& aGO,
                        const gp_Lin2d& aLine,
                        const Standard_Real aLength,
                        const Standard_Real anArrAngle,
                        const Standard_Real anArrLength,
                        const Prs2d_TypeOfArrow anArrType,
                        const Standard_Real aTxtScale ) 
  :
    Graphic2d_Line( aGO ),
    
       myX2( 0. ),
       myY2( 0. ),
       myXVertX( 1, 3 ),
       myYVertX( 1, 3 ),
       myXVertY( 1, 3 ),
       myYVertY( 1, 3 ),
       myArrType( anArrType ),
       myisXY( Standard_False ),
       myTextScale( aTxtScale )

{
   Standard_Real X0, Y0, X1, Y1;

   aLine.Location().Coord( X0, Y0 );
   aLine.Direction().Coord( X1, Y1 );

   X1 = X0 + X1 * aLength;
   Y1 = Y0 + Y1 * aLength;

   myX0 = Standard_ShortReal( X0 );
   myY0 = Standard_ShortReal( Y0 );
   myX1 = Standard_ShortReal( X1 );
   myY1 = Standard_ShortReal( Y1 );

   myMinX	= myX0;
   myMinY	= myY0;
   myMaxX	= myX0;
   myMaxY	= myY0;

   if ( myX1 < myMinX  ) myMinX = myX1;
   if ( myY1 < myMinY  ) myMinY = myY1;
   if ( myX1 > myMaxX  ) myMaxX = myX1;
   if ( myY1 > myMaxY  ) myMaxY = myY1;
   
   Standard_Real ArrAngle = M_PI/180.*anArrAngle, theAngle;
   gp_Pnt2d theOrigine( 0., 0.), P1, P2, P3;
   gp_Vec2d VX( 1., 0. ), VDir;
  
   P1 = gp_Pnt2d( X1, Y1 );
   P2 = gp_Pnt2d( anArrLength,  anArrLength*Tan( ArrAngle/2. ) );
   P3 = gp_Pnt2d( anArrLength, -anArrLength*Tan( ArrAngle/2. ) );
  
   VDir = gp_Vec2d( gp_Pnt2d( X1, Y1 ), gp_Pnt2d( X0, Y0 ) );
   theAngle = VX.Angle( VDir );

   P2.Rotate( theOrigine, theAngle );
   P3.Rotate( theOrigine, theAngle );
  
   P2.Translate( gp_Vec2d( theOrigine, P1 ) );
   P3.Translate( gp_Vec2d( theOrigine, P1 ) );
  
   myXVertX(1) = Standard_ShortReal( P2.X() );
   myYVertX(1) = Standard_ShortReal( P2.Y() );
   myXVertX(2) = Standard_ShortReal( P1.X() );
   myYVertX(2) = Standard_ShortReal( P1.Y() );
   myXVertX(3) = Standard_ShortReal( P3.X() );
   myYVertX(3) = Standard_ShortReal( P3.Y() );

   for ( Standard_Integer i = 1; i <= 3; i++ ) {
	 if ( myXVertX(i) < myMinX ) myMinX = myXVertX(i);
     if ( myYVertX(i) < myMinY ) myMinY = myYVertX(i);
     if ( myXVertX(i) > myMaxX ) myMaxX = myXVertX(i);
	 if ( myYVertX(i) > myMaxY ) myMaxY = myYVertX(i);
   }
  myNumOfElem = 6;
  myNumOfVert = 3;

}

void Prs2d_Axis::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {
 
  Standard_Boolean IsIn = Standard_False;

  TCollection_ExtendedString txtX("X");
  TCollection_ExtendedString txtY("Y");

  if ( !myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }

  if ( IsIn ) {

    DrawLineAttrib( aDrawer );
    Standard_ShortReal dxx = aDrawer->Convert(17);
    aDrawer->SetTextAttrib( myColorIndex, 0, 0, Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                            Standard_ShortReal(myTextScale*aDrawer->Scale()), Standard_False );
    Standard_ShortReal a = myX0, b = myY0,
                       c = myX1, d = myY1,
                       e = myX2, f = myY2,
                       t1 = myX1, t2 = myY1 + dxx,
                       tt1 = myX2 + dxx, tt2 = myY2;
    TShort_Array1OfShortReal XpointX( 1, 3 ), YpointX( 1, 3 ),
                             XpointY( 1, 3 ), YpointY( 1, 3 );

    if ( myGOPtr->IsTransformed () ) {
       gp_GTrsf2d aTrsf = myGOPtr->Transform ();
       Standard_Real a1, b1, a2, b2;
       for ( Standard_Integer j = 1; j <= 3; j++ ) {
	      a1 = myXVertX(j);
	      b1 = myYVertX(j);
          aTrsf.Transforms(a1, b1);
          XpointX(j) = Standard_ShortReal(a1);
	      YpointX(j) = Standard_ShortReal(b1);
          if ( myisXY ) {
            a2 = myXVertY(j);
	        b2 = myYVertY(j);
            aTrsf.Transforms( a2, b2 );
            XpointY(j) = Standard_ShortReal(a2);
            YpointY(j) = Standard_ShortReal(b2);
          }
        }
      
       Standard_Real A, B, C, D, T1, T2;
       A = a;
	   B = b;
	   C = c;
	   D = d;
       T1 = t1;
       T2 = t2;
	   aTrsf.Transforms( A, B );
	   aTrsf.Transforms( C, D );
       aTrsf.Transforms( T1, T2 );
	   a = Standard_ShortReal(A);
	   b = Standard_ShortReal(B);
	   c = Standard_ShortReal(C);
	   d = Standard_ShortReal(D);
       t1 = Standard_ShortReal(T1);
	   t2 = Standard_ShortReal(T2);
       
       if ( myisXY ) {
         Standard_Real E, F, TT1, TT2;
         E = e;
	     F = f;
         TT1 = tt1;
         TT2 = tt2;
	     aTrsf.Transforms( E, F );
         aTrsf.Transforms( TT1, TT2 );
	     e = Standard_ShortReal(E);
	     f = Standard_ShortReal(F);
         tt1 = Standard_ShortReal(TT1);
	     tt2 = Standard_ShortReal(TT2);
       }
    } else {
       XpointX.Assign( myXVertX );
       YpointX.Assign( myYVertX );
       if ( myisXY ) {
         XpointY.Assign( myXVertY );
         YpointY.Assign( myYVertY );
       }
    }  // end if GO is transformed

    aDrawer->MapSegmentFromTo( a, b, c, d );
    aDrawer->MapTextFromTo( txtX, t1, t2, 0., 0., 0., Aspect_TOT_SOLID );
    
    if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED )
	   aDrawer->MapPolygonFromTo( XpointX, YpointX );
     else 
       aDrawer->MapPolylineFromTo( XpointX, YpointX );
    
    if ( myisXY ) { 
        aDrawer->MapSegmentFromTo( a, b, e, f );
        aDrawer->MapTextFromTo( txtY, tt1, tt2, 0., 0., 0., Aspect_TOT_SOLID );
        if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED )
	      aDrawer->MapPolygonFromTo( XpointY, YpointY );
        else 
          aDrawer->MapPolylineFromTo( XpointY, YpointY );
    
    }
  } // end if IsIn is true 

}

void Prs2d_Axis::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                              const Standard_Integer anIndex) {

  Standard_Boolean IsIn = Standard_False;

  if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }

  if ( IsIn ) {

    DrawLineAttrib( aDrawer );
  //  aDrawer->SetTextAttrib( myColorIndex, 0, 0, myTextScale*aDrawer->Scale(), 
  //                          myTextScale*aDrawer->Scale(), Standard_False );
    Standard_ShortReal dxx = aDrawer->Convert(17);    
    Standard_ShortReal a = myX0, b = myY0, 
                       c = myX1, d = myY1,
                       e = myX2, f = myY2,
                       t1 = myX1, t2 = myY1 + dxx, 
                       tt1 = myX2 + dxx, tt2 = myY2;
    TShort_Array1OfShortReal Xpoint( 1, 3 ), Ypoint( 1, 3 ); 

    if ( anIndex == 1 ) {
     Xpoint.Assign( myXVertX );
     Ypoint.Assign( myYVertX );
    } else if ( anIndex == 2 ) {
     Xpoint.Assign( myXVertY );
     Ypoint.Assign( myYVertY );    
    } 

    if ( myGOPtr->IsTransformed () ) {
      gp_GTrsf2d aTrsf = myGOPtr->Transform ();
      Standard_Real a1, b1, c1, d1, e1, f1, at1, at2, att1, att2;
      
      if ( anIndex == 1 ) {      
        for ( Standard_Integer j = 1; j <= 3; j++ ) {
		     a1 = Standard_Real( Xpoint(j) );
		     b1 = Standard_Real( Xpoint(j) );
    	     aTrsf.Transforms( a1, b1 );
             Xpoint(j) = Standard_ShortReal( a1 );
		     Ypoint(j) = Standard_ShortReal( b1 );
         }
      } else if ( anIndex == 2 ) {

        for ( Standard_Integer j = 1; j <= 3; j++ ) {
             a1 = Standard_Real( Xpoint(j) );
		     b1 = Standard_Real( Xpoint(j) );
	         aTrsf.Transforms( a1, b1 );
             Xpoint(j) = Standard_ShortReal( a1 );
		     Ypoint(j) = Standard_ShortReal( b1 );
         }
      } else if ( anIndex == 5 ) {

        a1 = Standard_Real( a );
        b1 = Standard_Real( b );
        c1 = Standard_Real( c );
        d1 = Standard_Real( d );
	    aTrsf.Transforms( a1, b1 );
        aTrsf.Transforms( c1, d1 );
	    a = Standard_ShortReal( a1 );
	    b = Standard_ShortReal( b1 );
	    c = Standard_ShortReal( c1 );
	    d = Standard_ShortReal( d1 );
	          
      } else if ( anIndex == 6 ) {
        a1 = Standard_Real( a );
        b1 = Standard_Real( b );
        e1 = Standard_Real( e );
        f1 = Standard_Real( f );
	    aTrsf.Transforms( a1, b1 );
        aTrsf.Transforms( e1, f1 );
        a = Standard_ShortReal( a1 );
        b = Standard_ShortReal( b1);
        e = Standard_ShortReal( e1 );
        f = Standard_ShortReal( f1);

      } else if ( anIndex == 3 ) {
        c1 = Standard_Real( c );
        d1 = Standard_Real( d );
        at1 = Standard_Real( t1 );
        at2 = Standard_Real( t2 );
        aTrsf.Transforms( c1, d1 );
        aTrsf.Transforms( at1, at2 );
        c = Standard_ShortReal( c1 );
        d = Standard_ShortReal( d1);
        t1 = Standard_ShortReal( at1 );
        t2 = Standard_ShortReal( at2);

      } else if ( anIndex == 4 ) {
        e1 = Standard_Real( e );
        f1 = Standard_Real( f );
        att1 = Standard_Real( tt1 );
        att2 = Standard_Real( tt2 );
        aTrsf.Transforms( e1, f1 );
        aTrsf.Transforms( att1, att2 );
        e = Standard_ShortReal( e1 );
        f = Standard_ShortReal( f1);
        tt1 = Standard_ShortReal( att1 );
        tt2 = Standard_ShortReal( att2);

      }

    } 

    if ( anIndex == 1 || anIndex == 2 ) 
      if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) {
	    aDrawer->MapPolygonFromTo( Xpoint, Ypoint );
      } else {
        aDrawer->MapPolylineFromTo( Xpoint, Ypoint );
      } 
    else if ( anIndex == 5 )
        aDrawer->MapSegmentFromTo( a, b, c, d );
    else if ( anIndex == 6 )
        aDrawer->MapSegmentFromTo( a, b, e, f );
    else if ( anIndex == 3 ) {
        TCollection_ExtendedString txtX("X");
        aDrawer->MapTextFromTo( txtX, t1, t2, 0., 0., 0., Aspect_TOT_SOLID );
    } else if ( anIndex == 4 ) {
        TCollection_ExtendedString txtY("Y");
        aDrawer->MapTextFromTo( txtY, tt1, tt2, 0., 0., 0., Aspect_TOT_SOLID );
    } 
  } // end if IsIn is true 

}

void Prs2d_Axis::DrawVertex (const Handle(Graphic2d_Drawer)& aDrawer,
                              const Standard_Integer anIndex) {
 
    Standard_Boolean IsIn = Standard_False;

 if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
 else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
 }
 if ( IsIn ) {
  if ( anIndex == 1 || anIndex == 2 || anIndex == 3 ) {
   Standard_ShortReal X=0., Y=0.;
   if ( anIndex == 1 ) {
     X = myX0; Y = myY0;
   } else if ( anIndex == 2 ) {
     X = myX1; Y = myY1;
   } else if ( anIndex == 3 ) {
     X = myX2; Y = myY2;
   }

   DrawMarkerAttrib( aDrawer );
   if ( myGOPtr->IsTransformed() ) {
     gp_GTrsf2d aTrsf = myGOPtr->Transform();
     Standard_Real A = Standard_Real( X ), B = Standard_Real( Y );
     aTrsf.Transforms( A, B );
     X = Standard_ShortReal( A );
     Y = Standard_ShortReal( B );
   } 
     aDrawer->MapMarkerFromTo(VERTEXMARKER,X,Y, DEFAULTMARKERSIZE,DEFAULTMARKERSIZE,0.0);
  }
 }
}

Standard_Boolean Prs2d_Axis::Pick( const Standard_ShortReal X,
                                   const Standard_ShortReal Y,
                                   const Standard_ShortReal aPrecision,
                                   const Handle(Graphic2d_Drawer)& aDrawer )  {
   Standard_ShortReal SRX = X, SRY = Y;
   Standard_Boolean Result = Standard_False;
   
   if ( IsInMinMax (X, Y, aPrecision) ) {
      if ( myGOPtr->IsTransformed () ) {
         gp_GTrsf2d aTrsf = ( myGOPtr->Transform() ).Inverted();
         Standard_Real RX = Standard_Real(SRX), RY = Standard_Real(SRY);
		 aTrsf.Transforms( RX, RY );
		 SRX = Standard_ShortReal( RX );
		 SRY = Standard_ShortReal( RY );
      }

      if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX0, myY0, aPrecision ) ) {
         SetPickedIndex(-1);    
         return Standard_True;
      } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX1, myY1, aPrecision ) ) {
         SetPickedIndex(-2);    
         return Standard_True;
      } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX2, myY2, aPrecision ) ) {
         SetPickedIndex(-3);    
         return Standard_True;
      }

      for ( Standard_Integer i = 1; i < 3 ; i++) {
	    if ( IsOn( SRX, SRY, myXVertX(i), myYVertX(i), myXVertX(i+1), myYVertX( i+1 ), aPrecision ) ) {
	      SetPickedIndex(1);
	      return Standard_True;
	    }
        if ( myisXY ) 
         if ( IsOn( SRX, SRY, myXVertY(i), myYVertY(i), myXVertY(i+1), myYVertY( i+1 ), aPrecision ) ) {
	       SetPickedIndex(2);
	       return Standard_True;
         } 
        
       } // end for
	
 	  if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) 
	   if ( IsIn( SRX, SRY,  myXVertX, myYVertX, aPrecision ) ) {
	       SetPickedIndex(1);
	       return Standard_True;
       }
          
       if ( myisXY ) 
        if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) 
	     if ( IsIn( SRX, SRY,  myXVertY, myYVertY, aPrecision ) ) {
	       SetPickedIndex(2);
	       return Standard_True;
         }
    Standard_ShortReal width,height,xoffset,yoffset;
    Standard_ShortReal hscale = Standard_ShortReal(myTextScale*aDrawer->Scale()),
                       wscale = Standard_ShortReal(myTextScale*aDrawer->Scale());
    Standard_ShortReal TX = X, TY = Y;
    
    aDrawer->SetTextAttrib( myColorIndex, 0, 0, hscale, wscale, Standard_False );
    TCollection_ExtendedString txt("X");
    if ( !aDrawer->GetTextSize( txt, width, height, xoffset, yoffset ) ) {
      width = height = xoffset = yoffset = 0.;
    }
    Standard_ShortReal dxx = aDrawer->Convert(ADDPIX);
    Standard_ShortReal cosa = Standard_ShortReal(Cos( 0. )),
                       sina = Standard_ShortReal(Sin( 0. )),
                       dx = TX- myX1,
                       dy = TY- myY1 - dxx, 
                       x = XROTATE(dx,dy),
                       y = YROTATE(dx,dy);

    Result  =   (x >= -aPrecision + xoffset)            
            &&  (x <= width + xoffset + aPrecision)
            &&  (y >= -yoffset - aPrecision) 
            &&  (y <= height - yoffset + aPrecision);
  
    if ( Result ) {
      SetPickedIndex(3);
      return Standard_True;
   }
   txt = TCollection_ExtendedString("Y");
   if ( !aDrawer->GetTextSize( txt, width, height, xoffset, yoffset ) ) {
      width = height = xoffset = yoffset = 0.;
   }
    
   dx = TX - myX2 - dxx;
   dy = TY - myY2, 
   x = XROTATE(dx,dy),
   y = YROTATE(dx,dy);

   Result   =   (x >= -aPrecision + xoffset)            
            &&  (x <= width + xoffset + aPrecision)
            &&  (y >= -yoffset - aPrecision) 
            &&  (y <= height - yoffset + aPrecision);
  
  if ( Result ) {
    SetPickedIndex( 4 );
    return Standard_True;
  }

  if ( IsOn( SRX, SRY, myX0, myY0, myX1, myY1, aPrecision ) ) {
    SetPickedIndex(5);
    return Standard_True;
  }
  if ( myisXY )
      if ( IsOn( SRX, SRY, myX0, myY0, myX2, myY2, aPrecision ) ) {
          SetPickedIndex(6);
          return Standard_True;
      }

 }// end if IsInMinMax is true

 return Standard_False;

}

void Prs2d_Axis::Save(Aspect_FStream& aFStream) const
{
}

Prs2d_TypeOfArrow Prs2d_Axis::TypeOfArrow( ) const
{
  return myArrType;
}

const Handle(TColgp_HArray1OfPnt2d)  Prs2d_Axis::ArrayOfPnt2d( ) const
{
   Handle(TColgp_HArray1OfPnt2d) points = new TColgp_HArray1OfPnt2d( 1, 3 );
		
   points->SetValue( 1, gp_Pnt2d( myX1, myY1 ) );
   points->SetValue( 2, gp_Pnt2d( myX0, myY0 ) );
   points->SetValue( 3, gp_Pnt2d( myX2, myY2 ) );
   return points;
}

const Handle(TColgp_HArray1OfPnt2d)  Prs2d_Axis::ArrayOfXArrowPnt2d( ) const
{
   Handle(TColgp_HArray1OfPnt2d) points = new TColgp_HArray1OfPnt2d( 1, 3 );
		
   for ( Standard_Integer i = 1; i <= 3; i++ ) 
   {
     points->SetValue( i, gp_Pnt2d( myXVertX(i), myYVertX(i) ) );
   }   
   return points;
}

const Handle(TColgp_HArray1OfPnt2d)  Prs2d_Axis::ArrayOfYArrowPnt2d( ) const
{
   Handle(TColgp_HArray1OfPnt2d) points = new TColgp_HArray1OfPnt2d( 1, 3 );
		
   for ( Standard_Integer i = 1; i <= 3; i++ ) 
   {
     points->SetValue( i, gp_Pnt2d( myXVertY(i), myYVertY(i) ) );
   }   
   return points;
}

Standard_Real Prs2d_Axis::TextScale( ) const
{
  return myTextScale;
}


