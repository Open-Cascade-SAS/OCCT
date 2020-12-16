
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
#include <gp_Ax2.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <XCAFDimTolObjects_DimensionModif.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Vector.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>

class XCAFDimTolObjects_DimensionObject;
DEFINE_STANDARD_HANDLE(XCAFDimTolObjects_DimensionObject, Standard_Transient)
                            
//! Access object to store dimension data
class XCAFDimTolObjects_DimensionObject : public Standard_Transient
{

public:
  
  Standard_EXPORT XCAFDimTolObjects_DimensionObject();
  
  Standard_EXPORT XCAFDimTolObjects_DimensionObject(const Handle(XCAFDimTolObjects_DimensionObject)& theObj);
  
  //! Returns semantic name
  Standard_EXPORT Handle(TCollection_HAsciiString) GetSemanticName() const;

  //! Sets semantic name
  Standard_EXPORT void SetSemanticName(const Handle(TCollection_HAsciiString)& theName);

  //! Sets dimension qualifier as min., max. or average.
  Standard_EXPORT void SetQualifier (const XCAFDimTolObjects_DimensionQualifier theQualifier);
  
  //! Returns dimension qualifier.
  Standard_EXPORT XCAFDimTolObjects_DimensionQualifier GetQualifier() const;
  
  //! Returns True if the object has dimension qualifier.
  Standard_EXPORT Standard_Boolean HasQualifier() const;
  
  //! Sets a specific type of dimension.
  Standard_EXPORT void SetType (const XCAFDimTolObjects_DimensionType theTyupe);
  
  //! Returns dimension type.
  Standard_EXPORT XCAFDimTolObjects_DimensionType GetType() const;
  
  //! Returns the main dimension value.
  //! It will be the middle value in case of range dimension.
  Standard_EXPORT Standard_Real GetValue() const;
  
  //! Returns raw array of dimension values
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetValues() const;
  
  //! Sets the main dimension value.
  //! Overwrites previous values.
  Standard_EXPORT void SetValue (const Standard_Real theValue);
  
  //! Replaces current raw array of dimension values with theValues array.
  Standard_EXPORT void SetValues (const Handle(TColStd_HArray1OfReal)& theValue);
  
  //! Returns True if the dimension is of range kind.
  //! Dimension is of range kind if its values array contains two elements
  //! defining lower and upper bounds.
  Standard_EXPORT Standard_Boolean IsDimWithRange() const;
  
  //! Sets the upper bound of the range dimension, otherwise
  //! resets it to an empty range with the specified upper bound.
  Standard_EXPORT void SetUpperBound (const Standard_Real theUpperBound);
  
  //! Sets the lower bound of the range dimension, otherwise
  //! resets it to an empty range with the specified lower bound.
  Standard_EXPORT void SetLowerBound(const Standard_Real theLowerBound);
  
  //! Returns the upper bound of the range dimension, otherwise - zero.
  Standard_EXPORT Standard_Real GetUpperBound() const;
  
  //! Returns the lower bound of the range dimension, otherwise - zero.
  Standard_EXPORT Standard_Real GetLowerBound() const;
  
  //! Returns True if the dimension is of +/- tolerance kind.
  //! Dimension is of +/- tolerance kind if its values array contains three elements
  //! defining the main value and the lower/upper tolerances.
  Standard_EXPORT Standard_Boolean IsDimWithPlusMinusTolerance() const;
  
  //! Sets the upper value of the toleranced dimension, otherwise
  //! resets a simple dimension to toleranced one with the specified lower/upper tolerances.
  //! Returns False in case of range dimension.
  Standard_EXPORT Standard_Boolean SetUpperTolValue(const Standard_Real theUperTolValue);
  
  //! Sets the lower value of the toleranced dimension, otherwise
  //! resets a simple dimension to toleranced one with the specified lower/upper tolerances.
  //! Returns False in case of range dimension.
  Standard_EXPORT Standard_Boolean SetLowerTolValue(const Standard_Real theLowerTolValue);
  
  //! Returns the lower value of the toleranced dimension, otherwise - zero.
  Standard_EXPORT Standard_Real GetUpperTolValue() const;
  
  //! Returns the upper value of the toleranced dimension, otherwise - zero.
  Standard_EXPORT Standard_Real GetLowerTolValue() const;
  
  //! Returns True if the form variance was set to not XCAFDimTolObjects_DimensionFormVariance_None value.
  Standard_EXPORT Standard_Boolean IsDimWithClassOfTolerance() const;
  
  //! Sets tolerance class of the dimension.
  //! \param theHole - True if the tolerance applies to an internal feature
  //! \param theFormVariance - represents the fundamental deviation or "position letter"
  //!                          of the ISO 286 limits-and-fits tolerance classification.
  //! \param theGrade - represents the quality or the accuracy grade of a tolerance.
  Standard_EXPORT void SetClassOfTolerance (const Standard_Boolean theHole, 
                                            const XCAFDimTolObjects_DimensionFormVariance theFormVariance, 
                                            const XCAFDimTolObjects_DimensionGrade theGrade);
  
  //! Retrieves tolerance class parameters of the dimension.
  //! Returns True if the dimension is toleranced.
  Standard_EXPORT Standard_Boolean GetClassOfTolerance (Standard_Boolean& theHole, 
                                                        XCAFDimTolObjects_DimensionFormVariance& theFormVariance, 
                                                        XCAFDimTolObjects_DimensionGrade& theGrade) const;
  
  //! Sets the number of places to the left and right of the decimal point respectively.
  Standard_EXPORT void SetNbOfDecimalPlaces (const Standard_Integer theL, const Standard_Integer theR);
  
  //! Returns the number of places to the left and right of the decimal point respectively.
  Standard_EXPORT void GetNbOfDecimalPlaces(Standard_Integer& theL, Standard_Integer& theR) const;
  
  //! Returns a sequence of modifiers of the dimension.
  Standard_EXPORT XCAFDimTolObjects_DimensionModifiersSequence GetModifiers() const;
  
  //! Sets new sequence of dimension modifiers.
  Standard_EXPORT void SetModifiers (const XCAFDimTolObjects_DimensionModifiersSequence& theModifiers);
  
  //! Adds a modifier to the dimension sequence of modifiers.
  Standard_EXPORT void AddModifier (const XCAFDimTolObjects_DimensionModif theModifier);
  
  //! Returns a 'curve' along which the dimension is measured.
  Standard_EXPORT TopoDS_Edge GetPath() const;
  
  //! Sets a 'curve' along which the dimension is measured.
  Standard_EXPORT void SetPath (const TopoDS_Edge& thePath);
  
  //! Returns the orientation of the dimension in annotation plane.
  Standard_EXPORT Standard_Boolean GetDirection (gp_Dir& theDir) const;
  
  //! Sets an orientation of the dimension in annotation plane.
  Standard_EXPORT Standard_Boolean SetDirection (const gp_Dir& theDir);
  
  //! Sets position of the dimension text.
  void SetPointTextAttach (const gp_Pnt& thePntText)
  {
    myPntText = thePntText;
    myHasPntText = Standard_True;
  }

  //! Returns position of the dimension text.
  const gp_Pnt& GetPointTextAttach() const { return myPntText; }

  //! Returns True if the position of dimension text is specified.
  Standard_Boolean HasTextPoint() const 
  { 
    return myHasPntText; 
  }

  //! Sets annotation plane.
  void SetPlane (const gp_Ax2& thePlane)
  {
    myPlane    = thePlane;
    myHasPlane = Standard_True;
  }

  //! Returns annotation plane.
  const gp_Ax2& GetPlane() const { return myPlane; }

  //! Returns True if the object has annotation plane.
  Standard_Boolean HasPlane() const { return myHasPlane; }

  //! Returns true, if connection point exists (for dimesional_size),
  //! if connection point for the first shape exists (for dimensional_location).
  Standard_Boolean HasPoint() const { return myHasPoint1; }

  // Returns true, if connection point for the second shape exists (for dimensional_location only).
  Standard_Boolean HasPoint2() const { return myHasPoint2; }

  //! Set connection point (for dimesional_size),
  //! Set connection point for the first shape (for dimensional_location).
  void SetPoint(const gp_Pnt thePnt) {
    myPnt1 = thePnt;
    myHasPoint1 = Standard_True;
  }

  // Set connection point for the second shape (for dimensional_location only).
  void SetPoint2(const gp_Pnt thePnt) {
    myPnt2 = thePnt;
    myHasPoint2 = Standard_True;
  }

  //! Get connection point (for dimesional_size),
  //! Get connection point for the first shape (for dimensional_location).
  gp_Pnt GetPoint() const {
    return myPnt1;
  }

  // Get connection point for the second shape (for dimensional_location only).
  gp_Pnt GetPoint2() const {
    return myPnt2;
  }

  //! Set graphical presentation for the object.
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

  //! Returns graphical presentation of the object
  Standard_EXPORT Handle(TCollection_HAsciiString) GetPresentationName() const
  {
    return myPresentationName;
  }

  //! Returns true, if the object has descriptions.
  Standard_Boolean HasDescriptions() const
  {
    return (myDescriptions.Length() > 0);
  }

  //! Returns number of descriptions.
  Standard_Integer NbDescriptions() const
  {
    return myDescriptions.Length();
  }

  //! Returns description with the given number.
  Handle(TCollection_HAsciiString) GetDescription(const Standard_Integer theNumber) const
  {
    if (theNumber < myDescriptions.Lower() || theNumber > myDescriptions.Upper())
      return  new TCollection_HAsciiString();
    return myDescriptions.Value(theNumber);
  }

  //! Returns name of description with the given number.
  Handle(TCollection_HAsciiString) GetDescriptionName(const Standard_Integer theNumber) const
  {
    if (theNumber < myDescriptions.Lower() || theNumber > myDescriptions.Upper())
      return new TCollection_HAsciiString();
    return myDescriptionNames.Value(theNumber);
  }

  //! Remove description with the given number.
  Standard_EXPORT void RemoveDescription(const Standard_Integer theNumber);

  //! Add new description.
  void AddDescription(const Handle(TCollection_HAsciiString) theDescription, const Handle(TCollection_HAsciiString) theName)
  {
    myDescriptions.Append(theDescription);
    myDescriptionNames.Append(theName);
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(XCAFDimTolObjects_DimensionObject,Standard_Transient)

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
  gp_Pnt myPnt1, myPnt2;
  Standard_Boolean myHasPoint1, myHasPoint2;
  gp_Ax2 myPlane;
  Standard_Boolean myHasPlane;
  Standard_Boolean myHasPntText;
  gp_Pnt myPntText;
  TopoDS_Shape myPresentation;
  Handle(TCollection_HAsciiString) mySemanticName;
  Handle(TCollection_HAsciiString) myPresentationName;
  NCollection_Vector<Handle(TCollection_HAsciiString)> myDescriptions;
  NCollection_Vector<Handle(TCollection_HAsciiString)> myDescriptionNames;

};

#endif // _XCAFDimTolObjects_DimensionObject_HeaderFile
