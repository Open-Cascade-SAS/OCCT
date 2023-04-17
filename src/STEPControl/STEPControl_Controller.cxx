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

//:j4 gka 16.03.99 S4134
//    gka 05.04.99 S4136: parameters definitions changed

#include <APIHeaderSection_EditHeader.hxx>
#include <APIHeaderSection_MakeHeader.hxx>
#include <IFSelect_EditForm.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_SignAncestor.hxx>
#include <IFSelect_SignCounter.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <RWHeaderSection.hxx>
#include <RWStepAP214.hxx>
#include <Standard_Type.hxx>
#include <Standard_Version.hxx>
#include <STEPControl_ActorRead.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_Controller.hxx>
#include <StepData_FileProtocol.hxx>
#include <StepData_StepModel.hxx>
#include <STEPEdit.hxx>
#include <STEPEdit_EditContext.hxx>
#include <STEPEdit_EditSDR.hxx>
#include <StepSelect_WorkLibrary.hxx>
#include <STEPSelections_SelectAssembly.hxx>
#include <STEPSelections_SelectDerived.hxx>
#include <STEPSelections_SelectFaces.hxx>
#include <STEPSelections_SelectForTransfer.hxx>
#include <STEPSelections_SelectGSCurves.hxx>
#include <STEPSelections_SelectInstances.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <XSAlgo.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPControl_Controller,XSControl_Controller)

//  Pour NewModel et Write : definition de produit (temporaire ...)
STEPControl_Controller::STEPControl_Controller ()
: XSControl_Controller ("STEP", "step")
{
  static Standard_Boolean init = Standard_False;
  if (!init) {
    RWHeaderSection::Init();  RWStepAP214::Init();

    Interface_Static::Init ("step","write.step.product.name",'t',"Open CASCADE STEP translator " OCC_VERSION_STRING);
    Interface_Static::Init ("step","write.step.assembly",'e',"");
    Interface_Static::Init ("step","write.step.assembly",'&',"enum 0");
    Interface_Static::Init ("step","write.step.assembly",'&',"eval Off");
    Interface_Static::Init ("step","write.step.assembly",'&',"eval On");
    Interface_Static::Init ("step","write.step.assembly",'&',"eval Auto");
    Interface_Static::SetCVal("write.step.assembly","Auto"); 

    Interface_Static::Init("step","step.angleunit.mode", 'e',"");
    Interface_Static::Init("step","step.angleunit.mode", '&',"enum 0");
    Interface_Static::Init("step","step.angleunit.mode", '&',"eval File");
    Interface_Static::Init("step","step.angleunit.mode", '&',"eval Rad");
    Interface_Static::Init("step","step.angleunit.mode", '&',"eval Deg");
    Interface_Static::SetCVal("step.angleunit.mode","File"); 

    Interface_Static::Init("step","write.step.schema", 'e',"");  
    Interface_Static::Init("step","write.step.schema",'&',"enum 1");
    Interface_Static::Init("step","write.step.schema",'&',"eval AP214CD");
    Interface_Static::Init("step","write.step.schema",'&',"eval AP214DIS");
    Interface_Static::Init("step","write.step.schema",'&',"eval AP203");
    Interface_Static::Init("step","write.step.schema",'&',"eval AP214IS");
    Interface_Static::Init("step","write.step.schema",'&',"eval AP242DIS");
    Interface_Static::SetCVal("write.step.schema","AP214IS"); 

    // Type of Product Definition for reading
    // Note: the numbers should be consistent with function FindShapeReprType()
    // in STEPControl_ActorRead.cxx
    Interface_Static::Init("step","read.step.shape.repr",'e',"");
    Interface_Static::Init("step","read.step.shape.repr",'&',"enum 1");
    Interface_Static::Init("step","read.step.shape.repr",'&',"eval All");   // 1
    Interface_Static::Init("step","read.step.shape.repr",'&',"eval ABSR");  // 2
    Interface_Static::Init("step","read.step.shape.repr",'&',"eval MSSR");  // 3
    Interface_Static::Init("step","read.step.shape.repr",'&',"eval GBSSR"); // 4
    Interface_Static::Init("step","read.step.shape.repr",'&',"eval FBSR");  // 5
    Interface_Static::Init("step","read.step.shape.repr",'&',"eval EBWSR"); // 6
    Interface_Static::Init("step","read.step.shape.repr",'&',"eval GBWSR"); // 7
    Interface_Static::SetCVal("read.step.shape.repr","All");

    // Mode for reading shapes attached to main SDR by SRR
    // (hybrid model representation in AP203 since 1998)
    Interface_Static::Init("step","read.step.shape.relationship",'e',"");
    Interface_Static::Init("step","read.step.shape.relationship",'&',"enum 0");
    Interface_Static::Init("step","read.step.shape.relationship",'&',"eval OFF");
    Interface_Static::Init("step","read.step.shape.relationship",'&',"eval ON");
    Interface_Static::SetCVal("read.step.shape.relationship","ON");

    // Mode for reading shapes attached to Product by ShapeAspect 
    // (hybrid model representation in AP203 before 1998)
    Interface_Static::Init("step","read.step.shape.aspect",'e',"");
    Interface_Static::Init("step","read.step.shape.aspect",'&',"enum 0");
    Interface_Static::Init("step","read.step.shape.aspect",'&',"eval OFF");
    Interface_Static::Init("step","read.step.shape.aspect",'&',"eval ON");
    Interface_Static::SetCVal("read.step.shape.aspect","ON");

    // Mode for reading SDR and ShapeRepr if it is necessary
    Interface_Static::Init("step","read.step.product.mode",'e',"");
    Interface_Static::Init("step","read.step.product.mode",'&',"enum 0");
    Interface_Static::Init("step","read.step.product.mode",'&',"eval OFF");
    Interface_Static::Init("step","read.step.product.mode",'&',"eval ON");
    Interface_Static::SetCVal("read.step.product.mode","ON");

    // Order of reading ShapeDefinitionRepresentation in ProductDefinition
    Interface_Static::Init("step","read.step.product.context",'e',"");
    Interface_Static::Init("step","read.step.product.context",'&',"enum 1");
    Interface_Static::Init("step","read.step.product.context",'&',"eval all");     // 1
    Interface_Static::Init("step","read.step.product.context",'&',"eval design");  // 2
    Interface_Static::Init("step","read.step.product.context",'&',"eval analysis");// 3
    Interface_Static::SetCVal("read.step.product.context","all");

    // What we try to read in ProductDefinition
    Interface_Static::Init("step","read.step.assembly.level",'e',"");
    Interface_Static::Init("step","read.step.assembly.level",'&',"enum 1");
    Interface_Static::Init("step","read.step.assembly.level",'&',"eval all");      // 1
    Interface_Static::Init("step","read.step.assembly.level",'&',"eval assembly"); // 2
    Interface_Static::Init("step","read.step.assembly.level",'&',"eval structure");// 3
    Interface_Static::Init("step","read.step.assembly.level",'&',"eval shape");    // 4
    Interface_Static::SetCVal("read.step.assembly.level","all");

    // unit: supposed to be cascade unit (target unit for reading)
    Interface_Static::Init("step","write.step.unit", 'e',"");
    Interface_Static::Init("step","write.step.unit",'&',"enum 1");
    Interface_Static::Init("step","write.step.unit",'&',"eval INCH");  // 1
    Interface_Static::Init("step","write.step.unit",'&',"eval MM");    // 2
    Interface_Static::Init("step","write.step.unit",'&',"eval ??");    // 3
    Interface_Static::Init("step","write.step.unit",'&',"eval FT");    // 4
    Interface_Static::Init("step","write.step.unit",'&',"eval MI");    // 5
    Interface_Static::Init("step","write.step.unit",'&',"eval M");     // 6
    Interface_Static::Init("step","write.step.unit",'&',"eval KM");    // 7
    Interface_Static::Init("step","write.step.unit",'&',"eval MIL");   // 8
    Interface_Static::Init("step","write.step.unit",'&',"eval UM");    // 9
    Interface_Static::Init("step","write.step.unit",'&',"eval CM");    //10
    Interface_Static::Init("step","write.step.unit",'&',"eval UIN");   //11
    Interface_Static::SetCVal ("write.step.unit","MM");

    // Non-manifold topology reading: OFF by default (ssv; 26.11.2010)
    Interface_Static::Init ("step","read.step.nonmanifold",'e',"");
    Interface_Static::Init ("step","read.step.nonmanifold",'&',"enum 0");
    Interface_Static::Init ("step","read.step.nonmanifold",'&',"eval Off");
    Interface_Static::Init ("step","read.step.nonmanifold",'&',"eval On");
    Interface_Static::SetIVal("read.step.nonmanifold",0); 

    // Non-manifold topology writing: OFF by default (ssv; 26.11.2010)
    Interface_Static::Init ("step","write.step.nonmanifold",'e',"");
    Interface_Static::Init ("step","write.step.nonmanifold",'&',"enum 0");
    Interface_Static::Init ("step","write.step.nonmanifold",'&',"eval Off");
    Interface_Static::Init ("step","write.step.nonmanifold",'&',"eval On");
    Interface_Static::SetIVal("write.step.nonmanifold",0); 

    // I-Deas-like STEP processing: OFF by default (ssv; 22.11.2010)
    Interface_Static::Init ("step","read.step.ideas",'e',"");
    Interface_Static::Init ("step","read.step.ideas",'&',"enum 0");
    Interface_Static::Init ("step","read.step.ideas",'&',"eval Off");
    Interface_Static::Init ("step","read.step.ideas",'&',"eval On");
    Interface_Static::SetIVal("read.step.ideas",0); 

    //Parameter to write all free vertices in one SDR (name and style of vertex are lost) (default) 
    //or each vertex in its own SDR (name and style of vertex are exported). (ika; 21.07.2014) 
    Interface_Static::Init ("step","write.step.vertex.mode",'e',"");
    Interface_Static::Init ("step","write.step.vertex.mode",'&',"enum 0");
    Interface_Static::Init ("step","write.step.vertex.mode",'&',"eval One Compound");
    Interface_Static::Init ("step","write.step.vertex.mode",'&',"eval Single Vertex");
    Interface_Static::SetIVal("write.step.vertex.mode",0);

    // abv 15.11.00: ShapeProcessing
    Interface_Static::Init ("XSTEP", "write.step.resource.name",                      't', "STEP");
    Interface_Static::Init ("XSTEP", "read.step.resource.name",                       't', "STEP");
    Interface_Static::Init ("XSTEP", "write.step.sequence",                           't', "ToSTEP");
    Interface_Static::Init ("XSTEP", "read.step.sequence",                            't', "FromSTEP");
    Interface_Static::Init ("XSTEP", "ToSTEP.exec.op",                                't', "SplitCommonVertex,DirectFaces");
    Interface_Static::Init ("XSTEP", "FromSTEP.exec.op",                              't', "FixShape");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.Tolerance3d",                 't', "&Runtime.Tolerance");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.MaxTolerance3d",              't', "&Runtime.MaxTolerance");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.MinTolerance3d",              't', "1.e-7");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixFreeShellMode",            't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixFreeFaceMode",             't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixFreeWireMode",             't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixSameParameterMode",        't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixSolidMode",                't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixShellOrientationMode",     't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.CreateOpenSolidMode",         't', "0");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixShellMode",                't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixFaceOrientationMode",      't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixFaceMode",                 't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixWireMode",                 't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixOrientationMode",          't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixAddNaturalBoundMode",      't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixMissingSeamMode",          't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixSmallAreaWireMode",        't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.RemoveSmallAreaFaceMode",     't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixIntersectingWiresMode",    't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixLoopWiresMode",            't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixSplitFaceMode",            't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.AutoCorrectPrecisionMode",    't', "1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.ModifyTopologyMode",          't', "0");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.ModifyGeometryMode",          't', "1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.ClosedWireMode",              't', "1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.PreferencePCurveMode",        't', "1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixReorderMode",              't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixSmallMode",                't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixConnectedMode",            't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixEdgeCurvesMode",           't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixDegeneratedMode",          't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixLackingMode",              't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixSelfIntersectionMode",     't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.RemoveLoopMode",              't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixReversed2dMode",           't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixRemovePCurveMode",         't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixRemoveCurve3dMode",        't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixAddPCurveMode",            't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixAddCurve3dMode",           't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixSeamMode",                 't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixShiftedMode",              't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixEdgeSameParameterMode",    't', "0");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixNotchedEdgesMode",         't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixTailMode",                 't', "0");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.MaxTailAngle",                't', "0.0");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.MaxTailWidth",                't', "-1.0");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixSelfIntersectingEdgeMode", 't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixIntersectingEdgesMode",    't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixNonAdjacentIntersectingEdgesMode", 't', "-1");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixVertexPositionMode",       't', "0");
    Interface_Static::Init ("XSTEP", "FromSTEP.FixShape.FixVertexToleranceMode",      't', "-1"); 
    Interface_Static::Init ("XSTEP", "FromSTEP.FixFaceSize.Tolerance",                't', "1.e-7"); 

    // ika 28.07.16: Parameter to read all top level solids and shells,
    // should be used only in case of invalid shape_representation without links to shapes.
    Interface_Static::Init("step", "read.step.all.shapes", 'e', "");
    Interface_Static::Init("step", "read.step.all.shapes", '&', "enum 0");
    Interface_Static::Init("step", "read.step.all.shapes", '&', "eval Off");
    Interface_Static::Init("step", "read.step.all.shapes", '&', "eval On");
    Interface_Static::SetIVal("read.step.all.shapes", 0);

     // Mode for reading constructive geometry representation relationship to read
    //StepRepr_ConstructiveGeometryRepresentation method implemented only for StepGeom_MakeAxis2Placement3d
    //for axis placements representing axis for suplemented geometry. Axis placements are translated to planar faces with CS 
    //equal to translated axis placements
    Interface_Static::Init("step","read.step.constructivegeom.relationship",'e',"");
    Interface_Static::Init("step","read.step.constructivegeom.relationship",'&',"enum 0");
    Interface_Static::Init("step","read.step.constructivegeom.relationship",'&',"eval OFF");
    Interface_Static::Init("step","read.step.constructivegeom.relationship",'&',"eval ON");
    Interface_Static::SetCVal("read.step.constructivegeom.relationship","OFF");

    // Mode to variate apply or not transformation placed in the root shape representation.
    // Issues #29068 and #31491.
    Interface_Static::Init("step", "read.step.root.transformation", 'e', "");
    Interface_Static::Init("step", "read.step.root.transformation", '&', "enum 0");
    Interface_Static::Init("step", "read.step.root.transformation", '&', "eval OFF");
    Interface_Static::Init("step", "read.step.root.transformation", '&', "eval ON");
    Interface_Static::SetCVal("read.step.root.transformation", "ON");

    // STEP file encoding for names translation
    // Note: the numbers should be consistent with Resource_FormatType enumeration
    Interface_Static::Init("step", "read.step.codepage", 'e', "");
    Interface_Static::Init("step", "read.step.codepage", '&', "enum 0");
    Interface_Static::Init("step", "read.step.codepage", '&', "eval SJIS");         // Resource_FormatType_SJIS 0
    Interface_Static::Init("step", "read.step.codepage", '&', "eval EUC");          // Resource_FormatType_EUC 1
    Interface_Static::Init("step", "read.step.codepage", '&', "eval NoConversion"); // Resource_FormatType_NoConversion 2
    Interface_Static::Init("step", "read.step.codepage", '&', "eval GB");           // Resource_FormatType_GB 3
    Interface_Static::Init("step", "read.step.codepage", '&', "eval UTF8");         // Resource_FormatType_UTF8 4
    Interface_Static::Init("step", "read.step.codepage", '&', "eval SystemLocale"); // Resource_FormatType_SystemLocale 5 
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP1250");       // Resource_FormatType_CP1250 6
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP1251");       // Resource_FormatType_CP1251 7
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP1252");       // Resource_FormatType_CP1252 8
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP1253");       // Resource_FormatType_CP1253 9
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP1254");       // Resource_FormatType_CP1254 10
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP1255");       // Resource_FormatType_CP1255 11
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP1256");       // Resource_FormatType_CP1256 12
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP1257");       // Resource_FormatType_CP1257 13
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP1258");       // Resource_FormatType_CP1258 14 
    Interface_Static::Init("step", "read.step.codepage", '&', "eval iso8859-1");    // Resource_FormatType_iso8859_1 15
    Interface_Static::Init("step", "read.step.codepage", '&', "eval iso8859-2");    // Resource_FormatType_iso8859_2 16 
    Interface_Static::Init("step", "read.step.codepage", '&', "eval iso8859-3");    // Resource_FormatType_iso8859_3 17
    Interface_Static::Init("step", "read.step.codepage", '&', "eval iso8859-4");    // Resource_FormatType_iso8859_4 18
    Interface_Static::Init("step", "read.step.codepage", '&', "eval iso8859-5");    // Resource_FormatType_iso8859_5 19
    Interface_Static::Init("step", "read.step.codepage", '&', "eval iso8859-6");    // Resource_FormatType_iso8859_6 20
    Interface_Static::Init("step", "read.step.codepage", '&', "eval iso8859-7");    // Resource_FormatType_iso8859_7 21
    Interface_Static::Init("step", "read.step.codepage", '&', "eval iso8859-8");    // Resource_FormatType_iso8859_8 22
    Interface_Static::Init("step", "read.step.codepage", '&', "eval iso8859-9");    // Resource_FormatType_iso8859_9 23
    Interface_Static::Init("step", "read.step.codepage", '&', "eval CP850");        // Resource_FormatType_CP850 24
    Interface_Static::SetCVal("read.step.codepage", "UTF8");

    // Tessellated geometry reading: Off by default
    Interface_Static::Init("step", "read.step.tessellated", 'e', "");
    Interface_Static::Init("step", "read.step.tessellated", '&', "enum 0");
    Interface_Static::Init("step", "read.step.tessellated", '&', "eval Off");       // 0
    Interface_Static::Init("step", "read.step.tessellated", '&', "eval On");        // 1
    Interface_Static::Init("step", "read.step.tessellated", '&', "eval OnNoBRep");  // 2
    Interface_Static::SetCVal("read.step.tessellated", "On");

    // Tessellated geometry writing: Off by default
    Interface_Static::Init("step", "write.step.tessellated", 'e', "");
    Interface_Static::Init("step", "write.step.tessellated", '&', "enum 0");
    Interface_Static::Init("step", "write.step.tessellated", '&', "eval Off");      // 0
    Interface_Static::Init("step", "write.step.tessellated", '&', "eval On");       // 1
    Interface_Static::Init("step", "write.step.tessellated", '&', "eval OnNoBRep"); // 2
    Interface_Static::SetCVal("write.step.tessellated", "OnNoBRep");

    Standard_STATIC_ASSERT((int)Resource_FormatType_CP850 - (int)Resource_FormatType_CP1250 == 18); // "Error: Invalid Codepage Enumeration"

    init = Standard_True;
  }

  Handle(STEPControl_ActorWrite) ActWrite = new STEPControl_ActorWrite;
  ActWrite->SetGroupMode (Interface_Static::IVal("write.step.assembly"));
  myAdaptorWrite = ActWrite;

  Handle(StepSelect_WorkLibrary) swl = new StepSelect_WorkLibrary;
  swl->SetDumpLabel(1);
  myAdaptorLibrary  = swl;
  myAdaptorProtocol = STEPEdit::Protocol();
  myAdaptorRead     = new STEPControl_ActorRead;  // par ex pour Recognize

  SetModeWrite (0,4);
  SetModeWriteHelp (0,"As Is");
  SetModeWriteHelp (1,"Faceted Brep");
  SetModeWriteHelp (2,"Shell Based");
  SetModeWriteHelp (3,"Manifold Solid");
  SetModeWriteHelp (4,"Wireframe");
  TraceStatic ("read.surfacecurve.mode",5);

  //   ---  SELECTIONS, SIGNATURES, COMPTEURS, EDITEURS

  DeclareAndCast(IFSelect_Selection,xmr,SessionItem("xst-model-roots"));
  if (!xmr.IsNull()) {
    Handle(IFSelect_Signature) sty = STEPEdit::SignType();
    AddSessionItem (sty,"step-type");
    Handle(IFSelect_SignCounter) tys = new IFSelect_SignCounter(sty,Standard_False,Standard_True);
    AddSessionItem (tys,"step-types");

    //szv:mySignType = sty;
    
    //pdn S4133 18.02.99
    AddSessionItem (new IFSelect_SignAncestor(),"xst-derived");

    Handle(STEPSelections_SelectDerived) stdvar = new STEPSelections_SelectDerived();
    stdvar->SetProtocol(STEPEdit::Protocol());
    AddSessionItem (stdvar,"step-derived");
    
    Handle(IFSelect_SelectSignature) selsdr = STEPEdit::NewSelectSDR();
    selsdr->SetInput (xmr);
    AddSessionItem (selsdr,"step-shape-def-repr");

    AddSessionItem (STEPEdit::NewSelectPlacedItem(),"step-placed-items");
    // input deja pret avec ModelAll
    AddSessionItem (STEPEdit::NewSelectShapeRepr(),"step-shape-repr");
  }
  
  //pdn
  Handle(STEPSelections_SelectFaces) stfaces = new STEPSelections_SelectFaces;
  stfaces->SetInput (xmr);
  AddSessionItem (stfaces,"step-faces");
  
  Handle(STEPSelections_SelectInstances) stinst = new STEPSelections_SelectInstances;
  AddSessionItem (stinst,"step-instances");
  
  Handle(STEPSelections_SelectGSCurves) stcurves = new STEPSelections_SelectGSCurves;
  stcurves->SetInput (xmr);
  AddSessionItem (stcurves,"step-GS-curves");
  
  Handle(STEPSelections_SelectAssembly) assembly = new STEPSelections_SelectAssembly;
  assembly->SetInput (xmr);
  AddSessionItem (assembly,"step-assembly");
  
  Handle(APIHeaderSection_EditHeader) edhead = new APIHeaderSection_EditHeader;
  Handle(IFSelect_EditForm) edheadf = new IFSelect_EditForm (edhead,Standard_False,Standard_True,"Step Header");
  AddSessionItem (edhead,"step-header-edit");
  AddSessionItem (edheadf,"step-header");

  Handle(STEPEdit_EditContext) edctx = new STEPEdit_EditContext;
  Handle(IFSelect_EditForm) edctxf = new IFSelect_EditForm (edctx,Standard_False,Standard_True,"STEP Product Definition Context");
  AddSessionItem (edctx,"step-context-edit");
  AddSessionItem (edctxf,"step-context");


  Handle(STEPEdit_EditSDR) edsdr = new STEPEdit_EditSDR;
  Handle(IFSelect_EditForm) edsdrf = new IFSelect_EditForm (edsdr,Standard_False,Standard_True,"STEP Product Data (SDR)");
  AddSessionItem (edsdr,"step-SDR-edit");
  AddSessionItem (edsdrf,"step-SDR-data");
}

Handle(Interface_InterfaceModel)  STEPControl_Controller::NewModel () const
{
  return STEPEdit::NewModel();
}

//  ####    PROVISOIRE ???   ####

IFSelect_ReturnStatus  STEPControl_Controller::TransferWriteShape
  (const TopoDS_Shape& shape,
   const Handle(Transfer_FinderProcess)& FP,
   const Handle(Interface_InterfaceModel)& model,
   const Standard_Integer modeshape,
   const Message_ProgressRange& theProgress) const
{
  if (modeshape < 0 || modeshape > 4) return IFSelect_RetError;
  Handle(STEPControl_ActorWrite) ActWrite =
    Handle(STEPControl_ActorWrite)::DownCast(myAdaptorWrite);
//    A PRESENT ON PASSE PAR LE PROFILE
  if (!ActWrite.IsNull()) 
    ActWrite->SetGroupMode (Interface_Static::IVal("write.step.assembly"));

  return XSControl_Controller::TransferWriteShape(shape, FP, model, modeshape, theProgress);
}

Standard_Boolean STEPControl_Controller::Init ()
{
  static Standard_Boolean inic = Standard_False;
  if (!inic) {
    Handle(STEPControl_Controller) STEPCTL = new STEPControl_Controller;
    STEPCTL->AutoRecord();  // avec les noms donnes a la construction
    XSAlgo::Init();                                                                                                        
    inic = Standard_True;
  }
  return Standard_True;
}
//=======================================================================
//function : Customise
//purpose  : 
//=======================================================================

void STEPControl_Controller::Customise(Handle(XSControl_WorkSession)& WS) 
{
  XSControl_Controller::Customise(WS);

  Handle(IFSelect_SelectModelRoots) slr;
  Handle(Standard_Transient) slr1 = WS->NamedItem("xst-model-roots");
  if(!slr1.IsNull())
    slr = Handle(IFSelect_SelectModelRoots)::DownCast(slr1);
  else  {
    slr = new IFSelect_SelectModelRoots;
    WS->AddNamedItem ("xst-model-roots",slr);
  }

  Handle(STEPSelections_SelectForTransfer) st1= new STEPSelections_SelectForTransfer;
  st1->SetReader (WS->TransferReader());
  WS->AddNamedItem ("xst-transferrable-roots",st1);

  if (!slr.IsNull()) {
    Handle(IFSelect_Signature) sty = STEPEdit::SignType();
    WS->AddNamedItem ("step-type",sty);
    
    Handle(IFSelect_SignCounter) tys = new IFSelect_SignCounter(sty,Standard_False,Standard_True);
    WS->AddNamedItem ("step-types",tys);

	//szv:mySignType = sty;
    WS->SetSignType( sty );
    
    //pdn S4133 18.02.99
    WS->AddNamedItem ("xst-derived",new IFSelect_SignAncestor());
    Handle(STEPSelections_SelectDerived) stdvar = new STEPSelections_SelectDerived();
    stdvar->SetProtocol(STEPEdit::Protocol());
    WS->AddNamedItem ("step-derived",stdvar);
    
    Handle(IFSelect_SelectSignature) selsdr = STEPEdit::NewSelectSDR();
    selsdr->SetInput (slr);
    WS->AddNamedItem ("step-shape-def-repr",selsdr);
    Handle(IFSelect_SelectSignature) selrrs = STEPEdit::NewSelectPlacedItem();
    WS->AddNamedItem ("step-placed-items",selrrs);
    Handle(IFSelect_SelectSignature) selsr = STEPEdit::NewSelectShapeRepr();
    // input deja pret avec ModelAll
    WS->AddNamedItem ("step-shape-repr",selsr);
  }
  
  //pdn
  Handle(STEPSelections_SelectFaces) stfaces = new STEPSelections_SelectFaces;
  stfaces->SetInput (slr);
  WS->AddNamedItem ("step-faces",stfaces);
  
  Handle(STEPSelections_SelectInstances) stinst = new STEPSelections_SelectInstances;
  WS->AddNamedItem ("step-instances",stinst);
  
  Handle(STEPSelections_SelectGSCurves) stcurves = new STEPSelections_SelectGSCurves;
  stcurves->SetInput (slr);
  WS->AddNamedItem ("step-GS-curves",stcurves);
  
  Handle(STEPSelections_SelectAssembly) assembly = new STEPSelections_SelectAssembly;
  assembly->SetInput (slr);
  WS->AddNamedItem ("step-assembly",assembly);
  
  Handle(APIHeaderSection_EditHeader) edhead = new APIHeaderSection_EditHeader;
  Handle(IFSelect_EditForm) edheadf = new IFSelect_EditForm (edhead,Standard_False,Standard_True,"Step Header");
  WS->AddNamedItem ("step-header-edit",edhead);
  WS->AddNamedItem ("step-header",edheadf);

  Handle(STEPEdit_EditContext) edctx = new STEPEdit_EditContext;
  Handle(IFSelect_EditForm) edctxf = new IFSelect_EditForm (edctx,Standard_False,Standard_True,"STEP Product Definition Context");
  WS->AddNamedItem ("step-context-edit",edctx);
  WS->AddNamedItem ("step-context",edctxf);


  Handle(STEPEdit_EditSDR) edsdr = new STEPEdit_EditSDR;
  Handle(IFSelect_EditForm) edsdrf = new IFSelect_EditForm (edsdr,Standard_False,Standard_True,"STEP Product Data (SDR)");
  WS->AddNamedItem ("step-SDR-edit",edsdr);
  WS->AddNamedItem ("step-SDR-data",edsdrf);

  
  
}
