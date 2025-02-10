#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "graph.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadAndDisplayGraph(); // Function to load and display the graph
    void handleNodeSelection();
    void highlightEdge(int nodeId1, int nodeId2);
    void resetEdgeColors();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *m_scene; // Scene for rendering the graph
    Graph m_graph;           // Graph object
    QList<int> selectedNodeIds;

    int minLat, maxLat, minLon, maxLon;
    double scale, xOffset, yOffset;
    std::vector<Edge> shortestPathEdges;

};
#endif // MAINWINDOW_H
