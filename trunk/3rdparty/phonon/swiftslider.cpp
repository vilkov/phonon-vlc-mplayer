/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2008 Ricardo Villalba <rvm@escomposlinux.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "swiftslider.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_PHONON_SEEKSLIDER) && !defined(QT_NO_PHONON_VOLUMESLIDER)

namespace Phonon
{

SwiftSlider::SwiftSlider(Qt::Orientation orientation, QWidget * parent)
	: QSlider(orientation, parent)
{
}

SwiftSlider::~SwiftSlider()
{
}

// Function copied from qslider.cpp
inline int SwiftSlider::pick(const QPoint &pt) const
{
    return orientation() == Qt::Horizontal ? pt.x() : pt.y();
}

// Function copied from qslider.cpp and modified to make it compile
int SwiftSlider::pixelPosToRangeValue(int pos) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect gr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    int sliderMin, sliderMax, sliderLength;

    if (orientation() == Qt::Horizontal) {
        sliderLength = sr.width();
        sliderMin = gr.x();
        sliderMax = gr.right() - sliderLength + 1;
    } else {
        sliderLength = sr.height();
        sliderMin = gr.y();
        sliderMax = gr.bottom() - sliderLength + 1;
    }
    return QStyle::sliderValueFromPosition(minimum(), maximum(), pos - sliderMin,
                                           sliderMax - sliderMin, opt.upsideDown);
}

// Based on code from qslider.cpp
void SwiftSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        const QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        const QPoint center = sliderRect.center() - sliderRect.topLeft();
        // to take half of the slider off for the setSliderPosition call we use the center - topLeft

        if (!sliderRect.contains(event->pos())) {
            event->accept();

            setSliderPosition(pixelPosToRangeValue(pick(event->pos() - center)));
            triggerAction(SliderMove);
            setRepeatAction(SliderNoAction);
        } else {
            QSlider::mousePressEvent(event);
        }
    } else {
        QSlider::mousePressEvent(event);
    }
}

} // namespace Phonon

#endif //QT_NO_PHONON_VOLUMESLIDER && QT_NO_PHONON_VOLUMESLIDER

QT_END_NAMESPACE

#include "moc_swiftslider.cpp"