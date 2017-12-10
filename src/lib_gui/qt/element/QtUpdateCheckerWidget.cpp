#include "qt/element/QtUpdateCheckerWidget.h"

#include <QDesktopServices>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QUrl>

#include "qt/network/QtUpdateChecker.h"
#include "settings/ApplicationSettings.h"
#include "utility/TimeStamp.h"

QtUpdateCheckerWidget::QtUpdateCheckerWidget(QWidget* parent)
	: QWidget(parent)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	m_button = new QPushButton("check for new version");
	m_button->setObjectName("updateButton");
	m_button->setCursor(Qt::PointingHandCursor);
	layout->addWidget(m_button);

	if (appSettings->getAutomaticUpdateCheck())
	{
		if (QtUpdateChecker::needsAutomaticCheck())
		{
			m_button->setText("checking for update...");
			m_button->setEnabled(false);
			QTimer::singleShot(250,
				[this]()
				{
					checkUpdate(false);
				}
			);
		}
		else
		{
			Version version = appSettings->getUpdateVersion();
			QString url = QString::fromStdString(appSettings->getUpdateDownloadUrl());
			if (version > Version::getApplicationVersion() && !url.isEmpty())
			{
				setDownloadUrl(url);
			}
			else
			{
				m_button->setText("up-to-date");
				m_button->setEnabled(false);
			}
		}
	}
	else
	{
		connect(m_button, &QPushButton::clicked, [this](){ checkUpdate(true); });
	}
}

QtUpdateCheckerWidget::~QtUpdateCheckerWidget()
{
	if (m_deleteCheck)
	{
		*m_deleteCheck.get() = true;
	}
}

void QtUpdateCheckerWidget::checkUpdate(bool force)
{
	m_button->setText("checking for update...");
	m_button->setEnabled(false);

	std::shared_ptr<bool> deleteCheck = std::make_shared<bool>(false);
	m_deleteCheck = deleteCheck;

	QtUpdateChecker::check(force,
		[deleteCheck, this](QtUpdateChecker::Result result)
		{
			bool deleted = *deleteCheck.get();
			if (deleted)
			{
				return;
			}

			if (!result.success)
			{
				m_button->setText("update check failed");
			}
			else if (result.url.isEmpty())
			{
				m_button->setText("up-to-date");
			}
			else
			{
				setDownloadUrl(result.url);
			}
		}
	);
}

void QtUpdateCheckerWidget::setDownloadUrl(QString url)
{
	m_button->setText("new version available");
	m_button->disconnect();
	connect(m_button, &QPushButton::clicked, this,
		[url]()
		{
			QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
		}
	);
	m_button->setEnabled(true);
}
