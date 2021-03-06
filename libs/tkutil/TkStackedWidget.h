/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TKSTACKEDWIDGET_H
#define TKSTACKEDWIDGET_H

#include <tkutil/tkutil_export.h>

class QStackedWidget;

/**
 * Usefull functions for QStackedWidget.
 *
 * @see QStackedWidget
 * @author Tanguy Krotoff
 */
class TKUTIL_API TkStackedWidget {
public:

	static void removeAllWidgets(QStackedWidget * stackedWidget);

private:

	TkStackedWidget();

};

#endif	//TKSTACKEDWIDGET_H
