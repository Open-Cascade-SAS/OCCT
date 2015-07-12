// Created on: 1998-08-18
// Created by: Yves FRICAUD
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _TopOpeBRepDS_GapFiller_HeaderFile
#define _TopOpeBRepDS_GapFiller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_MapOfInteger.hxx>
class TopOpeBRepDS_HDataStructure;
class TopOpeBRepDS_GapTool;
class TopOpeBRepDS_Association;
class TopOpeBRepDS_Interference;
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;



class TopOpeBRepDS_GapFiller 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_GapFiller(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  
  Standard_EXPORT void Perform();
  
  //! Recherche parmi  l'ensemble  des points  d'Interference
  //! la Liste <LI> des points qui correspondent au point d'indice <Index>
  Standard_EXPORT void FindAssociatedPoints (const Handle(TopOpeBRepDS_Interference)& I, TopOpeBRepDS_ListOfInterference& LI);
  
  //! Enchaine les sections   via  les points d'Interferences  deja
  //! associe; Renvoit  dans   <L> les points extremites des Lignes.
  //! Methodes pour  construire la liste des Points qui
  //! peuvent correspondre a une Point donne.
  Standard_EXPORT Standard_Boolean CheckConnexity (TopOpeBRepDS_ListOfInterference& LI);
  
  Standard_EXPORT void AddPointsOnShape (const TopoDS_Shape& S, TopOpeBRepDS_ListOfInterference& LI);
  
  //! Methodes pour  reduire la liste des Points qui
  //! peuvent correspondre a une Point donne.
  Standard_EXPORT void AddPointsOnConnexShape (const TopoDS_Shape& F, const TopOpeBRepDS_ListOfInterference& LI);
  
  Standard_EXPORT void FilterByFace (const TopoDS_Face& F, TopOpeBRepDS_ListOfInterference& LI);
  
  Standard_EXPORT void FilterByEdge (const TopoDS_Edge& E, TopOpeBRepDS_ListOfInterference& LI);
  
  Standard_EXPORT void FilterByIncidentDistance (const TopoDS_Face& F, const Handle(TopOpeBRepDS_Interference)& I, TopOpeBRepDS_ListOfInterference& LI);
  
  //! Return TRUE si I a ete obtenu par une intersection
  //! avec <F>.
  Standard_EXPORT Standard_Boolean IsOnFace (const Handle(TopOpeBRepDS_Interference)& I, const TopoDS_Face& F) const;
  
  //! Return TRUE  si I ou une  de  ses representaions a
  //! pour support <E>.
  //! Methodes de  reconstructions des  geometries des point
  //! et des courbes de section
  Standard_EXPORT Standard_Boolean IsOnEdge (const Handle(TopOpeBRepDS_Interference)& I, const TopoDS_Edge& E) const;
  
  Standard_EXPORT void BuildNewGeometries();
  
  Standard_EXPORT void ReBuildGeom (const Handle(TopOpeBRepDS_Interference)& I1, TColStd_MapOfInteger& Done);




protected:





private:



  Handle(TopOpeBRepDS_HDataStructure) myHDS;
  Handle(TopOpeBRepDS_GapTool) myGapTool;
  Handle(TopOpeBRepDS_Association) myAsso;


};







#endif // _TopOpeBRepDS_GapFiller_HeaderFile
