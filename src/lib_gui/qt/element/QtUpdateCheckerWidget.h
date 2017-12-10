#ifndef QT_UPDATE_CHECKER_WIDGET_H
#define QT_UPDATE_CHECKER_WIDGET_H

#include <QWidget>

class QPushButton;

class QtUpdateCheckerWidget
	: public QWidget
{
public:
	QtUpdateCheckerWidget(QWidget* parent = nullptr);
	virtual ~QtUpdateCheckerWidget();

private:
	void checkUpdate(bool force);
	void setDownloadUrl(QString url);

	QPushButton* m_button;

	std::shared_ptr<bool> m_deleteCheck;
};

#endif // QT_UPDATE_CHECKER_WIDGET_H
