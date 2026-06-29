#ifndef ENUM_HPP
#define ENUM_HPP

enum class sizeEnum {
    s12x6, 
    s14x7, 
    s16x8, 
    s19x9
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

enum class ruleEnum {
    rule1,
    rule2,
    rule3,
    rule4,
    rule5
};
#endif // ENUM_HPP