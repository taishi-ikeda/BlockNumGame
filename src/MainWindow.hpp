#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QDebug>
#include "BoxWindow.hpp"
#include "Enum.hpp"

class MainWindow : public QWidget {

protected:
    QTimer *timer_;

    QVBoxLayout *main_layout_;
    QHBoxLayout *botton_layout_;
    QHBoxLayout *setup_layout_;
    QHBoxLayout *control_layout_;
    QHBoxLayout *game_layout_;

    QPushButton *start_button_;
    QPushButton *reset_button_;
    QPushButton *pause_button_;

    QComboBox *size_combobox_;
    QComboBox *blockNum_combobox_;
    QComboBox *speed_combobox_;
    BoxWindow *box_window_;
    speedEnum current_speed_;

    QPushButton *left_button_;
    QPushButton *right_button_;
    QPushButton *rotate_button_;
    QPushButton *drop_button_;

public:
    MainWindow(QWidget *parent = nullptr) : 
            QWidget(parent) {
        const int initial_block_size = 40;
        const int initial_row_count = 12;
        const int initial_col_count = 6;
        const int x_margin = 80;
        const int y_margin = 80;
        const sizeEnum initial_size = sizeEnum::s12x6;
        const blockNumEnum initial_blockNum = blockNumEnum::sum4b1;
        current_speed_ = speedEnum::Normal;
        main_layout_ = new QVBoxLayout(this);
    
        botton_layout_ = new QHBoxLayout();
        start_button_  = new QPushButton("Start", this);
        reset_button_  = new QPushButton("Reset", this);
        pause_button_  = new QPushButton("Pause", this);
        botton_layout_->addWidget(start_button_);
        botton_layout_->addWidget(reset_button_);
        botton_layout_->addWidget(pause_button_);

        setup_layout_  = new QHBoxLayout();
        size_combobox_ = new QComboBox(this);
        size_combobox_->addItem("12x6");
        size_combobox_->addItem("14x7");
        size_combobox_->addItem("16x8");
        size_combobox_->addItem("18x9");
        setup_layout_->addWidget(new QLabel("Size:", this));
        setup_layout_->addWidget(size_combobox_);

        blockNum_combobox_ = new QComboBox(this);
        blockNum_combobox_->addItem("block_max:1, sum:4");
        blockNum_combobox_->addItem("block_max:2, sum:10");
        blockNum_combobox_->addItem("block_max:5, sum:20");
        setup_layout_->addWidget(new QLabel("Block Number:", this));
        setup_layout_->addWidget(blockNum_combobox_);

        speed_combobox_ = new QComboBox(this);
        speed_combobox_->addItem("Slow");
        speed_combobox_->addItem("Normal");
        speed_combobox_->addItem("Fast");
        speed_combobox_->setCurrentText("Normal");
        setup_layout_->addWidget(new QLabel("Speed:", this));
        setup_layout_->addWidget(speed_combobox_);

        main_layout_->addLayout(setup_layout_);

        main_layout_->addLayout(botton_layout_);

        game_layout_ = new QHBoxLayout();
        box_window_ = new BoxWindow(initial_block_size*initial_col_count, 
                                    initial_block_size*initial_row_count,
                                    initial_size,
                                    initial_blockNum,
                                    this);
        box_window_->initialize(initial_size, initial_blockNum);
        game_layout_->addWidget(box_window_);
        main_layout_->addLayout(game_layout_);

        control_layout_ = new QHBoxLayout();
        left_button_ = new QPushButton("Left", this);
        right_button_ = new QPushButton("Right", this);
        rotate_button_ = new QPushButton("Rotate", this);
        drop_button_ = new QPushButton("Drop", this);
        control_layout_->addWidget(left_button_);
        control_layout_->addWidget(right_button_);
        control_layout_->addWidget(rotate_button_);
        control_layout_->addWidget(drop_button_);
        main_layout_->addLayout(control_layout_);

        // ウィンドウサイズの設定
        const int window_size_x = initial_block_size*initial_col_count + x_margin;
        const int window_size_y = initial_block_size*initial_row_count + y_margin;
        //setFixedSize(window_size_x, window_size_y);
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        connect(start_button_, &QPushButton::clicked, this, &MainWindow::onStartClicked);
        connect(reset_button_, &QPushButton::clicked, this, &MainWindow::onResetClicked);
        connect(pause_button_, &QPushButton::clicked, this, &MainWindow::onPauseClicked);
        connect(blockNum_combobox_, &QComboBox::currentTextChanged, this, &MainWindow::onBlockNumChanged);
        connect(size_combobox_, &QComboBox::currentTextChanged, this, &MainWindow::onSizeChanged);
        connect(left_button_, &QPushButton::clicked, this, &MainWindow::onLeftClicked);
        connect(right_button_, &QPushButton::clicked, this, &MainWindow::onRightClicked);
        connect(rotate_button_, &QPushButton::clicked, this, &MainWindow::onRotateClicked);
        connect(drop_button_, &QPushButton::clicked, this, &MainWindow::onDropClicked);
        connect(speed_combobox_, &QComboBox::currentTextChanged, this, &MainWindow::onSpeedChanged);

        // タイマーの設定（1秒ごとに落下）
        timer_ = new QTimer(this);
        connect(timer_, &QTimer::timeout, this, &MainWindow::gameLoop);

        start_button_->setEnabled(true);
        reset_button_->setEnabled(false);
        pause_button_->setEnabled(false);

        blockNum_combobox_->setEnabled(true);
        size_combobox_->setEnabled(true);
        speed_combobox_->setEnabled(true);
    }

    void gameLoop() {
        if (box_window_->isGameOver()) {
            timer_->stop();
            setWindowTitle("Game Over!");
            return;
        }
        if(box_window_->isStart() == false) {
            return;
        }
        if(box_window_->isPause()) {
            return;
        }
        box_window_->stepExecute();
        update(); // 画面再描画
    }

protected:
    // キーボード入力処理
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Left) {
            box_window_->decrementCurrentX();
        } else if (event->key() == Qt::Key_Right) {
            box_window_->incrementCurrentX();
        } else if (event->key() == Qt::Key_Down) {
            box_window_->incrementCurrentY();
        } else if (event->key() == Qt::Key_Up) {
            box_window_->rotateCurrentBlock();
        } else if (event->key() == Qt::Key_Space) {
            box_window_->dropCurrentBlock();
        }
        update(); // 画面を更新
    }

    // 描画処理（1マスの四角形の中に文字を描く）
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        if(box_window_->isPause()) {
            return;
        }
        box_window_->update();
    }

public slots:
    void onStartClicked() {
        if(current_speed_ == speedEnum::Slow) {
            timer_->start(1000);
            timer_->setInterval(1000);
        } else if(current_speed_ == speedEnum::Normal) {
            timer_->start(500);
            timer_->setInterval(500);
        } else if(current_speed_ == speedEnum::Fast) {
            timer_->start(200);
            timer_->setInterval(200);
        }

        start_button_->setEnabled(false);
        reset_button_->setEnabled(true);
        pause_button_->setEnabled(true);

        blockNum_combobox_->setEnabled(false);
        size_combobox_->setEnabled(false);
        speed_combobox_->setEnabled(false);

        box_window_->start();
    }

    void onResetClicked() {
        box_window_->reset();
        box_window_->stop();
        timer_->stop();
        start_button_->setEnabled(true);
        reset_button_->setEnabled(false);
        pause_button_->setEnabled(false);

        blockNum_combobox_->setEnabled(true);
        size_combobox_->setEnabled(true);
        speed_combobox_->setEnabled(true);
    }

    void onPauseClicked() {
        if (timer_->isActive()) {
            timer_->stop();
            start_button_->setEnabled(false);
            reset_button_->setEnabled(true);
            pause_button_->setEnabled(true);
        } else {
            if(current_speed_ == speedEnum::Slow) {
                timer_->start(1000);
            } else if(current_speed_ == speedEnum::Normal) {
                timer_->start(500);
            } else if(current_speed_ == speedEnum::Fast) {
                timer_->start(200);
            }
            start_button_->setEnabled(false);
            reset_button_->setEnabled(true);
            pause_button_->setEnabled(true);
        }
    }

    void onBlockNumChanged(const QString &text) {
        if (text == "block_max:1, sum:4") {
            box_window_->setBlockNum(blockNumEnum::sum4b1);
        } else if (text == "block_max:2, sum:10") {
            box_window_->setBlockNum(blockNumEnum::sum10b2);
        } else if (text == "block_max:5, sum:20") {
            box_window_->setBlockNum(blockNumEnum::sum20b5);
        }
    }

    void onSizeChanged(const QString &text) {
        if (text == "12x6") {
            box_window_->setSize(sizeEnum::s12x6);
        } else if (text == "14x7") {
            box_window_->setSize(sizeEnum::s14x7);
        } else if (text == "16x8") {
            box_window_->setSize(sizeEnum::s16x8);
        } else if (text == "18x9") {
            box_window_->setSize(sizeEnum::s19x9);
        }
    }

    void onLeftClicked() {
        box_window_->decrementCurrentX();
    }

    void onRightClicked() {
        box_window_->incrementCurrentX();
    }

    void onRotateClicked() {
        box_window_->rotateCurrentBlock();
    }

    void onDropClicked() {
        box_window_->dropCurrentBlock();
    }

    void onSpeedChanged(const QString &text) {
        if (text == "Slow") {
            current_speed_ = speedEnum::Slow;
        } else if (text == "Normal") {
            current_speed_ = speedEnum::Normal;
        } else if (text == "Fast") {
            current_speed_ = speedEnum::Fast;
        }
    }

};

#endif // MAINWINDOW_HPP