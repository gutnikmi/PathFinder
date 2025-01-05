#include "finder.h"

Finder::Finder(QObject *parent)
    : QObject{parent}
{}

void Finder::findShortestPath(Point startPoint, Point endPoint, int mapHeight, int mapWidth, QVector<Point> obstacles)
{
    QVector<Point> path; /// путь до точки
    QVector<QVector<Point>> paths; /// потенциальные пути
    QQueue<Point> queue;/// очередь для поиска в ширину

    /// заполнение поля потенциальных путей непосещенными точками
    paths.resize(mapWidth);

    for (auto& row: paths)
        row.resize(mapHeight);

    for (int i = 0; i < mapWidth; i++)
    {
        for (int j = 0; j < mapHeight; j++)
        {
            paths[i][j] = {-1, -1};
        }
    }

    /// установка точки начала
    paths[startPoint.x][startPoint.y] = {-2, -2};

    queue.push_back(startPoint);

    /// поиск в ширину
    while (!queue.isEmpty())
    {
        Point p = queue.front();/// извлечение первого элемента из очереди
        queue.pop_front();

        QVector<Point> directions = {{1,0},{-1,0},{0,1},{0,-1}};/// возможные направления

        for (const Point &dir : directions)
        {
            Point next = {p.x + dir.x, p.y + dir.y};/// следующая точка

            /// проверка доступности точки
            if (isValidPoint(next, obstacles, mapHeight, mapWidth) && paths[next.x][next.y].x == -1 && paths[next.x][next.y].y == -1)
            {
                paths[next.x][next.y] = p;/// добавление точки в потенциальные пути
                queue.push_back(next); /// добавляет валидные точки в конец очереди
            }
        }
    }

    /// построение пути по пройденным точкам от конца к началу
    Point p = endPoint;
    while(!(paths[p.x][p.y] == Point{-2, -2}))
    {
        if (paths[p.x][p.y].x == -1) /// если путь не найден
        {
            emit pathFound(QVector<Point>());
            return;
        }
        path.push_front(p); ///добавть точку в путь
        p = paths[p.x][p.y]; /// следующая точка пути
    }
    path.push_front(startPoint); /// добавить точку начала к пути

    emit pathFound(path);
    return;
}

bool Finder::isValidPoint(const Point &p, QVector<Point> obstacles, int mapHeight, int mapWidth)
{
    if(p.x < 0 || p.x >= mapWidth || p.y < 0 || p.y >= mapHeight || obstacles.contains(p))
    {
        return false;
    }
    return true;
}
