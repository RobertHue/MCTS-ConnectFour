#pragma once

#include <QItemDelegate>
#include <qvariant.h>
#include <QPainter>


class Delegate : public QItemDelegate
{
	Q_OBJECT
public:
	Delegate(QWidget *parent = 0) : QItemDelegate(parent) {}
	void paint(
		QPainter                     *painter,
		QStyleOptionViewItem const &  option,
		QModelIndex          const &  index) const;
};