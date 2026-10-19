// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <STEPControl_Reader.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_Direction.hxx>
#include <StepVisual_StyledItem.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

#include <istream>

//==================================================================================================

TEST(StepData_StepReaderDataTest, ReadEntity_UnresolvedReferenceDoesNotBindRecord)
{
  for (const int aReference : {11, 999, 10})
  {
    SCOPED_TRACE(aReference);
    TCollection_AsciiString aText("ISO-10303-21;\nHEADER;\n"
                                  "FILE_DESCRIPTION(('Unresolved reference'),'2;1');\n"
                                  "FILE_NAME('reference.stp','',(''),(''),'','','');\n"
                                  "FILE_SCHEMA(('AUTOMOTIVE_DESIGN'));\nENDSEC;\nDATA;\n"
                                  "#10=DIRECTION('',(0.,0.,1.));\n"
                                  "#20=DIRECTION('',(1.,0.,0.));\n"
                                  "#30=COLOUR_RGB('',1.,0.,0.);\n"
                                  "#40=FILL_AREA_STYLE_COLOUR('',#30);\n"
                                  "#50=FILL_AREA_STYLE('',(#40));\n"
                                  "#60=SURFACE_STYLE_FILL_AREA(#50);\n"
                                  "#70=SURFACE_SIDE_STYLE('',(#60));\n"
                                  "#80=SURFACE_STYLE_USAGE(.BOTH.,#70);\n"
                                  "#90=PRESENTATION_STYLE_ASSIGNMENT((#80));\n"
                                  "#100=STYLED_ITEM('color',(#90),#");
    aText += aReference;
    aText += ");\nENDSEC;\nEND-ISO-10303-21;\n";
    Standard_ArrayStreamBuffer aBuffer(aText.ToCString(), aText.Length());
    std::istream               aStream(&aBuffer);
    STEPControl_Reader         aReader;
    ASSERT_EQ(aReader.ReadStream("reference.stp", aStream), IFSelect_RetDone);
    const occ::handle<StepData_StepModel> aModel       = aReader.StepModel();
    int                                   aStyledItems = 0;
    for (int anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
    {
      const occ::handle<StepVisual_StyledItem> aStyled =
        occ::down_cast<StepVisual_StyledItem>(aModel->Value(anIndex));
      if (aStyled.IsNull())
      {
        continue;
      }
      ++aStyledItems;
      if (aReference == 10)
      {
        const occ::handle<StepGeom_Direction> aDirection =
          occ::down_cast<StepGeom_Direction>(aStyled->Item());
        ASSERT_FALSE(aDirection.IsNull());
        EXPECT_EQ(aModel->IdentLabel(aDirection), 10);
      }
      else
      {
        EXPECT_TRUE(aStyled->Item().IsNull());
      }
    }
    EXPECT_EQ(aStyledItems, 1);
  }
}
