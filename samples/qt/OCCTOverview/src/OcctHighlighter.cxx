// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "OcctHighlighter.h"

#include <Standard_WarningsDisable.hxx>
#include <QFont>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>





OcctHighlighter::OcctHighlighter(QTextDocument* theParent)
: QSyntaxHighlighter (theParent)
{
  QStringList aKeywordPatterns;
  aKeywordPatterns
    << "\\balignas\\b" << "\\balignof\\b" << "\\band\\b" << "\\band_eq\\b" << "\\basm\\b" 
    << "\\bauto\\b" << "\\bbitand\\b" << "\\bbitor\\b" << "\\bbool\\b" << "\\bbreak\\b" 
    << "\\bcase\\b" << "\\bcatch\\b" << "\\bchar\\b" << "\\bchar16_t\\b" << "\\bchar32_t\\b"
    << "\\bclass\\b" << "\\bcompl\\b" << "\\bconst\\b" << "\\bconstexpr\\b" << "\\bconst_cast\\b"
    << "\\bcontinue\\b" << "\\bdecltype\\b" << "\\bdefault\\b" << "\\bdelete\\b" << "\\bdo\\b" 
    << "\\bdouble\\b" << "\\bdynamic_cast\\b" << "\\belse\\b" << "\\benum\\b" << "\\bexplicit\\b"
    << "\\bexport\\b" << "\\bextern\\b" << "\\bfalse\\b" << "\\bfloat\\b" << "\\bfor\\b" 
    << "\\bfriend\\b" << "\\bgoto\\b" << "\\bif\\b" << "\\binline\\b" << "\\bint\\b" << "\\blong\\b" 
    << "\\bmutable\\b" << "\\bnamespace\\b" << "\\bnew\\b" << "\\bnoexcept\\b" << "\\bnot\\b" 
    << "\\bnot_eq\\b" << "\\bnullptr\\b" << "\\boperator\\b" << "\\bor\\b" << "\\bor_eq\\b"
    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b" << "\\bregister\\b" 
    << "\\breinterpret_cast\\b" << "\\breturn\\b" << "\\bshort\\b" << "\\bsigned\\b" << "\\bsizeof\\b"
    << "\\bstatic\\b" << "\\bstatic_assert\\b" << "\\bstatic_cast\\b" << "\\bstruct\\b" 
    << "\\bswitch\\b" << "\\btemplate\\b" << "\\bthis\\b" << "\\bthread_local\\b" << "\\bthrow\\b"
    << "\\btrue\\b" << "\\btry\\b" << "\\btypedef\\b" << "\\btypeid\\b" << "\\btypename\\b"
    << "\\bunion\\b" << "\\bunsigned\\b" << "\\busing\\b" << "\\bvirtual\\b" << "\\bvoid\\b" 
    << "\\bvolatile\\b" << "\\bwchar_t\\b" << "\\bwhile\\b" << "\\bxor\\b" << "\\bxor_eq\\b"
    << "\\boverride\\b" << "\\bfinal\\b";

  QStringList anOcctPatterns;
  anOcctPatterns 
    << "gp_Pnt" << "gp_XYZ" << "gp_Vec" << "gp_Dir" << "gp_Ax1" << "gp_Ax2" << "gp_Ax3" << "gp_Lin"
    << "gp_Circ" << "gp_Elips" << "gp_Parab" << "gp_Hypr" << "gp_Cylinder" << "gp_Cone" << "gp_Sphere" 
    << "gp_Torus" << "gp_Pnt2d" << "gp_XY" << "gp_Ax2d" << "gp_Ax22d" << "gp_Lin2d" << "gp_Circ2d" 
    << "gp_Elips2d" << "gp_Parab2d" << "gp_Hypr2d" << "Geom2d_BSplineCurve" << "Geom2d_BezierCurve" 
    << "Geom2d_OffsetCurve" << "ProjLib" << "ElSLib" << "IntAna_IntConicQuad" << "GccAna_Lin2d2Tan" 
    << "GccEnt_QualifiedCirc" << "Geom2dAPI_ProjectPointOnCurve" << "Geom2dAPI_ExtremaCurveCurve" 
    << "Geom2dAPI_InterCurveCurve" << "Geom2dAPI_PointsToBSpline" << "Geom_CartesianPoint" 
    << "Geom_VectorWithMagnitude" << "Geom_Axis1Placement" << "Geom_Axis2Placement" << "Geom_Line" 
    << "Geom_Circle" << "Geom_Ellipse" << "Geom_Parabola" << "Geom_Hyperbola" << "Geom_BSplineCurve" 
    << "Geom_BezierCurve" << "Geom_TrimmedCurve" << "Geom_OffsetCurve" << "Geom_BSplineSurface" 
    << "Geom_BezierSurface" << "Geom_Plane" << "Geom_CylindricalSurface" << "Geom_ConicalSurface" 
    << "Geom_SphericalSurface" << "Geom_ToroidalSurface" << "Geom_RectangularTrimmedSurface" 
    << "Geom_OffsetSurface" << "Geom_SurfaceOfLinearExtrusion" << "Geom_SurfaceOfRevolution" 
    << "BndLib_Add3dCurve" << "BndLib_AddSurface" << "GeomAdaptor_Curve" << "GeomAdaptor_Surface" 
    << "GeomAPI_PointsToBSpline" << "GeomAPI_PointsToBSplineSurface" << "GeomConvert" 
    << "Geom2d_CartesianPoint" << "Geom2d_VectorWithMagnitude" << "Geom2d_Line" << "Geom2d_Circle" 
    << "Geom2d_Ellipse" << "Geom2d_Parabola" << "Geom2d_Hyperbola" << "Geom2d_TrimmedCurve" 
    << "Geom2dAdaptor_Curve" << "Bnd_Box2d" << "BndLib_Add2dCurve" << "Adaptor2d_Curve2d" 
    << "BRepBuilderAPI_MakeEdge" << "BRepBuilderAPI_MakeFace" << "BRepPrimAPI_MakeBox" << "AIS_Point" 
    << "AIS_TextLabel" << "AIS_Axis" << "AIS_Circle" << "AIS_Plane" << "AIS_Shape" 
    << "AIS_ColoredShape" << "GProp_PEquation" << "Extrema_ExtCS" << "GCPnts_QuasiUniformDeflection" 
    << "GProp_GProps" << "GProp_PrincipalProps" << "TopoDS" << "TopoDS_Iterator" << "TopoDS_Compound" 
    << "TopoDS_Edge" << "TopoDS_Face" << "TopoDS_Shell" << "TopoDS_Solid" << "TopoDS_Vertex" 
    << "TopoDS_Wire" << "TopExp" << "TopExp_Explorer" << "TColgp_Array2OfPnt" << "BRep_Builder" 
    << "BRepGProp" << "BRep_Tool" << "BRepTools" << "BRepTools_ReShape" << "BRepAdaptor_Curve" 
    << "BRepAdaptor_CompCurve" << "BRepAdaptor_Surface" << "BRepAlgoAPI_Common" << "BRepAlgoAPI_Cut" 
    << "BRepAlgoAPI_Fuse" << "BRepAlgoAPI_Section" << "BRepAlgoAPI_Splitter" << "BRepAlgoAPI_Defeaturing" 
    << "BRepBuilderAPI_Copy" << "BRepBuilderAPI_MakeVertex" << "BRepBuilderAPI_MakeEdge" 
    << "BRepBuilderAPI_MakeFace" << "BRepBuilderAPI_MakePolygon" << "BRepBuilderAPI_MakeShell" 
    << "BRepBuilderAPI_MakeSolid" << "BRepBuilderAPI_MakeWire" << "BRepBuilderAPI_NurbsConvert" 
    << "BRepBuilderAPI_Sewing" << "BRepBuilderAPI_Transform" << "BRepCheck_Analyzer" 
    << "BRepPrimAPI_MakeBox" << "BRepPrimAPI_MakeCylinder" << "BRepPrimAPI_MakeRevol" 
    << "BRepFilletAPI_MakeChamfer" << "BRepFilletAPI_MakeFillet" << "BRepOffsetAPI_MakeOffset" 
    << "BRepOffsetAPI_MakeEvolved.hxx" << "Standard_Integer" << "Standard_Real" << "Standard_Boolean" 
    << "Standard_ShortReal" << "Standard_Character" << "Standard_Byte" << "Standard_Address" 
    << "Standard_Size" << "Standard_Time" << "Standard_Utf8Char" << "Standard_Utf8UChar"
    << "Standard_ExtCharacter" << "Standard_Utf16Char" << "Standard_Utf32Char" << "Standard_WideChar" 
    << "Standard_CString" << "Standard_ExtString" << "NCollection_Vector" << "TCollection_AsciiString" 
    << "TCollection_BaseSequence"<< "TCollection_BasicMap" << "TCollection_BasicMapIterator" 
    << "TCollection_ExtendedString" << "TCollection_HAsciiString" << "TCollection_HExtendedString" 
    << "TCollection_MapNode" << "TCollection_MapNodePtr" << "TCollection_SeqNode" 
    << "TCollection_SeqNodePtr" << "TCollection_Side" << "Standard_False" << "Standard_True" 
    << "TCollection" << "NCollection" << "gp_Trsf" << "Handle" << "Aspect_TOL_DASH" 
    << "Aspect_TOM_O_STAR" << "Aspect_TOL_SOLID" << "Aspect_TOM_O_STAR" << "AIS_InteractiveObject" 
    << "AIS_ListOfInteractive" << "Aspect_GDM_Lines" << "Aspect_GDM_Points" << "Aspect_TOM_POINT"
    << "Aspect_TOM_RING1" << "Aspect_TOM_O" << "BinDrivers" << "DefineFormat" << "Font_FA_Bold" 
    << "Font_FA_BoldItalic" << "Font_FA_Italic" << "Font_FA_Regular" << "DownCast" << "gp_Pln" 
    << "Graphic3d_AspectMarker3d" << "Graphic3d_HTA_LEFT" << "Graphic3d_NameOfMaterial" 
    << "Graphic3d_NOM_BRONZE" << "Graphic3d_NOM_PLASTIC" << "Graphic3d_VTA_BOTTOM" 
    << "OpenGl_GraphicDriver" << "PCDM_RS_OK" << "PCDM_SS_OK" << "PCDM_ReaderStatus" 
    << "PCDM_StoreStatus" << "Prs3d_Drawer" << "TPrsStd_AISPresentation" << "Quantity_Color"
    << "Quantity_NameOfColor" << "Quantity_NOC_BLUE1" << "Quantity_NOC_CADETBLUE"
    << "Quantity_NOC_GREEN" << "Quantity_NOC_MAGENTA1" << "Quantity_NOC_RED" << "Quantity_NOC_YELLOW"
    << "Quantity_NOC_WHITE" << "Quantity_NOC_MATRABLUE" << "Quantity_TOC_RGB" << "Quantity_TOC_HLS" 
    << "Standard_GUID" << "TColStd_ListIteratorOfListOfTransient" << "TColStd_ListOfTransient" 
    << "TDataStd_Integer" << "TDataStd_Name" << "TDataStd_Real" << "TFunction_Driver"
    << "TFunction_DriverTable" << "TFunction_Function" << "TFunction_Logbook" << "TDF_Label" 
    << "TDF_TagSource" << "TNaming_NamedShape" << "TopAbs_EDGE" << "TopAbs_FACE" << "TopAbs_VERTEX"
    << "TPrsStd_AISPresentation" << "TPrsStd_AISViewer" << "V3d_AmbientLight" 
    << "V3d_DirectionalLight" << "V3d_PositionalLight" << "V3d_SpotLight" << "XmlDrivers";

  QStringList aHelperPatterns;
  aHelperPatterns 
    << "AdaptorCurve_AIS" << "AdaptorVec_AIS" << "AdaptorCurve2d_AIS" << "AdaptorPnt2d_AIS"
    << "Sample2D_Image" << "Sample2D_Markers" << "Sample2D_Face" << "TOcafFunction_BoxDriver" 
    << "TOcafFunction_CylDriver" << "DisplayPresentation";

  HighlightingRule aRule;

  myOcctFormat.setForeground(Qt::darkCyan);

  foreach (const QString& aPattern, anOcctPatterns)
  {
    aRule.myPattern = QRegExp(aPattern);
    aRule.myFormat = myOcctFormat;
    myHighlightingRules.append(aRule);
  }

  myHelperFormat.setForeground(Qt::red);
  foreach (const QString& aPattern, aHelperPatterns)
  {
    aRule.myPattern = QRegExp(aPattern);
    aRule.myFormat = myHelperFormat;
    myHighlightingRules.append(aRule);
  }


  myKeywordFormat.setForeground(Qt::darkBlue);
  myKeywordFormat.setFontWeight(QFont::Bold);
  foreach (const QString& aPattern, aKeywordPatterns)
  {
    aRule.myPattern = QRegExp(aPattern);
    aRule.myFormat = myKeywordFormat;
    myHighlightingRules.append(aRule);
  }

  myMemberFormat.setFontWeight(QFont::Bold);
  aRule.myPattern = QRegExp(QLatin1String("\\bmy[0-9A-Za-z]+\\b"));
  aRule.myFormat = myMemberFormat;
  myHighlightingRules.append(aRule);

  myLocalFormat.setForeground(Qt::darkMagenta);
  aRule.myPattern = QRegExp(QLatin1String("\\ba[0-9A-Za-z]+\\b"));
  aRule.myFormat = myLocalFormat;
  myHighlightingRules.append(aRule);

  myQuotationFormat.setForeground(Qt::darkRed);
  aRule.myPattern = QRegExp(QLatin1String("\".*\""));
  aRule.myFormat = myQuotationFormat;
  myHighlightingRules.append(aRule);

  myFunctionFormat.setFontItalic(true);
  myFunctionFormat.setForeground(Qt::blue);
  aRule.myPattern = QRegExp(QLatin1String("\\b[A-Za-z0-9_]+(?=\\()"));
  aRule.myFormat = myFunctionFormat;
  myHighlightingRules.append(aRule);

  mySingleLineCommentFormat.setForeground(Qt::darkGreen);
  aRule.myPattern = QRegExp(QLatin1String("//[^\n]*"));
  aRule.myFormat = mySingleLineCommentFormat;
  myHighlightingRules.append(aRule);

  myMultiLineCommentFormat.setForeground(Qt::darkGreen);

  myCommentStartExpression = QRegExp(QLatin1String("/\\*"));
  myCommentEndExpression = QRegExp(QLatin1String("\\*/"));
}

void OcctHighlighter::highlightBlock (const QString& theText)
{
  foreach (const HighlightingRule &rule, myHighlightingRules) 
  {
    QRegExp expression(rule.myPattern);
    int index = expression.indexIn(theText);
    while (index >= 0) 
    {
        int length = expression.matchedLength();
        setFormat(index, length, rule.myFormat);
        index = expression.indexIn(theText, index + length);
    }
  }

  setCurrentBlockState(0);

  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = theText.indexOf(myCommentStartExpression);

  while (startIndex >= 0) 
  {
    int endIndex = myCommentEndExpression.indexIn(theText, startIndex);
    int commentLength;
    if (endIndex == -1) 
    {
      setCurrentBlockState(1);
      commentLength = theText.length() - startIndex;
    } 
    else 
    {
      commentLength = endIndex - startIndex
                      + myCommentEndExpression.matchedLength();
    }
    setFormat(startIndex, commentLength, myMultiLineCommentFormat);
    startIndex = myCommentEndExpression.indexIn(theText, startIndex + commentLength);
  }
}
