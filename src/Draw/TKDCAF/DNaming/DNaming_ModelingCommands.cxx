// Created on: 2010-01-15
// Created by: Sergey ZARITCHNY
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DNaming.hxx>
#include <DBRep.hxx>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <Message.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <TCollection_AsciiString.hxx>

#include <DDF.hxx>
#include <DDocStd.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Reference.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TFunction_Function.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TFunction_Logbook.hxx>
#include <TFunction_DriverTable.hxx>
#include <TNaming_Selector.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_Name.hxx>
#include <TDocStd_Document.hxx>

#include <DNaming_BoxDriver.hxx>
#include <DNaming_CylinderDriver.hxx>
#include <DNaming_SelectionDriver.hxx>
#include <DNaming_BooleanOperationDriver.hxx>
#include <DNaming_FilletDriver.hxx>
#include <DNaming_TransformationDriver.hxx>
#include <DNaming_PrismDriver.hxx>
#include <DNaming_RevolutionDriver.hxx>
#include <DNaming_SphereDriver.hxx>
#include <DNaming_PointDriver.hxx>
#include <DNaming_Line3DDriver.hxx>
#ifdef _WIN32
  #define EXCEPTION ...
#else
  #define EXCEPTION Standard_Failure const&
#endif
#include <ModelDefinitions.hxx>

// #define DEBUG
//=======================================================================
static occ::handle<TDataStd_UAttribute> AddObject(const occ::handle<TDocStd_Document>& aDoc)
{
  occ::handle<TDataStd_TreeNode> aRootNode = TDataStd_TreeNode::Set(aDoc->Main());
  const TDF_Label&               aLabel    = TDF_TagSource::NewChild(aDoc->Main());

  occ::handle<TDataStd_UAttribute> anObj = TDataStd_UAttribute::Set(aLabel, GEOMOBJECT_GUID);
  occ::handle<TDataStd_TreeNode>   aNode = TDataStd_TreeNode::Set(aLabel);
  aRootNode->Append(aNode);
  return anObj;
}

//=======================================================================
// function : DNaming_AddObject
// purpose  : AddObject D [Name]
//           - adds new object (UAttribute) under main label
//=======================================================================
static int DNaming_AddObject(Draw_Interpretor& di, int nb, const char** a)
{
  if (nb > 1)
  {
    occ::handle<TDocStd_Document> aDoc;
    if (!DDocStd::GetDocument(a[1], aDoc))
      return 1;
    occ::handle<TDataStd_UAttribute> anObj = AddObject(aDoc);
    if (!anObj.IsNull())
    {
      if (nb == 3)
        TDataStd_Name::Set(anObj->Label(), TCollection_ExtendedString(a[2], true));
      DDF::ReturnLabel(di, anObj->Label());
      return 0;
    }
  }
  di << "DNaming_AddObject: Error\n";
  return 1;
}

#include <NCollection_DataMap.hxx>
typedef NCollection_DataMap<TCollection_AsciiString, Standard_GUID> DataMapOfAStringGUID;
static bool                                                         isBuilt(false);
static DataMapOfAStringGUID                                         aDMap;

//=======================================================================
static bool GetFuncGUID(const char* aKey, Standard_GUID& GUID)
{
  bool aRes(false);

  if (!isBuilt)
  {
    aDMap.Bind("PntXYZ", PNTXYZ_GUID);
    aDMap.Bind("PntRLT", PNTRLT_GUID);
    aDMap.Bind("Line3D", LINE3D_GUID);
    aDMap.Bind("Box", BOX_GUID);
    aDMap.Bind("Sph", SPH_GUID);
    aDMap.Bind("Cyl", CYL_GUID);
    aDMap.Bind("Cut", CUT_GUID);
    aDMap.Bind("Fuse", FUSE_GUID);
    aDMap.Bind("Comm", COMMON_GUID);
    aDMap.Bind("Prism", PRISM_GUID);
    aDMap.Bind("FulRevol", FULREVOL_GUID);
    aDMap.Bind("SecRevol", SECREVOL_GUID);
    aDMap.Bind("PMirr", PMIRR_GUID);
    aDMap.Bind("PTxyz", PTXYZ_GUID);
    aDMap.Bind("PTALine", PTALINE_GUID);
    aDMap.Bind("PRLine", PRRLINE_GUID);
    aDMap.Bind("Fillet", FILLT_GUID);
    aDMap.Bind("Attach", ATTCH_GUID);
    aDMap.Bind("XAttach", XTTCH_GUID);
    aDMap.Bind("Section", SECTION_GUID);
    isBuilt = true;
  }

  if (aDMap.IsBound(aKey))
  {
    GUID = aDMap.Find(aKey);
    aRes = true;
  }
  return aRes;
}

//=======================================================================
static occ::handle<TFunction_Driver> GetDriver(const TCollection_AsciiString& name)
{
  occ::handle<TFunction_Driver> aDrv;
  if (name == "Box")
    aDrv = new DNaming_BoxDriver();
  else if (name == "Cyl")
    aDrv = new DNaming_CylinderDriver();
  else if (name == "Sph")
    aDrv = new DNaming_SphereDriver();
  else if (name == "Cut")
    aDrv = new DNaming_BooleanOperationDriver();
  else if (name == "Fuse")
    aDrv = new DNaming_BooleanOperationDriver();
  else if (name == "Comm")
    aDrv = new DNaming_BooleanOperationDriver();
  else if (name == "Prism")
    aDrv = new DNaming_PrismDriver();
  else if (name == "FulRevol")
    aDrv = new DNaming_RevolutionDriver();
  else if (name == "SecRevol")
    aDrv = new DNaming_RevolutionDriver();
  else if (name == "PTxyz")
    aDrv = new DNaming_TransformationDriver();
  else if (name == "PTALine")
    aDrv = new DNaming_TransformationDriver();
  else if (name == "PRLine")
    aDrv = new DNaming_TransformationDriver();
  else if (name == "PMirr")
    aDrv = new DNaming_TransformationDriver();
  else if (name == "Fillet")
    aDrv = new DNaming_FilletDriver();
  else if (name == "Attach")
    aDrv = new DNaming_SelectionDriver();
  else if (name == "XAttach")
    aDrv = new DNaming_SelectionDriver();
  else if (name == "PntXYZ")
    aDrv = new DNaming_PointDriver();
  else if (name == "PntRLT")
    aDrv = new DNaming_PointDriver();
  else if (name == "Line3D")
    aDrv = new DNaming_Line3DDriver();
  else if (name == "Section")
    aDrv = new DNaming_BooleanOperationDriver();
  else
    std::cout << "the specified driver is not supported" << std::endl;
  return aDrv;
}

//=======================================================================
// function : DNaming_AddDriver
// purpose  : "AddDriver Doc Name1 Name2 ..."
//=======================================================================
static int DNaming_AddDriver(Draw_Interpretor& /*theDI*/, int theNb, const char** theArg)
{
  if (theNb >= 3)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    occ::handle<TFunction_DriverTable> aFunctionDrvTable = TFunction_DriverTable::Get();
    for (int i = 2; i < theNb; i++)
    {
      Standard_GUID drvGUID;
      if (!GetFuncGUID(theArg[i], drvGUID))
        continue;
      aFunctionDrvTable->AddDriver(drvGUID, GetDriver(theArg[i]));
#ifdef OCCT_DEBUG
      std::cout << "DNaming_AddDriver : " << theArg[i] << " driver is added" << std::endl;
#endif
    }
    return 0;
  }
#ifdef OCCT_DEBUG
  std::cout << "DNaming_AddDriver : Error" << std::endl;
#endif
  return 1;
}

//=======================================================================
static occ::handle<TFunction_Function> SetFunctionDS(const TDF_Label&     objLabel,
                                                     const Standard_GUID& funGUID)
{
  // set function
  const TDF_Label&                aLabel = TDF_TagSource::NewChild(objLabel);
  occ::handle<TFunction_Function> aFun   = TFunction_Function::Set(aLabel, funGUID);

  occ::handle<TDataStd_TreeNode> aNode = TDataStd_TreeNode::Set(aLabel);
  occ::handle<TDataStd_TreeNode> objNode;
  objLabel.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), objNode);
  if (!objNode.IsNull())
    objNode->Append(aNode);

  // set function data sub-structure
  const TDF_Label&               aLab1  = TDF_TagSource::NewChild(aLabel);
  occ::handle<TDataStd_TreeNode> aNode1 = TDataStd_TreeNode::Set(aLab1);
  TDataStd_Name::Set(aLab1, "Arguments");
  if (!aNode.IsNull())
    aNode->Append(aNode1);

  const TDF_Label&               aLab2  = TDF_TagSource::NewChild(aLabel);
  occ::handle<TDataStd_TreeNode> aNode2 = TDataStd_TreeNode::Set(aLab2);
  TDataStd_Name::Set(aLab2, "Result");
  if (!aNode.IsNull())
    aNode->Append(aNode2);
  return aFun;
}

//=======================================================================
// function : DNaming_AddFunction
// purpose  : AddFunction D ObjEntry FunNane
//         :  - adds new function specified by FunName to an object
//         :  specified by ObjEntry
//=======================================================================
static int DNaming_AddFunction(Draw_Interpretor& di, int nb, const char** a)
{
  if (nb == 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    if (!DDocStd::GetDocument(a[1], aDoc))
      return 1;
    TDF_Label objLabel;
    if (!DDF::FindLabel(aDoc->GetData(), a[2], objLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj;
    if (!objLabel.FindAttribute(GEOMOBJECT_GUID, anObj))
    {
      di << "DNaming_AddFunction: Object is not found at the specified label: " << a[2] << "\n";
      return 1;
    }
    Standard_GUID funGUID;
    if (!GetFuncGUID(a[3], funGUID))
    {
      di << "DNaming_AddFunction: the specified function is not supported: " << a[3] << "\n";
      return 1;
    }
    occ::handle<TFunction_Function> aFun = SetFunctionDS(objLabel, funGUID);
    if (!aFun.IsNull())
    {
      TCollection_AsciiString aFName = TCollection_AsciiString(a[3]) + "_Function";
      TDataStd_Name::Set(aFun->Label(), aFName);
      // clang-format off
      TDF_Reference::Set(objLabel, aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
      // clang-format on
      DDF::ReturnLabel(di, aFun->Label());
      return 0;
    }
  }
  di << "DNaming_AddObject: Error\n";
  return 1;
}

//=======================================================================
// function : DNaming_AddBox
// purpose  : "AddBox Doc dx dy dz"
//=======================================================================
static int DNaming_AddBox(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb >= 4)
  {

    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;
    occ::handle<TDataStd_UAttribute> anObj = AddObject(aDocument);
    if (anObj.IsNull())
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Box", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "Box_Function");

    // clang-format off
    TDF_Reference::Set(anObj->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
    // clang-format on
    double dx, dy, dz;
    dx = Draw::Atof(theArg[2]);
    dy = Draw::Atof(theArg[3]);
    dz = Draw::Atof(theArg[4]);

    DNaming::GetReal(aFun, BOX_DX)->Set(dx);
    DNaming::GetReal(aFun, BOX_DY)->Set(dy);
    DNaming::GetReal(aFun, BOX_DZ)->Set(dz);

    DDF::ReturnLabel(theDI, anObj->Label());
    return 0;
  }
  Message::SendFail() << "DNaming_AddBox : Error";
  return 1;
}

//=======================================================================
static occ::handle<TFunction_Function> GetFunction(const TDF_Label&     objLabel,
                                                   const Standard_GUID& funGUID)
{
  occ::handle<TFunction_Function> aFun;
  occ::handle<TDataStd_TreeNode>  aNode;
  objLabel.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode);
  if (aNode.IsNull())
    return aFun;
  if (!aNode->HasFirst())
    return aFun;
  else
    aNode = aNode->First();
  while (!aNode.IsNull())
  {
    if (aNode->FindAttribute(TFunction_Function::GetID(), aFun))
    {
      const Standard_GUID& aGUID = aFun->GetDriverGUID();
      if (aGUID == funGUID)
        break;
      else
        aFun.Nullify();
    }
    aNode = aNode->Next();
  }
  return aFun;
}

//=======================================================================
// function : DNaming_BoxDX
// purpose  : "BoxDX Doc BoxLabel NewDX"
//=======================================================================

static int DNaming_BoxDX(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label objLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], objLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj;
    if (!objLabel.FindAttribute(GEOMOBJECT_GUID, anObj))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Box", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = GetFunction(objLabel, funGUID);
    if (!aFun.IsNull())
    {
      double value = Draw::Atof(theArg[3]);
      DNaming::GetReal(aFun, BOX_DX)->Set(value);
      DDF::ReturnLabel(theDI, DNaming::GetReal(aFun, BOX_DX)->Label());
      return 0;
    }
  }
  Message::SendFail() << "DNaming_BoxDX : Error";
  return 1;
}

//=======================================================================
// function : DNaming_BoxDY
// purpose  : "BoxDY Doc BoxLabel NewDY"
//=======================================================================

static int DNaming_BoxDY(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label objLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], objLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj;
    if (!objLabel.FindAttribute(GEOMOBJECT_GUID, anObj))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Box", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = GetFunction(objLabel, funGUID);
    if (!aFun.IsNull())
    {
      double value = Draw::Atof(theArg[3]);
      DNaming::GetReal(aFun, BOX_DY)->Set(value);
      DDF::ReturnLabel(theDI, DNaming::GetReal(aFun, BOX_DY)->Label());
      return 0;
    }
  }
  Message::SendFail() << "DNaming_BoxDY : Error";
  return 1;
}

//=======================================================================
// function : DNaming_BoxDZ
// purpose  : "BoxDZ Doc BoxLabel NewDZ"
//=======================================================================

static int DNaming_BoxDZ(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label objLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], objLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj;
    if (!objLabel.FindAttribute(GEOMOBJECT_GUID, anObj))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Box", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = GetFunction(objLabel, funGUID);
    if (!aFun.IsNull())
    {
      double value = Draw::Atof(theArg[3]);
      DNaming::GetReal(aFun, BOX_DZ)->Set(value);
      DDF::ReturnLabel(theDI, DNaming::GetReal(aFun, BOX_DZ)->Label());
      return 0;
    }
  }
  Message::SendFail() << "DNaming_BoxDZ : Error";
  return 1;
}

//=======================================================================
// function : Compute
// purpose  : Performs the calling to a driver with the given Function ID.
//=======================================================================
static int ComputeFunction(const occ::handle<TFunction_Function>& theFun,
                           occ::handle<TFunction_Logbook>&        theLog)
{
  occ::handle<TFunction_DriverTable> aTable = TFunction_DriverTable::Get();
  occ::handle<TFunction_Driver>      aDriver;
  int                                aRes(-1);
  if (aTable->FindDriver(theFun->GetDriverGUID(), aDriver))
  {
    aDriver->Init(theFun->Label());
    aRes = aDriver->Execute(theLog);
  }
  else
    aRes = 1;
  return aRes;
}

//=======================================================================
// function : DNaming_SolveFlatFrom
// purpose  : Performs sequential recomputation of all Aux Object starting from the specified.
//         : "SolveFlatFrom Doc FistAuxObjLabel"
//=======================================================================

static int DNaming_SolveFlatFrom(Draw_Interpretor& /*theDI*/, int theNb, const char** theArg)
{
  if (theNb == 3)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label ObjLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], ObjLabel))
      return 1;
    const TDF_Label& FatherLab = ObjLabel.Father();
    if (FatherLab.IsNull())
      goto ERR;
    TCollection_AsciiString entry;
    TDF_Tool::Entry(FatherLab, entry);
#ifdef OCCT_DEBUG
    std::cout << "DNaming_SolveFlatFrom: Father label = " << entry << std::endl;
#endif
    occ::handle<TFunction_Logbook> logbook = TFunction_Logbook::Set(FatherLab);
    bool                           found(false);
    TDF_ChildIterator              it(FatherLab, false);
    for (; it.More(); it.Next())
    {
      const TDF_Label& aLabel = it.Value();
      if (!found)
      {
        if (aLabel == ObjLabel)
        {
          found = true;
        }
        else
          continue;
      }
      const TDF_Label&                funLabel = aLabel.FindChild(FUNCTION_ARGUMENTS_LABEL, true);
      occ::handle<TFunction_Function> aFun;
      funLabel.FindAttribute(TFunction_Function::GetID(), aFun);
      if (aFun.IsNull())
      {
        std::cout << "DNaming_SolveFlatFrom:: Null function is found!" << std::endl;
        continue;
      }
      else
      {
        TDF_Tool::Entry(funLabel, entry);
        try
        {
          // We clear the logbook because the execution starts not from the beginning of the
          // function list (some functions ar skipped).
          logbook->Clear();
          int aRes = ComputeFunction(aFun, logbook);
          if (aRes != 0)
          {
            Message::SendFail() << "DNaming_SolveFlatFrom: Driver failed at label = " << entry;
            return 1;
          }
#ifdef OCCT_DEBUG
          std::cout << "DNaming_SolveFlatFrom : function from label " << entry << " is recomputed"
                    << std::endl;
#endif
        }
        catch (EXCEPTION)
        {
          std::cout << "DNaming_SolveFlatFrom : Exception computing function at label " << entry
                    << std::endl;
        }
      }
    }
    return 0;
  }
ERR:
  Message::SendFail() << "DNaming_SolveFlatFrom : Error";
  return 1;
}

//=======================================================================
// function : DNaming_InitLogBook
// purpose  : "InitLogBook Doc "
//=======================================================================
static int DNaming_InitLogBook(Draw_Interpretor& /*theDI*/, int theNb, const char** theArg)
{
  if (theNb == 2)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    occ::handle<TFunction_Logbook> logbook = TFunction_Logbook::Set(aDoc->Main());
    if (logbook->IsEmpty())
    {
#ifdef OCCT_DEBUG
      std::cout << "DNaming_InitLogBook : is empty" << std::endl;
#endif
    }
    else
    {
      logbook->Clear();
#ifdef OCCT_DEBUG
      std::cout << "DNaming_InitLogBook : cleaned" << std::endl;
#endif
    }
    return 0;
  }
  Message::SendFail() << "DNaming_InitLogBook : Error - No document ==> " << theNb;
  return 1;
}

//=======================================================================
// function : DNaming_CheckLogBook
// purpose  : "CheckLogBook Doc"
//=======================================================================
static int DNaming_CheckLogBook(Draw_Interpretor& /*theDI*/, int theNb, const char** theArg)
{
  if (theNb == 2)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    occ::handle<TFunction_Logbook> logbook = TFunction_Logbook::Set(aDoc->Main());
    if (logbook->IsEmpty())
      std::cout << "DNaming_CheckLogBook : is empty" << std::endl;
    else
    {
      const NCollection_Map<TDF_Label>&    aMap = logbook->GetValid();
      NCollection_Map<TDF_Label>::Iterator it(aMap);
      TCollection_AsciiString              entry;
      std::cout << "DNaming_CheckLogBook : LogBook current state:" << std::endl;
      for (; it.More(); it.Next())
      {
        TDF_Tool::Entry(it.Key(), entry);
        std::cout << entry << std::endl;
      }
    }
    return 0;
  }
  Message::SendFail() << "DNaming_CheckLogBook : Error - No document ==> " << theNb;
  return 1;
}

//=======================================================================
// function : DNaming_ComputeFun
// purpose  : "ComputeFun Doc FunctionLabel"
//=======================================================================
static int DNaming_ComputeFun(Draw_Interpretor& /*theDI*/, int theNb, const char** theArg)
{
  if (theNb == 3)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label funLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], funLabel))
      return 1;

    occ::handle<TFunction_Function> aFun;
    funLabel.FindAttribute(TFunction_Function::GetID(), aFun);
    if (aFun.IsNull())
      return 1;
    if (!aFun.IsNull())
    {
      occ::handle<TFunction_Logbook> logbook = TFunction_Logbook::Set(funLabel);
      int                            aRes    = ComputeFunction(aFun, logbook);
      if (aRes != 0)
      {
        Message::SendFail() << "DNaming_ComputeFun : No Driver or Driver failed";
        return 1;
      }
#ifdef OCCT_DEBUG
      std::cout << "DNaming_ComputeFun : function from label " << theArg[2] << " is recomputed"
                << std::endl;
#endif
      return 0;
    }
  }
  Message::SendFail() << "DNaming_ComputeFun : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AttachShape
// purpose  : "AttachShape Doc Shape Context [Container [KeepOrientation [Geometry]]]"
//=======================================================================
static int DNaming_AttachShape(Draw_Interpretor& di, int nb, const char** a)
{
  if (nb >= 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(a[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    const char*         aSS(a[2]);
    const TopoDS_Shape& aShape = DBRep::Get(aSS); // shape to be attached
    if (aShape.IsNull())
      return 1;

    occ::handle<TDataStd_UAttribute> aContainer, aContext;
    if (!DDocStd::Find(aDoc, a[3], GEOMOBJECT_GUID, aContext))
      return 1;
    if (nb > 4)
      DDocStd::Find(aDoc, a[4], GEOMOBJECT_GUID, aContainer);

    if (aContainer.IsNull())
      aContainer = aContext;

    occ::handle<TDataStd_UAttribute> anObj = AddObject(aDoc);
    if (!anObj.IsNull())
    {
      occ::handle<TDataStd_TreeNode> aNode, RNode;
      anObj->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode);
      if (aNode.IsNull())
        aNode = TDataStd_TreeNode::Set(anObj->Label());
      aNode->Remove();
      if (aContainer->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), RNode))
        RNode->Append(aNode);
      TDataStd_Name::Set(anObj->Label(), "Auxiliary_Object");
      Standard_GUID funGUID;
      if (GetFuncGUID("Attach", funGUID))
      {
        occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
        if (!aFun.IsNull())
        {
          TDataStd_Name::Set(aFun->Label(), "ISelection");
          TDF_Label aResultLabel = aFun->Label().FindChild(FUNCTION_RESULT_LABEL, true);
          TDF_Reference::Set(anObj->Label(), aResultLabel); // result of the object
          aResultLabel.ForgetAllAttributes(true);
          bool aKeepOrientation(false);
          if (nb >= 6)
            aKeepOrientation = Draw::Atoi(a[5]) != 0;
          bool aGeometry(false);
          if (nb == 7)
            aGeometry = Draw::Atoi(a[6]) != 0;
          occ::handle<TNaming_NamedShape> aCont = DNaming::GetObjectValue(aContext);
#ifdef OCCT_DEBUG
          if (aCont.IsNull() || aCont->IsEmpty())
            std::cout << "Wrong Context ..." << std::endl;
#endif
          try
          {
            TopoDS_Shape     aCONTEXT = aCont->Get();
            TNaming_Selector aSelector(aResultLabel);
            if (!aSelector.Select(aShape, aCONTEXT, aGeometry, aKeepOrientation))
              return 1;
          }
          catch (Standard_Failure const&)
          {
            std::cout << "EXCEPTION: SELECTION_IMPOSSIBLE" << std::endl;
          }

          if (!aCont.IsNull())
          {
#ifdef OCCT_DEBUG
            TCollection_AsciiString entry;
            TDF_Tool::Entry(aCont->Label(), entry);
            std::cout << "ContextNS Label = " << entry << std::endl;
#endif
            occ::handle<TFunction_Function> aCntFun;
            if (aCont->Label().Father().FindAttribute(TFunction_Function::GetID(), aCntFun))
            { // Fun:2 ==> result
              // First argument of Selection function refers to father function (of context object)
              // to which selection is attached (because sel obj. itself already has reference to
              // result NS
              TDF_Reference::Set(
                aFun->Label().FindChild(FUNCTION_ARGUMENTS_LABEL).FindChild(ATTACH_ARG),
                aCntFun->Label()); // ref to function produced Context shape
            }
          }

          DDF::ReturnLabel(di, anObj->Label());
          return 0;
        }
      }
    } // ###
  }
  Message::SendFail() << "DNaming_AttachShape : Error";
  return 1;
}

//=======================================================================
// function : DNaming_XAttachShape
// purpose  : "AttachShape Doc Shape Context [KeepOrientation [Geometry]]"
//=======================================================================
static int DNaming_XAttachShape(Draw_Interpretor& di, int nb, const char** a)
{
  if (nb >= 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(a[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    const char*         aSS(a[2]);
    const TopoDS_Shape& aShape = DBRep::Get(aSS);
    if (aShape.IsNull())
      return 1;

    occ::handle<TDataStd_UAttribute> aContext;
    if (!DDocStd::Find(aDoc, a[3], GEOMOBJECT_GUID, aContext))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj = AddObject(aDoc);
    if (!anObj.IsNull())
    {
      TDataStd_Name::Set(anObj->Label(), "Auxiliary_Object");
      Standard_GUID funGUID;
      if (GetFuncGUID("XAttach", funGUID))
      {
        occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
        if (!aFun.IsNull())
        {
          TDataStd_Name::Set(aFun->Label(), "XSelection");
          TDF_Label aResultLabel = aFun->Label().FindChild(FUNCTION_RESULT_LABEL, true);
          TDF_Reference::Set(anObj->Label(), aResultLabel); // result
          aResultLabel.ForgetAllAttributes(true);
          bool aKeepOrientation(false);
          if (nb >= 5)
            aKeepOrientation = Draw::Atoi(a[4]) != 0;
          bool aGeometry(false);
          if (nb == 6)
            aGeometry = Draw::Atoi(a[5]) != 0;
          occ::handle<TNaming_NamedShape> aCont = DNaming::GetObjectValue(aContext);

          if (aCont.IsNull() || aCont->IsEmpty())
            std::cout << "Wrong Context ..." << std::endl;
          else
          {
            TopoDS_Shape aCONTEXT = aCont->Get();
            try
            {
              TNaming_Selector aSelector(aResultLabel);
              if (!aSelector.Select(aShape, aCONTEXT, aGeometry, aKeepOrientation))
                return 1;
            }
            catch (Standard_Failure const&)
            {
              std::cout << "EXCEPTION: SELECTION_IMPOSSIBLE" << std::endl;
            }

            TDF_Reference::Set(
              aFun->Label().FindChild(FUNCTION_ARGUMENTS_LABEL).FindChild(ATTACH_ARG),
              aContext->Label()); // ref to Context object

            DDF::ReturnLabel(di, anObj->Label());
            return 0;
          }
        }
      }
    }
  }
  Message::SendFail() << "DNaming_XAttachShape : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddCyl
// purpose  : "AddCyl Doc Radius Height Axis
//=======================================================================
static int DNaming_AddCylinder(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 5)
  {

    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;
    occ::handle<TDataStd_UAttribute> anObj = AddObject(aDocument);
    if (anObj.IsNull())
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Cyl", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "Cyl_Function");

    // clang-format off
    TDF_Reference::Set(anObj->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
    // clang-format on

    double aR, aH;
    aR = Draw::Atof(theArg[2]);
    aH = Draw::Atof(theArg[3]);

    occ::handle<TDataStd_UAttribute> Axis;
    if (!DDocStd::Find(aDocument, theArg[4], GEOMOBJECT_GUID, Axis))
      return 1;
    DNaming::GetReal(aFun, CYL_RADIUS)->Set(aR);
    DNaming::GetReal(aFun, CYL_HEIGHT)->Set(aH);
    DNaming::SetObjectArg(aFun, CYL_AXIS, Axis);

    DDF::ReturnLabel(theDI, anObj->Label());
    return 0;
  }
  Message::SendFail() << "DNaming_AddCylinder : Error";
  return 1;
}

//=======================================================================
// function : DNaming_CylRad
// purpose  : "CylRad Doc CylLabel NewRad"
//=======================================================================

static int DNaming_CylRad(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label objLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], objLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj;
    if (!objLabel.FindAttribute(GEOMOBJECT_GUID, anObj))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Cyl", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = GetFunction(objLabel, funGUID);
    if (!aFun.IsNull())
    {
      double value = Draw::Atof(theArg[3]);
      DNaming::GetReal(aFun, CYL_RADIUS)->Set(value);
      DDF::ReturnLabel(theDI, DNaming::GetReal(aFun, CYL_RADIUS)->Label());
      return 0;
    }
  }
  Message::SendFail() << "DNaming_CylRadius : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddFuse
// purpose  : "AddFuse Doc Object Tool"
//=======================================================================

static int DNaming_AddFuse(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {

    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;

    occ::handle<TDataStd_UAttribute> anObject, aTool;
    if (!DDocStd::Find(aDocument, theArg[2], GEOMOBJECT_GUID, anObject))
      return 1;
    if (!DDocStd::Find(aDocument, theArg[3], GEOMOBJECT_GUID, aTool))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Fuse", funGUID))
      return 1;
    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObject->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "Fuse");

    // clang-format off
    TDF_Reference::Set(anObject->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
    // clang-format on
    DNaming::SetObjectArg(aFun, BOOL_TOOL, aTool);
    DDF::ReturnLabel(theDI, aFun->Label());
    return 0;
  }
  Message::SendFail() << "DModel_AddFuse : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddCut
// purpose  : "AddCut Doc Object Tool"
//=======================================================================

static int DNaming_AddCut(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {

    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;

    occ::handle<TDataStd_UAttribute> anObject, aTool;
    if (!DDocStd::Find(aDocument, theArg[2], GEOMOBJECT_GUID, anObject))
      return 1;
    if (!DDocStd::Find(aDocument, theArg[3], GEOMOBJECT_GUID, aTool))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Cut", funGUID))
      return 1;
    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObject->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "Cut");

    // clang-format off
    TDF_Reference::Set(anObject->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
    // clang-format on
    DNaming::SetObjectArg(aFun, BOOL_TOOL, aTool);
    DDF::ReturnLabel(theDI, aFun->Label());
    return 0;
  }
  Message::SendFail() << "DModel_AddCut : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddCommon
// purpose  : "AddCommon Doc Object Tool"
//=======================================================================

static int DNaming_AddCommon(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {

    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;

    occ::handle<TDataStd_UAttribute> anObject, aTool;
    if (!DDocStd::Find(aDocument, theArg[2], GEOMOBJECT_GUID, anObject))
      return 1;
    if (!DDocStd::Find(aDocument, theArg[3], GEOMOBJECT_GUID, aTool))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Comm", funGUID))
      return 1;
    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObject->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "Common");

    // clang-format off
    TDF_Reference::Set(anObject->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
    // clang-format on
    DNaming::SetObjectArg(aFun, BOOL_TOOL, aTool);
    DDF::ReturnLabel(theDI, aFun->Label());
    return 0;
  }
  Message::SendFail() << "DModel_AddComm : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddSection
// purpose  : "AddSection Doc Object Tool"
//=======================================================================

static int DNaming_AddSection(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {

    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;

    occ::handle<TDataStd_UAttribute> anObject, aTool;
    if (!DDocStd::Find(aDocument, theArg[2], GEOMOBJECT_GUID, anObject))
      return 1;
    if (!DDocStd::Find(aDocument, theArg[3], GEOMOBJECT_GUID, aTool))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Section", funGUID))
      return 1;
    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObject->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "Section");

    // clang-format off
    TDF_Reference::Set(anObject->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
    // clang-format on
    DNaming::SetObjectArg(aFun, BOOL_TOOL, aTool);
    DDF::ReturnLabel(theDI, aFun->Label());
    return 0;
  }
  Message::SendFail() << "DModel_AddSection : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddFillet
// purpose  : "AddFillet Doc Object Radius Path "
//=======================================================================
static int DNaming_AddFillet(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb < 5)
  {
    Message::SendFail() << "DNaming_AddFillet(): Wrong number of arguments";
    return 1;
  }

  occ::handle<TDocStd_Document> aDocument;
  const char*                   aDocS(theArg[1]);
  if (!DDocStd::GetDocument(aDocS, aDocument))
    return 1;

  occ::handle<TDataStd_UAttribute> anObject;
  if (!DDocStd::Find(aDocument, theArg[2], GEOMOBJECT_GUID, anObject))
    return 1;
  Standard_GUID funGUID;
  if (!GetFuncGUID("Fillet", funGUID))
    return 1;
  occ::handle<TFunction_Function> aFun = SetFunctionDS(anObject->Label(), funGUID);
  if (aFun.IsNull())
    return 1;
  TDataStd_Name::Set(aFun->Label(), "Fillet");

  // clang-format off
  TDF_Reference::Set(anObject->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
  // clang-format on

  double aRadius = Draw::Atof(theArg[3]);
  DNaming::GetReal(aFun, FILLET_RADIUS)->Set(aRadius);

  occ::handle<TDataStd_UAttribute> aPath;
  if (!DDocStd::Find(aDocument, theArg[4], GEOMOBJECT_GUID, aPath))
    return 1;
  DNaming::SetObjectArg(aFun, FILLET_PATH, aPath);
  DDF::ReturnLabel(theDI, aFun->Label());
  return 0;
}

//=======================================================================
// function : DNaming_TranslatePar
// purpose  : "TranslatePar Doc ShapeEntry dx dy dz"
//=======================================================================
static int DNaming_PTranslateDXYZ(Draw_Interpretor& di, int nb, const char** a)
{
  if (nb > 3)
  {
#ifdef OCCT_DEBUG
    std::cout << "NB = " << nb << std::endl;
#endif
    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(a[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;

    occ::handle<TDataStd_UAttribute> anObject; // aShape;
    if (!DDocStd::Find(aDocument, a[2], GEOMOBJECT_GUID, anObject))
      return 1;

    Standard_GUID funGUID;
    if (!GetFuncGUID("PTxyz", funGUID))
      return 1;
    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObject->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "ParTranslation");

    double aDx = 0., aDy = 0., aDz = 0.;
    aDx = Draw::Atof(a[3]);
    // std::cout << "DX = " << aDx<<std::endl;
    if (nb > 4)
    {
      aDy = Draw::Atof(a[4]);
      // std::cout << "DY = " << aDy<<std::endl;
      if (nb > 5)
      {
        aDz = Draw::Atof(a[5]);
        // std::cout << "DZ = " << aDz<<std::endl;
      }
    }

    DNaming::GetReal(aFun, PTRANSF_DX)->Set(aDx);
    DNaming::GetReal(aFun, PTRANSF_DY)->Set(aDy);
    DNaming::GetReal(aFun, PTRANSF_DZ)->Set(aDz);
    TDF_Reference::Set(anObject->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL));
    DDF::ReturnLabel(di, aFun->Label());
    return 0;
  }
  Message::SendFail() << "DNaming_Translate : Error";
  return 1;
}

//=======================================================================
// function : DNaming_PTranslateLine
// purpose  : "PTranslateAlongLine Doc ShapeEntry Line off"
//=======================================================================
static int DNaming_PTranslateLine(Draw_Interpretor& di, int nb, const char** a)
{
  if (nb > 4)
  {
#ifdef OCCT_DEBUG
    std::cout << "NB = " << nb << std::endl;
#endif
    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(a[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;

    occ::handle<TDataStd_UAttribute> anObject, aLine; // aShape, aLine;
    if (!DDocStd::Find(aDocument, a[2], GEOMOBJECT_GUID, anObject))
      return 1;
    if (!DDocStd::Find(aDocument, a[3], GEOMOBJECT_GUID, aLine))
      return 1;

    Standard_GUID funGUID;
    if (!GetFuncGUID("PTALine", funGUID))
      return 1;
    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObject->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "ParTranslationAlongLine");

    double anOff = 0.;
    anOff        = Draw::Atof(a[4]);
    DNaming::GetReal(aFun, PTRANSF_OFF)->Set(anOff);

    DNaming::SetObjectArg(aFun, PTRANSF_LINE, aLine);
    TDF_Reference::Set(anObject->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL));

    DDF::ReturnLabel(di, aFun->Label());
    return 0;
  }
  Message::SendFail() << "DNaming_PTranslateAlongLine : Error";
  return 1;
}

//=======================================================================
// function : DNaming_PRotateLine
// purpose  : "PRotateRoundLine Doc ShapeEntry Line Angle"
//=======================================================================
static int DNaming_PRotateLine(Draw_Interpretor& di, int nb, const char** a)
{
  if (nb > 4)
  {
    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(a[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;

    occ::handle<TDataStd_UAttribute> anObject, aLine; // aShape, aLine;
    if (!DDocStd::Find(aDocument, a[2], GEOMOBJECT_GUID, anObject))
      return 1;
    if (!DDocStd::Find(aDocument, a[3], GEOMOBJECT_GUID, aLine))
      return 1;

    Standard_GUID funGUID;
    if (!GetFuncGUID("PRLine", funGUID))
      return 1;
    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObject->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "ParRotationAroundLine");

    DNaming::SetObjectArg(aFun, PTRANSF_LINE, aLine);
    TDF_Reference::Set(anObject->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL));

    double anAngle = 0.;
    anAngle        = Draw::Atof(a[4]);
    double aK      = 2 * M_PI / 360;
    anAngle        = anAngle * aK;
    DNaming::GetReal(aFun, PTRANSF_ANG)->Set(anAngle);

    DDF::ReturnLabel(di, aFun->Label());
    return 0;
  }
  Message::SendFail() << "DNaming_PRotateRoundLine : Error";
  return 1;
}

//=======================================================================
// function : DNaming_PMirrorObject
// purpose  : "PMirror Doc ShapeEntry PlaneObj"
//=======================================================================
static int DNaming_PMirrorObject(Draw_Interpretor& di, int nb, const char** a)
{
  if (nb > 3)
  {
    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(a[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;

    occ::handle<TDataStd_UAttribute> anObject, aPlane;
    if (!DDocStd::Find(aDocument, a[2], GEOMOBJECT_GUID, anObject))
      return 1;
    if (!DDocStd::Find(aDocument, a[3], GEOMOBJECT_GUID, aPlane))
      return 1;

    Standard_GUID funGUID;
    if (!GetFuncGUID("PMirr", funGUID))
      return 1;
    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObject->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "ParMirror");

    DNaming::SetObjectArg(aFun, PTRANSF_PLANE, aPlane);
    TDF_Reference::Set(anObject->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL));

    DDF::ReturnLabel(di, aFun->Label());
    return 0;
  }
  Message::SendFail() << "DNaming_PMirrorObject : Error";
  return 1;
}

//=======================================================================
// function : DModel_AddPrism
// purpose  : "AddPrism Doc BasisLabel Height Reverse(0/1)"
//=======================================================================
static int DNaming_AddPrism(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb < 5)
  {
    Message::SendFail() << "DNaming_AddPrism(): Wrong number of arguments";
    return 1;
  }
  //
  occ::handle<TDocStd_Document> aDocument;
  const char*                   aDocS(theArg[1]);
  if (!DDocStd::GetDocument(aDocS, aDocument))
    return 1;

  occ::handle<TDataStd_UAttribute> anObj = AddObject(aDocument);
  if (anObj.IsNull())
    return 1;
  Standard_GUID funGUID;
  if (!GetFuncGUID("Prism", funGUID))
    return 1;
  occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
  if (aFun.IsNull())
    return 1;
  TDataStd_Name::Set(aFun->Label(), "Prism_Function");
  // clang-format off
  TDF_Reference::Set(anObj->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
  // clang-format on

  // arguments
  occ::handle<TDataStd_UAttribute> aBasisObj;
  if (!DDocStd::Find(aDocument, theArg[2], GEOMOBJECT_GUID, aBasisObj))
    return 1;
  DNaming::SetObjectArg(aFun, PRISM_BASIS, aBasisObj);
  double height = Draw::Atof(theArg[3]);
  DNaming::GetReal(aFun, PRISM_HEIGHT)->Set(height);
  int reverse = Draw::Atoi(theArg[4]);
  DNaming::GetInteger(aFun, PRISM_DIR)->Set(reverse);
  DDF::ReturnLabel(theDI, anObj->Label());
  return 0;
}

//=======================================================================
// function : DNaming_PrismHeight
// purpose  : "PrismHeight Doc PrismLabel NewHeight"
//=======================================================================
static int DNaming_PrismHeight(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label objLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], objLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj;
    if (!objLabel.FindAttribute(GEOMOBJECT_GUID, anObj))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Prism", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = GetFunction(objLabel, funGUID);
    if (!aFun.IsNull())
    {
      double value = Draw::Atof(theArg[3]);
      DNaming::GetReal(aFun, PRISM_HEIGHT)->Set(value);
      DDF::ReturnLabel(theDI, DNaming::GetReal(aFun, PRISM_HEIGHT)->Label());
      return 0;
    }
  }
  Message::SendFail() << "DNaming_PrismHeight : Error";
  return 1;
}

//=======================================================================
// function : DModel_AddRevol
// purpose  : "AddRevol Doc BasisLabel  AxisLabel [Angle [Reverse(0/1)]] "
//         : if Angle is presented - sectioned revolution, else - full
//=======================================================================
static int DNaming_AddRevol(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb < 4)
  {
    Message::SendFail() << "DNaming_AddRevol(): Wrong number of arguments";
    return 1;
  }

  occ::handle<TDocStd_Document> aDocument;
  const char*                   aDocS(theArg[1]);
  if (!DDocStd::GetDocument(aDocS, aDocument))
    return 1;

  occ::handle<TDataStd_UAttribute> aBasisObj;
  if (!DDocStd::Find(aDocument, theArg[2], GEOMOBJECT_GUID, aBasisObj))
    return 1;

  occ::handle<TDataStd_UAttribute> anAxObj;
  if (!DDocStd::Find(aDocument, theArg[3], GEOMOBJECT_GUID, anAxObj))
    return 1;

  bool                             aFull = true;
  occ::handle<TDataStd_UAttribute> anObj = AddObject(aDocument);
  if (anObj.IsNull())
    return 1;
  if (theNb > 4)
    aFull = false;
  Standard_GUID funGUID;
  if (aFull)
  {
    if (!GetFuncGUID("FulRevol", funGUID))
      return 1;
  }
  else if (!GetFuncGUID("SecRevol", funGUID))
    return 1;
  occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
  if (aFun.IsNull())
    return 1;
  if (aFull)
    TDataStd_Name::Set(aFun->Label(), "FulRevol_Function");
  else
    TDataStd_Name::Set(aFun->Label(), "SecRevol_Function");
  // clang-format off
  TDF_Reference::Set(anObj->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
  // clang-format on
  DNaming::SetObjectArg(aFun, REVOL_BASIS, aBasisObj);
  DNaming::SetObjectArg(aFun, REVOL_AXIS, anAxObj);

  if (theNb > 4)
  {
    double angle = Draw::Atof(theArg[4]);
    double aK    = 2 * M_PI / 360;
    angle        = angle * aK;
    DNaming::GetReal(aFun, REVOL_ANGLE)->Set(angle);
    if (theNb == 6)
    {
      int reverse = Draw::Atoi(theArg[5]);
      DNaming::GetInteger(aFun, REVOL_REV)->Set(reverse);
    }
  }
  DDF::ReturnLabel(theDI, anObj->Label());
  return 0;
}

//=======================================================================
// function : DNaming_RevolutionAngle
// purpose  : "RevolutionAngle Doc RevolutionLabel NewAngle"
//=======================================================================
static int DNaming_RevolutionAngle(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label objLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], objLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj;
    if (!objLabel.FindAttribute(GEOMOBJECT_GUID, anObj))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("SecRevol", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = GetFunction(objLabel, funGUID);
    if (!aFun.IsNull())
    {
      double value = Draw::Atof(theArg[3]);
      DNaming::GetReal(aFun, REVOL_ANGLE)->Set(value);
      DDF::ReturnLabel(theDI, DNaming::GetReal(aFun, REVOL_ANGLE)->Label());
      return 0;
    }
  }
  Message::SendFail() << "DNaming_RevolutionAngle : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddSphere
// purpose  : "AddSphere Doc CenterLabel Radius "
//=======================================================================
static int DNaming_AddSphere(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb != 4)
  {
    Message::SendFail() << "DNaming_AddSphere(): Wrong number of arguments";
    return 1;
  }
  occ::handle<TDocStd_Document> aDocument;
  const char*                   aDocS(theArg[1]);
  if (!DDocStd::GetDocument(aDocS, aDocument))
    return 1;

  occ::handle<TDataStd_UAttribute> anObj = AddObject(aDocument);
  if (anObj.IsNull())
    return 1;
  Standard_GUID funGUID;
  if (!GetFuncGUID("Sph", funGUID))
    return 1;
  occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
  if (aFun.IsNull())
    return 1;
  TDataStd_Name::Set(aFun->Label(), "Sphere_Function");
  // clang-format off
  TDF_Reference::Set(anObj->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
  // clang-format on

  occ::handle<TDataStd_UAttribute> aCenterObj;
  if (!DDocStd::Find(aDocument, theArg[2], GEOMOBJECT_GUID, aCenterObj))
    return 1;
  DNaming::SetObjectArg(aFun, SPHERE_CENTER, aCenterObj);

  double aRadius = Draw::Atof(theArg[3]);
  DNaming::GetReal(aFun, SPHERE_RADIUS)->Set(aRadius);

  DDF::ReturnLabel(theDI, anObj->Label());
  return 0;
}

//=======================================================================
// function : DNaming_SphereRadius
// purpose  : "SphereRadius Doc SphLabel NewRadius"
//=======================================================================
static int DNaming_SphereRadius(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 4)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label objLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], objLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj;
    if (!objLabel.FindAttribute(GEOMOBJECT_GUID, anObj))
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("Sph", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = GetFunction(objLabel, funGUID);
    if (!aFun.IsNull())
    {
      double value = Draw::Atof(theArg[3]);
      DNaming::GetReal(aFun, SPHERE_RADIUS)->Set(value);
      DDF::ReturnLabel(theDI, DNaming::GetReal(aFun, SPHERE_RADIUS)->Label());
      return 0;
    }
  }

  Message::SendFail() << "DNaming_SphRadius : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddPoint
// purpose  : "AddPoint Doc x y z"
//=======================================================================
static int DNaming_AddPoint(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb >= 4)
  {

    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;
    occ::handle<TDataStd_UAttribute> anObj = AddObject(aDocument);
    if (anObj.IsNull())
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("PntXYZ", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "PntXYZ_Function");

    // clang-format off
    TDF_Reference::Set(anObj->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
    // clang-format on
    double x, y, z;
    x = Draw::Atof(theArg[2]);
    y = Draw::Atof(theArg[3]);
    z = Draw::Atof(theArg[4]);

    DNaming::GetReal(aFun, PNT_DX)->Set(x);
    DNaming::GetReal(aFun, PNT_DY)->Set(y);
    DNaming::GetReal(aFun, PNT_DZ)->Set(z);

    DDF::ReturnLabel(theDI, anObj->Label());
    return 0;
  }

  Message::SendFail() << "DNaming_AddPoint : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddPointRlt
// purpose  : "AddPointRlt Doc RefPnt dx dy dz"
//=======================================================================
static int DNaming_AddPointRlt(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb >= 5)
  {

    occ::handle<TDocStd_Document> aDocument;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDocument))
      return 1;
    occ::handle<TDataStd_UAttribute> anObj = AddObject(aDocument);
    if (anObj.IsNull())
      return 1;
    Standard_GUID funGUID;
    if (!GetFuncGUID("PntRLT", funGUID))
      return 1;

    occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
    if (aFun.IsNull())
      return 1;
    TDataStd_Name::Set(aFun->Label(), "PntRLT_Function");

    // clang-format off
    TDF_Reference::Set(anObj->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
    // clang-format on

    occ::handle<TDataStd_UAttribute> aRefPnt;
    if (!DDocStd::Find(aDocument, theArg[2], GEOMOBJECT_GUID, aRefPnt))
      return 1;

    double dx, dy, dz;
    dx = Draw::Atof(theArg[3]);
    dy = Draw::Atof(theArg[4]);
    dz = Draw::Atof(theArg[5]);

    DNaming::GetReal(aFun, PNT_DX)->Set(dx);
    DNaming::GetReal(aFun, PNT_DY)->Set(dy);
    DNaming::GetReal(aFun, PNT_DZ)->Set(dz);

    DNaming::SetObjectArg(aFun, PNTRLT_REF, aRefPnt);

    DDF::ReturnLabel(theDI, anObj->Label());
    return 0;
  }

  Message::SendFail() << "DNaming_AddPoint : Error";
  return 1;
}

//=======================================================================
// function : DNaming_PntOffset
// purpose  : "PntOffset Doc PntLabel newDX newDY newDZ "
//=======================================================================

static int DNaming_PntOffset(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb == 6)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label objLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], objLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> anObj;
    if (!objLabel.FindAttribute(GEOMOBJECT_GUID, anObj))
      return 1;
    Standard_GUID funGUID;

    if (!GetFuncGUID("PntXYZ", funGUID))
    {
      if (!GetFuncGUID("PntRLT", funGUID))
        return 1;
    }

    occ::handle<TFunction_Function> aFun = GetFunction(objLabel, funGUID);
    if (!aFun.IsNull())
    {
      double value(0.0);
      bool   isDX = strcmp(theArg[3], "skip") != 0;
      if (isDX)
      {
        value = Draw::Atof(theArg[3]);
        DNaming::GetReal(aFun, PNT_DX)->Set(value);
      }
      bool isDY = strcmp(theArg[4], "skip") != 0;
      if (isDY)
      {
        value = Draw::Atof(theArg[4]);
        DNaming::GetReal(aFun, PNT_DY)->Set(value);
      }
      bool isDZ = strcmp(theArg[5], "skip") != 0;
      if (isDZ)
      {
        value = Draw::Atof(theArg[5]);
        DNaming::GetReal(aFun, PNT_DZ)->Set(value);
      }
      if (isDX || isDY || isDZ)
        DDF::ReturnLabel(theDI, objLabel);
      else
        std::cout << "DNaming_PntOffset : Nothing changed" << std::endl;
      return 0;
    }
  }

  Message::SendFail() << "DNaming_PntOffset : Error";
  return 1;
}

//=======================================================================
// function : DNaming_AddLine3D
// purpose  : "AddLine3D Doc CurveType(0|1) Pnt1Lab Pnt2Lab [Pnt3Lab [Pnt4Lab [...]]]"
//         : Type = 0 - open, Type = 1 - closed
//=======================================================================
static int DNaming_Line3D(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb < 5)
  {
    Message::SendFail() << "DNaming_AddLine3D: Wrong number of arguments";
    return 1;
  }
  occ::handle<TDocStd_Document> aDocument;
  const char*                   aDocS(theArg[1]);
  if (!DDocStd::GetDocument(aDocS, aDocument))
    return 1;

  occ::handle<TDataStd_UAttribute> anObj = AddObject(aDocument);
  if (anObj.IsNull())
    return 1;
  Standard_GUID funGUID;
  if (!GetFuncGUID("Line3D", funGUID))
    return 1;
  occ::handle<TFunction_Function> aFun = SetFunctionDS(anObj->Label(), funGUID);
  if (aFun.IsNull())
    return 1;
  TDataStd_Name::Set(aFun->Label(), "Line3D_Function");
  // clang-format off
  TDF_Reference::Set(anObj->Label(), aFun->Label().FindChild(FUNCTION_RESULT_LABEL)); //result is here
  // clang-format on

  int aType = Draw::Atoi(theArg[2]);
  DNaming::GetInteger(aFun, LINE3D_TYPE)->Set(aType);

  // LINE3D_PNTNB
  // set Pnts
  int aPos = LINE3D_TYPE;
  for (int i = 3; i < theNb; i++)
  {
    occ::handle<TDataStd_UAttribute> aPntObj;
    if (!DDocStd::Find(aDocument, theArg[i], GEOMOBJECT_GUID, aPntObj))
      return 1;
    aPos++;
    DNaming::SetObjectArg(aFun, aPos, aPntObj);
  }

  DNaming::GetInteger(aFun, LINE3D_PNTNB)->Set(aPos - 1); // set number of points

  DDF::ReturnLabel(theDI, anObj->Label());
  return 0;
}

// ***
//==========================================================================================
inline static void MapOfOrientedShapes(const TopoDS_Shape& S, NCollection_Map<TopoDS_Shape>& M)
{
  M.Add(S);
  TopoDS_Iterator It(S, true, true);
  while (It.More())
  {
    MapOfOrientedShapes(It.Value(), M);
    It.Next();
  }
}

//==========================================================================================
static void MapOfOrientedShapes(const TopoDS_Shape&            S1,
                                const TopoDS_Shape&            S2,
                                NCollection_Map<TopoDS_Shape>& M)
{

  TopoDS_Iterator it(S2, true, true);
  while (it.More())
  {
    if (it.Value().ShapeType() == S1.ShapeType())
      M.Add(it.Value());
    it.Next();
  }
}

//==========================================================================================
static void MapOfShapes(const TopoDS_Shape&                                     S1,
                        const TopoDS_Shape&                                     S2,
                        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& M)
{

  TopoDS_Iterator it(S2, true, true);
  while (it.More())
  {
    if (it.Value().ShapeType() == S1.ShapeType())
      M.Add(it.Value());
    it.Next();
  }
}

//==========================================================================================
inline static void CollectShapes(const TopoDS_Shape& S, NCollection_List<TopoDS_Shape>& List)
{
  List.Append(S);
  TopoDS_Iterator It(S, true, true);
  while (It.More())
  {
    CollectShapes(It.Value(), List);
    It.Next();
  }
}

//==========================================================================================
inline static void CollectMultShapes(const TopoDS_Shape& S, NCollection_List<TopoDS_Shape>& List)
{
  TopAbs_ShapeEnum aType = S.ShapeType();
  int              aStop(TopAbs_SHAPE);
  for (int i = (int)aType + 1; i < aStop; i++)
  {
    BRep_Builder    aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound(aCompound);
    TopExp_Explorer exp(S, (TopAbs_ShapeEnum)i);
    for (; exp.More(); exp.Next())
      aBuilder.Add(aCompound, exp.Current());
    List.Append(aCompound);
  }
}

//==========================================================================================
static bool MakeSelection(const occ::handle<TDataStd_UAttribute>& Obj,
                          const TopoDS_Shape&                     Selection,
                          const occ::handle<TDataStd_UAttribute>& ContextObj,
                          const bool                              Geometry,
                          const bool                              KeepOrientation)
{
  if (!Obj.IsNull())
  {
    occ::handle<TDataStd_TreeNode> aNode, RNode;
    Obj->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode);
    if (aNode.IsNull())
      aNode = TDataStd_TreeNode::Set(Obj->Label());
    aNode->Remove();
    const occ::handle<TDataStd_UAttribute>& aContainer = ContextObj;
    if (aContainer->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), RNode))
      RNode->Append(aNode);
    TDataStd_Name::Set(Obj->Label(), "Auxiliary_Object");
    Standard_GUID funGUID;
    if (GetFuncGUID("Attach", funGUID))
    {
      occ::handle<TFunction_Function> aFun = SetFunctionDS(Obj->Label(), funGUID);
      if (!aFun.IsNull())
      {
        TDataStd_Name::Set(aFun->Label(), "ISelection");
        TDF_Label aResultLabel = aFun->Label().FindChild(FUNCTION_RESULT_LABEL, true);
        TDF_Reference::Set(Obj->Label(), aResultLabel); // result of the object
        aResultLabel.ForgetAllAttributes(true);
        occ::handle<TNaming_NamedShape> aNS = DNaming::GetObjectValue(ContextObj);
        try
        {
          const TopoDS_Shape& aContext = aNS->Get();
          TNaming_Selector    aSelector(aResultLabel);
          if (!aSelector.Select(Selection, aContext, Geometry, KeepOrientation))
            return false;
        }
        catch (...)
        {
          std::cout << "EXCEPTION: SELECTION_IMPOSSIBLE" << std::endl;
        }

        if (!aNS.IsNull())
        {

          // TCollection_AsciiString entry;
          // TDF_Tool::Entry(aNS->Label(), entry);
          // std::cout << "ContextNS Label = " << entry <<std::endl;
          occ::handle<TFunction_Function> aCntFun;
          if (aNS->Label().Father().FindAttribute(TFunction_Function::GetID(), aCntFun))
          { // Fun:2 ==> result
            // First argument of Selection function refers to father function (of context object)
            // to which selection is attached (because sel obj. itself already has reference to
            // result NS
            TDF_Reference::Set(
              aFun->Label().FindChild(FUNCTION_ARGUMENTS_LABEL).FindChild(ATTACH_ARG),
              aCntFun->Label()); // ref to function produced Context shape
          }
        }
        return true;
      }
    }
  }
  return false;
}

//==========================================================================================
static bool MakeXSelection(const occ::handle<TDataStd_UAttribute>& Obj,
                           const TopoDS_Shape&                     Selection,
                           const occ::handle<TDataStd_UAttribute>& ContextObj,
                           const bool                              Geometry,
                           const bool                              KeepOrientation)
{
  if (!Obj.IsNull())
  {
    /*    occ::handle<TDataStd_TreeNode> aNode,RNode;
        Obj->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode);
        if(aNode.IsNull())
          aNode = TDataStd_TreeNode::Set(Obj->Label());
        aNode->Remove();
        occ::handle<TDataStd_UAttribute> aContainer = ContextObj;
        if(aContainer->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), RNode))
          RNode->Append(aNode);
    */
    TDataStd_Name::Set(Obj->Label(), "Auxiliary_Object");
    Standard_GUID funGUID;
    if (GetFuncGUID("XAttach", funGUID))
    {
      occ::handle<TFunction_Function> aFun = SetFunctionDS(Obj->Label(), funGUID);
      if (!aFun.IsNull())
      {
        TDataStd_Name::Set(aFun->Label(), "XSelection");
        TDF_Label aResultLabel = aFun->Label().FindChild(FUNCTION_RESULT_LABEL, true);
        TDF_Reference::Set(Obj->Label(), aResultLabel); // result of the object
        aResultLabel.ForgetAllAttributes(true);

        occ::handle<TNaming_NamedShape> aNS = DNaming::GetObjectValue(ContextObj);
        try
        {
          const TopoDS_Shape& aContext = aNS->Get();
          TNaming_Selector    aSelector(aResultLabel);
          if (!aSelector.Select(Selection, aContext, Geometry, KeepOrientation))
            return false;
        }
        catch (...)
        {
          std::cout << "EXCEPTION: SELECTION_IMPOSSIBLE" << std::endl;
        }

        if (!aNS.IsNull())
        {

          // TCollection_AsciiString entry;
          // TDF_Tool::Entry(aNS->Label(), entry);
          // std::cout << "ContextNS Label = " << entry <<std::endl;
          occ::handle<TFunction_Function> aCntFun;
          if (aNS->Label().Father().FindAttribute(TFunction_Function::GetID(), aCntFun))
          { // Fun:2 ==> result
            // First argument of Selection function refers to father function (of context object)
            // to which selection is attached (because sel obj. itself already has reference to
            // result NS
            TDF_Reference::Set(
              aFun->Label().FindChild(FUNCTION_ARGUMENTS_LABEL).FindChild(ATTACH_ARG),
              aCntFun->Label()); // ref to function produced Context shape
          }
        }
        return true;
      }
    }
  }
  return false;
}

//=======================================================================
inline static TCollection_ExtendedString compareShapes(const TopoDS_Shape& theShape1,
                                                       const TopoDS_Shape& theShape2,
                                                       const bool          keepOrientation)
{
  TCollection_ExtendedString aResult("");

  if ((theShape1.ShapeType() != TopAbs_COMPOUND) && theShape2.ShapeType() == TopAbs_COMPOUND)
  {
    if (keepOrientation)
    {
      NCollection_Map<TopoDS_Shape> M;
      MapOfOrientedShapes(theShape1, theShape2, M);
      if (M.Contains(theShape1))
      {
        aResult += " Warning => Type migration to Compound";
        return aResult;
      }
    }
    else
    {
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> M;
      MapOfShapes(theShape1, theShape2, M);
      if (M.Contains(theShape1))
      {
        aResult += " Warning => Type migration to Compound";
        return aResult;
      }
    }
  }
  bool isComma = false;
  if (theShape1.ShapeType() != theShape2.ShapeType())
  {
    aResult += " Type was changed:";
    aResult += " Initial type = ";
    aResult += TCollection_ExtendedString(theShape1.ShapeType());
    aResult += "; Resulting type = ";
    aResult += TCollection_ExtendedString(theShape2.ShapeType());
    isComma = true;
  }
  else if (theShape1.TShape() != theShape2.TShape())
  {
    aResult += " TShape was changed";
    isComma = true;
  }
  else if (theShape1.Location() != theShape2.Location())
  {
    aResult += " Location was changed";
    isComma = true;
  }

  if (keepOrientation)
  {
    if (theShape1.Orientation() != theShape2.Orientation()
        && theShape1.ShapeType() != TopAbs_VERTEX)
    {
      if (theShape2.ShapeType() != TopAbs_COMPOUND)
      {
        if (isComma)
          aResult += ",";
        aResult += " Orientation was changed";
      }
      else
      {
        TopoDS_Iterator it(theShape2);
        for (; it.More(); it.Next())
        {
          if (it.Value().IsSame(theShape1))
            if (theShape1.Orientation() != it.Value().Orientation())
            {
              if (isComma)
                aResult += ",";
              aResult += " Orientation was changed";
            }
        }
      }
    }
  }
  return aResult;
}

//=======================================================================
inline static TCollection_ExtendedString compareShapes(const TopoDS_Shape& theShape,
                                                       // theShape - Compound of Multiple selection
                                                       const NCollection_Map<TopoDS_Shape>& aMap)
{
  TCollection_ExtendedString aResult("");
  if (theShape.ShapeType() != TopAbs_COMPOUND)
  {
    aResult += "the specified shape is not COMPOUND";
  }
  else
  {
    TopoDS_Iterator it(theShape);
    for (; it.More(); it.Next())
    {
      const TopoDS_Shape& newShape = it.Value();
      if (!aMap.Contains(newShape))
      {
        aResult += " Not in the context";
      }
    }
  }
  return aResult;
}

//=======================================================================
// function : DNaming_TestSingle
// purpose  : "TestSingleSelection Doc ObjectLabel [Orientation [Xselection [Geometry]]]"
//         : returns DDF::ReturnLabel of a first Aux object
//=======================================================================
static int DNaming_TestSingle(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb >= 3)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label ObjLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], ObjLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> aCntObj;
    if (!ObjLabel.FindAttribute(GEOMOBJECT_GUID, aCntObj))
      return 1;
    bool Orientation(false);
    bool XSelection(false);
    bool Geometry(false);
    if (theNb == 4)
      Orientation = Draw::Atoi(theArg[3]) != 0;
    if (theNb == 5)
      XSelection = Draw::Atoi(theArg[4]) != 0;
    if (theNb == 6)
      Geometry = Draw::Atoi(theArg[5]) != 0;
    occ::handle<TNaming_NamedShape> aNS = DNaming::GetObjectValue(aCntObj);

    if (!aNS.IsNull() && !aNS->IsEmpty())
    {
      const TopoDS_Shape& aRootShape = aNS->Get();
      // NCollection_Map<TopoDS_Shape> aMap0;
      // MapOfOrientedShapes(aRootShape, aMap0);
      NCollection_List<TopoDS_Shape> aList, aFailedList;
      CollectShapes(aRootShape, aList);
      if (aList.Extent())
        aList.RemoveFirst();
      bool                                     isFirst(true);
      occ::handle<TDataStd_UAttribute>         FirstAuxObj;
      NCollection_List<TopoDS_Shape>::Iterator it(aList);
      for (; it.More(); it.Next())
      {
        const TopoDS_Shape& aCurShape = it.Value();
        if (aCurShape.IsNull())
          continue;
        if (aCurShape.ShapeType() == TopAbs_EDGE)
        {
          if (BRep_Tool::Degenerated(TopoDS::Edge(aCurShape)))
            continue;
        }
        occ::handle<TDataStd_UAttribute> auxObj = AddObject(aDoc);
        if (isFirst)
        {
          FirstAuxObj = auxObj;
          isFirst     = false;
          TCollection_AsciiString entry;
          TDF_Tool::Entry(FirstAuxObj->Label(), entry);
#ifdef OCCT_DEBUG
          std::cout << "First Selection function at " << entry << std::endl;
#endif
        }
        bool isSelected(false);
        try
        {
          OCC_CATCH_SIGNALS
          {
            if (!XSelection)
              isSelected = MakeSelection(auxObj, aCurShape, aCntObj, Geometry, Orientation);
            else
              isSelected = MakeXSelection(auxObj, aCurShape, aCntObj, Geometry, Orientation);
          }
        }
        catch (Standard_Failure const& anException)
        {
          std::cout << "%%%INFO:Error: ::TestSingleSelection failed :";
          std::cout << anException.GetMessageString() << std::endl;
        }
        catch (...)
        {
          std::cout
            << "%%%INFO:Error: ::TestSingleSelection selection failed : unknown exception type";
        }
        TCollection_AsciiString entry;
        TDF_Tool::Entry(auxObj->Label(), entry);
        TCollection_ExtendedString aResult("");
        if (isSelected)
        {
          const occ::handle<TNaming_NamedShape>& aSelNS = DNaming::GetObjectValue(auxObj);
          if (!aSelNS.IsNull() && !aSelNS->IsEmpty())
          {
            const TopoDS_Shape& aSelectedShape = aSelNS->Get();
            aResult += compareShapes(aCurShape, aSelectedShape, Orientation);
            TDF_ChildIDIterator itn(auxObj->Label(), TNaming_Naming::GetID(), true);
            for (; itn.More(); itn.Next())
            {
              occ::handle<TNaming_Naming> aNaming = occ::down_cast<TNaming_Naming>(itn.Value());
              if (!aNaming.IsNull())
              {
                const TNaming_Name& aName = aNaming->GetName();
                if (aName.Type() == TNaming_UNKNOWN)
                {
                  aResult += " Selection at label = ";
                  aResult += entry;
                  aResult += " has UNKNOWN name type, shape type = ";
                  aResult += TopAbs::ShapeTypeToString(aCurShape.ShapeType());
                }
              }
            }
          }
        }
        else
        {
          aResult += " Selection at label = ";
          aResult += entry;
          aResult += " failed, shape type = ";
          aResult += TopAbs::ShapeTypeToString(aCurShape.ShapeType());
          aFailedList.Append(aCurShape);
        }
        if (aResult.Length())
        {
          if (aResult.Search("Warning") == -1)
            std::cout << "Failed units: " << aResult << " at " << entry << std::endl;
          else
            std::cout << aResult << " at " << entry << std::endl;
          TDataStd_Name::Set(auxObj->Label(), aResult);
        }
      }
      if (aFailedList.Extent())
      {
        std::cout << "Failed units are kept at: ";
        NCollection_List<TopoDS_Shape>::Iterator it1(aFailedList);
        for (; it1.More(); it1.Next())
        {
          const TDF_Label& aLabel = TDF_TagSource::NewChild(aDoc->Main());

          TNaming_Builder B(aLabel);
          B.Generated(it1.Value());
          TCollection_AsciiString entry;
          TDF_Tool::Entry(aLabel, entry);
          std::cout << "\t" << entry << std::endl;
        }
      }
      if (!FirstAuxObj.IsNull())
        DDF::ReturnLabel(theDI, FirstAuxObj->Label());
      return 0;
    }
  }

  Message::SendFail() << "DNaming_TestSingle : Error";
  return 1;
}

//=======================================================================
// function : DNaming_Multiple
// purpose  : "TestMultipleSelection Doc ObjectLabel [Orientation [Xselection [Geometry]]]"
//         : returns DDF::ReturnLabel of a first Aux object
//=======================================================================
static int DNaming_Multiple(Draw_Interpretor& theDI, int theNb, const char** theArg)
{
  if (theNb >= 3)
  {
    occ::handle<TDocStd_Document> aDoc;
    const char*                   aDocS(theArg[1]);
    if (!DDocStd::GetDocument(aDocS, aDoc))
      return 1;
    TDF_Label ObjLabel;
    if (!DDF::FindLabel(aDoc->GetData(), theArg[2], ObjLabel))
      return 1;

    occ::handle<TDataStd_UAttribute> aCntObj;
    if (!ObjLabel.FindAttribute(GEOMOBJECT_GUID, aCntObj))
      return 1;
    bool Orientation(false);
    bool XSelection(false);
    bool Geometry(false);
    if (theNb == 4)
      Orientation = Draw::Atoi(theArg[3]) != 0;
    if (theNb == 5)
      XSelection = Draw::Atoi(theArg[4]) != 0;
    if (theNb == 6)
      Geometry = Draw::Atoi(theArg[5]) != 0;
    occ::handle<TNaming_NamedShape> aNS = DNaming::GetObjectValue(aCntObj);

    if (!aNS.IsNull() && !aNS->IsEmpty())
    {
      const TopoDS_Shape&           aRootShape = aNS->Get();
      NCollection_Map<TopoDS_Shape> aMap0;
      MapOfOrientedShapes(aRootShape, aMap0);
      NCollection_List<TopoDS_Shape> aList, aFailedList;
      CollectMultShapes(aRootShape, aList);

      bool                                     isFirst(true);
      occ::handle<TDataStd_UAttribute>         FirstAuxObj;
      NCollection_List<TopoDS_Shape>::Iterator it(aList);
      for (; it.More(); it.Next())
      {
        const TopoDS_Shape& aCurShape = it.Value();
        if (aCurShape.IsNull())
          continue;
        if (aCurShape.ShapeType() == TopAbs_EDGE)
        {
          if (BRep_Tool::Degenerated(TopoDS::Edge(aCurShape)))
            continue;
        } //
        occ::handle<TDataStd_UAttribute> auxObj = AddObject(aDoc);
        if (isFirst)
        {
          FirstAuxObj = auxObj;
          isFirst     = false;
#ifdef OCCT_DEBUG
          TCollection_AsciiString entry;
          TDF_Tool::Entry(FirstAuxObj->Label(), entry);
          std::cout << "First Selection function at " << entry << std::endl;
#endif
        }
        bool isSelected(false);
        try
        {
          OCC_CATCH_SIGNALS
          {
            if (!XSelection)
              isSelected = MakeSelection(auxObj, aCurShape, aCntObj, Geometry, Orientation);
            else
              isSelected = MakeXSelection(auxObj, aCurShape, aCntObj, Geometry, Orientation);
          }
        }
        catch (Standard_Failure const& anException)
        {
          std::cout << "%%%INFO:Error: ::TestSingleSelection failed :";
          std::cout << anException.GetMessageString() << std::endl;
        }
        catch (...)
        {
          std::cout
            << "%%%INFO:Error: ::TestSingleSelection selection failed : unknown exception type";
        }
        TCollection_AsciiString entry;
        TDF_Tool::Entry(auxObj->Label(), entry);
        TCollection_ExtendedString aResult("");
        if (isSelected)
        {
          const occ::handle<TNaming_NamedShape>& aSelNS = DNaming::GetObjectValue(auxObj);
          if (!aSelNS.IsNull() && !aSelNS->IsEmpty())
          {
            const TopoDS_Shape& aSelectedShape = aSelNS->Get();
            aResult += compareShapes(aSelectedShape, aMap0);
            TDF_ChildIDIterator itn(auxObj->Label(), TNaming_Naming::GetID(), true);
            for (; itn.More(); itn.Next())
            {
              occ::handle<TNaming_Naming> aNaming = occ::down_cast<TNaming_Naming>(itn.Value());
              if (!aNaming.IsNull())
              {
                const TNaming_Name& aName = aNaming->GetName();
                if (aName.Type() == TNaming_UNKNOWN)
                {
                  aResult += " Selection at label = ";
                  aResult += entry;
                  aResult += " has UNKNOWN name type, shape type = ";
                  aResult += TopAbs::ShapeTypeToString(aCurShape.ShapeType());
                }
              }
            }
          }
        }
        else
        {
          aResult += " Selection at label = ";
          aResult += entry;
          aResult += " failed, shape type = ";
          aResult += TopAbs::ShapeTypeToString(aCurShape.ShapeType());
          aFailedList.Append(aCurShape);
        }
        if (aResult.Length())
          std::cout << "Failed units: " << aResult << std::endl;
      }

      if (aFailedList.Extent())
      {
        NCollection_List<TopoDS_Shape>::Iterator it1(aFailedList);
        for (; it1.More(); it1.Next())
        {
          const TDF_Label& aLabel = TDF_TagSource::NewChild(aDoc->Main());

          TNaming_Builder B(aLabel);
          B.Generated(it1.Value());
        }
      }
      if (!FirstAuxObj.IsNull())
        DDF::ReturnLabel(theDI, FirstAuxObj->Label());
      return 0;
    }
  }

  Message::SendFail() << "DNaming_TestMultiple : Error";
  return 1;
}

//=================================================================================================

void DNaming::ModelingCommands(Draw_Interpretor& theCommands)
{

  static bool done = false;
  if (done)
    return;
  done           = true;
  const char* g2 = "Naming modeling commands";

  theCommands.Add("AddObject", "AddObject D", __FILE__, DNaming_AddObject, g2);

  theCommands.Add(
    "AddFunction",
    "AddFunction D ObjEntry FunName[Box|Sph|Cyl|Cut|Fuse|Prism|Revol|PMove|Fillet|Attach|XAttach]",
    __FILE__,
    DNaming_AddFunction,
    g2);

  theCommands.Add("AddBox", "AddBox Doc dx dy dz", __FILE__, DNaming_AddBox, g2);

  theCommands.Add("BoxDX", "BoxDX Doc BoxLabel NewDX", __FILE__, DNaming_BoxDX, g2);

  theCommands.Add("BoxDY", "BoxDY Doc BoxLabel NewDY", __FILE__, DNaming_BoxDY, g2);

  theCommands.Add("BoxDZ", "BoxDZ Doc BoxLabel NewDZ", __FILE__, DNaming_BoxDZ, g2);

  theCommands.Add("ComputeFun", "ComputeFun Doc FunLabel", __FILE__, DNaming_ComputeFun, g2);

  theCommands.Add("InitLogBook", "InitLogBook Doc", __FILE__, DNaming_InitLogBook, g2);

  theCommands.Add(
    "AddDriver",
    "AddDriver Doc Name "
    "[Box|Sph|Cyl|Cut|Fuse|Prism|Revol|PTxyz|PTALine|PRLine|PMirr|Fillet|Attach|XAttach]",
    __FILE__,
    DNaming_AddDriver,
    g2);

  theCommands.Add("AttachShape",
                  "AttachShape Doc Shape Context [Container [KeepOrientation [Geometry]]]",
                  __FILE__,
                  DNaming_AttachShape,
                  g2);

  theCommands.Add("XAttachShape",
                  "XAttachShape Doc Shape Context [KeepOrientation [Geometry]]",
                  __FILE__,
                  DNaming_XAttachShape,
                  g2);

  theCommands.Add("AddCyl", "AddCyl Doc Radius Height Axis", __FILE__, DNaming_AddCylinder, g2);

  theCommands.Add("CylRad", "CylRad Doc CylLabel NewRad", __FILE__, DNaming_CylRad, g2);

  theCommands.Add("AddFuse", "AddFuse Doc Object Tool", __FILE__, DNaming_AddFuse, g2);

  theCommands.Add("AddCut", "AddCut Doc Object Tool", __FILE__, DNaming_AddCut, g2);

  theCommands.Add("AddCommon", "AddCommon Doc Object Tool", __FILE__, DNaming_AddCommon, g2);

  theCommands.Add("AddSection", "AddSection Doc Object Tool", __FILE__, DNaming_AddSection, g2);

  theCommands.Add(
    "AddFillet",
    "AddFillet Doc Object Radius Path [SurfaceType(0-Rational;1-QuasiAngular;2-Polynomial)]",
    __FILE__,
    DNaming_AddFillet,
    g2);

  theCommands.Add("PTranslateDXYZ",
                  "PTranslateDXYZ Doc ShapeEntry dx dy dz",
                  __FILE__,
                  DNaming_PTranslateDXYZ,
                  g2);

  theCommands.Add("PTranslateAlongLine",
                  "PTranslateAlongLine Doc ShapeEntry  Line off",
                  __FILE__,
                  DNaming_PTranslateLine,
                  g2);

  theCommands.Add("PRotateRoundLine",
                  "PRotateRoundLine Doc ShapeEntry Line Angle",
                  __FILE__,
                  DNaming_PRotateLine,
                  g2);

  theCommands.Add("PMirror",
                  "PMirror Doc ShapeEntry PlaneObj",
                  __FILE__,
                  DNaming_PMirrorObject,
                  g2);

  theCommands.Add("AddPrism",
                  "AddPrism Doc BasisLabel Height Reverse(0/1) ",
                  __FILE__,
                  DNaming_AddPrism,
                  g2);

  theCommands.Add("PrismHeight",
                  "PrismHeight Doc PrismLabel NewHeight",
                  __FILE__,
                  DNaming_PrismHeight,
                  g2);

  theCommands.Add("AddRevol",
                  "AddRevol Doc BasisLabel  AxisLabel [Angle [Reverse(0/1)]] ",
                  __FILE__,
                  DNaming_AddRevol,
                  g2);

  theCommands.Add("RevolutionAngle",
                  "RevolutionAngle Doc RevolutionLabel NewAngle",
                  __FILE__,
                  DNaming_RevolutionAngle,
                  g2);

  theCommands.Add("AddSphere",
                  "AddSphere Doc CenterLabel Radius ",
                  __FILE__,
                  DNaming_AddSphere,
                  g2);

  theCommands.Add("SphereRadius",
                  "SphereRadius Doc SphereLabel NewRadius",
                  __FILE__,
                  DNaming_SphereRadius,
                  g2);

  theCommands.Add("TestSingleSelection",
                  "TestSingleSelection Doc ObjectLabel [Orientation [Xselection [Geometry]]]",
                  __FILE__,
                  DNaming_TestSingle,
                  g2);

  theCommands.Add("SolveFlatFrom",
                  "SolveFlatFrom Doc FistAuxObjLabel",
                  __FILE__,
                  DNaming_SolveFlatFrom,
                  g2);

  theCommands.Add("CheckLogBook", "CheckLogBook Doc", __FILE__, DNaming_CheckLogBook, g2);

  theCommands.Add("TestMultipleSelection",
                  "TestMultipleSelection Doc ObjectLabel [Orientation [Xselection [Geometry]]]",
                  __FILE__,
                  DNaming_Multiple,
                  g2);

  theCommands.Add("AddPoint", "AddPoint Doc x y z", __FILE__, DNaming_AddPoint, g2);

  theCommands.Add("AddPointRlt",
                  "AddPointRlt Doc RefPntObj dx dy dz",
                  __FILE__,
                  DNaming_AddPointRlt,
                  g2);

  theCommands.Add("PntOffset",
                  "PntOffset Doc PntLabel newDX|skip newDY|skip newDZ|skip",
                  __FILE__,
                  DNaming_PntOffset,
                  g2);

  theCommands.Add("AddLine3D",
                  "AddLine3D Doc CurveType(0|1) Pnt1 Pnt2 [Pnt3 [Pnt4 [...]]]",
                  __FILE__,
                  DNaming_Line3D,
                  g2);
}
