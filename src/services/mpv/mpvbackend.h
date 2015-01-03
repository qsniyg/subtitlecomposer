#ifndef MPVBACKEND_H
#define MPVBACKEND_H

/***************************************************************************
 *   Copyright (C) 2007-2009 Sergio Pistone (sergio_pistone@yahoo.com.ar)  *
 *   based on smpv by Ricardo Villalba                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mpvconfig.h"
#include "../playerbackend.h"

#include <mpv/qthelper.hpp>

#include <QtGui/QWidget>
#include <QtCore/QString>

namespace SubtitleComposer {
class MPVProcess;

class MPVBackend : public PlayerBackend
{
	Q_OBJECT

public:
	MPVBackend(Player *player);
	virtual ~MPVBackend();

	const MPVConfig * config() { return static_cast<const MPVConfig *const>(PlayerBackend::config()); }

	virtual AppConfigGroupWidget * newAppConfigGroupWidget(QWidget *parent);

protected:
	virtual VideoWidget * initialize(QWidget *videoWidgetParent);
	virtual void finalize();
	void _finalize();

	virtual bool openFile(const QString &filePath, bool &playingAfterCall);
	virtual void closeFile();

	virtual bool play();
	virtual bool pause();
	virtual bool seek(double seconds, bool accurate);
	virtual bool stop();

	virtual bool setActiveAudioStream(int audioStream);

	virtual bool setVolume(double volume);

	bool mpvInit(int audioStream = -1, int audioStreamCount = 1);
	void mpvExit();

signals:
	void mpvEvents();

protected slots:
	void onMPVEvents();

protected:
	void setupProcessArgs(const QString &filePath);

	void mpvEventHandle(mpv_event *event);

	static void wakeup(void *ctx);

protected:
	mpv_handle *m_mpv;
	QString currentFilePath;
};
}

#endif