#pragma once

#include <QWidget>
#include "ui_test_01.h"

QT_BEGIN_NAMESPACE
namespace Ui { class test_01Class; };
QT_END_NAMESPACE

class test_01 : public QWidget
{
	Q_OBJECT

public:
	test_01(QWidget *parent = nullptr);
	~test_01();

private:
	Ui::test_01Class *ui;
};

