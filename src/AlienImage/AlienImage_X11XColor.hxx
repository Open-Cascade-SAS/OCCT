//============================================================================
//==== Titre: AlienImage_X11XColor.hxx
//==== Role : The header file of primitve type "X11XColor" from package
//====  "AlienImage"
//====
//==== Implementation:  This is a primitive type implemented with typedef
//====        typedef XColor AlienImage_X11XColor;
//============================================================================

#ifndef AlienImage_X11XColor_HeaderFile
#define AlienImage_X11XColor_HeaderFile

/*
 * Data structure used by color operations from <X11/Xlib.h>
 */
//==== Definition de Type ====================================================
#include <Standard_Type.hxx>
extern const Handle_Standard_Type AlienImage_X11XColorType;
inline Handle(Standard_Type) AlienImage_X11XColorType_Type_() {return 0;}
//============================================================================

/*
invalide car unsigned long = 32 ou 64 !
#include <X11/Xlib.h>
typedef XColor AlienImage_X11XColor ;
*/

/*
 * Data structure used by color operations
 */
typedef struct {
	unsigned int pixel;
	unsigned short red, green, blue;
	char flags;  /* do_red, do_green, do_blue */
	char pad;
} AlienImage_X11XColor;

ostream& operator << ( ostream& s,  const AlienImage_X11XColor& color );

Standard_Boolean   operator == (const AlienImage_X11XColor& AnObject,
			        const AlienImage_X11XColor& AnotherObject);
void ShallowDump (const AlienImage_X11XColor& AnObject, Standard_OStream& S);

#endif
 
