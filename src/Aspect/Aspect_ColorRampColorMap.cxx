// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Modified : GG ; 14/09/01 Implements the new AddEntry method

#define IMP140901	// GG Compute correctly the color ramp
//			  accordingly to the requested color.

#include <Aspect_ColorRampColorMap.ixx>
#include <Aspect_ColorMapEntry.hxx>
#include <Quantity_NameOfColor.hxx>


Aspect_ColorRampColorMap::Aspect_ColorRampColorMap( 
	const Standard_Integer basepixel ,
	const Standard_Integer dimension ,
	const Quantity_NameOfColor  color )
	:Aspect_ColorMap(Aspect_TOC_ColorRamp )

{ ComputeEntry( basepixel, dimension, 
		Quantity_Color( color ) ) ; }

Aspect_ColorRampColorMap::Aspect_ColorRampColorMap( 
	const Standard_Integer basepixel ,
	const Standard_Integer dimension ,
	const Quantity_Color&  color )
	:Aspect_ColorMap(Aspect_TOC_ColorRamp )

{ ComputeEntry( basepixel, dimension, color ) ; }

void Aspect_ColorRampColorMap::ComputeEntry( 
	const Standard_Integer basepixel ,
	const Standard_Integer dimension ,
	const Quantity_Color&  color )

{ Standard_Integer i ;
  Aspect_ColorMapEntry  value ;
  Quantity_Color  	 rgb ;
  Standard_Real	 v ;
#ifdef IMP140901
  Standard_Real r,g,b;
#endif

	mycolor     = color ;
	mybasepixel = basepixel ;
	mydimension = dimension ;
#ifdef IMP140901
        mycolor.Values(r,g,b,Quantity_TOC_RGB);
#endif

	for( i = 0 ; i < dimension ; i++ ) {
		v = ( Standard_Real ) i / 
			( Standard_Real )(dimension-1) ;

#ifdef IMP140901
		rgb.SetValues( r*v, g*v, b*v, Quantity_TOC_RGB ) ;
#else
		rgb.SetValues( v, v, v, Quantity_TOC_RGB ) ;
#endif

		value.SetValue( basepixel+i, rgb ) ;

		mydata.Append( value ) ;
	}
}

const Aspect_ColorMapEntry& Aspect_ColorRampColorMap::NearestEntry( 
				const Quantity_Color&  color ) const

{ 
  return( Entry( NearestColorMapIndex( color ) ) ) ;
}

Standard_Integer Aspect_ColorRampColorMap::NearestColorMapIndex( 
				const Quantity_Color&  color ) const

{ 
return( (Standard_Integer ) ( ( color.Light() * (mydimension-1) ) + 1 )) ;
}


const Aspect_ColorMapEntry& Aspect_ColorRampColorMap::FindEntry( 
		const Standard_Integer index ) const 

{ 
  return( Entry( FindColorMapIndex( index ) ) ) ;
}

Standard_Integer Aspect_ColorRampColorMap::FindColorMapIndex( 
		const Standard_Integer index ) const 
{

  if ( 	index < mybasepixel || 
	index >= ( mybasepixel+mydimension ) ){
 	 Aspect_BadAccess::Raise ("FindEntryIndex() index not found.");
  }

  return( index - mybasepixel + 1 ) ;
}

void Aspect_ColorRampColorMap::ColorRampDefinition( 
		Standard_Integer& basepixel,
		Standard_Integer& size,
		Quantity_Color&   color ) const

{ basepixel = mybasepixel ;
  size      = mydimension ;
  color     = mycolor ;
}

Standard_Integer Aspect_ColorRampColorMap::AddEntry (const Quantity_Color &aColor) {

    return mybasepixel + NearestColorMapIndex(aColor) - 1;
}

