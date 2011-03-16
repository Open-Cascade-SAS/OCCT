#define PRO9581	//GG_270298
//		Creer la grille dans un objet graphique a l'altitude 0
//		de facon a etre tracee en premier.

#include <V2d_BackgroundGraphicObject.ixx>

#define BASE_PRIORITY 0
#define MAX_PRIORITY 0 

V2d_BackgroundGraphicObject::V2d_BackgroundGraphicObject
       (const Handle(Graphic2d_View)& aView) : Graphic2d_GraphicObject(aView) {
}

Standard_Integer V2d_BackgroundGraphicObject::BasePriority () const {

	return BASE_PRIORITY;

}

Standard_Integer V2d_BackgroundGraphicObject::MaxPriority () const {

	return MAX_PRIORITY;

}
