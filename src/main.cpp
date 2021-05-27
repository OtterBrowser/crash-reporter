/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2016 - 2021 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QLibraryInfo>
#include <QtCore/QProcess>
#include <QtCore/QTranslator>
#include <QtGui/QIcon>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	application.setQuitOnLastWindowClosed(false);
	application.setWindowIcon(QIcon::fromTheme(QLatin1String("otter-browser"), QIcon(QLatin1String(":/icons/otter-browser.png"))));

	const QString dumpPath(QCoreApplication::arguments().value(1));

	if (!QFile::exists(dumpPath))
	{
		QMessageBox::critical(nullptr, QCoreApplication::translate("main", "Error"), QCoreApplication::translate("main", "Invalid invocation."));

		return 0;
	}

	QTranslator qtTranslator;
	qtTranslator.load(QLatin1String("qt_") + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));

	QTranslator applicationTranslator;
	applicationTranslator.load(QLatin1String("otter-crash-reporter_") + QLocale::system().name(), QCoreApplication::applicationDirPath() + QLatin1String("/locale/"));

	application.installTranslator(&qtTranslator);
	application.installTranslator(&applicationTranslator);

	QString applicationPath(QLatin1String("otter-browser"));
#ifdef Q_OS_WIN32
	applicationPath.append(QLatin1String(".exe"));
#endif

	if (QFile::exists(QCoreApplication::applicationDirPath() + QDir::separator() + applicationPath))
	{
		applicationPath.prepend(QCoreApplication::applicationDirPath() + QDir::separator());
	}

	QProcess process;
	process.setProgram(applicationPath);
	process.setArguments(QStringList({QLatin1String("--report"), QLatin1String("noDialog"), QLatin1String("environment")}));
	process.start();
	process.waitForFinished();

	const QString report(process.readAll());
	CrashReporterDialog dialog(QCoreApplication::arguments().value(2));
	dialog.exec();

	if (dialog.getRestartMode() != CrashReporterDialog::NoRestart)
	{
		QProcess::startDetached(applicationPath, QStringList(QLatin1String("--session=") + ((dialog.getRestartMode() == CrashReporterDialog::FullRestart) ? QLatin1String("default") : QString())));
	}

	if (dialog.result() == QDialog::Accepted)
	{
		QFile file(dumpPath);
		file.open(QIODevice::ReadOnly);

		QHttpMultiPart multiPart(QHttpMultiPart::FormDataType);
		QHttpPart commentPart;
		commentPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QLatin1String("form-data; name=\"comment\"")));
		commentPart.setBody(dialog.getComment().toUtf8());

		QHttpPart urlPart;
		urlPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QLatin1String("form-data; name=\"url\"")));
		urlPart.setBody(dialog.getUrl().toUtf8());

		QHttpPart emailPart;
		emailPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QLatin1String("form-data; name=\"email\"")));
		emailPart.setBody(dialog.getEmail().toUtf8());

		QHttpPart reportPart;
		reportPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QLatin1String("form-data; name=\"report\"")));
		reportPart.setBody(report.toUtf8());

		QHttpPart dumpPart;
		dumpPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QLatin1String("application/octet-stream")));
		dumpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QLatin1String("form-data; name=\"dump\"; filename=\"report.dmp\"")));
		dumpPart.setBodyDevice(&file);

		multiPart.append(commentPart);
		multiPart.append(urlPart);
		multiPart.append(emailPart);
		multiPart.append(reportPart);
		multiPart.append(dumpPart);

		QNetworkRequest request(QUrl(QLatin1String("https://otter-browser.org/crash-reports/submit/")));
		request.setHeader(QNetworkRequest::UserAgentHeader, QLatin1String("Otter Browser Crash Reporter/1.0"));

		QNetworkAccessManager manager;
		QNetworkReply *reply(manager.post(request, &multiPart));
		QEventLoop eventLoop;

		QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
		QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &eventLoop, SLOT(quit()));

		eventLoop.exec();

		reply->deleteLater();
	}

	QFile::remove(dumpPath);

	return 0;
}
