#pragma once

#include <QtWidgets/QLayout>
#include <QtWidgets/QLayoutItem>

namespace QT {

	void ClearLayout(QLayout *layout)
	{
		QLayoutItem *item;
		while ((item = layout->takeAt(0))) {
			if (item->layout()) {
				ClearLayout(item->layout());
				delete item->layout();
			}
			if (item->widget()) {
				delete item->widget();
			}
			delete item;
		}
	};
}
