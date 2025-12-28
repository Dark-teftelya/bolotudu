// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_btnSave_clicked();
    void on_btnLoad_clicked();
    void on_btnReset_clicked();
    
private:
    Ui::MainWindow *ui;
    bool bot = false;
    bool gameOver = false;
    int clickCounter = 0;
    int gameCounter = 0;
    int player1Res = 0;
    int player2Res = 0;
    QString player1 = "Игрок 1";
    QString player2 = "Игрок 2";

    static constexpr int BOARD_SIZE = 5;
    static constexpr int WIN_STREAK = 3;
    QPushButton* buttonsArray[BOARD_SIZE * BOARD_SIZE]{};
    int clickedButton[BOARD_SIZE * BOARD_SIZE]{};

    void choiceWindow();
    void loginWindow(bool isBot);
    void update();
    void reset();
    void playerTurn(int turn);
    bool winner(int player);
};

#endif