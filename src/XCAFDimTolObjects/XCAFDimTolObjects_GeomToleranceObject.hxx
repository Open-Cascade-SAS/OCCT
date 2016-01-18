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


#ifndef _XCAFDimTolObjects_GeomToleranceObject_HeaderFile
#define _XCAFDimTolObjects_GeomToleranceObject_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XCAFDimTolObjects_GeomToleranceObjectSequence.hxx>
#include <XCAFDimTolObjects_GeomToleranceType.hxx>
#include <XCAFDimTolObjects_GeomToleranceTypeValue.hxx>
#include <Standard_Real.hxx>
#include <XCAFDimTolObjects_GeomToleranceMatReqModif.hxx>
#include <XCAFDimTolObjects_GeomToleranceZoneModif.hxx>
#include <XCAFDimTolObjects_GeomToleranceModifiersSequence.hxx>
#include <Standard_Transient.hxx>
#include <XCAFDimTolObjects_GeomToleranceModif.hxx>
#include <gp_Ax2.hxx>
#include <TopoDS_Shape.hxx>
#include <TCollection_HAsciiString.hxx>

class XCAFDimTolObjects_GeomToleranceObject;
DEFINE_STANDARD_HANDLE(XCAFDimTolObjects_GeomToleranceObject, Standard_Transient)

//! attribute to store dimension and tolerance
class XCAFDimTolObjects_GeomToleranceObject : public Standard_Transient
{

public:
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceObject();
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceObject(const Handle(XCAFDimTolObjects_GeomToleranceObject)& theObj);
  
  Standard_EXPORT void SetType (const XCAFDimTolObjects_GeomToleranceType theType);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceType GetType() const;
  
  Standard_EXPORT void SetTypeOfValue (const XCAFDimTolObjects_GeomToleranceTypeValue theTypeOfValue);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceTypeValue GetTypeOfValue() const;
  
  Standard_EXPORT void SetValue (const Standard_Real theValue);
  
  Standard_EXPORT Standard_Real GetValue() const;
  
  Standard_EXPORT void SetMaterialRequirementModifier (const XCAFDimTolObjects_GeomToleranceMatReqModif theMatReqModif);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceMatReqModif GetMaterialRequirementModifier() const;
  
  Standard_EXPORT void SetZoneModifier (const XCAFDimTolObjects_GeomToleranceZoneModif theZoneModif);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceZoneModif GetZoneModifier() const;
  
  Standard_EXPORT void SetValueOfZoneModifier (const Standard_Real theValue);
  
  Standard_EXPORT Standard_Real GetValueOfZoneModifier() const;
  
  Standard_EXPORT void SetModifiers (const XCAFDimTolObjects_GeomToleranceModifiersSequence& theModifiers);
  
  Standard_EXPORT void AddModifier (const XCAFDimTolObjects_GeomToleranceModif theModifier);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceModifiersSequence GetModifiers() const;
  
  Standard_EXPORT void SetMaxValueModifier (const Standard_Real theModifier);
  
  Standard_EXPORT Standard_Real GetMaxValueModifier() const;

  Standard_EXPORT void SetAxis (const gp_Ax2 theAxis);
  
  Standard_EXPORT gp_Ax2 GetAxis() const;
   
  Standard_EXPORT Standard_Boolean HasAxis () const;

  void SetPlane (const gp_Ax2& thePlane)
  {
    myPlane = thePlane;
    myHasPlane = Standard_True;
  }

  const gp_Ax2& GetPlane() const { return myPlane; }

  void SetPoint (const gp_Pnt& thePnt)
  {
    myPnt = thePnt;
    myHasPnt = Standard_True;
  }

  const gp_Pnt& GetPoint() const { return myPnt; }

  void SetPointTextAttach (const gp_Pnt& thePntText)
  {
    myPntText = thePntText;
    myHasPntText = Standard_True;
  }

  const gp_Pnt& GetPointTextAttach() const 
  { 
    return myPntText; 
  }
  Standard_Boolean HasPlane() const { return myHasPlane; }

  Standard_Boolean HasPoint() const { return myHasPnt; }
  
  Standard_Boolean HasPointText() const 
  { 
    return myHasPntText; 
  }

 
   //! Set graphical presentation for object
  Standard_EXPORT void SetPresentation(const TopoDS_Shape& thePresentation, 
    const Handle(TCollection_HAsciiString)& thePresentationName)
  {
    myPresentation = thePresentation;
    myPresentationName = thePresentationName;
  }

  //! Returns graphical presentation of the object
  Standard_EXPORT TopoDS_Shape GetPresentation() const
  {
    return myPresentation;
  }

   //! Returns graphical presentation of the object
  Standard_EXPORT Handle(TCollection_HAsciiString) GetPresentationName() const
  {
    return myPresentationName;
  }

  DEFINE_STANDARD_RTTIEXT(XCAFDimTolObjects_GeomToleranceObject,Standard_Transient)

private: 

  XCAFDimTolObjects_GeomToleranceType myType;
  XCAFDimTolObjects_GeomToleranceTypeValue myTypeOfValue;
  Standard_Real myValue;
  XCAFDimTolObjects_GeomToleranceMatReqModif myMatReqModif;
  XCAFDimTolObjects_GeomToleranceZoneModif myZoneModif;
  Standard_Real myValueOfZoneModif;
  XCAFDimTolObjects_GeomToleranceModifiersSequence myModifiers;
  Standard_Real myMaxValueModif;
  gp_Ax2 myAxis;
  Standard_Boolean myHasAxis;
  gp_Ax2 myPlane;
  gp_Pnt myPnt;
  gp_Pnt myPntText;;
  Standard_Boolean myHasPlane;
  Standard_Boolean myHasPnt;
  Standard_Boolean myHasPntText;
  TopoDS_Shape myPresentation;
  Handle(TCollection_HAsciiString) myPresentationName;

};







#endif // _XCAFDimTolObjects_GeomToleranceObject_HeaderFile
