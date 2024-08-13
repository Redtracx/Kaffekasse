#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QStack>
#include <QLabel>
#include <QDateTime>
#include <QtCharts>
#include <QDateTimeEdit>
#include <QSettings>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QtCharts/QCategoryAxis>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Employee {
public:
    QString name;
    double totalDeposits;
    Employee(const QString &n = "") : name(n), totalDeposits(0) {}
};

struct Action {
    enum Type { AddEmployee, RemoveEmployee, Deposit, Withdraw };
    Type type;
    QString employeeName;
    double amount;
    QDateTime timestamp;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addEmployee();
    void removeEmployee();
    void deposit();
    void withdraw();
    void showChart();
    void undoLastAction();
    void toggleDarkMode();
    void showActionHistory();
    void clearAllData();
    void showInfo();
    void showGuide();
    void toggleSideMenu();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    QMap<QString, Employee> employees;
    QStack<Action> actions;
    QList<Action> actionHistory;
    QLabel *balanceLabel;
    double totalBalance = 0.0;
    bool isDarkMode;
    QPushButton *menuButton;
    QWidget *sideMenu;
    bool isSideMenuVisible;

    void setupUI();
    void updateBalanceDisplay();
    void applyStyleSheet();
    void setupSideMenu();
    QString getActionTypeString(Action::Type type);
    void saveActions();
    void loadActions();
    QChartView* createChartView(const QMap<QString, QCheckBox*> &employeeCheckboxes);
    void updateChart(QChartView *chartView, const QMap<QString, QCheckBox*> &employeeCheckboxes);
    void updateSideMenuFontSize(double scaleFactor = 1.0);
    std::function<QPushButton*(const QString&)> createMenuButton;
    QWidget *overlay;
};

#endif // MAINWINDOW_H
