#ifndef APPLICATION_H
#define APPLICATION_H

#include "ApplicationCommon.h"
#include "IESample.h"

class Translate;

class IESAMPLE_EXPORT ApplicationWindow: public ApplicationCommonWindow
{
    Q_OBJECT

public:
  
  enum { FileImportBREPId=0, FileExportBREPId=1, FileImportIGESId=2,
         FileExportIGESId=3, FileImportSTEPId=4, FileExportSTEPId=5,
         FileExportSTLId=6,  FileExportVRMLId=7, FileUserId };
	
  ApplicationWindow();
  ~ApplicationWindow();

  static QString                  getIEResourceDir();

  virtual void                    updateFileActions();
  
public slots:
  void                            onImport();
  void                            onExport();
  void                            onExportImage();
  virtual void                    onSelectionChanged();
  
protected:
  virtual int                     translationFormat( const QAction* );
  virtual Translate*              createTranslator();

private:
  void                            createTranslatePopups();
  bool                            translate( const int, const bool );

protected:
  QList<QAction*>                 myCasCadeTranslateActions;
  QMenu*                          myImportPopup;
  QMenu*                          myExportPopup;
  QAction*                        mySeparator;
};

#endif


