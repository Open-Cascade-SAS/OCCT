#ifndef MATERIAL_H
#define MATERIAL_H

#include <QDialog>
#include <QList>
#include <QPushButton>

#include <AIS_InteractiveContext.hxx>

class DialogMaterial : public QDialog
{
	Q_OBJECT
public:
	DialogMaterial (QWidget * parent=0, bool modal=true, Qt::WindowFlags f=0 );
	~DialogMaterial();

signals:
	void sendMaterialChanged(int);

public slots:
	void updateButtons(bool isOn);

private:
	QList<QPushButton*> myButtons;
};

#endif
