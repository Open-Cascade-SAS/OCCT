// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdLPersistent_Data.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdObjMgt_WriteData.hxx>

#include <TDF_Data.hxx>
#include <TDF_Attribute.hxx>

//! Create a transient label tree from persistent data
class StdLPersistent_Data::Parser
{
public:
  //! Start parsing a persistent data.
  Parser (const TColStd_HArray1OfInteger&           theLabels,
          const StdLPersistent_HArray1OfPersistent& theAttributes)
    : myLabelsIter (theLabels)
    , myAttribIter (theAttributes) {}

  //! Fill a transient label with data.
  void FillLabel (TDF_Label theLabel)
  {
    Standard_Integer i;

    // Read attributes count
    myLabelsIter.Next();
    Standard_Integer anAttribCount = myLabelsIter.Value();

    // Add attributes to the label
    for (i = 0; i < anAttribCount; i++)
    {
      // read persistent attribute
      Handle(StdObjMgt_Persistent)& aPAttrib = myAttribIter.ChangeValue();
      myAttribIter.Next();
      // create transient attribute and add it to the label
      if (aPAttrib) {
        Handle (TDF_Attribute) anAtt = aPAttrib->CreateAttribute();
        anAtt->SetID();
        theLabel.AddAttribute (anAtt);
      }
    }

    // Read child labels count
    myLabelsIter.Next();
    Standard_Integer aSubLabelsCount = myLabelsIter.Value();

    // Create child labels
    for (i = 0; i < aSubLabelsCount; i++)
    {
      // read tag of child label
      myLabelsIter.Next();
      Standard_Integer aSubLabelTag = myLabelsIter.Value();

      // create and fill child label
      TDF_Label aSubLabel = theLabel.FindChild (aSubLabelTag, Standard_True);
      FillLabel (aSubLabel);
    }
  }

private:
  TColStd_HArray1OfInteger          ::Iterator myLabelsIter;
  StdLPersistent_HArray1OfPersistent::Iterator myAttribIter;
};

//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void StdLPersistent_Data::Read (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myVersion >> myLabels >> myAttributes;
}

//=======================================================================
//function : Write
//purpose  : Write persistent data to a file
//=======================================================================
void StdLPersistent_Data::Write (StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myVersion << myLabels << myAttributes;
}

//=======================================================================
//function : Import
//purpose  : Import transient data from the persistent data
//=======================================================================
Handle(TDF_Data) StdLPersistent_Data::Import() const
{
  if (myLabels.IsNull() || myAttributes.IsNull())
    return NULL;

  // Create tree of labels and add empty transient attributes to them
  Handle(TDF_Data) aData = new TDF_Data;
  Parser (*myLabels->Array(), *myAttributes->Array()).FillLabel (aData->Root());

  // Import transient attribuites from persistent data
  StdLPersistent_HArray1OfPersistent::Iterator anAttribIter (*myAttributes->Array());
  for (; anAttribIter.More(); anAttribIter.Next())
  {
    Handle(StdObjMgt_Persistent)& aPAttrib = anAttribIter.ChangeValue();
    if (aPAttrib)
      aPAttrib->ImportAttribute();
  }

  return aData;
}
