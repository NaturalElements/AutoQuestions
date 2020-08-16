#pragma once

#include <QtWidgets/qmainwindow.h>
#include "ui_AutoQuestion.h"
#include "AutoQuestion.h"

class CAutoQuestionWindow : public QMainWindow
{
	Q_OBJECT

public:
	CAutoQuestionWindow(QWidget *parent = Q_NULLPTR);

private:
	Ui::autoqusClass ui;
	CAutoQuestion m_autoQuestion;
};
