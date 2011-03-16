// ViewDlg.h: interface for the ViewDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWDLG_H__624EA574_FAC0_11D5_A986_0001021E946C__INCLUDED_)
#define AFX_VIEWDLG_H__624EA574_FAC0_11D5_A986_0001021E946C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qdialog.h>

#include <V3d_View.hxx>

class QCheckBox;
class ViewDlg : public QDialog  
{
    Q_OBJECT

public:
	ViewDlg(QWidget* parent/*, Handle(V3d_View)& view*/);
	virtual ~ViewDlg();
public slots:
	void Update();
protected:
    void        showEvent ( QShowEvent* );

private slots:
    void        onOk();
    void        onCancel();
    void        onHelp();
    void        onZBuffer( bool );
    void        onAnimation( bool );
    void        onDegeneration( bool );
    void        onAutoApply( bool );
    void        onApplyAllViews( bool );

private:
    Standard_Boolean    myAutoApply;
	Standard_Boolean    myApplyAllViews;
//    Handle_V3d_View     myView;
    QCheckBox*          myAnimBox;
    QCheckBox*          myDegBox;
	QCheckBox*          myZBufferBox;
    Standard_Boolean    myIsAnim;
    Standard_Boolean    myIsDeg;
};

#endif
