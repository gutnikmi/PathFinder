#include "mapwidget.h"

MapWidget::MapWidget(QWidget *parent):QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this); /// сцена для отрисовки карты
    m_thread = new QThread(this); /// тред для класса поиска пути
    Finder *finder = new Finder(); /// класса поиска пути

    m_mouseTimer = new QTimer(this); /// таймер после которого начинается поиск
    m_mouseTimer->setInterval(SOLVE_DELAY); /// интервал таймера
    m_mouseTimer->setSingleShot(true); /// таймер срабатывает только один раз
    connect(m_mouseTimer, &QTimer::timeout, this,&MapWidget::solve); /// поиск пути по окончанию таймера

    finder->moveToThread(m_thread); /// перемещение класса поиска пути в отдельный поток
    connect(m_thread, &QThread::finished, finder, &QObject::deleteLater);/// удаление класса поиска пути при завершении треда
    m_thread->start(); /// запуск треда

    /// регистрация мета типа для передачи вектора точек в отдельный поток
    qRegisterMetaType<QVector<Point>>();

    /// соединение метода поиска пути с сигналом с данными
    connect(this, &MapWidget::solveRequested, finder, &Finder::findShortestPath);
    /// соединение сигнала с найденым путем с методом отрисовки пути
    connect(finder, &Finder::pathFound, this, &MapWidget::drawPath);

    setScene(m_scene); /// установка сцены в MainWindow
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse); /// установка якоря под курсор для масштабирования
}

MapWidget::~MapWidget()
{
    m_thread->quit();
    m_thread->wait();
}

void MapWidget::solve()
{
    /// сигнал с данными для поиска пути
    emit solveRequested(m_startPoint, m_endPoint, m_mapHeight, m_mapWidth, m_obstacles);
}

void MapWidget::setMapSize(int width, int height)
{
    m_mapWidth = width;
    m_mapHeight = height;
    m_scene->setSceneRect(0,0,width * SQUARE_SIZE, height * SQUARE_SIZE);
}

void MapWidget::setStartPoint(Point start)
{
    m_startPoint = start;
}

void MapWidget::setEndPoint(Point end)
{
    m_endPoint = end;
}

void MapWidget::setObstacle(int x, int y)
{
    if(m_startPoint != Point{x, y} && m_endPoint != Point{x, y})
        m_obstacles.append({x,y});
}

void MapWidget::reset()
{ 
    clearPath();
    m_obstacles.clear();

    if(m_scene)
    {
        delete m_scene;
        m_scene = new QGraphicsScene(this);
        setScene(m_scene);
    }
}

void MapWidget::setAddingBool(bool addingObstacles)
{
    m_addingObstacles = addingObstacles;
}

void MapWidget::setSearchingBool(bool searchingWithMouse)
{
    m_searchingWithMouse = searchingWithMouse;
}

void MapWidget::clearObstacles()
{
    m_obstacles.clear();
    m_scene->update();
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if(!m_addingObstacles) /// если не устанавливаются препятствия
    {
        if (event->button() == Qt::LeftButton) /// точка начала
        {
            ///получение точки под курсором
            QPointF scenePoint = mapToScene(event->pos());
            Point point = toPoint(scenePoint);

            /// проверка того что точка находится в пределах поля, не на препятствии и не перекрывает точку конца
            if(isValidPoint(scenePoint) && !m_obstacles.contains(point))
            {
                /// нельзя поставить конец и начало в одну точку если не поиск по наведению
                if(point != m_endPoint && !m_searchingWithMouse)
                {
                    m_startPoint = point;
                    scene()->update();
                    solve();
                }
                else if (m_searchingWithMouse)
                {
                    clearPath();
                    m_startPoint = point;
                    scene()->update();
                    solve();
                }
            }
        }
        else if (event->button() == Qt::RightButton) /// точка конца
        {
            ///получение точки под курсором
            QPointF scenePoint = mapToScene(event->pos());
            Point point = toPoint(scenePoint);

            /// проверка того что точка находится в пределах поля, не на препятствии и не перекрывает точку начала
            if(isValidPoint(scenePoint) && !m_obstacles.contains(point) && point != m_startPoint)
            {
                m_endPoint = point;
                scene()->update();
                solve();
            }
        }
    }
    else
    {
        if (event->button() == Qt::LeftButton) /// добавление препятствия
        {
            ///получение точки под курсором
            QPointF scenePoint = mapToScene(event->pos());
            Point point = toPoint(scenePoint);

            /// проверка того что препятствие находится в рамках поля
            if(isValidPoint(scenePoint))
            {
                if(!m_obstacles.contains(point))
                {
                    m_obstacles.append(point);
                    scene()->update();
                    solve();
                }
            }
        }
        else if (event->button() == Qt::RightButton) /// удаление препятствия
        {
            ///получение точки под курсором
            QPointF scenePoint = mapToScene(event->pos());

            /// проверка того что препятствие находится в рамках поля
            if(isValidPoint(scenePoint))
            {
                Point obstacle = toPoint(scenePoint);

                for (int i = 0; i <m_obstacles.size(); ++i)
                {
                    if(m_obstacles.at(i) == obstacle)
                    {
                        m_obstacles.remove(i);
                        solve();
                    }
                }
                scene()->update();
            }
        }
    }
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    ///если не добавляются препятствия и включен режим поиска пути по наведению
    if(!m_addingObstacles && m_searchingWithMouse)
    {
        m_mouseTimer->start();
        ///получение точки под курсором
        QPointF scenePoint = mapToScene(event->pos());
        Point point = toPoint(scenePoint);;

        /// если курсор передвинут на другую точку
        if (point != m_lastPoint)
        {
            clearPath();
            m_lastPoint = point;

            /// проверка того что точка находится в рамках поля и не на препятствии
            if(isValidPoint(scenePoint) && !m_obstacles.contains(point))
            {
                m_endPoint = point;
                scene()->update();
            }
        }
    }
}

void MapWidget::drawBackground(QPainter *painter, const QRectF &rect)
{

    for(int x = 0; x < m_mapWidth; ++x)
    {
        for(int y = 0; y < m_mapHeight; ++y)
        {
            QRectF square(x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);

            /// отрисовка точки начала
            if(m_startPoint.x == x && m_startPoint.y == y)
            {
                painter->fillRect(square, Qt::green);
            }
            /// отрисовка точки конца
            else if (m_endPoint.x == x && m_endPoint.y == y)
            {
                painter->fillRect(square, Qt::red);
            }
            /// отрисовка препятствия
            else if(m_obstacles.contains(Point{x, y}))
            {
                painter->fillRect(square, Qt::black);
            }

            /// отрисовка границ точек
            painter->setPen(Qt::black);
            for(int x = square.left(); x < square.right() + SQUARE_SIZE; x += SQUARE_SIZE)
            {
                painter->drawLine(x,square.top(),x,square.bottom());
            }
            for(int y = square.top(); y < square.bottom() + SQUARE_SIZE; y += SQUARE_SIZE)
            {
                painter->drawLine(square.left(),y ,square.right(), y);
            }
        }
    }
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    QPoint delta = event->angleDelta(); /// градусы поворота колеса мыши

    /// если повернуто от пользователя
    if(delta.y() > 0)
    {
        if(m_currentScale * m_scaleFactor <= m_maxScale)
        {
            ///увеличить масштаб
            scale(m_scaleFactor, m_scaleFactor);
            m_currentScale *= m_scaleFactor;
        }
    }
    /// если повернуто к пользователю
    else if(delta.y() < 0 )
    {
        if(m_currentScale * (1.0 / m_scaleFactor) >= m_minScale)
        {
            /// уменьшить масштаб
            scale(1.0 / m_scaleFactor, 1.0 / m_scaleFactor);
            m_currentScale *= (1.0 / m_scaleFactor);
        }
    }
}

QVector<QPointF> MapWidget::convertPointsToQPoints(const QVector<Point> &points)
{
    QVector<QPointF> qPoints;
    for(const Point &point : points)
    {
        qPoints.append(QPointF((point.x * SQUARE_SIZE) + SQUARE_SIZE / 2, (point.y * SQUARE_SIZE) + SQUARE_SIZE / 2));
    }
    return qPoints;
}

void MapWidget::addArrowToPath(QPainterPath &path, const QPointF &startPoint, const QPointF &endPoint)
{
    const qreal arrowSize = SQUARE_SIZE / 3;

    QLineF line(endPoint, startPoint); /// для расчета направления стрелки

    double angle = acos(line.dx()/line.length());///вычисление угла стрелки

    /// угол инвертируется если разница между начальной и конечной точкой не отрицательная
    if (line.dy() >= 0)
    {
        angle = (M_PI * 2) - angle;
    }

    ///первое крыло стрелки
    QPointF arrowP1 =line.p1() + QPointF(std::sin(angle + M_PI / 3) * arrowSize, std::cos(angle + M_PI / 3) * arrowSize);
    ///второе крыло стрелки
    QPointF arrowP2 =line.p1() + QPointF(std::sin(angle + M_PI - M_PI/ 3) * arrowSize, std::cos(angle + M_PI - M_PI / 3) * arrowSize);

    ///отрисовка линий от конца пути до точек крыльев стрелки
    path.moveTo(endPoint);
    path.lineTo(arrowP1);
    path.moveTo(endPoint);
    path.lineTo(arrowP2);
    path.moveTo(endPoint);
}

void MapWidget::drawPath(const QVector<Point> &pathPoints)
{
    QVector<QPointF> points = convertPointsToQPoints(pathPoints);
    ///уведомление об отсутствии пути если выключен режим поиска по наведению
    if(points.isEmpty() && !m_searchingWithMouse)
    {
        clearPath();
        QMessageBox msgBox;
        msgBox.setText(tr("Путь не найден"));
        msgBox.setWindowTitle(tr("Ошибка"));
        msgBox.addButton(QMessageBox::Ok);
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
    else if (points.isEmpty() && m_searchingWithMouse)
    {
        clearPath();
        return;
    }

    if (points.size() < 2)
        return;

    /// построение пути по точкам
    QPainterPath path(points.at(0));

    for (int i = 1; i < points.size() ; ++i)
    {
        path.lineTo(points.at(i));
    }

    /// добавление стрелки к концу пути
    addArrowToPath(path, points[points.size() - 2], points.last());

    clearPath();

    /// отрисовка пути и его сохранение для последующей очистки
    m_lastPath = new QGraphicsPathItem(path);

    QPen pen(Qt::blue, PEN_SIZE);
    m_lastPath->setPen(pen);

    m_scene->addItem(m_lastPath);
}

void MapWidget::clearPath()
{
    if(m_lastPath)
    {
        m_scene->removeItem(m_lastPath);
        delete m_lastPath;
        m_lastPath = nullptr;
        m_scene->update();
    }
}

bool MapWidget::isValidPoint(QPointF scenePoint)
{
    return(scenePoint.x()/ SQUARE_SIZE >= 0 && scenePoint.x()/ SQUARE_SIZE < m_mapWidth &&
            scenePoint.y()/ SQUARE_SIZE >= 0 && scenePoint.y()/ SQUARE_SIZE < m_mapHeight);
}

Point MapWidget::toPoint(QPointF scenePoint)
{
    return Point{static_cast<int>(scenePoint.x())/ SQUARE_SIZE, static_cast<int>(scenePoint.y())/ SQUARE_SIZE};
}
