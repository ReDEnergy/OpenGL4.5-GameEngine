#include "SceneWindow.h"

#include <iostream>
#include <include/math.h>

#include <Editor/Windows/Interface/QtListTemplates.h>
#include <Editor/UI/GUI.h>

#include <QLayout>
#include <QMenu>
#include <QPushButton>
#include <QDragEnterEvent>

#include <Core/GameObject.h>


typedef QTTreeItem<GameObject> SceneItem;
typedef QTListItem<GameObject> DropItem;

SceneWindow::SceneWindow()
{
	setWindowTitle("Scene Objects");

	InitUI();
	CreatePopupMenu();

	GUI::Set(QT_INSTACE::SCENE_EDITOR, (void*) this);
	SubscribeToEvent(EventType::EDITOR_OBJECT_SELECTED);
	SubscribeToEvent(EventType::EDITOR_OBJECT_ADDED);
	SubscribeToEvent(EventType::EDITOR_OBJECT_REMOVED);
}

void SceneWindow::Init()
{
	Clear();
}

void SceneWindow::InitUI()
{
	qtTree = new SceneTreeWidget();
	qtTree->setDragEnabled(true);
	qtTree->setDragDropMode(QAbstractItemView::InternalMove);
	qtTree->setHeaderLabel("3D Sound Sources");
	qtTree->setHeaderHidden(true);
	qtTree->setAlternatingRowColors(true);
	qtLayout->addWidget(qtTree);

	QObject::connect(qtTree, &QTreeWidget::itemPressed, this, &SceneWindow::QtItemClicked);

	QPushButton *button = new QPushButton();
	button->setMinimumSize(0, 25);
	button->setText("Add Source");
	qtLayout->addWidget(button);

	QObject::connect(button, &QPushButton::clicked, this, &SceneWindow::CreateItem);

	rootItem = qtTree->invisibleRootItem();
}


void SceneWindow::Clear()
{
	qtTree->clear();
}

void SceneWindow::InsertItem()
{
	//auto item = new SceneItem(data);
	//rootItem->addChild(item);
}

void SceneWindow::RemoveItem()
{
	int row = qtTree->currentIndex().row();
	auto item = qtTree->takeTopLevelItem(row);
	SelectFirstTopLevelItem(row);
}

void SceneWindow::RenameItem()
{
	qtTree->editItem(qtTree->currentItem());
}

void SceneWindow::CreateItem()
{
	cout << "TODO - create item" << endl;
}

void SceneWindow::DropItem()
{
	cout << "TODO - drop item" << endl;
}

void SceneWindow::CreatePopupMenu()
{
	qtContextMenu = new QMenu(this);

	QAction* actionPlay = qtContextMenu->addAction("Play");
	QAction* actionStop = qtContextMenu->addAction("Stop");
	QAction* actionReload = qtContextMenu->addAction("Reload");
	QAction* actionRename = qtContextMenu->addAction("Rename");
	QAction* actionDelete = qtContextMenu->addAction("Delete");

	qtTree->setContextMenuPolicy(Qt::CustomContextMenu);

	QObject::connect(qtTree, &QWidget::customContextMenuRequested, this, &SceneWindow::OpenPopupMenu);

	QObject::connect(actionRename, &QAction::triggered, this, &SceneWindow::RenameItem);
	QObject::connect(actionDelete, &QAction::triggered, this, &SceneWindow::RemoveItem);
}

void SceneWindow::FindAndSelectItem(void *data)
{
	if (data == nullptr) return;

	auto noItems = qtTree->topLevelItemCount();
	for (int i = 0; i < noItems; i++) {
		auto item = qtTree->topLevelItem(i);
		auto itemData = ((SceneItem*)item)->GetData();
		if (itemData == data) {
			qtTree->setCurrentItem(item);
			QtItemClicked(item, 0);
			return;
		}
	}
}

void SceneWindow::FindAndDeleteItem(void *data)
{
	if (data == nullptr) return;
	auto obj = dynamic_cast<GameObject*>((GameObject*)data);

	if (obj == nullptr) return;

	auto noItems = qtTree->topLevelItemCount();
	for (int i = 0; i < noItems; i++) {
		auto itemData = ((SceneItem*)qtTree->topLevelItem(i))->GetData();
		if (itemData == data)
		{
			bool selectNewItem = (qtTree->currentIndex().row() == i);
			qtTree->takeTopLevelItem(i);
			if (selectNewItem && noItems > 1)
			{
				// select the previous element if there is any
				if (i) i--;
				auto item = qtTree->topLevelItem(i);
				qtTree->setCurrentItem(item);
				QtItemClicked(item, 0);
			}
			else {
				Clear();
			}
		break;
		}
	}
	cout << "Delete: " << ((GameObject*)data)->GetName() << endl;
}

void SceneWindow::OpenPopupMenu(const QPoint & pos)
{
	cout << pos.x() << "-" << pos.y() << endl;
	auto qtItem = qtTree->itemAt(pos);

	if (qtItem) {
		qtContextMenu->popup(qtTree->mapToGlobal(pos));
	}
}

void SceneWindow::QtItemClicked(QTreeWidgetItem *item, int column)
{
	auto Data = ((SceneItem*)item)->GetData();
	cout << "TODO - Select: " << Data->GetName() << endl;
}

void SceneWindow::QtItemRenamed(QListWidgetItem * item)
{
	auto C = ((SceneItem*)item)->GetData();
	auto val = item->text().toStdString();
	if (val.compare(C->GetName())) {
		cout << "Item [" << C->GetName() << "] renamed: to" << val.c_str() << endl;
	}
}

void SceneWindow::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::EDITOR_OBJECT_SELECTED) {
		FindAndSelectItem(data);
	}
	if (Event == EventType::EDITOR_OBJECT_REMOVED) {
		FindAndDeleteItem(data);
	}
}

void SceneWindow::SelectFirstTopLevelItem(int row)
{
	// Clear the Sound Editor Panel if the list is empty
	int count = qtTree->topLevelItemCount();
	if (count == 0)
	{
	}

	auto item = qtTree->topLevelItem(0);
	qtTree->setCurrentItem(item, 0);
	QtItemClicked(item, 0);
}

void SceneTreeWidget::dragEnterEvent(QDragEnterEvent * event)
{
	cout << "enter" << endl;
	auto source = event->source();
	if (source == this) {
		event->acceptProposedAction();
	}
}

void SceneTreeWidget::dropEvent(QDropEvent * event)
{
	auto source = event->source();

	if (source == this) {
		QTreeWidget::dropEvent(event);
		return;
	}
}
