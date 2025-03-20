// Created on: 1997-04-10
// Created by: Prestataire Mary FABIEN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_Check_HeaderFile
#define _TopOpeBRepDS_Check_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopOpeBRepDS_DataMapOfCheckStatus.hxx>
#include <Standard_Transient.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_OStream.hxx>
#include <Standard_CString.hxx>
#include <TopOpeBRepDS_CheckStatus.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopOpeBRepDS_HDataStructure;

class TopOpeBRepDS_Check;
DEFINE_STANDARD_HANDLE(TopOpeBRepDS_Check, Standard_Transient)

//! a tool verifying integrity and structure of DS
class TopOpeBRepDS_Check : public Standard_Transient
{

public:
  Standard_EXPORT TopOpeBRepDS_Check();

  Standard_EXPORT TopOpeBRepDS_Check(const Handle(TopOpeBRepDS_HDataStructure)& HDS);

  //! Check integrition of DS
  Standard_EXPORT Standard_Boolean ChkIntg();

  //! Check integrition of interferences
  //! (les supports et les geometries de LI)
  Standard_EXPORT Standard_Boolean ChkIntgInterf(const TopOpeBRepDS_ListOfInterference& LI);

  //! Verifie que le ieme element de la DS existe, et
  //! pour un K de type topologique, verifie qu'il est du
  //! bon type (VERTEX, EDGE, WIRE, FACE, SHELL ou SOLID)
  Standard_EXPORT Standard_Boolean CheckDS(const Standard_Integer i, const TopOpeBRepDS_Kind K);

  //! Check integrition des champs SameDomain de la DS
  Standard_EXPORT Standard_Boolean ChkIntgSamDom();

  //! Verifie que les Shapes existent bien dans la DS
  //! Utile pour les Shapes SameDomain
  //! si la liste est vide, renvoie vrai
  Standard_EXPORT Standard_Boolean CheckShapes(const TopTools_ListOfShape& LS) const;

  //! Verifie que les Vertex   non   SameDomain sont bien
  //! nonSameDomain, que  les  vertex sameDomain sont  bien
  //! SameDomain,  que    les  Points sont  non    confondus
  //! ni entre eux, ni avec des Vertex.
  Standard_EXPORT Standard_Boolean OneVertexOnPnt();

  Standard_EXPORT const Handle(TopOpeBRepDS_HDataStructure)& HDS() const;

  Standard_EXPORT Handle(TopOpeBRepDS_HDataStructure)& ChangeHDS();

  Standard_EXPORT Standard_OStream& PrintIntg(Standard_OStream& S);

  //! Prints the name  of CheckStatus  <stat>  as  a String
  Standard_EXPORT Standard_OStream& Print(const TopOpeBRepDS_CheckStatus stat, Standard_OStream& S);

  //! Prints the name  of CheckStatus  <stat>  as  a String
  Standard_EXPORT Standard_OStream& PrintShape(const TopAbs_ShapeEnum SE, Standard_OStream& S);

  //! Prints the name  of CheckStatus  <stat>  as  a String
  Standard_EXPORT Standard_OStream& PrintShape(const Standard_Integer index, Standard_OStream& S);

  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_Check, Standard_Transient)

protected:
private:
  Standard_EXPORT Standard_OStream& PrintMap(TopOpeBRepDS_DataMapOfCheckStatus& MapStat,
                                             const Standard_CString             eltstr,
                                             Standard_OStream&                  S);

  Standard_EXPORT Standard_OStream& PrintElts(TopOpeBRepDS_DataMapOfCheckStatus& MapStat,
                                              const TopOpeBRepDS_CheckStatus     Stat,
                                              Standard_Boolean&                  b,
                                              Standard_OStream&                  S);

  Handle(TopOpeBRepDS_HDataStructure) myHDS;
  TopOpeBRepDS_DataMapOfCheckStatus   myMapSurfaceStatus;
  TopOpeBRepDS_DataMapOfCheckStatus   myMapCurveStatus;
  TopOpeBRepDS_DataMapOfCheckStatus   myMapPointStatus;
  TopOpeBRepDS_DataMapOfCheckStatus   myMapShapeStatus;
};

#endif // _TopOpeBRepDS_Check_HeaderFile
