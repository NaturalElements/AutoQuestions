#include "IntEdit.h"
#include <QtGui/qvalidator.h>


CIntEdit::CIntEdit(QWidget* parent)
    :QLineEdit(parent)
{
    setValidator(new QIntValidator(0, 1000, this));
}


CIntEdit::~CIntEdit()
{
}

int CIntEdit::GetValue()
{
    return text().toInt();
}

void CIntEdit::SetValue(int value)
{
    setText(QString::number(value));
}
