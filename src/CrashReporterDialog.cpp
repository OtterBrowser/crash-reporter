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

#include "CrashReporterDialog.h"

#include "ui_CrashReporterDialog.h"

#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QPushButton>

CrashReporterDialog::CrashReporterDialog(const QString &currentUrl, QWidget *parent) : QDialog(parent),
	m_isIgnored(true),
	m_ui(new Ui::CrashReporterDialog)
{
	m_ui->setupUi(this);
	m_ui->urlLineEdit->setText(currentUrl);
	m_ui->urlLineEdit->home(false);
	m_ui->buttonBox->addButton(tr("Send Report"), QDialogButtonBox::AcceptRole);
	m_ui->buttonBox->addButton(tr("Do Not Send Report"), QDialogButtonBox::RejectRole);

	QButtonGroup *buttonGroup(new QButtonGroup(this));
	buttonGroup->addButton(m_ui->fullRestartButton);
	buttonGroup->addButton(m_ui->minimalRestartButton);
	buttonGroup->addButton(m_ui->noRestartButton);

	connect(m_ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
}

CrashReporterDialog::~CrashReporterDialog()
{
	delete m_ui;
}

void CrashReporterDialog::changeEvent(QEvent *event)
{
	QDialog::changeEvent(event);

	if (event->type() == QEvent::LanguageChange)
	{
		m_ui->retranslateUi(this);

		const QList<QAbstractButton*> buttons(m_ui->buttonBox->buttons());

		for (int i = 0; i < buttons.count(); ++i)
		{
			m_ui->buttonBox->removeButton(buttons.at(i));

			buttons.at(i)->deleteLater();
		}

		m_ui->buttonBox->addButton(tr("Send Report"), QDialogButtonBox::AcceptRole);
		m_ui->buttonBox->addButton(tr("Do Not Send Report"), QDialogButtonBox::RejectRole);
	}
}

void CrashReporterDialog::buttonClicked(QAbstractButton *button)
{
	m_isIgnored = false;

	if (m_ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
	{
		accept();
	}
	else
	{
		reject();
	}
}

QString CrashReporterDialog::getComment() const
{
	return m_ui->commentTextEdit->toPlainText();
}

QString CrashReporterDialog::getEmail() const
{
	return m_ui->emailLineEdit->text();
}

QString CrashReporterDialog::getUrl() const
{
	return m_ui->urlLineEdit->text();
}

CrashReporterDialog::RestartMode CrashReporterDialog::getRestartMode() const
{
	if (!m_isIgnored)
	{
		if (m_ui->fullRestartButton)
		{
			return FullRestart;
		}

		if (m_ui->minimalRestartButton)
		{
			return MinimalRestart;
		}
	}

	return NoRestart;
}

