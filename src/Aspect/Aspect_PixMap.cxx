 
// File		Aspect_PixMap.cxx
// Created	14 October 1999
// Author	VKH
// Updated	GG IMP100701 Add the "depth" field and method
//		to the pixmap object.

//-Copyright	MatraDatavision 1991,1992,1999

//-Version

//
#include <Aspect_PixMap.ixx>

//-Constructor
////////////////////////////////////////////////////////////////////
Aspect_PixMap::Aspect_PixMap ( const Standard_Integer aWidth,
                               const Standard_Integer anHeight,
			       const Standard_Integer aDepth )
{
  myWidth = aWidth;
  myHeight = anHeight;
  myDepth = aDepth;
}

////////////////////////////////////////////////////////////
void Aspect_PixMap::Size ( Standard_Integer &aWidth,
                  Standard_Integer &anHeight ) const {
  aWidth = myWidth;
  anHeight = myHeight;
}

////////////////////////////////////////////////////////////
Standard_Integer Aspect_PixMap::Depth () const { 
  return myDepth;
}

