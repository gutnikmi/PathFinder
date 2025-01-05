#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QSettings settings("placeHolder", "placeHolder");
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());/// загрузка положения окна

    ui->setupUi(this);
    on_generateButton_clicked();///создание первого поля
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::rand(int low, int high)
{
    return (QRandomGenerator::global()->bounded(low, high));
}

void MainWindow::on_generateButton_clicked()
{
    int width = ui->widthEdit->text().toInt();
    int height = ui->heightEdit->text().toInt();

    if(width > 100 || height > 100)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Ширина и высота поля должны быть меньше 100"));
        msgBox.setWindowTitle(tr("Введен слишком большой размер поля"));
        msgBox.addButton(QMessageBox::Ok);
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
    else if(width < 4 || height < 4)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Ширина и высота поля должны быть больше 4"));
        msgBox.setWindowTitle(tr("Введен слишком маленький размер поля"));
        msgBox.addButton(QMessageBox::Ok);
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    /// очистка поля
    ui->mapWidget->reset();

    ui->mapWidget->setMapSize(width, height);

    /// установка точки начала в левой половине карты
    ui->mapWidget->setStartPoint(Point{rand(0, width/2),rand(0, height)});
    /// установка точки конца в правой половине карты
    ui->mapWidget->setEndPoint(Point{rand(width/2, width),rand(0, height)});

    ///установка препятствий
    if (width > height)
    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < rand(0, width * m_maxObstModifier); ++j)
                ui->mapWidget->setObstacle(i,rand(0,width));
        }
    }
    else
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < rand(0, height * m_maxObstModifier); ++j)
                ui->mapWidget->setObstacle(rand(0, height), i);
        }
    }

    ///поиск пути
    ui->mapWidget->solve();
}

void MainWindow::on_clearButton_clicked()
{
    ui->mapWidget->clearObstacles();
}

void MainWindow::on_addButton_clicked()
{
    /// отключение поиска по наведению мыши при добавлении препятствия
    if(ui->searchMouseButton->isChecked())
    {
        ui->searchMouseButton->setChecked(false);
        ui->mapWidget->setSearchingBool(false);
    }
    /// устанавливает режим добавления препятствий если кнопка нажата иначе снимает
    ui->mapWidget->setAddingBool(ui->addButton->isChecked());
}

void MainWindow::on_searchMouseButton_clicked()
{
    ///отключение режима добаления препятствий
    if(ui->addButton->isChecked())
    {
        ui->addButton->setChecked(false);
        ui->mapWidget->setAddingBool(false);
    }
    /// устанавливает режим поиска по наведению если кнопка нажата иначе снимает
    ui->mapWidget->setSearchingBool(ui->searchMouseButton->isChecked());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("placeHolder", "placeHolder");
    settings.setValue("mainWindowGeometry",saveGeometry());
    QMainWindow::closeEvent(event);
}
