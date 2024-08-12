/*#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

QT_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
    setWindowTitle("Moderne Mitarbeiterverwaltung");
    setStyleSheet("background-color: #f0f0f0;");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    // Kontostand Label
    balanceLabel = new QLabel(this);
    balanceLabel->setAlignment(Qt::AlignCenter);
    balanceLabel->setStyleSheet("font-size: 24pt; font-weight: bold; color: #2c3e50; margin: 20px 0;");
    updateBalanceDisplay();

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(10);

    auto createButton = [](const QString &text, const QString &color) {
        QPushButton *button = new QPushButton(text);
        button->setStyleSheet(
            QString("QPushButton {"
                    "background-color: %1;"
                    "color: white;"
                    "border: none;"
                    "padding: 10px;"
                    "border-radius: 5px;"
                    "font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "background-color: %2;"
                    "}").arg(color, color).replace("1.0)", "0.8)"));
        return button;
    };

    QPushButton *addEmployeeButton = createButton("Mitarbeiter\nhinzufügen", "rgba(46, 204, 113, 1.0)");
    QPushButton *removeEmployeeButton = createButton("Mitarbeiter\nlöschen", "rgba(231, 76, 60, 1.0)");
    QPushButton *depositButton = createButton("Einzahlen", "rgba(52, 152, 219, 1.0)");
    QPushButton *withdrawButton = createButton("Auszahlen", "rgba(241, 196, 15, 1.0)");
    QPushButton *chartButton = createButton("Säulendiagramm", "rgba(155, 89, 182, 1.0)");

    connect(addEmployeeButton, &QPushButton::clicked, this, &MainWindow::addEmployee);
    connect(removeEmployeeButton, &QPushButton::clicked, this, &MainWindow::removeEmployee);
    connect(depositButton, &QPushButton::clicked, this, &MainWindow::deposit);
    connect(withdrawButton, &QPushButton::clicked, this, &MainWindow::withdraw);
    connect(chartButton, &QPushButton::clicked, this, &MainWindow::showChart);

    buttonLayout->addWidget(addEmployeeButton);
    buttonLayout->addWidget(removeEmployeeButton);
    buttonLayout->addWidget(depositButton);
    buttonLayout->addWidget(withdrawButton);
    buttonLayout->addWidget(chartButton);

    // Hauptlayout zusammensetzen
    mainLayout->addWidget(balanceLabel);
    mainLayout->addLayout(buttonLayout);

    QWidget *container = new QWidget;
    container->setLayout(mainLayout);
    setCentralWidget(container);

    resize(800, 400);
}

void MainWindow::updateBalanceDisplay()
{
    balanceLabel->setText(QString("Kontostand: %1 €").arg(totalBalance, 0, 'f', 2));
}

void MainWindow::addEmployee()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Mitarbeiter hinzufügen", "Name des Mitarbeiters:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        if (employees.contains(name)) {
            QMessageBox::warning(this, "Fehler", "Ein Mitarbeiter mit diesem Namen existiert bereits.");
        } else {
            employees[name] = Employee(name);
            QMessageBox::information(this, "Erfolg", "Mitarbeiter hinzugefügt");
        }
    }
}

void MainWindow::removeEmployee()
{
    if (employees.isEmpty()) {
        QMessageBox::warning(this, "Fehler", "Es gibt keine Mitarbeiter zum Löschen.");
        return;
    }

    bool ok;
    QString name = QInputDialog::getItem(this, "Mitarbeiter löschen", "Wählen Sie den Mitarbeiter:", employees.keys(), 0, false, &ok);
    if (ok && !name.isEmpty()) {
        employees.remove(name);
        QMessageBox::information(this, "Erfolg", "Mitarbeiter gelöscht");
    }
}

void MainWindow::deposit()
{
    if (employees.isEmpty()) {
        QMessageBox::warning(this, "Fehler", "Es gibt keine Mitarbeiter für eine Einzahlung.");
        return;
    }

    bool ok;
    QString name = QInputDialog::getItem(this, "Einzahlung", "Wählen Sie den Mitarbeiter:", employees.keys(), 0, false, &ok);
    if (ok && !name.isEmpty()) {
        double amount = QInputDialog::getDouble(this, "Einzahlung", "Betrag eingeben:", 0, 0, 1000000, 2, &ok);
        if (ok && amount > 0) {
            employees[name].totalDeposits += amount;
            totalBalance += amount;
            updateBalanceDisplay();
            QMessageBox::information(this, "Erfolg", QString("Betrag von %1 € für %2 eingezahlt").arg(amount).arg(name));
        }
    }
}

void MainWindow::withdraw()
{
    if (totalBalance <= 0) {
        QMessageBox::warning(this, "Fehler", "Es ist kein Geld zum Auszahlen verfügbar.");
        return;
    }

    bool ok;
    double amount = QInputDialog::getDouble(this, "Auszahlung", "Betrag eingeben:", 0, 0, totalBalance, 2, &ok);
    if (ok && amount > 0) {
        if (amount <= totalBalance) {
            totalBalance -= amount;
            updateBalanceDisplay();
            QMessageBox::information(this, "Erfolg", QString("Betrag von %1 € ausgezahlt").arg(amount));
        } else {
            QMessageBox::warning(this, "Fehler", "Nicht genügend Guthaben für diese Auszahlung.");
        }
    }
}

void MainWindow::showChart()
{
    if (employees.isEmpty()) {
        QMessageBox::warning(this, "Fehler", "Es gibt keine Daten zum Anzeigen.");
        return;
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Säulendiagramm - Einzahlungen");
    dialog->setStyleSheet("background-color: #f0f0f0;");

    QVBoxLayout *dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->addWidget(createChartView());
    dialog->setLayout(dialogLayout);
    dialog->resize(800, 600);

    dialog->exec();
}

QChartView* MainWindow::createChartView()
{
    QBarSeries *series = new QBarSeries();

    double maxDeposit = 0;
    for (const Employee &employee : employees) {
        maxDeposit = qMax(maxDeposit, employee.totalDeposits);
        QBarSet *set = new QBarSet(employee.name);
        *set << employee.totalDeposits;
        series->append(set);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Einzahlungen pro Mitarbeiter");
    chart->setTitleFont(QFont("Arial", 18, QFont::Bold));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories = employees.keys();

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxDeposit * 1.1);
    axisY->setLabelFormat("%.2f €");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

*/
