// File:      BestFitBnd_HUBTreeOfSphere.hxx
// Created:   Thu Mar 17 14:30:42 2005
// Author:    OPEN CASCADE Support
// Copyright: OPEN CASCADE SA 2005

#ifndef _Extrema_HUBTreeOfSphere_HeaderFile
#define _Extrema_HUBTreeOfSphere_HeaderFile

#include <NCollection_UBTree.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_Handle.hxx>
#include <Bnd_Sphere.hxx>

typedef NCollection_UBTree<Standard_Integer,Bnd_Sphere> Extrema_UBTreeOfSphere;
typedef NCollection_UBTreeFiller<Standard_Integer,Bnd_Sphere> Extrema_UBTreeFillerOfSphere;
typedef NCollection_Handle<Extrema_UBTreeOfSphere> Extrema_HUBTreeOfSphere;

#endif //_Extrema_HUBTreeOfSphere_HeaderFile
