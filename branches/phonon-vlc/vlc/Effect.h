/*
 * VLC backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *               2008       Lukas Durfina <lukas.durfina@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PHONON_VLC_EFFECT_H
#define PHONON_VLC_EFFECT_H

#include "SinkNode.h"
#include "EffectManager.h"

#include <phonon/effectinterface.h>
#include <phonon/effectparameter.h>

namespace Phonon
{
namespace VLC
{

class EffectManager;


class Effect : public SinkNode, public EffectInterface
{
	Q_OBJECT
	Q_INTERFACES( Phonon::EffectInterface )
public:

	Effect( EffectManager *p_em, int i_effectId, QObject *p_parent);
	~Effect();

	QList<EffectParameter> parameters() const;

	QVariant parameterValue( const EffectParameter & param ) const;

	void setParameterValue( const EffectParameter & param, const QVariant & newValue );

	void connectToMediaObject( PrivateMediaObject *p_media_object );

	void disconnectFromMediaObject( PrivateMediaObject *p_media_object );

private:

    EffectManager *p_effectManager;
	int i_effect_filter;
	EffectInfo::Type effect_type;
	QList<Phonon::EffectParameter> parameterList;
};

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_EFFECT_H
