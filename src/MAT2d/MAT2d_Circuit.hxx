// Created on: 1993-11-18
// Created by: Yves FRICAUD
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

#ifndef _MAT2d_Circuit_HeaderFile
#define _MAT2d_Circuit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MAT2d_DataMapOfIntegerConnexion.hxx>
#include <MAT2d_DataMapOfBiIntSequenceOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <GeomAbs_JoinType.hxx>
#include <Standard_Transient.hxx>
#include <MAT2d_SequenceOfSequenceOfGeometry.hxx>
#include <TColStd_SequenceOfBoolean.hxx>
#include <Standard_Integer.hxx>
#include <MAT2d_SequenceOfConnexion.hxx>
class Geom2d_Geometry;
class MAT2d_Connexion;
class MAT2d_BiInt;
class MAT2d_MiniPath;


class MAT2d_Circuit;
DEFINE_STANDARD_HANDLE(MAT2d_Circuit, Standard_Transient)

//! Constructs a circuit on a set of lines.
//! EquiCircuit gives a Circuit passing by all the lines
//! in a set and all the connexions of the minipath associated.
class MAT2d_Circuit : public Standard_Transient
{

public:

  
  Standard_EXPORT MAT2d_Circuit(const GeomAbs_JoinType aJoinType = GeomAbs_Arc, const Standard_Boolean IsOpenResult = Standard_False);
  
  Standard_EXPORT void Perform (MAT2d_SequenceOfSequenceOfGeometry& aFigure, const TColStd_SequenceOfBoolean& IsClosed, const Standard_Integer IndRefLine, const Standard_Boolean Trigo);
  
  //! Returns the Number of Items .
  Standard_EXPORT Standard_Integer NumberOfItems() const;
  
  //! Returns the item at position <Index> in <me>.
  Standard_EXPORT Handle(Geom2d_Geometry) Value (const Standard_Integer Index) const;
  
  //! Returns the number of items on the line <IndexLine>.
  Standard_EXPORT Standard_Integer LineLength (const Standard_Integer IndexLine) const;
  
  //! Returns the set of index of the items in <me>corresponding
  //! to the curve <IndCurve> on the line <IndLine> from the
  //! initial figure.
  Standard_EXPORT const TColStd_SequenceOfInteger& RefToEqui (const Standard_Integer IndLine, const Standard_Integer IndCurve) const;
  
  //! Returns the Connexion on the item <Index> in me.
  Standard_EXPORT Handle(MAT2d_Connexion) Connexion (const Standard_Integer Index) const;
  
  //! Returns <True> is there is a connexion on the item <Index>
  //! in <me>.
  Standard_EXPORT Standard_Boolean ConnexionOn (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(MAT2d_Circuit,Standard_Transient)

protected:




private:

  
  Standard_EXPORT Standard_Boolean IsSharpCorner (const Handle(Geom2d_Geometry)& Geom1, const Handle(Geom2d_Geometry)& Geom2, const Standard_Real Direction) const;
  
  Standard_EXPORT Standard_Boolean PassByLast (const Handle(MAT2d_Connexion)& C1, const Handle(MAT2d_Connexion)& C2) const;
  
  Standard_EXPORT Standard_Real Side (const Handle(MAT2d_Connexion)& C, const TColGeom2d_SequenceOfGeometry& Line) const;
  
  Standard_EXPORT void UpDateLink (const Standard_Integer IFirst, const Standard_Integer ILine, const Standard_Integer ICurveFirst, const Standard_Integer ICurveLast);
  
  Standard_EXPORT void SortRefToEqui (const MAT2d_BiInt& aBiInt);
  
  Standard_EXPORT void InitOpen (TColGeom2d_SequenceOfGeometry& Line) const;
  
  Standard_EXPORT void InsertCorner (TColGeom2d_SequenceOfGeometry& Line) const;
  
  Standard_EXPORT void DoubleLine (TColGeom2d_SequenceOfGeometry& Line, MAT2d_SequenceOfConnexion& Connexions, const Handle(MAT2d_Connexion)& Father, const Standard_Real Side) const;
  
  Standard_EXPORT void ConstructCircuit (const MAT2d_SequenceOfSequenceOfGeometry& aFigure, const Standard_Integer IndRefLine, const MAT2d_MiniPath& aPath);

  Standard_Real direction;
  TColGeom2d_SequenceOfGeometry geomElements;
  MAT2d_DataMapOfIntegerConnexion connexionMap;
  MAT2d_DataMapOfBiIntSequenceOfInteger linkRefEqui;
  TColStd_SequenceOfInteger linesLength;
  GeomAbs_JoinType myJoinType;
  Standard_Boolean myIsOpenResult;


};







#endif // _MAT2d_Circuit_HeaderFile
