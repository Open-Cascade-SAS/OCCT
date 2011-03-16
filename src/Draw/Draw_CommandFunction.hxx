// File:	Draw_CommandFunction.hxx
// Created:	Thu Feb 23 17:59:38 1995
// Author:	Remi LEQUETTE
//		<rle@bravox>


#ifndef _Draw_CommandFunction_HeaderFile
#define _Draw_CommandFunction_HeaderFile

class Draw_Interpretor;

typedef Standard_Integer (*Draw_CommandFunction)(Draw_Interpretor&, Standard_Integer, const char**);

#endif
