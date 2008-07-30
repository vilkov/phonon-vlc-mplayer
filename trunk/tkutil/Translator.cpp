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

#include "Translator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QLibraryInfo>
#include <QtCore/QTranslator>
#include <QtCore/QDebug>

static const QString TRANSLATIONS_PATH = "translations/";

Translator * Translator::_instance = NULL;

Translator & Translator::instance() {
	//Lazy initialization
	if (!_instance) {
		_instance = new Translator();
	}

	return *_instance;
}

Translator::Translator() {
	_translatorInstalled = false;
}

Translator::~Translator() {
}

void Translator::install() {
	QCoreApplication::instance()->installTranslator(&_appTranslator);
	QCoreApplication::instance()->installTranslator(&_qtTranslator);
	_translatorInstalled = true;
}

void Translator::remove() {
	QCoreApplication::instance()->removeTranslator(&_appTranslator);
	QCoreApplication::instance()->removeTranslator(&_qtTranslator);
	_translatorInstalled = false;
}

void Translator::load(const QString & locale) {
	if (!_translatorInstalled) {
		install();
	}

	QString myLocale = locale;
	if (myLocale.isEmpty()) {
		myLocale = QLocale::system().name();
	}

	//Qt translation
	bool qtRet = loadLocale(_qtTranslator, "qt", myLocale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	if (!qtRet) {
		//No Qt framework installed
		//Try with the application path
		qtRet = loadLocale(_qtTranslator, "qt", myLocale, TRANSLATIONS_PATH);
	}

	//Application translation
	bool appRet = loadLocale(_appTranslator, QCoreApplication::applicationName().toLower(), myLocale, TRANSLATIONS_PATH);

	//Both Qt and app locale loading failed
	//Let's go back to english builtin
	if (!qtRet && !appRet) {
		qDebug() << __FUNCTION__ << "Back to builtin english";
		remove();
	}
}

bool Translator::loadLocale(QTranslator & translator, const QString & name, const QString & locale, const QString & dir) {
	QString filename = name + "_" + locale;
	bool ret = translator.load(filename, dir);
	if (!ret) {
		qDebug() << __FUNCTION__ << "Error: couldn't load translation:" << filename << "from:" << dir;
	} else {
		qDebug() << __FUNCTION__ << "Translation loaded:" << filename << "from:" << dir;
	}
	return ret;
}
