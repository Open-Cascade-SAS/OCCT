
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
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <XCAFDimTolObjects_DimensionQualifier.hxx>
#include <XCAFDimTolObjects_DimensionFormVariance.hxx>
#include <XCAFDimTolObjects_DimensionGrade.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <XCAFDimTolObjects_DimensionModif.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <XCAFDimTolObjects_DimensionModif.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Vector.hxx>
#include <XCAFDimTolObjects_AngularQualifier.hxx>

//! Access object to store dimension data
class XCAFDimTolObjects_DimensionObject : public Standard_Transient
{

public:
  Standard_EXPORT XCAFDimTolObjects_DimensionObject();

  Standard_EXPORT XCAFDimTolObjects_DimensionObject(
    const occ::handle<XCAFDimTolObjects_DimensionObject>& theObj);

  //! Returns semantic name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> GetSemanticName() const;

  //! Sets semantic name
  Standard_EXPORT void SetSemanticName(const occ::handle<TCollection_HAsciiString>& theName);

  //! Sets dimension qualifier as min., max. or average.
  Standard_EXPORT void SetQualifier(const XCAFDimTolObjects_DimensionQualifier theQualifier);

  //! Returns dimension qualifier.
  Standard_EXPORT XCAFDimTolObjects_DimensionQualifier GetQualifier() const;

  //! Returns True if the object has dimension qualifier.
  Standard_EXPORT bool HasQualifier() const;

  //! Sets angular qualifier as small, large or equal.
  Standard_EXPORT void SetAngularQualifier(
    const XCAFDimTolObjects_AngularQualifier theAngularQualifier);

  //! Returns angular qualifier.
  Standard_EXPORT XCAFDimTolObjects_AngularQualifier GetAngularQualifier() const;

  //! Returns True if the object has angular qualifier.
  Standard_EXPORT bool HasAngularQualifier() const;

  //! Sets a specific type of dimension.
  Standard_EXPORT void SetType(const XCAFDimTolObjects_DimensionType theTyupe);

  //! Returns dimension type.
  Standard_EXPORT XCAFDimTolObjects_DimensionType GetType() const;

  //! Returns the main dimension value.
  //! It will be the middle value in case of range dimension.
  Standard_EXPORT double GetValue() const;

  //! Returns raw array of dimension values
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> GetValues() const;

  //! Sets the main dimension value.
  //! Overwrites previous values.
  Standard_EXPORT void SetValue(const double theValue);

  //! Replaces current raw array of dimension values with theValues array.
  Standard_EXPORT void SetValues(const occ::handle<NCollection_HArray1<double>>& theValue);

  //! Returns True if the dimension is of range kind.
  //! Dimension is of range kind if its values array contains two elements
  //! defining lower and upper bounds.
  Standard_EXPORT bool IsDimWithRange() const;

  //! Sets the upper bound of the range dimension, otherwise
  //! resets it to an empty range with the specified upper bound.
  Standard_EXPORT void SetUpperBound(const double theUpperBound);

  //! Sets the lower bound of the range dimension, otherwise
  //! resets it to an empty range with the specified lower bound.
  Standard_EXPORT void SetLowerBound(const double theLowerBound);

  //! Returns the upper bound of the range dimension, otherwise - zero.
  Standard_EXPORT double GetUpperBound() const;

  //! Returns the lower bound of the range dimension, otherwise - zero.
  Standard_EXPORT double GetLowerBound() const;

  //! Returns True if the dimension is of +/- tolerance kind.
  //! Dimension is of +/- tolerance kind if its values array contains three elements
  //! defining the main value and the lower/upper tolerances.
  Standard_EXPORT bool IsDimWithPlusMinusTolerance() const;

  //! Sets the upper value of the toleranced dimension, otherwise
  //! resets a simple dimension to toleranced one with the specified lower/upper tolerances.
  //! Returns False in case of range dimension.
  Standard_EXPORT bool SetUpperTolValue(const double theUperTolValue);

  //! Sets the lower value of the toleranced dimension, otherwise
  //! resets a simple dimension to toleranced one with the specified lower/upper tolerances.
  //! Returns False in case of range dimension.
  Standard_EXPORT bool SetLowerTolValue(const double theLowerTolValue);

  //! Returns the lower value of the toleranced dimension, otherwise - zero.
  Standard_EXPORT double GetUpperTolValue() const;

  //! Returns the upper value of the toleranced dimension, otherwise - zero.
  Standard_EXPORT double GetLowerTolValue() const;

  //! Returns True if the form variance was set to not XCAFDimTolObjects_DimensionFormVariance_None
  //! value.
  Standard_EXPORT bool IsDimWithClassOfTolerance() const;

  //! Sets tolerance class of the dimension.
  //! \param theHole - True if the tolerance applies to an internal feature
  //! \param theFormVariance - represents the fundamental deviation or "position letter"
  //!                          of the ISO 286 limits-and-fits tolerance classification.
  //! \param theGrade - represents the quality or the accuracy grade of a tolerance.
  Standard_EXPORT void SetClassOfTolerance(
    const bool                        theHole,
    const XCAFDimTolObjects_DimensionFormVariance theFormVariance,
    const XCAFDimTolObjects_DimensionGrade        theGrade);

  //! Retrieves tolerance class parameters of the dimension.
  //! Returns True if the dimension is toleranced.
  Standard_EXPORT bool
    GetClassOfTolerance(bool&                        theHole,
                        XCAFDimTolObjects_DimensionFormVariance& theFormVariance,
                        XCAFDimTolObjects_DimensionGrade&        theGrade) const;

  //! Sets the number of places to the left and right of the decimal point respectively.
  Standard_EXPORT void SetNbOfDecimalPlaces(const int theL,
                                            const int theR);

  //! Returns the number of places to the left and right of the decimal point respectively.
  Standard_EXPORT void GetNbOfDecimalPlaces(int& theL, int& theR) const;

  //! Returns a sequence of modifiers of the dimension.
  Standard_EXPORT NCollection_Sequence<XCAFDimTolObjects_DimensionModif> GetModifiers() const;

  //! Sets new sequence of dimension modifiers.
  Standard_EXPORT void SetModifiers(
    const NCollection_Sequence<XCAFDimTolObjects_DimensionModif>& theModifiers);

  //! Adds a modifier to the dimension sequence of modifiers.
  Standard_EXPORT void AddModifier(const XCAFDimTolObjects_DimensionModif theModifier);

  //! Returns a 'curve' along which the dimension is measured.
  Standard_EXPORT TopoDS_Edge GetPath() const;

  //! Sets a 'curve' along which the dimension is measured.
  Standard_EXPORT void SetPath(const TopoDS_Edge& thePath);

  //! Returns the orientation of the dimension in annotation plane.
  Standard_EXPORT bool GetDirection(gp_Dir& theDir) const;

  //! Sets an orientation of the dimension in annotation plane.
  Standard_EXPORT bool SetDirection(const gp_Dir& theDir);

  //! Sets position of the dimension text.
  void SetPointTextAttach(const gp_Pnt& thePntText)
  {
    myPntText    = thePntText;
    myHasPntText = true;
  }

  //! Returns position of the dimension text.
  const gp_Pnt& GetPointTextAttach() const { return myPntText; }

  //! Returns True if the position of dimension text is specified.
  bool HasTextPoint() const { return myHasPntText; }

  //! Sets annotation plane.
  void SetPlane(const gp_Ax2& thePlane)
  {
    myPlane    = thePlane;
    myHasPlane = true;
  }

  //! Returns annotation plane.
  const gp_Ax2& GetPlane() const { return myPlane; }

  //! Returns True if the object has annotation plane.
  bool HasPlane() const { return myHasPlane; }

  //! Returns true, if connection point exists (for dimensional_size),
  //! if connection point for the first shape exists (for dimensional_location).
  bool HasPoint() const { return myHasConnection1; }

  // Returns true, if connection point for the second shape exists (for dimensional_location only).
  bool HasPoint2() const { return myHasConnection2; }

  //! Returns true, if the connection is a point not coordinate system (for dimensional_size),
  //! if connection point for the first shape exists (for dimensional_location).
  bool IsPointConnection() const { return myConnectionIsPoint1; }

  // Returns true, if the connection for the second shape is a point not coordinate system (for
  // dimensional_location only).
  bool IsPointConnection2() const { return myConnectionIsPoint2; }

  //! Set connection point (for dimensional_size),
  //! Set connection point for the first shape (for dimensional_location).
  void SetPoint(const gp_Pnt& thePnt)
  {
    myConnection1.SetLocation(thePnt);
    myHasConnection1     = true;
    myConnectionIsPoint1 = true;
  }

  // Set connection point for the second shape (for dimensional_location only).
  void SetPoint2(const gp_Pnt& thePnt)
  {
    myConnection2.SetLocation(thePnt);
    myHasConnection2     = true;
    myConnectionIsPoint2 = true;
  }

  //! Set connection point as a coordinate system (for dimensional_size),
  //! Set connection point as a coordinate system for the first shape (for dimensional_location).
  void SetConnectionAxis(const gp_Ax2& theAxis)
  {
    myConnection1        = theAxis;
    myHasConnection1     = true;
    myConnectionIsPoint1 = false;
  }

  // Set connection point as a coordinate system for the second shape (for dimensional_location
  // only).
  void SetConnectionAxis2(const gp_Ax2& theAxis)
  {
    myConnection2        = theAxis;
    myHasConnection2     = true;
    myConnectionIsPoint2 = false;
  }

  //! Get connection point (for dimensional_size),
  //! Get connection point for the first shape (for dimensional_location).
  gp_Pnt GetPoint() const { return myConnection1.Location(); }

  // Get connection point for the second shape (for dimensional_location only).
  gp_Pnt GetPoint2() const { return myConnection2.Location(); }

  //! Get connection point as a coordinate system (for dimensional_size),
  //! Get connection point as a coordinate system for the first shape (for dimensional_location).
  gp_Ax2 GetConnectionAxis() const { return myConnection1; }

  // Get connection point as a coordinate system for the second shape (for dimensional_location
  // only).
  gp_Ax2 GetConnectionAxis2() const { return myConnection2; }

  //! Returns connection name of the object.
  occ::handle<TCollection_HAsciiString> GetConnectionName() const { return myConnectionName1; }

  //! Returns 2nd connection name of the object.
  occ::handle<TCollection_HAsciiString> GetConnectionName2() const { return myConnectionName2; }

  //! Sets connection name of the object.
  void SetConnectionName(const occ::handle<TCollection_HAsciiString>& theName)
  {
    myConnectionName1 = theName;
  }

  //! Sets 2nd connection name of the object.
  void SetConnectionName2(const occ::handle<TCollection_HAsciiString>& theName)
  {
    myConnectionName2 = theName;
  }

  //! Set graphical presentation for the object.
  void SetPresentation(const TopoDS_Shape&                     thePresentation,
                       const occ::handle<TCollection_HAsciiString>& thePresentationName)
  {
    myPresentation     = thePresentation;
    myPresentationName = thePresentationName;
  }

  //! Returns graphical presentation of the object.
  TopoDS_Shape GetPresentation() const { return myPresentation; }

  //! Returns graphical presentation of the object
  Standard_EXPORT occ::handle<TCollection_HAsciiString> GetPresentationName() const
  {
    return myPresentationName;
  }

  //! Returns true, if the object has descriptions.
  bool HasDescriptions() const { return (myDescriptions.Length() > 0); }

  //! Returns number of descriptions.
  int NbDescriptions() const { return myDescriptions.Length(); }

  //! Returns description with the given number.
  occ::handle<TCollection_HAsciiString> GetDescription(const int theNumber) const
  {
    if (theNumber < myDescriptions.Lower() || theNumber > myDescriptions.Upper())
      return new TCollection_HAsciiString();
    return myDescriptions.Value(theNumber);
  }

  //! Returns name of description with the given number.
  occ::handle<TCollection_HAsciiString> GetDescriptionName(const int theNumber) const
  {
    if (theNumber < myDescriptions.Lower() || theNumber > myDescriptions.Upper())
      return new TCollection_HAsciiString();
    return myDescriptionNames.Value(theNumber);
  }

  //! Remove description with the given number.
  Standard_EXPORT void RemoveDescription(const int theNumber);

  //! Add new description.
  void AddDescription(const occ::handle<TCollection_HAsciiString> theDescription,
                      const occ::handle<TCollection_HAsciiString> theName)
  {
    myDescriptions.Append(theDescription);
    myDescriptionNames.Append(theName);
  }

  //! Returns true if the dimension type is a location.
  Standard_EXPORT static bool IsDimensionalLocation(
    const XCAFDimTolObjects_DimensionType theType);

  //! Returns true if the dimension type is a size.
  Standard_EXPORT static bool IsDimensionalSize(
    const XCAFDimTolObjects_DimensionType theType);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(XCAFDimTolObjects_DimensionObject, Standard_Transient)

private:
  XCAFDimTolObjects_DimensionType                      myType;
  occ::handle<NCollection_HArray1<double>>                        myVal;
  XCAFDimTolObjects_DimensionQualifier                 myQualifier;
  XCAFDimTolObjects_AngularQualifier                   myAngularQualifier;
  bool                                     myIsHole;
  XCAFDimTolObjects_DimensionFormVariance              myFormVariance;
  XCAFDimTolObjects_DimensionGrade                     myGrade;
  int                                     myL;
  int                                     myR;
  NCollection_Sequence<XCAFDimTolObjects_DimensionModif>         myModifiers;
  TopoDS_Edge                                          myPath;
  gp_Dir                                               myDir;
  gp_Ax2                                               myConnection1, myConnection2;
  bool                                     myHasConnection1, myHasConnection2;
  bool                                     myConnectionIsPoint1, myConnectionIsPoint2;
  occ::handle<TCollection_HAsciiString>                     myConnectionName1, myConnectionName2;
  gp_Ax2                                               myPlane;
  bool                                     myHasPlane;
  bool                                     myHasPntText;
  gp_Pnt                                               myPntText;
  TopoDS_Shape                                         myPresentation;
  occ::handle<TCollection_HAsciiString>                     mySemanticName;
  occ::handle<TCollection_HAsciiString>                     myPresentationName;
  NCollection_Vector<occ::handle<TCollection_HAsciiString>> myDescriptions;
  NCollection_Vector<occ::handle<TCollection_HAsciiString>> myDescriptionNames;
};

#endif // _XCAFDimTolObjects_DimensionObject_HeaderFile
