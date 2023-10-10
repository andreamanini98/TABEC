#ifndef UTOTPARSER_STRUCTS_H
#define UTOTPARSER_STRUCTS_H

/**
 * Struct used to keep track of the name of a TA, the result of its translation and the result of its emptiness check.
 */
typedef struct de {
    std::string nameTA;
    bool translationResult{};
    bool emptinessResult{};
} DashBoardEntry;

#endif //UTOTPARSER_STRUCTS_H
