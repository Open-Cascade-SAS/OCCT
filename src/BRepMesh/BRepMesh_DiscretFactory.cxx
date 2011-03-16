// File:	BRepMesh_DiscretFactory.cxx
// Created:	Thu Apr 10 13:32:00 2008
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BRepMesh_DiscretFactory.ixx>

#include <OSD_SharedLibrary.hxx>
#include <OSD_Function.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

static 
  void MakeLibName(const TCollection_AsciiString& , 
		   TCollection_AsciiString& );
static
  Standard_Integer CreateDiscret(const TopoDS_Shape& theShape,
				 const Standard_Real    ,
				 const Standard_Real    ,
				 OSD_Function& ,
				 BRepMesh_PDiscretRoot& );

//=======================================================================
//function : BRepMesh_DiscretFactory
//purpose  : 
//=======================================================================
BRepMesh_DiscretFactory::BRepMesh_DiscretFactory()
{
  myFixedNames[0]="FastDiscret";

  //
  myNames.Add(myFixedNames[0]);
  myDefaultName=myFixedNames[0];
  myFunctionName="DISCRETALGO";
  myPDiscret=NULL;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BRepMesh_DiscretFactory::~BRepMesh_DiscretFactory()
{
  Clear();
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
void BRepMesh_DiscretFactory::Clear()
{
  if (myPDiscret) {
    delete myPDiscret;
    myPDiscret=NULL;
  }
}
//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
BRepMesh_DiscretFactory& BRepMesh_DiscretFactory::Get() 
{
  static BRepMesh_DiscretFactory* sFactory;
  static Standard_Boolean isInit=Standard_False;
  if(!isInit) {
    isInit = Standard_True;
    sFactory = new BRepMesh_DiscretFactory;
  }
  return *sFactory;
}
//=======================================================================
//function : ErrorStatus
//purpose  : 
//=======================================================================
BRepMesh_FactoryError BRepMesh_DiscretFactory::ErrorStatus()const
{
  return myErrorStatus;
}
//=======================================================================
//function : Names
//purpose  : 
//=======================================================================
const TColStd_MapOfAsciiString& BRepMesh_DiscretFactory::Names()const
{
  return myNames;
}
//=======================================================================
//function : SetDefaultName
//purpose  : 
//=======================================================================
void BRepMesh_DiscretFactory::SetDefaultName(const TCollection_AsciiString& theName)
{
  myDefaultName=theName;
}
//=======================================================================
//function : DefaultName
//purpose  : 
//=======================================================================
const TCollection_AsciiString& BRepMesh_DiscretFactory::DefaultName()const
{
  return myDefaultName;
}
//=======================================================================
//function : SetFunctionName
//purpose  : 
//=======================================================================
void BRepMesh_DiscretFactory::SetFunctionName(const TCollection_AsciiString& theName)
{
  myFunctionName=theName;
}
//=======================================================================
//function : FunctionName
//purpose  : 
//=======================================================================
const TCollection_AsciiString& BRepMesh_DiscretFactory::FunctionName()const
{
  return myFunctionName;
}
//=======================================================================
//function : Discret
//purpose  : 
//=======================================================================
BRepMesh_PDiscretRoot& 
  BRepMesh_DiscretFactory::Discret(const TopoDS_Shape& theShape,
				   const Standard_Real theDeflection,
				   const Standard_Real theAngle)
{
  myErrorStatus=BRepMesh_FE_NOERROR;
  Clear();
  // DEB f
  //myDefaultName="TKXMesh";
  // DEB t
  if(myDefaultName==myFixedNames[0]) {
    myPDiscret=new BRepMesh_IncrementalMesh;
    myPDiscret->SetDeflection(theDeflection);
    myPDiscret->SetAngle(theAngle);
    myPDiscret->SetShape(theShape);
  }
  else {
    Standard_Integer iErr;
    TCollection_AsciiString aLibName;
    OSD_Function aF;
    //
    myPDiscret=NULL;
    //
    MakeLibName(myDefaultName, aLibName);
    //
    OSD_SharedLibrary aSL(aLibName.ToCString());
    if (!aSL.DlOpen(OSD_RTLD_LAZY)) {
      myErrorStatus=BRepMesh_FE_LIBRARYNOTFOUND; // library is not found  
      return myPDiscret;
    }
    //
    aF = aSL.DlSymb(myFunctionName.ToCString());
    if(aF==NULL ) {
      myErrorStatus=BRepMesh_FE_FUNCTIONNOTFOUND; // function is not found  
      return myPDiscret;
    }
    //
    iErr=CreateDiscret(theShape,
		       theDeflection,
		       theAngle,
		       aF,
		       myPDiscret);
    if (iErr) {
      myErrorStatus=BRepMesh_FE_CANNOTCREATEALGO; // can not create the algo specified  
    }
    else {
      myNames.Add(myDefaultName);
    }
  }
  //
  return myPDiscret;
}
//=======================================================================
//function : CreateDiscret
//purpose  : 
//=======================================================================
Standard_Integer CreateDiscret(const TopoDS_Shape& theShape,
			       const Standard_Real theDeflection,
			       const Standard_Real theAngle,
			       OSD_Function& theF,
			       BRepMesh_PDiscretRoot& theAlgo)
{
  Standard_Integer iErr;
  Standard_Integer (*fp) (const TopoDS_Shape& ,
			  const Standard_Real ,
			  const Standard_Real ,
			  BRepMesh_PDiscretRoot& );
  //
  fp=(Standard_Integer (*)(const TopoDS_Shape& ,
			   const Standard_Real ,
			   const Standard_Real ,
			   BRepMesh_PDiscretRoot&)) theF;
  //
  iErr=(*fp)(theShape,
	     theDeflection,
	     theAngle,
	     theAlgo);
  //
  return iErr;
}     
//=======================================================================
//function : MakeLibName
//purpose  : 
//=======================================================================
void MakeLibName(const TCollection_AsciiString& theDefaultName, 
		 TCollection_AsciiString& theLibName)
{
  theLibName="";
#ifndef WNT
  theLibName+="lib";
#endif
  theLibName+=theDefaultName;
#ifdef WNT
  theLibName+=".dll";
#elif __APPLE__
  theLibName+=".dylib";
#elif defined (HPUX) || defined(_hpux)
  theLibName+=".sl";
#else
  theLibName+=".so";
#endif  
}
