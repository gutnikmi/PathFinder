#pragma once

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
#include <QThread>
#include <QTimer>
#include <QStyle>

#include "finder.h"

const int SOLVE_DELAY = 50; // задержка перед поиском пути

const int SQUARE_SIZE = 50; // размер квадрата

const int PEN_SIZE = 5; // ширина пути

/*!
 * \brief The MapWidget class - виджет в котором рисуется поле, точки начала и конца, препятствия и путь
 */
class MapWidget : public QGraphicsView
{
    Q_OBJECT
public:
    MapWidget(QWidget *parent = nullptr);
    ~MapWidget();
    /*!
     * \brief solve - поиск пути
     */
    void solve();
    /*!
     * \brief reset - создает новое поле и удаляет старое
     */
    void reset();
    /*!
     * \brief clearObstacles - очищает препятствие с поля
     */
    void clearObstacles();
    /*!
     * \brief setMapSize - устанавливает размер поля
     * \param width - ширина поля
     * \param height - высота поля
     */
    void setMapSize(int width, int height);
    /*!
     * \brief setStartPoint - устанавливает точку начала
     * \param start
     */
    void setStartPoint(Point start);
    /*!
     * \brief setEndPoint - устанавливает точку конца
     * \param end
     */
    void setEndPoint(Point end);
    /*!
     * \brief setObstacle - устанавливает препятствие
     * \param x - координата препятствия х
     * \param y - координата препятствия у
     */
    void setObstacle(int x, int y);
    /*!
     * \brief setAddingBool - устанавливает режим установки препятствий
     * \param addingObstacles
     */
    void setAddingBool(bool addingObstacles);
    /*!
     * \brief setSearchingBool - устанавливает режим поиска пути по наведению мыши
     * \param searchingWithMouse
     */
    void setSearchingBool(bool searchingWithMouse);

signals:
    /*!
     * \brief solveRequested - отправлет данные для поиска пути в отдельный поток
     * \param start- точка начала
     * \param end - точка конца
     * \param height - высота поля
     * \param width - ширина поля
     * \param obstacles - препятсвтия
     */
    void solveRequested(Point start, Point end, int height, int width, QVector<Point> obstacles);

protected:
    /*!
     * \brief mousePressEvent - для установки точек начала и конца
     * \param event
     */
    void mousePressEvent(QMouseEvent *event) override;
    /*!
     * \brief mouseMoveEvent - для поиска пути по наведению
     * \param event
     */
    void mouseMoveEvent(QMouseEvent *event) override;
    /*!
     * \brief drawBackground - рисует фон и препятствия
     * \param painter
     * \param rect
     */
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    /*!
     * \brief wheelEvent - для масштабирования
     * \param event
     */
    void wheelEvent(QWheelEvent *event) override;

private:
    /*!
     * \brief convertPointsToQPoints - преобразует точки пути в точки на карте
     * \param points - точки пути
     * \return точки на карте
     */
    QVector<QPointF> convertPointsToQPoints(const QVector<Point> &points);
    /*!
     * \brief addArrowToPath - добавляет стрелку к пути
     * \param path - путь
     * \param startPoint - начало пути
     * \param endPoint - конец пути
     */
    void addArrowToPath(QPainterPath &path, const QPointF &startPoint, const QPointF &endPoint);
    /*!
     * \brief drawPath - рисует путь
     * \param pathPoints - точки пути
     */
    void drawPath(const QVector<Point> &pathPoints);
    /*!
     * \brief clearPath - очищает путь
     */
    void clearPath();
    /*!
     * \brief isValidPoint проверяет находится ли точка в рамках поля
     * \param scenePoint
     * \return находится ли точка в рамках поля
     */
    bool isValidPoint(QPointF scenePoint);
    /*!
     * \brief toPoint - конвертирует QPointF в Point
     * \param scenePoint - QPointF
     * \return Point
     */
    Point toPoint(QPointF scenePoint);
private:
    QGraphicsScene *m_scene = nullptr; /// поле
    QGraphicsPathItem *m_lastPath = nullptr; /// последний нарисованный путь
    QThread *m_thread = nullptr; /// поток для класса поиска пути
    QTimer *m_mouseTimer = nullptr; /// таймер по окончании которого ищется путь

    int m_mapWidth = 0; /// ширина карты
    int m_mapHeight = 0; /// высота карты

    Point m_startPoint, m_endPoint; /// точки начала и конца
    Point m_lastPoint; /// последняя точка на которой была мышь

    QVector<Point> m_obstacles; /// прептяствия

    bool m_addingObstacles = false; /// режим установки препятствий
    bool m_searchingWithMouse = false; /// режим поиска мышью

    double m_currentScale = 1.0; /// текущий уровень масштабирования
    const double m_scaleFactor = 1.15; /// на сколько изменяется масштаб при масштабировании
    const double m_maxScale = 5.0; ///макисмальный уровень масштабирования
    const double m_minScale = 0.1; ///минимальный уровень масштабирования
};
