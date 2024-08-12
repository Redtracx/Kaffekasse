/*#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QChart>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QVBoxLayout *layout = new QVBoxLayout;

    QPushButton *addEmployeeButton = new QPushButton("Mitarbeiter hinzufügen");
    QPushButton *removeEmployeeButton = new QPushButton("Mitarbeiter löschen");
    QPushButton *depositButton = new QPushButton("Einzahlen");
    QPushButton *withdrawButton = new QPushButton("Auszahlen");
    QPushButton *chartButton = new QPushButton("Säulendiagramm");

    connect(addEmployeeButton, &QPushButton::clicked, this, &MainWindow::addEmployee);
    connect(removeEmployeeButton, &QPushButton::clicked, this, &MainWindow::removeEmployee);
    connect(depositButton, &QPushButton::clicked, this, &MainWindow::deposit);
    connect(withdrawButton, &QPushButton::clicked, this, &MainWindow::withdraw);
    connect(chartButton, &QPushButton::clicked, this, &MainWindow::showChart);

    layout->addWidget(addEmployeeButton);
    layout->addWidget(removeEmployeeButton);
    layout->addWidget(depositButton);
    layout->addWidget(withdrawButton);
    layout->addWidget(chartButton);

    QWidget *container = new QWidget;
    container->setLayout(layout);
    setCentralWidget(container);
}

void MainWindow::addEmployee() {
    bool ok;
    QString name = QInputDialog::getText(this, "Mitarbeiter hinzufügen", "Name des Mitarbeiters:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        employees[name] = Employee(name);
        QMessageBox::information(this, "Erfolg", "Mitarbeiter hinzugefügt");
    }
}

void MainWindow::removeEmployee() {
    bool ok;
    QString name = QInputDialog::getText(this, "Mitarbeiter löschen", "Name des Mitarbeiters:", QLineEdit::Normal, "", &ok);
    if (ok && employees.contains(name)) {
        employees.remove(name);
        QMessageBox::information(this, "Erfolg", "Mitarbeiter gelöscht");
    } else {
        QMessageBox::warning(this, "Fehler", "Mitarbeiter nicht gefunden");
    }
}

void MainWindow::deposit() {
    bool ok;
    QString name = QInputDialog::getText(this, "Einzahlung", "Name des Mitarbeiters:", QLineEdit::Normal, "", &ok);
    if (ok && employees.contains(name)) {
        double amount = QInputDialog::getDouble(this, "Einzahlung", "Betrag eingeben:", 0, 0, 1000000, 2, &ok);
        if (ok) {
            employees[name].totalDeposits += amount;
            totalBalance += amount;
            QMessageBox::information(this, "Erfolg", "Betrag eingezahlt");
        }
    } else {
        QMessageBox::warning(this, "Fehler", "Mitarbeiter nicht gefunden");
    }
}

void MainWindow::withdraw() {
    bool ok;
    double amount = QInputDialog::getDouble(this, "Auszahlung", "Betrag eingeben:", 0, 0, 1000000, 2, &ok);
    if (ok && amount <= totalBalance) {
        totalBalance -= amount;
        QMessageBox::information(this, "Erfolg", "Betrag ausgezahlt");
    } else {
        QMessageBox::warning(this, "Fehler", "Nicht genügend Guthaben");
    }
}

void MainWindow::showChart() {
    QBarSeries *series = new QBarSeries();

    for (const Employee& employee : employees) {
        QBarSet *set = new QBarSet(employee.name);
        *set << employee.totalDeposits;
        series->append(set);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Einzahlungen pro Mitarbeiter");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    for (const QString& name : employees.keys()) {
        categories << name;
    }

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 1000000);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QDialog *dialog = new QDialog(this);
    QVBoxLayout *dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->addWidget(chartView);
    dialog->setLayout(dialogLayout);
    dialog->exec();
}
*/
