#include "StdAfx.h"
#include ".\occviewer.h"

#pragma warning( disable : 4800 )
OCCViewer::OCCViewer(void)
{
	myGraphicDevice=NULL;
	myViewer=NULL;
	myView=NULL;
	myAISContext=NULL;
}

OCCViewer::~OCCViewer(void)
{
	myView->Remove();
}

bool OCCViewer::InitViewer(void* wnd)
{
	try {
			myGraphicDevice = new  Graphic3d_WNTGraphicDevice();
		} catch (Standard_Failure) {
			return false;
		}
	TCollection_ExtendedString a3DName("Visu3D");
	myViewer = new V3d_Viewer( myGraphicDevice, a3DName.ToExtString(),"", 1000.0, 
									V3d_XposYnegZpos, Quantity_NOC_GRAY30,
									V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT, 
									Standard_True, Standard_False);

	myViewer->Init();
	myViewer->SetDefaultLights();
	myViewer->SetLightOn();
	myView = myViewer->CreateView();
	Handle(WNT_Window) aWNTWindow = new WNT_Window(myGraphicDevice, reinterpret_cast<HWND> (wnd));
	myView->SetWindow(aWNTWindow);
	if (!aWNTWindow->IsMapped()) 
		 aWNTWindow->Map();
	myAISContext = new AIS_InteractiveContext(myViewer);
	myAISContext->UpdateCurrentViewer();
	myView->Redraw();
	myView->MustBeResized();
	return true;

}

bool OCCViewer::ImportBRep(char* filename)
{
	Standard_CString aFileName = (Standard_CString) filename;
	TopoDS_Shape aShape;
    BRep_Builder aBuilder;
    Standard_Boolean result = BRepTools::Read(aShape,aFileName,aBuilder);
	if (!result)
		return false;
	if(myAISContext->HasOpenedContext())
		myAISContext->CloseLocalContext();
	myAISContext->Display(new AIS_Shape(aShape));
	return true;
}

void OCCViewer::UpdateView(void)
{
	if (!myView.IsNull())
		myView->MustBeResized();
}

void OCCViewer::RedrawView(void)
{
	if (!myView.IsNull())
		myView->Redraw();
}

void OCCViewer::SetDegenerateModeOn(void)
{
	if (!myView.IsNull())
		myView->SetDegenerateModeOn();
}

void OCCViewer::SetDegenerateModeOff(void)
{
	if (!myView.IsNull())
		myView->SetDegenerateModeOff();
}

void OCCViewer::WindowFitAll(int Xmin, int Ymin, int Xmax, int Ymax)
{
	if (!myView.IsNull())
		myView->WindowFitAll(Xmin, Ymin, Xmax, Ymax);
}

void OCCViewer::Place(int x, int y, float zoomFactor)
{
	Quantity_Factor aZoomFactor = zoomFactor;
	if (!myView.IsNull())
		myView->Place(x, y, aZoomFactor);
}

void OCCViewer::Zoom(int x1, int y1, int x2, int y2)
{
	if (!myView.IsNull())
		myView->Zoom(x1, y1, x2, y2);
}

void OCCViewer::Pan(int x, int y)
{
	if (!myView.IsNull())
		myView->Pan(x, y);
}

void OCCViewer::Rotation(int x, int y)
{
	if (!myView.IsNull())
		myView->Rotation(x, y);
}

void OCCViewer::StartRotation(int x, int y)
{
	if (!myView.IsNull())
		myView->StartRotation(x, y);
}

void OCCViewer::Select(int x1, int y1, int x2, int y2)
{
	if (!myAISContext.IsNull())
		myAISContext->Select(x1, y1, x2, y2, myView);
}

void OCCViewer::Select(void)
{
	if (!myAISContext.IsNull())
		myAISContext->Select();
}

void OCCViewer::MoveTo(int x, int y)
{
	if ((!myAISContext.IsNull()) && (!myView.IsNull()))
		myAISContext->MoveTo(x, y, myView);
}

void OCCViewer::ShiftSelect(int x1, int y1, int x2, int y2)
{
	if ((!myAISContext.IsNull()) && (!myView.IsNull()))
		myAISContext->ShiftSelect(x1, y1, x2, y2, myView);
}

void OCCViewer::ShiftSelect(void)
{
	if (!myAISContext.IsNull())
		myAISContext->ShiftSelect();
}

void OCCViewer::BackgroundColor(int& r, int& g, int& b)
{
	Standard_Real R1;
	Standard_Real G1;
	Standard_Real B1;
	if (!myView.IsNull())
		myView->BackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
	r = (int)R1*255;
	g = (int)G1*255;
	b = (int)B1*255;
}



void OCCViewer::UpdateCurrentViewer(void)
{
	if (!myAISContext.IsNull())
		myAISContext->UpdateCurrentViewer();
}

void OCCViewer::FrontView(void)
{
	if (!myView.IsNull())
		myView->SetProj(V3d_Xpos); 
}

void OCCViewer::TopView(void)
{
	if (!myView.IsNull())
		myView->SetProj(V3d_Zpos); 
}

void OCCViewer::LeftView(void)
{
	if (!myView.IsNull())
		myView->SetProj(V3d_Ypos);
}

void OCCViewer::BackView(void)
{
	if (!myView.IsNull())
		myView->SetProj(V3d_Xneg);
}

void OCCViewer::RightView(void)
{
	if (!myView.IsNull())
		myView->SetProj(V3d_Yneg);
}

void OCCViewer::BottomView(void)
{
	if (!myView.IsNull())
		myView->SetProj(V3d_Zneg); 
}

void OCCViewer::AxoView(void)
{
	if (!myView.IsNull())
		myView->SetProj(V3d_XposYnegZpos);
}

void OCCViewer::ZoomAllView(void)
{
	if (!myView.IsNull())
	{
		myView->FitAll();
		myView->ZFitAll();
	}
}

float OCCViewer::Scale(void)
{
	if (myView.IsNull())
		return -1;
	else
		return (float)myView->Scale();
}

void OCCViewer::ResetView(void)
{
	if (!myView.IsNull())
		myView->Reset();
}

void OCCViewer::SetDisplayMode(int aMode)
{
	if (myAISContext.IsNull())
		return;
	AIS_DisplayMode CurrentMode;
	if (aMode == 0) 
		CurrentMode=AIS_WireFrame;
	else
		CurrentMode=AIS_Shaded;
    if(myAISContext->NbCurrents()==0 || myAISContext->NbSelected()==0)
       myAISContext->SetDisplayMode(CurrentMode);
    else 
	{
       for(myAISContext->InitCurrent();myAISContext->MoreCurrent();myAISContext->NextCurrent())
	           myAISContext->SetDisplayMode(myAISContext->Current(),aMode,Standard_False);
	         
	}
	 myAISContext->UpdateCurrentViewer();
}

void OCCViewer::SetColor(int r, int g, int b)
{
	if (myAISContext.IsNull())
		return;
	Quantity_Color col =  Quantity_Color(r/255.,g/255.,b/255.,Quantity_TOC_RGB);
	for (;myAISContext->MoreCurrent ();myAISContext->NextCurrent ())
	          myAISContext->SetColor (myAISContext->Current(),col.Name());
}

void OCCViewer::ObjectColor(int& r, int& g, int& b)
{
	if (myAISContext.IsNull())
		return;
	r=255;
	g=255;
	b=255;
	Handle_AIS_InteractiveObject Current ;
	Quantity_Color ObjCol;
	myAISContext->InitCurrent();
	if (!myAISContext->MoreCurrent())
		return;
    Current = myAISContext->Current();
	if ( Current->HasColor () ) {
      ObjCol = myAISContext->Color(myAISContext->Current());
	  Quantity_Parameter r1, r2, r3;
	  ObjCol.Values(r1, r2, r3, Quantity_TOC_RGB);
		r=(int)r1*255;
		g=(int)r2*255;
		b=(int)r3*255;
  	}
}

void OCCViewer::SetBackgroundColor(int r, int g, int b)
{
	if (!myView.IsNull())
		myView->SetBackgroundColor(Quantity_TOC_RGB,r/255.,g/255.,b/255.);
}

void OCCViewer::EraseObjects(void)
{
	if (myAISContext.IsNull())
		return;
	for(myAISContext->InitCurrent();myAISContext->MoreCurrent();myAISContext->NextCurrent())
        myAISContext->Erase(myAISContext->Current(),Standard_True,Standard_False);
	myAISContext->ClearCurrents();
}

float OCCViewer::GetVersion(void)
{
	return (float)OCC_VERSION;
}

void OCCViewer::SetMaterial(int theMaterial)
{
	if (myAISContext.IsNull())
		return;
    for ( myAISContext->InitCurrent(); myAISContext->MoreCurrent (); myAISContext->NextCurrent () )
        myAISContext->SetMaterial( myAISContext->Current(), (Graphic3d_NameOfMaterial)theMaterial );
	myAISContext->UpdateCurrentViewer();
}

void OCCViewer::SetTransparency(int theTrans)
{
	if (myAISContext.IsNull())
		return;
	for( myAISContext->InitCurrent(); myAISContext->MoreCurrent(); myAISContext->NextSelected() )
		myAISContext->SetTransparency( myAISContext->Current(), ((Standard_Real)theTrans) / 10.0 );
}

bool OCCViewer::ImportCsfdb(char* filename)
{
	Standard_CString aFileName = (Standard_CString) filename;
    if ( FSD_File::IsGoodFileType(aFileName) != Storage_VSOk )
	    return false;

    static FSD_File fileDriver;
    TCollection_AsciiString aName( aFileName );
    if ( fileDriver.Open( aName, Storage_VSRead ) != Storage_VSOk )
        return false;

    Handle(ShapeSchema) schema = new ShapeSchema();
    Handle(Storage_Data) data  = schema->Read( fileDriver );
    if ( data->ErrorStatus() != Storage_VSOk )
        return false;
    fileDriver.Close();

    Handle(Storage_HSeqOfRoot) roots = data->Roots();
    for ( int i = 1; i <= roots->Length() ; i++ )
    {
        Handle(Storage_Root) r = roots->Value( i );
        Handle(Standard_Persistent) p = r->Object();
        Handle(PTopoDS_HShape) aPShape = Handle(PTopoDS_HShape)::DownCast(p);
        if ( !aPShape.IsNull() )
        {
	        PTColStd_PersistentTransientMap aMap;
	        TopoDS_Shape aTShape;
            MgtBRep::Translate( aPShape, aMap, aTShape, MgtBRep_WithTriangle );
			myAISContext->Display(new AIS_Shape(aTShape));
        }
    }

	return true;
}

bool OCCViewer::ImportIges(char* filename)
{
	Standard_CString aFileName = (Standard_CString) filename;
    IGESControl_Reader Reader;
    int status = Reader.ReadFile( aFileName );

    if ( status == IFSelect_RetDone )
    {
        Reader.TransferRoots();
        TopoDS_Shape aShape = Reader.OneShape();
        myAISContext->Display(new AIS_Shape(aShape));
    } else
		return false;
	myAISContext->UpdateCurrentViewer();
	return true;
}

bool OCCViewer::ImportStep(char* filename)
{
	Standard_CString aFileName = (Standard_CString) filename;
	STEPControl_Reader aReader;
	IFSelect_ReturnStatus status = aReader.ReadFile(aFileName);
	if ( status == IFSelect_RetDone )
    {
	    bool failsonly = false;
	    aReader.PrintCheckLoad( failsonly, IFSelect_ItemsByEntity );

	    int nbr = aReader.NbRootsForTransfer();
	    aReader.PrintCheckTransfer( failsonly, IFSelect_ItemsByEntity );
	    for ( Standard_Integer n = 1; n <= nbr; n++ )
	    {
	        Standard_Boolean ok = aReader.TransferRoot( n );
	        int nbs = aReader.NbShapes();
	        if ( nbs > 0 )
            {
	            for ( int i = 1; i <= nbs; i++ )
                {
		            TopoDS_Shape shape = aReader.Shape( i );
		            myAISContext->Display(new AIS_Shape(shape));
	            }
            }
        }
	} else
		return false;
	return true;
}

bool OCCViewer::ExportBRep(char* filename)
{
	myAISContext->InitCurrent();
	if (!myAISContext->MoreCurrent())
		return false;
	Handle_AIS_InteractiveObject anIO = myAISContext->Current();
	Handle_AIS_Shape anIS=Handle_AIS_Shape::DownCast(anIO);
	return (bool)BRepTools::Write( anIS->Shape(), (Standard_CString)filename ); ;
}


bool OCCViewer::ExportIges(char* filename)
{
	IGESControl_Controller::Init();
	IGESControl_Writer writer( Interface_Static::CVal( "XSTEP.iges.unit" ),
                               Interface_Static::IVal( "XSTEP.iges.writebrep.mode" ) );
 
	
	for ( myAISContext->InitCurrent(); myAISContext->MoreCurrent(); myAISContext->NextCurrent() )
	{
		Handle_AIS_InteractiveObject anIO = myAISContext->Current();
		Handle_AIS_Shape anIS=Handle_AIS_Shape::DownCast(anIO);
		TopoDS_Shape shape = anIS->Shape();
		writer.AddShape ( shape );
	}
	writer.ComputeModel();
	return (bool)writer.Write( (Standard_CString)filename );
}

bool OCCViewer::ExpotStep(char* filename)
{
    STEPControl_StepModelType type = STEPControl_AsIs;
    IFSelect_ReturnStatus status;
    STEPControl_Writer writer;
	for ( myAISContext->InitCurrent(); myAISContext->MoreCurrent(); myAISContext->NextCurrent() )
    {
		Handle_AIS_InteractiveObject anIO = myAISContext->Current();
		Handle_AIS_Shape anIS=Handle_AIS_Shape::DownCast(anIO);
		TopoDS_Shape shape = anIS->Shape();
		status = writer.Transfer( shape , type );
        if ( status != IFSelect_RetDone )
            return false;
    }

    status = writer.Write( (Standard_CString)filename );
    if ( status != IFSelect_RetDone )
            return false;
	return true;
}

bool OCCViewer::ExportStl(char* filename)
{
	TopoDS_Compound comp;
	BRep_Builder builder;
	builder.MakeCompound( comp );

	for ( myAISContext->InitCurrent(); myAISContext->MoreCurrent(); myAISContext->NextCurrent() )
	{
		Handle_AIS_InteractiveObject anIO = myAISContext->Current();
		Handle_AIS_Shape anIS=Handle_AIS_Shape::DownCast(anIO);
		TopoDS_Shape shape = anIS->Shape();
		if ( shape.IsNull() ) 
			return false;    
		builder.Add( comp, shape );
	}

	StlAPI_Writer writer;
	writer.Write( comp, (Standard_CString)filename );
	return true;
}

bool OCCViewer::ExportVrml(char* filename)
{
	TopoDS_Compound res;
	BRep_Builder builder;
	builder.MakeCompound( res );

	for ( myAISContext->InitCurrent(); myAISContext->MoreCurrent(); myAISContext->NextCurrent() )
	{
		Handle_AIS_InteractiveObject anIO = myAISContext->Current();
		Handle_AIS_Shape anIS=Handle_AIS_Shape::DownCast(anIO);
		TopoDS_Shape shape = anIS->Shape();
		if ( shape.IsNull() )
			return false;
        
		builder.Add( res, shape );
	}

	VrmlAPI_Writer writer;
	writer.Write( res, (Standard_CString)filename );

	return true;
}

bool OCCViewer::Dump(char *filename)
{
	if (myView.IsNull())
		return false;
	myView->Redraw();
    return (bool)myView->Dump(filename);
}

bool OCCViewer::IsObjectSelected(void)
{
	if (myAISContext.IsNull())
		return false;
	myAISContext->InitCurrent();
	return (bool)myAISContext->MoreCurrent();
}

int OCCViewer::DisplayMode(void)
{
	if (myAISContext.IsNull())
		return -1;
	int mode = -1;
	bool OneOrMoreInShading=false;
	bool OneOrMoreInWireframe=false;
	for (myAISContext->InitCurrent(); myAISContext->MoreCurrent(); myAISContext->NextCurrent())
	{
		if ( myAISContext->IsDisplayed( myAISContext->Current(), 1 ) )
             OneOrMoreInShading = true;
        if ( myAISContext->IsDisplayed( myAISContext->Current(), 0 ) )
             OneOrMoreInWireframe = true;
	}
	if (OneOrMoreInShading&&OneOrMoreInWireframe)
		mode=10;
	else if(OneOrMoreInShading)
		mode=1;
	else if (OneOrMoreInWireframe)
		mode=0;
	return mode;
}

void OCCViewer::CreateNewView(void* wnd)
{
	if (myAISContext.IsNull())
		return;
	myView = myAISContext->CurrentViewer()->CreateView();
	if (myGraphicDevice.IsNull())
			myGraphicDevice = new  Graphic3d_WNTGraphicDevice();
	Handle(WNT_Window) aWNTWindow = new WNT_Window(myGraphicDevice, reinterpret_cast<HWND> (wnd));
	myView->SetWindow(aWNTWindow);
	Standard_Integer w=100, h=100;
	aWNTWindow->Size(w,h);
	if (!aWNTWindow->IsMapped()) 
		 aWNTWindow->Map();
}

bool OCCViewer::SetAISContext(OCCViewer* Viewer)
{
	this->myAISContext=Viewer->GetAISContext();
	if (myAISContext.IsNull())
		return false;
	return true;
}

Handle_AIS_InteractiveContext OCCViewer::GetAISContext(void)
{
	return myAISContext;
}

int OCCViewer::CharToInt(char symbol)
{
	TCollection_AsciiString msg = symbol;
	return msg.IntegerValue();
}
