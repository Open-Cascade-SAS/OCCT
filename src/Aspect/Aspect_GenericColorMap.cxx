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

#define IMP080300	//GG Add protection on FindColorMapIndex()

#define IMP060400	//GG Take in account the Hue of the color
//			for computing the nearest color index.

#include <Aspect_GenericColorMap.ixx>
#include <Aspect_ColorMapEntry.hxx>

Aspect_GenericColorMap::Aspect_GenericColorMap ():Aspect_ColorMap (Aspect_TOC_Generic) {
Aspect_ColorMapEntry theDefaultEntry;

    AddEntry(theDefaultEntry);
}

void Aspect_GenericColorMap::AddEntry (const Aspect_ColorMapEntry& AnEntry) {
Standard_Integer index = AnEntry.Index();

    if ( myDataMap.IsBound( index ) ) {
        Standard_Integer i = myDataMap( index ) ;
        mydata.SetValue( i , AnEntry ) ;
    } else {
        mydata.Append( AnEntry ) ;
        myDataMap.Bind( index , mydata.Length() ) ; 
    }
}

Standard_Integer Aspect_GenericColorMap::AddEntry (const Quantity_Color &aColor) {
Aspect_ColorMapEntry theEntry ;
Standard_Integer i,maxindex = 0 ;

    for( i=1 ; i<=mydata.Length() ; i++ ) {
	theEntry = mydata.Value(i) ;
	maxindex = Max(maxindex,theEntry.Index()) ;
	if( theEntry.Color() == aColor ) return theEntry.Index() ;
    }

    maxindex++ ;
    theEntry.SetValue(maxindex,aColor) ;
    mydata.Append( theEntry ) ;
    myDataMap.Bind( maxindex , mydata.Length() ) ; 
    return maxindex ;
}

void Aspect_GenericColorMap::RemoveEntry (const Standard_Integer index) {

   mydata.Remove( index ) ;
}

const Aspect_ColorMapEntry& Aspect_GenericColorMap::NearestEntry( 
				const Quantity_Color&  color ) const

{ 
  return( Entry( NearestColorMapIndex( color ) ) ) ;
}

Standard_Integer Aspect_GenericColorMap::NearestColorMapIndex( 
				const Quantity_Color&  color ) const

{ 

  Standard_Real    dist ;
  struct {
	Standard_Real    dist ;
	Standard_Integer index ;
  } nearest;

  nearest.dist  = 0. ;
  nearest.index = 0 ;

#ifdef IMP060400
  Standard_Integer ehue,hue = (color.Hue() < 0.) ? -1 :
				Standard_Integer(color.Hue())/60;
#endif

  Quantity_Color ecolor;

  if ( Size() == 0 )
  	Aspect_BadAccess::Raise ("NearestColorMapIndex() ColorMap is empty.");

  for ( Standard_Integer i = 1 ; i <= Size() ; i++ ) {
    if ( Entry(i).IsAllocated() ) {
	ecolor = Entry(i).Color();
	dist = color.SquareDistance( ecolor ) ;
#ifdef IMP060400
	ehue = (ecolor.Hue() < 0.) ? -1 : Standard_Integer(ecolor.Hue())/60;
	if ( (nearest.index == 0) || 
		((dist < nearest.dist) && (hue == ehue)) ) {
#else
	if ( nearest.index == 0 || dist < nearest.dist ) {
#endif
		nearest.index = i ;
		nearest.dist  = dist ;
#ifdef IMP060400
		if( dist == 0.0 ) break;
#endif
	}
    }
  }

  if ( nearest.index == 0 )
  	Aspect_BadAccess::Raise ("NearestEntryIndex() ColorMap is empty.");

  return( nearest.index ) ;
}

const Aspect_ColorMapEntry& Aspect_GenericColorMap::FindEntry( 
		const Standard_Integer ColorEntryIndex ) const 

{ 
  return( Entry( FindColorMapIndex( ColorEntryIndex ) ) ) ;
}

Standard_Integer Aspect_GenericColorMap::FindColorMapIndex( 
		const Standard_Integer ColorEntryIndex ) const 
{
Standard_Integer index = 0;
#ifdef IMP080300
  if( myDataMap.IsBound( ColorEntryIndex ) ) 
	index = myDataMap.Find( ColorEntryIndex );
#else
  index = myDataMap ( ColorEntryIndex );
#endif
  return index;
} 
