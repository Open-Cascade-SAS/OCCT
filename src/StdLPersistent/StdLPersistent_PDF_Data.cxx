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

#include <StdLPersistent_PDF_Data.hxx>

#include <StdObjMgt_ReadData.hxx>

#include <TDF_Data.hxx>
#include <TDF_Attribute.hxx>

//! Create a transient label tree from persistent data
class StdLPersistent_PDF_Data::Parser
{
public:
  //! Start parsing a persistent data.
  Parser (const StdLPersistent_PDF_Data& theSource)
    : myLabelsIter (theSource.myLabels->Array())
    , myAttribIter (theSource.myAttributes->Array()) {}

  //! Fill a transient label with data.
  void FillLabel (TDF_Label theLabel)
  {
    Standard_Integer i;

    // Read count of attributes
    myLabelsIter.Next();
    Standard_Integer anAttribCount = myLabelsIter.Value();

    // Add attributes to the label
    for (i = 0 ; i < anAttribCount; i++)
    {
      // read persistent attribute
      const Handle(StdObjMgt_Persistent)& aPAttrib = myAttribIter.Value();
      myAttribIter.Next();

      // create transient attribute and add it to the label
      if (!aPAttrib.IsNull())
      {
        Handle(TDF_Attribute) aTAttrib = aPAttrib->ImportAttribute();
        if (!aTAttrib.IsNull())
          theLabel.AddAttribute (aTAttrib);
      }
    }

    // Read count of child labels
    myLabelsIter.Next();
    Standard_Integer aSubLabelsCount = myLabelsIter.Value();

    // Create child labels
    for (i = 0 ; i < aSubLabelsCount; i++)
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
  NCollection_Array1<Standard_Integer            >::Iterator myLabelsIter;
  NCollection_Array1<Handle(StdObjMgt_Persistent)>::Iterator myAttribIter;
};

//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void StdLPersistent_PDF_Data::Read (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myVersion >> myLabels >> myAttributes;
}

//=======================================================================
//function : Import
//purpose  : Import transient data from the persistent data
//=======================================================================
Handle(TDF_Data) StdLPersistent_PDF_Data::Import() const
{
  if (myLabels.IsNull() || myAttributes.IsNull())
    return NULL;

  Handle(TDF_Data) aData = new TDF_Data;
  Parser (*this).FillLabel (aData->Root());
  return aData;
}
