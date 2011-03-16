// File:	SampleImportExportPackage.cxx
// Created:	Tue Nov 23 10:34:43 1999
// Author:	UI team
//		<ui@zamox.nnov.matra-dtv.fr>


#include <SampleImportExportPackage.ixx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <FSD_File.hxx>
#include <ShapeSchema.hxx>
#include <Storage_Data.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Standard_Persistent.hxx>
#include <Storage_Root.hxx>
#include <PTopoDS_HShape.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <MgtBRep.hxx>
#include <Interface_Static.hxx>
#include <STEPControl_Controller.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <Aspect_Window.hxx>
#include <V3d_View.hxx>

#ifdef WNT
#include <WNT_Window.hxx>
#include <WNT_GraphicDevice.hxx>
#include <WNT_WDriver.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <Xw_Window.hxx>
#include <Xw_GraphicDevice.hxx>
#include <Xw_Driver.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif




/*----------------------------------------------------------------------*/
#ifdef WNT
static Handle(Graphic3d_WNTGraphicDevice) defaultDevice;
static Handle(WNT_GraphicDevice) default2dDevice;
#else
static Handle(Graphic3d_GraphicDevice) defaultDevice;
static Handle(Xw_GraphicDevice) default2dDevice;
#endif 



//===============================================================
// Function name: CreateViewer3d
//===============================================================
 Handle(V3d_Viewer) SampleImportExportPackage::CreateViewer3d(const Standard_ExtString aName) 
{
#ifdef WNT
  if (defaultDevice.IsNull()) 
    defaultDevice = new Graphic3d_WNTGraphicDevice();
  return new V3d_Viewer(defaultDevice, aName);
#else
  if (defaultDevice.IsNull()) 
    defaultDevice = new Graphic3d_GraphicDevice("");
  return new V3d_Viewer(defaultDevice, aName);
#endif //WNT
}

//===============================================================
// Function name: SetWindow3d
//===============================================================
void SampleImportExportPackage::SetWindow3d (const Handle(V3d_View)& aView,
					     const Standard_Integer hiwin,
					     const Standard_Integer lowin)
{
#ifdef WNT
  Handle(Graphic3d_WNTGraphicDevice) d = 
    Handle(Graphic3d_WNTGraphicDevice)::DownCast(aView->Viewer()->Device());
  Handle(WNT_Window) w = new WNT_Window(d,hiwin,lowin);
#else
  Handle(Graphic3d_GraphicDevice) d = 
    Handle(Graphic3d_GraphicDevice)::DownCast(aView->Viewer()->Device());
  Handle(Xw_Window) w = new Xw_Window(d,hiwin,lowin,Xw_WQ_3DQUALITY);
#endif
  aView->SetWindow(w);
}


/*----------------------------------------------------------------------*/

Handle(TopTools_HSequenceOfShape) BuildSequenceFromContext(const Handle(AIS_InteractiveContext)& aContext) 
{
  Handle(TopTools_HSequenceOfShape) aSequence = new TopTools_HSequenceOfShape();
  Handle(AIS_InteractiveObject) picked;
  for(aContext->InitCurrent();aContext->MoreCurrent();aContext->NextCurrent())
    {
      picked = aContext->Current();
      if (aContext->Current()->IsKind(STANDARD_TYPE(AIS_Shape)))
	{
	  TopoDS_Shape aShape = Handle(AIS_Shape)::DownCast(picked)->Shape();
          aSequence->Append(aShape);
	}
    }
  return aSequence;
}


//======================================================================
//=                                                                    =
//=                      BREP                                          =
//=                                                                    =
//======================================================================

 Standard_Boolean SampleImportExportPackage::ReadBREP(const Standard_CString aFileName,
						      const Handle(AIS_InteractiveContext)& aContext) 
{
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;
  Standard_Boolean result = BRepTools::Read(aShape,aFileName,aBuilder);
  if (result) 
    aContext->Display(new AIS_Shape(aShape));
  return result;
}

 Standard_Boolean SampleImportExportPackage::SaveBREP(const Standard_CString aFileName,
						      const Handle(AIS_InteractiveContext)& aContext) 
{
  Handle(TopTools_HSequenceOfShape) aSequence = BuildSequenceFromContext(aContext);

  if (aSequence->Length() == 0)
    return Standard_False;

  TopoDS_Shape aShape = aSequence->Value(1);

  Standard_Boolean result = BRepTools::Write(aShape,aFileName); 
  return result;
}


//======================================================================
//=                                                                    =
//=                      CSFDB                                         =
//=                                                                    =
//======================================================================
TCollection_AsciiString BuildStorageErrorMessage (Storage_Error anError)
{ 
  TCollection_AsciiString aMessage("Storage Status :");
  switch ( anError ) 
    {
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


 Standard_Boolean SampleImportExportPackage::ReadCSFDB(const Standard_CString aFileName,
						       const Handle(AIS_InteractiveContext)& aContext,
						       TCollection_AsciiString& ReturnMessage) 
{
  Handle(TopTools_HSequenceOfShape) aSequence = new TopTools_HSequenceOfShape();
  Standard_Integer i;

  // an I/O driver
  FSD_File f;

  // the applicative Schema
  Handle(ShapeSchema) s = new ShapeSchema;

  // a Read/Write data object
  Handle(Storage_Data) d = new Storage_Data;

  d->ClearErrorStatus();

  // Check file type
  if (FSD_File::IsGoodFileType(aFileName) != Storage_VSOk)  {
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
  for (i=1; i<=UserInfo.Length(); i++)
    {ReturnMessage += UserInfo(i);ReturnMessage += "\n";}
  ReturnMessage += "== Comments : ==\n";
  const TColStd_SequenceOfExtendedString& Comments=d->Comments();
  for (i=1; i<=Comments.Length(); i++)
    {ReturnMessage += Comments(i);ReturnMessage += "\n";}
  ReturnMessage += "----------------\n";


  // Read all the root objects
  // Get the root list
  Handle(Storage_HSeqOfRoot)  roots = d->Roots();
  Handle(Standard_Persistent) p;
  Handle(Storage_Root) r;
  Handle(PTopoDS_HShape) aPShape;
  for (i = 1; i <= roots->Length(); i++ ) 
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

      if (!aPShape.IsNull()) 
        {
	  //   To Be  ReWriten to suppress the cout,
          //    and provide a CallBack method for dynamic information. 
	  // Get the persistent shape
	  PTColStd_PersistentTransientMap aMap;
	  TopoDS_Shape aTShape;
          MgtBRep::Translate(aPShape,aMap,aTShape,
			     MgtBRep_WithTriangle);
          aSequence->Append(aTShape);
        } 
      else
	{
	  ReturnMessage += "Error -> Unable to read\n";
	} 
    }

  // Display shapes
  for(i=1; i<=aSequence->Length(); i++)
    aContext->Display(new AIS_Shape(aSequence->Value(i)));

  return Standard_True;
}


 Standard_Boolean SampleImportExportPackage::SaveCSFDB(const Standard_CString aFileName,
						       const Handle(AIS_InteractiveContext)& aContext,
						       TCollection_AsciiString& ReturnMessage,
						       const MgtBRep_TriangleMode aTriangleMode) 
{
  Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = BuildSequenceFromContext(aContext);
  Standard_Integer ReturnValue = Standard_True;
  if (aHSequenceOfShape->Length() == 0)
    return Standard_False;

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

  d->SetApplicationName(TCollection_ExtendedString("SampleImportExport"));
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
	MgtBRep::Translate(aTShape, aMap, aTriangleMode);

 
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
//=                      STEP                                          =
//=                                                                    =
//======================================================================
 IFSelect_ReturnStatus SampleImportExportPackage::ReadSTEP(const Standard_CString aFileName,
							   const Handle(AIS_InteractiveContext)& aContext) 
{
  Handle(TopTools_HSequenceOfShape) aSequence = new TopTools_HSequenceOfShape();
  TopoDS_Shape aShape;
  STEPControl_Controller::Init();
  STEPControl_Reader aReader;
  IFSelect_ReturnStatus status = aReader.ReadFile(aFileName);

  if (status == IFSelect_RetDone)
    {
      Standard_Boolean failsonly = Standard_False;
      aReader.PrintCheckLoad (failsonly, IFSelect_ItemsByEntity);
      // Root transfers
      Standard_Integer nbr = aReader.NbRootsForTransfer();
      aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);
      for ( Standard_Integer n = 1; n<= nbr; n++) 
	{
	  aReader.TransferRoot(n);
	  // Collecting resulting entities
	  Standard_Integer nbs = aReader.NbShapes();
	  if (nbs == 0) 
            {
	      aSequence.Nullify();
	      return IFSelect_RetVoid;
            }
	  else 
	    {
	      for (Standard_Integer i =1; i<=nbs; i++) 
		{
		  aShape=aReader.Shape(i);
		  aSequence->Append(aShape);
		}
	    }
	}
    }
  else
    {
      aSequence.Nullify();
    }

  // Display shapes
  if (!aSequence.IsNull()) {	
    for(int i=1;i<= aSequence->Length();i++)
      aContext->Display(new AIS_Shape(aSequence->Value(i)));
  }

  return status;
}

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


 IFSelect_ReturnStatus SampleImportExportPackage::SaveSTEP(const Standard_CString aFileName,
							   const Handle(AIS_InteractiveContext)& aContext,
							   const STEPControl_StepModelType aValue) 
{
  Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = BuildSequenceFromContext(aContext);

  if (aHSequenceOfShape->Length() == 0)
    return IFSelect_RetError;

  if (aValue == STEPControl_FacetedBrep)
    if (!TestFacetedBrep(aHSequenceOfShape))
      return IFSelect_RetError;

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
//=                      IGES                                          =
//=                                                                    =
//======================================================================
 Standard_Integer SampleImportExportPackage::ReadIGES(const Standard_CString aFileName,
						      const Handle(AIS_InteractiveContext)& aContext) 
{
  Handle(TopTools_HSequenceOfShape) aSequence = new TopTools_HSequenceOfShape();
  IGESControl_Reader Reader;
  Standard_Integer status = Reader.ReadFile(aFileName);
  
  if (status != IFSelect_RetDone) return status;
  Reader.TransferRoots();
  TopoDS_Shape aShape = Reader.OneShape();     
  aSequence->Append(aShape);

  // Display shapes
  for(int i=1;i<= aSequence->Length();i++)
    aContext->Display(new AIS_Shape(aSequence->Value(i)));

  return status;
}


 Standard_Boolean SampleImportExportPackage::SaveIGES(const Standard_CString aFileName,
						      const Handle(AIS_InteractiveContext)& aContext) 
{
  Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = BuildSequenceFromContext(aContext);

  if (aHSequenceOfShape->Length() == 0)
    return Standard_False;

  IGESControl_Controller::Init();
  IGESControl_Writer ICW (Interface_Static::CVal("XSTEP.iges.unit"),
			     Interface_Static::IVal("XSTEP.iges.writebrep.mode"));
  
  for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)  
    ICW.AddShape (aHSequenceOfShape->Value(i));
  
  ICW.ComputeModel();
  Standard_Boolean result = ICW.Write(aFileName);
  return result;
}


//===============================================================
// Function name: SaveImage
//===============================================================
#ifndef WNT
 Standard_Boolean SampleImportExportPackage::SaveImage(const Standard_CString ,
						       const Standard_CString ,
						       const Handle(V3d_View)& ) {
#else 
 Standard_Boolean SampleImportExportPackage::SaveImage(const Standard_CString aFileName,
						       const Standard_CString aFormat,
						       const Handle(V3d_View)& aView) 
{
  Handle(Aspect_Window) anAspectWindow = aView->Window();
  Handle(WNT_Window) aWNTWindow = Handle(WNT_Window)::DownCast(anAspectWindow);

  if (aFormat == "bmp") aWNTWindow->SetOutputFormat(WNT_TOI_BMP);
  if (aFormat == "gif") aWNTWindow->SetOutputFormat(WNT_TOI_GIF);
  if (aFormat == "xwd") aWNTWindow->SetOutputFormat(WNT_TOI_XWD);

  aWNTWindow->Dump(aFileName);
#endif
  return Standard_True;
}

