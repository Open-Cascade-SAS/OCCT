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
#include <Message_MsgFile.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Data.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TObj_Application.hxx>
#include <TObj_Model.hxx>
#include <TObj_Object.hxx>
#include <TObj_ObjectIterator.hxx>
#include <TObj_TModel.hxx>
#include <TObj_TNameContainer.hxx>
#include <TObjDRAW.hxx>

#include <BinTObjDrivers.hxx>
#include <XmlTObjDrivers.hxx>

#include <stdio.h>

//=======================================================================
// Section: General commands
//=======================================================================


//! simple model with redefined pure virtual method
class TObjDRAW_Model : public TObj_Model
{
 public:
  Standard_EXPORT TObjDRAW_Model()
    : TObj_Model() {}
  
  virtual Standard_EXPORT Handle(TObj_Model) NewEmpty() Standard_OVERRIDE
    {
      return new TObjDRAW_Model();
    }
  
 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(TObjDRAW_Model,TObj_Model)

};
DEFINE_STANDARD_HANDLE (TObjDRAW_Model,TObj_Model)



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
    RefTag_Other, //!< here we test only one refrence to other
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
    : TObj_Object( theLab )  {}
  
  //! sets int value
  Standard_EXPORT void SetInt( const Standard_Integer theVal )
    { setInteger( theVal, DataTag_IntVal ); }
  //! returns int value
  Standard_EXPORT Standard_Integer GetInt() const
    { return getInteger( DataTag_IntVal ); }
  
  //! sets array of real
  Standard_EXPORT void SetRealArr( const Handle(TColStd_HArray1OfReal)& theHArr )
    { setArray( theHArr, DataTag_RealArr ); }
  //! returns array of real
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetRealArr() const
    { return getRealArray( 0, DataTag_RealArr ); }
  
  //! set reference to other object
  Standard_EXPORT void SetRef( const Handle(TObj_Object)& theOther )
    { setReference( theOther, RefTag_Other ); }
  //! return reference
  Standard_EXPORT Handle(TObj_Object) GetRef() const
    { return getReference( RefTag_Other ); }
  
  //! add child object
  Standard_EXPORT Handle(TObj_Object) AddChild()
    {
      TDF_Label aChL = getChildLabel( ChildTag_Child ).NewChild();
      return new TObjDRAW_Object( aChL );
    }
  
  protected:
  // Persistence of TObj object
  DECLARE_TOBJOCAF_PERSISTENCE(TObjDRAW_Object,TObj_Object)

 public:
  // Declaration of CASCADE RTTI
 DEFINE_STANDARD_RTTI_INLINE(TObjDRAW_Object,TObj_Object)
  
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (TObjDRAW_Object,TObj_Object)


IMPLEMENT_TOBJOCAF_PERSISTENCE(TObjDRAW_Object)

//=======================================================================
//function : newModel
//purpose  :
//=======================================================================
static Standard_Integer newModel (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {di<<"Use "<< argv[0] << "nameDoc\n";return 1;}

  Handle(TDocStd_Document) D;
  Handle(DDocStd_DrawDocument) DD;

  if (!DDocStd::GetDocument(argv[1],D,Standard_False)) {
    Handle(TObjDRAW_Model) aModel = new TObjDRAW_Model();
    // initializes the new model: filename is empty
    aModel->Load("");
    D = aModel->GetDocument();
    DD = new DDocStd_DrawDocument(D);
    TDataStd_Name::Set(D->GetData()->Root(),argv[1]);
    Draw::Set(argv[1],DD);
    di << "document " << argv[1] << " created\n";
  }
  else di << argv[1] << " is already a document\n";

  return 0;
}

static Handle(TObj_Model) getModelByName( const char* theName )
{
  Handle(TObj_Model) aModel;
  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(theName,D)) return aModel;
  
  TDF_Label aLabel = D->Main();
  Handle(TObj_TModel) aModelAttr;
  if (!aLabel.IsNull() && aLabel.FindAttribute(TObj_TModel::GetID(), aModelAttr))
    aModel = aModelAttr->Model();
  return aModel;
}

//=======================================================================
//function : saveModel
//purpose  :
//=======================================================================
static Standard_Integer saveModel (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {di<<"Use "<< argv[0] << "nameDoc [fileName]\n";return 1;}
  
  Handle(TObj_Model) aModel = getModelByName(argv[1]);
  if ( aModel.IsNull() ) return 1;
  Standard_Boolean isSaved = Standard_False; 
  if (argc > 2 )
    isSaved = aModel->SaveAs( argv[2] );
  else
    isSaved = aModel->Save();
  
  if (!isSaved) {
    di << "Error: Document not saved\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//function : loadModel
//purpose  :
//=======================================================================
static Standard_Integer loadModel (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {di<<"Use "<< argv[0] << "nameDoc fileName\n";return 1;}
  
  Standard_Boolean isLoaded = Standard_False;
  Handle(TObj_Model) aModel = getModelByName(argv[1]);
  if ( aModel.IsNull() )
  {
    // create new
    aModel = new TObjDRAW_Model();
    isLoaded = aModel->Load( argv[2] );
    if ( isLoaded )
    {
      Handle(TDocStd_Document) D = aModel->GetDocument();
      Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(D);
    
      TDataStd_Name::Set(D->GetData()->Root(),argv[1]);
      Draw::Set(argv[1],DD);
    }
  }
  else
    isLoaded = aModel->Load( argv[2] );
  
  
  if (!isLoaded) {
    di << "Error: Document not loaded\n";
    return 1;
  }
  return 0;
}


//=======================================================================
//function : closeModel
//purpose  :
//=======================================================================
static Standard_Integer closeModel (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {di<<"Use "<< argv[0] << "nameDoc\n";return 1;}
  
  Handle(TObj_Model) aModel = getModelByName(argv[1]);
  if ( aModel.IsNull() ) return 1;
  aModel->Close();

  return 0;
}

//=======================================================================
//function : addObj
//purpose  :
//=======================================================================
static Standard_Integer addObj (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {di<<"Use "<< argv[0] << "DocName ObjName\n";return 1;}
  Handle(TObj_Model) aModel = getModelByName(argv[1]);
  if ( aModel.IsNull() ) return 1;
  Handle(TObjDRAW_Object) tObj =
    new TObjDRAW_Object( aModel->GetMainPartition()->NewLabel() );
  if ( tObj.IsNull() )
  {
    di << "Error: Object not created\n";
    return 1;
  }
  tObj->SetName( argv[2] );
  
  return 0;
}

static Handle(TObjDRAW_Object) getObjByName( const char* modelName, const char* objName )
{
  Handle(TObjDRAW_Object) tObj;
  Handle(TObj_Model) aModel = getModelByName(modelName);
  if ( aModel.IsNull() )
    return tObj;
  Handle(TCollection_HExtendedString) aName = new TCollection_HExtendedString( objName );
  Handle(TObj_TNameContainer) aDict;
  tObj = Handle(TObjDRAW_Object)::DownCast( aModel->FindObject(aName, aDict) );
  return tObj;
}

//=======================================================================
//function : setVal
//purpose  :
//=======================================================================
static Standard_Integer setVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {di<<"Use "<< argv[0] << "DocName ObjName1 intVal | -r N r1 r2 ... rN\n";return 1;}
  Handle(TObjDRAW_Object) tObj = getObjByName( argv[1], argv[2] );
  if ( tObj.IsNull() )
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  if ( !strcmp(argv[3],"-r") )
  {
    int Nb = Draw::Atoi(argv[4]);
    Handle(TColStd_HArray1OfReal) rArr = new TColStd_HArray1OfReal(1,Nb);
    for ( int i = 1; i <= Nb; i++ )
      rArr->SetValue(i, Draw::Atof(argv[4+i]));
    tObj->SetRealArr( rArr );
  }
  else
    tObj->SetInt( Draw::Atoi(argv[3] ) ); 
  
  return 0;
}

//=======================================================================
//function : getVal
//purpose  :
//=======================================================================
static Standard_Integer getVal (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {di<<"Use "<< argv[0] << "DocName ObjName1 -i | -r\n";return 1;}

  Handle(TObjDRAW_Object) tObj = getObjByName( argv[1], argv[2] );
  if ( tObj.IsNull() )
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  if ( !strcmp(argv[3],"-i") )
    di << tObj->GetInt();
  else
  {
    Handle(TColStd_HArray1OfReal) rArr = tObj->GetRealArr();
    if ( !rArr.IsNull() )
      for ( int i = 1, n = rArr->Upper(); i <= n; i++ )
      {
        if ( i > 1 )
          di << " ";
        di << rArr->Value(i);
      }
  }

  return 0;
}

//=======================================================================
//function : setRef
//purpose  :
//=======================================================================
static Standard_Integer setRef (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {di<<"Use "<< argv[0] << "DocName ObjName1 ObjName2\n";return 1;}

  Handle(TObjDRAW_Object) tObj1 = getObjByName( argv[1], argv[2] );
  Handle(TObjDRAW_Object) tObj2 = getObjByName( argv[1], argv[3] );
  if ( tObj1.IsNull() || tObj2.IsNull() )
  {
    di << "Error: Object " << argv[2] << " or object " << argv[3] << " not found\n";
    return 1;
  }
  tObj1->SetRef( tObj2 );

  return 0;
}

//=======================================================================
//function : getRef
//purpose  :
//=======================================================================
static Standard_Integer getRef (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {di<<"Use "<< argv[0] << "DocName ObjName\n";return 1;}

  Handle(TObjDRAW_Object) tObj = getObjByName( argv[1], argv[2] );
  if ( tObj.IsNull() )
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  Handle(TObj_Object) aRefObj = tObj->GetRef();
  if ( aRefObj.IsNull() )
    return 1;
  else
  {
    TCollection_AsciiString aName;
    aRefObj->GetName( aName );
    di << aName.ToCString();
  }
  
  return 0;
}

//=======================================================================
//function : addChild
//purpose  :
//=======================================================================
static Standard_Integer addChild (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {di<<"Use "<< argv[0] << "DocName ObjName childObj\n";return 1;}

  Handle(TObjDRAW_Object) tObj = getObjByName( argv[1], argv[2] );
  if ( tObj.IsNull() )
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  Handle(TObj_Object) chldObj = tObj->AddChild();
  if ( chldObj.IsNull() )
  {
    di << "Error: No child object created\n";
    return 1;
  }
  chldObj->SetName( new TCollection_HExtendedString( argv[3] ) );
  
  return 0;
}

//=======================================================================
//function : getChild
//purpose  :
//=======================================================================
static Standard_Integer getChild (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {di<<"Use "<< argv[0] << "DocName ObjName\n";return 1;}

  Handle(TObjDRAW_Object) tObj = getObjByName( argv[1], argv[2] );
  if ( tObj.IsNull() )
  {
    di << "Error: Object " << argv[2] << " not found\n";
    return 1;
  }
  Handle(TObj_ObjectIterator) anItr = tObj->GetChildren();
  int i = 0;
  for ( ; anItr->More(); anItr->Next(), i++ )
  {
    Handle(TObj_Object) anObj = anItr->Value();
    TCollection_AsciiString aName;
    anObj->GetName( aName );
    if ( i > 0 )
      di << " ";
    di << aName.ToCString();
  }
  
  return 0;
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================

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

  di.Add ("TObjNew","DocName \t: Create new TObj model with document named DocName",
		   __FILE__, newModel, g);

  di.Add ("TObjSave","DocName [Path] \t: Save Model with DocName",
		   __FILE__, saveModel, g);

  di.Add ("TObjLoad","DocName Path \t: Load model DocName from file Path",
		   __FILE__, loadModel, g);

  di.Add ("TObjClose","DocName\t: Close model DocName",
		   __FILE__, closeModel, g);

  di.Add ("TObjAddObj","DocName ObjName \t: Add object to model document",
		   __FILE__, addObj, g);

  di.Add ("TObjSetVal","DocName ObjName1 intVal | -r N r1 r2 ... rN \t: Set one integer or set of real values",
		   __FILE__, setVal, g);

  di.Add ("TObjGetVal","DocName ObjName1 -i | -r \t: Returns one integer or set of real values",
		   __FILE__, getVal, g);

  di.Add ("TObjSetRef","DocName ObjName1 ObjName2 \t: Set reference from object1 to object2",
		   __FILE__, setRef, g);

  di.Add ("TObjGetRef","DocName ObjName \t: Returns list of children objects",
		   __FILE__, getRef, g);
  
  di.Add ("TObjAddChild","DocName ObjName chldName \t: Add child object to indicated object",
		   __FILE__, addChild, g);
  
  di.Add ("TObjGetChildren","DocName ObjName \t: Returns list of children objects",
		   __FILE__, getChild, g);
  
}


//==============================================================================
// TObjDRAW::Factory

//==============================================================================
void TObjDRAW::Factory(Draw_Interpretor& theDI)
{
  // Initialize TObj OCAF formats
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  BinTObjDrivers::DefineFormat(anApp);
  XmlTObjDrivers::DefineFormat(anApp);

  // define formats for TObj specific application
  anApp = TObj_Application::GetInstance();
  BinTObjDrivers::DefineFormat(anApp);
  XmlTObjDrivers::DefineFormat(anApp);

  TObjDRAW::Init(theDI);

#ifdef OCCT_DEBUG
      theDI << "Draw Plugin : All TKTObjDRAW commands are loaded\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(TObjDRAW)
