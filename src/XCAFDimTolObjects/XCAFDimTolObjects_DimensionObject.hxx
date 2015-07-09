
// Created on: 2015-08-06
// Created by: Ilya Novikov
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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


#ifndef _XCAFDimTolObjects_DimensionObject_HeaderFile
#define _XCAFDimTolObjects_DimensionObject_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XCAFDimTolObjects_DimensionObjectSequence.hxx>
#include <XCAFDimTolObjects_DimensionType.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <XCAFDimTolObjects_DimensionQualifier.hxx>
#include <Standard_Boolean.hxx>
#include <XCAFDimTolObjects_DimensionFormVariance.hxx>
#include <XCAFDimTolObjects_DimensionGrade.hxx>
#include <Standard_Integer.hxx>
#include <XCAFDimTolObjects_DimensionModifiersSequence.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Dir.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <XCAFDimTolObjects_DimensionModif.hxx>

class XCAFDimTolObjects_DimensionObject;
DEFINE_STANDARD_HANDLE(XCAFDimTolObjects_DimensionObject, Standard_Transient)
                            
//! object to store dimension
class XCAFDimTolObjects_DimensionObject : public Standard_Transient
{

public:
  
  Standard_EXPORT XCAFDimTolObjects_DimensionObject();
  
  Standard_EXPORT XCAFDimTolObjects_DimensionObject(const Handle(XCAFDimTolObjects_DimensionObject)& theObj);
  
  Standard_EXPORT void SetQualifier (const XCAFDimTolObjects_DimensionQualifier theQualifier);
  
  Standard_EXPORT XCAFDimTolObjects_DimensionQualifier GetQualifier() const;
  
  Standard_EXPORT Standard_Boolean HasQualifier() const;
  
  Standard_EXPORT void SetType (const XCAFDimTolObjects_DimensionType theTyupe);
  
  Standard_EXPORT XCAFDimTolObjects_DimensionType GetType() const;
  
  Standard_EXPORT Standard_Real GetValue() const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetValues() const;
  
  Standard_EXPORT void SetValue (const Standard_Real theValue);
  
  Standard_EXPORT void SetValues (const Handle(TColStd_HArray1OfReal)& theValue);
  
  Standard_EXPORT Standard_Boolean IsDimWithRange() const;
  
  Standard_EXPORT void SetUpperBound (const Standard_Real theUpperBound);
  
  Standard_EXPORT void SetLowerBound (const Standard_Real theLowerBound);
  
  Standard_EXPORT Standard_Real GetUpperBound() const;
  
  Standard_EXPORT Standard_Real GetLowerBound() const;
  
  Standard_EXPORT Standard_Boolean IsDimWithPlusMinusTolerance() const;
  
  Standard_EXPORT Standard_Boolean SetUpperTolValue (const Standard_Real theUperTolValue);
  
  Standard_EXPORT Standard_Boolean SetLowerTolValue (const Standard_Real theLowerTolValue);
  
  Standard_EXPORT Standard_Real GetUpperTolValue() const;
  
  Standard_EXPORT Standard_Real GetLowerTolValue() const;
  
  Standard_EXPORT Standard_Boolean IsDimWithClassOfTolerance() const;
  
  Standard_EXPORT void SetClassOfTolerance (const Standard_Boolean theHole, const XCAFDimTolObjects_DimensionFormVariance theFormVariance, const XCAFDimTolObjects_DimensionGrade theGrade);
  
  Standard_EXPORT Standard_Boolean GetClassOfTolerance (Standard_Boolean& theHole, XCAFDimTolObjects_DimensionFormVariance& theFormVariance, XCAFDimTolObjects_DimensionGrade& theGrade) const;
  
  Standard_EXPORT void SetNbOfDecimalPlaces (const Standard_Integer theL, const Standard_Integer theR);
  
  Standard_EXPORT void GetNbOfDecimalPlaces (Standard_Integer& theL, Standard_Integer& theR) const;
  
  Standard_EXPORT XCAFDimTolObjects_DimensionModifiersSequence GetModifiers() const;
  
  Standard_EXPORT void SetModifiers (const XCAFDimTolObjects_DimensionModifiersSequence& theModifiers);
  
  Standard_EXPORT void AddModifier (const XCAFDimTolObjects_DimensionModif theModifier);
  
  Standard_EXPORT TopoDS_Edge GetPath() const;
  
  Standard_EXPORT void SetPath (const TopoDS_Edge& thePath);
  
  Standard_EXPORT Standard_Boolean GetDirection (gp_Dir& theDir) const;
  
  Standard_EXPORT Standard_Boolean SetDirection (const gp_Dir& theDir);
  
  Standard_EXPORT Handle(TColgp_HArray1OfPnt) GetPoints() const;
  
  Standard_EXPORT void SetPoints (const Handle(TColgp_HArray1OfPnt)& thePnts);


  DEFINE_STANDARD_RTTI(XCAFDimTolObjects_DimensionObject,Standard_Transient)

private: 

  XCAFDimTolObjects_DimensionType myType;
  Handle(TColStd_HArray1OfReal) myVal;
  XCAFDimTolObjects_DimensionQualifier myQualifier;
  Standard_Boolean myIsHole;
  XCAFDimTolObjects_DimensionFormVariance myFormVariance;
  XCAFDimTolObjects_DimensionGrade myGrade;
  Standard_Integer myL;
  Standard_Integer myR;
  XCAFDimTolObjects_DimensionModifiersSequence myModifiers;
  TopoDS_Edge myPath;
  gp_Dir myDir;
  Handle(TColgp_HArray1OfPnt) myPnts;

};

#endif // _XCAFDimTolObjects_DimensionObject_HeaderFile
