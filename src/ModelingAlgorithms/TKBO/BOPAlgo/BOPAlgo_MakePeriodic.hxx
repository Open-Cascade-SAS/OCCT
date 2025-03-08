// Created on: 2018-03-16
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_MakePeriodic_HeaderFile
#define _BOPAlgo_MakePeriodic_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_Options.hxx>
#include <BRepTools_History.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

//! BOPAlgo_MakePeriodic is the tool for making an arbitrary shape periodic
//! in 3D space in specified directions.
//!
//! Periodicity of the shape means that the shape can be repeated in any
//! periodic direction any number of times without creation of the new
//! geometry or splits.
//!
//! The idea is to make the shape look identical on the opposite sides of the
//! periodic directions, so when translating the copy of a shape on the period
//! there will be no coinciding parts of different dimensions.
//!
//! If necessary the algorithm will trim the shape to fit it into the
//! requested period by splitting it by the planes limiting the shape's
//! requested period.
//!
//! For making the shape periodic in certain direction the algorithm performs
//! the following steps:
//! * Creates the copy of the shape and moves it on the period into negative
//!   side of the requested direction;
//! * Splits the negative side of the shape by the moved copy, ensuring copying
//!   of the geometry from positive side to negative;
//! * Creates the copy of the shape (with already split negative side) and moves
//!   it on the period into the positive side of the requested direction;
//! * Splits the positive side of the shape by the moved copy, ensuring copying
//!   of the geometry from negative side to positive.
//!
//! The algorithm also associates the identical (or twin) shapes located
//! on the opposite sides of the result shape.
//! Using the *GetTwins()* method it is possible to get the twin shapes from
//! the opposite sides.
//!
//! Algorithm also provides the methods to repeat the periodic shape in
//! periodic directions. The subsequent repetitions are performed on the
//! repeated shape, thus repeating the shape two times in X direction will
//! create result in three shapes (original plus two copies).
//! Single subsequent repetition will result already in 6 shapes.
//! The repetitions can be cleared and started over.
//!
//! The algorithm supports History of shapes modifications, thus
//! it is possible to track how the shape has been changed to make it periodic
//! and what new shapes have been created during repetitions.
//!
//! The algorithm supports the parallel processing mode, which allows faster
//! completion of the operations.
//!
//! The algorithm supports the Error/Warning system and returns the following alerts:
//! - *BOPAlgo_AlertNoPeriodicityRequired* - Error alert is given if no periodicity
//!                                          has been requested in any direction;
//! - *BOPAlgo_AlertUnableToTrim* - Error alert is given if the trimming of the shape
//!                                 for fitting it into requested period has failed;
//! - *BOPAlgo_AlertUnableToMakeIdentical* - Error alert is given if splitting of the
//!                                          shape by its moved copies has failed;
//! - *BOPAlgo_AlertUnableToRepeat* - Warning alert is given if the gluing of the repeated
//!                                   shapes has failed.
//!
//! Example of usage of the algorithm:
//! ~~~~
//! TopoDS_Shape aShape = ...;                 // The shape to make periodic
//! Standard_Boolean bMakeXPeriodic = ...;     // Flag for making or not the shape periodic in X
//! direction Standard_Real aXPeriod = ...;              // X period for the shape Standard_Boolean
//! isXTrimmed = ...;         // Flag defining whether it is necessary to trimming
//!                                            // the shape to fit to X period
//! Standard_Real aXFirst = ...;               // Start of the X period
//!                                            // (really necessary only if the trimming is
//!                                            requested)
//! Standard_Boolean bRunParallel = ...;       // Parallel processing mode or single
//!
//! BOPAlgo_MakePeriodic aPeriodicityMaker;                   // Periodicity maker
//! aPeriodicityMaker.SetShape(aShape);                       // Set the shape
//! aPeriodicityMaker.MakeXPeriodic(bMakePeriodic, aXPeriod); // Making the shape periodic in X
//! direction aPeriodicityMaker.SetTrimmed(isXTrimmed, aXFirst);        // Trim the shape to fit X
//! period aPeriodicityMaker.SetRunParallel(bRunParallel);           // Set the parallel processing
//! mode aPeriodicityMaker.Perform();                              // Performing the operation
//!
//! if (aPeriodicityMaker.HasErrors())                        // Check for the errors
//! {
//!   // errors treatment
//!   Standard_SStream aSStream;
//!   aPeriodicityMaker.DumpErrors(aSStream);
//!   return;
//! }
//! if (aPeriodicityMaker.HasWarnings())                      // Check for the warnings
//! {
//!   // warnings treatment
//!   Standard_SStream aSStream;
//!   aPeriodicityMaker.DumpWarnings(aSStream);
//! }
//! const TopoDS_Shape& aPeriodicShape = aPeriodicityMaker.Shape(); // Result periodic shape
//!
//!
//! aPeriodicityMaker.XRepeat(2);                                    // Making repetitions
//! const TopoDS_Shape& aRepeat = aPeriodicityMaker.RepeatedShape(); // Getting the repeated shape
//! aPeriodicityMaker.ClearRepetitions();                            // Clearing the repetitions
//! ~~~~
//!
class BOPAlgo_MakePeriodic : public BOPAlgo_Options
{
public:
  DEFINE_STANDARD_ALLOC

public: //! @name Constructor
  //! Empty constructor
  BOPAlgo_MakePeriodic()
      : BOPAlgo_Options()
  {
    myRepeatPeriod[0] = myRepeatPeriod[1] = myRepeatPeriod[2] = 0.0;
  }

public: //! @name Setting the shape to make it periodic
  //! Sets the shape to make it periodic.
  //! @param[in] theShape  The shape to make periodic.
  void SetShape(const TopoDS_Shape& theShape) { myInputShape = theShape; }

public: //! @name Definition of the structure to keep all periodicity parameters
  //! Structure to keep all periodicity parameters:
  struct PeriodicityParams
  {
    PeriodicityParams() { Clear(); }

    //! Returns all previously set parameters to default values
    void Clear()
    {
      myPeriodic[0] = myPeriodic[1] = myPeriodic[2] = Standard_False;
      myPeriod[0] = myPeriod[1] = myPeriod[2] = 0.0;
      myIsTrimmed[0] = myIsTrimmed[1] = myIsTrimmed[2] = Standard_True;
      myPeriodFirst[0] = myPeriodFirst[1] = myPeriodFirst[2] = 0.0;
    }

    Standard_Boolean myPeriodic[3]; //!< Array of flags defining whether the shape should be
                                    //! periodic in XYZ directions
    Standard_Real myPeriod[3];      //!< Array of XYZ period values. Defining the period for any
                                    //! direction the corresponding flag for that direction in
                                    //! myPeriodic should be set to true
                                    // clang-format off
    Standard_Boolean myIsTrimmed[3]; //!< Array of flags defining whether the input shape has to be
                                     //! trimmed to fit the required period in the required direction
    Standard_Real myPeriodFirst[3];  //!< Array of start parameters of the XYZ periods: required for trimming
                                    // clang-format on
  };

public: //! @name Setters/Getters for periodicity parameters structure
  //! Sets the periodicity parameters.
  //! @param[in] theParams  Periodicity parameters
  void SetPeriodicityParameters(const PeriodicityParams& theParams)
  {
    myPeriodicityParams = theParams;
  }

  const PeriodicityParams& PeriodicityParameters() const { return myPeriodicityParams; }

public: //! @name Methods for setting/getting periodicity info using ID as a direction
  //! Sets the flag to make the shape periodic in specified direction:
  //! - 0 - X direction;
  //! - 1 - Y direction;
  //! - 2 - Z direction.
  //!
  //! @param[in] theDirectionID  The direction's ID;
  //! @param[in] theIsPeriodic  Flag defining periodicity in given direction;
  //! @param[in] thePeriod  Required period in given direction.
  void MakePeriodic(const Standard_Integer theDirectionID,
                    const Standard_Boolean theIsPeriodic,
                    const Standard_Real    thePeriod = 0.0)
  {
    Standard_Integer id                = ToDirectionID(theDirectionID);
    myPeriodicityParams.myPeriodic[id] = theIsPeriodic;
    myPeriodicityParams.myPeriod[id]   = theIsPeriodic ? thePeriod : 0.0;
  }

  //! Returns the info about Periodicity of the shape in specified direction.
  //! @param[in] theDirectionID  The direction's ID.
  Standard_Boolean IsPeriodic(const Standard_Integer theDirectionID) const
  {
    return myPeriodicityParams.myPeriodic[ToDirectionID(theDirectionID)];
  }

  //! Returns the Period of the shape in specified direction.
  //! @param[in] theDirectionID  The direction's ID.
  Standard_Real Period(const Standard_Integer theDirectionID) const
  {
    Standard_Integer id = ToDirectionID(theDirectionID);
    return myPeriodicityParams.myPeriodic[id] ? myPeriodicityParams.myPeriod[id] : 0.0;
  }

public: //! @name Named methods for setting/getting info about shape's periodicity
  //! Sets the flag to make the shape periodic in X direction.
  //! @param[in] theIsPeriodic  Flag defining periodicity in X direction;
  //! @param[in] thePeriod  Required period in X direction.
  void MakeXPeriodic(const Standard_Boolean theIsPeriodic, const Standard_Real thePeriod = 0.0)
  {
    MakePeriodic(0, theIsPeriodic, thePeriod);
  }

  //! Returns the info about periodicity of the shape in X direction.
  Standard_Boolean IsXPeriodic() const { return IsPeriodic(0); }

  //! Returns the XPeriod of the shape
  Standard_Real XPeriod() const { return Period(0); }

  //! Sets the flag to make the shape periodic in Y direction.
  //! @param[in] theIsPeriodic  Flag defining periodicity in Y direction;
  //! @param[in] thePeriod  Required period in Y direction.
  void MakeYPeriodic(const Standard_Boolean theIsPeriodic, const Standard_Real thePeriod = 0.0)
  {
    MakePeriodic(1, theIsPeriodic, thePeriod);
  }

  //! Returns the info about periodicity of the shape in Y direction.
  Standard_Boolean IsYPeriodic() const { return IsPeriodic(1); }

  //! Returns the YPeriod of the shape.
  Standard_Real YPeriod() const { return Period(1); }

  //! Sets the flag to make the shape periodic in Z direction.
  //! @param[in] theIsPeriodic  Flag defining periodicity in Z direction;
  //! @param[in] thePeriod  Required period in Z direction.
  void MakeZPeriodic(const Standard_Boolean theIsPeriodic, const Standard_Real thePeriod = 0.0)
  {
    MakePeriodic(2, theIsPeriodic, thePeriod);
  }

  //! Returns the info about periodicity of the shape in Z direction.
  Standard_Boolean IsZPeriodic() const { return IsPeriodic(2); }

  //! Returns the ZPeriod of the shape.
  Standard_Real ZPeriod() const { return Period(2); }

public: //! @name Methods for setting/getting trimming info taking Direction ID as a parameter
  //! Defines whether the input shape is already trimmed in specified direction
  //! to fit the period in this direction.
  //! Direction is defined by an ID:
  //! - 0 - X direction;
  //! - 1 - Y direction;
  //! - 2 - Z direction.
  //!
  //! If the shape is not trimmed it is required to set the first parameter
  //! of the period in that direction.
  //! The algorithm will make the shape fit into the period.
  //!
  //! Before calling this method, the shape has to be set to be periodic in this direction.
  //!
  //! @param[in] theDirectionID  The direction's ID;
  //! @param[in] theIsTrimmed  The flag defining trimming of the shape in given direction;
  //! @param[in] theFirst  The first periodic parameter in the given direction.
  void SetTrimmed(const Standard_Integer theDirectionID,
                  const Standard_Boolean theIsTrimmed,
                  const Standard_Real    theFirst = 0.0)
  {
    Standard_Integer id = ToDirectionID(theDirectionID);
    if (IsPeriodic(id))
    {
      myPeriodicityParams.myIsTrimmed[id]   = theIsTrimmed;
      myPeriodicityParams.myPeriodFirst[id] = !theIsTrimmed ? theFirst : 0.0;
    }
  }

  //! Returns whether the input shape was trimmed in the specified direction.
  //! @param[in] theDirectionID  The direction's ID.
  Standard_Boolean IsInputTrimmed(const Standard_Integer theDirectionID) const
  {
    return myPeriodicityParams.myIsTrimmed[ToDirectionID(theDirectionID)];
  }

  //! Returns the first periodic parameter in the specified direction.
  //! @param[in] theDirectionID  The direction's ID.
  Standard_Real PeriodFirst(const Standard_Integer theDirectionID) const
  {
    Standard_Integer id = ToDirectionID(theDirectionID);
    return !myPeriodicityParams.myIsTrimmed[id] ? myPeriodicityParams.myPeriodFirst[id] : 0.0;
  }

public: //! @name Named methods for setting/getting trimming info
  //! Defines whether the input shape is already trimmed in X direction
  //! to fit the X period. If the shape is not trimmed it is required
  //! to set the first parameter for the X period.
  //! The algorithm will make the shape fit into the period.
  //!
  //! Before calling this method, the shape has to be set to be periodic in this direction.
  //!
  //! @param[in] theIsTrimmed  Flag defining whether the shape is already trimmed
  //!                          in X direction to fit the X period;
  //! @param[in] theFirst  The first X periodic parameter.
  void SetXTrimmed(const Standard_Boolean theIsTrimmed, const Standard_Boolean theFirst = 0.0)
  {
    SetTrimmed(0, theIsTrimmed, theFirst);
  }

  //! Returns whether the input shape was already trimmed for X period.
  Standard_Boolean IsInputXTrimmed() const { return IsInputTrimmed(0); }

  //! Returns the first parameter for the X period.
  Standard_Real XPeriodFirst() const { return PeriodFirst(0); }

  //! Defines whether the input shape is already trimmed in Y direction
  //! to fit the Y period. If the shape is not trimmed it is required
  //! to set the first parameter for the Y period.
  //! The algorithm will make the shape fit into the period.
  //!
  //! Before calling this method, the shape has to be set to be periodic in this direction.
  //!
  //! @param[in] theIsTrimmed  Flag defining whether the shape is already trimmed
  //!                          in Y direction to fit the Y period;
  //! @param[in] theFirst  The first Y periodic parameter.
  void SetYTrimmed(const Standard_Boolean theIsTrimmed, const Standard_Boolean theFirst = 0.0)
  {
    SetTrimmed(1, theIsTrimmed, theFirst);
  }

  //! Returns whether the input shape was already trimmed for Y period.
  Standard_Boolean IsInputYTrimmed() const { return IsInputTrimmed(1); }

  //! Returns the first parameter for the Y period.
  Standard_Real YPeriodFirst() const { return PeriodFirst(1); }

  //! Defines whether the input shape is already trimmed in Z direction
  //! to fit the Z period. If the shape is not trimmed it is required
  //! to set the first parameter for the Z period.
  //! The algorithm will make the shape fit into the period.
  //!
  //! Before calling this method, the shape has to be set to be periodic in this direction.
  //!
  //! @param[in] theIsTrimmed  Flag defining whether the shape is already trimmed
  //!                          in Z direction to fit the Z period;
  //! @param[in] theFirst  The first Z periodic parameter.
  void SetZTrimmed(const Standard_Boolean theIsTrimmed, const Standard_Boolean theFirst = 0.0)
  {
    SetTrimmed(2, theIsTrimmed, theFirst);
  }

  //! Returns whether the input shape was already trimmed for Z period.
  Standard_Boolean IsInputZTrimmed() const { return IsInputTrimmed(2); }

  //! Returns the first parameter for the Z period.
  Standard_Real ZPeriodFirst() const { return PeriodFirst(2); }

public: //! @name Performing  the operation
  //! Makes the shape periodic in necessary directions
  Standard_EXPORT void Perform();

public: //! @name Using the algorithm to repeat the shape
  //! Performs repetition of the shape in specified direction
  //! required number of times.
  //! Negative value of times means that the repetition should
  //! be perform in negative direction.
  //! Makes the repeated shape a base for following repetitions.
  //!
  //! @param[in] theDirectionID  The direction's ID;
  //! @param[in] theTimes  Requested number of repetitions.
  Standard_EXPORT const TopoDS_Shape& RepeatShape(const Standard_Integer theDirectionID,
                                                  const Standard_Integer theTimes);

  //! Repeats the shape in X direction specified number of times.
  //! Negative value of times means that the repetition should be
  //! perform in negative X direction.
  //! Makes the repeated shape a base for following repetitions.
  //!
  //! @param[in] theTimes  Requested number of repetitions.
  const TopoDS_Shape& XRepeat(const Standard_Integer theTimes) { return RepeatShape(0, theTimes); }

  //! Repeats the shape in Y direction specified number of times.
  //! Negative value of times means that the repetition should be
  //! perform in negative Y direction.
  //! Makes the repeated shape a base for following repetitions.
  //!
  //! @param[in] theTimes  Requested number of repetitions.
  const TopoDS_Shape& YRepeat(const Standard_Integer theTimes) { return RepeatShape(1, theTimes); }

  //! Repeats the shape in Z direction specified number of times.
  //! Negative value of times means that the repetition should be
  //! perform in negative Z direction.
  //! Makes the repeated shape a base for following repetitions.
  //!
  //! @param[in] theTimes  Requested number of repetitions.
  const TopoDS_Shape& ZRepeat(const Standard_Integer theTimes) { return RepeatShape(2, theTimes); }

public: //! @name Starting the repetitions over
  //! Returns the repeated shape
  const TopoDS_Shape& RepeatedShape() const { return myRepeatedShape; }

  //! Clears all performed repetitions.
  //! The next repetition will be performed on the base shape.
  void ClearRepetitions()
  {
    myRepeatPeriod[0] = myRepeatPeriod[1] = myRepeatPeriod[2] = 0.0;
    myRepeatedShape.Nullify();
    myRepeatedTwins.Clear();
    if (!myHistory.IsNull())
    {
      myHistory->Clear();
      if (!mySplitHistory.IsNull())
        myHistory->Merge(mySplitHistory);
    }
  }

public: //! @name Obtaining the result shape
  //! Returns the resulting periodic shape
  const TopoDS_Shape& Shape() const { return myShape; }

public: //! @name Getting the identical shapes
  //! Returns the identical shapes for the given shape located
  //! on the opposite periodic side.
  //! Returns empty list in case the shape has no twin.
  //!
  //! @param[in] theS  Shape to get the twins for.
  const TopTools_ListOfShape& GetTwins(const TopoDS_Shape& theS) const
  {
    static TopTools_ListOfShape empty;
    const TopTools_ListOfShape* aTwins =
      myRepeatedTwins.IsEmpty() ? myTwins.Seek(theS) : myRepeatedTwins.Seek(theS);
    return (aTwins ? *aTwins : empty);
  }

public: //! @name Getting the History of the algorithm
  //! Returns the History of the algorithm
  const Handle(BRepTools_History)& History() const { return myHistory; }

public: //! @name Clearing the algorithm from previous runs
  //! Clears the algorithm from previous runs
  void Clear()
  {
    BOPAlgo_Options::Clear();
    myPeriodicityParams.Clear();
    myShape.Nullify();
    if (!mySplitHistory.IsNull())
      mySplitHistory->Clear();
    if (!myHistory.IsNull())
      myHistory->Clear();

    ClearRepetitions();
  }

public: //! @name Conversion of the integer to ID of periodic direction
  //! Converts the integer to ID of periodic direction
  static Standard_Integer ToDirectionID(const Standard_Integer theDirectionID)
  {
    return Abs(theDirectionID % 3);
  }

protected: //! @name Protected methods performing the operation
  //! Checks the validity of input data
  Standard_EXPORT void CheckData();

  //! Trims the shape to fit to the periodic bounds
  Standard_EXPORT void Trim();

  //! Makes the shape identical on opposite sides
  Standard_EXPORT void MakeIdentical();

  //! Splits the negative side of the shape with the geometry
  //! located on the positive side copying the geometry from
  //! positive side to the negative.
  Standard_EXPORT void SplitNegative();

  //! Splits the positive side of the shape with the geometry
  //! located on the negative side of the shape.
  //! Ensures that the geometries on the opposite sides will
  //! be identical.
  //! Associates the identical opposite sub-shapes.
  Standard_EXPORT void SplitPositive();

  //! Splits the shape by the given tools, copying the geometry of coinciding
  //! parts from the given tools to the split shape.
  //! @param[in] theTools  The tools to split the shape and take the geometry
  //!                      for coinciding parts.
  //! @param[out] theSplitShapeHistory  The history of shape split
  //! @param[out] theSplitToolsHistory  The history of tools modifications during the split
  Standard_EXPORT void SplitShape(const TopTools_ListOfShape& theTools,
                                  Handle(BRepTools_History)   theSplitShapeHistory = NULL,
                                  Handle(BRepTools_History)   theSplitToolsHistory = NULL);

  //! Updates the map of twins after periodic shape repetition.
  //! @param[in] theTranslationHistory  The history of translation of the periodic shape.
  //! @param[in] theGluingHistory  The history of gluing of the repeated shapes.
  Standard_EXPORT void UpdateTwins(const BRepTools_History& theTranslationHistory,
                                   const BRepTools_History& theGluingHistory);

protected: //! @name Fields
  // Inputs
  TopoDS_Shape myInputShape; //!< Input shape to make periodic

  PeriodicityParams myPeriodicityParams; //!< Periodicity parameters

  // Results
  TopoDS_Shape  myShape;           //!< Resulting periodic shape (base for repetitions)
  TopoDS_Shape  myRepeatedShape;   //!< Resulting shape after making repetitions of the base
  Standard_Real myRepeatPeriod[3]; //!< XYZ repeat period
                                   // clang-format off
  TopTools_DataMapOfShapeListOfShape myRepeatedTwins; //!< Map of associations of the identical sub-shapes
                                                      //! after repetition of the periodic shape
                                   // clang-format on

  // Twins
  TopTools_DataMapOfShapeListOfShape myTwins; //!< Map of associations of the identical sub-shapes
                                              //! located on the opposite sides of the shape

  // History
  Handle(BRepTools_History) mySplitHistory; //!< Split history - history of shapes modification
                                            //! after the split for making the shape periodic
  Handle(BRepTools_History) myHistory;      //!< Final history of shapes modifications
                                            //! (to include the history of shape repetition)
};

#endif // _BOPAlgo_MakePeriodic_HeaderFile
