/*=====================================================================

     FONCTION :
     ----------
        Classe Graphic2d_SetOfPolylines

     TEST :
     ------

        Voir TestG2D/TestG21



=====================================================================*/

// PRO13369	//GG_280498
//			MapPolylineFromTo() should be called with exact number of points

#define G002    //GG_140400 Use SetPickedIndex method 

#include <Graphic2d_SetOfPolylines.ixx>
#include <Graphic2d_Vertex.hxx>
#include <Graphic2d_HSequenceOfVertex.hxx>

Graphic2d_SetOfPolylines::Graphic2d_SetOfPolylines (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject)

  : Graphic2d_Line (aGraphicObject) {

}

void Graphic2d_SetOfPolylines::Add( const Standard_Real X, 
                                    const Standard_Real Y,
	                                const Standard_Boolean NewPolyline ) {
  Standard_Integer n = myPolylines.Length();
  Standard_ShortReal x = Standard_ShortReal( X ),
                     y = Standard_ShortReal( Y );
  Graphic2d_Vertex V( x, y );

	if( NewPolyline || (n == 0) ) {
	  Handle(Graphic2d_HSequenceOfVertex) P = 
					new Graphic2d_HSequenceOfVertex();
	  P->Append(V);
	  myPolylines.Append(P);
	} else {
	  Handle(Graphic2d_HSequenceOfVertex) P = myPolylines.Last();
	  if( !V.IsEqual(P->Value(P->Length())) ) {
	    P->Append(V);
	  }
	}

	myMinX	= Min(myMinX,x);
	myMinY	= Min(myMinY,y);
	myMaxX	= Max(myMaxX,x);
	myMaxY	= Max(myMaxY,y);
}

void Graphic2d_SetOfPolylines::Add( const Standard_Real X1, 
                                    const Standard_Real Y1,
	                                const Standard_Real X2, 
                                    const Standard_Real Y2 ) {

  Standard_ShortReal x1 = Standard_ShortReal( X1 ),
                     y1 = Standard_ShortReal( Y1 ),
                     x2 = Standard_ShortReal( X2 ),
                     y2 = Standard_ShortReal( Y2 );
  Graphic2d_Vertex V1( x1, y1 ),
                   V2( x2, y2 );
  Standard_Integer np = myPolylines.Length();

	if( !V1.IsEqual(V2) ) {
	  if( np == 0 ) {
	    Handle(Graphic2d_HSequenceOfVertex) P = 
					new Graphic2d_HSequenceOfVertex();
	    if( (x1 > x2) || (y1 > y2) ) {	
	      V1.SetCoord(x2,y2);	// seg orientation
	      V2.SetCoord(x1,y1);
	    }
	    P->Append(V1); P->Append(V2);
	    myPolylines.Append(P);
	  } else {		// Try to build a polyline 
				// Warning,take an aspirine before continue reading...
	    Handle(Graphic2d_HSequenceOfVertex) P,PP;
	    Standard_Integer ip,ipp,lv;
	    for( ip=np ; ip>0 ; ip-- ) {
	      P = myPolylines.Value(ip);//ref line to check
	      lv = P->Length();		//number of vertex in the ref line
	      if( V1.IsEqual(P->Value(lv)) ) {
		//first vertex of segment is the same that the last vertex of the
		//ref line
		if( ip > 1 ) { 	//Try to concatenate ref line with others
		  for( ipp=1 ; ipp<ip ; ipp++ ) {
	      	    PP = myPolylines.Value(ipp);	//other polyline 
		    if( V2.IsEqual(PP->Value(1)) ) {
		      PP->Prepend(P); 		// Move the ref line before this
		      myPolylines.Remove(ip);	// and destroy the ref line
		      ip = -1; break;
		    } else if( V2.IsEqual(PP->Value(PP->Length())) ) {
		      P->Reverse();
		      PP->Append(P); 		// Reverse & Move the ref line after this
		      myPolylines.Remove(ip);	// and destroy the ref line
		      ip = -1; break;
		    }
		  }
		}
	        if( ip > 0 ) {
		  P->Append(V2);		// Append new vertex to ref line
		  ip = -1;
		}
	      } else if( V2.IsEqual(P->Value(lv)) ) {
		//second vertex of segment is the same that the last vertex of the
		//ref line
		if( ip > 1 ) { 	//Try to concatenate ref line with others
		  for( ipp=1 ; ipp<ip ; ipp++ ) {
	      	    PP = myPolylines.Value(ipp);	//other polyline 
		    if( V1.IsEqual(PP->Value(1)) ) {
		      PP->Prepend(P); 		// Move the ref line before this
		      myPolylines.Remove(ip);	// and destroy the ref line
		      ip = -1; break;
		    } else if( V1.IsEqual(PP->Value(PP->Length())) ) {
		      P->Reverse();
		      PP->Append(P); 		// Reverse & Move the ref line after this
		      myPolylines.Remove(ip);	// and destroy the ref line
		      ip = -1; break;
		    }
		  }
		}
	        if( ip > 0 ) {
		  P->Append(V1);		// Append new vertex to ref line
		  ip = -1;
		}
	      } else if( V1.IsEqual(P->Value(1)) ) {
		//first vertex of segment is the same that the first vertex of the
		//ref line
		if( ip > 1 ) { 	//Try to concatenate ref line with others
		  for( ipp=1 ; ipp<ip ; ipp++ ) {
	      	    PP = myPolylines.Value(ipp);	//other polyline 
		    if( V2.IsEqual(PP->Value(PP->Length())) ) {
		      P->Prepend(PP); 		// Move this line before the ref line
		      myPolylines.Remove(ipp);	// and destroy this line
		      ip = -1; break;
		    } else if( V2.IsEqual(PP->Value(1)) ) {
		      PP->Reverse();
		      P->Prepend(PP); 		// Reverse & Move this line before the ref line 
		      myPolylines.Remove(ipp);	// and destroy this line
		      ip = -1; break;
		    }
		  }
		}
	        if( ip > 0 ) {
		  P->Prepend(V2);		// Prepend new vertex to ref line
		  ip = -1;
		}
	      } else if( V2.IsEqual(P->Value(1)) ) {
		//second vertex of segment is the same that the first vertex of the
		//ref line
		if( ip > 1 ) { 	//Try to concatenate ref line with others
		  for( ipp=1 ; ipp<ip ; ipp++ ) {
	      	    PP = myPolylines.Value(ipp);	//other polyline 
		    if( V1.IsEqual(PP->Value(PP->Length())) ) {
		      P->Prepend(PP); 		// Move this line before the ref line
		      myPolylines.Remove(ipp);	// and destroy this line
		      ip = -1; break;
		    } else if( V1.IsEqual(PP->Value(1)) ) {
		      PP->Reverse();
		      P->Prepend(PP); 		// Reverse & Move this line before the ref line
		      myPolylines.Remove(ipp);	// and destroy this line
		      ip = -1; break;
		    }
		  }
		}
	        if( ip > 0 ) {
		  P->Prepend(V1);		// Append new vertex to ref line
		  ip = -1;
		}
	      }
	    }
				//create new line
	    if( ip >= 0 ) {
	      Handle(Graphic2d_HSequenceOfVertex) P = 
					new Graphic2d_HSequenceOfVertex();
	      if( (x1 > x2) || (y1 > y2) ) {	
	        V1.SetCoord(x2,y2);	// seg orientation
	        V2.SetCoord(x1,y1);
	      }
	      P->Append(V1);
	      P->Append(V2);
	      myPolylines.Append(P);
	    }
	  }

	  myMinX = Min(myMinX,Min(x1,x2));
	  myMinY = Min(myMinY,Min(y1,y2));
	  myMaxX = Max(myMaxX,Max(x1,x2));
	  myMaxY = Max(myMaxY,Max(y1,y2));
	}
}

Standard_Integer Graphic2d_SetOfPolylines::Length () const {
   return myPolylines.Length();
}

Standard_Integer Graphic2d_SetOfPolylines::Length (const Standard_Integer aPrank) const {
    if( aPrank < 1 || aPrank > Length() ) 
                Standard_OutOfRange::Raise
			("the polyline rank is out of bounds in the set");
    return myPolylines.Value(aPrank)->Length();
}

void Graphic2d_SetOfPolylines::Values( const Standard_Integer aPrank,
				                       const Standard_Integer aVrank,
				                       Standard_Real &X,
                                       Standard_Real &Y ) const {

    if( aPrank < 1 || aPrank > Length() ) 
                Standard_OutOfRange::Raise
			("the polyline rank is out of bounds in the set");
    if( aVrank < 1 || aVrank > Length(aPrank) ) 
                Standard_OutOfRange::Raise
			("the point rank is out of bounds in the polyline");

    Graphic2d_Vertex V = myPolylines.Value(aPrank)->Value(aVrank);
    X = V.X(); Y = V.Y();
}

void Graphic2d_SetOfPolylines::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {
Standard_Integer np = myPolylines.Length();
Standard_Boolean IsIn = Standard_False,transform = Standard_False;

  if( np <= 0 ) return;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    transform = Standard_True;
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if ( IsIn ) {
    static gp_GTrsf2d theTrsf;
    Handle(Graphic2d_HSequenceOfVertex) P;
    Standard_Real A, B;
    Standard_ShortReal x1,y1,x2,y2;
    Standard_Integer ip,lp,iv;
    DrawLineAttrib(aDrawer);
    if( transform ) theTrsf = myGOPtr->Transform ();
    for( ip=1 ; ip<=np ; ip++ ) { 
      P = myPolylines.Value(ip);
      lp = P->Length();
      if( lp > 2 ) {
	P->Value(1).Coord(A,B);
	if( transform ) theTrsf.Transforms(A,B); 
	x1 = Standard_ShortReal( A ); 
    y1 = Standard_ShortReal( B );
	aDrawer->MapPolylineFromTo(x1,y1,lp); 
	for( iv=2 ; iv<lp ; iv++ ) {
	  P->Value(iv).Coord(A,B);
	  if( transform ) theTrsf.Transforms(A,B); 
	  x1 = Standard_ShortReal( A ); 
      y1 = Standard_ShortReal( B );
	  aDrawer->MapPolylineFromTo(x1,y1,0); 
	}
	P->Value(lp).Coord(A,B);
	if( transform ) theTrsf.Transforms(A,B); 
	x1 = Standard_ShortReal( A ); 
    y1 = Standard_ShortReal( B );
	aDrawer->MapPolylineFromTo(x1,y1,-1); 
      } else if( lp > 1 ) {
	P->Value(1).Coord(A,B);
	if( transform ) theTrsf.Transforms(A,B); 
	x1 = Standard_ShortReal( A ); 
    y1 = Standard_ShortReal( B );
	P->Value(2).Coord(A,B);
	if( transform ) theTrsf.Transforms(A,B); 
	x2 = Standard_ShortReal( A ); 
    y2 = Standard_ShortReal( B );
    aDrawer->MapSegmentFromTo(x1,y1,x2,y2);
      }
    }
  }
}

Standard_Boolean Graphic2d_SetOfPolylines::Pick (const Standard_ShortReal X,
					  const Standard_ShortReal Y,
					  const Standard_ShortReal aPrecision,
					  const Handle(Graphic2d_Drawer)& aDrawer)
{
Standard_Integer np = myPolylines.Length();
Standard_ShortReal SRX = X, SRY = Y;

    if ( (np > 0) && IsInMinMax (X, Y, aPrecision)) {
      if (myGOPtr->IsTransformed ()) {
        gp_GTrsf2d theTrsf((myGOPtr->Transform ()).Inverted ());
        Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
        theTrsf.Transforms (RX, RY);
        SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
      }

      Handle(Graphic2d_HSequenceOfVertex) P;
      Standard_Integer ip,iv=0,lp;
      Standard_ShortReal x1,y1,x2,y2;
      Standard_Real A,B;
      for( ip=1 ; ip<=np ; ip++ ) { 
        P = myPolylines.Value(ip);
        lp = P->Length();
        if( lp > 1 ) {
	     for( iv=1 ; iv<lp ; iv++ ) {
	       P->Value(iv).Coord(A,B);
	       x1 = Standard_ShortReal( A ); 
           y1 = Standard_ShortReal( B );
	       P->Value(iv+1).Coord(A,B);
	       x2 = Standard_ShortReal( A ); 
           y2 = Standard_ShortReal( B );
           if (IsOn (SRX, SRY, x1, y1, x2, y2, aPrecision) ) { 
#ifdef G002
	      SetPickedIndex((ip << 16) | iv);
#else
	      myPickedIndex = (ip << 16) | iv;
#endif
	      return Standard_True;
	    }
	  }
	}
      }
      return Standard_False;
    } 
    return Standard_False;
}

void Graphic2d_SetOfPolylines::Save(Aspect_FStream& aFStream) const
{
}
