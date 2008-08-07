/*
	Aseba - an event-based framework for distributed robot control
	Copyright (C) 2006 - 2008:
		Stephane Magnenat <stephane at magnenat dot net>
		(http://stephane.magnenat.net)
		and other contributors, see authors.txt for details
		Mobots group, Laboratory of Robotics Systems, EPFL, Lausanne
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any other version as decided by the two original authors
	Stephane Magnenat and Valentin Longchamp.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <signal.h>
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QString>
#include <QLocale>
#include "MainWindow.h"

/**
	\defgroup studio Integrated Development Editor
*/

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	// override dashel signal handling
	signal(SIGTERM, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	
	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name());
	app.installTranslator(&qtTranslator);
	
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	
	QTranslator translator;
	translator.load(QString(":/asebastudio_") + QLocale::system().name());
	app.installTranslator(&translator);
	
	Aseba::MainWindow window;
	window.show();
	return app.exec();
}


