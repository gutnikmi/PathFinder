#pragma once

#include <QObject>
#include <cmath>

#include <QGraphicsView>
#include <QDebug>

#include <QQueue>
#include <QVector>
#include <QSet>
#include <QHash>
#include <QMouseEvent>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QMessageBox>

/*!
 * \brief The Point class - один квдарат на поле
 */
struct Point
{
    int x = 0;
    int y = 0;

    bool operator==(const Point& other) const
    {
        if ((this->x == other.x) && (this->y == other.y))
            return true;
        else
            return false;
    }

    bool operator!=(const Point& other) const
    {
        if ((this->x != other.x) || (this->y != other.y))
            return true;
        else
            return false;
    }
};
    Q_DECLARE_METATYPE(Point);/// для вынесения в отдельный поток

/*!
 * \brief The Finder class - класс для поиска пути, вынесен в отдельный поток
 */
class Finder : public QObject
{
    Q_OBJECT
public:
    explicit Finder(QObject *parent = nullptr);
    /*!
     * \brief findShortestPath - выполняет поиск кратчайщего пути с помощью поиска в ширину
     * \param startPoint - точка начала
     * \param endPoint - точка конца
     * \param mapHeight - высота поля
     * \param mapWidth - ширина поля
     * \param obstacles - препятсвтия
     */
    void findShortestPath(Point startPoint, Point endPoint, int mapHeight, int mapWidth, QVector<Point> obstacles);

signals:
    /*!
     * \brief pathFound - сигнал в котором найденный путь передаётся в основной поток
     * \param path - путь
     */
    void pathFound(QVector<Point> path);
private:
    /*!
     * \brief isValidPoint - определяет можно ли пройти в точку
     * \param p - точка
     * \param obstacles - препятствия
     * \param mapHeight - высота карты
     * \param mapWidth - ширина карты
     * \return можно ли пройти в точку
     */
    bool isValidPoint(const Point& p, QVector<Point> obstacles, int mapHeight, int mapWidth);

};
