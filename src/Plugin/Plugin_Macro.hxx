// File:	Plugin_Macro.hxx
// Created:	Tue Mar  4 10:47:14 1997
// Author:	Mister rmi
//		<rmi@frilox.paris1.matra-dtv.fr>


#ifndef _Plugin_Macro_HeaderFile
#define _Plugin_Macro_HeaderFile

#define PLUGIN(name) \
extern "C" {Standard_EXPORT Handle(Standard_Transient) PLUGINFACTORY(const Standard_GUID&);} \
Handle(Standard_Transient) PLUGINFACTORY(const Standard_GUID& aGUID) { \
   return name::Factory(aGUID);}\
\
					

#endif
