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

#ifndef _BRepAlgoAPI_Defeaturing_HeaderFile
#define _BRepAlgoAPI_Defeaturing_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_RemoveFeatures.hxx>
#include <BRepAlgoAPI_Algo.hxx>

//! The BRepAlgoAPI_Defeaturing algorithm is the API algorithm intended for
//! removal of the unwanted parts from the shape. The unwanted parts
//! (or features) can be holes, protrusions, gaps, chamfers, fillets etc.
//! The shape itself is not modified, the new shape is built as the result.
//!
//! The actual removal of the features from the shape is performed by
//! the low-level *BOPAlgo_RemoveFeatures* tool. So the defeaturing algorithm
//! has the same options, input data requirements, limitations as the
//! low-level algorithm.
//!
//! <b>Input data</b>
//!
//! Currently, only the shapes of type SOLID, COMPSOLID, and COMPOUND of Solids
//! are supported. And only the FACEs can be removed from the shape.
//!
//! On the input the algorithm accepts the shape itself and the
//! features which have to be removed. It does not matter how the features
//! are given. It could be the separate faces or the collections
//! of faces. The faces should belong to the initial shape, and those that
//! do not belong will be ignored.
//!
//! <b>Options</b>
//!
//! The algorithm has the following options:
//! - History support;
//!
//! and the options available from base class:
//! - Error/Warning reporting system;
//! - Parallel processing mode.
//!
//! Please note that the other options of the base class are not supported
//! here and will have no effect.
//!
//! For the details on the available options please refer to the description
//! of *BOPAlgo_RemoveFeatures* algorithm.
//!
//! <b>Limitations</b>
//!
//! The defeaturing algorithm has the same limitations as *BOPAlgo_RemoveFeatures*
//! algorithm.
//!
//! <b>Example</b>
//!
//! Here is the example of usage of the algorithm:
//! ~~~~
//! TopoDS_Shape aSolid = ...;               // Input shape to remove the features from
//! TopTools_ListOfShape aFeatures = ...;    // Features to remove from the shape
//! Standard_Boolean bRunParallel = ...;     // Parallel processing mode
//! Standard_Boolean isHistoryNeeded = ...;  // History support
//!
//! BRepAlgoAPI_Defeaturing aDF;             // De-Featuring algorithm
//! aDF.SetShape(aSolid);                    // Set the shape
//! aDF.AddFacesToRemove(aFaces);            // Add faces to remove
//! aDF.SetRunParallel(bRunParallel);        // Define the processing mode (parallel or single)
//! aDF.SetToFillHistory(isHistoryNeeded);   // Define whether to track the shapes modifications
//! aDF.Build();                             // Perform the operation
//! if (!aDF.IsDone())                       // Check for the errors
//! {
//!   // error treatment
//!   Standard_SStream aSStream;
//!   aDF.DumpErrors(aSStream);
//!   return;
//! }
//! if (aDF.HasWarnings())                   // Check for the warnings
//! {
//!   // warnings treatment
//!   Standard_SStream aSStream;
//!   aDF.DumpWarnings(aSStream);
//! }
//! const TopoDS_Shape& aResult = aDF.Shape(); // Result shape
//! ~~~~
//!
//! The algorithm preserves the type of the input shape in the result shape. Thus,
//! if the input shape is a COMPSOLID, the resulting solids will also be put into a COMPSOLID.
//!
class BRepAlgoAPI_Defeaturing : public BRepAlgoAPI_Algo
{
public:
  DEFINE_STANDARD_ALLOC

public: //! @name Constructors
  //! Empty constructor
  BRepAlgoAPI_Defeaturing()
      : BRepAlgoAPI_Algo(),
        myFillHistory(Standard_True)
  {
  }

public: //! @name Setting input data for the algorithm
  //! Sets the shape for processing.
  //! @param[in] theShape  The shape to remove the features from.
  //!                      It should either be the SOLID, COMPSOLID or COMPOUND of Solids.
  void SetShape(const TopoDS_Shape& theShape) { myInputShape = theShape; }

  //! Returns the input shape
  const TopoDS_Shape& InputShape() const { return myInputShape; }

  //! Adds the features to remove from the input shape.
  //! @param[in] theFace  The shape to extract the faces for removal.
  void AddFaceToRemove(const TopoDS_Shape& theFace) { myFacesToRemove.Append(theFace); }

  //! Adds the faces to remove from the input shape.
  //! @param[in] theFaces  The list of shapes to extract the faces for removal.
  void AddFacesToRemove(const TopTools_ListOfShape& theFaces)
  {
    TopTools_ListIteratorOfListOfShape it(theFaces);
    for (; it.More(); it.Next())
      myFacesToRemove.Append(it.Value());
  }

  //! Returns the list of faces which have been requested for removal
  //! from the input shape.
  const TopTools_ListOfShape& FacesToRemove() const { return myFacesToRemove; }

public: //! @name Performing the operation
  //! Performs the operation
  Standard_EXPORT virtual void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

public: //! @name History Methods
  //! Defines whether to track the modification of the shapes or not.
  void SetToFillHistory(const Standard_Boolean theFlag) { myFillHistory = theFlag; }

  //! Returns whether the history was requested or not.
  Standard_Boolean HasHistory() const { return myFillHistory; }

  //! Returns the list of shapes modified from the shape <theS> during the operation.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified(const TopoDS_Shape& theS)
    Standard_OVERRIDE;

  //! Returns the list of shapes generated from the shape <theS> during the operation.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated(const TopoDS_Shape& theS)
    Standard_OVERRIDE;

  //! Returns true if the shape <theS> has been deleted during the operation.
  //! It means that the shape has no any trace in the result.
  //! Otherwise it returns false.
  Standard_EXPORT virtual Standard_Boolean IsDeleted(const TopoDS_Shape& theS) Standard_OVERRIDE;

  //! Returns true if any of the input shapes has been modified during operation.
  Standard_EXPORT virtual Standard_Boolean HasModified() const;

  //! Returns true if any of the input shapes has generated shapes during operation.
  Standard_EXPORT virtual Standard_Boolean HasGenerated() const;

  //! Returns true if any of the input shapes has been deleted during operation.
  Standard_EXPORT virtual Standard_Boolean HasDeleted() const;

  //! Returns the History of shapes modifications
  Handle(BRepTools_History) History() { return myFeatureRemovalTool.History(); }

protected: //! @name Setting the algorithm into default state
  virtual void Clear() Standard_OVERRIDE
  {
    BRepAlgoAPI_Algo::Clear();
    myFeatureRemovalTool.Clear();
  }

protected:                                     //! @name Fields
  TopoDS_Shape         myInputShape;           //!< Input shape to remove the features from
  TopTools_ListOfShape myFacesToRemove;        //!< Features to remove from the shape
  Standard_Boolean     myFillHistory;          //!< Defines whether to track the history of
                                               //! shapes modifications or not (true by default)
  BOPAlgo_RemoveFeatures myFeatureRemovalTool; //!< Tool for the features removal
};

#endif // _BRepAlgoAPI_Defeaturing_HeaderFile
