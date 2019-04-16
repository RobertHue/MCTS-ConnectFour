#include "Delegate.h"

void Delegate::paint(
	QPainter                     *painter,
	QStyleOptionViewItem const &  option,
	QModelIndex          const &  index) const
{
	QStyleOptionViewItem opt(option);

	if (index.column() == 2)
	{
		opt.palette.setColor(
			QPalette::Base,
			QColor(111, 255, 204));

		opt.palette.setColor(
			QPalette::Window,
			QColor(111, 124, 204));
	}

	return QItemDelegate::paint(painter, opt, index);
};