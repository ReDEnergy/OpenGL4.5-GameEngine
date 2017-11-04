#include "EditorList.h"

#include <iostream>

#include <Editor/Windows/Interface/GenericList.h>

#include <QListWidget>
#include <QLayout>
#include <QMenu>
#include <QAction>

using namespace std;

EditorList::EditorList()
{
	setWindowTitle("EditorList");

	InitUI();
	CreatePopupMenu();
}

QListWidget * EditorList::GetQtList() const
{
	return qtList;
}

void EditorList::InitUI()
{
	qtList = new QListWidget();
	qtList->setDragEnabled(true);
	qtList->setDragDropMode(QAbstractItemView::InternalMove);
	qtList->setAlternatingRowColors(true);
	qtLayout->addWidget(qtList);

	QObject::connect(qtList, &QListWidget::itemPressed, this, &EditorList::QtItemClicked);
	QObject::connect(qtList, &QListWidget::itemChanged, this, &EditorList::QtItemRenamed);
}

void EditorList::RemoveItem()
{
	qtList->takeItem(qtList->currentRow());
}

void EditorList::RenameItem()
{
	qtList->editItem(qtList->currentItem());
}

void EditorList::CreatePopupMenu()
{
	qtContextMenu = new QMenu(this);

	//QAction* actionDefine = qtContextMenu->addAction("Add");
	QAction* actionRename = qtContextMenu->addAction("Rename");
	QAction* actionRemove = qtContextMenu->addAction("Delete");

	qtList->setContextMenuPolicy(Qt::CustomContextMenu);

	QObject::connect(qtList, &QWidget::customContextMenuRequested, this, &EditorList::OpenPopupMenu);

	//QObject::connect(actionDefine, &QAction::triggered, this, &EditorList::AddItem);
	QObject::connect(actionRename, &QAction::triggered, this, &EditorList::RenameItem);
	QObject::connect(actionRemove, &QAction::triggered, this, &EditorList::RemoveItem);
}

void EditorList::OpenPopupMenu(const QPoint& pos)
{
	auto qtItem = qtList->itemAt(pos);

	if (qtItem) {
		qtContextMenu->popup(qtList->mapToGlobal(pos));
	}
}

void EditorList::QtItemClicked(QListWidgetItem * item)
{
}

void EditorList::QtItemRenamed(QListWidgetItem * item)
{
}

void EditorList::keyPressEvent(QKeyEvent * e)
{
}
