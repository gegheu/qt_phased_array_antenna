#ifndef MODULE7_H
#define MODULE7_H

#include <QWidget>

namespace Ui {
    class Module7;
}

class Module7 : public QWidget
{
    Q_OBJECT

public:
    explicit Module7(QWidget* parent = nullptr);
    ~Module7();


private:
    Ui::Module7* ui;
};

#endif // MODULE7_H