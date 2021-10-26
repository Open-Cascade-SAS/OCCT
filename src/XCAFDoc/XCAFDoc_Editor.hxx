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

#include <Standard_DefineAlloc.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelDataMap.hxx>
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
  Standard_EXPORT static Standard_Boolean Expand(const TDF_Label& theDoc,
                                                 const TDF_Label& theShape,
                                                 const Standard_Boolean theRecursively = Standard_True);

  //! Converts all compounds shapes in the document to assembly
  //! @param[in] theDoc input document
  //! @param[in] theShape input shape label
  //! @param[in] theRecursively recursively expand a compound subshape
  //! @return True if shape successfully expanded
  Standard_EXPORT static Standard_Boolean Expand(const TDF_Label& theDoc,
                                                 const Standard_Boolean theRecursively = Standard_True);

  //! Clones all labels to a new position, keeping the structure with all the attributes
  //! @param[in] theSrcLabels original labels to copy from
  //! @param[in] theDstLabel label to set result as a component of or a main document's label to simply set new shape
  //! @param[in] theIsNoVisMat get a VisMaterial attributes as is or convert to color
  //! @return True if shape successfully extracted
  Standard_EXPORT static Standard_Boolean Extract(const TDF_LabelSequence& theSrcLabels,
                                                  const TDF_Label& theDstLabel,
                                                  const Standard_Boolean theIsNoVisMat = Standard_False);

  //! Clones the label to a new position, keeping the structure with all the attributes
  //! @param[in] theSrcLabel original label to copy from
  //! @param[in] theDstLabel label to set result as a component of or a main document's label to simply set new shape
  //! @param[in] theIsNoVisMat get a VisMaterial attributes as is or convert to color
  //! @return True if shape successfully extracted
  Standard_EXPORT static Standard_Boolean Extract(const TDF_Label& theSrcLabel,
                                                  const TDF_Label& theDstLabel,
                                                  const Standard_Boolean theIsNoVisMat = Standard_False);

  //! Copies shapes label with keeping of shape structure (recursively)
  //! @param[in] theSrcLabel original label to copy from
  //! @param[in] theSrcShapeTool shape tool to get
  //! @param[in] theDstShapeTool shape tool to set
  //! @param[out] theMap relating map of the original shapes label and labels created from them
  //! @return result shape label
  Standard_EXPORT static TDF_Label CloneShapeLabel(const TDF_Label& theSrcLabel,
                                                   const Handle(XCAFDoc_ShapeTool)& theSrcShapeTool,
                                                   const Handle(XCAFDoc_ShapeTool)& theDstShapeTool,
                                                   TDF_LabelDataMap& theMap);

  //! Copies metadata contains from the source label to the destination label.
  //! Protected against creating a new label for non-existent tools
  //! @param[in] theSrcLabel original label to copy from
  //! @param[in] theDstLabel destination shape label to set attributes
  //! @param[in] theVisMatMap relating map of the original VisMaterial and created. Can be NULL for the same document
  //! @param[in] theToCopyColor copying visible value and shape color (handled all color type)
  //! @param[in] theToCopyLayer copying layer
  //! @param[in] theToCopyMaterial copying  material
  //! @param[in] theToCopyVisMaterial copying visual material
  //! @param[in] theToCopyAttributes copying of other node attributes, for example, a shape's property
  Standard_EXPORT static void CloneMetaData(const TDF_Label& theSrcLabel,
                                            const TDF_Label& theDstLabel,
                                            NCollection_DataMap<Handle(XCAFDoc_VisMaterial), Handle(XCAFDoc_VisMaterial)>* theVisMatMap,
                                            const Standard_Boolean theToCopyColor = Standard_True,
                                            const Standard_Boolean theToCopyLayer = Standard_True,
                                            const Standard_Boolean theToCopyMaterial = Standard_True,
                                            const Standard_Boolean theToCopyVisMaterial = Standard_True,
                                            const Standard_Boolean theToCopyAttributes = Standard_True);

};

#endif // _XCAFDoc_Editor_HeaderFile
