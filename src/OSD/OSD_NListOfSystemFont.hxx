// File:  OSD_NListOfSystemFont.hxx
// Created: 20.01.2009
// Author:      Alexander A. BORODIN

#ifndef _OSD_NListOfSystemFont_HeaderFile
#define _OSD_NListOfSystemFont_HeaderFile

#include <NCollection_DefineList.hxx>
#include <OSD_SystemFont.hxx>

DEFINE_LIST (OSD_NListOfSystemFont,
             NCollection_List,
             Handle(OSD_SystemFont))

#endif
