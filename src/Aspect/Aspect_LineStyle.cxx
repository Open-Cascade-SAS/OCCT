// Created on: 1992-09-14
// Created by: GG
// Copyright (c) 1992-1999 Matra Datavision
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




//-Version	

//-Design	Declaration des variables specifiques aux Type de traits

//-Warning	Un style est definie, soit par son type predefini TOL_...
//		soit par sa description en METRE

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Aspect_LineStyle.ixx>
#include <Aspect_Units.hxx>

//-Aliases

//-Global data definitions

//	MyLineType	 	: TypeOfLine from Aspect;
//	MyLineDescriptor	: HArray1OfReal from TColStd;

//-Constructors

//-Destructors

//-Methods, in order

Aspect_LineStyle::Aspect_LineStyle () : MyLineDescriptor(PredefinedStyle(Aspect_TOL_SOLID)) {
}

Aspect_LineStyle::Aspect_LineStyle (const Aspect_TypeOfLine Type) : MyLineDescriptor(PredefinedStyle(Type)) {
}

Aspect_LineStyle::Aspect_LineStyle (const TColQuantity_Array1OfLength& UserDefinedStyle) {
Standard_Integer i ;

	MyLineType = Aspect_TOL_USERDEFINED ;
	// Modif CAL 8/2/95
	MyLineDescriptor = new TColQuantity_HArray1OfLength
		(UserDefinedStyle.Lower(), UserDefinedStyle.Upper()) ;

	for( i=UserDefinedStyle.Lower() ; i<=UserDefinedStyle.Upper() ; i++ ) {
	    // Modif CAL 8/2/95
	    MyLineDescriptor->SetValue(i, UserDefinedStyle(i)) ;
	    if( UserDefinedStyle(i) <= 0. )
		Aspect_LineStyleDefinitionError::Raise ("Bad Descriptor") ;
	}
}

Aspect_LineStyle& Aspect_LineStyle::Assign (const Aspect_LineStyle& Other) {
Standard_Integer i ;

	// Modif CAL 8/2/95
	MyLineDescriptor = new TColQuantity_HArray1OfLength
		((Other.Values()).Lower(), (Other.Values()).Upper()) ;
	MyLineType = Other.MyLineType ;

	if( MyLineType != Aspect_TOL_SOLID ) {
	  for( i=MyLineDescriptor->Lower() ; 
				i<=MyLineDescriptor->Upper() ; i++ ){
	    // Modif CAL 8/2/95
	    MyLineDescriptor->SetValue(i, (Other.Values())(i)) ;
	    if( (Other.Values())(i) <= 0. )
		Aspect_LineStyleDefinitionError::Raise ("Bad Descriptor") ;
	  }
	}

	return (*this);

}

void Aspect_LineStyle::SetValues (const Aspect_TypeOfLine Type) {

	MyLineDescriptor = PredefinedStyle(Type) ;

}

void Aspect_LineStyle::SetValues (const TColQuantity_Array1OfLength& UserDefinedStyle) {
Standard_Integer i ;

	// Modif CAL 8/2/95
	MyLineDescriptor = new TColQuantity_HArray1OfLength
		(UserDefinedStyle.Lower(), UserDefinedStyle.Upper()) ;
	MyLineType = Aspect_TOL_USERDEFINED ;

	for( i=UserDefinedStyle.Lower() ; i<=UserDefinedStyle.Upper() ; i++ ) {
	    // Modif CAL 8/2/95
	    MyLineDescriptor->SetValue(i, UserDefinedStyle(i)) ;
	    if( UserDefinedStyle(i) <= 0. )
		Aspect_LineStyleDefinitionError::Raise ("Bad Descriptor") ;
	}
}

Aspect_TypeOfLine Aspect_LineStyle::Style () const {

	return (MyLineType);
}

Standard_Integer Aspect_LineStyle::Length () const {

	if( MyLineDescriptor.IsNull() ) return (0) ;
	else return (MyLineDescriptor->Length());
}

const TColQuantity_Array1OfLength& Aspect_LineStyle::Values () const {

	return (MyLineDescriptor->Array1());
}

Handle(TColQuantity_HArray1OfLength) Aspect_LineStyle::PredefinedStyle(const Aspect_TypeOfLine Type) {
Handle(TColQuantity_HArray1OfLength) descriptor ; 

	MyLineType	= Type ;
	switch ( Type ) {
	    case Aspect_TOL_USERDEFINED :
		Aspect_LineStyleDefinitionError::Raise ("Bad Line Type Style");
	    case Aspect_TOL_SOLID :
		descriptor = new TColQuantity_HArray1OfLength(1,1) ;
		descriptor->SetValue(1,0.) ;
		break ;
	    case Aspect_TOL_DASH :
		descriptor = new TColQuantity_HArray1OfLength(1,2) ;
		descriptor->SetValue(1,2.0 MILLIMETER) ;
		descriptor->SetValue(2,1.0 MILLIMETER) ;
		break ;
	    case Aspect_TOL_DOT :
		descriptor = new TColQuantity_HArray1OfLength(1,2) ;
		descriptor->SetValue(1,0.2 MILLIMETER) ;
		descriptor->SetValue(2,0.5 MILLIMETER) ;
		break ;
	    case Aspect_TOL_DOTDASH :
		descriptor = new TColQuantity_HArray1OfLength(1,4) ;
		descriptor->SetValue(1,10.0 MILLIMETER) ;
		descriptor->SetValue(2,1.0 MILLIMETER) ;
		descriptor->SetValue(3,2.0 MILLIMETER) ;
		descriptor->SetValue(4,1.0 MILLIMETER) ;
		break ;
	    default :
		descriptor.Nullify() ;
	}

	return (descriptor) ;
}

Standard_Boolean Aspect_LineStyle::IsEqual(const Aspect_LineStyle& Other) const
{
  return (
	  (MyLineType == Other.MyLineType) &&
	  (MyLineDescriptor == Other.MyLineDescriptor));
}

Standard_Boolean Aspect_LineStyle::IsNotEqual(const Aspect_LineStyle& Other) const
{
  return !IsEqual(Other);
}
