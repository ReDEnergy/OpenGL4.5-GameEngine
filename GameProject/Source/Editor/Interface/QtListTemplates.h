#pragma once

#include <vector>

#include <QDragEnterEvent>
#include <QListWidgetItem>
#include <QObject>
#include <QTreeWidgetItem>
#include <QLineEdit>

// ***************************************************************************
// QTListItem

template <class T>
class QTListItem
	: public QListWidgetItem
{
	public:
		QTListItem(T* appData)
		{
			setText(appData->GetName());
			setSizeHint(QSize(0, 22));
			setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable |  Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			widget = nullptr;
			this->appData = appData;
		};

		T* GetData() const
		{
			return appData;
		};
		QWidget * GetWdiget() const
		{
			return widget;
		};

	protected:
		T* appData;
		QWidget *widget;
};

// ***************************************************************************
// QTTreeItem

template <class T>
class QTTreeItem
	: public QTreeWidgetItem
{
	public:
		QTTreeItem(T* appData)
		{
			setText(0, appData->GetName());
			setSizeHint(0, QSize(0, 22));
			setFlags(
				Qt::ItemIsSelectable | 	Qt::ItemIsEditable | Qt::ItemIsDragEnabled |
				//Qt::ItemIsDropEnabled |
				Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

			widget = nullptr;
			this->appData = appData;
		};

		T* GetData() const
		{
			return appData;
		};

		QWidget * GetWdiget() const
		{
			return widget;
		};

	protected:
		T* appData;
		QWidget *widget;
};

// ***************************************************************************
// QTLineEdit

template <class T>
class QTLineEdit
	: public QLineEdit
{
	public:
		QTLineEdit(T* appData)
		{
			setText(appData->GetValue());
			//setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

			widget = nullptr;
			this->GetData() = appData;
		};

		// TODO - method added because a certain template doesn't implment it. Should be removed in the future
		const char *GetName() {
			return nullptr;
		};

		T* GetData() const
		{
			return appData;
		};

		QWidget * GetWdiget() const
		{
			return widget;
		};

	protected:
		T* appData;
		QWidget *widget;
};

// ****************************************************************************
// Generic QListWidget with binding data using list items declared above

template <class T, class P>
class QTListWidget : public QListWidget
{
public:
	QTListWidget()
	{
		AddDragnDropSource(this);
	};

	void AddItem(T* appData)
	{
		auto item = new P(appData);
		addItem(item);

		auto itemWidget = item->GetWdiget();
		if (itemWidget)
			setItemWidget(item, itemWidget);
	};

	T* GetItemData(QListWidgetItem *item) const
	{
		return (T*)(((P*)item)->GetData());
	};

	void AddDragnDropSource(QObject* S)
	{
		sources.push_back(S);
	};

	void dragEnterEvent(QDragEnterEvent *event)
	{
		auto source = event->source();
		for (auto s : sources) {
			if (s == source) {
				event->acceptProposedAction();
				return;
			}
		}
	};

	void dropEvent(QDropEvent *event)
	{
		auto source = event->source();
		if (source == this) {
			QListWidget::dropEvent(event);
		}
		this->DropEvent(source);
	};

	virtual void DropEvent(QObject *source) {};

	private:
		vector <QObject*> sources;
};