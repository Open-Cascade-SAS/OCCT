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

#include <XCAFDimTolObjects_DatumObjectSequence.hxx>
#include <XCAFDimTolObjects_DatumTargetType.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFDimTolObjects_DatumModifiersSequence.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>
#include <Standard_Real.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <XCAFDimTolObjects_DatumSingleModif.hxx>
#include <gp_Ax2.hxx>

class XCAFDimTolObjects_DatumObject;
DEFINE_STANDARD_HANDLE(XCAFDimTolObjects_DatumObject, Standard_Transient)

//! Access object to store datum
class XCAFDimTolObjects_DatumObject : public Standard_Transient
{

public:

  Standard_EXPORT XCAFDimTolObjects_DatumObject();
  
  Standard_EXPORT XCAFDimTolObjects_DatumObject(const Handle(XCAFDimTolObjects_DatumObject)& theObj);

  //! Returns semantic name
  Standard_EXPORT Handle(TCollection_HAsciiString) GetSemanticName() const;

  //! Sets semantic name
  Standard_EXPORT void SetSemanticName(const Handle(TCollection_HAsciiString)& theName);

  //! Returns datum name.
  Standard_EXPORT Handle(TCollection_HAsciiString) GetName() const;
  
  //! Sets datum name.
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& theTag);
  
  //! Returns a sequence of modifiers of the datum.
  Standard_EXPORT XCAFDimTolObjects_DatumModifiersSequence GetModifiers() const;
  
  //! Sets new sequence of datum modifiers.
  Standard_EXPORT void SetModifiers (const XCAFDimTolObjects_DatumModifiersSequence& theModifiers);
  
  //! Retrieves datum modifier with value.
  Standard_EXPORT void GetModifierWithValue (XCAFDimTolObjects_DatumModifWithValue& theModifier, 
                                             Standard_Real& theValue) const;
  
  //! Sets datum modifier with value.
  Standard_EXPORT void SetModifierWithValue (const XCAFDimTolObjects_DatumModifWithValue theModifier, 
                                             const Standard_Real theValue);
  
  //! Adds a modifier to the datum sequence of modifiers.
  Standard_EXPORT void AddModifier (const XCAFDimTolObjects_DatumSingleModif theModifier);
  
  //! Returns datum target shape.
  Standard_EXPORT TopoDS_Shape GetDatumTarget() const;
  
  //! Sets datum target shape.
  Standard_EXPORT void SetDatumTarget (const TopoDS_Shape& theShape);

  //! Returns datum position in the related geometric tolerance object.
  Standard_EXPORT Standard_Integer GetPosition () const;
  
  //! Sets datum position in the related geometric tolerance object.
  Standard_EXPORT void SetPosition (const Standard_Integer thePosition);
  
  //! Returns True if the datum target is specified.
  Standard_EXPORT Standard_Boolean IsDatumTarget() const;

  //! Sets or drops the datum target indicator.
  Standard_EXPORT void IsDatumTarget(const Standard_Boolean theIsDT);

  //! Returns datum target type
  Standard_EXPORT XCAFDimTolObjects_DatumTargetType GetDatumTargetType() const;

  //! Sets datum target to point, line, rectangle, circle or area type.
  Standard_EXPORT void SetDatumTargetType (const XCAFDimTolObjects_DatumTargetType theType);

  //! Returns datum target axis.
  //! The Z axis of the datum placement denotes the normal of the surface 
  //! pointing away from the material. 
  Standard_EXPORT gp_Ax2 GetDatumTargetAxis() const;

  //! Sets datum target axis.
  Standard_EXPORT void SetDatumTargetAxis (const gp_Ax2& theAxis);

  //! Returns datum target length for line and rectangle types.
  //! The length along the X axis of the datum placement.
  Standard_EXPORT Standard_Real GetDatumTargetLength() const;

  //! Sets datum target length.
  Standard_EXPORT void SetDatumTargetLength (const Standard_Real theLength);

  //! Returns datum target width for rectangle type.
  //! The width along the derived Y axis, with the placement itself positioned
  //! at the centre of the rectangle.
  Standard_EXPORT Standard_Real GetDatumTargetWidth() const;

  //! Sets datum target width.
  Standard_EXPORT void SetDatumTargetWidth (const Standard_Real theWidth);

  //! Returns datum target number.
  Standard_EXPORT Standard_Integer GetDatumTargetNumber() const;

  //! Sets datum target number.
  Standard_EXPORT void SetDatumTargetNumber (const Standard_Integer theNumber);

  //! Sets annotation plane.
  void SetPlane (const gp_Ax2& thePlane)
  {
    myPlane = thePlane;
    myHasPlane = Standard_True;
  }

  //! Returns annotation plane.
  const gp_Ax2& GetPlane() const { return myPlane; }

  //! Sets a point on the datum target shape.
  void SetPoint (const gp_Pnt& thePnt)
  {
    myPnt = thePnt;
    myHasPnt = Standard_True;
  }

  //! Gets point on the datum shape.
  const gp_Pnt& GetPoint() const 
  { 
    return myPnt; 
  }
   
  //! Sets a position of the datum text.
  void SetPointTextAttach (const gp_Pnt& thePntText)
  {
    myPntText = thePntText;
    myHasPntText = Standard_True;
  }

  //! Gets datum text position.
  const gp_Pnt& GetPointTextAttach() const 
  { 
    return myPntText; 
  }

  //! Returns True if the datum has annotation plane.
  Standard_Boolean HasPlane() const { return myHasPlane; }

  //! Returns True if point on the datum target is specified.
  Standard_Boolean HasPoint() const { return myHasPnt; }

  //! Returns True if the datum text position is specified.
  Standard_Boolean HasPointText() const 
  { 
    return myHasPntText; 
  }

  //! Set graphical presentation for object.
  void SetPresentation(const TopoDS_Shape& thePresentation, 
    const Handle(TCollection_HAsciiString)& thePresentationName)
  {
    myPresentation = thePresentation;
    myPresentationName = thePresentationName;
  }

  //! Returns graphical presentation of the object.
  TopoDS_Shape GetPresentation() const
  {
    return myPresentation;
  }

  //! Returns graphical presentation of the object.
  Handle(TCollection_HAsciiString) GetPresentationName() const
  {
    return myPresentationName;
  }

  //! Returns True if the datum has valid parameters for datum target (width, length, circle radius etc)
  Standard_Boolean HasDatumTargetParams()
  {
    return myIsValidDT;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

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
  Standard_Real myLength;
  Standard_Real myWidth;
  Standard_Integer myDatumTargetNumber;
  gp_Ax2 myAxis;
  gp_Ax2 myPlane;
  gp_Pnt myPnt;
  gp_Pnt myPntText;
  Standard_Boolean myHasPlane;
  Standard_Boolean myHasPnt;
  Standard_Boolean myHasPntText;
  TopoDS_Shape myPresentation;
  Handle(TCollection_HAsciiString) mySemanticName;
  Handle(TCollection_HAsciiString) myPresentationName;
};

#endif // _XCAFDimTolObjects_DatumObject_HeaderFile
