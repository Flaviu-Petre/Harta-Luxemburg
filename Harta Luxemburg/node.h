#ifndef NODE_H
#define NODE_H

#include <QPoint>
class Node
{
public:
    Node();
    int getValue();
    QPoint getPos() {return m_pos;};
    void setValue(int val) {m_value = val;};
    void setPos(QPoint p){m_pos = p;};

private:
    int m_value;
    QPoint m_pos;

};

#endif // NODE_H
