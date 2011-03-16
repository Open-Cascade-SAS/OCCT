// File:	Draw_PluginMacro.hxx
// Created:	Thu Aug 14 14:15:00 2003
// Author:	Sergey ZARITCHNY <szy@nnov.matra-dtv.fr>


#ifndef _Draw_PluginMacro_HeaderFile
#define _Draw_PluginMacro_HeaderFile

#define DPLUGIN(name) \
extern "C" {Standard_EXPORT void PLUGINFACTORY(Draw_Interpretor&);} \
void  PLUGINFACTORY(Draw_Interpretor& theDI) { \
        name::Factory(theDI);} \
\

#endif
