//
// Modified : GG ; 14/09/01 Implements the new AddEntry method
//

#include <Aspect_ColorCubeColorMap.ixx>
#include <Aspect_ColorMapEntry.hxx>

#ifdef TRACE
static int Tlevel = 0 ;
#endif

Aspect_ColorCubeColorMap::Aspect_ColorCubeColorMap( 
	const Standard_Integer basepixel ,
	const Standard_Integer red_max ,
	const Standard_Integer red_mult ,
	const Standard_Integer green_max ,
	const Standard_Integer green_mult ,
	const Standard_Integer blue_max ,
	const Standard_Integer blue_mult )
	:Aspect_ColorMap( Aspect_TOC_ColorCube )

{ Standard_Integer 	r,g,b,i ;
  Aspect_ColorMapEntry value ;
  Quantity_Color  	rgb ;
  Standard_Integer	max[3], mult[3], ti[3];

  mybasepixel = basepixel ;
  myredmax    = red_max ;
  myredmult   = red_mult ;
  mygreenmax  = green_max ;
  mygreenmult = green_mult ;
  mybluemax   = blue_max ;
  mybluemult  = blue_mult ;

  r = 0 ; g = 1 ; b = 2 ;

  if ( ( myredmult < mygreenmult ) && ( myredmult < mybluemult ) ) {
	r = 0 ; 
	if ( mygreenmult < mybluemult ) { g = 1 ; b = 2 ; }
	else 			        { b = 1 ; g = 2 ; }
  }
  if ( ( mygreenmult < myredmult ) && ( mygreenmult < mybluemult ) ) {
	g = 0 ; 
	if ( myredmult < mybluemult ) { r = 1 ; b = 2 ; }
	else 			      { b = 1 ; r = 2 ; }
  }
  if ( ( mybluemult < myredmult ) && ( mybluemult < mygreenmult ) ) {
	b = 0 ; 
	if ( myredmult < mygreenmult ) { r = 1 ; g = 2 ; }
	else 			       { g = 1 ; r = 2 ; }
  }

#ifdef TRACE
  if ( Tlevel ) 
  	cout << "ColorCubeColorMap constructor (r,g,b) :" 
		<< r << "," << g << "," << b << endl << flush ;
#endif

  mult[r] = myredmult ;		max[r] = myredmax ;
  mult[g] = mygreenmult ;	max[g] = mygreenmax ;
  mult[b] = mybluemult ;	max[b] = mybluemax ;

  for( ti[2] = 0 ; ti[2] <= max[2] ; ti[2]++ ) {
    for( ti[1] = 0 ; ti[1] <= max[1] ; ti[1]++ ) {
      for( ti[0] = 0 ; ti[0] <= max[0] ; ti[0]++ ) {
	rgb.SetValues( (Standard_Real) ti[r] / (Standard_Real) max[r],
		       (Standard_Real) ti[g] / (Standard_Real) max[g],
		       (Standard_Real) ti[b] / (Standard_Real) max[b],
			Quantity_TOC_RGB ) ;

	i = ti[r] * mult[r] + ti[g] * mult[g] + ti[b] * mult[b] ;

	value.SetValue( mybasepixel+i, rgb ) ;
#ifdef TRACE
  if ( Tlevel ) 
	value.Dump() ;
#endif
	mydata.Append( value ) ;
      }
    }
  }
}

const Aspect_ColorMapEntry& Aspect_ColorCubeColorMap::NearestEntry( 
				const Quantity_Color&  color ) const

{ 
  return( Entry( NearestColorMapIndex( color ) ) ) ;
}

Standard_Integer Aspect_ColorCubeColorMap::NearestColorMapIndex( 
					const Quantity_Color&  color ) const

{ Standard_Integer 	r,g,b ;

  r = (Standard_Integer ) ( color.Red()   * myredmax   + 0.5) ;
  g = (Standard_Integer ) ( color.Green() * mygreenmax + 0.5) ;
  b = (Standard_Integer ) ( color.Blue()  * mybluemax  + 0.5) ;

  return( r*myredmult+g*mygreenmult+b*mybluemult + 1 ) ;
}

const Aspect_ColorMapEntry& Aspect_ColorCubeColorMap::FindEntry( 
		const Standard_Integer index ) const 

{ 
  return( Entry( FindColorMapIndex( index ) ) ) ;
}

Standard_Integer Aspect_ColorCubeColorMap::FindColorMapIndex( 
		const Standard_Integer index ) const 
{

  if ( 	index < mybasepixel || 
	index >= ( mybasepixel+Size() ) ){
 	 Aspect_BadAccess::Raise ("FindEntryIndex() index not found.");
  }

  return( index - mybasepixel + 1 ) ;
}

void Aspect_ColorCubeColorMap::ColorCubeDefinition(
	 Standard_Integer& basepixel ,
	 Standard_Integer& red_max ,
	 Standard_Integer& red_mult ,
	 Standard_Integer& green_max ,
	 Standard_Integer& green_mult ,
	 Standard_Integer& blue_max ,
	 Standard_Integer& blue_mult ) const

{
  basepixel = mybasepixel ;
  red_max    = myredmax ;
  red_mult   = myredmult ;
  green_max  = mygreenmax ;
  green_mult = mygreenmult ;
  blue_max   = mybluemax ;
  blue_mult  = mybluemult ;
}

Standard_Integer Aspect_ColorCubeColorMap::AddEntry (const Quantity_Color &aColor) {

    return mybasepixel + NearestColorMapIndex(aColor) - 1;
}
