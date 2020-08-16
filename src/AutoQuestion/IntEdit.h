#pragma once
#include <QtWidgets/qlineedit.h>
class CIntEdit:public QLineEdit
{
public:
	CIntEdit(QWidget* parent = nullptr);
    ~CIntEdit();

public:
    int GetValue();
    void SetValue(int value);
};

