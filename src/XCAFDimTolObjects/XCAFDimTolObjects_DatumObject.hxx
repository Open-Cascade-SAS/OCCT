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


#ifndef _XCAFDimTolObjects_DatumObject_HeaderFile
#define _XCAFDimTolObjects_DatumObject_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XCAFDimTolObjects_DatumObjectSequence.hxx>
#include <XCAFDimTolObjects_DatumTargetType.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFDimTolObjects_DatumModifiersSequence.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>
#include <Standard_Real.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <XCAFDimTolObjects_DatumSingleModif.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Ax2.hxx>

class XCAFDimTolObjects_DatumObject;
DEFINE_STANDARD_HANDLE(XCAFDimTolObjects_DatumObject, Standard_Transient)

//! object to store datum
class XCAFDimTolObjects_DatumObject : public Standard_Transient
{

public:

  Standard_EXPORT XCAFDimTolObjects_DatumObject();
  
  Standard_EXPORT XCAFDimTolObjects_DatumObject(const Handle(XCAFDimTolObjects_DatumObject)& theObj);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetName() const;
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& theTag);
  
  Standard_EXPORT XCAFDimTolObjects_DatumModifiersSequence GetModifiers() const;
  
  Standard_EXPORT void SetModifiers (const XCAFDimTolObjects_DatumModifiersSequence& theModifiers);
  
  Standard_EXPORT void GetModifierWithValue (XCAFDimTolObjects_DatumModifWithValue& theModifier, Standard_Real& theValue) const;
  
  Standard_EXPORT void SetModifierWithValue (const XCAFDimTolObjects_DatumModifWithValue theModifier, const Standard_Real theValue);
  
  Standard_EXPORT void AddModifier (const XCAFDimTolObjects_DatumSingleModif theModifier);
  
  Standard_EXPORT TopoDS_Shape GetDatumTarget() const;
  
  Standard_EXPORT void SetDatumTarget (const TopoDS_Shape& theShape);

  Standard_EXPORT Standard_Integer GetPosition () const;
  
  Standard_EXPORT void SetPosition (const Standard_Integer thePosition);
  
  Standard_EXPORT Standard_Boolean IsDatumTarget() const;

  Standard_EXPORT void IsDatumTarget(const Standard_Boolean theIsDT);

  Standard_EXPORT XCAFDimTolObjects_DatumTargetType GetDatumTargetType() const;

  Standard_EXPORT void SetDatumTargetType (const XCAFDimTolObjects_DatumTargetType theType);

  Standard_EXPORT gp_Ax2 GetDatumTargetAxis() const;

  Standard_EXPORT void SetDatumTargetAxis (const gp_Ax2& theAxis);

  Standard_EXPORT Standard_Real GetDatumTargetLength() const;

  Standard_EXPORT void SetDatumTargetLength (const Standard_Real theLength);

  Standard_EXPORT Standard_Real GetDatumTargetWidth() const;

  Standard_EXPORT void SetDatumTargetWidth (const Standard_Real theWidth);

  Standard_EXPORT Standard_Integer GetDatumTargetNumber() const;

  Standard_EXPORT void SetDatumTargetNumber (const Standard_Integer theNumber);

   Standard_EXPORT void SetPlane (const gp_Ax2& thePlane)
  {
    myPlane = thePlane;
    myHasPlane = Standard_True;
  }

   Standard_EXPORT const gp_Ax2& GetPlane() const { return myPlane; }

   Standard_EXPORT void SetPoint (const gp_Pnt& thePnt)
  {
    myPnt = thePnt;
    myHasPnt = Standard_True;
  }

   Standard_EXPORT const gp_Pnt& GetPoint() const 
  { 
    return myPnt; 
  }

 
   Standard_EXPORT void SetPointTextAttach (const gp_Pnt& thePntText)
  {
    myPntText = thePntText;
    myHasPntText = Standard_True;
  }

   Standard_EXPORT const gp_Pnt& GetPointTextAttach() const 
  { 
    return myPntText; 
  }

  Standard_Boolean HasPlane() const { return myHasPlane; }

  Standard_Boolean HasPoint() const { return myHasPnt; }

   Standard_EXPORT Standard_Boolean HasPointText() const 
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

  //! Returns true if datum has valid parameters for datum target (width, length, circle radius etc)
  Standard_EXPORT Standard_Boolean HasDatumTargetParams()
  {
    return myIsValidDT;
  }

  DEFINE_STANDARD_RTTIEXT(XCAFDimTolObjects_DatumObject,Standard_Transient)

private: 

  Handle(TCollection_HAsciiString) myName;
  XCAFDimTolObjects_DatumModifiersSequence myModifiers;
  XCAFDimTolObjects_DatumModifWithValue myModifierWithValue;
  Standard_Real myValueOfModifier;
  TopoDS_Shape myDatumTarget;
  Standard_Integer myPosition;
  Standard_Boolean myIsDTarget;
  Standard_Boolean myIsValidDT;
  XCAFDimTolObjects_DatumTargetType myDTargetType;
  gp_Ax2 myAxis;
  Standard_Real myLength;
  Standard_Real myWidth;
  Standard_Integer myDatumTargetNumber;
  gp_Ax2 myPlane;
  gp_Pnt myPnt;
  gp_Pnt myPntText;
  Standard_Boolean myHasPlane;
  Standard_Boolean myHasPnt;
  Standard_Boolean myHasPntText;
  TopoDS_Shape myPresentation;
   Handle(TCollection_HAsciiString) myPresentationName;
};

#endif // _XCAFDimTolObjects_DatumObject_HeaderFile
