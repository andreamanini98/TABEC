#ifndef UTOTPARSER_DOUBLELINKEDLIST_H
#define UTOTPARSER_DOUBLELINKEDLIST_H


template<class T>
class DoublyLinkedList {

protected:
    //Struct representing a node of the doubly linked list.
    struct Node {
        [[maybe_unused]] T content;
        std::shared_ptr<Node> next;
        std::shared_ptr<Node> prev;


        explicit Node(T content) : content(content), next(nullptr), prev(nullptr)
        {};
    };


private:
    // Represents the first position of the list.
    std::shared_ptr<Node> head;

    // Represents the last position of the list.
    std::shared_ptr<Node> tail;


public:
    DoublyLinkedList() : head(nullptr), tail(nullptr)
    {};


    /**
     * Method used to insert an element of type T in the first position of the list.
     * @param elem the element to insert.
     */
    void insertFirst(T elem)
    {
        std::shared_ptr<Node> newNode = std::make_shared<Node>(elem);

        // If either head or tail are nullptr, we have to directly connect them to the newly allocated node.
        if (!(head || tail))
        {
            head = newNode;
            tail = newNode;
        } else
        {
            newNode->prev = head;
            head->next = newNode;
            head = newNode;
        }
    }


    /**
     * Method used to print the list starting from the tail and ending in the head.
     */
    void printList()
    {
        std::shared_ptr<Node> cursor = tail;

        // Integer keeping track of the node number which is being currently visited.
        int nodeCtr = 0;

        // While cursor is not a nullptr, we continue to traverse the list.
        while (cursor)
        {
            std::cout << "Content of node " << nodeCtr << ": " << std::to_string(cursor->content) << '\n';
            cursor = cursor->next;
            nodeCtr++;
        }
    }


    std::shared_ptr<Node> getHead()
    {
        return head;
    }


    std::shared_ptr<Node> getTail()
    {
        return tail;
    }


};


#endif //UTOTPARSER_DOUBLELINKEDLIST_H
