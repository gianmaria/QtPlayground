#include "main_window.h"
#include "./ui_main_window.h"
#include <QDebug>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/spaceship.png"));

    QObject::connect(ui->stackedWidget, &QStackedWidget::currentChanged,
                     this, [this](int index)
    {
        qDebug() << "new index is:" << index;

        QString msg = QString("Page %1 %2").arg(index+1).arg("Yo");

        this->ui->statusbar->showMessage(msg, 3000);
    });

    QShortcut* shortcut = new QShortcut(
        QKeySequence(Qt::CTRL | Qt::Key_A),
        this);

    QObject::connect(shortcut, &QShortcut::activated,
                     this, []()
    {
        qDebug() << "Shortcut triggered!";
    });


    QKeySequence seq(Qt::Key_Escape);
    QShortcut* shortcut2 =
        new QShortcut(seq, this);

    auto lampada = [this](){
        this->close();
    };
    connect(shortcut2, &QShortcut::activated,
            this, lampada);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pb_next_released() {
    auto current_index = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(current_index + 1);
}

void MainWindow::on_pb_prev_released() {
    auto current_index = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(current_index - 1);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A)
    {
        qDebug() << "A key pressed!";
    }

    QMainWindow::keyPressEvent(event);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "goodbyeeeeeeeeeeeee";
    QMainWindow::closeEvent(event);
}
