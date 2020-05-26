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

#include "TranslateDialog.h"

#include <Standard_WarningsDisable.hxx>
#include <QGridLayout>
#include <Standard_WarningsRestore.hxx>

TranslateDialog::TranslateDialog(QWidget* parent, Qt::WindowFlags flags, bool modal)
: QFileDialog(parent, flags)
{
  setOption(QFileDialog::DontUseNativeDialog);
  setModal(modal);

  QGridLayout* grid = ::qobject_cast<QGridLayout*>(layout());

  if (grid)
  {
    QVBoxLayout *vbox = new QVBoxLayout;

    QWidget* paramGroup = new QWidget(this);
    paramGroup->setLayout(vbox);

    myBox = new QComboBox(paramGroup);
    vbox->addWidget(myBox);

    int row = grid->rowCount();
    grid->addWidget(paramGroup, row, 1, 1, 3); // make combobox occupy 1 row and 3 columns starting from 1
  }
}

TranslateDialog::~TranslateDialog()
{
}

int TranslateDialog::getMode() const
{
  if (myBox->currentIndex() < 0 || myBox->currentIndex() > (int)myList.count() - 1)
  {
    return -1;
  }
  else
  {
    return myList.at(myBox->currentIndex());
  }
}

void TranslateDialog::setMode(const int mode)
{
  int idx = myList.indexOf(mode);
  if (idx >= 0)
  {
    myBox->setCurrentIndex(idx);
  }
}

void TranslateDialog::addMode(const int mode, const QString& name)
{
  myBox->show();
  myBox->addItem(name);
  myList.append(mode);
  myBox->updateGeometry();
  updateGeometry();
}

void TranslateDialog::clear()
{
  myList.clear();
  myBox->clear();
  myBox->hide();
  myBox->updateGeometry();
  updateGeometry();
}

QListView* TranslateDialog::findListView(const QObjectList & childList)
{
  QListView* listView = 0;
  for (int i = 0, n = childList.count(); i < n && !listView; i++)
  {
    listView = qobject_cast<QListView*>(childList.at(i));
    if (!listView && childList.at(i))
    {
      listView = findListView(childList.at(i)->children());
    }
  }
  return listView;
}

void TranslateDialog::showEvent(QShowEvent* event)
{
  QFileDialog::showEvent(event);
  QListView* aListView = findListView(children());
  aListView->setViewMode(QListView::ListMode);
}
