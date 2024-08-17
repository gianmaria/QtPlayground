#include "main_window.h"
#include "./ui_main_window.h"

#include <utility>

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
#include <QTextBoundaryFinder>

#include <private/qstringiterator_p.h>

void foo(QStringView sv)
{
    int stop = 0;
}


void iterateOverCodePoints(QStringView str) 
{
    static_assert(sizeof(QChar) == 2);

    for (qsizetype i = 0; 
         i < str.size(); 
         ++i) 
    {
        const QChar& ch = str.at(i);
        
        uint32_t codePoint = 0;
        if (not ch.isSurrogate())
        {
            // not a surrogate, lies in the BMP,
            // just grap the unicode code point 
            codePoint = ch.unicode();
        }
        else 
        {
            if (i + 1 < str.size() and
                str.at(i).isHighSurrogate() and
                str.at(i + 1).isLowSurrogate())
            {
                const QChar& highSurrogate = str.at(i);
                const QChar& lowSurrogate = str.at(i + 1);
                codePoint = QChar::surrogateToUcs4(highSurrogate, lowSurrogate);
                
                i++; // Skip the low surrogate
            }
        }

        qDebug() << QString("U+%1").arg(codePoint, 0, 16);
    }
}


auto count_grapheme_clusters(QStringView str, bool debug = false)
{
    auto tbf = QTextBoundaryFinder(QTextBoundaryFinder::Grapheme, str);
    tbf.toStart();

    if (debug)
    {
        auto raw = str.toUtf8();
        raw = raw.toHex(':');
        qDebug() << raw;

        /*qDebug() << "---------------------------------";
        for (const auto& item : raw)
        {
            qDebug() << item;
        }
        qDebug() << "---------------------------------";*/
    }

    uint32_t len = 0;
    while (true)
    {
        qsizetype start = tbf.position();
        qsizetype end = tbf.toNextBoundary();

        if (end == -1)
            break;

        if (debug) qDebug() << "start byte:" << start << "end byte:" << end;

        ++len;
    }

    return len;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/spaceship_icon"));


    {

        {
            auto complex = QString("ab👍c");
            qDebug() << "complex is:" << qUtf8Printable(complex)
                     << "size:" << complex.size();

            //qDebug() << "c0 is:" << complex.at(0);
            //qDebug() << "c1 is:" << complex.at(1);

            //auto utf16_encoder = QStringEncoder(QStringEncoder::Utf16);
            //QByteArray encoded_string = utf16_encoder(complex);

            for (qsizetype i = 0;
                 i < complex.size();
                 ++i)
            {
                const QChar& c = complex.at(i);
                qDebug() << QString("[%1] %2")
                                .arg(i)
                                .arg(c.unicode(), 4, 16, QChar('0'));

            }

            const auto* rainbow = "👍";

            if (complex.contains(rainbow))
            {
                qDebug() << "we have a rainbow!";

                auto pos = complex.indexOf(rainbow);
                auto len = QString(rainbow).size();

                qDebug() << "at pos:" << pos;
                qDebug() << "len:" << len;

                QString substr = complex.sliced(pos, len);
                qDebug() << "it is:" << substr;
            }

            //iterateOverCodePoints(complex);

            int stop = 0;
        }
        {
            auto asdasdas = QString::fromUtf8("👍 🏻");
            qDebug() << "asdasdas is:" << qUtf8Printable(asdasdas);
            qDebug() << "len of asdasdas:" << count_grapheme_clusters(asdasdas);
        }

        {
            const char* sleep = "😴";
            auto s = QString::fromUtf8("😴");
            qDebug() << "s is:" << qUtf8Printable(s);
            qDebug() << "len of s:" << count_grapheme_clusters(s);

            // auto s2 = QString::fromLatin1("hello", -1);
            // auto s3 = QString::fromLocal8Bit(sleep, -1); // On Windows the system's current code page is used.

            const QChar c1 = s.at(0);
            s.append(c1);
            qDebug() << "s + c1 is:" << qUtf8Printable(s);

            QByteArray ba;
            ba.resize(2);
            ba[0] = (uint16_t)0x34;
            ba[1] = (uint16_t)0xde;

            const auto c2 = QChar::fromUcs2(0xde34);
            s.append(c2);
            qDebug() << "s + c2 is:" << qUtf8Printable(s);

            foo(s);

            s = QString::fromUtf8("hello 😴 world");
            qDebug() << "string is:" << qUtf8Printable(s);

            bool res = s.contains(sleep);
            auto index = s.indexOf(sleep);

            s.replace(sleep, "🤤");
            qDebug() << "s is:" << qUtf8Printable(s);
            qDebug() << "s is:" << s;


            for (const QChar& c : std::as_const(s))
            {
                qDebug() << "c is:" << c;
            }
        }

        {
            auto s = QString::fromUtf8("👍🏻");
            qDebug() << "qUtf8Printable, s is:" << qUtf8Printable(s);
            qDebug() << "s is:" << s;
            qDebug() << "len of s:" << count_grapheme_clusters(s);
            int stop = 0;
        }

        {
            const char* sleep_c = "👍🏻";
            auto sleep = QString(sleep_c);

            auto available_codecs = QStringConverter::availableCodecs();
            
            qDebug() << "available codecs:";
            for (const auto& codec : available_codecs)
            {
                qDebug() << codec;
            }


            {
                auto utf32_encoder = QStringEncoder(QStringEncoder::Utf32);
                QByteArray encoded_string = utf32_encoder(sleep);
                qDebug() << "sleep:" << sleep;
                qDebug() << "**dangerous** sleep len is:" << encoded_string.length() / 4;
                qDebug() << "**safe** sleep len is:" << count_grapheme_clusters(sleep);

                int stop = 0;
            }

            {
                auto str = QString("🇩🇪🏳️‍🌈");
                qDebug("str: '%s'", qUtf8Printable(str));
                qDebug() << "count str:" << count_grapheme_clusters(str, true);
                

                auto si = QStringIterator(str);
                while (si.hasNext())
                {
                    auto code_point = si.next();
                    qDebug() << "code_point is:" << code_point;
                }

                {
                    auto pi = QChar(u'π');
                    auto res = pi.unicode();
                    auto surr = pi.isSurrogate();
                }

                {
                    qDebug() << "**************************************";
                    qDebug() << "**************************************";
                    qDebug() << "**************************************";

                    // 👍 -> U+1F44D
                    auto c1 = QChar(0xD83D);
                    auto c2 = QChar(0xDC4D);
                    //auto res = c1.unicode();
                    
                    auto surr_c1 = c1.isSurrogate();
                    auto surr_c2 = c2.isSurrogate();
                    
                    auto hugh_surr_c1 = c1.isHighSurrogate();
                    auto low_surr_c2 = c2.isLowSurrogate();

                    auto tmp = QString("👍");
                    tmp.clear();
                    tmp.append(c1);
                    tmp.append(c2);

                    QString zuzu = "Hello, 世界! 👋🌍";
                    iterateOverCodePoints(zuzu);

                    qDebug("tmp is: %s", qUtf8Printable(tmp));

                    si = QStringIterator(tmp);
                    while (si.hasNext())
                    {
                        auto code_point = si.next();
                        qDebug() << "tmp code_point is:" << code_point;
                    }

                    for (const auto& elem : tmp)
                    {
                        qDebug() << elem;
                    }

                    qDebug() << "tmp grapheme cluster len is:" << count_grapheme_clusters(tmp);

                    int stop = 0;
                }

                auto xx = QString(u8"∃y ∀x ¬(x ≺ y)");
                int stop = 0;
            }
        }


        {
            const char* c_str = "👍"; // in memory use utf8 encoding
            const wchar_t* w_str = L"👍"; // in memory use utf16 encoding
            const char8_t* u8_str = u8"👍"; // in memory use utf8 encoding
            const char16_t* u16_str = u"👍"; // in memory use utf16 encoding

            auto test1 = QString::fromUtf8(c_str);
            auto test2 = QString::fromWCharArray(w_str);
            auto test3 = QString::fromUtf8(u8_str);
            auto test4 = QString::fromUtf16(u16_str);

            int stop = 0;
        }
        int stop = 0;
    }




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

    auto program = QString("%1/%2").arg(current_dir, "nic.exe");
    QStringList arguments;
    // arguments << "help";
    qDebug() << "Program is:" << program;

    QFile file(program);

    if (not file.exists()) 
    {
        qDebug() << "File does not exist.";
        this->ui->statusbar->showMessage(QString("Error! file '%1' does not exist!").arg(program), 3000);
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


void MainWindow::on_pb_next_clicked()
{

}

