// Created on: 2018-03-29
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

#ifndef _BOPAlgo_MakeConnected_HeaderFile
#define _BOPAlgo_MakeConnected_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_Options.hxx>
#include <BOPAlgo_MakePeriodic.hxx>

#include <BRepTools_History.hxx>

#include <NCollection_DataMap.hxx>

#include <TopTools_OrientedShapeMapHasher.hxx>

//! BOPAlgo_MakeConnected is the algorithm for making the touching
//! shapes connected or glued, i.e. for making the coinciding geometries
//! be topologically shared among the shapes.
//!
//! The input shapes should be of the same dimension, otherwise
//! the gluing will not make any sense.
//!
//! After the shapes are made connected, the border elements of input shapes
//! are associated with the shapes to which they belong. At that, the orientation of
//! the border element in the shape is taken into account.
//! The associations are made for the following types:
//! - For input SOLIDS, the resulting FACES are associated with the input solids;
//! - For input FACES, the resulting EDGES are associated with the input faces;
//! - For input EDGES, the resulting VERTICES are associated with the input edges.
//!
//! In frames of this algorithm the input shapes are called materials,
//! and the association process is called the material association.
//! The material association allows finding the coinciding elements for the opposite
//! input shapes. These elements will be associated to at least two materials.
//!
//! After making the shapes connected, it is possible to make the connected
//! shape periodic using the *BOPAlgo_MakePeriodic* tool.
//! After making the shape periodic, the material associations will be updated
//! to correspond to the actual state of the result shape.
//! Repetition of the periodic shape is also possible here. Material associations
//! are not going to be lost.
//!
//! The algorithm supports history of shapes modification, thus it is possible
//! to track the modification of the input shapes during the operations.
//! Additionally to standard history methods, the algorithm provides the
//! the method *GetOrigins()* which allows obtaining the input shapes from which
//! the resulting shape has been created.
//!
//! The algorithm supports the parallel processing mode, which allows faster
//! completion of the operations.
//!
//! The algorithm returns the following Error/Warning messages:
//! - *BOPAlgo_AlertTooFewArguments* - error alert is given on the attempt to run
//!     the algorithm without the arguments;
//! - *BOPAlgo_AlertMultiDimensionalArguments* - error alert is given on the attempt
//!     to run the algorithm on multi-dimensional arguments;
//! - *BOPAlgo_AlertUnableToGlue* - error alert is given if the gluer algorithm
//!     is unable to glue the given arguments;
//! - *BOPAlgo_AlertUnableToMakePeriodic* - warning alert is given if the periodicity
//!     maker is unable to make the connected shape periodic with given options;
//! - *BOPAlgo_AlertShapeIsNotPeriodic* - warning alert is given on the attempt to
//!     repeat the shape before making it periodic.
//!
//! Here is the example of usage of the algorithm:
//! ~~~~
//! TopTools_ListOfShape anArguments = ...;  // Shapes to make connected
//! Standard_Boolean bRunParallel = ...;     // Parallel processing mode
//!
//! BOPAlgo_MakeConnected aMC;               // Tool for making the shapes connected
//! aMC.SetArguments(anArguments);           // Set the shapes
//! aMC.SetRunParallel(bRunParallel);        // Set parallel processing mode
//! aMC.Perform();                           // Perform the operation
//!
//! if (aMC.HasErrors())                     // Check for the errors
//! {
//!   // errors treatment
//!   Standard_SStream aSStream;
//!   aMC.DumpErrors(aSStream);
//!   return;
//! }
//! if (aMC.HasWarnings())                   // Check for the warnings
//! {
//!   // warnings treatment
//!   Standard_SStream aSStream;
//!   aMC.DumpWarnings(aSStream);
//! }
//!
//! const TopoDS_Shape& aGluedShape = aMC.Shape(); // Connected shape
//!
//! // Checking material associations
//! TopAbs_ShapeEnum anElemType = ...;       // Type of border element
//! TopExp_Explorer anExp(anArguments.First(), anElemType);
//! for (; anExp.More(); anExp.Next())
//! {
//!   const TopoDS_Shape& anElement = anExp.Current();
//!   const TopTools_ListOfShape& aNegativeM = aMC.MaterialsOnNegativeSide(anElement);
//!   const TopTools_ListOfShape& aPositiveM = aMC.MaterialsOnPositiveSide(anElement);
//! }
//!
//! // Making the connected shape periodic
//! BOPAlgo_MakePeriodic::PeriodicityParams aParams = ...; // Options for periodicity of the connected shape
//! aMC.MakePeriodic(aParams);
//!
//! // Shape repetition after making it periodic
//! // Check if the shape has been made periodic successfully
//! if (aMC.PeriodicityTool().HasErrors())
//! {
//!   // Periodicity maker error treatment
//! }
//!
//! // Shape repetition in periodic directions
//! aMC.RepeatShape(0, 2);
//!
//! const TopoDS_Shape& aShape = aMC.PeriodicShape(); // Periodic and repeated shape
//! ~~~~
//!
class BOPAlgo_MakeConnected : public BOPAlgo_Options
{
public:

  DEFINE_STANDARD_ALLOC

public: //! @name Constructor

  //! Empty constructor
  BOPAlgo_MakeConnected() : BOPAlgo_Options()
  {
  }


public: //! @name Setters for the shapes to make connected

  //! Sets the shape for making them connected.
  //! @param theArgs [in] The arguments for the operation.
  void SetArguments(const TopTools_ListOfShape& theArgs)
  {
    myArguments = theArgs;
  }

  //! Adds the shape to the arguments.
  //! @param theS [in] One of the argument shapes.
  void AddArgument(const TopoDS_Shape& theS)
  {
    myArguments.Append(theS);
  }

  //! Returns the list of arguments of the operation.
  const TopTools_ListOfShape& Arguments() const
  {
    return myArguments;
  }

public: //! @name Performing the operations

  //! Performs the operation, i.e. makes the input shapes connected.
  Standard_EXPORT void Perform();


public: //! @name Shape periodicity & repetition

  //! Makes the connected shape periodic.
  //! Repeated calls of this method overwrite the previous calls
  //! working with the basis connected shape.
  //! @param theParams [in] Periodic options.
  Standard_EXPORT void MakePeriodic(const BOPAlgo_MakePeriodic::PeriodicityParams& theParams);

  //! Performs repetition of the periodic shape in specified direction
  //! required number of times.
  //! @param theDirectionID [in] The direction's ID (0 for X, 1 for Y, 2 for Z);
  //! @param theTimes [in] Requested number of repetitions (sign of the value defines
  //!                      the side of the repetition direction (positive or negative)).
  Standard_EXPORT void RepeatShape(const Standard_Integer theDirectionID,
                                   const Standard_Integer theTimes);

  //! Clears the repetitions performed on the periodic shape,
  //! keeping the shape periodic.
  Standard_EXPORT void ClearRepetitions();

  //! Returns the periodicity tool.
  const BOPAlgo_MakePeriodic& PeriodicityTool() const
  {
    return myPeriodicityMaker;
  }


public: //! @name Material transitions

  //! Returns the original shapes which images contain the
  //! the given shape with FORWARD orientation.
  //! @param theS [in] The shape for which the materials are necessary.
  const TopTools_ListOfShape& MaterialsOnPositiveSide(const TopoDS_Shape& theS)
  {
    const TopTools_ListOfShape* pLM = myMaterials.Seek(theS.Oriented(TopAbs_FORWARD));
    return (pLM ? *pLM : EmptyList());
  }

  //! Returns the original shapes which images contain the
  //! the given shape with REVERSED orientation.
  //! @param theS [in] The shape for which the materials are necessary.
  const TopTools_ListOfShape& MaterialsOnNegativeSide(const TopoDS_Shape& theS)
  {
    const TopTools_ListOfShape* pLM = myMaterials.Seek(theS.Oriented(TopAbs_REVERSED));
    return (pLM ? *pLM : EmptyList());
  }


public: //! @name History methods

  //! Returns the history of operations
  const Handle(BRepTools_History)& History() const
  {
    return myHistory;
  }

  //! Returns the list of shapes modified from the given shape.
  //! @param theS [in] The shape for which the modified shapes are necessary.
  const TopTools_ListOfShape& GetModified(const TopoDS_Shape& theS)
  {
    return (myHistory.IsNull() ? EmptyList() : myHistory->Modified(theS));
  }

  //! Returns the list of original shapes from which the current shape has been created.
  //! @param theS [in] The shape for which the origins are necessary.
  const TopTools_ListOfShape& GetOrigins(const TopoDS_Shape& theS)
  {
    const TopTools_ListOfShape* pLOr = myOrigins.Seek(theS);
    return (pLOr ? *pLOr : EmptyList());
  }


public: //! @name Getting the result shapes

  //! Returns the resulting connected shape
  const TopoDS_Shape& Shape() const
  {
    return myGlued;
  }

  //! Returns the resulting periodic & repeated shape
  const TopoDS_Shape& PeriodicShape() const
  {
    return myShape;
  }


public: //! @name Clearing the contents of the algorithm from previous runs

  //! Clears the contents of the algorithm.
  void Clear()
  {
    BOPAlgo_Options::Clear();
    myArguments.Clear();
    myAllInputsMap.Clear();
    myPeriodicityMaker.Clear();
    myOrigins.Clear();
    myMaterials.Clear();
    if (!myGlueHistory.IsNull())
      myGlueHistory->Clear();
    if (!myHistory.IsNull())
      myHistory->Clear();
    myGlued.Nullify();
    myShape.Nullify();
  }


protected: //! @name Protected methods performing the operation

  //! Checks the validity of input data.
  Standard_EXPORT void CheckData();

  //! Makes the argument shapes connected (or glued).
  Standard_EXPORT void MakeConnected();

  //! Associates the materials transitions for the border elements:
  //! - For input Solids, associates the Faces to Solids;
  //! - For input Faces, associates the Edges to Faces;
  //! - For input Edges, associates the Vertices to Edges.
  Standard_EXPORT void AssociateMaterials();

  //! Fills the map of origins
  Standard_EXPORT void FillOrigins();

  //! Updates the history, material associations, origins map
  //! after periodicity operations.
  Standard_EXPORT void Update();

private:

  //! Returns an empty list.
  const TopTools_ListOfShape& EmptyList()
  {
    static const TopTools_ListOfShape anEmptyList;
    return anEmptyList;
  }

protected: //! @name Fields

  // Inputs
  TopTools_ListOfShape myArguments;          //!< Input shapes for making them connected
  TopTools_IndexedMapOfShape myAllInputsMap; //!< Map of all BRep sub-elements of the input shapes

  // Tools
  BOPAlgo_MakePeriodic myPeriodicityMaker;   //!< Tool for making the shape periodic

  // Results
  NCollection_DataMap
    <TopoDS_Shape,
     TopTools_ListOfShape,
     TopTools_OrientedShapeMapHasher> myMaterials; //!< Map of the materials associations
                                                   //! for the border elements
  TopTools_DataMapOfShapeListOfShape myOrigins;    //!< Map of origins
                                                   //! (allows tracking the shape's ancestors)

  Handle(BRepTools_History) myGlueHistory;         //!< Gluing History
  Handle(BRepTools_History) myHistory;             //!< Final History of shapes modifications
                                                   //! (including making the shape periodic and repetitions)

  TopoDS_Shape myGlued;                            //!< The resulting connected (glued) shape
  TopoDS_Shape myShape;                            //!< The resulting shape
};

#endif // _BOPAlgo_MakeConnected_HeaderFile
