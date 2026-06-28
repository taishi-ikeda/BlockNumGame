#ifndef ENUM_HPP
#define ENUM_HPP

enum class sizeEnum {
    s12x6, 
    s14x7, 
    s16x8, 
    s19x9
    };
enum class blockNumEnum {
    sum4b1, 
    sum10b2, 
    sum20b5
};

enum class blockColor {
    Red,
    Blue,
    Green,
    While,
    None
};

enum class rotationEnum {
    BottomUp, 
    UpBottom, 
    LeftRight, 
    RightLeft
};

enum class executionState {
    droppingBlock, 
    erasingBlocks, 
    applyingGravity, 
    spawningBlock,
    pauseGame
};

enum class speedEnum {
    Slow,
    Normal,
    Fast
};
#endif // ENUM_HPP