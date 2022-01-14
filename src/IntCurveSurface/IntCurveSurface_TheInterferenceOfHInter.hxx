// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IntCurveSurface_TheInterferenceOfHInter_HeaderFile
#define _IntCurveSurface_TheInterferenceOfHInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Intf_Interference.hxx>
#include <Intf_Array1OfLin.hxx>
class IntCurveSurface_ThePolygonOfHInter;
class IntCurveSurface_ThePolygonToolOfHInter;
class IntCurveSurface_ThePolyhedronOfHInter;
class IntCurveSurface_ThePolyhedronToolOfHInter;
class gp_Lin;
class Bnd_BoundSortBox;
class gp_Pnt;
class gp_XYZ;



class IntCurveSurface_TheInterferenceOfHInter  : public Intf_Interference
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs  an  empty   interference  between Polygon   and
  //! Polyhedron.
  Standard_EXPORT IntCurveSurface_TheInterferenceOfHInter();
  
  //! Constructs and computes an interference between the Polygon
  //! and the Polyhedron.
  Standard_EXPORT IntCurveSurface_TheInterferenceOfHInter(const IntCurveSurface_ThePolygonOfHInter& thePolyg, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
  //! Constructs  and computes  an  interference   between    the
  //! Straight Line and the Polyhedron.
  Standard_EXPORT IntCurveSurface_TheInterferenceOfHInter(const gp_Lin& theLin, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
  //! Constructs   and   computes  an  interference   between the
  //! Straight Lines and the Polyhedron.
  Standard_EXPORT IntCurveSurface_TheInterferenceOfHInter(const Intf_Array1OfLin& theLins, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
  //! Computes  an interference    between the   Polygon  and the
  //! Polyhedron.
  Standard_EXPORT void Perform (const IntCurveSurface_ThePolygonOfHInter& thePolyg, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
  //! Computes an interference between the Straight  Line and the
  //! Polyhedron.
  Standard_EXPORT void Perform (const gp_Lin& theLin, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
  //! Computes an interference  between the  Straight Lines  and
  //! the Polyhedron.
  Standard_EXPORT void Perform (const Intf_Array1OfLin& theLins, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
  //! Constructs and computes an interference between the Polygon
  //! and the Polyhedron.
  Standard_EXPORT IntCurveSurface_TheInterferenceOfHInter(const IntCurveSurface_ThePolygonOfHInter& thePolyg, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, Bnd_BoundSortBox& theBoundSB);
  
  //! Constructs  and computes  an  interference   between    the
  //! Straight Line and the Polyhedron.
  Standard_EXPORT IntCurveSurface_TheInterferenceOfHInter(const gp_Lin& theLin, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, Bnd_BoundSortBox& theBoundSB);
  
  //! Constructs   and   computes  an  interference   between the
  //! Straight Lines and the Polyhedron.
  Standard_EXPORT IntCurveSurface_TheInterferenceOfHInter(const Intf_Array1OfLin& theLins, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, Bnd_BoundSortBox& theBoundSB);
  
  //! Computes  an interference    between the   Polygon  and the
  //! Polyhedron.
  Standard_EXPORT void Perform (const IntCurveSurface_ThePolygonOfHInter& thePolyg, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, Bnd_BoundSortBox& theBoundSB);
  
  //! Computes an interference between the Straight  Line and the
  //! Polyhedron.
  Standard_EXPORT void Perform (const gp_Lin& theLin, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, Bnd_BoundSortBox& theBoundSB);
  
  //! Computes an interference  between the  Straight Lines  and
  //! the Polyhedron.
  Standard_EXPORT void Perform (const Intf_Array1OfLin& theLins, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, Bnd_BoundSortBox& theBoundSB);
  
  //! Compares the boundings between the segment of <thePolyg> and
  //! the facets of <thePolyh>.
  Standard_EXPORT void Interference (const IntCurveSurface_ThePolygonOfHInter& thePolyg, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, Bnd_BoundSortBox& theBoundSB);
  
  //! Compares the boundings between the segment of <thePolyg> and
  //! the facets of <thePolyh>.
  Standard_EXPORT void Interference (const IntCurveSurface_ThePolygonOfHInter& thePolyg, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);




protected:





private:

  
  //! Computes the intersection between  the segment <BegO><EndO>
  //! and the triangle <TTri> of <thePolyh>.
  Standard_EXPORT void Intersect (const gp_Pnt& BegO, const gp_Pnt& EndO, const Standard_Boolean Infinite, const Standard_Integer TTri, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
  
  //! Computes the intersection between  the segment <BegO><EndO>
  //! and the triangle <TTri> of <thePolyh>.
  Standard_EXPORT void Intersect (const gp_Pnt& BegO, const gp_Pnt& EndO, const Standard_Boolean Infinite, const Standard_Integer TTri, const IntCurveSurface_ThePolyhedronOfHInter& thePolyh, const gp_XYZ& TriNormal, const Standard_Real TriDp, const Standard_Real dBegTri, const Standard_Real dEndTri);


  Standard_Boolean BeginOfClosedPolygon;
  Standard_Integer iLin;


};







#endif // _IntCurveSurface_TheInterferenceOfHInter_HeaderFile
