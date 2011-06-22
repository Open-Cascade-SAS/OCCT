#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qapplication.h>

#include "Translate.h"
#include "Application.h"
#include "Document.h"
#include "MDIWindow.h"
#include "global.h"

#include <StlAPI_Writer.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <TopoDS_Compound.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>


// STL
//#include <Mesh_InteractiveObject.hxx>
//#include <Mesh_STLRead.hxx>
//#include <Mesh_Drawer.hxx>
//#include <Mesh_Model.hxx>

#ifdef a
#include "OSD_Timer.hxx"
#include <FSD_File.hxx>
#include <ShapeSchema.hxx>
#include <Storage_Data.hxx>
#include <PTopoDS_HShape.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Storage_Root.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <MgtBRep.hxx>
#include <TCollection_ExtendedString.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <Interface_Static.hxx>
#include <STEPControl_Reader.hxx>
#include <Interface_TraceFile.hxx>
#include <STEPControl_Writer.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <VrmlAPI_Writer.hxx>
#endif
  
Translate::Translate( QObject* parent ):
QObject( parent )
{
}

Translate::~Translate()
{
}

/*!
    Selects a file from standard dialog acoording to selection 
    'filter'
*/
QString Translate::selectFileName( const QString& filter, bool isImport ) const                                          
{
  return isImport ? 
    QFileDialog::getOpenFileName( QApplication::activeWindow(), tr("INF_APP_IMPORT"), QString::null, filter ) : 
    QFileDialog::getSaveFileName( QApplication::activeWindow(), tr("INF_APP_EXPORT"), QString::null, filter );
    /*
    QFileDialog fd ( 0, 0, true );     
    fd.setFilters( filter );
	if(isImport)
	    fd.setCaption ( tr("INF_APP_IMPORT") );
    int ret = fd.exec(); 
    
    qApp->processEvents();

    return ( ret == QDialog::Accepted ? fd.selectedFile() : QString::null );
    */
}

Handle(TopTools_HSequenceOfShape) 
Translate::BuildSequenceFromContext(const Handle(AIS_InteractiveContext)& cxt)
{
    Handle(TopTools_HSequenceOfShape) sequence = new TopTools_HSequenceOfShape();
    Handle(AIS_InteractiveObject) object;
    AIS_ListOfInteractive displayed;
    cxt->DisplayedObjects( displayed );

    AIS_ListIteratorOfListOfInteractive it( displayed );
    for ( ; it.More(); it.Next() ) {
        object = it.Value();
        if ( object->IsKind( STANDARD_TYPE( AIS_Shape ) ) ) {
	       TopoDS_Shape shape = Handle(AIS_Shape)::DownCast( object )->Shape();
           sequence->Append( shape );
	     }
    }
    
#ifdef OLD_CODE
    for(anInteractiveContext->InitCurrent();anInteractiveContext->MoreCurrent();anInteractiveContext->NextCurrent())
      {
        picked = anInteractiveContext->Current();
        if (anInteractiveContext->Current()->IsKind(STANDARD_TYPE(AIS_Shape)))
	     {
	       TopoDS_Shape aShape = Handle(AIS_Shape)::DownCast(picked)->Shape();
           aSequence->Append(aShape);
	     }
      }
#endif //OLD_CODE
      return sequence;
}

void Translate::importBREP(const Handle(AIS_InteractiveContext) theContext)
{
	static QString filter = "BREP Files (*.brep )";
	importBREP(theContext,filter);
}

void Translate::importBREP(const Handle(AIS_InteractiveContext) theContext, const QString& filter)
{
	QString file = selectFileName( filter, true );
    if ( !file.isNull() ) {
        QApplication::setOverrideCursor( Qt::WaitCursor );
		if(!importBREP(theContext, (const Standard_CString) file.toLatin1().constData())) {
  		        QApplication::restoreOverrideCursor();                
              QMessageBox::information ( QApplication::activeWindow(),tr("TIT_ERROR"), tr("INF_TRANSLATE_ERROR"), tr("BTN_OK"),
				QString::null, QString::null, 0, 0);
			qApp->processEvents();  /* update desktop */	
		} else 
		  QApplication::restoreOverrideCursor();                
	}
}

bool Translate::importBREP(const Handle(AIS_InteractiveContext) theContext, const Standard_CString theFileName)
{
	Handle(TopTools_HSequenceOfShape) aSequence = new TopTools_HSequenceOfShape();
    TopoDS_Shape aShape;
	BRep_Builder aBuilder;
	
    Application::startTimer();
	Standard_Boolean result = BRepTools::Read(aShape,theFileName,aBuilder);
    Application::stopTimer( 0, "Loading BREP file" );

	if(result)
		aSequence->Append(aShape);

    int curMode = 
        ( (MDIWindow*)Application::getApplication()->getActiveMDI() )->getDisplayMode();

    for(int i=1;i<= aSequence->Length();i++) {

        Application::startTimer();
        Handle(AIS_Shape) shape = new AIS_Shape( aSequence->Value( i ) );
        Application::stopTimer( 0, "Build shape" );
        Application::startTimer();
		theContext->Display( shape, curMode, 0, false );
        Application::stopTimer( 0, "Display" );
    }
    Application::startTimer();
    theContext->UpdateCurrentViewer();
    Application::stopTimer( 0, "Update" );

	return result;
}

