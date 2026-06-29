#ifndef BOXWINDOW_HPP
#define BOXWINDOW_HPP

#include <QWidget>
#include <QPainter>
#include "Enum.hpp"

class BoxWindow : public QWidget {
    Q_OBJECT

public:
    BoxWindow(const int width, 
              const int height,
              const sizeEnum size,
              const ruleEnum rule,
              QWidget *parent = nullptr) 
                : QWidget(parent),
                row_(0), 
                col_(0), 
                blockSize_(0), 
                currentX_(0), 
                currentY_(0),
                game_over_(false),
                start_(false),
                rule_(rule),
                size_(size),
                width_(width),
                height_(height),
                score_(0) {
        //setFixedSize(width, height);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void setSize(sizeEnum size) {
        size_ = size;
        this->initialize(size_, rule_);
    }

    void setRule(ruleEnum rule) {
        rule_ = rule;
        this->initialize(size_, rule_);
    }

    void initialize(sizeEnum size, ruleEnum rule) {
        size_ = size;
        rule_ = rule;

        switch (size_) {
            case sizeEnum::s12x6:
                row_ = 12;
                col_ = 6;
                blockSize_ = 40;
                break;
            case sizeEnum::s14x7:
                row_ = 14;
                col_ = 7;
                blockSize_ = 40*(6./7.);
                break;
            case sizeEnum::s16x8:
                row_ = 16;
                col_ = 8;
                blockSize_ = 40*(6./8.);
                break;
            case sizeEnum::s19x9:
                row_ = 19;
                col_ = 9;
                blockSize_ = 40*(6./9.);
                break;
        }

        width_ = col_ * blockSize_;
        height_ = row_ * blockSize_;
        setFixedSize(width_, height_);

        switch (rule_) {
            case ruleEnum::rule1:
                blockMaxNum_ = 1;
                break;
            case ruleEnum::rule2:
                blockMaxNum_ = 2;
                break;
            case ruleEnum::rule3:
                blockMaxNum_ = 5;
                break;
            case ruleEnum::rule4:
                blockMaxNum_ = 3;
                break;
            case ruleEnum::rule5:
                blockMaxNum_ = 5;
                break;
        }
        board_.clear();
        board_.resize(row_, std::vector<std::pair<blockColor, int>>(col_, {blockColor::None, 0}));
        current_execution_state_ = executionState::pauseGame;
        score_ = 0;
    }

    void start() {
        start_ = true;
        spawnPuyo();
    }

    void stop() {
        start_ = false;
        current_execution_state_ = executionState::pauseGame;
    }

    void reset() {
        board_.clear();
        board_.resize(row_, std::vector<std::pair<blockColor, int>>(col_, {blockColor::None, 0}));
        currentX_ = col_ / 2;
        currentY_ = 0;
        currentBlockInfo_[0] = std::make_pair(static_cast<blockColor>(0), 0);
        currentBlockInfo_[1] = std::make_pair(static_cast<blockColor>(0), 0);
        game_over_ = false;
        score_ = 0;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);

        const int contentWidth = blockSize_ * col_;
        const int contentHeight = blockSize_ * row_;
        int widgetWidth = this->width();
        int widgetHeight = this->height();
        int offsetX = (widgetWidth - contentWidth) / 2;
        int offsetY = (widgetHeight - contentHeight) / 2;

        //painter.fillRect(rect(), Qt::darkGray);
    
        painter.translate(offsetX, offsetY);

        painter.fillRect(rect(), Qt::black);

        for (int r = 0; r < row_; ++r) {
            for (int c = 0; c < col_; ++c) {
                drawBlock(painter, c, r, board_[r][c]);
            }
        }
        if(!isPause()) {
            if(current_execution_state_ == executionState::droppingBlock) {
                switch (currentRotation_)
                {
                    case rotationEnum::BottomUp:
                        drawBlock(painter, currentX_, currentY_, currentBlockInfo_[0]);
                        drawBlock(painter, currentX_, currentY_ - 1, currentBlockInfo_[1]);
                        break;
                    case rotationEnum::UpBottom:
                        drawBlock(painter, currentX_, currentY_, currentBlockInfo_[0]);
                        drawBlock(painter, currentX_, currentY_+1, currentBlockInfo_[1]);
                        break;
                    case rotationEnum::LeftRight:
                        drawBlock(painter, currentX_, currentY_, currentBlockInfo_[0]);
                        drawBlock(painter, currentX_+1, currentY_, currentBlockInfo_[1]);
                        break;
                    case rotationEnum::RightLeft:
                        drawBlock(painter, currentX_, currentY_, currentBlockInfo_[0]);
                        drawBlock(painter, currentX_-1, currentY_, currentBlockInfo_[1]);
                        break;
                    default:
                        break;
                }
            }
        }
    }
public:
    void stepExecute() {
        switch (current_execution_state_)
        {
            case executionState::droppingBlock:
                stepDropBlock();
                break;
            case executionState::erasingBlocks:
                checkAddErase();
                break;
            case executionState::applyingGravity:
                applyGravity();
                break;
            case executionState::spawningBlock:
                spawnPuyo();
                break;
            default:
                break;
        }
    }
protected:
    void stepDropBlock() {
        bool canMoveDown = false;
        switch (currentRotation_)
        {
            case rotationEnum::BottomUp:                    
                canMoveDown = canMove(currentX_, currentY_+1) && canMove(currentX_, currentY_);
                break;
            case rotationEnum::UpBottom:
                canMoveDown = canMove(currentX_, currentY_+2) && canMove(currentX_, currentY_+1);
                break;
            case rotationEnum::LeftRight:
                canMoveDown = canMove(currentX_, currentY_+1) && canMove(currentX_ + 1, currentY_+1);
                break;
            case rotationEnum::RightLeft:
                canMoveDown = canMove(currentX_, currentY_+1) && canMove(currentX_ - 1, currentY_+1);
                break;
            default:
                break;
        }
        if (canMoveDown) {
            currentY_++;
        } else {
            switch (currentRotation_)
            {
                case rotationEnum::BottomUp:
                    board_[currentY_][currentX_] = currentBlockInfo_[0];
                    board_[currentY_-1][currentX_] = currentBlockInfo_[1];
                    break;
                case rotationEnum::UpBottom:
                    board_[currentY_][currentX_] = currentBlockInfo_[0];
                    board_[currentY_+1][currentX_] = currentBlockInfo_[1];
                    break;
                case rotationEnum::LeftRight:
                    board_[currentY_][currentX_] = currentBlockInfo_[0];
                    board_[currentY_][currentX_+1] = currentBlockInfo_[1];
                    break;
                case rotationEnum::RightLeft:
                    board_[currentY_][currentX_] = currentBlockInfo_[0];
                    board_[currentY_][currentX_-1] = currentBlockInfo_[1];
                    break;
                default:
                    break;
            }
            current_execution_state_ = executionState::applyingGravity;
        }
    }

    void spawnPuyo() {
        currentX_ = col_ / 2;
        currentY_ = 0;
        const int currentBlockColor1 = (rand() % 4);
        const int currentBoxNumber1 = (rand() % blockMaxNum_) + 1;
        const int currentBlockColor2 = (rand() % 4) ;
        const int currentBoxNumber2 = (rand() % blockMaxNum_) + 1;
        currentBlockInfo_[0] = std::make_pair(static_cast<blockColor>(currentBlockColor1), 
                            currentBoxNumber1);
        currentBlockInfo_[1] = std::make_pair(static_cast<blockColor>(currentBlockColor2), 
                            currentBoxNumber2);
        if (board_[currentY_][currentX_].first != blockColor::None) {
            game_over_ = true;
        } else {
            game_over_ = false;
        }
        currentRotation_ = rotationEnum::BottomUp;
        current_execution_state_ = executionState::droppingBlock;
    }

    void checkAddErase() {
        while(true) {
            bool anyErased = false;
            std::vector<std::vector<bool>> 
                visited(row_, std::vector<bool>(col_, false));

            for (int r = 0; r < row_; ++r) {
                for (int c = 0; c < col_; ++c) {
                    if (board_[r][c].first == blockColor::None || visited[r][c]) continue;

                    blockColor color = board_[r][c].first;
                    QVector<QPoint> connected;
                    QVector<QPoint> queue;

                    queue.append(QPoint(c, r));
                    visited[r][c] = true;

                    while (!queue.isEmpty()) {
                        QPoint p = queue.takeFirst();
                        connected.append(p);

                        // 上下左右の4方向
                        int dx[] = {0, 0, -1, 1};
                        int dy[] = {-1, 1, 0, 0};
                        for (int i = 0; i < 4; ++i) {
                            int nx = p.x() + dx[i];
                            int ny = p.y() + dy[i];

                            if (nx >= 0 && nx < col_ && ny >= 0 && ny < row_) {
                                if (!visited[ny][nx] && board_[ny][nx].first == color) {
                                    visited[ny][nx] = true;
                                    queue.append(QPoint(nx, ny));
                                }
                            }
                        }
                    }

                    int sum = 0;
                    for (const QPoint& p : connected) {
                        sum += board_[p.y()][p.x()].second;
                    }

                    // 4つ以上繋がっていたら消去
                    if (this->conditionErase(sum) && connected.size() >= 4) {
                        for (const QPoint& p : connected) {
                            board_[p.y()][p.x()].first = blockColor::None;
                            board_[p.y()][p.x()].second = 0;
                            anyErased = true;
                        }
                        score_ += sum;
                        // 消えた後の自由落下処理（簡易版）
                        current_execution_state_ = executionState::applyingGravity; 
                    }
                }
            }
            if (!anyErased) break;
        }
        this->update();
        if(current_execution_state_ != executionState::applyingGravity) {
            spawnPuyo();
        }
    }

    void applyGravity() {
        for (int c = 0; c < col_; ++c) {
            for (int r = row_ - 1; r >= 0; --r) {
                if (board_[r][c].first == blockColor::None) {
                    for (int kr = r - 1; kr >= 0; --kr) {
                        if (board_[kr][c].first != blockColor::None) {
                            board_[r][c] = board_[kr][c];
                            board_[kr][c] = std::make_pair(blockColor::None, 0);
                            break;
                        }
                    }
                }
            }
        }
        current_execution_state_ = executionState::erasingBlocks;
    }

    bool canMove(int newX, int newY) {
        return (newX >= 0 && 
                newX < col_ && 
                newY >= 0 && 
                newY < row_ && 
                board_[newY][newX].first == blockColor::None);
    }

    bool canRotate(const rotationEnum newRotation) {
        switch (newRotation)
        {
            case rotationEnum::BottomUp:
                return canMove(currentX_, currentY_ + 1);
            case rotationEnum::UpBottom:
                return canMove(currentX_, currentY_ - 1);
            case rotationEnum::LeftRight:
                return canMove(currentX_ + 1, currentY_);
            case rotationEnum::RightLeft:
                return canMove(currentX_ - 1, currentY_);
            default:
                break;
        }
        return false;                
    }

    public:
    void decrementCurrentX() {
        if(current_execution_state_ == executionState::droppingBlock) {
            if (canMove(currentX_ - 1, currentY_)) {
                currentX_--;
            }
        }
    }

    void incrementCurrentX() {
        if(current_execution_state_ == executionState::droppingBlock) {
            if (canMove(currentX_ + 1, currentY_)) {
                currentX_++;
            }
        }
    }

    void incrementCurrentY() {
        if(current_execution_state_ == executionState::droppingBlock) {
            if (canMove(currentX_, currentY_ + 1)) {
                currentY_++;
            }
        }
    }

    void decrementCurrentY() {
        if(current_execution_state_ == executionState::droppingBlock) {
            if (canMove(currentX_, currentY_ - 1)) {
                currentY_--;
            }
        }
    }

    void dropCurrentBlock() {
        if(current_execution_state_ != executionState::droppingBlock) {
            return;
        }
        switch (currentRotation_)
        {
            case rotationEnum::BottomUp:
                while (canMove(currentX_, currentY_+1) &&
                    canMove(currentX_, currentY_)) {
                    currentY_++;
                }
            case rotationEnum::UpBottom:
                while (canMove(currentX_, currentY_+1) &&
                    canMove(currentX_, currentY_ + 2)) {
                    currentY_++;
                }
                break;
            case rotationEnum::LeftRight:
                while (canMove(currentX_, currentY_+1) &&
                    canMove(currentX_ + 1, currentY_+1)) {
                    currentY_++;
                }
                break;
            case rotationEnum::RightLeft:
                while (canMove(currentX_, currentY_+1) &&
                    canMove(currentX_ - 1, currentY_+1)) {
                    currentY_++;
                }
                break;
            default:
                break;
        }
    }

    void rotateCurrentBlock() {
        if(current_execution_state_ != executionState::droppingBlock) {
            return;
        }
        switch (currentRotation_)
        {
            case rotationEnum::BottomUp:
                if(canRotate(rotationEnum::LeftRight)) {
                    currentRotation_ = rotationEnum::LeftRight;
                }
                break;
            case rotationEnum::LeftRight:
                if(canRotate(rotationEnum::UpBottom)) {
                    currentRotation_ = rotationEnum::UpBottom;
                }
                break;
            case rotationEnum::UpBottom:
                if(canRotate(rotationEnum::RightLeft)) {
                    currentRotation_ = rotationEnum::RightLeft;
                }
                break;
            case rotationEnum::RightLeft:
                if(canRotate(rotationEnum::BottomUp)) {
                    currentRotation_ = rotationEnum::BottomUp;
                }
                break;
            default:
                break;
        }
    }

    bool isGameOver() const {
        return game_over_;
    }
    bool isStart() const {
        return start_;
    }
    bool isPause() const {
        return current_execution_state_ == executionState::pauseGame;
    }

    int getScore() const {
        return score_;
    }

   private:
    void drawBlock(QPainter &painter, int x, int y, std::pair<blockColor,int>  blockInfo) {
        if (blockInfo.first == blockColor::None) return;

        blockColor colorType = blockInfo.first;
        QColor color;
        switch (colorType) {
            case blockColor::Red: color = Qt::red; break;
            case blockColor::Blue: color = Qt::blue; break;
            case blockColor::Green: color = Qt::green; break;
            default: color = Qt::white; break;
        }

        int blockNum = blockInfo.second;

        QString blockNumText = QString::number(blockNum);
        painter.setPen(color);
        QRect rect(x * blockSize_ + small_eps, 
                   y * blockSize_ + small_eps, 
                   blockSize_ - 2.*small_eps, 
                   blockSize_ - 2.*small_eps);
        painter.drawText(rect, Qt::AlignCenter, blockNumText);
        painter.drawRect(x * blockSize_ + small_eps, 
                         y * blockSize_ + small_eps, 
                         blockSize_ - 2.*small_eps, 
                         blockSize_ - 2.*small_eps);
    }

    inline bool conditionErase(const int sum) {
        switch (rule_)
        {
        case ruleEnum::rule1:
            if(sum >= 4) {
                return true;
            } else {
                return false;
            }
            break;
        case ruleEnum::rule2:
            if(sum >= 10) {
                return true;
            } else {
                return false;
            }
            break;
        case ruleEnum::rule3:
            if(sum >= 20) {
                return true;
            } else {
                return false;
            }
            break;
        case ruleEnum::rule4:
            if(sum % 4 == 0) {
                return true;
            } else {
                return false;
            }
            break;
        case ruleEnum::rule5:
            if(sum % 7 == 0) {
                return true;
            } else {
                return false;
            }
            break;
        default:
            break;
        }
        return false;
    }

    private:
    int row_;
    int col_;
    int blockSize_;
    int blockMaxNum_;
    std::vector<std::vector<std::pair<blockColor, int>>> board_;
    int currentX_;
    int currentY_;
    std::array<std::pair<blockColor, int>, 2> currentBlockInfo_;
    rotationEnum currentRotation_;
    bool game_over_;
    bool start_;
    ruleEnum rule_;
    sizeEnum size_;
    int width_;
    int height_;
    executionState current_execution_state_;
    const double small_eps = 1e-1;
    int score_;

};

#endif // BOXWINDOW_HPP