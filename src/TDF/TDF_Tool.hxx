// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _TDF_Tool_HeaderFile
#define _TDF_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_AttributeMap.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <Standard_CString.hxx>
#include <TDF_LabelList.hxx>
#include <TDF_LabelIntegerMap.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class TDF_IDFilter;
class TCollection_AsciiString;
class TDF_Data;


//! This class provides general services for a data framework.
class TDF_Tool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the number of labels of the tree,
  //! including <aLabel>. aLabel is also included in this figure.
  //! This information is useful in setting the size of an array.
  Standard_EXPORT static Standard_Integer NbLabels (const TDF_Label& aLabel);
  
  //! Returns the total number of attributes attached
  //! to the labels dependent on the label aLabel.
  //! The attributes of aLabel are also included in this figure.
  //! This information is useful in setting the size of an array.
  Standard_EXPORT static Standard_Integer NbAttributes (const TDF_Label& aLabel);
  
  //! Returns the number of attributes of the tree,
  //! selected by a<Filter>, including those of
  //! <aLabel>.
  Standard_EXPORT static Standard_Integer NbAttributes (const TDF_Label& aLabel, const TDF_IDFilter& aFilter);
  
  //! Returns true if <aLabel> and its descendants
  //! reference only attributes or labels attached to
  //! themselves.
  Standard_EXPORT static Standard_Boolean IsSelfContained (const TDF_Label& aLabel);
  
  //! Returns true if <aLabel> and its descendants
  //! reference only attributes or labels attached to
  //! themselves and kept by <aFilter>.
  Standard_EXPORT static Standard_Boolean IsSelfContained (const TDF_Label& aLabel, const TDF_IDFilter& aFilter);
  
  //! Returns in <theAtts> the attributes having out
  //! references.
  //!
  //! Caution: <theAtts> is not cleared before use!
  Standard_EXPORT static void OutReferers (const TDF_Label& theLabel, TDF_AttributeMap& theAtts);
  
  //! Returns in <atts> the attributes having out
  //! references and kept by <aFilterForReferers>.
  //! It considers only the references kept by <aFilterForReferences>.
  //! Caution: <atts> is not cleared before use!
  Standard_EXPORT static void OutReferers (const TDF_Label& aLabel, const TDF_IDFilter& aFilterForReferers, const TDF_IDFilter& aFilterForReferences, TDF_AttributeMap& atts);
  
  //! Returns in <atts> the referenced attributes.
  //! Caution: <atts> is not cleared before use!
  Standard_EXPORT static void OutReferences (const TDF_Label& aLabel, TDF_AttributeMap& atts);
  
  //! Returns in <atts> the referenced attributes and kept by <aFilterForReferences>.
  //! It considers only the referers kept by <aFilterForReferers>.
  //! Caution: <atts> is not cleared before use!
  Standard_EXPORT static void OutReferences (const TDF_Label& aLabel, const TDF_IDFilter& aFilterForReferers, const TDF_IDFilter& aFilterForReferences, TDF_AttributeMap& atts);
  
  //! Returns the label having the same sub-entry as
  //! <aLabel> but located as descendant as <toRoot>
  //! instead of <fromRoot>.
  //!
  //! Exemple :
  //!
  //! aLabel = 0:3:24:7:2:7
  //! fromRoot = 0:3:24
  //! toRoot = 0:5
  //! returned label = 0:5:7:2:7
  Standard_EXPORT static void RelocateLabel (const TDF_Label& aSourceLabel, const TDF_Label& fromRoot, const TDF_Label& toRoot, TDF_Label& aTargetLabel, const Standard_Boolean create = Standard_False);
  
  //! Returns the entry for the label aLabel in the form
  //! of the ASCII character string anEntry containing
  //! the tag list for aLabel.
  Standard_EXPORT static void Entry (const TDF_Label& aLabel, TCollection_AsciiString& anEntry);
  
  //! Returns the entry of <aLabel> as list of integers
  //! in <aTagList>.
  Standard_EXPORT static void TagList (const TDF_Label& aLabel, TColStd_ListOfInteger& aTagList);
  
  //! Returns the entry expressed by <anEntry> as list
  //! of integers in <aTagList>.
  Standard_EXPORT static void TagList (const TCollection_AsciiString& anEntry, TColStd_ListOfInteger& aTagList);
  
  //! Returns the label expressed by <anEntry>; creates
  //! the label if it does not exist and if <create> is
  //! true.
  Standard_EXPORT static void Label (const Handle(TDF_Data)& aDF, const TCollection_AsciiString& anEntry, TDF_Label& aLabel, const Standard_Boolean create = Standard_False);
  
  //! Returns the label expressed by <anEntry>; creates
  //! the label if it does not exist and if <create> is
  //! true.
  Standard_EXPORT static void Label (const Handle(TDF_Data)& aDF, const Standard_CString anEntry, TDF_Label& aLabel, const Standard_Boolean create = Standard_False);
  
  //! Returns the label expressed by <anEntry>; creates
  //! the label if it does not exist and if <create> is
  //! true.
  Standard_EXPORT static void Label (const Handle(TDF_Data)& aDF, const TColStd_ListOfInteger& aTagList, TDF_Label& aLabel, const Standard_Boolean create = Standard_False);
  
  //! Adds the labels of <aLabelList> to <aLabelMap> if
  //! they are unbound, or increases their reference
  //! counters. At the end of the process, <aLabelList>
  //! contains only the ADDED labels.
  Standard_EXPORT static void CountLabels (TDF_LabelList& aLabelList, TDF_LabelIntegerMap& aLabelMap);
  
  //! Decreases the reference counters of the labels of
  //! <aLabelList> to <aLabelMap>, and removes labels
  //! with null counter. At the end of the process,
  //! <aLabelList> contains only the SUPPRESSED labels.
  Standard_EXPORT static void DeductLabels (TDF_LabelList& aLabelList, TDF_LabelIntegerMap& aLabelMap);
  
  //! Dumps <aDF> and its labels and their attributes.
  Standard_EXPORT static void DeepDump (Standard_OStream& anOS, const Handle(TDF_Data)& aDF);
  
  //! Dumps <aDF> and its labels and their attributes,
  //! if their IDs are kept by <aFilter>. Dumps also the
  //! attributes content.
  Standard_EXPORT static void ExtendedDeepDump (Standard_OStream& anOS, const Handle(TDF_Data)& aDF, const TDF_IDFilter& aFilter);
  
  //! Dumps <aLabel>, its chilren and their attributes.
  Standard_EXPORT static void DeepDump (Standard_OStream& anOS, const TDF_Label& aLabel);
  
  //! Dumps <aLabel>, its chilren and their attributes,
  //! if their IDs are kept by <aFilter>. Dumps also the
  //! attributes content.
  Standard_EXPORT static void ExtendedDeepDump (Standard_OStream& anOS, const TDF_Label& aLabel, const TDF_IDFilter& aFilter);




protected:





private:





};







#endif // _TDF_Tool_HeaderFile
