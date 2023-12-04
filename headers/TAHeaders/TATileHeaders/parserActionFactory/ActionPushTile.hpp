#ifndef UTOTPARSER_ACTIONPUSHTILE_H
#define UTOTPARSER_ACTIONPUSHTILE_H

#include "nlohmann/json.hpp"

#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"
#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/Action.hpp"

using json = nlohmann::json;


class ActionPushTile : public Action {

public:
    ActionPushTile(StringsGetter &stringsGetter, DoublyLinkedList<ParserNode> &parserList, const std::string &token) :
            Action(stringsGetter, parserList, token)
    {};


    /**
     * Method used to perform an action corresponding to a tile name.
     * The action consists in pushing the tile's name on top to the tileStack, contained in the parserList.
     */
    void performAction() override
    {
        json tile = getJsonFromFileName(stringsGetter.getInputTilesDirPath(), token);

        // Each tile has to be renamed in order to avoid name clashes.
        TATileRenamer::renameIDs(tile);
        parserList.getHead()->content.tileStack.push(tile);
    }

};


#endif //UTOTPARSER_ACTIONPUSHTILE_H
