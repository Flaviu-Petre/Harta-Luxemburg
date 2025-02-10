#ifndef GRAPH_H
#define GRAPH_H
#include "edge.h"
#include <vector>
#include <QDebug>
#include <string>
#include <fstream>
#include <QFile>

class Graph
{
public:
    Graph();

    //Methodes
    std::vector<Node*>& getNodes() {return m_nodes;};
    std::vector<Edge>& getEdges(){return m_edges;};
    bool isOriented() const { return m_isOriented; }
    void setOriented(bool oriented) { m_isOriented = oriented; }

    bool NodesOverlap(QPoint& p);

    void addNode(QPoint p);

    void addEdge(Node* f, Node* s, int cost);

    void saveMAtoFile(const char* filename);

    void loadFromXml(const QString& filename);

    std::vector<Edge> Dijkstra(int sourceNodeId, int targetNodeId);

    //Variables
    std::vector<std::vector<Edge>> m_adjList;
private:
    //Variables
    std::vector<Node*> m_nodes;
    std::vector<Edge> m_edges;
    bool m_isOriented = false;

};

#endif // GRAPH_H
