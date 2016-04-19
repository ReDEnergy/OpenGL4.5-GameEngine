#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Event/EventListener.h>

#include <QTreeWidget>

class SceneTreeWidget : public QTreeWidget
{
public:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
};

class SceneWindow
	: public DockWindow
	, public EventListener
{
	friend class SceneTreeWidget;

	public:
		SceneWindow();
		virtual ~SceneWindow() {};

		void InsertItem();
		void CreateItem();
		void Clear();
		void Init();

	private:
		void InitUI();

		void RemoveItem();
		void RenameItem();
		void CreatePopupMenu();
		void FindAndSelectItem(void *data);
		void FindAndDeleteItem(void * data);

		void OpenPopupMenu(const QPoint & pos);
		void QtItemClicked(QTreeWidgetItem* item, int column);
		void QtItemRenamed(QListWidgetItem* item);
		void OnEvent(EventType Event, void *data);

	protected:
		void DropItem();

	private:
		void SelectFirstTopLevelItem(int row = 0);

	private:
		QTreeWidget *qtTree;
		QTreeWidgetItem* rootItem;
		QMenu *qtContextMenu;
};