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

#ifndef NAMED_VALUES_VECTOR_MODEL_H
#define NAMED_VALUES_VECTOR_MODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>
#include "../compiler/compiler.h"


namespace Aseba
{
	/** \addtogroup studio */
	/*@{*/
	
	class NamedValuesVectorModel: public QAbstractTableModel
	{
		Q_OBJECT
	
	public:
		NamedValuesVectorModel(NamedValuesVector* namedValues, QObject *parent = 0);
		
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		Qt::ItemFlags flags(const QModelIndex & index) const;
		
		bool setData(const QModelIndex &index, const QVariant &value, int role);
		
	public slots:
		void addNamedValue(const NamedValue& namedValue);
		void delNamedValue(int index);
		void clear();
		
	private:
		NamedValuesVector* namedValues;
	};
	
	/*@}*/
}; // Aseba

#endif
