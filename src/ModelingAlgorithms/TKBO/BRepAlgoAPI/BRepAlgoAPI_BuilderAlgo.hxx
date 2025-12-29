// Created by: Peter KURNEV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _BRepAlgoAPI_BuilderAlgo_HeaderFile
#define _BRepAlgoAPI_BuilderAlgo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_GlueEnum.hxx>
#include <BOPAlgo_PPaveFiller.hxx>
#include <BOPAlgo_PBuilder.hxx>
#include <BRepAlgoAPI_Algo.hxx>
#include <BRepTools_History.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

//! The class contains API level of the General Fuse algorithm.
//!
//! Additionally to the options defined in the base class, the algorithm has
//! the following options:
//! - *Safe processing mode* - allows to avoid modification of the input
//!                            shapes during the operation (by default it is off);
//! - *Gluing options* - allows to speed up the calculation of the intersections
//!                      on the special cases, in which some sub-shapes are coinciding.
//! - *Disabling the check for inverted solids* - Disables/Enables the check of the input solids
//!                          for inverted status (holes in the space). The default value is TRUE,
//!                          i.e. the check is performed. Setting this flag to FALSE for inverted
//!                          solids, most likely will lead to incorrect results.
//! - *Disabling history collection* - allows disabling the collection of the history
//!                                    of shapes modifications during the operation.
//!
//! It returns the following Error statuses:
//! - 0 - in case of success;
//! - *BOPAlgo_AlertTooFewArguments* - in case there are no enough arguments to perform the
//! operation;
//! - *BOPAlgo_AlertIntersectionFailed* - in case the intersection of the arguments has failed;
//! - *BOPAlgo_AlertBuilderFailed* - in case building of the result shape has failed.
//!
//! Warnings statuses from underlying DS Filler and Builder algorithms
//! are collected in the report.
//!
//! The class provides possibility to simplify the resulting shape by unification
//! of the tangential edges and faces. It is performed by the method *SimplifyResult*.
//! See description of this method for more details.
//!
class BRepAlgoAPI_BuilderAlgo : public BRepAlgoAPI_Algo
{
public:
  DEFINE_STANDARD_ALLOC

public: //! @name Constructors
  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_BuilderAlgo();
  Standard_EXPORT ~BRepAlgoAPI_BuilderAlgo() override;

  //! Constructor with prepared Filler object
  Standard_EXPORT BRepAlgoAPI_BuilderAlgo(const BOPAlgo_PaveFiller& thePF);

  //! Copy constructor is not allowed
  BRepAlgoAPI_BuilderAlgo(const BRepAlgoAPI_BuilderAlgo&) = delete;

  //! Move constructor is not allowed
  BRepAlgoAPI_BuilderAlgo(BRepAlgoAPI_BuilderAlgo&&) = delete;

  //! Copy assignment is not allowed
  BRepAlgoAPI_BuilderAlgo& operator=(const BRepAlgoAPI_BuilderAlgo&) = delete;

  //! Move assignment is not allowed
  BRepAlgoAPI_BuilderAlgo& operator=(BRepAlgoAPI_BuilderAlgo&&) = delete;

public: //! @name Setting/Getting data for the algorithm
  //! Sets the arguments
  void SetArguments(const NCollection_List<TopoDS_Shape>& theLS) { myArguments = theLS; }

  //! Gets the arguments
  const NCollection_List<TopoDS_Shape>& Arguments() const { return myArguments; }

public: //! @name Setting options
  //! Sets the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  void SetNonDestructive(const bool theFlag) { myNonDestructive = theFlag; }

  //! Returns the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  bool NonDestructive() const { return myNonDestructive; }

  //! Sets the glue option for the algorithm,
  //! which allows increasing performance of the intersection
  //! of the input shapes.
  void SetGlue(const BOPAlgo_GlueEnum theGlue) { myGlue = theGlue; }

  //! Returns the glue option of the algorithm
  BOPAlgo_GlueEnum Glue() const { return myGlue; }

  //! Enables/Disables the check of the input solids for inverted status
  void SetCheckInverted(const bool theCheck) { myCheckInverted = theCheck; }

  //! Returns the flag defining whether the check for input solids on inverted status
  //! should be performed or not.
  bool CheckInverted() const { return myCheckInverted; }

public: //! @name Performing the operation
  //! Performs the algorithm
  Standard_EXPORT void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) override;

public: //! @name Result simplification
  //! Simplification of the result shape is performed by the means of
  //! *ShapeUpgrade_UnifySameDomain* algorithm. The result of the operation will
  //! be overwritten with the simplified result.
  //!
  //! The simplification is performed without creation of the Internal shapes,
  //! i.e. shapes connections will never be broken.
  //!
  //! Simplification is performed on the whole result shape. Thus, if the input
  //! shapes contained connected tangent edges or faces unmodified during the operation
  //! they will also be unified.
  //!
  //! After simplification, the History of result simplification is merged into the main
  //! history of operation. So, it is taken into account when asking for Modified,
  //! Generated and Deleted shapes.
  //!
  //! Some options of the main operation are passed into the Unifier:
  //! - Fuzzy tolerance of the operation is given to the Unifier as the linear tolerance.
  //! - Non destructive mode here controls the safe input mode in Unifier.
  //!
  //! @param theUnifyEdges Controls the edges unification. TRUE by default.
  //! @param theUnifyFaces Controls the faces unification. TRUE by default.
  //! @param theAngularTol Angular criteria for tangency of edges and faces.
  //!                      Precision::Angular() by default.
  Standard_EXPORT void SimplifyResult(const bool   theUnifyEdges = true,
                                      const bool   theUnifyFaces = true,
                                      const double theAngularTol = Precision::Angular());

public: //! @name History support
  //! Returns the shapes modified from the shape <theS>.
  //! If any, the list will contain only those splits of the
  //! given shape, contained in the result.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Modified(const TopoDS_Shape& theS) override;

  //! Returns the list of shapes generated from the shape <theS>.
  //! In frames of Boolean Operations algorithms only Edges and Faces
  //! could have Generated elements, as only they produce new elements
  //! during intersection:
  //! - Edges can generate new vertices;
  //! - Faces can generate new edges and vertices.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Generated(
    const TopoDS_Shape& theS) override;

  //! Checks if the shape <theS> has been completely removed from the result,
  //! i.e. the result does not contain the shape itself and any of its splits.
  //! Returns TRUE if the shape has been deleted.
  Standard_EXPORT bool IsDeleted(const TopoDS_Shape& aS) override;

  //! Returns true if any of the input shapes has been modified during operation.
  Standard_EXPORT virtual bool HasModified() const;

  //! Returns true if any of the input shapes has generated shapes during operation.
  Standard_EXPORT virtual bool HasGenerated() const;

  //! Returns true if any of the input shapes has been deleted during operation.
  //! Normally, General Fuse operation should not have Deleted elements,
  //! but all derived operation can have.
  Standard_EXPORT virtual bool HasDeleted() const;

public: //! @name Enabling/Disabling the history collection.
  //! Allows disabling the history collection
  void SetToFillHistory(const bool theHistFlag) { myFillHistory = theHistFlag; }

  //! Returns flag of history availability
  bool HasHistory() const { return myFillHistory; }

public: //! @name Getting the section edges
  //! Returns a list of section edges.
  //! The edges represent the result of intersection between arguments of operation.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& SectionEdges();

public: //! @name Getting tools performing the job
  //! Returns the Intersection tool
  const BOPAlgo_PPaveFiller& DSFiller() const { return myDSFiller; }

  //! Returns the Building tool
  const BOPAlgo_PBuilder& Builder() const { return myBuilder; }

  //! History tool
  occ::handle<BRepTools_History> History() const { return myFillHistory ? myHistory : nullptr; }

protected: //! @name Setting options to the Intersection tool
  //! Sets options (available in child classes) for the intersection tool.
  //! Here it does nothing.
  virtual void SetAttributes() {}

protected: //! @name Protected methods for shapes intersection and building result
  //! Intersects the given shapes with the intersection tool
  Standard_EXPORT void IntersectShapes(const NCollection_List<TopoDS_Shape>& theArgs,
                                       const Message_ProgressRange&          theRange);

  //! Builds the resulting shape
  Standard_EXPORT void BuildResult(const Message_ProgressRange& theRange = Message_ProgressRange());

protected: //! @name Clearing the contents of the algorithm
  //! Clears the algorithm from previous runs
  Standard_EXPORT void Clear() override;

protected: //! @name Fields
  // Inputs
  NCollection_List<TopoDS_Shape> myArguments; //!< Arguments of the operation

  // Options
  bool             myNonDestructive; //!< Non-destructive mode management
  BOPAlgo_GlueEnum myGlue;           //!< Gluing mode management
  bool             myCheckInverted;  //!< Check for inverted solids management
  bool             myFillHistory;    //!< Controls the history collection

  // Tools
  bool myIsIntersectionNeeded;              //!< Flag to control whether the intersection
                                            //! of arguments should be performed or not
  BOPAlgo_PPaveFiller myDSFiller;           //!< Intersection tool performs intersection of the
                                            //! argument shapes.
  BOPAlgo_PBuilder myBuilder;               //!< Building tool performs construction of the result
                                            //! basing on the results of intersection
  occ::handle<BRepTools_History> myHistory; //!< General History tool, containing all History of
                                            //! shapes modifications during the operation
                                            //! (including result simplification)
  occ::handle<BRepTools_History> mySimplifierHistory; //!< History of result shape simplification
};

#endif // _BRepAlgoAPI_BuilderAlgo_HeaderFile
