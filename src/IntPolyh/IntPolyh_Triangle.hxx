// Created on: 1999-03-05
// Created by: Fabrice SERVANT
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IntPolyh_Triangle_HeaderFile
#define _IntPolyh_Triangle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <IntPolyh_ArrayOfPoints.hxx>
#include <IntPolyh_ArrayOfTriangles.hxx>
#include <IntPolyh_ArrayOfEdges.hxx>
#include <IntPolyh_ArrayOfCouples.hxx>
class Adaptor3d_HSurface;
class Bnd_Box;
class IntPolyh_StartPoint;



class IntPolyh_Triangle 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntPolyh_Triangle();
  
  Standard_EXPORT IntPolyh_Triangle(const Standard_Integer i1, const Standard_Integer i2, const Standard_Integer i3);
  
  Standard_EXPORT Standard_Integer FirstPoint() const;
  
  Standard_EXPORT Standard_Integer SecondPoint() const;
  
  Standard_EXPORT Standard_Integer ThirdPoint() const;
  
  Standard_EXPORT Standard_Integer FirstEdge() const;
  
  Standard_EXPORT Standard_Integer FirstEdgeOrientation() const;
  
  Standard_EXPORT Standard_Integer SecondEdge() const;
  
  Standard_EXPORT Standard_Integer SecondEdgeOrientation() const;
  
  Standard_EXPORT Standard_Integer ThirdEdge() const;
  
  Standard_EXPORT Standard_Integer ThirdEdgeOrientation() const;
  
  Standard_EXPORT Standard_Real GetFleche() const;
  
  Standard_EXPORT Standard_Integer IndiceIntersectionPossible() const;
  
  Standard_EXPORT Standard_Integer IndiceIntersection() const;
  
  Standard_EXPORT void SetFirstPoint (const Standard_Integer v);
  
  Standard_EXPORT void SetSecondPoint (const Standard_Integer v);
  
  Standard_EXPORT void SetThirdPoint (const Standard_Integer v);
  
  Standard_EXPORT void SetFirstEdge (const Standard_Integer v, const Standard_Integer s);
  
  Standard_EXPORT void SetSecondEdge (const Standard_Integer v, const Standard_Integer s);
  
  Standard_EXPORT void SetThirdEdge (const Standard_Integer v, const Standard_Integer s);
  
  Standard_EXPORT void SetFleche (const Standard_Real v);
  
  Standard_EXPORT void SetIndiceIntersectionPossible (const Standard_Integer v);
  
  Standard_EXPORT void SetIndiceIntersection (const Standard_Integer v);
  
  Standard_EXPORT Standard_Integer GetEdgeNumber (const Standard_Integer v) const;
  
  Standard_EXPORT void SetEdge (const Standard_Integer v, const Standard_Integer en);
  
  Standard_EXPORT Standard_Integer GetEdgeOrientation (const Standard_Integer v) const;
  
  Standard_EXPORT void SetEdgeOrientation (const Standard_Integer v, const Standard_Integer oe);
  
  Standard_EXPORT void TriangleDeflection (const Handle(Adaptor3d_HSurface)& MaSurface, const IntPolyh_ArrayOfPoints& TP);
  
  Standard_EXPORT Standard_Integer CheckCommonEdge (const Standard_Integer PE1, const Standard_Integer PE2, const Standard_Integer P3, const Standard_Integer Index, const IntPolyh_ArrayOfTriangles& TTriangles) const;
  
  Standard_EXPORT Standard_Integer GetNextTriangle2 (const Standard_Integer NumTri, const Standard_Integer NumEdge, const IntPolyh_ArrayOfEdges& TEdges) const;
  
  Standard_EXPORT void MiddleRefinement (const Standard_Integer TriangleNumber, const Handle(Adaptor3d_HSurface)& MySurface, IntPolyh_ArrayOfPoints& TPoints, IntPolyh_ArrayOfTriangles& TTriangles, IntPolyh_ArrayOfEdges& TEdges);
  
  Standard_EXPORT void MultipleMiddleRefinement (const Standard_Integer NombreAffinages, const Standard_Integer TriangleNumber, const Handle(Adaptor3d_HSurface)& MySurface, IntPolyh_ArrayOfPoints& TPoints, IntPolyh_ArrayOfTriangles& TTriangles, IntPolyh_ArrayOfEdges& TEdges);
  
  Standard_EXPORT Standard_Integer CompareBoxTriangle (const Bnd_Box& b, const IntPolyh_ArrayOfPoints& TPoints) const;
  
  Standard_EXPORT void MultipleMiddleRefinement2 (const Standard_Real RefineCriterion, const Bnd_Box& thebox, const Standard_Integer TriangleNumber, const Handle(Adaptor3d_HSurface)& MySurface, IntPolyh_ArrayOfPoints& TPoints, IntPolyh_ArrayOfTriangles& TTriangles, IntPolyh_ArrayOfEdges& TEdges);
  
  Standard_EXPORT Standard_Integer GetNextChainTriangle (const IntPolyh_StartPoint& SPIni, const Standard_Integer LastTTC, IntPolyh_ArrayOfCouples& TriContactsArray, const IntPolyh_ArrayOfTriangles& TTriangles1, const IntPolyh_ArrayOfTriangles& TTriangles2, Standard_Integer& NumContact, Standard_Integer& NextTriangle) const;
  
  Standard_EXPORT void LinkEdges2Triangle (const IntPolyh_ArrayOfEdges& TEdges, const Standard_Integer ed1, const Standard_Integer ed2, const Standard_Integer ed3);
  
  Standard_EXPORT void SetEdgeandOrientation (const Standard_Integer Edge, const IntPolyh_ArrayOfEdges& TEdges);
  
  Standard_EXPORT void Dump (const Standard_Integer v) const;
  
  Standard_EXPORT void DumpFleche (const Standard_Integer v) const;




protected:





private:



  Standard_Integer p1;
  Standard_Integer p2;
  Standard_Integer p3;
  Standard_Integer e1;
  Standard_Integer oe1;
  Standard_Integer e2;
  Standard_Integer oe2;
  Standard_Integer e3;
  Standard_Integer oe3;
  Standard_Integer II;
  Standard_Integer IP;
  Standard_Real Fleche;


};







#endif // _IntPolyh_Triangle_HeaderFile
