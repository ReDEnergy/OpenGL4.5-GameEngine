#pragma once
#include <Editor/Windows/Interface/DockWindow.h>

class EditorList
	: public DockWindow
{
	public:
		EditorList();
		virtual ~EditorList() {};
		QListWidget * GetQtList() const;

	protected:
		virtual void InitUI();

		//virtual void AddItem();
		virtual void RemoveItem();
		virtual void RenameItem();
		virtual void CreatePopupMenu();

		virtual void OpenPopupMenu(const QPoint & pos);
		virtual void QtItemClicked(QListWidgetItem *item);
		virtual void QtItemRenamed(QListWidgetItem *item);

		virtual void keyPressEvent(QKeyEvent* e);

	protected:
		QListWidget *qtList;
		QMenu *qtContextMenu;
};