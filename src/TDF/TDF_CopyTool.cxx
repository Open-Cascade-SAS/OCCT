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

//      	---------------------
// Version:	0.0
//Version	Date		Purpose
//		0.0	Mar 11 1997	Creation

#include <Standard_TypeMismatch.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeDataMap.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ClosureTool.hxx>
#include <TDF_CopyTool.hxx>
#include <TDF_DataMapIteratorOfAttributeDataMap.hxx>
#include <TDF_DataMapIteratorOfLabelDataMap.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelDataMap.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_RelocationTable.hxx>

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

void TDF_CopyTool::Copy
(const Handle(TDF_DataSet)& aSourceDataSet,
 const Handle(TDF_RelocationTable)& aRelocationTable) 
{
  TDF_IDFilter privilegeFilter; // Ignore the target attribute's privilege!
  TDF_IDFilter refFilter; // Will not be used.
  TDF_CopyTool::Copy
    (aSourceDataSet, aRelocationTable, privilegeFilter,
     refFilter, Standard_False);
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

void TDF_CopyTool::Copy
(const Handle(TDF_DataSet)& aSourceDataSet,
 const Handle(TDF_RelocationTable)& aRelocationTable,
 const TDF_IDFilter& aPrivilegeFilter) 
{
  TDF_IDFilter refFilter; // Will not be used.
  TDF_CopyTool::Copy
    (aSourceDataSet, aRelocationTable, aPrivilegeFilter,
     refFilter, Standard_False);

}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

void TDF_CopyTool::Copy
(const Handle(TDF_DataSet)&             aSourceDataSet,
 const Handle(TDF_RelocationTable)&     aRelocationTable,
 const TDF_IDFilter&                    aPrivilegeFilter,
 const TDF_IDFilter&                    /* aRefFilter */,
 const Standard_Boolean                 /* setSelfContained */) 
{
  if (aSourceDataSet->IsEmpty()) return;

  TDF_LabelMap&        srcLabs = aSourceDataSet->Labels();
  TDF_AttributeMap&    srcAtts = aSourceDataSet->Attributes();
  TDF_LabelList&       rootLst = aSourceDataSet->Roots();

  TDF_LabelDataMap&     theLabMap  = aRelocationTable->LabelTable();
  TDF_AttributeDataMap& theAttMap  = aRelocationTable->AttributeTable();

  // Parallel exploration of the root label structures:
  // * builds the target labels not found;
  // * binds the source attributes with the target ones;
  // * binds the source attributes with new target ones if there is none.

  // Label pre-binding is tested before paste.
  // For it is possible to copy the roots at another place with OTHER TAGS,
  // we first ask <theLabMap> if each source root label is already bound.

  for (TDF_ListIteratorOfLabelList labLItr(rootLst);
       labLItr.More(); labLItr.Next()) {
    const TDF_Label& sLab = labLItr.Value();
    if (theLabMap.IsBound(sLab)) {
      TDF_Label tIns(theLabMap.Find(sLab));
      TDF_CopyTool::CopyLabels(sLab,tIns,
        theLabMap,theAttMap,srcLabs,srcAtts);
    }
    // if not bound : do nothing!
  }

  // The relocation attribute table is now ready,
  // except for the label unattached attributes,
  // but everybody can update the relocation table...

  // Now: the paste phasis!
  TDF_DataMapIteratorOfAttributeDataMap attItr2(theAttMap);
  for (;attItr2.More(); attItr2.Next()) {
    const Handle(TDF_Attribute)& sAtt = attItr2.Key();
    if (!sAtt.IsNull()) { // This condition looks superfluous; and below also.
      const Handle(TDF_Attribute)& tAtt = attItr2.Value();
      // 1 - No copy on itself.
      // 2 - The target attribute is present BUT its privilege over the
      // source one must be ignored. The source attribute can be copied.
      if ((sAtt != tAtt) && aPrivilegeFilter.IsIgnored(tAtt->ID()))
        sAtt->Paste(tAtt,aRelocationTable);
    }
  }
}



//=======================================================================
//function : CopyLabels
//purpose  : Internal root label copy recursive method.
//=======================================================================

void TDF_CopyTool::CopyLabels
(const TDF_Label& aSLabel,
 TDF_Label& aTargetLabel,
 TDF_LabelDataMap& aLabMap,
 TDF_AttributeDataMap& aAttMap,
 const TDF_LabelMap& aSrcLabelMap,
 const TDF_AttributeMap& aSrcAttributeMap)
{
  TDF_CopyTool::CopyAttributes(aSLabel,aTargetLabel,
    aAttMap,aSrcAttributeMap);

  // Does the same for the children.
  for (TDF_ChildIterator childItr(aSLabel); childItr.More(); childItr.Next()){
    const TDF_Label& childSLab = childItr.Value();
    if (aSrcLabelMap.Contains(childSLab)) {
      TDF_Label childTIns = aTargetLabel.FindChild(childSLab.Tag());
      aLabMap.Bind(childSLab,childTIns);
      TDF_CopyTool::CopyLabels(childSLab,childTIns,
        aLabMap,aAttMap,
        aSrcLabelMap,aSrcAttributeMap);
    }
  }
}

//=======================================================================
//function : CopyAttributes
//purpose  : Internal attribute copy method.
//=======================================================================

void TDF_CopyTool::CopyAttributes
(const TDF_Label& aSLabel,
 TDF_Label& aTargetLabel,
 TDF_AttributeDataMap& aAttMap,
 const TDF_AttributeMap& aSrcAttributeMap)
{
  Handle(TDF_Attribute) tAtt;

  // Finds the target attributes or creates them empty.
  for (TDF_AttributeIterator attItr(aSLabel);
       attItr.More(); attItr.Next()) {
    const Handle(TDF_Attribute) sAtt = attItr.Value();
    if (aSrcAttributeMap.Contains(sAtt)) {
      const Standard_GUID& id = sAtt->ID();
      if (!aTargetLabel.FindAttribute(id,tAtt)) {
        tAtt = sAtt->NewEmpty();
        if(tAtt->ID() != id) 
          tAtt->SetID(id);//
        aTargetLabel.AddAttribute(tAtt, Standard_True);
        aAttMap.Bind(sAtt,tAtt);
      }
      else {
        // Some attributes have the same ID, but are different and
        // exclusive. This obliged to test the dynamic type identity.
        if (tAtt->IsInstance(sAtt->DynamicType()))
          aAttMap.Bind(sAtt,tAtt);
        else
          throw Standard_TypeMismatch("TDF_CopyTool: Cannot paste to a different type attribute.");
      }
    }
  }
}
