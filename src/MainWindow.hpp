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
#include <QLineEdit>
#include <QMessageBox>
#include "BoxWindow.hpp"
#include "Enum.hpp"

class MainWindow : public QWidget {

protected:
    QTimer *timer_;

    QVBoxLayout *main_layout_;
    QHBoxLayout *botton_layout_;
    QHBoxLayout *setup_layout_;
    QHBoxLayout *control_layout_;
    QHBoxLayout *control2_layout_;
    QHBoxLayout *game_layout_;
    QHBoxLayout *score_layout_;

    QPushButton *start_button_;
    QPushButton *reset_button_;
    QPushButton *pause_button_;

    QComboBox *size_combobox_;
    QComboBox *rule_combobox_;
    QComboBox *speed_combobox_;
    BoxWindow *box_window_;
    speedEnum current_speed_;
    ruleEnum current_rule_;

    QPushButton *left_button_;
    QPushButton *right_button_;
    QPushButton *down_button_;
    QPushButton *rotate_button_;
    QPushButton *drop_button_;

    QLineEdit *score_line_;

public:
    MainWindow(QWidget *parent = nullptr) : 
            QWidget(parent) {
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        const int assume_width = 400;
        const int assume_height = 800;
        const int actual_width = this->width();
        const int actual_height = this->height();
        const int initial_block_size = 30.*(actual_width / assume_width);
        const int initial_row_count = 12. * (actual_height / assume_height);
        const int initial_col_count = 6. * (actual_width / assume_width);
        //const int x_margin = 80;
        //const int y_margin = 80;
        const sizeEnum initial_size = sizeEnum::s12x6;
        const ruleEnum initial_rule = ruleEnum::rule1;
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

        rule_combobox_ = new QComboBox(this);
        rule_combobox_->addItem("Rule1");
        rule_combobox_->addItem("Rule2");
        rule_combobox_->addItem("Rule3");
        rule_combobox_->addItem("Rule4");
        rule_combobox_->addItem("Rule5");
        setup_layout_->addWidget(rule_combobox_);

        speed_combobox_ = new QComboBox(this);
        speed_combobox_->addItem("Slow");
        speed_combobox_->addItem("Normal");
        speed_combobox_->addItem("Fast");
        speed_combobox_->setCurrentText("Normal");
        setup_layout_->addWidget(new QLabel("Speed:", this));
        setup_layout_->addWidget(speed_combobox_);

        main_layout_->addLayout(setup_layout_);
        main_layout_->addLayout(botton_layout_);

        score_layout_ = new QHBoxLayout();
        score_line_ = new QLineEdit(this);
        score_line_->setReadOnly(true);
        score_line_->setText("Score: 0");
        score_line_->setAlignment(Qt::AlignCenter);
        score_line_->setFocusPolicy(Qt::NoFocus);
        score_layout_->addWidget(score_line_);
        main_layout_->addLayout(score_layout_);

        game_layout_ = new QHBoxLayout();
        box_window_ = new BoxWindow(assume_width,
                                    assume_height,
                                    initial_block_size*initial_col_count, 
                                    initial_block_size*initial_row_count,
                                    initial_size,
                                    initial_rule,
                                    this);
        box_window_->initialize(initial_size, initial_rule);
        game_layout_->addWidget(box_window_);
        main_layout_->addLayout(game_layout_);

        control2_layout_ = new QHBoxLayout();
        rotate_button_ = new QPushButton("Rotate", this);
        drop_button_ = new QPushButton("Drop", this);
        control2_layout_->addWidget(rotate_button_);
        control2_layout_->addWidget(drop_button_);
        main_layout_->addLayout(control2_layout_);

        control_layout_ = new QHBoxLayout();
        left_button_ = new QPushButton("Left", this);
        left_button_->setIcon(this->style()->standardIcon(QStyle::SP_ArrowLeft));
        right_button_ = new QPushButton("Right", this);
        right_button_->setIcon(this->style()->standardIcon(QStyle::SP_ArrowRight));
        down_button_ = new QPushButton("Down", this);
        down_button_->setIcon(this->style()->standardIcon(QStyle::SP_ArrowDown));
        control_layout_->addWidget(left_button_);
        control_layout_->addWidget(down_button_);
        control_layout_->addWidget(right_button_);
        main_layout_->addLayout(control_layout_);



        //const int window_size_x = initial_block_size*initial_col_count + x_margin;
        //const int window_size_y = initial_block_size*initial_row_count + y_margin;
        //setFixedSize(window_size_x, window_size_y);
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        connect(start_button_, &QPushButton::clicked, this, &MainWindow::onStartClicked);
        connect(reset_button_, &QPushButton::clicked, this, &MainWindow::onResetClicked);
        connect(pause_button_, &QPushButton::clicked, this, &MainWindow::onPauseClicked);
        connect(rule_combobox_, &QComboBox::currentTextChanged, this, &MainWindow::onRuleChanged);
        connect(size_combobox_, &QComboBox::currentTextChanged, this, &MainWindow::onSizeChanged);
        connect(left_button_, &QPushButton::clicked, this, &MainWindow::onLeftClicked);
        connect(right_button_, &QPushButton::clicked, this, &MainWindow::onRightClicked);
        connect(down_button_, &QPushButton::clicked, this, &MainWindow::onDownClicked);
        connect(rotate_button_, &QPushButton::clicked, this, &MainWindow::onRotateClicked);
        connect(drop_button_, &QPushButton::clicked, this, &MainWindow::onDropClicked);
        connect(speed_combobox_, &QComboBox::currentTextChanged, this, &MainWindow::onSpeedChanged);

        timer_ = new QTimer(this);
        connect(timer_, &QTimer::timeout, this, &MainWindow::gameLoop);

        start_button_->setEnabled(true);
        reset_button_->setEnabled(false);
        pause_button_->setEnabled(false);

        rule_combobox_->setEnabled(true);
        size_combobox_->setEnabled(true);
        speed_combobox_->setEnabled(true);
    }

    void gameLoop() {
        if (box_window_->isGameOver()) {
            timer_->stop();

            QMessageBox::information(this, "Game Over", "Game Over!! Your score: " + QString::number(box_window_->getScore()));
            return;
        }
        if(box_window_->isStart() == false) {
            return;
        }
        if(box_window_->isPause()) {
            return;
        }
        box_window_->stepExecute();
        score_line_->setText(QString("Score: %1").arg(box_window_->getScore()));
        update();
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Left) {
            //qDebug() << "Left key pressed";
            box_window_->decrementCurrentX();
        } else if (event->key() == Qt::Key_Right) {
            //qDebug() << "Right key pressed";
            box_window_->incrementCurrentX();
        } else if (event->key() == Qt::Key_Down) {
            //qDebug() << "Down key pressed";
            box_window_->incrementCurrentY();
        } else if (event->key() == Qt::Key_Up) {
            //qDebug() << "Up key pressed";
            box_window_->rotateCurrentBlock();
        } else if (event->key() == Qt::Key_Space) {
            //qDebug() << "Space key pressed";
            box_window_->dropCurrentBlock();
        }
        update(); 
    }

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

        rule_combobox_->setEnabled(false);
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

        rule_combobox_->setEnabled(true);
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

    void onRuleChanged(const QString &text) {
        if (text == "Rule1") {
            current_rule_ = ruleEnum::rule1;
        } else if (text == "Rule2") {
            current_rule_ = ruleEnum::rule2;
        } else if (text == "Rule3") {
            current_rule_ = ruleEnum::rule3;
        } else if (text == "Rule4") {
            current_rule_ = ruleEnum::rule4;
        } else if (text == "Rule5") {
            current_rule_ = ruleEnum::rule5;
        }
        box_window_->setRule(current_rule_);
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

    void onDownClicked() {
        box_window_->incrementCurrentY();
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