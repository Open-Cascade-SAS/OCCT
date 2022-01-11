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


#include <BOPTest.hxx>
#include <BOPTest_Objects.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <BOPAlgo_GlueEnum.hxx>

#include <string.h>
static Standard_Integer boptions (Draw_Interpretor&, Standard_Integer, const char**); 
static Standard_Integer brunparallel (Draw_Interpretor&, Standard_Integer, const char**); 
static Standard_Integer bnondestructive(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bfuzzyvalue(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bGlue(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bdrawwarnshapes(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bcheckinverted(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer buseobb(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bsimplify(Draw_Interpretor&, Standard_Integer, const char**);

//=======================================================================
//function : OptionCommands
//purpose  : 
//=======================================================================
void BOPTest::OptionCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOPTest commands";
  // Commands  
  theCommands.Add("boptions", "Usage: boptions [-default]\n"
                              "\t\tw/o arguments shows current value of BOP options\n"
                              "\t\t-default - allows setting all options to default values",
                  __FILE__, boptions, g);

  theCommands.Add("brunparallel", "Enables/Disables parallel processing mode.\n"
                                  "\t\tUsage: brunparallel 0/1",
                  __FILE__, brunparallel, g);

  theCommands.Add("bnondestructive", "Enables/Disables the safe processing mode.\n"
                                     "\t\tUsage: bnondestructive 0/1",
                  __FILE__, bnondestructive, g);

  theCommands.Add("bfuzzyvalue", "Sets the additional tolerance for BOP algorithms.\n"
                                 "\t\tUsage: bfuzzyvalue value",
                  __FILE__, bfuzzyvalue, g);

  theCommands.Add("bglue", "Sets the gluing mode for the BOP algorithms.\n"
                           "\t\tUsage: bglue [0 (off) / 1 (shift) / 2 (full)]",
                  __FILE__, bGlue, g);

  theCommands.Add("bdrawwarnshapes", "Enables/Disables drawing of warning shapes of BOP algorithms.\n"
                                     "\t\tUsage: bdrawwarnshapes 0 (do not draw) / 1 (draw warning shapes)",
                  __FILE__, bdrawwarnshapes, g);

  theCommands.Add("bcheckinverted", "Enables/Disables the check of the input solids on inverted status in BOP algorithms\n"
                                    "\t\tUsage: bcheckinverted 0 (off) / 1 (on)",
                  __FILE__, bcheckinverted, g);

  theCommands.Add("buseobb", "Enables/disables the usage of OBB in BOP algorithms\n"
                             "\t\tUsage: buseobb 0 (off) / 1 (on)",
                  __FILE__, buseobb, g);

  theCommands.Add("bsimplify", "Enables/Disables the result simplification after BOP\n"
                               "\t\tUsage: bsimplify [-e 0/1] [-f 0/1] [-a tol]\n"
                               "\t\t-e 0/1 - enables/disables edges unification\n"
                               "\t\t-f 0/1 - enables/disables faces unification\n"
                               "\t\t-a tol - changes default angular tolerance of unification algo (accepts value in degrees).",
                  __FILE__, bsimplify, g);
}
//=======================================================================
//function : boptions
//purpose  : 
//=======================================================================
Standard_Integer boptions(Draw_Interpretor& di,
                          Standard_Integer n, 
                          const char** a) 
{ 
  if (n > 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  if (n == 2)
  {
    if (strcmp(a[1], "-default"))
    {
      di.PrintHelp(a[0]);
      return 1;
    }

    // Set all options to default values
    BOPTest_Objects::SetDefaultOptions();
    return 0;
  }
  //
  char buf[128];
  BOPAlgo_GlueEnum aGlue = BOPTest_Objects::Glue();
  //
  Sprintf(buf, " RunParallel: %s \t\t(%s)\n",  BOPTest_Objects::RunParallel() ? "Yes" : "No",
               "use \"brunparallel\" command to change");
  di << buf;
  Sprintf(buf, " NonDestructive: %s \t\t(%s)\n", BOPTest_Objects::NonDestructive() ? "Yes" : "No",
                "use \"bnondestructive\" command to change");
  di << buf;
  Sprintf(buf, " FuzzyValue: %g \t\t(%s)\n", BOPTest_Objects::FuzzyValue(),
               "use \"bfuzzyvalue\" command to change");
  di << buf;
  Sprintf(buf, " GlueOption: %s \t\t(%s)\n", ((aGlue == BOPAlgo_GlueOff) ? "Off" :
    ((aGlue == BOPAlgo_GlueFull) ? "Full" : "Shift")),
               "use \"bglue\" command to change");
  di << buf;
  Sprintf(buf, " Draw Warning Shapes: %s \t(%s)\n", BOPTest_Objects::DrawWarnShapes() ? "Yes" : "No",
               "use \"bdrawwarnshapes\" command to change");
  di << buf;
  Sprintf(buf, " Check for invert solids: %s \t(%s)\n", BOPTest_Objects::CheckInverted() ? "Yes" : "No",
               "use \"bcheckinverted\" command to change");
  di << buf;
  Sprintf(buf, " Use OBB: %s \t\t\t(%s)\n", BOPTest_Objects::UseOBB() ? "Yes" : "No",
               "use \"buseobb\" command to change");
  di << buf;
  Sprintf(buf, " Unify Edges: %s \t\t(%s)\n", BOPTest_Objects::UnifyEdges() ? "Yes" : "No",
               "use \"bsimplify -e\" command to change");
  di << buf;
  Sprintf(buf, " Unify Faces: %s \t\t(%s)\n", BOPTest_Objects::UnifyFaces() ? "Yes" : "No",
               "use \"bsimplify -f\" command to change");
  di << buf;
  Sprintf(buf, " Angular: %g \t\t(%s)\n", BOPTest_Objects::Angular(),
               "use \"bsimplify -a\" command to change");
  di << buf;
  //
  return 0;
}
//=======================================================================
//function : bfuzzyvalue
//purpose  : 
//=======================================================================
Standard_Integer bfuzzyvalue(Draw_Interpretor& di,
                             Standard_Integer n, 
                             const char** a) 
{
  if (n != 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  Standard_Real aFuzzyValue = Draw::Atof(a[1]);
  BOPTest_Objects::SetFuzzyValue(aFuzzyValue);
  return 0;
}
//=======================================================================
//function : brunparallel
//purpose  : 
//=======================================================================
Standard_Integer brunparallel(Draw_Interpretor& di,
                              Standard_Integer n,
                              const char** a)
{
  if (n != 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  Standard_Integer iRunParallel = Draw::Atoi(a[1]);
  BOPTest_Objects::SetRunParallel(iRunParallel != 0);
  return 0;
}
//=======================================================================
//function : bnondestructive
//purpose  : 
//=======================================================================
Standard_Integer bnondestructive(Draw_Interpretor& di,
                                 Standard_Integer n,
                                 const char** a)
{
  if (n != 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  Standard_Integer iNonDestructive = Draw::Atoi(a[1]);
  BOPTest_Objects::SetNonDestructive(iNonDestructive != 0);
  return 0;
}

//=======================================================================
//function : bglue
//purpose  : 
//=======================================================================
Standard_Integer bGlue(Draw_Interpretor& di,
                       Standard_Integer n,
                       const char** a)
{
  if (n != 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  Standard_Integer iGlue = Draw::Atoi(a[1]);
  if (iGlue < 0 || iGlue > 2)
  {
    di << "Wrong value.\n";
    di.PrintHelp(a[0]);
    return 1;
  }

  BOPAlgo_GlueEnum aGlue = BOPAlgo_GlueEnum(iGlue);
  BOPTest_Objects::SetGlue(aGlue);
  return 0;
}

//=======================================================================
//function : bdrawwarnshapes
//purpose  : 
//=======================================================================
Standard_Integer bdrawwarnshapes(Draw_Interpretor& di,
                              Standard_Integer n,
                              const char** a)
{ 
  if (n != 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  Standard_Integer iDraw = Draw::Atoi(a[1]);
  BOPTest_Objects::SetDrawWarnShapes(iDraw != 0);
  return 0;
}

//=======================================================================
//function : bcheckinverted
//purpose  : 
//=======================================================================
Standard_Integer bcheckinverted(Draw_Interpretor& di,
                                Standard_Integer n,
                                const char** a)
{
  if (n != 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  Standard_Integer iCheck = Draw::Atoi(a[1]);
  BOPTest_Objects::SetCheckInverted(iCheck != 0);
  return 0;
}

//=======================================================================
//function : buseobb
//purpose  : 
//=======================================================================
Standard_Integer buseobb(Draw_Interpretor& di,
                         Standard_Integer n,
                         const char** a)
{
  if (n != 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  Standard_Integer iUse = Draw::Atoi(a[1]);
  BOPTest_Objects::SetUseOBB(iUse != 0);
  return 0;
}

//=======================================================================
//function : bsimplify
//purpose  : 
//=======================================================================
Standard_Integer bsimplify(Draw_Interpretor& di,
                           Standard_Integer n,
                           const char** a)
{
  if (n == 1 || n%2 == 0)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  for (Standard_Integer i = 1; i < n - 1; ++i)
  {
    if (!strcmp(a[i], "-e"))
    {
      Standard_Integer iUnifyEdges = Draw::Atoi(a[++i]);
      BOPTest_Objects::SetUnifyEdges(iUnifyEdges != 0);
    }
    else if (!strcmp(a[i], "-f"))
    {
      Standard_Integer iUnifyFaces = Draw::Atoi(a[++i]);
      BOPTest_Objects::SetUnifyFaces(iUnifyFaces != 0);
    }
    else if (!strcmp(a[i], "-a"))
    {
      Standard_Real anAngTol = Draw::Atof(a[++i]) * (M_PI / 180.0);
      BOPTest_Objects::SetAngular(anAngTol);
    }
    else
    {
      di << "Wrong key option.\n";
      di.PrintHelp(a[0]);
      return 1;
    }
  }
  return 0;
}
