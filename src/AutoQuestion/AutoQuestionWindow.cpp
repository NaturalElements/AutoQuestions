
#include <chrono>
#include <memory>
#include <QtCore/qfile.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qprocess.h>
#include <QtWidgets/qmessagebox.h>
#include <QtGui/qtextdocument.h>
#include <QtPrintSupport/qprinter.h>
#include "AutoQuestionWindow.h"

bool g_resAtRight = true;

QString QStringUnit()
{
	return QString();
}

QString QStringUnit(int t)
{
	return QString::number(t);
}

QString QStringUnit(int t, int pred)
{
	QString res = QString::number(t);
	while (res.size() < pred)
		res = QStringLiteral(u" ") + res;
	return res;
}

QString QStringUnit(const char* t)
{
	return QString(t);
}

QString QStringUnit(const QString& t)
{
	return t;
}

QString QStringUnit(const Cal& t)
{
	switch (t)
	{
	case Cal::Add:
		return QStringLiteral(u"＋");
	case Cal::Decrease:
		return QStringLiteral(u"－");
	case Cal::Multiply:
		return QStringLiteral(u"×");
	case Cal::Div:
		return QStringLiteral(u"÷");
	default:
		break;
	}
	return QStringLiteral(u"");
}


template<class T, class ...T2>
QString QStringUnit(T a, T2... args)
{
	return QStringUnit(a) + QStringUnit(args...);
}

// 问题转换成字符串
QString QuestionToString(OneQuestion question)
{
	QString res;

	if(g_resAtRight)
	{
		// 结果
		if (question.misNumberIndex >= (int)question.numbers.size())
		{
			res = QStringUnit(question.numbers[0]);
			for (int i = 0;i < (int)question.cals.size();++i)
			{
				res += QStringUnit(question.cals[i], question.numbers[i + 1]);
			}
			res += QStringLiteral(u"＝___");
		}
		else
		{
			if (question.misNumberIndex == 0)
				res = QStringLiteral(u"___");
			else
				res = QStringUnit(question.numbers[0]);
			for (int i = 0;i < (int)question.cals.size();++i)
			{
				res += QStringUnit(question.cals[i]);
				if (question.misNumberIndex == i + 1)
					res += QStringLiteral(u"___");
				else
					res += QStringUnit(question.numbers[i + 1]);
			}
			res += QStringUnit(QStringLiteral(u"＝"), question.result);
		}
	}
	else
	{
		// 结果
		if (question.misNumberIndex >= (int)question.numbers.size())
		{
			res += QStringLiteral(u"___＝");
			res += QStringUnit(question.numbers[0]);
			for (int i = 0;i < (int)question.cals.size();++i)
			{
				res += QStringUnit(question.cals[i], question.numbers[i + 1]);
			}
		}
		else
		{
			res += QStringUnit(question.result, QStringLiteral(u"＝"));
			if (question.misNumberIndex == 0)
				res += QStringLiteral(u"___");
			else
				res += QStringUnit(question.numbers[0]);
			for (int i = 0;i < (int)question.cals.size();++i)
			{
				res += QStringUnit(question.cals[i]);
				if (question.misNumberIndex == i + 1)
					res += QStringLiteral(u"___");
				else
					res += QStringUnit(question.numbers[i + 1]);
			}
		}
	}
	return res;
}

CAutoQuestionWindow::CAutoQuestionWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.m_pushButtonOK, &QPushButton::clicked, [this]() {
		// 点击确定
		int colCount = ui.comboBoxRowQuestionNumber->currentText().toInt();
		int rowCount = ui.m_editRowCount->GetValue();
		if (colCount < 0 || rowCount < 0)
			return;

		bool withAnswer = ui.m_withAnswer->isChecked();
		QString body;
		QString table;
		QString tr;
		QString tdIndex;
		QString tdQuestion;
		QString tdAnswer;
		// 打印风格
		QString stylefileName = QStringUnit(QStringLiteral(u"./style"), colCount, QStringLiteral(u".txt"));
		QFile file(stylefileName);
		if (file.open(QFile::ReadWrite))
		{
			body = file.readLine();
			table = file.readLine();
			tr = file.readLine();
			tdIndex = file.readLine();
			tdQuestion = file.readLine();
			tdAnswer = file.readLine();
		}

		// 数值最大最小值
		m_autoQuestion.SetAddDecreaseMinMaxNumber(ui.m_minNum->GetValue(), ui.m_maxNum->GetValue());
		m_autoQuestion.SetMultDivMinMaxNumber(ui.m_minNum_2->GetValue(), ui.m_maxNum_2->GetValue());

		// 等号左右
		g_resAtRight = ui.m_resAtRight->isChecked();

		// 应用的算法
		std::set<Cal> cals;
		if (ui.m_withAdd->isChecked())
			cals.insert(Cal::Add);
		if (ui.m_withDecrease->isChecked())
			cals.insert(Cal::Decrease);
		if (ui.m_withMuty->isChecked())
			cals.insert(Cal::Multiply);
		if (ui.m_withDiv->isChecked())
			cals.insert(Cal::Div);

		if (cals.size() == 0)
		{
			QMessageBox::information(this, QStringLiteral(u"请使用至少一种算法"), QStringLiteral(u"请至少选择一种！"));
			return;
		}

		m_autoQuestion.SetUseCals(cals);


		// 随机种子
		auto tm = std::chrono::system_clock::now().time_since_epoch().count();
		std::srand(tm);

		// 题目总数，并生成题目
		int count = colCount*rowCount;
		std::vector<OneQuestion> questions;
		questions.resize(count);
		for (int i = 0; i < count; ++i)
		{
			questions[i] = m_autoQuestion.MakeQuestion(ui.comboBox_NumberCount->currentText().toInt(), ui.m_MutiDivContinuous->isChecked());
		}

		// 生成题目pdf文本
		QString allString;
		allString += body + table;
		for (int row = 0; row < rowCount; ++row)
		{
			QString rowString;
			rowString += tr;
			for (int col = 0; col < colCount; ++col)
			{
				rowString += tdIndex;
				rowString += QStringUnit(row*colCount + col + 1, QStringLiteral(u"."));
				rowString += QStringLiteral(u"</td>");
				rowString += tdQuestion;
				rowString += QuestionToString(questions[row*colCount + col]);
				rowString += QStringLiteral(u"</td>");
			}
			rowString += QStringLiteral(u"</tr>");
			allString += rowString;
		}
		allString += QStringLiteral(u"</table></body>");
		QDateTime&& time = QDateTime::currentDateTime();
		QString fileName = QApplication::applicationDirPath() + QStringLiteral(u"/题目/") + time.toString(QStringLiteral(u"yyyy-MM-dd__hh_mm_ss")) + QStringUnit(QStringLiteral(u"_"), colCount, QStringLiteral(u"_"), rowCount, QStringLiteral(u".pdf"));
		QString answerFileName = QApplication::applicationDirPath() + QStringLiteral(u"/题目/") + time.toString(QStringLiteral(u"yyyy-MM-dd__hh_mm_ss")) + QStringUnit(QStringLiteral(u"_"), colCount, QStringLiteral(u"_"), rowCount, QStringLiteral(u"_答案.pdf"));

		// 纸张
		QString paper = ui.comboBox_Paper->currentText();
		QPrinter::PageSize pageSize = QPrinter::A4;
		if (paper == QStringLiteral(u"A4"))
			pageSize = QPrinter::A4;
		else if (paper == QStringLiteral(u"A3"))
			pageSize = QPrinter::A3;
		else if (paper == QStringLiteral(u"A5"))
			pageSize = QPrinter::A5;
		// 输出到PDF
		QPrinter printer(QPrinter::HighResolution);
		printer.setPageSize(pageSize);
		printer.setOutputFileName(fileName);
		QTextDocument txtdoc;
		txtdoc.setHtml(allString);
		txtdoc.print(&printer);

		// 生成答案文本
		if (withAnswer)
		{
			QString allAnswer;
			allString += body + table;
			int colDef = ui.comboBox_RowAnswerNumber->currentText().toInt();
			int irowCount = (colCount * rowCount) / colDef + ((((colCount * rowCount) % colDef) > 0) ? 1 : 0);
			for (int row = 0; row < irowCount; ++row)
			{
				QString rowString;
				rowString += tr;
				for (int col = 0; col < colDef; ++col)
				{
					int index = row * colDef + col;
					if (index >= colCount * rowCount)
						break;

					rowString += QStringLiteral(u"<td style=\"text-align:right;padding-right:6;padding-top:13\" width=2 % >");
					rowString += QStringUnit(row * colDef + col + 1, ".");
					rowString += QStringLiteral(u"</td>");
					rowString += tdAnswer;
					int index0 = row * colDef + col;
					auto question = questions[index0];
					int answer = question.result;
					if (question.misNumberIndex < (int)question.numbers.size())
						answer = question.numbers[question.misNumberIndex];
					rowString += QStringUnit(answer);
					rowString += QStringLiteral(u"</td>");
				}
				rowString += QStringLiteral(u"</tr>");
				allAnswer += rowString;
			}
			allAnswer += QStringLiteral(u"</table></body>");

			QPrinter printerAnswer(QPrinter::HighResolution);
			printerAnswer.setPageSize(pageSize);
			printerAnswer.setOutputFileName(answerFileName);
			QTextDocument txtdocAnswer;
			txtdocAnswer.setHtml(allAnswer);
			txtdocAnswer.print(&printerAnswer);
		}

		// 打开文件位置
		if (ui.m_openDir->isChecked())
		{
#ifdef _WIN32
			QString cmd = QStringLiteral(u"explorer /select, ") + fileName.replace(QStringLiteral(u"/"), QStringLiteral(u"\\"));
			QProcess::startDetached(cmd);
#endif
		}
	});
}
