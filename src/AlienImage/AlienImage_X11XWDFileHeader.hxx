//============================================================================
//==== Titre: AlienImage_X11XWDFileHeader.hxx
//==== Role : The header file of primitve type "XWDFileHeader" from package //====           "X11"
//====
//==== Implementation:  This is a primitive type implemented with typedef
//====        typedef XColor AlienImage_X11XWDFileHeader;
//============================================================================

#ifndef _AlienImage_X11XWDFileHeader_HeaderFile
#define _AlienImage_X11XWDFileHeader_HeaderFile

/*
 * Data structure used by color operations from <X11/XWDFile.h>
 */
//==== Definition de Type ====================================================

#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_X11XWDFileHeader);
//============================================================================

#include <Aspect_XWD.hxx>

typedef XWDFileHeader AlienImage_X11XWDFileHeader ;

ostream& operator << ( ostream& s,  const AlienImage_X11XWDFileHeader& h );

Standard_Boolean   operator==(const AlienImage_X11XWDFileHeader& AnObject,
                              const AlienImage_X11XWDFileHeader& AnotherObject);
void ShallowDump (const AlienImage_X11XWDFileHeader& AnObject,Standard_OStream& S)  ;
#endif
 
