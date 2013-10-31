#ifndef MDIWINDOW_H
#define MDIWINDOW_H

#include <QMainWindow>
#include "CommonSample.h"

class DocumentCommon;
class View;

class COMMONSAMPLE_EXPORT MDIWindow: public QMainWindow
{
    Q_OBJECT

public:
  MDIWindow( DocumentCommon* aDocument, QWidget* parent, Qt::WindowFlags wflags, bool theRT = false );
  MDIWindow( View* aView, DocumentCommon* aDocument, QWidget* parent, Qt::WindowFlags wflags );
  ~MDIWindow();

	DocumentCommon*            getDocument();
	void                       fitAll();
  virtual QSize              sizeHint() const;

#ifdef HAVE_OPENCL

  void                       setRaytracedShadows( int state );
  void                       setRaytracedReflections( int state );
  void                       setRaytracedAntialiasing( int state );

  bool                       ShadowsEnabled() { return myShadowsEnabled; }
  bool                       ReflectionsEnabled() { return myReflectionsEnabled; }
  bool                       AntialiasingEnabled() { return myAntialiasingEnabled; }

#endif

signals:
  void                       selectionChanged();
  void                       message(const QString&, int );
	void                       sendCloseView(MDIWindow* theView);

public slots:
  void                       closeEvent(QCloseEvent* e);        
  void                       onWindowActivated ();
  void                       dump();

protected:
  void                       createViewActions();

protected:
  DocumentCommon*            myDocument;
  View*                      myView;

#ifdef HAVE_OPENCL

  bool                       myShadowsEnabled;
  bool                       myReflectionsEnabled;
  bool                       myAntialiasingEnabled;

#endif
};

#endif

