#ifndef _OCCT_TestUtils_HeaderFile
#define _OCCT_TestUtils_HeaderFile

#include <gtest/gtest.h>
#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <OSD_File.hxx>
#include <iostream>
#include <fstream>
#include <Draw_Drawable3D.hxx>
#include <DBRep_DrawableShape.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Curve.hxx>
#include <DrawTrSurf_Surface.hxx>
#include <DrawTrSurf_BSplineCurve.hxx>
#include <DrawTrSurf_BSplineSurface.hxx>
#include <DrawTrSurf_BezierCurve.hxx>
#include <DrawTrSurf_BezierSurface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Precision.hxx>

/**
 * @brief Robustly load test data from disk.
 * Tries OCCT_TEST_DATA_DIR macro (CMake), then CSF_TestDataPath (Env).
 */
static Standard_Boolean LoadTestData(const TCollection_AsciiString& theFileName, TopoDS_Shape& theShape)
{
  static Standard_Boolean factoriesRegistered = Standard_False;
  if (!factoriesRegistered)
  {
    DBRep_DrawableShape::RegisterFactory();
    DrawTrSurf_Curve::RegisterFactory();
    DrawTrSurf_Surface::RegisterFactory();
    DrawTrSurf_BSplineCurve::RegisterFactory();
    DrawTrSurf_BSplineSurface::RegisterFactory();
    DrawTrSurf_BezierCurve::RegisterFactory();
    DrawTrSurf_BezierSurface::RegisterFactory();
    factoriesRegistered = Standard_True;
  }

  TCollection_AsciiString anEnvVars;

#ifdef OCCT_TEST_DATA_DIR
  anEnvVars = OCCT_TEST_DATA_DIR;
#else
  anEnvVars = OSD_Environment("CSF_TestDataPath").Value();
#endif

  if (anEnvVars.IsEmpty()) {
    std::cerr << "Error: Neither OCCT_TEST_DATA_DIR macro nor CSF_TestDataPath env variable is set." << std::endl;
    return Standard_False;
  }

  // Common subdirectories to check
  TCollection_AsciiString aSubDirs[] = { "", "/brep", "/geom", "/step", "/iges", "/stl" };

#ifdef _WIN32
  const char* aSep = ";";
#else
  const char* aSep = ":";
#endif

  BRep_Builder aBuilder;
  for (Standard_Integer i = 1; ; ++i)
  {
    TCollection_AsciiString aBaseDir = anEnvVars.Token(aSep, i);
    if (aBaseDir.IsEmpty())
      break;

    for (const auto& aSub : aSubDirs) {
      TCollection_AsciiString aPath = aBaseDir + aSub + "/" + theFileName;
      
      // Try leading with Draw Restore (handles DrawTrSurf_ and DBRep_DrawableShape headers)
      std::ifstream aFile(aPath.ToCString());
      if (aFile.is_open())
      {
        char aBuff[256];
        if (aFile >> aBuff) {
          TCollection_AsciiString aType(aBuff);
          Handle(Draw_Drawable3D) aDrawable;
          try {
            aDrawable = Draw_Drawable3D::Restore(aType.ToCString(), aFile);
            if (aDrawable.IsNull()) {
              // Fallback to DBRep_DrawableShape (for Draw BRep or binary BRep)
              aFile.clear();
              aFile.seekg(0, std::ios::beg);
              aDrawable = Draw_Drawable3D::Restore("DBRep_DrawableShape", aFile);
            }
          } catch (...) {}

          if (!aDrawable.IsNull()) {
            if (Handle(DBRep_DrawableShape) aDS = Handle(DBRep_DrawableShape)::DownCast(aDrawable)) {
              theShape = aDS->Shape();
            } else if (Handle(DrawTrSurf_Curve) aDC = Handle(DrawTrSurf_Curve)::DownCast(aDrawable)) {
              theShape = BRepBuilderAPI_MakeEdge(aDC->GetCurve());
            } else if (Handle(DrawTrSurf_Surface) aDSur = Handle(DrawTrSurf_Surface)::DownCast(aDrawable)) {
              theShape = BRepBuilderAPI_MakeFace(aDSur->GetSurface(), Precision::Confusion());
            }

            if (!theShape.IsNull()) {
              //std::cout << "LoadTestData: Loaded " << aPath << " via Draw Restore. Type: " << theShape.ShapeType() << std::endl;
              return Standard_True;
            }
          }
        }
        aFile.close();
      }

      // Try final fallback with standard BRepTools::Read (no headers)
      try {
        if (BRepTools::Read(theShape, aPath.ToCString(), aBuilder)) {
          //std::cout << "LoadTestData: Loaded " << aPath << " via BRepTools. Type: " << theShape.ShapeType() << std::endl;
          return Standard_True;
        }
      } catch (...) {}
    }
  }

  std::cerr << "Error: Could not find or read test data file '" << theFileName.ToCString() 
            << "' in paths '" << anEnvVars.ToCString() << "'" << std::endl;
  return Standard_False;
}

#endif // _OCCT_TestUtils_HeaderFile
