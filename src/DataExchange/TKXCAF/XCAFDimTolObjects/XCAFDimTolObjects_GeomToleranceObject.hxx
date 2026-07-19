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

#include <XCAFDimTolObjects_GeomToleranceType.hxx>
#include <XCAFDimTolObjects_GeomToleranceTypeValue.hxx>
#include <Standard_Real.hxx>
#include <XCAFDimTolObjects_GeomToleranceMatReqModif.hxx>
#include <XCAFDimTolObjects_GeomToleranceZoneModif.hxx>
#include <NCollection_Sequence.hxx>
#include <XCAFDimTolObjects_GeomToleranceModif.hxx>
#include <Standard_Transient.hxx>
#include <XCAFDimTolObjects_ToleranceZoneAffectedPlane.hxx>
#include <gp_Pln.hxx>
#include <TopoDS_Shape.hxx>
#include <TCollection_HAsciiString.hxx>

//! Access object to store dimension and tolerance
class XCAFDimTolObjects_GeomToleranceObject : public Standard_Transient
{

public:
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceObject();

  Standard_EXPORT XCAFDimTolObjects_GeomToleranceObject(
    const occ::handle<XCAFDimTolObjects_GeomToleranceObject>& theObj);

  //! Returns semantic name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> GetSemanticName() const;

  //! Sets semantic name
  Standard_EXPORT void SetSemanticName(const occ::handle<TCollection_HAsciiString>& theName);

  //! Sets type of the object.
  Standard_EXPORT void SetType(const XCAFDimTolObjects_GeomToleranceType theType);

  //! Returns type of the object.
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceType GetType() const;

  //! Sets type of tolerance value.
  Standard_EXPORT void SetTypeOfValue(
    const XCAFDimTolObjects_GeomToleranceTypeValue theTypeOfValue);

  //! Returns type of tolerance value.
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceTypeValue GetTypeOfValue() const;

  //! Sets tolerance value.
  Standard_EXPORT void SetValue(const double theValue);

  //! Returns tolerance value.
  Standard_EXPORT double GetValue() const;

  //! Sets material requirement of the tolerance.
  Standard_EXPORT void SetMaterialRequirementModifier(
    const XCAFDimTolObjects_GeomToleranceMatReqModif theMatReqModif);

  //! Returns material requirement of the tolerance.
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceMatReqModif GetMaterialRequirementModifier() const;

  //! Sets tolerance zone.
  Standard_EXPORT void SetZoneModifier(const XCAFDimTolObjects_GeomToleranceZoneModif theZoneModif);

  //! Returns tolerance zone.
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceZoneModif GetZoneModifier() const;

  //! Sets value associated with tolerance zone.
  Standard_EXPORT void SetValueOfZoneModifier(const double theValue);

  //! Returns value associated with tolerance zone.
  Standard_EXPORT double GetValueOfZoneModifier() const;

  //! Sets new sequence of tolerance modifiers.
  Standard_EXPORT void SetModifiers(
    const NCollection_Sequence<XCAFDimTolObjects_GeomToleranceModif>& theModifiers);

  //! Adds a tolerance modifier to the sequence of modifiers.
  Standard_EXPORT void AddModifier(const XCAFDimTolObjects_GeomToleranceModif theModifier);

  //! Returns a sequence of modifiers of the tolerance.
  Standard_EXPORT NCollection_Sequence<XCAFDimTolObjects_GeomToleranceModif> GetModifiers() const;

  //! Sets the maximal upper tolerance value for tolerance with modifiers.
  Standard_EXPORT void SetMaxValueModifier(const double theModifier);

  //! Returns the maximal upper tolerance.
  Standard_EXPORT double GetMaxValueModifier() const;

  Standard_EXPORT void SetAxis(const gp_Ax2& theAxis);

  Standard_EXPORT gp_Ax2 GetAxis() const;

  Standard_EXPORT bool HasAxis() const;

  //! Sets annotation plane.
  void SetPlane(const gp_Ax2& thePlane)
  {
    myPlane    = thePlane;
    myHasPlane = true;
  }

  //! Returns annotation plane.
  const gp_Ax2& GetPlane() const { return myPlane; }

  //! Sets reference point.
  void SetPoint(const gp_Pnt& thePnt)
  {
    myPnt    = thePnt;
    myHasPnt = true;
  }

  //! Returns reference point.
  const gp_Pnt& GetPoint() const { return myPnt; }

  //! Sets text position.
  void SetPointTextAttach(const gp_Pnt& thePntText)
  {
    myPntText    = thePntText;
    myHasPntText = true;
  }

  //! Returns the text position.
  const gp_Pnt& GetPointTextAttach() const { return myPntText; }

  //! Returns True if the object has annotation plane.
  bool HasPlane() const { return myHasPlane; }

  //! Returns True if reference point is specified.
  bool HasPoint() const { return myHasPnt; }

  //! Returns True if text position is specified.
  bool HasPointText() const { return myHasPntText; }

  //! Set graphical presentation for object.
  void SetPresentation(const TopoDS_Shape&                          thePresentation,
                       const occ::handle<TCollection_HAsciiString>& thePresentationName)
  {
    myPresentation     = thePresentation;
    myPresentationName = thePresentationName;
  }

  //! Returns graphical presentation of the object.
  TopoDS_Shape GetPresentation() const { return myPresentation; }

  //! Returns graphical presentation of the object.
  occ::handle<TCollection_HAsciiString> GetPresentationName() const { return myPresentationName; }

  // Returns true, if affected plane is specified.
  bool HasAffectedPlane() const
  {
    return (myAffectedPlaneType != XCAFDimTolObjects_ToleranceZoneAffectedPlane_None);
  }

  // Returns type of affected plane.
  XCAFDimTolObjects_ToleranceZoneAffectedPlane GetAffectedPlaneType() const
  {
    return myAffectedPlaneType;
  }

  // Sets affected plane type.
  void SetAffectedPlaneType(const XCAFDimTolObjects_ToleranceZoneAffectedPlane theType)
  {
    myAffectedPlaneType = theType;
  }

  //! Sets affected plane.
  void SetAffectedPlane(const gp_Pln& thePlane) { myAffectedPlane = thePlane; }

  //! Sets affected plane.
  void SetAffectedPlane(const gp_Pln&                                      thePlane,
                        const XCAFDimTolObjects_ToleranceZoneAffectedPlane theType)
  {
    myAffectedPlaneType = theType;
    myAffectedPlane     = thePlane;
  }

  //! Returns affected plane.
  const gp_Pln& GetAffectedPlane() const { return myAffectedPlane; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(XCAFDimTolObjects_GeomToleranceObject, Standard_Transient)

private:
  XCAFDimTolObjects_GeomToleranceType                        myType;
  XCAFDimTolObjects_GeomToleranceTypeValue                   myTypeOfValue;
  double                                                     myValue;
  XCAFDimTolObjects_GeomToleranceMatReqModif                 myMatReqModif;
  XCAFDimTolObjects_GeomToleranceZoneModif                   myZoneModif;
  double                                                     myValueOfZoneModif;
  NCollection_Sequence<XCAFDimTolObjects_GeomToleranceModif> myModifiers;
  double                                                     myMaxValueModif;
  gp_Ax2                                                     myAxis;
  bool                                                       myHasAxis;
  gp_Ax2                                                     myPlane;
  gp_Pnt                                                     myPnt;
  gp_Pnt                                                     myPntText;
  bool                                                       myHasPlane;
  bool                                                       myHasPnt;
  bool                                                       myHasPntText;
  TopoDS_Shape                                               myPresentation;
  occ::handle<TCollection_HAsciiString>                      mySemanticName;
  occ::handle<TCollection_HAsciiString>                      myPresentationName;
  XCAFDimTolObjects_ToleranceZoneAffectedPlane               myAffectedPlaneType;
  gp_Pln                                                     myAffectedPlane;
};

#endif // _XCAFDimTolObjects_GeomToleranceObject_HeaderFile
