#pragma once

#include <QMainWindow>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QButtonGroup>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    /*!
     * \brief rand - создает случайное число
     * \param low - нижняя граница
     * \param high - верхняя граница
     * \return случайное число
     */
    int rand(int low, int high);

private slots:
    /*!
     * \brief on_generateButton_clicked - создание нового поля
     */
    void on_generateButton_clicked();
    /*!
     * \brief on_clearButton_clicked - очистка поля
     */
    void on_clearButton_clicked();
    /*!
     * \brief on_addButton_clicked - включение режима добавления препятствий
     */
    void on_addButton_clicked();
    /*!
     * \brief on_searchMouseButton_clicked - включение режима поиска по наведению мыши
     */
    void on_searchMouseButton_clicked();

protected:
    /*!
     * \brief closeEvent - переопределение события закрытия для сохранения положения окна
     * \param event - событие закрытия
     */
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

    const int m_maxObstModifier = 3; ///плотность генерации препятствий
    bool m_addingObstacles = false; /// режим добавлений препятсвтий
};
