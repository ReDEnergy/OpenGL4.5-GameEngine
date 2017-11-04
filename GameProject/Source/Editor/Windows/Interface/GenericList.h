#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Windows/Interface/CustomWidget.h>
#include <Editor/Interface/QtInput.h>

#include <QLayout>

template <class QTListWidget>
class GenericList
	: public CustomWidget
{
	public:
		GenericList()
		{
			InitUI();
			CreatePopupMenu();
		};
		
		virtual ~GenericList() {};

		QTListWidget * GetQtList() const
		{
			return qtList;
		};

		virtual void Clear()
		{
			activeIndex = -1;
			qtList->clear();
			ListCleared();
		};

		virtual void Update() {};

	protected:
		virtual void InitUI()
		{
			qtList = new QTListWidget();
			qtList->setDragEnabled(true);
			qtList->setDragDropMode(QAbstractItemView::InternalMove);
			qtList->setAlternatingRowColors(true);
			qtLayout->addWidget(qtList);

			QObject::connect(qtList, &QListWidget::itemPressed, this, &GenericList::QtItemClicked);
			QObject::connect(qtList, &QListWidget::itemChanged, this, &GenericList::QtItemRenamed);
		};

		virtual void ListCleared() {};
		virtual void AddItem() {};
		virtual void RemoveItem()
		{
			qtList->takeItem(qtList->currentRow());
			if (qtList->count() == 0)
				Clear();
		};

		virtual void RenameItem()
		{
			qtList->editItem(qtList->currentItem());
		};

		virtual void CreatePopupMenu()
		{
			qtList->setContextMenuPolicy(Qt::CustomContextMenu);
			QObject::connect(qtList, &QWidget::customContextMenuRequested, this, &GenericList::OpenPopupMenu);

			qtContextMenu = new QMenu(this);

			QAction* actionDefine = qtContextMenu->addAction("Add");
			QAction* actionRename = qtContextMenu->addAction("Rename");
			QAction* actionRemove = qtContextMenu->addAction("Delete");

			QObject::connect(actionDefine, &QAction::triggered, this, &GenericList::AddItem);
			QObject::connect(actionRename, &QAction::triggered, this, &GenericList::RenameItem);
			QObject::connect(actionRemove, &QAction::triggered, this, &GenericList::RemoveItem);
		}

		virtual void OpenPopupMenu(const QPoint & pos)
		{
			auto qtItem = qtList->itemAt(pos);

			if (qtItem) {
				qtContextMenu->popup(qtList->mapToGlobal(pos));
			}
		};

		virtual void QtItemClicked(QListWidgetItem *item)
		{
			activeIndex = qtList->currentRow();
			//if (item) {
			//	cout << item->text().toStdString().c_str() << endl;
			//}
		};

		virtual void QtItemRenamed(QListWidgetItem *item) {};

		virtual void DropEvent(QObject *source)
		{
			cout << "DROP EVENT" << endl;
		}


	protected:
		unsigned int activeIndex;
		QTListWidget *qtList;
		QMenu *qtContextMenu;
};