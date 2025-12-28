// mainwindow.cpp
#include <QDebug>
#include <QDir>
#include <QMovie>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QRandomGenerator>
#include <QVector>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include <algorithm>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Болотуду 5x5");

    QPushButton* saveBtn = findChild<QPushButton*>("btnSave");
    QPushButton* loadBtn = findChild<QPushButton*>("btnLoad");
    QPushButton* resetBtn = findChild<QPushButton*>("btnReset");

    if (saveBtn) {
        connect(saveBtn, &QPushButton::clicked, this, &MainWindow::on_btnSave_clicked);
        qDebug() << "btnSave подключён успешно";
    } else {
        qDebug() << "ОШИБКА: btnSave не найден!";
    }

    if (loadBtn) {
        connect(loadBtn, &QPushButton::clicked, this, &MainWindow::on_btnLoad_clicked);
        qDebug() << "btnLoad подключён успешно";
    } else {
        qDebug() << "ОШИБКА: btnLoad не найден!";
    }

    if (resetBtn) {
        connect(resetBtn, &QPushButton::clicked, this, &MainWindow::on_btnReset_clicked);
        qDebug() << "btnReset подключён успешно";
    } else {
        qDebug() << "ОШИБКА: btnReset не найден!";
    }

    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        QString name = QString("pushButton_%1").arg(i + 1);
        buttonsArray[i] = findChild<QPushButton*>(name);
        if (buttonsArray[i]) {
            connect(buttonsArray[i], &QPushButton::clicked, [this, i]() { playerTurn(i); });
            buttonsArray[i]->setFlat(true);
            buttonsArray[i]->setStyleSheet("QPushButton { background-color: white; border: 2px solid black; border-radius: 0px; }");
        } else {
            qDebug() << "Предупреждение: не найдена игровая кнопка" << name;
        }
    }

    choiceWindow();
    reset();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::choiceWindow()
{
    QDialog d(this);
    d.setWindowTitle("Болотуду 5x5");

    QVBoxLayout *mainLayout = new QVBoxLayout(&d);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    QLabel *title = new QLabel("Выберите режим игры");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #00FFFF;");
    mainLayout->addWidget(title);

    QHBoxLayout *gifAndOkLayout = new QHBoxLayout();
    gifAndOkLayout->setSpacing(40);

    QLabel *gifLabel = new QLabel();
    gifLabel->setFixedSize(498, 422);
    gifLabel->setAlignment(Qt::AlignCenter);

    QMovie *movie = new QMovie(":/coin.gif");  
    if (movie->isValid()) {
        gifLabel->setMovie(movie);
        movie->start();
    } else {
        gifLabel->setText("Гифка не загрузилась");
    }
    gifAndOkLayout->addWidget(gifLabel);

   
    QPushButton *okButton = new QPushButton("ОК");
    okButton->setFixedSize(150, 150);
    okButton->setStyleSheet(
        "QPushButton {"
        "    font-size: 36px;"
        "    font-weight: bold;"
        "    color: #00FFFF;"          
        "    background-color: rgba(0, 0, 0, 100);"
        "    border: 3px solid #00FFFF;"
        "    border-radius: 75px;"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(0, 255, 255, 50);"
        "}"
      
        "QPushButton {"
        "    text-shadow:"
        "        0 0 10px #00FFFF,"
        "        0 0 20px #00FFFF,"
        "        0 0 30px #00FFFF,"
        "        0 0 40px #0000FF;"
        "}"
    );
    connect(okButton, &QPushButton::clicked, &d, &QDialog::accept);

    gifAndOkLayout->addWidget(okButton);
    mainLayout->addLayout(gifAndOkLayout);

    QRadioButton *radio1 = new QRadioButton("С ботом");
    QRadioButton *radio2 = new QRadioButton("С игроком");
    radio1->setStyleSheet("font-size: 18px;");
    radio2->setStyleSheet("font-size: 18px;");
    radio2->setChecked(true);

    mainLayout->addWidget(radio1);
    mainLayout->addWidget(radio2);

    QLabel *rulesTitle = new QLabel("Правила");
    rulesTitle->setAlignment(Qt::AlignCenter);
    rulesTitle->setStyleSheet(
        "font-size: 32px; font-weight: bold; color: #00FFFF;"
        "text-shadow: 0 0 10px #00FFFF, 0 0 20px #00FFFF, 0 0 30px #00FFFF, 0 0 40px #0000FF;"
    );
    mainLayout->addWidget(rulesTitle);

    QLabel *rulesText = new QLabel(
        "• Игра на поле 5x5.\n"
        "• Игроки по очереди ставят свои фишки.\n"
        "• Победитель — первый с 3 фишками в ряд (горизонталь, вертикаль, диагональ).\n"
        "• Ничья при полном поле без победителя.\n"
        "• Удачи!"
    );
    rulesText->setAlignment(Qt::AlignCenter);
    rulesText->setWordWrap(true);
    rulesText->setStyleSheet(
        "font-size: 16px; color: white; background-color: rgba(0, 0, 0, 150); padding: 15px; border-radius: 10px;"
    );
    mainLayout->addWidget(rulesText);

    mainLayout->addStretch();
    d.resize(600, 750);
    d.exec();

    bot = radio1->isChecked();
    loginWindow(bot);
}

void MainWindow::loginWindow(bool isBot)
{
    QDialog d(this);
    QFormLayout form;
    QLineEdit name1(&d);
    QLineEdit name2(&d);
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok);
    connect(&buttonBox, &QDialogButtonBox::accepted, &d, &QDialog::accept);

    form.addRow("Первый игрок:", &name1);
    if (!isBot) form.addRow("Второй игрок:", &name2);
    form.addRow(&buttonBox);
    d.setLayout(&form);
    d.setWindowTitle("Представьтесь:");

    if (d.exec() == QDialog::Accepted) {
        player1 = name1.text().isEmpty() ? "Игрок 1" : name1.text();
        if (!isBot) player2 = name2.text().isEmpty() ? "Игрок 2" : name2.text();
    }
}

void MainWindow::update()
{
    ui->label->setText(gameOver ? (clickCounter == BOARD_SIZE*BOARD_SIZE ? "Ничья" : "Победил") : "Ходит");
    ui->label_2->setText(gameOver ? "" : (clickCounter % 2 == 0 ? player1 : (bot ? "Бот" : player2)));
    ui->labelPlayer1->setText(player1 + ":");
    ui->labelPlayer2->setText((bot ? "Бот" : player2) + ":");
    ui->labelPlayer1Res->setText(QString::number(player1Res));
    ui->labelPlayer2Res->setText(QString::number(player2Res));
    ui->labelCountRes->setText(QString::number(gameCounter));
}

void MainWindow::reset()
{
    gameOver = false;
    clickCounter = 0;
    std::fill(std::begin(clickedButton), std::end(clickedButton), 0);
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        if (buttonsArray[i]) {
            buttonsArray[i]->setStyleSheet("QPushButton { background-color: white; border: 2px solid black; border-radius: 0px; }");
        }
    }
    update();
}

void MainWindow::playerTurn(int turn)
{
    if (gameOver || clickedButton[turn] != 0 || !buttonsArray[turn]) return;

    clickCounter++;
    int player = (clickCounter % 2 == 1) ? 1 : 2;
    clickedButton[turn] = player;

    QString style = (player == 1) ?
        "QPushButton { background-color: black; border: 2px solid black; border-radius: 50px; }" :
        "QPushButton { background-color: #800020; border: 2px solid black; border-radius: 50px; }";

    buttonsArray[turn]->setStyleSheet(style);

    if (winner(player)) {
        gameOver = true;
        (player == 1 ? player1Res : player2Res)++;
        gameCounter++;
        update();
        return;
    }

    if (clickCounter == BOARD_SIZE * BOARD_SIZE) {
        gameOver = true;
        gameCounter++;
        update();
        return;
    }

    update();

    if (bot && player == 1) {
        QVector<int> free;
        for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
            if (clickedButton[i] == 0) free << i;
        if (!free.isEmpty()) {
            int botTurn = free[QRandomGenerator::global()->bounded(free.size())];
            QTimer::singleShot(500, this, [this, botTurn]() { playerTurn(botTurn); });
        }
    }
}

bool MainWindow::winner(int player)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c <= BOARD_SIZE - WIN_STREAK; ++c) {
            bool win = true;
            for (int k = 0; k < WIN_STREAK; ++k)
                if (clickedButton[r * BOARD_SIZE + c + k] != player) { win = false; break; }
            if (win) return true;
        }

    for (int c = 0; c < BOARD_SIZE; ++c)
        for (int r = 0; r <= BOARD_SIZE - WIN_STREAK; ++r) {
            bool win = true;
            for (int k = 0; k < WIN_STREAK; ++k)
                if (clickedButton[(r + k) * BOARD_SIZE + c] != player) { win = false; break; }
            if (win) return true;
        }

    for (int r = 0; r <= BOARD_SIZE - WIN_STREAK; ++r)
        for (int c = 0; c <= BOARD_SIZE - WIN_STREAK; ++c) {
            bool win = true;
            for (int k = 0; k < WIN_STREAK; ++k)
                if (clickedButton[(r + k) * BOARD_SIZE + (c + k)] != player) { win = false; break; }
            if (win) return true;
        }

    for (int r = 0; r <= BOARD_SIZE - WIN_STREAK; ++r)
        for (int c = WIN_STREAK - 1; c < BOARD_SIZE; ++c) {
            bool win = true;
            for (int k = 0; k < WIN_STREAK; ++k)
                if (clickedButton[(r + k) * BOARD_SIZE + (c - k)] != player) { win = false; break; }
            if (win) return true;
        }

    return false;
}

void MainWindow::on_btnSave_clicked()
{
    QString filePath = QDir::homePath() + "/savegame_tictactoe.dat";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "ОШИБКА сохранения: не удалось открыть файл" << filePath;
        return;
    }
    QDataStream out(&file);
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) out << clickedButton[i];
    out << player1 << player1Res << player2 << player2Res
        << clickCounter << bot << gameOver << gameCounter;
    file.close();
    qDebug() << "Игра успешно сохранена в" << filePath;
}

void MainWindow::on_btnLoad_clicked()
{
    QString filePath = QDir::homePath() + "/savegame_tictactoe.dat";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "ОШИБКА загрузки: файл не найден или не открывается" << filePath;
        return;
    }
    QDataStream in(&file);
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        in >> clickedButton[i];
        if (buttonsArray[i]) {
            if (clickedButton[i] == 1)
                buttonsArray[i]->setStyleSheet("QPushButton { background-color: black; border: 2px solid black; border-radius: 50px; }");
            else if (clickedButton[i] == 2)
                buttonsArray[i]->setStyleSheet("QPushButton { background-color: #800020; border: 2px solid black; border-radius: 50px; }");
            else
                buttonsArray[i]->setStyleSheet("QPushButton { background-color: white; border: 2px solid black; border-radius: 0px; }");
        }
    }
    in >> player1 >> player1Res >> player2 >> player2Res
       >> clickCounter >> bot >> gameOver >> gameCounter;
    update();
    file.close();
    qDebug() << "Игра успешно загружена из" << filePath;
}


void MainWindow::on_btnReset_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Сброс игры",
                                  "Начать новую партию с теми же игроками?\n\n"
                                  "Да — новая партия\n"
                                  "Нет — полный рестарт (выбор режима заново)",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        reset();         
    } else {
        choiceWindow(); 
    }
}