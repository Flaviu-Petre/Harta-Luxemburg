#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_scene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    ui->graphicsView->setScene(m_scene);

    loadAndDisplayGraph();
    connect(m_scene, &QGraphicsScene::selectionChanged, this, &MainWindow::handleNodeSelection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadAndDisplayGraph()
{
    QString filename = "../../Harta_Luxemburg.xml";
    m_graph.loadFromXml(filename);

    if (m_graph.getNodes().empty()) {
        qWarning() << "No nodes loaded from the XML file.";
        return;
    }

    minLat = std::numeric_limits<int>::max();
    maxLat = std::numeric_limits<int>::min();
    minLon = std::numeric_limits<int>::max();
    maxLon = std::numeric_limits<int>::min();

    for (Node* node : m_graph.getNodes()) {
        QPoint pos = node->getPos();
        int lat = pos.x();
        int lon = pos.y();
        minLat = std::min(minLat, lat);
        maxLat = std::max(maxLat, lat);
        minLon = std::min(minLon, lon);
        maxLon = std::max(maxLon, lon);
    }

    double width = maxLon - minLon;
    double height = maxLat - minLat;

    double sceneWidth = 800.0;
    double sceneHeight = 600.0;

    double scaleX = sceneWidth / width;
    double scaleY = sceneHeight / height;

    scale = std::min(scaleX, scaleY);

    xOffset = -minLon * scale;
    yOffset = -minLat * scale;

    QMap<int, QGraphicsEllipseItem*> nodeItems;

    QPen edgePen(Qt::gray);
    for (Edge& edge : m_graph.getEdges()) {
        Node* first = edge.getFirst();
        Node* second = edge.getSecond();

        if (first != nullptr && second != nullptr) {
            QPoint pos1 = first->getPos();
            QPoint pos2 = second->getPos();

            double x1 = (pos1.y() - minLon) * scale + xOffset;
            double y1 = (pos1.x() - minLat) * scale + yOffset;
            double x2 = (pos2.y() - minLon) * scale + xOffset;
            double y2 = (pos2.x() - minLat) * scale + yOffset;

            m_scene->addLine(x1, y1, x2, y2, edgePen);
        }
    }

    float nodeRadius = 0.01;
    QPen nodePen(Qt::black);
    QBrush nodeBrush(Qt::blue);

    for (Node* node : m_graph.getNodes()) {
        QPoint pos = node->getPos();
        double x = (pos.y() - minLon) * scale + xOffset;
        double y = (pos.x() - minLat) * scale + yOffset;

        QGraphicsEllipseItem* nodeItem = m_scene->addEllipse(x - nodeRadius, y - nodeRadius, 2 * nodeRadius, 2 * nodeRadius, nodePen, nodeBrush);

        nodeItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
        nodeItem->setData(0, node->getValue());
        nodeItem->setZValue(1);
    }
}



void MainWindow::handleNodeSelection() {
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

    for (QGraphicsItem* item : selectedItems) {
        if (item->data(0).isValid()) {
            int nodeId = item->data(0).toInt();
            selectedNodeIds.append(nodeId);
        }
    }

    qDebug() << "Selected Nodes:" << selectedNodeIds;

    if (selectedNodeIds.size() == 2) {
        int node1Id = selectedNodeIds[0];
        int node2Id = selectedNodeIds[1];
        qDebug() << "Selected Node 1:" << node1Id << "Node 2:" << node2Id;

        highlightEdge(node1Id, node2Id);
        selectedNodeIds.clear();
    }
}

void MainWindow::highlightEdge(int nodeId1, int nodeId2)
{
    resetEdgeColors();

    QPen highlightPen(Qt::darkGreen, 2);

    shortestPathEdges = m_graph.Dijkstra(nodeId1, nodeId2);

    for (Edge& edge : shortestPathEdges) {
        QPoint pos1 = edge.getFirst()->getPos();
        QPoint pos2 = edge.getSecond()->getPos();

        double x1 = (pos1.y() - minLon) * scale + xOffset;
        double y1 = (pos1.x() - minLat) * scale + yOffset;
        double x2 = (pos2.y() - minLon) * scale + xOffset;
        double y2 = (pos2.x() - minLat) * scale + yOffset;

        m_scene->addLine(x1, y1, x2, y2, highlightPen);
    }
}

void MainWindow::resetEdgeColors()
{
    QPen defaultPen(Qt::gray);
    for (Edge& edge : shortestPathEdges) {
        QPoint pos1 = edge.getFirst()->getPos();
        QPoint pos2 = edge.getSecond()->getPos();

        double x1 = (pos1.y() - minLon) * scale + xOffset;
        double y1 = (pos1.x() - minLat) * scale + yOffset;
        double x2 = (pos2.y() - minLon) * scale + xOffset;
        double y2 = (pos2.x() - minLat) * scale + yOffset;

        m_scene->addLine(x1, y1, x2, y2, defaultPen);
    }
}


