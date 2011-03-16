
#include <Aspect_WidthMap.ixx>

Aspect_WidthMap::Aspect_WidthMap( ) {
Aspect_WidthMapEntry theDefaultEntry;

    	AddEntry(theDefaultEntry);
}

void Aspect_WidthMap::AddEntry (const Aspect_WidthMapEntry& AnEntry) {
Standard_Integer i,index = AnEntry.Index();
Aspect_WidthMapEntry theEntry;

        for( i=1 ; i<=mydata.Length() ; i++ ) {
            theEntry = mydata.Value(i);
            if( index == theEntry.Index() ) break;
        }

        if( i > mydata.Length() ) {
            mydata.Append( AnEntry );
        } else {
            mydata.SetValue(i,AnEntry);
        }
}

Standard_Integer Aspect_WidthMap::AddEntry (const Aspect_WidthOfLine aStyle) {
Aspect_WidthMapEntry theEntry ;
Standard_Integer i,maxindex = 0 ;
 
    for( i=1 ; i<=mydata.Length() ; i++ ) {
        theEntry = mydata.Value(i) ;
        maxindex = Max(maxindex,theEntry.Index()) ;
        if( theEntry.Type() == aStyle ) return theEntry.Index() ;
    }
 
    maxindex++ ;
    theEntry.SetValue(maxindex,aStyle) ;
    mydata.Append( theEntry ) ;
    return maxindex ;
}

Standard_Integer Aspect_WidthMap::AddEntry (const Quantity_Length aStyle) {
Aspect_WidthMapEntry theEntry ;
Standard_Integer i,maxindex = 0 ;
 
    for( i=1 ; i<=mydata.Length() ; i++ ) {
        theEntry = mydata.Value(i) ;
        maxindex = Max(maxindex,theEntry.Index()) ;
        if( theEntry.Width() == aStyle ) return theEntry.Index() ;
    }
 
    maxindex++ ;
    theEntry.SetValue(maxindex,aStyle) ;
    mydata.Append( theEntry ) ;
    return maxindex ;
}

Standard_Integer     Aspect_WidthMap::Size() const {

	return mydata.Length() ;
}

Standard_Integer     Aspect_WidthMap::Index(const Standard_Integer anIndex) const {

        if( anIndex < 1 || anIndex > Size() ) {
            Aspect_BadAccess::Raise ("Undefined widthmap Index");
        }

        Aspect_WidthMapEntry theEntry = mydata.Value(anIndex) ;

        return theEntry.Index() ;
}

Aspect_WidthMapEntry Aspect_WidthMap::Entry (const Standard_Integer AnIndex) const {

	if( AnIndex < 1 || AnIndex > mydata.Length() )
		Aspect_BadAccess::Raise ("Aspect_WidthMap::Entry Bad Index");

	return mydata.Value(AnIndex);
}

void Aspect_WidthMap::Dump () const {

  Standard_Integer i ;

  cout << "Widthmap Dump-->\n" ;

  for ( i = 1 ; i <= Size() ; i++ ) (Entry(i)).Dump() ;

  cout << "<--End Widthmap Dump\n" ;
}
