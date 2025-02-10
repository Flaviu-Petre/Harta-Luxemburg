#ifndef EDGE_H
#define EDGE_H

#include "node.h"

class Edge
{
public:
    Edge() : m_firstNode(nullptr), m_secondNode(nullptr), m_cost(0) {}
    Edge(Node *f, Node *s, int cost): m_firstNode(f), m_secondNode(s), m_cost(cost){}
    Node* getFirst(){return m_firstNode;}
    Node* getSecond(){return m_secondNode;}
    int getCost(){return m_cost;}
private:
    Node *m_firstNode, *m_secondNode;
    int m_cost;
};

#endif // EDGE_H
