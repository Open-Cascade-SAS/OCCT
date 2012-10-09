#ifndef _VIEWER_H_
#define _VIEWER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QWidget.h>

#include <AIS_InteractiveContext.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_View.hxx>
#include <OpenGl_View.hxx>

#include <Voxel_Prs.hxx>
#include <Voxel_Selector.hxx>

class Viewer : public QWidget 
{
	
	Q_OBJECT

public:

	Viewer(QWidget* parent);
	virtual ~Viewer();

	Handle(AIS_InteractiveContext) getIC() const { return myIC; }
	Handle(V3d_View) getView() const { return myView; }
    Handle(OpenGl_GraphicDriver) getGraphicDriver() const { return myGraphicDriver; }
    Voxel_Selector& getSelector() { return mySelector; }
    void setPrs(const Handle(Voxel_Prs)& prs) { myPrs = prs; }
	virtual QPaintEngine*          paintEngine() const;

signals:
	
	void mousePressed(Qt::KeyboardModifiers,int,int);
	void mouseMoved(Qt::KeyboardModifiers,int,int);
	void mouseReleased(Qt::KeyboardModifiers,int,int);
	void mouseDoubleClick(Qt::KeyboardModifiers,int,int);

protected:
	virtual void paintEvent(QPaintEvent* pEvent);
	virtual void resizeEvent(QResizeEvent* rsEvent);
	virtual void mousePressEvent(QMouseEvent* mpEvent);
	virtual void mouseMoveEvent(QMouseEvent* mmEvent);
	virtual void mouseReleaseEvent(QMouseEvent* mrEvent);
	virtual void mouseDoubleClickEvent(QMouseEvent* mdcEvent);

private:
	Handle(AIS_InteractiveContext) myIC;
	Handle(V3d_View)               myView;
    Handle(OpenGl_GraphicDriver)   myGraphicDriver;

	bool myRotate;
	bool myZoom;
	bool myPan;

	QPoint myStartPnt;

    void setDegenerateMode(const bool );

    Voxel_Selector mySelector;
    Handle(Voxel_Prs) myPrs;
};

#endif // _VIEWER_H_
