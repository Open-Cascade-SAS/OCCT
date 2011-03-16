// File:  Visual3d_NListOfLayerItem.hxx
// Created:  20.03.2009
// Author:      Alexander A. BORODIN

#ifndef _Visual3d_NListOfLayerItem_HeaderFile
#define _Visual3d_NListOfLayerItem_HeaderFile

#include <NCollection_DefineList.hxx>
#include <Visual3d_LayerItem.hxx>

DEFINE_LIST   (Visual3d_NListOfLayerItem,
               NCollection_List,
               Handle(Visual3d_LayerItem))

#endif
