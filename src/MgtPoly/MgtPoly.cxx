// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <MgtPoly.ixx>

#include <TColgp_Array1OfPnt.hxx>
#include <PColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <PColgp_HArray1OfPnt2d.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <PColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <PColStd_HArray1OfInteger.hxx>

#include <Poly_Array1OfTriangle.hxx>
#include <PPoly_HArray1OfTriangle.hxx>

#define MgtBRepSpeedDownCast

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the gp_Pnt
//           from an Array1 from TColgp (TCollection)
//           to an HArray1 from PColgp (PCollection)
//=======================================================================

static Handle(PColgp_HArray1OfPnt) ArrayCopy
       (const TColgp_Array1OfPnt& TArray)
{
  Standard_Integer Lower = TArray.Lower();
  Standard_Integer Upper = TArray.Upper();
  Standard_Integer Index;
  Handle(PColgp_HArray1OfPnt) PArray = new PColgp_HArray1OfPnt(Lower, Upper);
  for (Index = Lower; Index <= Upper; Index++) {
    PArray->SetValue(Index, TArray(Index));
  }
  return PArray;
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the gp_Pnt
//           from an HArray1 from PColgp (PCollection)
//           to an Array1 from TColgp (TCollection)
//=======================================================================

static void ArrayCopy(const Handle(PColgp_HArray1OfPnt)& PArray,
		      TColgp_Array1OfPnt& TArray)
{
  Standard_Integer Lower = PArray->Lower();
  Standard_Integer Upper = PArray->Upper();
  Standard_Integer Index;
  for (Index = Lower; Index <= Upper; Index++) {
    TArray(Index) = PArray->Value(Index);
  }
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the gp_Pnt2d
//           from an Array1 from TColgp (TCollection)
//           to an Array1 from PColgp (PCollection)
//=======================================================================

static Handle(PColgp_HArray1OfPnt2d) ArrayCopy
       (const TColgp_Array1OfPnt2d& TArray)
{
  Standard_Integer Lower = TArray.Lower();
  Standard_Integer Upper = TArray.Upper();
  Standard_Integer Index;
  Handle(PColgp_HArray1OfPnt2d) PArray = 
    new PColgp_HArray1OfPnt2d(Lower, Upper);
  for (Index = Lower; Index <= Upper; Index++) {
    PArray->SetValue(Index, TArray(Index));
  }
  return PArray;
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the gp_Pnt2d
//           from an HArray1 from PColgp (PCollection)
//           to an Array1 from TColgp (TCollection)
//=======================================================================

static void ArrayCopy
  (const Handle(PColgp_HArray1OfPnt2d)& PArray,
   TColgp_Array1OfPnt2d& TArray)
{
  Standard_Integer Lower = PArray->Lower();
  Standard_Integer Upper = PArray->Upper();
  Standard_Integer Index;
  for (Index = Lower; Index <= Upper; Index++) {
    TArray(Index) = PArray->Value(Index);
  }
}


//=======================================================================
//function : ArrayCopy
//purpose  : Copy the Triangle
//           from an Array1 from Poly (TCollection)
//           to an HArray1 from PPoly (PCollection)
//=======================================================================

static Handle(PPoly_HArray1OfTriangle) ArrayCopy
(const Poly_Array1OfTriangle& TArray)
{
  Standard_Integer Lower = TArray.Lower();
  Standard_Integer Upper = TArray.Upper();
  Standard_Integer Index;
  Handle(PPoly_HArray1OfTriangle) PArray = 
    new PPoly_HArray1OfTriangle(Lower, Upper);
  for (Index = Lower; Index <= Upper; Index++) {
    PPoly_Triangle aPTriangle = MgtPoly::Translate(TArray(Index));
    PArray->SetValue(Index, aPTriangle);
  }
  return PArray;
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy the Triangle
//           from an HArray1 from PPoly (PCollection)
//           to an Array1 from Poly (TCollection)
//=======================================================================

static void ArrayCopy
  (const Handle(PPoly_HArray1OfTriangle)& PArray,
   Poly_Array1OfTriangle& TArray)
{
  Standard_Integer Lower = PArray->Lower();
  Standard_Integer Upper = PArray->Upper();
  Standard_Integer Index;
  for (Index = Lower; Index <= Upper; Index++) {
    Poly_Triangle aTTriangle = MgtPoly::Translate(PArray->Value(Index));
    TArray(Index) = aTTriangle;
  }
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy PColStd_HArray1OfInteger to TColStd_Array1OfInteger
//=======================================================================

static void ArrayCopy(const Handle(PColStd_HArray1OfInteger)& PArray,
		      TColStd_Array1OfInteger&                TArray)
{
  Standard_Integer Lower = PArray->Lower();
  Standard_Integer Upper = PArray->Upper();
  Standard_Integer Index;
  for (Index = Lower; Index <= Upper; Index++) {
    TArray(Index) = PArray->Value(Index);
  }
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy TColStd_Array1OfInteger to PColStd_HArray1OfInteger
//=======================================================================

static Handle(PColStd_HArray1OfInteger) 
ArrayCopy(const TColStd_Array1OfInteger& TArray)
{
  Standard_Integer Lower = TArray.Lower();
  Standard_Integer Upper = TArray.Upper();
  Standard_Integer Index;
  Handle(PColStd_HArray1OfInteger) PArray = 
    new PColStd_HArray1OfInteger(Lower, Upper);
  for (Index = Lower; Index <= Upper; Index++) {
    PArray->SetValue(Index, TArray.Value(Index));
  }
  return PArray;
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy PColStd_HArray1OfReal to TColStd_Array1OfReal
//=======================================================================

static void ArrayCopy(const Handle(PColStd_HArray1OfReal)& PArray,
		      TColStd_Array1OfReal&                TArray)
{
  Standard_Integer Lower = PArray->Lower();
  Standard_Integer Upper = PArray->Upper();
  Standard_Integer Index;
  for (Index = Lower; Index <= Upper; Index++) {
    TArray(Index) = PArray->Value(Index);
  }
}

//=======================================================================
//function : ArrayCopy
//purpose  : Copy TColStd_Array1OfReal to PColStd_HArray1OfReal
//=======================================================================

static Handle(PColStd_HArray1OfReal) 
ArrayCopy(const TColStd_Array1OfReal& TArray)
{
  Standard_Integer Lower = TArray.Lower();
  Standard_Integer Upper = TArray.Upper();
  Standard_Integer Index;
  Handle(PColStd_HArray1OfReal) PArray = 
    new PColStd_HArray1OfReal(Lower, Upper);
  for (Index = Lower; Index <= Upper; Index++) {
    PArray->SetValue(Index, TArray.Value(Index));
  }
  return PArray;
}


//=======================================================================
//function : 
//purpose  : Persistent -> Transient
//=======================================================================

Poly_Triangle MgtPoly::Translate(const PPoly_Triangle& PObj)
{
  Standard_Integer n1, n2, n3;
  PObj.Get(n1,n2,n3);
  Poly_Triangle TT(n1, n2, n3);
  return TT;
}

//=======================================================================
//function : 
//purpose  : Transient -> Persistent
//=======================================================================

PPoly_Triangle MgtPoly::Translate(const Poly_Triangle& TObj)
{
  Standard_Integer n1, n2, n3;
  TObj.Get(n1,n2,n3);
  PPoly_Triangle PT(n1, n2, n3);
  return PT;
}

//=======================================================================
//function : Translate Triangulation
//purpose  : Persistent -> Transient
//=======================================================================

Handle(Poly_Triangulation) MgtPoly::Translate
(const Handle(PPoly_Triangulation)& PObj, 
 PTColStd_PersistentTransientMap& aMap)
{
  Handle(Poly_Triangulation) TT;
  if (!PObj.IsNull()) {
    if (aMap.IsBound(PObj)) {

#ifdef MgtBRepSpeedDownCast
      Handle(Standard_Transient) aTrans = aMap.Find(PObj);
      TT = (Handle(Poly_Triangulation)&) aTrans;
#else
      TT = Handle(Poly_Triangulation)::DownCast(aMap.Find(PObj));
#endif
    }
    else {              // translate and bind
      
      // myNodes
      Handle(PColgp_HArray1OfPnt) PNodes = PObj->Nodes();
      TColgp_Array1OfPnt TNodes (PNodes->Lower(), PNodes->Upper());
      ArrayCopy(PNodes, TNodes);

      // myTriangles
      Handle(PPoly_HArray1OfTriangle) PTriangles = PObj->Triangles();
      Poly_Array1OfTriangle TTriangles (PTriangles->Lower(), 
					PTriangles->Upper());
      ArrayCopy(PTriangles, TTriangles);
      
      // myUVNodes
      if (PObj->HasUVNodes()) {
	Handle(PColgp_HArray1OfPnt2d) PUVNodes = PObj->UVNodes();
	TColgp_Array1OfPnt2d TUVNodes(PUVNodes->Lower(), PUVNodes->Upper());
	ArrayCopy(PUVNodes, TUVNodes);
	// Constructor
	TT = new Poly_Triangulation(TNodes, TUVNodes, TTriangles);
      }
      else {
	// Constructor
	TT = new Poly_Triangulation(TNodes, TTriangles);
      }
      // Set deflection
      TT->Deflection(PObj->Deflection());
      aMap.Bind(PObj, TT);
    }
  }
  return TT;
}

//=======================================================================
//function : Translate Triangulation
//purpose  : Transient -> Persistent
//=======================================================================

Handle(PPoly_Triangulation) MgtPoly::Translate
(const Handle(Poly_Triangulation)& TObj, 
 PTColStd_TransientPersistentMap& aMap)
{
  Handle(PPoly_Triangulation) PT;
  if (!TObj.IsNull()) {
    if (aMap.IsBound(TObj)) {
#ifdef MgtBRepSpeedDownCast
      Handle(Standard_Persistent) aPers = aMap.Find(TObj);
      PT = (Handle(PPoly_Triangulation)&) aPers;
#else
      PT = Handle(PPoly_Triangulation)::DownCast(aMap.Find(TObj));
#endif      
    }
    else {
      // myNodes
      const TColgp_Array1OfPnt& TNodes = TObj->Nodes();
      Handle(PColgp_HArray1OfPnt) PNodes = 
	new PColgp_HArray1OfPnt(TNodes.Lower(), 
				TNodes.Upper());
      PNodes = ArrayCopy(TNodes);
      
      // myTriangles
      const Poly_Array1OfTriangle& TTriangle = TObj->Triangles();
      Handle(PPoly_HArray1OfTriangle) PTriangle =
	new PPoly_HArray1OfTriangle(TTriangle.Lower(), 
				    TTriangle.Upper());
      PTriangle = ArrayCopy(TTriangle);
      
      // myUVNodes
      Handle(PColgp_HArray1OfPnt2d) PUVNodes;
      if (TObj->HasUVNodes()) {
	const TColgp_Array1OfPnt2d& TUVNodes = TObj->UVNodes();
	PUVNodes = 
	  new PColgp_HArray1OfPnt2d(TUVNodes.Lower(), 
				    TUVNodes.Upper());
	PUVNodes = ArrayCopy(TUVNodes);
      }
      // Constructor + Deflection
      PT = new PPoly_Triangulation(TObj->Deflection(),
				   PNodes, PUVNodes, PTriangle);
      aMap.Bind(TObj, PT);
    }
  }
  return PT;
}

//=======================================================================
//function : Translate Polygon3D
//purpose  : Persistent -> Transient
//=======================================================================

Handle(Poly_Polygon3D) MgtPoly::Translate
(const Handle(PPoly_Polygon3D)& PObj, 
 PTColStd_PersistentTransientMap& aMap)
{
  Handle(Poly_Polygon3D) TP;
  if (!PObj.IsNull()) {
    if (aMap.IsBound(PObj)) {
#ifdef MgtBRepSpeedDownCast
      Handle(Standard_Transient) aTrans = aMap.Find(PObj);
      TP = (Handle(Poly_Polygon3D)&) aTrans;
#else
      TP = Handle(Poly_Polygon3D)::DownCast(aMap.Find(PObj));
#endif      
    }
    else {

      // myNodes
      Handle(PColgp_HArray1OfPnt) PNodes = PObj->Nodes();
      TColgp_Array1OfPnt TNodes(PNodes->Lower(), PNodes->Upper());
      ArrayCopy(PNodes, TNodes);
      
      // myParameters

      Handle(PColStd_HArray1OfReal) PParam = PObj->Parameters();
      TColStd_Array1OfReal TParam(PParam->Lower(), PParam->Upper());
      ArrayCopy(PParam, TParam);

      // Constructor + Deflection
      TP = new Poly_Polygon3D(TNodes, TParam);
      TP->Deflection(PObj->Deflection());
      aMap.Bind(PObj, TP);
    }
  }
  return TP;
}

//=======================================================================
//function : Translate Polygon3D
//purpose  : Transient -> Persistent
//=======================================================================

Handle(PPoly_Polygon3D) MgtPoly::Translate
(const Handle(Poly_Polygon3D)& TObj, 
 PTColStd_TransientPersistentMap& aMap)
{
  Handle(PPoly_Polygon3D) PT;
  if (!TObj.IsNull()) {
    if (aMap.IsBound(TObj)) {
#ifdef MgtBRepSpeedDownCast
      Handle(Standard_Persistent) aPers = aMap.Find(TObj);
      PT = (Handle(PPoly_Polygon3D)&) aPers;
#else
      PT = Handle(PPoly_Polygon3D)::DownCast(aMap.Find(TObj));
#endif      
    }
    else {
      // myNodes
      const TColgp_Array1OfPnt& TNodes = TObj->Nodes();
      Handle(PColgp_HArray1OfPnt) PNodes = 
	new PColgp_HArray1OfPnt(TNodes.Lower(), 
				TNodes.Upper());
      PNodes = ArrayCopy(TNodes);

      // myParameters
      const TColStd_Array1OfReal& TParam = TObj->Parameters();
      Handle(PColStd_HArray1OfReal) PParam = 
	new PColStd_HArray1OfReal(TParam.Lower(), TParam.Upper());
      PParam = ArrayCopy(TParam);

      // constructor + Deflection
      PT = new PPoly_Polygon3D(PNodes, PParam, TObj->Deflection());
      aMap.Bind(TObj, PT);
    }
  }
  return PT;  
}

//=======================================================================
//function : Translate Polygon2D
//purpose  : Persistent -> Transient
//=======================================================================

Handle(Poly_Polygon2D) MgtPoly::Translate
(const Handle(PPoly_Polygon2D)& PObj, 
 PTColStd_PersistentTransientMap& aMap)
{
  Handle(Poly_Polygon2D) TP;
  if (!PObj.IsNull()) {
    if (aMap.IsBound(PObj)) {
#ifdef MgtBRepSpeedDownCast
      Handle(Standard_Transient) aTrans = aMap.Find(PObj);
      TP = (Handle(Poly_Polygon2D)&) aTrans;
#else
      TP = Handle(Poly_Polygon2D)::DownCast(aMap.Find(PObj));
#endif      
    }
    else {
      // myNodes
      Handle(PColgp_HArray1OfPnt2d) PUVNodes = PObj->Nodes();
      TColgp_Array1OfPnt2d TUVNodes(PUVNodes->Lower(), PUVNodes->Upper());
      ArrayCopy(PUVNodes, TUVNodes);

      // Constructor + Deflection
      TP = new Poly_Polygon2D(TUVNodes);
      TP->Deflection(PObj->Deflection());
      aMap.Bind(PObj, TP);
    }
  }
  return TP;
}

//=======================================================================
//function : Translate Polygon2D
//purpose  : Transient -> Persistent
//=======================================================================

Handle(PPoly_Polygon2D) MgtPoly::Translate
(const Handle(Poly_Polygon2D)& TObj, 
 PTColStd_TransientPersistentMap& aMap)
{
  Handle(PPoly_Polygon2D) PT;
  if (!TObj.IsNull()) {
    if (aMap.IsBound(TObj)) {
#ifdef MgtBRepSpeedDownCast
      Handle(Standard_Persistent) aPers = aMap.Find(TObj);
      PT = (Handle(PPoly_Polygon2D)&) aPers;
#else
      PT = Handle(PPoly_Polygon2D)::DownCast(aMap.Find(TObj));
#endif      
    }
    else {
      // myNodes
      const TColgp_Array1OfPnt2d& TUVNodes = TObj->Nodes();
      Handle(PColgp_HArray1OfPnt2d) PUVNodes = 
	new PColgp_HArray1OfPnt2d(TUVNodes.Lower(), 
				  TUVNodes.Upper());
      PUVNodes = ArrayCopy(TUVNodes);
      
      // constructor + Deflection
      PT = new PPoly_Polygon2D(PUVNodes, TObj->Deflection());
      aMap.Bind(TObj, PT);
    }
  }
  return PT;
}

//=======================================================================
//function : Translate PolygonOnTriangulation
//purpose  : Persistent -> Transient
//=======================================================================

Handle(Poly_PolygonOnTriangulation) MgtPoly::Translate
(const Handle(PPoly_PolygonOnTriangulation)& PObj, 
 PTColStd_PersistentTransientMap& aMap)
{
  Handle(Poly_PolygonOnTriangulation) TP;
  if (!PObj.IsNull()) {
    if (aMap.IsBound(PObj)) {
#ifdef MgtBRepSpeedDownCast
      Handle(Standard_Transient) aTrans = aMap.Find(PObj);
      TP = (Handle(Poly_PolygonOnTriangulation)&) aTrans;
#else
      TP = Handle(Poly_PolygonOnTriangulation)::DownCast(aMap.Find(PObj));
#endif      
    }
    else {

      // myNodes
      Handle(PColStd_HArray1OfInteger) PNodes = PObj->Nodes();
      TColStd_Array1OfInteger TNodes(PNodes->Lower(), PNodes->Upper());
      ArrayCopy(PNodes, TNodes);

      // myParameters

      if (PObj->HasParameters()) {
	Handle(PColStd_HArray1OfReal) PParam = PObj->Parameters();
	TColStd_Array1OfReal TParam(PParam->Lower(), PParam->Upper());
	ArrayCopy(PParam, TParam);
	
	// Constructor + Deflection
	TP = new Poly_PolygonOnTriangulation(TNodes, TParam);
	TP->Deflection(PObj->Deflection());
	aMap.Bind(PObj, TP);
      }
      else {
	TP = new Poly_PolygonOnTriangulation(TNodes);
	TP->Deflection(PObj->Deflection());
	aMap.Bind(PObj, TP);
      }
    }
  }
  return TP;
}

//=======================================================================
//function : Translate PolygonOnTriangulation
//purpose  : Transient -> Persistent
//=======================================================================

Handle(PPoly_PolygonOnTriangulation) MgtPoly::Translate
(const Handle(Poly_PolygonOnTriangulation)& TObj, 
 PTColStd_TransientPersistentMap& aMap)
{
  Handle(PPoly_PolygonOnTriangulation) PT;
  if (!TObj.IsNull()) {
    if (aMap.IsBound(TObj)) {
#ifdef MgtBRepSpeedDownCast
      Handle(Standard_Persistent) aPers = aMap.Find(TObj);
      PT = (Handle(PPoly_PolygonOnTriangulation)&) aPers;
#else
      PT = Handle(PPoly_PolygonOnTriangulation)::DownCast(aMap.Find(TObj));
#endif      
    }
    else {
      // myNodes
      const TColStd_Array1OfInteger& TNodes = TObj->Nodes();
      Handle(PColStd_HArray1OfInteger) PNodes = 
	new PColStd_HArray1OfInteger(TNodes.Lower(), 
				     TNodes.Upper());
      PNodes = ArrayCopy(TNodes);
      
      // myParameters

      if (TObj->HasParameters()) {
	const TColStd_Array1OfReal& TParam = TObj->Parameters()->Array1();
	Handle(PColStd_HArray1OfReal) PParam =
	  new PColStd_HArray1OfReal(TParam.Lower(), TParam.Upper());
	PParam = ArrayCopy(TParam);
	// constructor + Deflection
	PT = new PPoly_PolygonOnTriangulation(PNodes, TObj->Deflection(),
					      PParam);
	aMap.Bind(TObj, PT);
	
      }
      else {
	// constructor + Deflection
	PT = new PPoly_PolygonOnTriangulation(PNodes, TObj->Deflection());
	aMap.Bind(TObj, PT);
      }
    }
  }
  return PT;
}
