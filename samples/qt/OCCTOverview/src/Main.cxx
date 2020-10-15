// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "ApplicationCommon.h"

#include <OSD_Environment.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QLocale>
#include <QSettings>
#include <QStringList>
#include <QTranslator>
#include <Standard_WarningsRestore.hxx>

int main ( int argc, char* argv[] )
{
  QApplication aQApp( argc, argv );

#include <Standard_WarningsDisable.hxx>
  Q_INIT_RESOURCE(OCCTOverview);
#include <Standard_WarningsRestore.hxx>

  QSettings settings("OCCTOverview.conf", QSettings::IniFormat);
  settings.beginGroup("ApplicationSetting");
    ApplicationType aCategory = static_cast<ApplicationType>(settings.value("ApplicationType", "").toInt());
  settings.endGroup();

  ApplicationCommonWindow* aWindow = new ApplicationCommonWindow(aCategory);
  QString aResName(":/icons/lamp.png");
  aWindow->setWindowIcon(QPixmap(aResName));

  settings.beginGroup("WindowPosition");
    int x = settings.value("x", -1).toInt();
    int y = settings.value("y", -1).toInt();
    int width = settings.value("width", -1).toInt();
    int height = settings.value("height", -1).toInt();
  settings.endGroup();

  if (x > 0 && y > 0 && width > 0 && height > 0)
  {
    aWindow->setGeometry(x, y, width, height);
  }
  aWindow->SetApplicationType(aCategory);

  aWindow->show();
  int aResult = aQApp.exec();

  settings.beginGroup("WindowPosition");
    settings.setValue("x", aWindow->x());
    settings.setValue("y", aWindow->y());
    settings.setValue("width", aWindow->width());
    settings.setValue("height", aWindow->height());
  settings.endGroup();

  settings.beginGroup("ApplicationSetting");
    settings.setValue("ApplicationType", aWindow->GetApplicationType());
  settings.endGroup();

  return aResult;
}
