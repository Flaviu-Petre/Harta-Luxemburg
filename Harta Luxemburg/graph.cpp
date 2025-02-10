#include "graph.h"
#include <cmath>
#include <fstream>
#include <algorithm>
#include <QFile>
#include <QXmlStreamReader>
#include <queue>

Graph::Graph()
{

}

bool Graph::NodesOverlap(QPoint &p)
{
    int minDistance = 20;
    for(auto& node: m_nodes){
        int dx = node->getPos().x() - p.x();
        int dy = node->getPos().y() - p.y();
        int Distance = std::sqrt(dx * dx + dy * dy);
        if(Distance < minDistance) {
            return true;
        }
    }
    return false;
}

void Graph::addNode(QPoint p)
{
    if(NodesOverlap(p)){
        qDebug("Nodurile se suprapun!");
        return;
    }

    Node* n = new Node();
    n->setPos(p);
    n->setValue(m_nodes.size()+1);
    m_nodes.push_back(n);
   // saveMAtoFile("../../MatAdiacenta.txt");
}

void Graph::addEdge(Node *f, Node *s, int cost)
{
    for(auto& edge: m_edges){
        if(m_isOriented){
            if ((edge.getFirst()->getValue() == f->getValue() && edge.getSecond()->getValue() == s->getValue())){
                qDebug() << "Arcul deja exista intre nodurile" << f->getValue() << "si" << s->getValue();
                return;
            }
        }
        else
        {
            if(edge.getFirst()->getValue() == s->getValue() && edge.getSecond()->getValue() == f->getValue() ||
                (edge.getFirst()->getValue() == f->getValue() && edge.getSecond()->getValue() == s->getValue())){
                qDebug() << "Arcul deja exista intre nodurile" << f->getValue() << "si" << s->getValue();
                return;
            }
        }

    }

    if(!m_isOriented){
        if(f->getValue() != s->getValue()){
            Edge e(f, s, cost);
            m_edges.push_back(e);
        }
    }
    else {
        Edge e(f, s, cost);
        m_edges.push_back(e);
    }


   //saveMAtoFile("../../MatAdiacenta.txt");
}

/*void Graph::saveMAtoFile(const char *filename)
{
    std::ofstream out(filename);
    std::vector<std::vector<int>> MatAdiacenta(m_nodes.size(), std::vector<int>(m_nodes.size(), 0));
    out << m_nodes.size() << "\n";

    for (auto edge : m_edges){
        int i = edge.getFirst()->getValue() - 1;
        int j = edge.getSecond()->getValue() - 1;
        MatAdiacenta[i][j] = 1;
        if(!m_isOriented)
            MatAdiacenta[j][i] = 1;
    }

    for (const auto& row : MatAdiacenta){
        for (int val : row) {
            out << val << " ";
        }
        out << "\n";
    }

    m_MatAdiacenta.resize(MatAdiacenta.size());
    for(int row = 0; row < m_MatAdiacenta.size(); ++row)
        m_MatAdiacenta[row].resize(MatAdiacenta.size());

    for(int row = 0; row < MatAdiacenta.size(); ++row)
        for(int column = 0; column < MatAdiacenta.size(); ++column)
            m_MatAdiacenta[row][column] = MatAdiacenta[row][column];

    out.close();
}
*/


void Graph::loadFromXml(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << filename;
        return;
    } else {
        qDebug() << "File loaded successfully:" << filename;
    }

    QXmlStreamReader xml(&file);

    QMap<int, Node*> m_nodeMap;

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        if (xml.isStartElement()){
            if (xml.name().toString() == "node"){
                int id = xml.attributes().value("id").toInt();
                int latitude = xml.attributes().value("latitude").toInt();
                int longitude = xml.attributes().value("longitude").toInt();

                QPoint position(latitude, longitude);
                Node* newNode = new Node();
                newNode->setPos(position);
                newNode->setValue(id);
                m_nodes.push_back(newNode);
                m_nodeMap[id] = newNode;
                m_adjList.resize(m_nodes.size());
            }
            else if(xml.name().toString() == "arc"){
                int from = xml.attributes().value("from").toInt();
                int to = xml.attributes().value("to").toInt();
                int cost = xml.attributes().value("length").toInt();

                Node* firstNode = m_nodeMap[from];
                Node* secondNode = m_nodeMap[to];

                Edge currentEdge(firstNode, secondNode, cost);

                m_edges.emplace_back(currentEdge);
                m_adjList[firstNode->getValue()].emplace_back(currentEdge);
            }
        }
    }

    if (xml.hasError()){
         qWarning() << "XML error:" << xml.errorString();
    }

    qDebug() << "Nodes loaded:" << m_nodes.size();
    qDebug() << "Edges loaded:" << m_edges.size();

    file.close();
}

std::vector<Edge> Graph::Dijkstra(int sourceNodeId, int targetNodeId)
{
    std::unordered_map<int, int> distances;
    std::unordered_map<int, int> predecessors;
    std::unordered_map<int, bool> visited;

    auto compare = [&](int n1, int n2) { return distances[n1] > distances[n2]; };
    std::priority_queue<int, std::vector<int>, decltype(compare)> PriorityQueue(compare);

    for (Node* node : m_nodes) {
        distances[node->getValue()] = std::numeric_limits<int>::max();
        visited[node->getValue()] = false;
        predecessors[node->getValue()] = -1;
    }

    distances[sourceNodeId] = 0;
    PriorityQueue.push(sourceNodeId);

    while(!PriorityQueue.empty()){
        int currentNodeId = PriorityQueue.top();
        PriorityQueue.pop();

        if (visited[currentNodeId]) continue;
        visited[currentNodeId] = true;

        if (currentNodeId == targetNodeId) break;

        for(Edge& edge : m_adjList[currentNodeId]){
            int neighborNodeId = -1;

            if (edge.getFirst()->getValue() == currentNodeId)
                neighborNodeId = edge.getSecond()->getValue();
            else if (edge.getSecond()->getValue() == currentNodeId)
                neighborNodeId = edge.getFirst()->getValue();

            if (neighborNodeId == -1 || visited[neighborNodeId]) continue;

            int lenght = edge.getCost();
            if (distances[currentNodeId] + lenght < distances[neighborNodeId]) {
                distances[neighborNodeId] = distances[currentNodeId] + lenght;
                predecessors[neighborNodeId] = currentNodeId;
                PriorityQueue.push(neighborNodeId);
            }
        }
    }

    std::vector<Edge> shortestPathEdges;
    int currentNodeId = targetNodeId;

    while (predecessors[currentNodeId] != -1) {
        int prevNodeId = predecessors[currentNodeId];

        for (Edge& edge : m_adjList[currentNodeId]) {
            if ((edge.getFirst()->getValue() == prevNodeId && edge.getSecond()->getValue() == currentNodeId) ||
                (edge.getFirst()->getValue() == currentNodeId && edge.getSecond()->getValue() == prevNodeId)) {
                shortestPathEdges.push_back(edge);
                break;
            }
        }
        currentNodeId = prevNodeId;
    }
    std::reverse(shortestPathEdges.begin(), shortestPathEdges.end());
    return shortestPathEdges;
}




