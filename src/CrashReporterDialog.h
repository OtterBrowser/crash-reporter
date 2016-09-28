/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2016 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#ifndef DIALOG_H
#define DIALOG_H

#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QDialog>

namespace Ui
{
	class CrashReporterDialog;
}

class CrashReporterDialog : public QDialog
{
	Q_OBJECT

public:
	enum RestartMode
	{
		NoRestart = 0,
		FullRestart,
		MinimalRestart
	};

	explicit CrashReporterDialog(const QString &currentUrl, QWidget *parent = NULL);
	~CrashReporterDialog();

	QString getComment() const;
	QString getEmail() const;
	QString getUrl() const;
	RestartMode getRestartMode() const;

protected:
	void changeEvent(QEvent *event);

protected slots:
	void buttonClicked(QAbstractButton *button);

private:
	bool m_isIgnored;
	Ui::CrashReporterDialog *m_ui;
};

#endif
