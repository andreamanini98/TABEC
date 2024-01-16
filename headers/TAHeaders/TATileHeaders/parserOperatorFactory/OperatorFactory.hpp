#ifndef UTOTPARSER_OPERATORFACTORY_H
#define UTOTPARSER_OPERATORFACTORY_H

#include "TAHeaders/TATileHeaders/parserOperatorFactory/Operator.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/OperatorMatchInOutSize.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/OperatorOnlyOneOut.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/OperatorTreeOp.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/ParserOperatorFactory.hpp"
#include "TAHeaders/TATileHeaders/lexerAndParser/ParserNode.hpp"
#include "DoublyLinkedList.hpp"
#include "TAHeaders/TATileHeaders/TileTokensEnum.h"


class OperatorFactory : public ParserOperatorFactory {

public:
    /**
     * Method used to select which operator must be executed based on the given 'token' parameter.
     * @param parserList the data structure containing the tiles and operators needed by the parser.
     * @param token the token which will be used to determine which operator to execute.
     * @return an Operator that will affect the 'parserList' parameter based on the 'token' parameter.
     */
    Operator *createOperator(DoublyLinkedList<ParserNode> &parserList, const std::string &token) override
    {
        TileTokensEnum tk = fromStrTileTokenEnum(token);

        switch (tk)
        {
            case match_inout_size:
                return new OperatorMatchInOutSize(parserList);

            case only_one_out:
                return new OperatorOnlyOneOut(parserList);

            case tree_op:
                return new OperatorTreeOp(parserList);

            default:
                std::cerr << BHRED << "Not available operator." << rstColor << std::endl;
                std::cerr << BHRED << "Action will not be performed. This may cause unexpected behaviour" << rstColor << std::endl;
                break;
        }
    }

};


#endif //UTOTPARSER_OPERATORFACTORY_H
