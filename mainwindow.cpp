#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QApplication>
#include <QTableWidget>
#include <QDateEdit>

QT_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isDarkMode(true)
    , createMenuButton([](const QString &text) {
        QPushButton *btn = new QPushButton(text);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setStyleSheet("QPushButton { background-color: #2c3e50; color: #ecf0f1; border: none; padding: 10px; text-align: left; }"
                           "QPushButton:hover { background-color: #3498db; }");
        return btn;
    })
    , overlay(nullptr)
{
    ui->setupUi(this);
    setupUI();
    loadActions();
    setWindowTitle("STJ Kaffeekasse");
    applyStyleSheet();

    // Menü-Button hinzufügen
    menuButton = new QPushButton(this);
    menuButton->setIcon(QIcon(":/assets/icon.png"));
    menuButton->setIconSize(QSize(40, 40));
    menuButton->setStyleSheet("QPushButton { border: none; }");
    menuButton->setFixedSize(50, 50);
    connect(menuButton, &QPushButton::clicked, this, &MainWindow::toggleSideMenu);

    // Seitenmenü erstellen
    setupSideMenu();

    // Menü-Button zum Layout hinzufügen
    QWidget *centralWidget = this->centralWidget();
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());
    if (mainLayout) {
        QHBoxLayout *topLayout = new QHBoxLayout();
        topLayout->addWidget(menuButton);
        topLayout->addStretch();
        mainLayout->insertLayout(0, topLayout);
    }
}

void MainWindow::setupSideMenu()
{
    sideMenu = new QWidget(this);
    sideMenu->setFixedWidth(250);
    sideMenu->setStyleSheet("background-color: #34495e; color: #ecf0f1;");
    sideMenu->hide();

    QVBoxLayout *menuLayout = new QVBoxLayout(sideMenu);

    // Verwenden Sie die bestehende createMenuButton Lambda-Funktion
    QPushButton *addEmployeeBtn = createMenuButton("Mitarbeiter hinzufügen");
    QPushButton *removeEmployeeBtn = createMenuButton("Mitarbeiter entfernen");
    QPushButton *historyBtn = createMenuButton("Verlauf anzeigen");
    QPushButton *modeBtn = createMenuButton("Modus ändern");
    QPushButton *clearDataBtn = createMenuButton("Daten löschen");
    QPushButton *infoBtn = createMenuButton("Info");
    QPushButton *guideBtn = createMenuButton("Anleitung");

    connect(addEmployeeBtn, &QPushButton::clicked, this, &MainWindow::addEmployee);
    connect(removeEmployeeBtn, &QPushButton::clicked, this, &MainWindow::removeEmployee);
    connect(historyBtn, &QPushButton::clicked, this, &MainWindow::showActionHistory);
    connect(modeBtn, &QPushButton::clicked, this, &MainWindow::toggleDarkMode);
    connect(clearDataBtn, &QPushButton::clicked, this, &MainWindow::clearAllData);
    connect(infoBtn, &QPushButton::clicked, this, &MainWindow::showInfo);
    connect(guideBtn, &QPushButton::clicked, this, &MainWindow::showGuide);

    menuLayout->addWidget(addEmployeeBtn);
    menuLayout->addWidget(removeEmployeeBtn);
    menuLayout->addWidget(historyBtn);
    menuLayout->addWidget(modeBtn);
    menuLayout->addWidget(clearDataBtn);
    menuLayout->addWidget(infoBtn);
    menuLayout->addWidget(guideBtn);
    menuLayout->addStretch();

    sideMenu->setLayout(menuLayout);
}

void MainWindow::updateSideMenuFontSize(double scaleFactor)
{
    if (!sideMenu) return;

    QList<QPushButton*> buttons = sideMenu->findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        int fontSize = qRound(12 * scaleFactor);
        int padding = qRound(10 * scaleFactor);
        button->setStyleSheet(button->styleSheet() +
                              QString("font-size: %1pt; padding: %2px;")
                                  .arg(fontSize)
                                  .arg(padding));
    }
}
void MainWindow::toggleSideMenu()
{
    if (sideMenu->isVisible()) {
        sideMenu->hide();
        if (overlay) {
            overlay->hide();
            overlay->deleteLater();
            overlay = nullptr;
        }
    } else {
        int menuWidth = qMin(width() / 3, 250);  // Maximal 1/3 der Fensterbreite oder 250px
        sideMenu->setFixedWidth(menuWidth);
        sideMenu->setGeometry(0, menuButton->height(), menuWidth, height() - menuButton->height());
        sideMenu->show();
        sideMenu->raise();

        // Erstelle und zeige den Overlay
        overlay = new QWidget(this);
        overlay->setGeometry(menuWidth, menuButton->height(), width() - menuWidth, height() - menuButton->height());
        overlay->setStyleSheet("background-color: rgba(0, 0, 0, 50);"); // Halbtransparenter schwarzer Hintergrund
        overlay->show();

        // Installiere einen Event-Filter für den Overlay
        overlay->installEventFilter(this);
    }
}

void MainWindow::clearAllData()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Daten löschen");
    QVBoxLayout layout(&dialog);

    QLabel label("Geben Sie 'BESTÄTIGEN' ein, um alle Daten zu löschen:");
    QLineEdit lineEdit;
    QPushButton confirmButton("Löschen");

    layout.addWidget(&label);
    layout.addWidget(&lineEdit);
    layout.addWidget(&confirmButton);

    connect(&confirmButton, &QPushButton::clicked, [&]() {
        if (lineEdit.text() == "BESTÄTIGEN") {
            employees.clear();
            actions.clear();
            actionHistory.clear();
            totalBalance = 0.0;
            updateBalanceDisplay();
            QMessageBox::information(this, "Erfolg", "Alle Daten wurden gelöscht.");
            dialog.accept();
        } else {
            QMessageBox::warning(this, "Fehler", "Falsche Eingabe. Daten wurden nicht gelöscht.");
        }
    });

    dialog.exec();
}

void MainWindow::showInfo()
{
    QMessageBox::information(this, "Info", "Kaffeekassenprogramm\n"
                                           "Release v1.0\n"
                                           "© 2024 Redtracx");
}

void MainWindow::showGuide()
{
    QString guideText =
        "Anleitung für STJ Kaffeekasse\n\n"
        "1. Hauptfunktionen:\n"
        "   a) Einzahlen: Wählen Sie einen Mitarbeiter und geben Sie den Einzahlungsbetrag ein.\n"
        "   b) Auszahlen: Geben Sie den auszuzahlenden Betrag ein.\n"
        "   c) Säulendiagramm: Zeigt die Einzahlungen pro Mitarbeiter grafisch an.\n"
        "   d) Rückgängig: Macht die letzte Aktion rückgängig.\n\n"
        "2. Seitenmenü (oben links):\n"
        "   a) Mitarbeiter hinzufügen: Fügt einen neuen Mitarbeiter hinzu.\n"
        "   b) Mitarbeiter entfernen: Entfernt einen bestehenden Mitarbeiter.\n"
        "   c) Verlauf anzeigen: Zeigt alle bisherigen Aktionen tabellarisch an.\n"
        "   d) Modus ändern: Wechselt zwischen Hell- und Dunkelmodus.\n"
        "   e) Daten löschen: Löscht alle gespeicherten Daten (Vorsicht!).\n"
        "   f) Info: Zeigt Programminformationen an.\n"
        "   g) Anleitung: Zeigt diese Hilfe an.\n\n"
        "3. Tipps:\n"
        "   - Überprüfen Sie regelmäßig den Gesamtkontostand.\n"
        "   - Nutzen Sie das Säulendiagramm für einen Überblick der Einzahlungen.\n"
        "   - Verwenden Sie die Verlaufsansicht zur Überprüfung spezifischer Transaktionen.\n"
        "   - Bei Fehlern nutzen Sie die 'Rückgängig'-Funktion.\n\n"
        "4. Sicherheit:\n"
        "   - Löschen Sie Daten nur, wenn Sie sich absolut sicher sind.\n"
        "   - Überprüfen Sie regelmäßig die Richtigkeit der Ein- und Auszahlungen.\n"
        "   - Bei Unstimmigkeiten nutzen Sie die Verlaufsansicht zur Überprüfung.\n\n"
        "Das Programm speichert automatisch alle Aktionen und den aktuellen Kontostand beim Beenden.";

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Anleitung");
    dialog->setStyleSheet(isDarkMode ? "background-color: #2c3e50; color: #ecf0f1;" : "background-color: #f0f0f0; color: #2c3e50;");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QTextEdit *textEdit = new QTextEdit(dialog);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(guideText);
    textEdit->setStyleSheet("QTextEdit { border: none; }");

    layout->addWidget(textEdit);

    QPushButton *closeButton = new QPushButton("Schließen", dialog);
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);
    layout->addWidget(closeButton);

    dialog->setLayout(layout);
    dialog->resize(600, 400);
    dialog->exec();
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
    balanceLabel->setStyleSheet("font-size: 24pt; font-weight: bold; margin: 20px 0;");
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

    QPushButton *depositButton = createButton("Einzahlen", "rgba(52, 152, 219, 1.0)");
    QPushButton *withdrawButton = createButton("Auszahlen", "rgba(241, 196, 15, 1.0)");
    QPushButton *chartButton = createButton("Säulendiagramm", "rgba(155, 89, 182, 1.0)");
    QPushButton *undoButton = createButton("Rückgängig", "rgba(149, 165, 166, 1.0)");

    connect(depositButton, &QPushButton::clicked, this, &MainWindow::deposit);
    connect(withdrawButton, &QPushButton::clicked, this, &MainWindow::withdraw);
    connect(chartButton, &QPushButton::clicked, this, &MainWindow::showChart);
    connect(undoButton, &QPushButton::clicked, this, &MainWindow::undoLastAction);

    buttonLayout->addWidget(depositButton);
    buttonLayout->addWidget(withdrawButton);
    buttonLayout->addWidget(chartButton);
    buttonLayout->addWidget(undoButton);

    // Hauptlayout zusammensetzen
    mainLayout->addWidget(balanceLabel);
    mainLayout->addLayout(buttonLayout);

    QWidget *container = new QWidget;
    container->setLayout(mainLayout);
    setCentralWidget(container);

    // Setze eine Anfangsgröße für das Fenster
    resize(800, 300);
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
            Action newAction = {Action::AddEmployee, name, 0, QDateTime::currentDateTime()};
            actions.push(newAction);
            actionHistory.append(newAction);
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
        double totalDeposits = employees[name].totalDeposits;
        Action newAction = {Action::RemoveEmployee, name, totalDeposits, QDateTime::currentDateTime()};
        actions.push(newAction);
        actionHistory.append(newAction);
        employees.remove(name);
        QMessageBox::information(this, "Erfolg", "Mitarbeiter gelöscht (Einzahlungen bleiben erhalten)");
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
            Action newAction = {Action::Deposit, name, amount, QDateTime::currentDateTime()};
            actions.push(newAction);
            actionHistory.append(newAction);
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
            Action newAction = {Action::Withdraw, "", amount, QDateTime::currentDateTime()};
            actions.push(newAction);
            actionHistory.append(newAction);
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
    dialog->setStyleSheet(isDarkMode ? "background-color: #2c3e50; color: #ecf0f1;" : "background-color: #f0f0f0; color: #2c3e50;");

    QVBoxLayout *dialogLayout = new QVBoxLayout(dialog);

    // Erstellen Sie ein Widget für die Checkboxen
    QWidget *checkboxWidget = new QWidget(dialog);
    QVBoxLayout *checkboxLayout = new QVBoxLayout(checkboxWidget);
    checkboxLayout->setAlignment(Qt::AlignLeft);
    checkboxLayout->setSpacing(5);
    QMap<QString, QCheckBox*> employeeCheckboxes;

    for (const auto &employee : employees.keys()) {
        QCheckBox *checkbox = new QCheckBox(employee, checkboxWidget);
        checkbox->setChecked(true);
        employeeCheckboxes[employee] = checkbox;
        checkboxLayout->addWidget(checkbox);
    }

    // Fügen Sie einen Stretch hinzu, um die Checkboxen oben zu halten
    checkboxLayout->addStretch();

    // Erstellen Sie ein horizontales Layout für Checkboxen und Chart
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->addWidget(checkboxWidget);

    // Erstellen Sie das Chart-View
    QChartView *chartView = createChartView(employeeCheckboxes);
    contentLayout->addWidget(chartView, 1);  // Geben Sie dem Chart mehr Platz

    dialogLayout->addLayout(contentLayout);

    // Verbinden Sie die Checkboxen mit der Aktualisierungsfunktion
    for (auto checkbox : employeeCheckboxes) {
        connect(checkbox, &QCheckBox::stateChanged, [this, chartView, &employeeCheckboxes]() {
            updateChart(chartView, employeeCheckboxes);
        });
    }

    dialog->setLayout(dialogLayout);
    dialog->resize(1000, 600);  // Größeres Fenster für bessere Sichtbarkeit

    dialog->exec();
}

QChartView* MainWindow::createChartView(const QMap<QString, QCheckBox*> &employeeCheckboxes)
{
    QBarSeries *series = new QBarSeries();

    double maxDeposit = 0;
    QStringList categories;

    QBarSet *set = new QBarSet("Einzahlungen");

    for (const auto &pair : employees.toStdMap()) {
        const Employee &employee = pair.second;
        if (employeeCheckboxes[employee.name]->isChecked()) {
            categories << employee.name;
            *set << employee.totalDeposits;
            maxDeposit = qMax(maxDeposit, employee.totalDeposits);
        }
    }

    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Einzahlungen pro Mitarbeiter");
    chart->setTitleFont(QFont("Arial", 18, QFont::Bold));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsAngle(-90);  // Dreht die Labels um 90 Grad
    axisX->setLabelsBrush(QBrush(isDarkMode ? QColor("#ecf0f1") : QColor("#2c3e50")));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, qCeil(maxDeposit * 1.1));
    axisY->setLabelFormat("%d");  // Ganze Zahlen
    axisY->setTickCount(11);  // 10 Schritte + 0
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);

    // Linksbündige Ausrichtung durch Anpassung der Achsenoptionen
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    axisX->setGridLineVisible(false);
    axisX->setLineVisible(false);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Anpassen der Farben für den Dark Mode
    if (isDarkMode) {
        chart->setBackgroundBrush(QBrush(QColor("#2c3e50")));
        chart->setTitleBrush(QBrush(QColor("#ecf0f1")));
        axisX->setLabelsColor(QColor("#ecf0f1"));
        axisY->setLabelsColor(QColor("#ecf0f1"));
        set->setColor(QColor("#3498db"));
    }

    return chartView;
}

void MainWindow::updateChart(QChartView *chartView, const QMap<QString, QCheckBox*> &employeeCheckboxes)
{
    QChart *chart = chartView->chart();
    chart->removeAllSeries();

    QBarSeries *series = new QBarSeries();
    QBarSet *set = new QBarSet("Einzahlungen");

    double maxDeposit = 0;
    QStringList categories;

    for (const auto &pair : employees.toStdMap()) {
        const Employee &employee = pair.second;
        if (employeeCheckboxes[employee.name]->isChecked()) {
            categories << employee.name;
            *set << employee.totalDeposits;
            maxDeposit = qMax(maxDeposit, employee.totalDeposits);
        }
    }

    series->append(set);
    chart->addSeries(series);

    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis*>(chart->axes(Qt::Horizontal).first());
    axisX->clear();
    axisX->append(categories);

    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    axisY->setRange(0, qCeil(maxDeposit * 1.1));

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    if (isDarkMode) {
        set->setColor(QColor("#3498db"));
    }
}

void MainWindow::undoLastAction()
{
    if (actions.isEmpty()) {
        QMessageBox::information(this, "Information", "Keine Aktionen zum Rückgängig machen.");
        return;
    }

    Action lastAction = actions.pop();
    actionHistory.removeLast();  // Entfernen Sie die letzte Aktion aus der Historie
    switch (lastAction.type) {
    case Action::Deposit:
        employees[lastAction.employeeName].totalDeposits -= lastAction.amount;
        totalBalance -= lastAction.amount;
        break;
    case Action::Withdraw:
        totalBalance += lastAction.amount;
        break;
    case Action::AddEmployee:
        employees.remove(lastAction.employeeName);
        break;
    case Action::RemoveEmployee:
        employees[lastAction.employeeName] = Employee(lastAction.employeeName);
        employees[lastAction.employeeName].totalDeposits = lastAction.amount;
        break;
    }
    updateBalanceDisplay();
    QMessageBox::information(this, "Erfolg", "Letzte Aktion rückgängig gemacht");
}

void MainWindow::toggleDarkMode()
{
    isDarkMode = !isDarkMode;
    applyStyleSheet();
}

void MainWindow::applyStyleSheet()
{
    if (isDarkMode) {
        setStyleSheet(
            "QWidget { background-color: #2c3e50; color: #ecf0f1; }"
            "QPushButton { background-color: #34495e; color: #ecf0f1; border: none; padding: 10px; border-radius: 5px; }"
            "QPushButton:hover { background-color: #4e6d8c; }"
            "QLabel { color: #ecf0f1; }"
            );
    } else {
        setStyleSheet(
            "QWidget { background-color: #f0f0f0; color: #2c3e50; }"
            "QPushButton { background-color: #3498db; color: white; border: none; padding: 10px; border-radius: 5px; }"
            "QPushButton:hover { background-color: #2980b9; }"
            "QLabel { color: #2c3e50; }"
            );
    }
}

void MainWindow::showActionHistory()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Aktionsverlauf");
    dialog->setStyleSheet(isDarkMode ? "background-color: #2c3e50; color: #ecf0f1;" : "background-color: #f0f0f0; color: #2c3e50;");

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QTableWidget *tableWidget = new QTableWidget(dialog);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Datum", "Aktion", "Mitarbeiter", "Betrag"});

    QDateTimeEdit *startDate = new QDateTimeEdit(QDate::currentDate().addDays(-30), dialog);
    QDateTimeEdit *endDate = new QDateTimeEdit(QDate::currentDate(), dialog);
    startDate->setCalendarPopup(true);
    endDate->setCalendarPopup(true);
    startDate->setDisplayFormat("dd.MM.yyyy");
    endDate->setDisplayFormat("dd.MM.yyyy");

    QComboBox *actionTypeFilter = new QComboBox(dialog);
    actionTypeFilter->addItem("Alle Aktionen", -1);
    actionTypeFilter->addItem("Mitarbeiter hinzugefügt", Action::AddEmployee);
    actionTypeFilter->addItem("Mitarbeiter entfernt", Action::RemoveEmployee);
    actionTypeFilter->addItem("Einzahlung", Action::Deposit);
    actionTypeFilter->addItem("Auszahlung", Action::Withdraw);

    QPushButton *filterButton = new QPushButton("Filter anwenden", dialog);

    connect(filterButton, &QPushButton::clicked, [=]() {
        tableWidget->setRowCount(0);
        int selectedActionType = actionTypeFilter->currentData().toInt();
        for (const Action &action : actionHistory) {
            if (action.timestamp.date() >= startDate->date() &&
                action.timestamp.date() <= endDate->date() &&
                (selectedActionType == -1 || selectedActionType == action.type)) {
                int row = tableWidget->rowCount();
                tableWidget->insertRow(row);
                tableWidget->setItem(row, 0, new QTableWidgetItem(action.timestamp.toString("dd.MM.yyyy hh:mm")));
                tableWidget->setItem(row, 1, new QTableWidgetItem(getActionTypeString(action.type)));
                tableWidget->setItem(row, 2, new QTableWidgetItem(action.employeeName));
                tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(action.amount, 'f', 2) + " €"));
            }
        }
    });

    QHBoxLayout *filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel("Von:"));
    filterLayout->addWidget(startDate);
    filterLayout->addWidget(new QLabel("Bis:"));
    filterLayout->addWidget(endDate);
    filterLayout->addWidget(actionTypeFilter);
    filterLayout->addWidget(filterButton);

    layout->addLayout(filterLayout);
    layout->addWidget(tableWidget);

    dialog->setLayout(layout);
    dialog->resize(800, 600);

    filterButton->click();

    dialog->exec();
}

QString MainWindow::getActionTypeString(Action::Type type)
{
    switch (type) {
    case Action::AddEmployee: return "Mitarbeiter hinzugefügt";
    case Action::RemoveEmployee: return "Mitarbeiter entfernt";
    case Action::Deposit: return "Einzahlung";
    case Action::Withdraw: return "Auszahlung";
    default: return "Unbekannt";
    }
}

void MainWindow::saveActions()
{
    QSettings settings("YourCompany", "EmployeeManagement");
    settings.beginWriteArray("actions");
    for (int i = 0; i < actions.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("type", (int)actions[i].type);
        settings.setValue("employeeName", actions[i].employeeName);
        settings.setValue("amount", actions[i].amount);
        settings.setValue("timestamp", actions[i].timestamp);
    }
    settings.endArray();

    // Speichern Sie auch den aktuellen Kontostand
    settings.setValue("totalBalance", totalBalance);
}


void MainWindow::loadActions()
{
    QSettings settings("YourCompany", "EmployeeManagement");
    int size = settings.beginReadArray("actions");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        Action action;
        action.type = (Action::Type)settings.value("type").toInt();
        action.employeeName = settings.value("employeeName").toString();
        action.amount = settings.value("amount").toDouble();
        action.timestamp = settings.value("timestamp").toDateTime();
        actions.push(action);
        actionHistory.append(action);
    }
    settings.endArray();

    // Laden Sie den gespeicherten Kontostand
    totalBalance = settings.value("totalBalance", 0.0).toDouble();
    updateBalanceDisplay();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveActions();
    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    static const QSize initialSize(800, 300); // Anfangsgröße des Fensters

    QMainWindow::resizeEvent(event);

    // Berechne den Vergrößerungsfaktor
    double widthFactor = (double)width() / initialSize.width();
    double heightFactor = (double)height() / initialSize.height();
    double scaleFactor = qMin(widthFactor, heightFactor);

    // Begrenze den Skalierungsfaktor
    scaleFactor = qMin(scaleFactor, 2.0); // Maximale Verdopplung der Größe

    // Passe die Schriftgröße des Kontostand-Labels an
    int fontSize = qRound(24 * scaleFactor);
    balanceLabel->setStyleSheet(QString("font-size: %1pt; font-weight: bold; margin: %2px 0;")
                                    .arg(fontSize)
                                    .arg(qRound(20 * scaleFactor)));

    // Passe die Schriftgröße und Padding der Buttons an
    QList<QPushButton*> buttons = centralWidget()->findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        int buttonFontSize = qRound(12 * scaleFactor);
        int buttonPadding = qRound(10 * scaleFactor);
        button->setStyleSheet(button->styleSheet() +
                              QString("font-size: %1pt; padding: %2px;")
                                  .arg(buttonFontSize)
                                  .arg(buttonPadding));

        // Passe die Mindestgröße der Buttons an
        int minWidth = qRound(100 * scaleFactor);
        int minHeight = qRound(40 * scaleFactor);
        button->setMinimumSize(minWidth, minHeight);
    }

    // Passe die Größe und Position des Seitenmenüs an
    if (sideMenu) {
        int menuWidth = qMin(qRound(250 * scaleFactor), width() / 3);
        sideMenu->setFixedWidth(menuWidth);
        sideMenu->setGeometry(0, menuButton->height(), menuWidth, height() - menuButton->height());
    }

    // Aktualisiere die Schriftgröße im Seitenmenü
    updateSideMenuFontSize(scaleFactor);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == overlay && event->type() == QEvent::MouseButtonPress) {
        toggleSideMenu();
        return true;
    }
    return QMainWindow::eventFilter(watched, event);
}
