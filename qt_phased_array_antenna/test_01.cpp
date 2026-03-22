#include "test_01.h"

test_01::test_01(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::test_01Class())
{
	ui->setupUi(this);
}

test_01::~test_01()
{
	delete ui;
}

