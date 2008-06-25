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

#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QtGui/QDialog>

#include <QtCore/QList>
#include <QtCore/QMap>

class IConfigWidget;
namespace Ui { class ConfigWindow; }

class QAbstractItemView;

/**
 * Window that shows QuarkPlayer configuration.
 *
 * @author Tanguy Krotoff
 */
class ConfigWindow : public QDialog {
	Q_OBJECT
public:

	ConfigWindow(QWidget * parent);

	~ConfigWindow();

private slots:

	void showConfigWidget(int row);

	void saveConfig();

	void retranslate();

private:

	void populateStackedWidget();

	static int computeListViewMinimumWidth(QAbstractItemView * view);

	Ui::ConfigWindow * _ui;

	/** List of all config widget. */
	QList<IConfigWidget *> _configWidgetList;

	/**
	 * List of opened config widget.
	 *
	 * Trick for not having saving conflicts between config widgets.
	 * Saves the config widgets in the exact order they were last opened.
	 */
	QMap<IConfigWidget *, int> _configWidgetOpenedMap;

	int _lastConfigWindowOpenedIndex;
};

#endif	//CONFIGWINDOW_H