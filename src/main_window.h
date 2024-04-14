#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QString some_var READ some_var WRITE setSome_var NOTIFY some_varChanged FINAL)

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


// QWidget interface
    QString some_var() const;
    void setSome_var(const QString &newSome_var);

signals:
    void some_varChanged();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;


private slots:
    void on_pb_next_released();
    void on_pb_prev_released();

    void on_pb_start_program_released();

private:
    Ui::MainWindow *ui;

    QString m_some_var;
};
#endif // MAIN_WINDOW_H
