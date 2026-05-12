#include "LEOTrackPage.h"

LEOTrackPage::LEOTrackPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TaskTrajectoryDisplay)
{
    ui->setupUi(this);
}

LEOTrackPage::~LEOTrackPage()
{
    delete ui;
}
