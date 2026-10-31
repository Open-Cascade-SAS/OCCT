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
#include <StepVisual_ColourRgb.hxx>
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

//==================================================================================================

// Line breaks inside a string are not part of its value (ISO 10303-21, 5.2 and 5.6).
// They must be ignored even when they cut a UTF-8 character, a control directive or a doubled
// apostrophe or backslash.
TEST(StepData_StepReaderDataTest, ReadString_LineBreaksInsideStringAreIgnored)
{
  // the expected value is a two-character Korean word (U+C544 U+C8FC), written in UTF-8
  const char* const aCases[][2] = {
    {"'\xEC\x95\n\x84\xEC\xA3\xBC'", "\xEC\x95\x84\xEC\xA3\xBC"},   // inside a UTF-8 character
    {"'\xEC\x95\x84\r\n\xEC\xA3\xBC'", "\xEC\x95\x84\xEC\xA3\xBC"}, // between characters, CR LF
    {"'\\X2\\C544\nC8FC\\X0\\'", "\xEC\x95\x84\xEC\xA3\xBC"},       // between two hex groups
    {"'\\X2\\C5\n44C8FC\\X0\\'", "\xEC\x95\x84\xEC\xA3\xBC"},       // inside one hex group
    {"'\\\nX2\\C544C8FC\\X0\\'", "\xEC\x95\x84\xEC\xA3\xBC"},       // after the first backslash
    {"'\\X2\\C544C8FC\\X\r\n0\\'", "\xEC\x95\x84\xEC\xA3\xBC"},     // inside the closing directive
    {"'It'\n's'", "It's"},                                          // between doubled apostrophes
    {"'C:\\\n\\temp'", "C:\\temp"},                                 // between doubled backslashes
    {"'\n'", ""}};
  TCollection_AsciiString aText("ISO-10303-21;\nHEADER;\n"
                                "FILE_DESCRIPTION(('Line breaks'),'2;1');\n"
                                "FILE_NAME('breaks.stp','',(''),(''),'','','');\n"
                                "FILE_SCHEMA(('AUTOMOTIVE_DESIGN'));\nENDSEC;\nDATA;\n");
  for (int anIndex = 0; anIndex < int(sizeof(aCases) / sizeof(aCases[0])); ++anIndex)
  {
    aText += "#";
    aText += anIndex + 1;
    aText += "=COLOUR_RGB(";
    aText += aCases[anIndex][0];
    aText += ",1.,0.,0.);\n";
  }
  aText += "ENDSEC;\nEND-ISO-10303-21;\n";

  Standard_ArrayStreamBuffer aBuffer(aText.ToCString(), aText.Length());
  std::istream               aStream(&aBuffer);
  STEPControl_Reader         aReader;
  ASSERT_EQ(aReader.ReadStream("breaks.stp", aStream), IFSelect_RetDone);
  const occ::handle<StepData_StepModel> aModel = aReader.StepModel();
  ASSERT_EQ(aModel->NbEntities(), int(sizeof(aCases) / sizeof(aCases[0])));
  for (int anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
  {
    SCOPED_TRACE(anIndex);
    const occ::handle<StepVisual_ColourRgb> aColour =
      occ::down_cast<StepVisual_ColourRgb>(aModel->Value(anIndex));
    ASSERT_FALSE(aColour.IsNull());
    EXPECT_STREQ(aColour->Name()->ToCString(), aCases[anIndex - 1][1]);
  }
}
