#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <qobject.h>
#include <AIS_InteractiveContext.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <Storage_Error.hxx>
#include <TCollection_AsciiString.hxx>
#include "MgtBRep_TriangleMode.hxx"
#include <STEPControl_StepModelType.hxx>

class Translate: public QObject
{
	Q_OBJECT
public:
    Translate(QObject* parent);
	~Translate();

	void        importBREP( const Handle(AIS_InteractiveContext) );


	void        importBREP( const Handle(AIS_InteractiveContext), const QString& );

	bool        importBREP( const Handle(AIS_InteractiveContext), const Standard_CString );

private :
    QString     selectFileName( const QString& , bool isImport ) const;

    static Handle(TopTools_HSequenceOfShape) 
	            BuildSequenceFromContext( const Handle(AIS_InteractiveContext)& );
};

#endif
