// Created on: 2015-05-14
// Created by: Ilya Novikov
// Copyright (c) 2000-2015 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_Editor_HeaderFile
#define _XCAFDoc_Editor_HeaderFile

#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelDataMap.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_LabelSequence.hxx>

class XCAFDoc_VisMaterial;
class XCAFDoc_ShapeTool;

//! Tool for edit structure of document.
class XCAFDoc_Editor
{
public:
  DEFINE_STANDARD_ALLOC

  //! Converts shape (compound/compsolid/shell/wire) to assembly.
  //! @param[in] theDoc input document
  //! @param[in] theShape input shape label
  //! @param[in] theRecursively recursively expand a compound subshape
  //! @return True if shape successfully expanded
  Standard_EXPORT static Standard_Boolean Expand(
    const TDF_Label&       theDoc,
    const TDF_Label&       theShape,
    const Standard_Boolean theRecursively = Standard_True);

  //! Converts all compounds shapes in the document to assembly
  //! @param[in] theDoc input document
  //! @param[in] theRecursively recursively expand a compound subshape
  //! @return True if shape successfully expanded
  Standard_EXPORT static Standard_Boolean Expand(
    const TDF_Label&       theDoc,
    const Standard_Boolean theRecursively = Standard_True);

  //! Clones all labels to a new position, keeping the structure with all the attributes
  //! @param[in] theSrcLabels original labels to copy from
  //! @param[in] theDstLabel label to set result as a component of or a main document's label to
  //! simply set new shape
  //! @param[in] theIsNoVisMat get a VisMaterial attributes as is or convert to color
  //! @return True if shape successfully extracted
  Standard_EXPORT static Standard_Boolean Extract(
    const TDF_LabelSequence& theSrcLabels,
    const TDF_Label&         theDstLabel,
    const Standard_Boolean   theIsNoVisMat = Standard_False);

  //! Clones the label to a new position, keeping the structure with all the attributes
  //! @param[in] theSrcLabel original label to copy from
  //! @param[in] theDstLabel label to set result as a component of or a main document's label to
  //! simply set new shape
  //! @param[in] theIsNoVisMat get a VisMaterial attributes as is or convert to color
  //! @return True if shape successfully extracted
  Standard_EXPORT static Standard_Boolean Extract(
    const TDF_Label&       theSrcLabel,
    const TDF_Label&       theDstLabel,
    const Standard_Boolean theIsNoVisMat = Standard_False);

  //! Copies shapes label with keeping of shape structure (recursively)
  //! @param[in] theSrcLabel original label to copy from
  //! @param[in] theSrcShapeTool shape tool to get
  //! @param[in] theDstShapeTool shape tool to set
  //! @param[out] theMap relating map of the original shapes label and labels created from them
  //! @return result shape label
  Standard_EXPORT static TDF_Label CloneShapeLabel(const TDF_Label&                 theSrcLabel,
                                                   const Handle(XCAFDoc_ShapeTool)& theSrcShapeTool,
                                                   const Handle(XCAFDoc_ShapeTool)& theDstShapeTool,
                                                   TDF_LabelDataMap&                theMap);

  //! Copies metadata contains from the source label to the destination label.
  //! Protected against creating a new label for non-existent tools
  //! @param[in] theSrcLabel original label to copy from
  //! @param[in] theDstLabel destination shape label to set attributes
  //! @param[in] theVisMatMap relating map of the original VisMaterial and created. Can be NULL for
  //! the same document
  //! @param[in] theToCopyColor copying visible value and shape color (handled all color type)
  //! @param[in] theToCopyLayer copying layer
  //! @param[in] theToCopyMaterial copying  material
  //! @param[in] theToCopyVisMaterial copying visual material
  //! @param[in] theToCopyAttributes copying of other node attributes, for example, a shape's
  //! property
  Standard_EXPORT static void CloneMetaData(
    const TDF_Label&                                                               theSrcLabel,
    const TDF_Label&                                                               theDstLabel,
    NCollection_DataMap<Handle(XCAFDoc_VisMaterial), Handle(XCAFDoc_VisMaterial)>* theVisMatMap,
    const Standard_Boolean theToCopyColor       = Standard_True,
    const Standard_Boolean theToCopyLayer       = Standard_True,
    const Standard_Boolean theToCopyMaterial    = Standard_True,
    const Standard_Boolean theToCopyVisMaterial = Standard_True,
    const Standard_Boolean theToCopyAttributes  = Standard_True);

  //! Gets shape labels that has down relation with the input label.
  //! @param[in] theLabel input label
  //! @param[out] theRelatedLabels output labels
  Standard_EXPORT static void GetParentShapeLabels(const TDF_Label& theLabel,
                                                   TDF_LabelMap&    theRelatedLabels);

  //! Gets shape labels that has up relation with the input label.
  //! @param[in] theLabel input label
  //! @param[out] theRelatedLabels output labels
  Standard_EXPORT static void GetChildShapeLabels(const TDF_Label& theLabel,
                                                  TDF_LabelMap&    theRelatedLabels);

  //! Filters original shape tree with keeping structure.
  //! The result will include the full label hierarchy lower then input labels.
  //! Any higher hierarchy labels will be filtered to keep only necessary labels.
  //! All not related shape labels with input will be cleared (all attributes will be removed).
  //!
  //! The result impact directly into original document and existed shape labels.
  //! Attributes related to removed shape can became invalide.
  //! For example, GDT with relation on removed shape label(s) and without
  //! attachment point(s) became invalid for visualization.
  //!
  //! @param[in] theShapeTool shape tool to extract from
  //! @param[in] theLabelsToKeep labels to keep
  //! @return true if the tree was filtered successfully.
  Standard_EXPORT static bool FilterShapeTree(const Handle(XCAFDoc_ShapeTool)& theShapeTool,
                                              const TDF_LabelMap&              theLabelsToKeep);

  //! Applies geometrical scaling to the following assembly components:
  //! - part geometry
  //! - sub-assembly/part occurrence location
  //! - part's centroid, area and volume attributes
  //! - PMIs (warnings and errors are reported if it is impossible to make changes)
  //! Normally, should start from a root sub-assembly, but if theForceIfNotRoot true
  //! scaling will be applied forcibly. If theLabel corresponds to the shape tool
  //! scaling is applied to the whole assembly.
  //! @param[in] theLabel starting label
  //! @param[in] theScaleFactor scale factor, should be positive
  //! @param[in] theForceIfNotRoot allows scaling of a non root assembly if true,
  //!                              otherwise - returns false
  //! @return true in case of success, otherwise - false.
  Standard_EXPORT static Standard_Boolean RescaleGeometry(
    const TDF_Label&       theLabel,
    const Standard_Real    theScaleFactor,
    const Standard_Boolean theForceIfNotRoot = Standard_False);
};

#endif // _XCAFDoc_Editor_HeaderFile
