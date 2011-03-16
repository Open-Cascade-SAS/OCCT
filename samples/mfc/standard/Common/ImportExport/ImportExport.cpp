// ImportExport.cpp: implementation of the CImportExport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImportExport.h"
#include <OCC_BaseApp.h>

#include "SaveCSFDBDlg.h"

#include "SaveSTEPDlg.h"

#include "TColStd_SequenceOfAsciiString.hxx"
#include "TColStd_SequenceOfExtendedString.hxx"
#include "OSD_Timer.hxx"

#include "IGESControl_Reader.hxx"
#include "STEPControl_Controller.hxx"

#include <BRepAlgo.hxx>
#include <FSD_File.hxx>
#include <ShapeSchema.hxx>
#include <PTopoDS_HShape.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Storage_Root.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <MgtBRep.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <Interface_Static.hxx>
#include <STEPControl_Reader.hxx>
#include <Geom_Curve.hxx>
#include <STEPControl_Writer.hxx>
#include <TopoDS_Compound.hxx>
#include <Geom_Line.hxx>
#include <StlAPI_Writer.hxx>
#include <VrmlAPI_Writer.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_ShapeConvert.hxx>
#include <VrmlData_Appearance.hxx>
#include <VrmlData_Material.hxx>
#include <VrmlData_Group.hxx>
#include <VrmlData_ListOfNode.hxx>
#include <VrmlData_ShapeNode.hxx>

#include <XSControl_WorkSession.hxx>
#include <STEPConstruct_Styles.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <STEPConstruct.hxx>
#include <StepVisual_StyledItem.hxx>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Handle(TopTools_HSequenceOfShape) CImportExport::BuildSequenceFromContext(const Handle(AIS_InteractiveContext)& anInteractiveContext,
                                                                          Handle(Quantity_HArray1OfColor)&      anArrayOfColors,
                                                                          Handle(TColStd_HArray1OfReal)&        anArrayOfTransparencies) 
{
    Handle(TopTools_HSequenceOfShape) aSequence;
    Standard_Integer nb = anInteractiveContext->NbCurrents(), i = 1;
    if (!nb)
        return aSequence;

    aSequence               = new TopTools_HSequenceOfShape();
    anArrayOfColors         = new Quantity_HArray1OfColor(1, nb);
    anArrayOfTransparencies = new TColStd_HArray1OfReal  (1, nb);

    Handle(AIS_InteractiveObject) picked;
    for(anInteractiveContext->InitCurrent();anInteractiveContext->MoreCurrent();anInteractiveContext->NextCurrent())
      {
        picked = anInteractiveContext->Current();
        if (anInteractiveContext->Current()->IsKind(STANDARD_TYPE(AIS_Shape)))
	     {
            Handle(AIS_Shape) aisShape = Handle(AIS_Shape)::DownCast(picked);
	        TopoDS_Shape aShape = aisShape->Shape();
            aSequence->Append(aShape);
            
            Quantity_Color color;
            aisShape->Color(color);
            anArrayOfColors->SetValue(i, color);
            
            Standard_Real transparency = aisShape->Transparency();
            anArrayOfTransparencies->SetValue(i, transparency);

            i++;
	     }
      }
      return aSequence;
}

//======================================================================
//=                                                                    =
//=                      BREP                                          =
//=                                                                    =
//======================================================================

int CImportExport::ReadBREP(const Handle_AIS_InteractiveContext& anInteractiveContext,LPCTSTR InitialDir /* = NULL*/)
{
    Handle(TopTools_HSequenceOfShape) aSequence = CImportExport::ReadBREP(InitialDir);
	if(aSequence->IsEmpty())
		return 1;
	Handle_AIS_Shape aShape;
    for(int i=1;i<= aSequence->Length();i++){
		aShape = new AIS_Shape(aSequence->Value(i));
		anInteractiveContext->SetDisplayMode(aShape, 1, Standard_False);
		anInteractiveContext->Display(aShape, Standard_False);
		anInteractiveContext->SetCurrentObject(aShape, Standard_False);
	} 
	return 0;
}

Handle(TopTools_HSequenceOfShape) CImportExport::ReadBREP(LPCTSTR  InitialDir /* = NULL*/) // not by reference --> the sequence is created here !!
{
  CFileDialog dlg(TRUE,
				  NULL,
				  NULL,
				  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				  "BREP Files (*.brep , *.rle)|*.brep;  *.BREP; *.rle; *.RLE; |All Files (*.*)|*.*||", 
				  NULL ); 

TCHAR tchBuf[80];

CString CASROOTValue = ((GetEnvironmentVariable("CASROOT", tchBuf, 80) > 0) ? tchBuf : NULL); 
CString initdir = (CASROOTValue + "\\..\\data\\occ");

dlg.m_ofn.lpstrInitialDir = initdir;

  Handle(TopTools_HSequenceOfShape) aSequence= new TopTools_HSequenceOfShape();

  if (dlg.DoModal() == IDOK) 
  {
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	CString filename = dlg.GetPathName();
    TopoDS_Shape aShape;
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)filename;
	Standard_Boolean result = ReadBREP(aFileName,aShape);
	if (result)
    {
	 	    if (!BRepAlgo::IsValid(aShape))
	        MessageBox(AfxGetMainWnd()->m_hWnd,"Warning: The shape is not valid!","Cascade Warning",MB_ICONWARNING);
		    aSequence->Append(aShape);
    }
    else 
	   MessageBox(AfxGetMainWnd()->m_hWnd,"Error: The file was not read","Cascade Error",MB_ICONERROR);

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }

  return aSequence;
}
//----------------------------------------------------------------------

Standard_Boolean CImportExport::ReadBREP(const Standard_CString& aFileName,
                                        TopoDS_Shape& aShape)
{
	BRep_Builder aBuilder;
	Standard_Boolean result = BRepTools::Read(aShape,aFileName,aBuilder);
    return result;
}

void CImportExport::SaveBREP(const Handle_AIS_InteractiveContext& anInteractiveContext)
{
	anInteractiveContext->InitCurrent();
	if (anInteractiveContext->NbCurrents() == 0){
		AfxMessageBox("No shape selected for export!");
		return;
	}
	Handle(TopTools_HSequenceOfShape) aHSequenceOfShape;
    Handle(Quantity_HArray1OfColor)   anArrayOfColors;
    Handle(TColStd_HArray1OfReal)     anArrayOfTransparencies;
	aHSequenceOfShape = BuildSequenceFromContext(anInteractiveContext, anArrayOfColors, anArrayOfTransparencies);
	int aLength = aHSequenceOfShape->Length();
	if (aLength == 1){
		TopoDS_Shape RES = aHSequenceOfShape->Value(1);
		CImportExport::SaveBREP(RES);
	} else {
		TopoDS_Compound RES;
		BRep_Builder MKCP;
		MKCP.MakeCompound(RES);
		for (Standard_Integer i=1;i<=aLength;i++)
		{
			TopoDS_Shape aShape= aHSequenceOfShape->Value(i);
			if ( aShape.IsNull() ) 
			{
				continue;
			}
			MKCP.Add(RES, aShape);
		}
		CImportExport::SaveBREP(RES);
	}
}

Standard_Boolean CImportExport::SaveBREP(const TopoDS_Shape& aShape)
{
  CFileDialog dlg(FALSE,_T("*.brep"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	              "BREP Files (*.brep)|*.brep;|BREP Files (*.BREP)|*.BREP;||", NULL );
  
TCHAR tchBuf[80];

CString CASROOTValue = ((GetEnvironmentVariable("CASROOT", tchBuf, 80) > 0) ? tchBuf : NULL); 
CString initdir = (CASROOTValue + "\\..\\data\\occ");

dlg.m_ofn.lpstrInitialDir = initdir;

  Standard_Boolean result = Standard_False; 
  if (dlg.DoModal() == IDOK)  
  { 
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT)); 
    CString filename = dlg.GetPathName(); 
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)filename;
    result =  SaveBREP(aFileName,aShape);
    if (!result)  
       MessageBox(0,"Error : The shape or shapes were not saved.",
	                "CasCade Error",MB_ICONERROR); 
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); 
  } 
  return result;
}

//----------------------------------------------------------------------------------------
Standard_Boolean CImportExport::SaveBREP(const Standard_CString& aFileName,
                                        const TopoDS_Shape& aShape)
{
    Standard_Boolean result = BRepTools::Write(aShape,aFileName); 
    return result;
}


//======================================================================
//=                                                                    =
//=                      CSFDB                                         =
//=                                                                    =
//======================================================================


TCollection_AsciiString CImportExport::BuildStorageErrorMessage( Storage_Error anError)

{ 
    TCollection_AsciiString aMessage("Storage Status :");
    switch ( anError ) {
        case Storage_VSOk : 
            aMessage += "no problem \n";
        break;
        case Storage_VSOpenError : 
        aMessage += "OpenError while opening the stream \n";
        break;
        case Storage_VSModeError : 
            aMessage += "the stream is opened with a wrong mode for operation \n";
        break;
        case Storage_VSCloseError : 
            aMessage += "CloseError while closing the stream \n";
        break;
        case Storage_VSAlreadyOpen : 
            aMessage += "stream is already opened \n";
        break;
        case Storage_VSNotOpen : 
            aMessage += "stream not opened \n";
        break;
        case Storage_VSSectionNotFound : 
            aMessage += "the section is not found \n";
        break;
        case Storage_VSWriteError : 
            aMessage += "error during writing \n";
        break;
        case Storage_VSFormatError : 
            aMessage += "wrong format error occured while reading \n";
        break;
        case Storage_VSUnknownType : 
            aMessage += "try to read an unknown type \n";
        break;
        case Storage_VSTypeMismatch : 
            aMessage += "try to read a wrong primitive type (read a char while expecting a real) \n";
        break;
        case Storage_VSInternalError : 
            aMessage += "internal error \n ";
        break;
        case Storage_VSExtCharParityError : 
            aMessage += "problem with 16bit characters, may be an 8bit character is inserted inside a 16bit string \n";
        break;
        default :
            aMessage += "Unknown Status ";
            aMessage += anError;
            aMessage += " \n";
        break;
    }
    return aMessage;
}

void CImportExport::ReadCSFDB(const Handle(AIS_InteractiveContext)& anInteractiveContext)
{
    Handle(TopTools_HSequenceOfShape) aSequence = CImportExport::ReadCSFDB();
    for(int i=1;i<= aSequence->Length();i++)
        anInteractiveContext->Display(new AIS_Shape(aSequence->Value(i)), Standard_False);
}

Handle(TopTools_HSequenceOfShape) CImportExport::ReadCSFDB() // not by reference --> the sequence is created here !!
{
  CFileDialog dlg(TRUE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  "CSFDB Files (*.CSFDB , *.csf)|*.csfdb;  *.csf; |All Files (*.*)|*.*||", 
                  NULL );

TCHAR tchBuf[80];

CString CASROOTValue = ((GetEnvironmentVariable("CASROOT", tchBuf, 80) > 0) ? tchBuf : NULL); 
CString initdir = (CASROOTValue + "\\..\\data\\csfdb");

dlg.m_ofn.lpstrInitialDir = initdir;
  
  Handle(TopTools_HSequenceOfShape) aSequence = new TopTools_HSequenceOfShape();;
  if (dlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CString filename = dlg.GetPathName();
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)filename;
    TCollection_AsciiString Message;
    Standard_Boolean result = ReadCSFDB(aFileName,aSequence,Message);
	if (result)
    {
       // Display The Message :
	   MessageBox(0,Message.ToCString(),"CasCade ",MB_OK);

 
    }
    else 
	   MessageBox(0,Message.ToCString(),"CasCade Error",MB_ICONERROR);
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }
  return aSequence;
}

Standard_Boolean CImportExport::ReadCSFDB(const Standard_CString& aFileName,
                                          Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape, // out parameter
                                          TCollection_AsciiString& ReturnMessage)               // out parameter
{
    // an I/O driver
    FSD_File f;

    // the applicative Schema
    Handle(ShapeSchema) s = new ShapeSchema;

    // a Read/Write data object
    Handle(Storage_Data) d = new Storage_Data;

    d->ClearErrorStatus();

    // Check file type
    if ( FSD_File::IsGoodFileType( aFileName ) != Storage_VSOk)  {
    ReturnMessage = "Bad file type for ";
    ReturnMessage += aFileName;
    ReturnMessage += " \n";
    return Standard_False;
    }

    // Open the archive, Read mode
    Storage_Error err = f.Open(aFileName, Storage_VSRead);
    // Read all the persistent object in the file with the schema
    if ( err != Storage_VSOk ) {
      ReturnMessage += BuildStorageErrorMessage(d->ErrorStatus());
      return Standard_False;
    }

    d = s->Read( f );
	err = d->ErrorStatus() ;

    if ( err != Storage_VSOk ) {
      ReturnMessage += BuildStorageErrorMessage(d->ErrorStatus());
      return Standard_False;
    }
    // Close the file driver
    f.Close();

    ReturnMessage += "Application Name :"; ReturnMessage += d->ApplicationName();ReturnMessage += "\n";
    ReturnMessage += "Application Version :"; ReturnMessage += d->ApplicationVersion();ReturnMessage += "\n";
    ReturnMessage += "Data type :"; ReturnMessage += d->DataType();ReturnMessage += "\n";
    ReturnMessage += "== User Infos : ==\n";
    const TColStd_SequenceOfAsciiString& UserInfo = d->UserInfo();
    for (int i=1;i<=UserInfo.Length();i++)
      {ReturnMessage += UserInfo(i);ReturnMessage += "\n";}
    ReturnMessage += "== Comments : ==\n";
    const TColStd_SequenceOfExtendedString& Comments=d->Comments();
    for ( int i=1;i<=Comments.Length();i++)
      {ReturnMessage += Comments(i);ReturnMessage += "\n";}
    ReturnMessage += "----------------\n";


    // Read all the root objects
    // Get the root list
    Handle(Storage_HSeqOfRoot)  roots = d->Roots();
    Handle(Standard_Persistent) p;
    Handle(Storage_Root) r;
    Handle(PTopoDS_HShape) aPShape;
    for ( int i = 1; i <= roots->Length() ; i++ ) 
     {
      // Get the root
      r = roots->Value(i);

      // Get the persistent application object from the root
      p = r->Object();

      // Display information
      ReturnMessage += "Persistent Object "; ReturnMessage += i; ReturnMessage += "\n";
      ReturnMessage += "Name             :"; ReturnMessage += r->Name(); ReturnMessage += "\n";
      ReturnMessage += "Type             :"; ReturnMessage += r->Type(); ReturnMessage += "\n";

      aPShape  = Handle(PTopoDS_HShape)::DownCast(p);

      if ( !aPShape.IsNull() ) 
        {
	      //   To Be  ReWriten to suppress the cout,
          //    and provide a CallBack method for dynamic information. 
	      // Get the persistent shape
	      PTColStd_PersistentTransientMap aMap;
	      TopoDS_Shape aTShape;
          MgtBRep::Translate(aPShape,aMap,aTShape,
		    MgtBRep_WithTriangle);
          aHSequenceOfShape->Append(aTShape);
        } 
	    else
	    {
		      ReturnMessage += "Error -> Unable to read\n";
	    } 
    } 
    return Standard_True;
}
//----------------------------------------------------------------------
void CImportExport::SaveCSFDB(const Handle(AIS_InteractiveContext)& anInteractiveContext)
{
    anInteractiveContext->InitCurrent();
	if (anInteractiveContext->NbCurrents() == 0){
		AfxMessageBox("No shape selected for export!");
		return;
	}
    Handle(Quantity_HArray1OfColor)   anArrayOfColors;
    Handle(TColStd_HArray1OfReal)     anArrayOfTransparencies;
	CImportExport::SaveCSFDB(BuildSequenceFromContext(anInteractiveContext, anArrayOfColors, anArrayOfTransparencies));
}

Standard_Boolean CImportExport::SaveCSFDB(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
  Standard_Boolean result = Standard_False; 
	CFileSaveCSFDBDialog aDlg(NULL);
	aDlg.m_TriangleMode = MgtBRep_WithTriangle;
	if (aDlg.DoModal() == IDOK) {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT)); 
        CString filename = aDlg.GetPathName(); 
        Standard_CString aFileName = (Standard_CString)(LPCTSTR)filename;
        TCollection_AsciiString Message;
		MgtBRep_TriangleMode selection = aDlg.m_TriangleMode;
        Standard_Boolean result = SaveCSFDB(aFileName,aHSequenceOfShape,Message);
	    if (result)
        {
            // Display The Message :
	        MessageBox(0,Message.ToCString(),"CasCade ",MB_OK);
        }
        else 
	        MessageBox(0,Message.ToCString(),"CasCade Error",MB_ICONERROR);
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); 
    } 
  return result;
}

Standard_Boolean CImportExport::SaveCSFDB(const Standard_CString& aFileName,
                                          const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape,
                                          TCollection_AsciiString& ReturnMessage,// out parameter
                                          MgtBRep_TriangleMode aTriangleMode /* = MgtBRep_WithTriangle */)
{
	Standard_Boolean ReturnValue = Standard_True;
    if (aHSequenceOfShape->Length() == 0)
    {
        MessageBox(0,"No Shape in the HSequence!!","CasCade Warning",MB_ICONWARNING);
        return Standard_False;
    }
 
    // an I/O driver
    FSD_File f;

    // the applicative Schema containing 
    // Pesistent Topology and Geometry
    Handle(ShapeSchema) s = new ShapeSchema;

    // a Read/Write data object
    Handle(Storage_Data) d = new Storage_Data;

    d->ClearErrorStatus();

    //   To Be  ReWriten to suppress the Strings,
    //    and provide a CallBack method for dynamic information. 

    d->SetApplicationName  (TCollection_ExtendedString("SampleImportExport"));
    d->SetApplicationVersion("1");
    d->SetDataType(TCollection_ExtendedString("Shapes"));
    d->AddToUserInfo("Try to store a Persistent set of Shapes in a flat file");
    d->AddToComments(TCollection_ExtendedString("application is based on CasCade 2.0"));

    // Open the archive, Write mode
    Storage_Error err = f.Open(aFileName, Storage_VSWrite);

    if ( err != Storage_VSOk ) {
      ReturnMessage += BuildStorageErrorMessage(err);
      return Standard_False;
    }

    PTColStd_TransientPersistentMap aMap;
    ReturnMessage += "The Object have be saved in the file ";
    ReturnMessage += aFileName;
    ReturnMessage += "\n with the names : ";

	for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)
	{
		TopoDS_Shape aTShape= aHSequenceOfShape->Value(i);
		TCollection_AsciiString anObjectName("anObjectName_");
		anObjectName += i;
		ReturnMessage += anObjectName;
		ReturnMessage += " \n";

		if ( aTShape.IsNull() ) 
		{
			ReturnMessage += " Error : Invalid shape \n";
			ReturnValue = Standard_False;
			continue;
		 }

		//Create the persistent Shape

		Handle(PTopoDS_HShape) aPShape = 
		 MgtBRep::Translate(aTShape, aMap, MgtBRep_WithTriangle);

 
		// Add the object in the data structure as root
		//   To Be  ReWriten to suppress the cout,
		//    and provide a CallBack method for dynamic information. 
		d->AddRoot(anObjectName, aPShape);
	}

    // Write the object in the file with the schema
    s->Write( f, d);

    // Close the driver
    f.Close();

    if ( d->ErrorStatus() != Storage_VSOk ) 
     {
        ReturnMessage += BuildStorageErrorMessage(d->ErrorStatus());
        return Standard_False;
     } 
    return ReturnValue;
}


//======================================================================
//=                                                                    =
//=                      IGES                                          =
//=                                                                    =
//======================================================================



void CImportExport::ReadIGES(const Handle(AIS_InteractiveContext)& anInteractiveContext)
{
    Handle(TopTools_HSequenceOfShape) aSequence = CImportExport::ReadIGES();
    for(int i=1;i<= aSequence->Length();i++)
        anInteractiveContext->Display(new AIS_Shape(aSequence->Value(i)));

}

Handle(TopTools_HSequenceOfShape) CImportExport::ReadIGES()// not by reference --> the sequence is created here !!
{
  CFileDialog dlg(TRUE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  "IGES Files (*.iges , *.igs)|*.iges; *.igs|All Files (*.*)|*.*||", 
                  NULL );

TCHAR tchBuf[80];

CString CASROOTValue = ((GetEnvironmentVariable("CASROOT", tchBuf, 80) > 0) ? tchBuf : NULL); 
CString initdir = (CASROOTValue + "\\..\\data\\iges");

dlg.m_ofn.lpstrInitialDir = initdir;
  
  Handle(TopTools_HSequenceOfShape) aSequence = new TopTools_HSequenceOfShape();
  if (dlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CString C = dlg.GetPathName();
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)C;
    Standard_Integer status = ReadIGES(aFileName,aSequence);
	if (status != IFSelect_RetDone)
    {
		   MessageBox(0,"Error : The file is not read","CasCade Error",MB_ICONERROR);
	}
	
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   }
  return aSequence;
}
//----------------------------------------------------------------------

Standard_Integer CImportExport::ReadIGES(const Standard_CString& aFileName,
                                         Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{

    IGESControl_Reader Reader;

    Standard_Integer status = Reader.ReadFile(aFileName);

    if (status != IFSelect_RetDone) return status;
    Reader.TransferRoots();
    TopoDS_Shape aShape = Reader.OneShape();     
	aHSequenceOfShape->Append(aShape);

    return status;
}
//----------------------------------------------------------------------

void CImportExport::SaveIGES(const Handle(AIS_InteractiveContext)& anInteractiveContext)
{
	anInteractiveContext->InitCurrent();
	if (anInteractiveContext->NbCurrents() == 0){
		AfxMessageBox("No shape selected for export!");
		return;
	}
    Handle(Quantity_HArray1OfColor)   anArrayOfColors;
    Handle(TColStd_HArray1OfReal)     anArrayOfTransparencies;
    CImportExport::SaveIGES(BuildSequenceFromContext(anInteractiveContext, anArrayOfColors, anArrayOfTransparencies));
}

Standard_Boolean CImportExport::SaveIGES(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
    if (aHSequenceOfShape->Length() == 0)
    {
        MessageBox(0,"No Shape in the HSequence!!","CasCade Warning",MB_ICONWARNING);
        return Standard_False;
    }

  CFileDialog dlg(FALSE,_T("*.iges"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	              "IGES Files (*.iges )|*.iges;|IGES Files (*.igs )| *.igs;||", NULL ); 

TCHAR tchBuf[80];

CString CASROOTValue = ((GetEnvironmentVariable("CASROOT", tchBuf, 80) > 0) ? tchBuf : NULL); 
CString initdir = (CASROOTValue + "\\..\\data\\iges");

dlg.m_ofn.lpstrInitialDir = initdir;
  
  Standard_Boolean result=Standard_False;
  if (dlg.DoModal() == IDOK)  
  { 
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT)); 
    CString C = dlg.GetPathName(); 
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)C; 


    result = SaveIGES(aFileName,aHSequenceOfShape);
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   }
    return result;
}
//----------------------------------------------------------------------

Standard_Boolean CImportExport::SaveIGES(const Standard_CString& aFileName,
                                         const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{

    IGESControl_Controller::Init();
	IGESControl_Writer ICW (Interface_Static::CVal("XSTEP.iges.unit"),
               Interface_Static::IVal("XSTEP.iges.writebrep.mode"));
	
	for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)  
		ICW.AddShape (aHSequenceOfShape->Value(i));			 

	ICW.ComputeModel();
	Standard_Boolean result = ICW.Write(aFileName );
    return result;
}

//======================================================================

//======================================================================
//=                                                                    =
//=                      STEP                                          =
//=                                                                    =
//======================================================================

void CImportExport::ReadSTEP(const Handle(AIS_InteractiveContext)& anInteractiveContext)
{
    Handle(TopTools_HSequenceOfShape) aSequence = CImportExport::ReadSTEP();
		if (!aSequence.IsNull()) {	
			for(int i=1;i<= aSequence->Length();i++)
        anInteractiveContext->Display(new AIS_Shape(aSequence->Value(i)), Standard_False);
		}
}

Handle(TopTools_HSequenceOfShape) CImportExport::ReadSTEP()// not by reference --> the sequence is created here !!
{
  CFileDialog dlg(TRUE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  "STEP Files (*.stp;*.step)|*.stp; *.step|All Files (*.*)|*.*||", 
                  NULL );

TCHAR tchBuf[80];

CString CASROOTValue = ((GetEnvironmentVariable("CASROOT", tchBuf, 80) > 0) ? tchBuf : NULL); 
CString initdir = (CASROOTValue + "\\..\\data\\step");

dlg.m_ofn.lpstrInitialDir = initdir;
  
  Handle(TopTools_HSequenceOfShape) aSequence= new TopTools_HSequenceOfShape();
  if (dlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CString C = dlg.GetPathName();
    Standard_CString aFileName = (Standard_CString)(LPCTSTR)C;
	IFSelect_ReturnStatus ReturnStatus = ReadSTEP(aFileName,aSequence);
    switch (ReturnStatus) 
    {
       case IFSelect_RetError :
           MessageBox(0,"Not a valid Step file","ERROR",MB_ICONWARNING);
       break;
       case IFSelect_RetFail :
           MessageBox(0,"Reading has failed","ERROR",MB_ICONWARNING);
       break;
       case IFSelect_RetVoid :
            MessageBox(0,"Nothing to transfer","ERROR",MB_ICONWARNING);
       break;
    }
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));       
  }
  return aSequence;
}

IFSelect_ReturnStatus CImportExport::ReadSTEP(const Standard_CString& aFileName,
                                              Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
  aHSequenceOfShape->Clear();

  // create additional log file
  STEPControl_Reader aReader;
  IFSelect_ReturnStatus status = aReader.ReadFile(aFileName);
  if (status != IFSelect_RetDone)
    return status;

  aReader.WS()->MapReader()->SetTraceLevel(2); // increase default trace level

  Standard_Boolean failsonly = Standard_False;
  aReader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity);

  // Root transfers
  Standard_Integer nbr = aReader.NbRootsForTransfer();
  aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);
  for ( Standard_Integer n = 1; n<=nbr; n++) {
    Standard_Boolean ok = aReader.TransferRoot(n);
  }

  // Collecting resulting entities
  Standard_Integer nbs = aReader.NbShapes();
  if (nbs == 0) {
    return IFSelect_RetVoid;
  }
  for (Standard_Integer i=1; i<=nbs; i++) {
    aHSequenceOfShape->Append(aReader.Shape(i));
  }

  return status;
}


//----------------------------------------------------------------------
void CImportExport::SaveSTEP(const Handle(AIS_InteractiveContext)& anInteractiveContext)
{
	anInteractiveContext->InitCurrent();
	if (anInteractiveContext->NbCurrents() == 0){
		AfxMessageBox("No shape selected for export!");
		return;
	}
    Handle(Quantity_HArray1OfColor)   anArrayOfColors;
    Handle(TColStd_HArray1OfReal)     anArrayOfTransparencies;
    CImportExport::SaveSTEP(BuildSequenceFromContext(anInteractiveContext, anArrayOfColors, anArrayOfTransparencies));
}

// Return True if no error
Standard_Boolean TestFacetedBrep(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
	Standard_Boolean OneErrorFound = Standard_False;
	for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)
	{
	  TopoDS_Shape aShape= aHSequenceOfShape->Value(i);

	  TopExp_Explorer Ex(aShape,TopAbs_FACE);
	  while (Ex.More() && !OneErrorFound)
		{
		// Get the 	Geom_Surface outside the TopoDS_Face
		Handle(Geom_Surface) aSurface = BRep_Tool::Surface(TopoDS::Face(Ex.Current()));
		// check if it is a plane.
		if (!aSurface->IsKind(STANDARD_TYPE(Geom_Plane)))
		    OneErrorFound=Standard_True;
		Ex.Next();
		}
	  TopExp_Explorer Ex2(aShape,TopAbs_EDGE);
	  while (Ex2.More() && !OneErrorFound)
		{
		// Get the 	Geom_Curve outside the TopoDS_Face
		Standard_Real FirstDummy,LastDummy;
		Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(Ex2.Current()),FirstDummy,LastDummy);
		// check if it is a line.
		if (!aCurve->IsKind(STANDARD_TYPE(Geom_Line)))
		    OneErrorFound=Standard_True;
		Ex2.Next();
		}
	}
	return !OneErrorFound;
}

IFSelect_ReturnStatus CImportExport::SaveSTEP(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
    if (aHSequenceOfShape->Length() == 0)
      {
        MessageBox(0,"No Shape in the HSequence!!","CasCade Warning",MB_ICONWARNING);
        return IFSelect_RetError;
      }

    IFSelect_ReturnStatus status;

	CFileSaveSTEPDialog aDlg(NULL);

	aDlg.m_Cc1ModelType = STEPControl_ManifoldSolidBrep;

	if (aDlg.DoModal() == IDOK) {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT)); 
        CString C = aDlg.GetPathName(); 
        Standard_CString aFileName = (Standard_CString)(LPCTSTR)C; 

		STEPControl_StepModelType selection = aDlg.m_Cc1ModelType;

        if(selection == STEPControl_FacetedBrep)

    	if (!TestFacetedBrep(aHSequenceOfShape))
	    {
            MessageBox(0,"At least one shape doesn't contain facetes","CasCade Warning",MB_ICONWARNING);
            return IFSelect_RetError;
	    }


        status =  SaveSTEP(aFileName,aHSequenceOfShape,selection);
        switch (status)
          {
            case IFSelect_RetError:
                MessageBox(0,"Incorrect Data","ERROR",MB_ICONWARNING); 
            break;
            case IFSelect_RetFail:
                MessageBox(0,"Writing has failed","ERROR",MB_ICONWARNING); 
            break;
            case IFSelect_RetVoid:
                MessageBox(0,"Nothing to transfer","ERROR",MB_ICONWARNING); 
            break;
          }
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); 
  } 
  return status;
}
//----------------------------------------------------------------------------------------
IFSelect_ReturnStatus CImportExport::SaveSTEP(const Standard_CString& aFileName,
                                              const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape,

const STEPControl_StepModelType aValue /* =TopoDSToCc1Act_ManifoldSolidBrep */ )

{
    // CREATE THE WRITER

    STEPControl_Writer aWriter;

	IFSelect_ReturnStatus status;
	for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)  
        {
			status =  aWriter.Transfer(aHSequenceOfShape->Value(i), aValue);
            if ( status != IFSelect_RetDone ) return status;
        }     
    status = aWriter.Write(aFileName);
    return status;
}



//======================================================================
//=                                                                    =
//=                      STL                                           =
//=                                                                    =
//======================================================================

void CImportExport::SaveSTL(const Handle(AIS_InteractiveContext)& anInteractiveContext)
{
    anInteractiveContext->InitCurrent();
	if (anInteractiveContext->NbCurrents() == 0){
		AfxMessageBox("No shape selected for export!");
		return;
	}
    Handle(Quantity_HArray1OfColor)   anArrayOfColors;
    Handle(TColStd_HArray1OfReal)     anArrayOfTransparencies;
	CImportExport::SaveSTL(BuildSequenceFromContext(anInteractiveContext, anArrayOfColors, anArrayOfTransparencies));
}

Standard_Boolean CImportExport::SaveSTL(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
  CFileDialog dlg(FALSE,_T("*.stl"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	              "stl Files (*.stl)|*.stl;|STL Files (*.STL)|*.STL;||", NULL ); 

TCHAR tchBuf[80];

CString CASROOTValue = ((GetEnvironmentVariable("CASROOT", tchBuf, 80) > 0) ? tchBuf : NULL); 
CString initdir = (CASROOTValue + "\\..\\data\\stl");

dlg.m_ofn.lpstrInitialDir = initdir;

	Standard_Boolean result;

	if (dlg.DoModal() == IDOK) {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT)); 
        CString C = dlg.GetPathName(); 
        Standard_CString aFileName = (Standard_CString)(LPCTSTR)C; 
        TCollection_AsciiString Message;
        result = SaveSTL(aFileName,aHSequenceOfShape,Message);
	    if (result)
        {
            // Display The Message :
	        MessageBox(0,Message.ToCString(),"CasCade ",MB_OK);
        }
        else 
	        MessageBox(0,Message.ToCString(),"CasCade Error",MB_ICONERROR);
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); 
    } 
  return result;
}

Standard_Boolean CImportExport::SaveSTL(const Standard_CString& aFileName,
                                          const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape,
                                          TCollection_AsciiString& ReturnMessage)
{
	Standard_Boolean ReturnValue = Standard_True;
    if (aHSequenceOfShape->Length() == 0)
    {
        MessageBox(0,"No Shape in the HSequence!!","CasCade Warning",MB_ICONWARNING);
        return Standard_False;
    }

    PTColStd_TransientPersistentMap aMap;
    ReturnMessage += "The Object have be saved in the file ";
    ReturnMessage += aFileName;
    ReturnMessage += "\n with the names : ";

	TopoDS_Compound RES;
	BRep_Builder MKCP;
	MKCP.MakeCompound(RES);

	for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)
	{
		TopoDS_Shape aShape= aHSequenceOfShape->Value(i);
		TCollection_AsciiString anObjectName("anObjectName_");
		anObjectName += i;
		ReturnMessage += anObjectName;
		ReturnMessage += " \n";

		if ( aShape.IsNull() ) 
		{
			ReturnMessage += " Error : Invalid shape \n";
			ReturnValue = Standard_False;
			continue;
		 }

		MKCP.Add(RES, aShape);
	}

	StlAPI_Writer myStlWriter;
	myStlWriter.Write(RES, aFileName);

    return ReturnValue;
}


//======================================================================
//=                                                                    =
//=                      VRML                                          =
//=                                                                    =
//======================================================================

void CImportExport::SaveVRML(const Handle(AIS_InteractiveContext)& anInteractiveContext)
{
   anInteractiveContext->InitCurrent();
	if (anInteractiveContext->NbCurrents() == 0){
		AfxMessageBox("No shape selected for export!");
		return;
	}
    Handle(Quantity_HArray1OfColor) anArrayOfColors;
    Handle(TColStd_HArray1OfReal)   anArrayOfTransparencies;
	CImportExport::SaveVRML(BuildSequenceFromContext(anInteractiveContext, anArrayOfColors, anArrayOfTransparencies),
                            anArrayOfColors, anArrayOfTransparencies);
}

Standard_Boolean CImportExport::SaveVRML(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape,
                                         const Handle(Quantity_HArray1OfColor)&   anArrayOfColors,
                                         const Handle(TColStd_HArray1OfReal)&     anArrayOfTransparencies)
{
  CFileDialog dlg(FALSE,_T("*.vrml"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	              "vrml Files (*.vrml)|*.vrml;|vrm Files (*.vrm)|*.vrm;||", NULL ); 

TCHAR tchBuf[80];

CString CASROOTValue = ((GetEnvironmentVariable("CASROOT", tchBuf, 80) > 0) ? tchBuf : NULL); 
CString initdir = (CASROOTValue + "\\..\\data\\vrml");

dlg.m_ofn.lpstrInitialDir = initdir;
  
  Standard_Boolean result;

	if (dlg.DoModal() == IDOK) {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT)); 
        CString C = dlg.GetPathName(); 
        Standard_CString aFileName = (Standard_CString)(LPCTSTR)C; 
        TCollection_AsciiString Message;
        result = SaveVRML(aFileName,aHSequenceOfShape,anArrayOfColors, anArrayOfTransparencies, Message);
	    if (result)
        {
            // Display The Message :
	        MessageBox(0,Message.ToCString(),"CasCade ",MB_OK);
        }
        else 
	        MessageBox(0,Message.ToCString(),"CasCade Error",MB_ICONERROR);
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); 
    } 
  return result;
}

Standard_Boolean CImportExport::SaveVRML(const Standard_CString&                  aFileName,
                                         const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape,
                                         const Handle(Quantity_HArray1OfColor)&   anArrayOfColors,
                                         const Handle(TColStd_HArray1OfReal)&     anArrayOfTransparencies,
                                         TCollection_AsciiString&                 ReturnMessage)
{
	Standard_Boolean ReturnValue = Standard_True;
    if (aHSequenceOfShape->Length() == 0)
    {
        MessageBox(0,"No Shape in the HSequence!!","CasCade Warning",MB_ICONWARNING);
        return Standard_False;
    }

    PTColStd_TransientPersistentMap aMap;
    ReturnMessage += "The Object has been saved in the file ";
    ReturnMessage += aFileName;
    ReturnMessage += "\n with the names : ";

    // VRML scene.
    VrmlData_Scene scene;
    VrmlData_ShapeConvert converter(scene/*, 0.001*/); // from mm to meters 
    Standard_Integer iShape = 1; // Counter of shapes

	for ( int i = 1; i <= aHSequenceOfShape->Length(); i++ )
	{
        // Shape
		TopoDS_Shape shape = aHSequenceOfShape->Value( i );
		if ( shape.IsNull() )
		{
			ReturnMessage += " Error : Invalid shape \n";
			ReturnValue = Standard_False;
			continue;
        }

        // Color
        Quantity_Color color; // yellow
        if (!anArrayOfColors.IsNull())
            color = anArrayOfColors->Value(i);

        // Transparency
        Standard_Real transparency = 0.0;
        if (!anArrayOfTransparencies.IsNull())
            transparency = anArrayOfTransparencies->Value(i);

        // Give a name to the shape.
        TCollection_AsciiString name("Shape");
        name += TCollection_AsciiString(iShape++);
        converter.AddShape(shape, name.ToCString());
        ReturnMessage += name;
        ReturnMessage += '\n';

        // Check presence of faces in the shape.
        TopExp_Explorer expl(shape, TopAbs_FACE);
        if (expl.More())
            converter.Convert(true, false, 0.01); // faces only
        else
            converter.Convert(false, true, 0.01); // edges only

        // Name of the color & transparency.
        // It will be uniquely saved in VRML file.
        TCollection_AsciiString cname = Quantity_Color::StringName(color.Name());
        cname += transparency;

        // Make the appearance (VRML attribute)
        Handle(VrmlData_Appearance) appearance = Handle(VrmlData_Appearance)::DownCast(scene.FindNode(cname.ToCString()));
        if (appearance.IsNull())
        {
            // Not found ... create a new one.
            Handle(VrmlData_Material) material = new VrmlData_Material(scene, cname.ToCString(), 0.2, 0.2, transparency);
            material->SetDiffuseColor(color);
            material->SetEmissiveColor(color);
            material->SetSpecularColor(color);
            scene.AddNode(material, false);
            appearance = new VrmlData_Appearance(scene, cname.ToCString());
            appearance->SetMaterial(material);
            scene.AddNode(appearance, false);
        }

        // Apply the material to the shape of entity.
        Handle(VrmlData_Group) group = Handle(VrmlData_Group)::DownCast(scene.FindNode(name.ToCString()));
        if (!group.IsNull())
        {
            VrmlData_ListOfNode::Iterator itr = group->NodeIterator();
            for (; itr.More(); itr.Next())
            {
                Handle(VrmlData_Node) node = itr.Value();
                if (node->DynamicType() == STANDARD_TYPE(VrmlData_ShapeNode))
                {
                    Handle(VrmlData_ShapeNode) shape = Handle(VrmlData_ShapeNode)::DownCast(node);
                    shape->SetAppearance(appearance);
                }
                else if (itr.Value()->DynamicType() == STANDARD_TYPE(VrmlData_Group))
                {
                    Handle(VrmlData_Group) groupc = Handle(VrmlData_Group)::DownCast(itr.Value());
                    VrmlData_ListOfNode::Iterator itrc = groupc->NodeIterator();
                    for (; itrc.More(); itrc.Next())
                    {
                        Handle(VrmlData_Node) nodec = itrc.Value();
                        if (nodec->DynamicType() == STANDARD_TYPE(VrmlData_ShapeNode))
                        {
                            Handle(VrmlData_ShapeNode) shapec = Handle(VrmlData_ShapeNode)::DownCast(nodec);
                            shapec->SetAppearance(appearance);
                        }
                    } // for of group nodes...
                } // if (it is a shape node...
            } // for of group nodes...
        } // if (!group.IsNull...
    } // iterator of shapes

    // Call VRML writer
    ofstream writer(aFileName);
    writer<<scene;
    writer.close();

    /* Old approach to store shapes in VRML (without color & transparency).
	TopoDS_Compound RES;
	BRep_Builder MKCP;
	MKCP.MakeCompound(RES);

	for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)
	{
		TopoDS_Shape aShape= aHSequenceOfShape->Value(i);
		TCollection_AsciiString anObjectName("anObjectName_");
		anObjectName += i;
		ReturnMessage += anObjectName;
		ReturnMessage += " \n";

		if ( aShape.IsNull() ) 
		{
			ReturnMessage += " Error : Invalid shape \n";
			ReturnValue = Standard_False;
			continue;
		 }

		MKCP.Add(RES, aShape);
	}

	VrmlAPI_Writer myVrmlWriter;
	myVrmlWriter.Write(RES, aFileName);
    */

    return ReturnValue;
}




