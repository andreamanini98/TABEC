#ifndef UTOTPARSER_RANDOMCREATORBARABASIALBERT_H
#define UTOTPARSER_RANDOMCREATORBARABASIALBERT_H

#include <random>
#include <cassert>

#include "defines/UPPAALxmlAttributes.h"
#include "TAHeaders/TAContentExtractor.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreator.hpp"


class RandomCreatorBarabasiAlbert : public RandomCreator {

private:
    // The total number of nodes the resulting random network will have.
    int numNodes { 20 };

    // The initial number of nodes.
    int m0 { 5 };

    // The number of links added when inserting a new node.
    int m { 3 };

    // A vector of vectors representing the adjacency list of the network.
    // Each position in the outer vector represents a node by its integer index.
    std::vector<std::vector<int>> adjacencyList {};

    // A vector containing the degrees of all nodes.
    // Each entry in this vector takes into account both in and out degree of a single node.
    // Each position in the vector represents a node by its integer index.
    std::vector<int> degrees {};

    // The global degree defined as the sum of all the single degrees.
    int globalDegree {};

    // The maximum degree between all the degrees.
    int maxDegree {};


    /**
     * Method used to get a valid string representation for identifiers to use in locations.
     * @param idNumber the number to associate to the identifier.
     * @return a string representation of the identifier.
     */
    static std::string getValidId(int idNumber)
    {
        return "Id" + std::to_string(idNumber);
    }


    /**
     * Method used to print the generated BA network.
     */
    [[maybe_unused]] void printNetwork()
    {
        std::cout << "Now printing BA network.\n";

        int nodeIdx = 0;
        for (auto &node: adjacencyList)
        {
            std::cout << "Node " << nodeIdx << ": {";

            std::string neighborsList {};
            for (auto &neighbor: node)
                neighborsList.append(" " + std::to_string(neighbor) + ",");

            // Popping the trailing ',' character.
            neighborsList.pop_back();

            std::cout << neighborsList << " }\n";
            ++nodeIdx;
        }
    }


    /**
     * Method used to print the degrees of the nodes inside the generated BA network.
     */
    [[maybe_unused]] void printDegrees()
    {
        std::cout << "Now printing degrees.\n";

        std::cout << "Global degree: " << globalDegree << '\n';

        int nodeIdx = 0;
        for (auto &degree: degrees)
        {
            std::cout << "Node " << nodeIdx << ": " << degree << '\n';
            ++nodeIdx;
        }
    }


    /**
     * Method used to create the initial network as stated in the original BA's algorithm.
     * The current implementation creates a clique as initial network.
     */
    void createInitialNetwork()
    {
        // Index 'i' also represents the current node (since nodes are referenced by their integer index,
        // both in the adjacency list vector and in the degrees vector).
        for (int i = 0; i < m0; i++)
        {
            // Vector temporarily storing the nodes connected to the current one.
            std::vector<int> tmpVec {};

            // For each node different from the current one, a link is created.
            for (int j = 0; j < m0; j++)
                if (i != j)
                    tmpVec.push_back(j);

            // Updating the current node's adjacency list.
            adjacencyList.push_back(tmpVec);

            // Updating current node's degree, global degree and max degree.
            int nodeDegree = static_cast<int>(tmpVec.size());
            degrees.push_back(nodeDegree);
            globalDegree += nodeDegree;
            if (maxDegree < nodeDegree)
                maxDegree = nodeDegree;
        }
    }


    /**
     * Method used to insert a new node and connect it to the existing nodes by using the preferential attachment technique.
     */
    void insertNode()
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        // The new node to connect with other existing nodes.
        // Since nodes are referenced by their integer index, and since the indexes start from 0,
        // by taking the size of the adjacency list as the new node index will suffice.
        int newNode = static_cast<int>(adjacencyList.size());

        // Distribution used to extract the probability with which a node will be connected to other nodes.
        std::uniform_real_distribution<double> double_dist(0.0, 1.0);

        // Distribution used to extract a node which will be connected to other nodes.
        std::uniform_int_distribution<int> int_dist(0, newNode - 1);

        // The number of chosen nodes in the current iteration of the BA's algorithm.
        int totalChosenNodes {};

        // A vector containing the index of already chosen nodes, so that a node cannot be chosen more than once.
        std::vector<int> alreadyChosenNodes {};

        // The loop will cycle until there have been inserted exactly 'm' new links.
        while (totalChosenNodes < m)
        {
            // A double random number (a probability) that will be used to determine if a new link has to be created or not.
            double pLink = double_dist(gen);

            // A randomly-chosen integer index representing a candidate node to link with the new node.
            int randomChosenNode = int_dist(gen);

            // A double random number (a probability) representing the probability of creating a new link by preferential attachment.
            double pNode = static_cast<double>(degrees[randomChosenNode]) / static_cast<double>(globalDegree);

            // If the probabilities allow the connection and the chosen node has not been already chose, a new connection will be created.
            if (pLink <= pNode && std::find(alreadyChosenNodes.begin(), alreadyChosenNodes.end(), randomChosenNode) == alreadyChosenNodes.end())
            {
                // Uncomment the following line if you also want a connection from the random chosen node to the new node.
                // adjacencyList[randomChosenNode].push_back(newNode);

                // Still have to update since in the preferential attachment we assume to use the total degree (in + out).
                degrees[randomChosenNode] += 1;

                alreadyChosenNodes.push_back(randomChosenNode);
                ++totalChosenNodes;
                ++globalDegree;

                if (maxDegree < degrees[randomChosenNode])
                    maxDegree = degrees[randomChosenNode];
            }
        }

        // At the end, we have to push a new list of nodes for the newly created node.
        adjacencyList.push_back(alreadyChosenNodes);

        // Also, we have to push back the degree of the new node.
        degrees.push_back(m);
    }


    /**
     * Method used to compute the degree distribution of the resulting BA network.
     * Use it to test if a power-law distribution arise (few nodes should have high degree, while many nodes should have low degree).
     */
    void computeDegreeDistribution()
    {
        // The outer pair contains:
        // - 1st: an integer representing a particular degree found in the network.
        // - 2nd: a pair containing:
        //        - 1st: the total number of nodes having such a degree.
        //        - 2nd: the percentage of nodes having such degree with respect to the entire network.
        // This is a vector containing the computed network's degree distribution.
        std::vector<std::pair<int, std::pair<int, double>>> degreeDistribution {};

        // For each integer from 1 to the maximum degree, we look in the network to find some node having such degree.
        for (int i = 1; i <= maxDegree; i++)
        {
            // The total number of nodes having a degree equal to the current value of 'i'.
            int degreeTotNodes {};

            for (auto &degree: degrees)
                if (i == degree)
                    ++degreeTotNodes;

            if (degreeTotNodes > 0)
                degreeDistribution.emplace_back(i, std::make_pair(degreeTotNodes, static_cast<double>(degreeTotNodes) / static_cast<double>(numNodes)));
        }

        // Printing the degree distribution.
        std::cout << "Now printing degrees distribution.\n";

        for (auto &d: degreeDistribution)
            std::cout << "Degree " << d.first << "  :  Total nodes = " << d.second.first << "  :  Percentage: " << d.second.second << '\n';
    }


    /**
     * Method used to create a BA network and print some statistics.
     */
    void createBANetwork()
    {
        // TODO: change assertions in exceptions.

        std::cout << "Creating random Barabasi-Albert tile.\n";

        // The number of newly added links must be at most equal to the number of nodes in the initial network
        assert (m <= m0);

        createInitialNetwork();

        // The number of nodes in the initial list must be less than the total number of nodes.
        assert(adjacencyList.size() < numNodes);

        int totalNodes = static_cast<int>(adjacencyList.size());

        while (totalNodes < numNodes)
        {
            insertNode();
            ++totalNodes;
        }

        // At the end, the adjacency list must contain all nodes requested.
        assert (adjacencyList.size() == numNodes);

        std::cout << '\n';
        computeDegreeDistribution();
    }


    //TODO: document method
    void insertLocationsInRandomTileFromNetwork(json &randomTile)
    {
        // Setting the initial location.
        randomTile.at(NTA).at(TEMPLATE).at(INIT).at(REF) = getBlankInitialLocationName();

        // Inserting the initial location.
        json *locationsPtr = getJsonPtrAsArray(TAContentExtractor::getLocationsPtr(randomTile));
        // Since no locations are still present, clearing makes the 'null' value disappear.
        locationsPtr->clear();

        locationsPtr->push_back(getBlankInitialLocation());
        locationsPtr->push_back(getBlankOutLocation());

        for (int i = 0; i < adjacencyList.size(); i++)
            locationsPtr->push_back({{ ID, getValidId(i) }});
    }


    //TODO: document method
    void connectInAndOut(json *transitionPtr)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        // Distribution used to extract a random node.
        std::uniform_int_distribution<int> int_dist(0, static_cast<int>(adjacencyList.size()) - 1);

        // Extracting a random node so that a transition from the initial node to such random node can be created.
        int randomNode = int_dist(gen);
        transitionPtr->push_back(getBlankTransition(getBlankInitialLocationName(), getValidId(randomNode)));

        // Extracting a random node so that a transition from such random node to the out node can be created.
        randomNode = int_dist(gen);
        transitionPtr->push_back(getBlankTransition(getValidId(randomNode), getBlankOutLocationName()));
    }


    //TODO: document method
    void insertTransitionsInRandomTileFromNetwork(json &randomTile)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        json *transitionsPtr = getJsonPtrAsArray(TAContentExtractor::getTransitionsPtr(randomTile));
        // Since no transitions are still present, clearing makes the 'null' value disappear.
        transitionsPtr->clear();

        for (int i = 0; i < adjacencyList.size(); i++)
        {
            for (auto &neighbor: adjacencyList[i])
            {
                std::string srcLoc = getValidId(i);
                std::string dstLoc = getValidId(neighbor);
                transitionsPtr->push_back(getLabeledTransition(srcLoc, dstLoc, gen));
            }
        }
        connectInAndOut(transitionsPtr);
    }


public:
    //TODO: document method
    json createRandomTile() override
    {
        // TODO: create a std::randomDevice here and pass it around where necessary

        createBANetwork();

        json randomTile = getBlankTA();

        insertLocationsInRandomTileFromNetwork(randomTile);

        insertTransitionsInRandomTileFromNetwork(randomTile);

        std::cout << "\n\nResultingTiledTA:\n\n";
        std::cout << std::setw(4) << randomTile << std::endl;

        return randomTile;
    }

};


#endif //UTOTPARSER_RANDOMCREATORBARABASIALBERT_H

// t4 ++ (t4 ++ (t:BA + tile_accepting) tile_accepting) (t4 ++ (t1 + tile_accepting) (t:BA + t:BA + t:BA + tile_accepting))
