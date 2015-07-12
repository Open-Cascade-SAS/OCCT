// Created on: 1995-09-01
// Created by: Yves FRICAUD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepAlgo_FaceRestrictor_HeaderFile
#define _BRepAlgo_FaceRestrictor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
class TopoDS_Face;
class TopoDS_Wire;


//! Builds all  the faces  limited  with a set of non
//! jointing   and     planars     wires.    if
//! <ControlOrientation> is false  The Wires must have
//! correct orientations.  Sinon orientation des wires
//! de telle sorte que les faces ne soient pas infinies
//! et qu'elles soient disjointes.
class BRepAlgo_FaceRestrictor 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepAlgo_FaceRestrictor();
  
  //! the surface of <F> will be the the surface of each new
  //! faces built.
  //! <Proj> is used to update pcurves on edges if necessary.
  //! See Add().
  Standard_EXPORT void Init (const TopoDS_Face& F, const Standard_Boolean Proj = Standard_False, const Standard_Boolean ControlOrientation = Standard_False);
  
  //! Add the wire <W> to the set of wires.
  //!
  //! Warning:
  //! The Wires must be closed.
  //!
  //! The edges of <W> can be modified if they  have not pcurves
  //! on the surface <S>  of <F>. In this  case
  //! if <Proj> is false the first pcurve of  the edge
  //! is positionned on <S>.
  //! if <Proj> is True ,the Pcurve On <S> is the
  //! projection of the  curve 3d on <F>.
  Standard_EXPORT void Add (TopoDS_Wire& W);
  
  //! Removes all the Wires
  Standard_EXPORT void Clear();
  
  //! Evaluate all the faces limited by the set of Wires.
  Standard_EXPORT void Perform();
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT TopoDS_Face Current() const;




protected:





private:

  
  //! Evaluate all the faces limited by the set of Wires.
  Standard_EXPORT void PerformWithCorrection();


  Standard_Boolean myDone;
  Standard_Boolean modeProj;
  TopoDS_Face myFace;
  TopTools_ListOfShape wires;
  TopTools_ListOfShape faces;
  Standard_Boolean myCorrection;
  TopTools_DataMapOfShapeListOfShape keyIsIn;
  TopTools_DataMapOfShapeListOfShape keyContains;


};







#endif // _BRepAlgo_FaceRestrictor_HeaderFile
