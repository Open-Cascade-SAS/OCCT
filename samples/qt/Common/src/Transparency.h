#include <Standard_WarningsDisable.hxx>
#include <QDialog>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>

class DialogTransparency : public QDialog
{
	Q_OBJECT
public:
	DialogTransparency ( QWidget * parent=0, Qt::WindowFlags f=0, bool modal=true );
	~DialogTransparency();
signals:
	void sendTransparencyChanged(int value);
};
