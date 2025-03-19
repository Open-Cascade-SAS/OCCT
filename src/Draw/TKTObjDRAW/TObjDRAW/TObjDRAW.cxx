// Created on: 2008-06-07
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Tool.hxx>
#include <TObj_Application.hxx>
#include <TObj_Model.hxx>
#include <TObj_Object.hxx>
#include <TObj_OcafObjectIterator.hxx>
#include <TObj_TModel.hxx>
#include <TObj_TNameContainer.hxx>
#include <TObjDRAW.hxx>

#include <BinTObjDrivers.hxx>
#include <XmlTObjDrivers.hxx>
#include <OSD_FileSystem.hxx>

#include <stdio.h>

//=======================================================================
// Section: General commands
//=======================================================================

//! simple model with redefined pure virtual method
class TObjDRAW_Model : public TObj_Model
{
public:
  Standard_EXPORT TObjDRAW_Model()
      : TObj_Model()
  {
  }

  virtual Standard_EXPORT Handle(TObj_Model) NewEmpty() Standard_OVERRIDE
  {
    return new TObjDRAW_Model();
  }

public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(TObjDRAW_Model, TObj_Model)
};
DEFINE_STANDARD_HANDLE(TObjDRAW_Model, TObj_Model)

//! simple object to check API and features of TObj_Object
class TObjDRAW_Object : public TObj_Object
{
protected:
  //! enumeration for the ranks of label under Data section.
  enum DataTag
  {
    DataTag_First = TObj_Object::DataTag_Last,
    DataTag_IntVal,
    DataTag_RealArr,
    DataTag_Last = DataTag_First + 100
  };

  // enumeration for the ranks of label under Reference section.
  enum RefTag
  {
    RefTag_First = TObj_Object::RefTag_Last,
    RefTag_Other, //!< here we test only one reference to other
    RefTag_Last = RefTag_First + 100
  };

  //! enumeration for the ranks of label under Children section.
  enum ChildTag
  {
    ChildTag_First = TObj_Object::ChildTag_Last,
    ChildTag_Child, //!< here we test only one child (or one branch of children)
    ChildTag_Last = ChildTag_First + 100
  };

public:
  Standard_EXPORT TObjDRAW_Object(const TDF_Label& theLab)
      : TObj_Object(theLab)
  {
  }

  //! sets int value
  Standard_EXPORT void SetInt(const Standard_Integer theVal) { setInteger(theVal, DataTag_IntVal); }

  //! returns int value
  Standard_EXPORT Standard_Integer GetInt() const { return getInteger(DataTag_IntVal); }

  //! sets array of real
  Standard_EXPORT void SetRealArr(const Handle(TColStd_HArray1OfReal)& theHArr)
  {
    setArray(theHArr, DataTag_RealArr);
  }

  //! returns array of real
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetRealArr() const
  {
    return getRealArray(0, DataTag_RealArr);
  }

  //! set reference to other object
  Standard_EXPORT void SetRef(const Handle(TObj_Object)& theOther)
  {
    setReference(theOther, RefTag_Other);
  }

  //! return reference
  Standard_EXPORT Handle(TObj_Object) GetRef() const { return getReference(RefTag_Other); }

  //! add child object
  Standard_EXPORT Handle(TObj_Object) AddChild()
  {
    TDF_Label aChL = getChildLabel(ChildTag_Child).NewChild();
    return new TObjDRAW_Object(aChL);
  }

protected:
  // Persistence of TObj object
  DECLARE_TOBJOCAF_PERSISTENCE(TObjDRAW_Object, TObj_Object)

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(TObjDRAW_Object, TObj_Object)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE(TObjDRAW_Object, TObj_Object)

IMPLEMENT_TOBJOCAF_PERSISTENCE(TObjDRAW_Object)

//=================================================================================================

static Standard_Integer newModel(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use " << argv[0] << "nameDoc\n";
    return 1;
  }

  Handle(TDocStd_Document)     D;
  Handle(DDocStd_DrawDocument) DD;

  if (!DDocStd::GetDocument(argv[1], D, Standard_False))
  {
    Handle(TObjDRAW_Model) aModel = new TObjDRAW_Model();
    // initializes the new model: filename is empty
    aModel->Load("");
    D  = aModel->GetDocument();
    DD = new DDocStd_DrawDocument(D);
    TDataStd_Name::Set(D->GetData()->Root(), argv[1]);
    Draw::Set(argv[1], DD);
    di << "document " << argv[1] << " created\n";
  }
  else
    di << argv[1] << " is already a document\n";

  return 0;
}

static Handle(TObj_Model) getModelByName(const char* theName)
{
  Handle(TObj_Model)       aModel;
  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(theName, D))
    return aModel;

  TDF_Label           aLabel = D->Main();
  Handle(TObj_TModel) aModelAttr;
  if (!aLabel.IsNull() && aLabel.FindAttribute(TObj_TModel::GetID(), aModelAttr))
    aModel = aModelAttr->Model();
  return aModel;
}

//=================================================================================================

static Standard_Integer saveModel(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use " << argv[0] << "nameDoc [fileName] [-stream]\n";
    return 1;
  }

  Handle(TObj_Model) aModel = getModelByName(argv[1]);
  if (aModel.IsNull())
    return 1;
  Standard_Boolean isSaved = Standard_False;
  if (argc > 2)
  {
    Standard_Boolean anUseStream(Standard_False);
    for (Standard_Integer i = 3; i < argc && !anUseStream; i++)
    {
      if (!strcmp(argv[i], "-stream"))
      {
        di << "standard SEEKABLE stream is used\n";
        anUseStream = Standard_True;
      }
    }
    if (anUseStream)
    {
      const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
      std::shared_ptr<std::ostream> aFileStream =
        aFileSystem->OpenOStream(argv[2], std::ios::out | std::ios::binary);
      isSaved = aModel->SaveAs(*aFileStream);
    }
    else
      isSaved = aModel->SaveAs(TCollection_ExtendedString(argv[2], Standard_True));
  }
  else
    isSaved = aModel->Save();

  if (!isSaved)
  {
    di << "Error: Document not saved\n";
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer loadModel(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use " << argv[0] << "nameDoc fileName [-stream]\n";
    return 1;
  }

  Standard_Boolean anUseStream = Standard_False;
  for (Standard_Integer i = 3; i < argc && !anUseStream; i++)
  {
    if (!strcmp(argv[i], "-stream"))
    {
      di << "standard SEEKABLE stream is used\n";
      anUseStream = Standard_True;
    }
  }

  Standard_Boolean           isLoaded = Standard_False;
  Handle(TObj_Model)         aModel   = getModelByName(argv[1]);
  TCollection_ExtendedString aPath(argv[2], Standard_True);
  if (aModel.IsNull())
  {
    // create new
    aModel = new TObjDRAW_Model();
    if (anUseStream)
    {
      const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
      std::shared_ptr<std::istream> aFileStream =
        aFileSystem->OpenIStream(aPath, std::ios::in | std::ios::binary);
      isLoaded = aModel->Load(*aFileStream);
    }
    else
      isLoaded = aModel->Load(aPath);

    if (isLoaded)
    {
      Handle(TDocStd_Document)     D  = aModel->GetDocument();
      Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(D);

      TDataStd_Name::Set(D->GetData()->Root(), argv[1]);
      Draw::Set(argv[1], DD);
    }
  }
  else
  {
    isLoaded = aModel->Load(aPath);
  }

  if (!isLoaded)
  {
    di << "Error: Document not loaded\n";
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer closeModel(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use " << argv[0] << "nameDoc\n";
    return 1;
  }

  Handle(TObj_Model) aModel = getModelByName(argv[1]);
  if (aModel.IsNull())
    return 1;
  aModel->Close();

  return 0;
}

//=================================================================================================

static Standard_Integer addObj(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use " << argv[0] << "DocName ObjName\n";
    return 1;
  }
  Handle(TObj_Model) aModel = getModelByName(argv[1]);
  if (aModel.IsNull())
    return 1;
  Handle(TObjDRAW_Object) tObj = new TObjDRAW_Object(aModel->GetMainPartition()->NewLabel());
  if (tObj.IsNull())
  {
    di << "Error: Object not created\n";
    return 1;
  }
  tObj->SetName(argv[2]);

  return 0;
}

static Handle(TObjDRAW_Object) getObjByName(const char* modelName, const char* objName)
{
  Handle(TObjDRAW_Object) tObj;
  Handle(TObj_Model)      aModel = getModelByName(modelName);
  if (aModel.IsNull())
    return tObj;
  Handle(TCollection_HExtendedString) aName = new TCollection_HExtendedString(objName);
  Handle(TObj_TNameContainer)         aDict;
  tObj = Handle(TObjDRAW_Object)::DownCast(aModel->FindObject(aName, aDict));
  return tObj;
}

//=================================================================================================

static Standard_Integer setVal(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    di << "Use " << argv[0] << "DocName ObjName1 intVal | -r N r1 r2 ... rN\n";
    return 1;
  }
  Handle(TObjDRAW_Object) tObj = getObjByName(argv[1], argv[2]);
  if (tObj.IsNull())
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  if (!strcmp(argv[3], "-r"))
  {
    int                           Nb   = Draw::Atoi(argv[4]);
    Handle(TColStd_HArray1OfReal) rArr = new TColStd_HArray1OfReal(1, Nb);
    for (int i = 1; i <= Nb; i++)
      rArr->SetValue(i, Draw::Atof(argv[4 + i]));
    tObj->SetRealArr(rArr);
  }
  else
    tObj->SetInt(Draw::Atoi(argv[3]));

  return 0;
}

//=================================================================================================

static Standard_Integer getVal(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    di << "Use " << argv[0] << "DocName ObjName1 -i | -r\n";
    return 1;
  }

  Handle(TObjDRAW_Object) tObj = getObjByName(argv[1], argv[2]);
  if (tObj.IsNull())
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  if (!strcmp(argv[3], "-i"))
    di << tObj->GetInt();
  else
  {
    Handle(TColStd_HArray1OfReal) rArr = tObj->GetRealArr();
    if (!rArr.IsNull())
      for (int i = 1, n = rArr->Upper(); i <= n; i++)
      {
        if (i > 1)
          di << " ";
        di << rArr->Value(i);
      }
  }

  return 0;
}

//=================================================================================================

static Standard_Integer setRef(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    di << "Use " << argv[0] << "DocName ObjName1 ObjName2\n";
    return 1;
  }

  Handle(TObjDRAW_Object) tObj1 = getObjByName(argv[1], argv[2]);
  Handle(TObjDRAW_Object) tObj2 = getObjByName(argv[1], argv[3]);
  if (tObj1.IsNull() || tObj2.IsNull())
  {
    di << "Error: Object " << argv[2] << " or object " << argv[3] << " not found\n";
    return 1;
  }
  tObj1->SetRef(tObj2);

  return 0;
}

//=================================================================================================

static Standard_Integer getRef(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use " << argv[0] << "DocName ObjName\n";
    return 1;
  }

  Handle(TObjDRAW_Object) tObj = getObjByName(argv[1], argv[2]);
  if (tObj.IsNull())
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  Handle(TObj_Object) aRefObj = tObj->GetRef();
  if (aRefObj.IsNull())
    return 1;
  else
  {
    TCollection_AsciiString aName;
    aRefObj->GetName(aName);
    di << aName.ToCString();
  }

  return 0;
}

//=================================================================================================

static Standard_Integer addChild(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    di << "Use " << argv[0] << "DocName ObjName childObj\n";
    return 1;
  }

  Handle(TObjDRAW_Object) tObj = getObjByName(argv[1], argv[2]);
  if (tObj.IsNull())
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  Handle(TObj_Object) chldObj = tObj->AddChild();
  if (chldObj.IsNull())
  {
    di << "Error: No child object created\n";
    return 1;
  }
  chldObj->SetName(new TCollection_HExtendedString(argv[3]));

  return 0;
}

//=================================================================================================

static Standard_Integer getChildren(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use " << argv[0] << "DocName ObjName [-all]\n";
    di << "    -all: recurse to list children of all levels\n";
    return 1;
  }

  Handle(TObjDRAW_Object) tObj = getObjByName(argv[1], argv[2]);
  if (tObj.IsNull())
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }

  bool                        aGetSubs = (argc > 3 && !strcasecmp(argv[3], "-all"));
  Handle(TObj_ObjectIterator) anItr =
    aGetSubs
      ? new TObj_OcafObjectIterator(tObj->GetChildLabel(), NULL, Standard_True, Standard_True)
      : tObj->GetChildren();

  int i = 0;
  for (; anItr->More(); anItr->Next(), i++)
  {
    Handle(TObj_Object)     anObj = anItr->Value();
    TCollection_AsciiString aName;
    anObj->GetName(aName);
    if (i > 0)
      di << " ";
    di << aName.ToCString();
  }

  return 0;
}

//=================================================================================================

static Standard_Integer hasModifications(Draw_Interpretor& di,
                                         Standard_Integer  argc,
                                         const char**      argv)
{
  if (argc < 3)
  {
    di << "Use " << argv[0] << "DocName ObjName\n";
    return 1;
  }

  Handle(TObjDRAW_Object) tObj = getObjByName(argv[1], argv[2]);
  if (tObj.IsNull())
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  di << "Status modifications : " << (tObj->HasModifications() ? 1 : 0) << "\n";

  return 0;
}

//=================================================================================================

void TObjDRAW::Init(Draw_Interpretor& di)
{
  static Standard_Boolean initactor = Standard_False;
  if (initactor)
  {
    return;
  }
  initactor = Standard_True;

  //=====================================
  // General commands
  //=====================================

  Standard_CString g = "TObj general commands";

  di.Add("TObjNew",
         "DocName \t: Create new TObj model with document named DocName",
         __FILE__,
         newModel,
         g);

  di.Add("TObjSave",
         "DocName [Path] [-stream] \t: Save Model with DocName [by file path] [into a file stream]",
         __FILE__,
         saveModel,
         g);

  di.Add("TObjLoad",
         "DocName Path [-stream] \t: Load model DocName from file Path [file stream]",
         __FILE__,
         loadModel,
         g);

  di.Add("TObjClose", "DocName\t: Close model DocName", __FILE__, closeModel, g);

  di.Add("TObjAddObj", "DocName ObjName \t: Add object to model document", __FILE__, addObj, g);

  di.Add("TObjSetVal",
         "DocName ObjName1 intVal | -r N r1 r2 ... rN \t: Set one integer or set of real values",
         __FILE__,
         setVal,
         g);

  di.Add("TObjGetVal",
         "DocName ObjName1 -i | -r \t: Returns one integer or set of real values",
         __FILE__,
         getVal,
         g);

  di.Add("TObjSetRef",
         "DocName ObjName1 ObjName2 \t: Set reference from object1 to object2",
         __FILE__,
         setRef,
         g);

  di.Add("TObjGetRef", "DocName ObjName \t: Returns list of children objects", __FILE__, getRef, g);

  di.Add("TObjAddChild",
         "DocName ObjName chldName \t: Add child object to indicated object",
         __FILE__,
         addChild,
         g);

  di.Add("TObjGetChildren",
         "DocName ObjName [-all]\t: Returns list of children objects (-all to recurse)",
         __FILE__,
         getChildren,
         g);

  di.Add("TObjHasModifications",
         "DocName ObjName \t: Returns status of modification of the object (if object has been "
         "modified 1, otherwise 0)",
         __FILE__,
         hasModifications,
         g);
}

//==============================================================================
// TObjDRAW::Factory

//==============================================================================
void TObjDRAW::Factory(Draw_Interpretor& theDI)
{
  // Initialize TObj OCAF formats
  Handle(TDocStd_Application) anApp = TObj_Application::GetInstance(); // DDocStd::GetApplication();
  BinTObjDrivers::DefineFormat(anApp);
  XmlTObjDrivers::DefineFormat(anApp);

  // define formats for TObj specific application
  BinTObjDrivers::DefineFormat(anApp);
  XmlTObjDrivers::DefineFormat(anApp);

  TObjDRAW::Init(theDI);

#ifdef OCCT_DEBUG
  theDI << "Draw Plugin : All TKTObjDRAW commands are loaded\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(TObjDRAW)
