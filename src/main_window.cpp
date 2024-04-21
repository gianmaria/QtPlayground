#include "main_window.h"
#include "./ui_main_window.h"
#include <QDebug>
#include <QProcess>
#include <QShortcut>
#include <QFile>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/spaceship_icon"));

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

QString MainWindow::some_var() const
{
    return m_some_var;
}

void MainWindow::setSome_var(const QString &newSome_var)
{
    if (m_some_var == newSome_var)
        return;
    m_some_var = newSome_var;
    emit some_varChanged();
}

void MainWindow::on_pb_start_program_released()
{
    auto current_dir = QCoreApplication::applicationDirPath();
    qDebug() << "Current dir is:" << current_dir;

    auto program = QString("%1/%2").arg(current_dir).arg("nic.exe");
    QStringList arguments;
    // arguments << "help";
    qDebug() << "Program is:" << program;

    QFile file(program);

    if (not file.exists()) 
    {
        qDebug() << "File does not exist.";
        return;
    }

    QProcess *my_process = new QProcess(this);

    connect(my_process, &QProcess::readyReadStandardOutput,
            this, [this, my_process]()
            {
                QByteArray data = my_process->readAllStandardOutput();

                char char_to_remove = '\0';
                int index = data.indexOf(char_to_remove);
                while (index != -1)
                {
                    data.remove(index, 1); // Remove 1 character at index
                    index = data.indexOf(char_to_remove, index); // Find next occurrence
                }


                QString output = QString::fromUtf8(data);

                // qDebug() << "Standard Output: " << output;
                this->ui->plainTextEdit->clear();
                this->ui->plainTextEdit->appendPlainText(output);

            });

    my_process->start(program, arguments);

}


void MainWindow::on_pb_IP_released()
{
    auto* manager = new QNetworkAccessManager(this);

    QUrl url("https://api.ipify.org?format=json");
    auto* request = new QNetworkRequest(url);

    // Optionally, configure SSL if necessary (e.g., for self-signed certificates)
    QSslConfiguration sslConfig = request->sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request->setSslConfiguration(sslConfig);

    // Make the HTTPS GET request
    QNetworkReply *reply = manager->get(*request);

    // Connect signals to handle the response
    QObject::connect(reply, &QNetworkReply::finished, this,
                     [this, reply, request, manager]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            // Successful request, read the response data
            QByteArray responseData = reply->readAll();
            qDebug() << "Response:" << responseData;



            // Parse the JSON input
            QJsonParseError error;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &error);

            if (error.error != QJsonParseError::NoError)
            {
                qDebug() << "Parse error:" << error.errorString();
                return;
            }

            if (jsonDoc.isObject())
            {
                QJsonObject jsonObj = jsonDoc.object();
                if (jsonObj.contains("ip"))
                {
                    auto the_ip = jsonObj["ip"].toString("??????");
                    this->ui->plainTextEdit->clear();
                    this->ui->plainTextEdit->appendPlainText(the_ip);
                }
            }
        }
        else
        {
            // Error handling
            qDebug() << "Error:" << reply->errorString();
        }

        // Clean up
        reply->deleteLater();
        delete request;
        manager->deleteLater();
    });
}

