
#include <Aspect_ColorMap.ixx>

Aspect_ColorMap::Aspect_ColorMap( const Aspect_TypeOfColorMap type ) {

	mytype		= type;
}

Aspect_TypeOfColorMap Aspect_ColorMap::Type() const {

	return mytype ;
}

Standard_Integer     Aspect_ColorMap::Size() const {

	return mydata.Length() ;
}

Standard_Integer     Aspect_ColorMap::Index(const Standard_Integer anIndex) const {

	if( anIndex < 1 || anIndex > Size() ) {
            Aspect_BadAccess::Raise ("Undefined colormap Index");
	}

	Aspect_ColorMapEntry theEntry = mydata.Value(anIndex) ;

	return theEntry.Index() ;
}

const Aspect_ColorMapEntry& Aspect_ColorMap::Entry (const Standard_Integer AnIndex) const {

		return mydata.Value(AnIndex);
}

void Aspect_ColorMap::Dump () const {

  Standard_Integer i ;

  cout << "Colormap Dump-->\n" ;

  for ( i = 1 ; i <= Size() ; i++ ) (Entry(i)).Dump() ;

  cout << "<--End Colormap Dump\n" ;
}
