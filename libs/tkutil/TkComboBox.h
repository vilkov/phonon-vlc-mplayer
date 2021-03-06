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

#ifndef TKCOMBOBOX_H
#define TKCOMBOBOX_H

#include <tkutil/tkutil_export.h>

class QComboBox;
class QString;

/**
 * Useful functions for QComboBox.
 *
 * @see QComboBox
 * @author Tanguy Krotoff
 */
class TKUTIL_API TkComboBox {
public:

	static void setCurrentText(QComboBox * comboBox, const QString & text);

private:

	TkComboBox();

};

#endif	//TKCOMBOBOX_H
