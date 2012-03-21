// Created on: 1995-01-14
// Created by: GG
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


// Modified	23/02/98 : FMN ; Remplacement PI par Standard_PI
// JR 02.01.100 : Implicit conversions


//-Version	

//-Design	Declaration des variables specifiques aux Type de markers

//-Warning	Un style est definie, soit par son type predefini TOM_...
//		soit par sa description dans l'espace -1,+1

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Aspect_MarkerStyle.ixx>

//-Aliases

//-Global data definitions

//	MyMarkerType	 	: TypeOfMarker from Aspect;
//	MyXpoint		: Array1OfShortReal from TShort;
//	MyYpoint		: Array1OfShortReal from TShort;
//	MySpoint		: Array1OfBoolean from TColStd;

//-Constructors

//-Destructors

//-Methods, in order

Aspect_MarkerStyle::Aspect_MarkerStyle () : MyMarkerType(Aspect_TOM_POINT) { 
  SetPredefinedStyle();
}

Aspect_MarkerStyle::Aspect_MarkerStyle (
		const Aspect_TypeOfMarker aType) : MyMarkerType(aType) {
  SetPredefinedStyle();
}

Aspect_MarkerStyle::Aspect_MarkerStyle (const TColStd_Array1OfReal& aXpoint,
					const TColStd_Array1OfReal& aYpoint)
 		: MyMarkerType(Aspect_TOM_USERDEFINED) {
Standard_Integer i,j=1;

	MyXpoint = new TShort_HArray1OfShortReal(1,aXpoint.Length());
	MyYpoint = new TShort_HArray1OfShortReal(1,aXpoint.Length());
	MySpoint = new TColStd_HArray1OfBoolean(1,aXpoint.Length());

	if( aXpoint.Length() != aYpoint.Length() ) {
	  Aspect_MarkerStyleDefinitionError::Raise ("Bad Descriptor length") ;
	}

	for( i=aXpoint.Lower() ; i<=aXpoint.Upper() ; i++,j++ ) {
	  Standard_ShortReal X = (Standard_ShortReal ) aXpoint(i);
	  Standard_ShortReal Y = (Standard_ShortReal ) aYpoint(i);
	  if( X < -1. || X > 1. || Y < -1. || Y > 1. ) {
	    Aspect_MarkerStyleDefinitionError::Raise ("Bad Descriptor value") ;
	  }
	  MyXpoint->SetValue(j,X);
	  MyYpoint->SetValue(j,Y);
	  MySpoint->SetValue(j,(j > 1) ? Standard_True : Standard_False);
	}
}

Aspect_MarkerStyle::Aspect_MarkerStyle (const TColStd_Array1OfReal& aXpoint,
					const TColStd_Array1OfReal& aYpoint,
					const TColStd_Array1OfBoolean& aSpoint)
 		: MyMarkerType(Aspect_TOM_USERDEFINED) {
Standard_Integer i,j=1;

	MyXpoint = new TShort_HArray1OfShortReal(1,aXpoint.Length());
	MyYpoint = new TShort_HArray1OfShortReal(1,aXpoint.Length());
	MySpoint = new TColStd_HArray1OfBoolean(1,aXpoint.Length());

	if( (aXpoint.Length() != aYpoint.Length()) ||
	    	(aXpoint.Length() != aSpoint.Length()) ) {
	  Aspect_MarkerStyleDefinitionError::Raise ("Bad Descriptor length") ;
	}

	for( i=aXpoint.Lower() ; i<=aXpoint.Upper() ; i++,j++ ) {
	  Standard_ShortReal X = (Standard_ShortReal ) aXpoint(i);
	  Standard_ShortReal Y = (Standard_ShortReal ) aYpoint(i);
	  Standard_Boolean S = aSpoint(i);
	  if( X < -1. || X > 1. || Y < -1. || Y > 1. ) {
	    Aspect_MarkerStyleDefinitionError::Raise ("Bad Descriptor value") ;
	  }
	  MyXpoint->SetValue(j,X);
	  MyYpoint->SetValue(j,Y);
	  MySpoint->SetValue(j,S);
	  MySpoint->SetValue(j,(j > 1) ? S : Standard_False);
	}
}

Aspect_MarkerStyle& Aspect_MarkerStyle::Assign (const Aspect_MarkerStyle& Other) {

        MyMarkerType = Other.MyMarkerType ;
        MyXpoint = Other.MyXpoint ;
        MyYpoint = Other.MyYpoint ;
        MySpoint = Other.MySpoint ;

        return (*this);

}

Aspect_TypeOfMarker Aspect_MarkerStyle::Type () const {

	return MyMarkerType;
}

Standard_Integer Aspect_MarkerStyle::Length () const {

	return MyXpoint->Length();
}

Standard_Boolean Aspect_MarkerStyle::Values (const Standard_Integer aRank,
				Standard_Real &X,Standard_Real &Y) const {

	if( aRank < 1 || aRank > Length() ) {
	  Aspect_MarkerStyleDefinitionError::Raise ("Bad Descriptor rank") ;
	}

	X = MyXpoint->Value(aRank);
	Y = MyYpoint->Value(aRank);
	return MySpoint->Value(aRank);
}

const TShort_Array1OfShortReal& Aspect_MarkerStyle::XValues () const {

	return MyXpoint->Array1();
}

const TShort_Array1OfShortReal& Aspect_MarkerStyle::YValues () const {

	return MyYpoint->Array1();
}

const TColStd_Array1OfBoolean& Aspect_MarkerStyle::SValues () const {

	return MySpoint->Array1();
}

#define MAX_O_POINT 12
#define MAX_BALL_LINE 12
#ifndef AIX
#define FALSE Standard_False
#define TRUE  Standard_True
#endif

void Aspect_MarkerStyle::SetPredefinedStyle() {

	switch ( MyMarkerType ) {
	    case Aspect_TOM_USERDEFINED :
		Aspect_MarkerStyleDefinitionError::Raise
			("Bad Marker Type Style");
		break;
	    case Aspect_TOM_POINT :
		MyXpoint = new TShort_HArray1OfShortReal(1,5) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,5) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,5) ;

		MyXpoint->SetValue(1,-1.); 
		MyYpoint->SetValue(1,-1.); 
		MySpoint->SetValue(1,FALSE); 

		MyXpoint->SetValue(2,-1.);
		MyYpoint->SetValue(2,1.);
		MySpoint->SetValue(2,TRUE); 

		MyXpoint->SetValue(3,1.);
		MyYpoint->SetValue(3,1.);
		MySpoint->SetValue(3,TRUE); 

		MyXpoint->SetValue(4,1.);
		MyYpoint->SetValue(4,-1.);
		MySpoint->SetValue(4,TRUE); 

		MyXpoint->SetValue(5,-1.);
		MyYpoint->SetValue(5,-1.);
		MySpoint->SetValue(5,TRUE); 
		break ;
	    case Aspect_TOM_PLUS :
		MyXpoint = new TShort_HArray1OfShortReal(1,4) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,4) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,4) ;

		MyXpoint->SetValue(1, 0.);
		MyYpoint->SetValue(1,-1.);
		MySpoint->SetValue(1,FALSE); 

		MyXpoint->SetValue(2, 0.);
		MyYpoint->SetValue(2, 1.);
		MySpoint->SetValue(2,TRUE); 

		MyXpoint->SetValue(3,-1.);
		MyYpoint->SetValue(3, 0.);
		MySpoint->SetValue(3,FALSE); 

		MyXpoint->SetValue(4, 1.);
		MyYpoint->SetValue(4, 0.);
		MySpoint->SetValue(4,TRUE); 
		break ;
	    case Aspect_TOM_STAR :
		MyXpoint = new TShort_HArray1OfShortReal(1,8) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,8) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,8) ;

		MyXpoint->SetValue(1, 0.);
		MyYpoint->SetValue(1,-1.);
		MySpoint->SetValue(1,FALSE); 

		MyXpoint->SetValue(2, 0.);
		MyYpoint->SetValue(2, 1.);
		MySpoint->SetValue(2,TRUE); 

		MyXpoint->SetValue(3,-1.);
		MyYpoint->SetValue(3, 0.);
		MySpoint->SetValue(3,FALSE); 

		MyXpoint->SetValue(4, 1.);
		MyYpoint->SetValue(4, 0.);
		MySpoint->SetValue(4,TRUE); 

		MyXpoint->SetValue(5,(float ) -0.7);
		MyYpoint->SetValue(5,(float ) -0.7);
		MySpoint->SetValue(5,FALSE); 

		MyXpoint->SetValue(6,(float ) 0.7);
		MyYpoint->SetValue(6,(float ) 0.7);
		MySpoint->SetValue(6,TRUE); 

		MyXpoint->SetValue(7,(float ) 0.7);
		MyYpoint->SetValue(7,(float ) -0.7);
		MySpoint->SetValue(7,FALSE); 

		MyXpoint->SetValue(8,(float ) -0.7);
		MyYpoint->SetValue(8,(float ) 0.7);
		MySpoint->SetValue(8,TRUE); 
		break ;
	    case Aspect_TOM_O :
		MyXpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+1) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+1) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,MAX_O_POINT+1) ;

		{ Standard_Integer i;
		  Standard_Real da = 2. * M_PI / MAX_O_POINT;
		  Standard_Real a = 0.;
		  for( i=1 ; i<= MAX_O_POINT+1 ; i++,a += da ) {
		    MyXpoint->SetValue(i,(float ) Cos(a));
		    MyYpoint->SetValue(i,(float ) Sin(a));
		    MySpoint->SetValue(i,(i > 1) ? TRUE : FALSE);
		  }
		  i = MAX_O_POINT+1;
		  MyXpoint->SetValue(i,1.);
		  MyYpoint->SetValue(i,0.);
		}
		break ;
	    case Aspect_TOM_X :
		MyXpoint = new TShort_HArray1OfShortReal(1,4) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,4) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,4) ;

		MyXpoint->SetValue(1,(float ) -0.7);
		MyYpoint->SetValue(1,(float ) -0.7);
		MySpoint->SetValue(1,FALSE);

		MyXpoint->SetValue(2,(float ) 0.7);
		MyYpoint->SetValue(2,(float ) 0.7);
		MySpoint->SetValue(2,TRUE);
		
		MyXpoint->SetValue(3,(float ) 0.7);
		MyYpoint->SetValue(3,(float ) -0.7);
		MySpoint->SetValue(3,FALSE);
		
		MyXpoint->SetValue(4,(float ) -0.7);
		MyYpoint->SetValue(4,(float ) 0.7);
		MySpoint->SetValue(4,TRUE); 
		break ;
	    case Aspect_TOM_O_POINT :
		MyXpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+6) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+6) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,MAX_O_POINT+6) ;
		{ Standard_Integer i;
		  Standard_Real da = 2. * M_PI / MAX_O_POINT;
		  Standard_Real a = 0.;
		  for( i=1 ; i<= MAX_O_POINT+1 ; i++,a += da ) {
		    MyXpoint->SetValue(i,(float ) Cos(a));
		    MyYpoint->SetValue(i,(float ) Sin(a));
		    MySpoint->SetValue(i,(i > 1) ? TRUE : FALSE);
		  }
		  i = MAX_O_POINT+1;
		  MyXpoint->SetValue(i,1.);
		  MyYpoint->SetValue(i,0.);

		  MyXpoint->SetValue(i+1,-0.25);
		  MyYpoint->SetValue(i+1,-0.25);
		  MySpoint->SetValue(i+1,FALSE);
		
		  MyXpoint->SetValue(i+2,-0.25);
		  MyYpoint->SetValue(i+2,0.25);
		  MySpoint->SetValue(i+2,TRUE);
		
		  MyXpoint->SetValue(i+3,0.25);
		  MyYpoint->SetValue(i+3,0.25);
		  MySpoint->SetValue(i+3,TRUE);
		
		  MyXpoint->SetValue(i+4,0.25);
		  MyYpoint->SetValue(i+4,-0.25);
		  MySpoint->SetValue(i+4,TRUE);
		
		  MyXpoint->SetValue(i+5,-0.25);
		  MyYpoint->SetValue(i+5,-0.25);
		  MySpoint->SetValue(i+5,TRUE);
		}
		break ;
	    case Aspect_TOM_O_PLUS :
		MyXpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+5) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+5) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,MAX_O_POINT+5) ;

		{ Standard_Integer i;
		  Standard_Real da = 2. * M_PI / MAX_O_POINT;
		  Standard_Real a = 0.;
		  for( i=1 ; i<= MAX_O_POINT+1 ; i++,a += da ) {
		    MyXpoint->SetValue(i,(float ) Cos(a));
		    MyYpoint->SetValue(i,(float ) Sin(a));
		    MySpoint->SetValue(i,(i > 1) ? TRUE : FALSE);
		  }
		  i = MAX_O_POINT+1;
		  MyXpoint->SetValue(i,1.);
		  MyYpoint->SetValue(i,0.);

		  MyXpoint->SetValue(i+1,0.);
		  MyYpoint->SetValue(i+1,-0.5);
		  MySpoint->SetValue(i+1,FALSE);
		
		  MyXpoint->SetValue(i+2,0.);
		  MyYpoint->SetValue(i+2,0.5);
		  MySpoint->SetValue(i+2,TRUE);
		
		  MyXpoint->SetValue(i+3,-0.5);
		  MyYpoint->SetValue(i+3,0.);
		  MySpoint->SetValue(i+3,FALSE);
		
		  MyXpoint->SetValue(i+4,0.5);
		  MyYpoint->SetValue(i+4,0.);
		  MySpoint->SetValue(i+4,TRUE);
		}
		break ;
	    case Aspect_TOM_O_STAR :
		MyXpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+9) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+9) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,MAX_O_POINT+9) ;

		{ Standard_Integer i;
		  Standard_Real da = 2. * M_PI / MAX_O_POINT;
		  Standard_Real a = 0.;
		  for( i=1 ; i<= MAX_O_POINT+1 ; i++,a += da ) {
		    MyXpoint->SetValue(i,(float ) Cos(a));
		    MyYpoint->SetValue(i,(float ) Sin(a));
		    MySpoint->SetValue(i,(i > 1) ? TRUE : FALSE);
		  }
		  i = MAX_O_POINT+1;
		  MyXpoint->SetValue(i,1.);
		  MyYpoint->SetValue(i,0.);

		  MyXpoint->SetValue(i+1, 0.);
		  MyYpoint->SetValue(i+1,-0.5);
		  MySpoint->SetValue(i+1,FALSE);
		
		  MyXpoint->SetValue(i+2, 0.);
		  MyYpoint->SetValue(i+2,0.5);
		  MySpoint->SetValue(i+2,TRUE);
		
		  MyXpoint->SetValue(i+3,-0.5);
		  MyYpoint->SetValue(i+3, 0.);
		  MySpoint->SetValue(i+3,FALSE);
		
		  MyXpoint->SetValue(i+4,0.5);
		  MyYpoint->SetValue(i+4, 0.);
		  MySpoint->SetValue(i+4,TRUE);
		
		  MyXpoint->SetValue(i+5,(float ) -0.35);
		  MyYpoint->SetValue(i+5,(float ) -0.35);
		  MySpoint->SetValue(i+5,FALSE);
		
		  MyXpoint->SetValue(i+6,(float ) 0.35);
		  MyYpoint->SetValue(i+6,(float ) 0.35);
		  MySpoint->SetValue(i+6,TRUE);
		
		  MyXpoint->SetValue(i+7,(float ) 0.35);
		  MyYpoint->SetValue(i+7,(float ) -0.35);
		  MySpoint->SetValue(i+7,FALSE);
		
		  MyXpoint->SetValue(i+8,(float ) -0.35);
		  MyYpoint->SetValue(i+8,(float ) 0.35);
		  MySpoint->SetValue(i+8,TRUE);
		}
		break ;
	    case Aspect_TOM_O_X :
		MyXpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+5) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,MAX_O_POINT+5) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,MAX_O_POINT+5) ;

		{ Standard_Integer i;
		  Standard_Real da = 2. * M_PI / MAX_O_POINT;
		  Standard_Real a = 0.;
		  for( i=1 ; i<= MAX_O_POINT+1 ; i++,a += da ) {
		    MyXpoint->SetValue(i,(float ) Cos(a));
		    MyYpoint->SetValue(i,(float ) Sin(a));
		    MySpoint->SetValue(i,(i > 1) ? TRUE : FALSE);
		  }
		  i = MAX_O_POINT+1;
		  MyXpoint->SetValue(i,1.);
		  MyYpoint->SetValue(i,0.);

		  MyXpoint->SetValue(i+1,(float ) -0.35);
		  MyYpoint->SetValue(i+1,(float ) -0.35);
		  MySpoint->SetValue(i+1,FALSE);
		
		  MyXpoint->SetValue(i+2,(float ) 0.35);
		  MyYpoint->SetValue(i+2,(float ) 0.35);
		  MySpoint->SetValue(i+2,TRUE);
		
		  MyXpoint->SetValue(i+3,(float ) 0.35);
		  MyYpoint->SetValue(i+3,(float ) -0.35);
		  MySpoint->SetValue(i+3,FALSE);
		
		  MyXpoint->SetValue(i+4,(float ) -0.35);
		  MyYpoint->SetValue(i+4,(float ) 0.35);
		  MySpoint->SetValue(i+4,TRUE); 
		}
		break ;
	    case Aspect_TOM_BALL :
		MyXpoint = new TShort_HArray1OfShortReal(1,
						MAX_BALL_LINE*(MAX_O_POINT+1)) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,
						MAX_BALL_LINE*(MAX_O_POINT+1)) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,
						MAX_BALL_LINE*(MAX_O_POINT+1)) ;

		{ Standard_Integer i,j,n = 0;
		  Standard_Real da = 2. * M_PI / MAX_O_POINT;
		  Standard_Real dr = 1./MAX_BALL_LINE;
		  Standard_Real a,r = 1.;
		  
		  for( i=1 ; i<= MAX_BALL_LINE ; i++ ) {
		    a = 0.;
		    for( j=1 ; j<= MAX_O_POINT+1 ; j++,a += da ) {
		      n++;
		      MyXpoint->SetValue(n,(float )( r*Cos(a)));
		      MyYpoint->SetValue(n,(float )( r*Sin(a)));
		      MySpoint->SetValue(n,(j > 1) ? TRUE : FALSE);
		    }
		    MyXpoint->SetValue(n,(float ) r);
		    MyYpoint->SetValue(n,0.);
		    r -= dr;
		  }
		}
		break ;
	    case Aspect_TOM_RING1 :
		MyXpoint = new TShort_HArray1OfShortReal(1,
						(MAX_BALL_LINE/4)*(MAX_O_POINT+1)) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,
						(MAX_BALL_LINE/4)*(MAX_O_POINT+1)) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,
						(MAX_BALL_LINE/4)*(MAX_O_POINT+1)) ;

		{ Standard_Integer i,j,n = 0;
		  Standard_Real da = 2. * M_PI / MAX_O_POINT;
		  Standard_Real dr = 1./MAX_BALL_LINE;
		  Standard_Real a,r = 1.;
		  
		  for( i=1 ; i<= MAX_BALL_LINE/4 ; i++ ) {
		    a = 0.;
		    for( j=1 ; j<= MAX_O_POINT+1 ; j++,a += da ) {
		      n++;
		      MyXpoint->SetValue(n,(float )( r*Cos(a)));
		      MyYpoint->SetValue(n,(float )( r*Sin(a)));
		      MySpoint->SetValue(n,(j > 1) ? TRUE : FALSE);
		    }
		    MyXpoint->SetValue(n,(float ) r);
		    MyYpoint->SetValue(n,0.);
		    r -= dr;
		  }
		}
		break ;
	    case Aspect_TOM_RING2 :
		MyXpoint = new TShort_HArray1OfShortReal(1,
						(MAX_BALL_LINE/3)*(MAX_O_POINT+1)) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,
						(MAX_BALL_LINE/3)*(MAX_O_POINT+1)) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,
						(MAX_BALL_LINE/3)*(MAX_O_POINT+1)) ;

		{ Standard_Integer i,j,n = 0;
		  Standard_Real da = 2. * M_PI / MAX_O_POINT;
		  Standard_Real dr = 1./MAX_BALL_LINE;
		  Standard_Real a,r = 1.;
		  
		  for( i=1 ; i<= MAX_BALL_LINE/3 ; i++ ) {
		    a = 0.;
		    for( j=1 ; j<= MAX_O_POINT+1 ; j++,a += da ) {
		      n++;
		      MyXpoint->SetValue(n,(float )( r*Cos(a)));
		      MyYpoint->SetValue(n,(float )( r*Sin(a)));
		      MySpoint->SetValue(n,(j > 1) ? TRUE : FALSE);
		    }
		    MyXpoint->SetValue(n,(float ) r);
		    MyYpoint->SetValue(n,0.);
		    r -= dr;
		  }
		}
		break ;
	    case Aspect_TOM_RING3 :
		MyXpoint = new TShort_HArray1OfShortReal(1,
						(MAX_BALL_LINE)/2*(MAX_O_POINT+1)) ;
		MyYpoint = new TShort_HArray1OfShortReal(1,
						(MAX_BALL_LINE)/2*(MAX_O_POINT+1)) ;
		MySpoint = new TColStd_HArray1OfBoolean(1,
						(MAX_BALL_LINE)/2*(MAX_O_POINT+1)) ;

		{ Standard_Integer i,j,n = 0;
		  Standard_Real da = 2. * M_PI / MAX_O_POINT;
		  Standard_Real dr = 1./MAX_BALL_LINE;
		  Standard_Real a,r = 1.;
		  
		  for( i=1 ; i<= MAX_BALL_LINE/2 ; i++ ) {
		    a = 0.;
		    for( j=1 ; j<= MAX_O_POINT+1 ; j++,a += da ) {
		      n++;
		      MyXpoint->SetValue(n,(float )( r*Cos(a)));
		      MyYpoint->SetValue(n,(float )( r*Sin(a)));
		      MySpoint->SetValue(n,(j > 1) ? TRUE : FALSE);
		    }
		    MyXpoint->SetValue(n,(float ) r);
		    MyYpoint->SetValue(n,0.);
		    r -= dr;
		  }
		}
		break ;
	}

}

Standard_Boolean Aspect_MarkerStyle::IsEqual(const Aspect_MarkerStyle& Other) const
{
  return (
	  (MyMarkerType == Other.MyMarkerType) &&
	  (MyXpoint == Other.MyXpoint) &&
	  (MyYpoint == Other.MyYpoint) &&
	  (MySpoint == Other.MySpoint));
}

Standard_Boolean Aspect_MarkerStyle::IsNotEqual(const Aspect_MarkerStyle& Other) const
{
  return !IsEqual(Other);
}
