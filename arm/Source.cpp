#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addpersondialog.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rasstroystva_ball.resize(8);

    //создаем базу данных с драйвером
    db = QSqlDatabase::addDatabase("QSQLITE");
    //размещение базы данных
    db.setDatabaseName("./testDb.db");
    if (db.open())
        qDebug("OPen");
    else
        qDebug("No Open");

    //вывод базы данных
    query = new QSqlQuery(db);
    query->exec("CREATE TABLE Client(Дата TEXT, Время TIME, ФИО TEXT);");   //создание/открытие таблицы

    model = new CustomSqlTableModel(this, db);   //создание кастомной модели для вывода в приложение.
    model->setTable("Client");
    QDate currentDate = QDate::currentDate();
    QString currentDateString = currentDate.toString("dd.MM.yyyy");
    model->setFilter("Дата = '" + currentDateString + "'");
    model->setSort(1, Qt::AscendingOrder);
    model->select();
    //model->setEditStrategy(QSqlTableModel::OnFieldChange);
    ui->tablePerson->setModel(model);
    //ui->tablePerson->resizeColumnToContents(0);
    //ui->tablePerson->resizeColumnToContents(1);
    ui->tablePerson->setColumnWidth(0, 70);
    ui->tablePerson->setColumnWidth(1, 70);
    ui->tablePerson->setColumnWidth(2, 163);
    this->row = -1;
    ui->calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    createGrahpicStart();   //создание пустого окна для графика
    //вывод картинки
    QPixmap pix(":/img/img/img.jpg");
    int w = ui->PixLabel->width();
    ui->PixLabel->setPixmap(pix.scaledToWidth(w));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//открытие окна для записи клиента
void MainWindow::on_addButton_clicked()
{
    addPersonDialog windowDialog(this, model);
    windowDialog.setModal(true);
    windowDialog.exec();
}

//установка выбранной строки
void MainWindow::on_tablePerson_clicked(const QModelIndex& index)
{
    row = index.row();
}
//удаление клиента
void MainWindow::on_deleteButton_clicked()
{
    if (row == -1)
        QMessageBox::information(nullptr, "Внимание", "Выберите фамилию для удаления.");
    else {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Удаление", "Удалить пациента?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            model->removeRow(row);
            row = -1;
        }
    }
}

//при смене даты выполняется sql запрос для вывода только нужных записей
void MainWindow::on_calendarWidget_selectionChanged()
{
    QDate selectedDate = ui->calendarWidget->selectedDate();
    QString currentDateString = selectedDate.toString("dd.MM.yyyy");
    model->setFilter("Дата = '" + currentDateString + "'");
    model->select();
}



//отпрака email через сокет
void MainWindow::on_pushEmail_clicked()
{
    QString username = "pstu.ucheba@gmail.com"; // Ваша учетная запись Gmail
    QString password = "ufvggvdptehdogbj"; // Пароль от учетной записи Gmail
    QString fromAddress = "pstu.ucheba@gmail.com"; // учетная запись Gmail
    QString toAddress = ui->textEmailAdress->text(); // Адрес получателя
    QString subject = "Диагноз и лечение"; // Тема письма
    QString body = ui->textEmail->toPlainText(); // Текст письма

    QString from = "Лечащий врач";
    QString to = "Пациент";
    QTcpSocket socket;
    socket.connectToHost("smtp.gmail.com", 587); // Подключение к серверу по адресу smtp.gmail.com и порту 587 (SSL)

    if (socket.waitForConnected()) {
        // Соединение успешно установлено
        qDebug() << "Connected to SMTP server";


        socket.write("HELO localhost\r\n"); // Отправка приветственного сообщения "EHLO localhost"

        if (socket.waitForBytesWritten()) {
            qDebug() << "EHLO sent";
        }
        else {
            qDebug() << "Error sending EHLO: " << socket.errorString();
        }

        socket.write("AUTH LOGIN\r\n"); // Отправка команды "AUTH LOGIN"

        if (socket.waitForBytesWritten()) {
            qDebug() << "AUTH LOGIN sent";
        }
        else {
            qDebug() << "Error sending AUTH LOGIN: " << socket.errorString();
        }

        QString encodedUsername = QByteArray().append(username.toUtf8().toBase64()) + "\r\n";//кодировани данных, для безопасной отправки
        QString encodedPassword = QByteArray().append(password.toUtf8().toBase64()) + "\r\n";

        socket.write(encodedUsername.toUtf8());
        socket.write(encodedPassword.toUtf8());

        if (socket.waitForBytesWritten()) {
            qDebug() << "Username and password sent";
        }
        else {
            qDebug() << "Error sending username and password: " << socket.errorString();
        }

        QString mailFromCommand = "MAIL FROM: <" + fromAddress + ">\r\n";       //отправка данных письма
        socket.write(mailFromCommand.toUtf8());

        if (socket.waitForBytesWritten()) {
            qDebug() << "MAIL FROM command sent";
        }
        else {
            qDebug() << "Error sending MAIL FROM command: " << socket.errorString();
        }

        QString rcptToCommand = "RCPT TO: <" + toAddress + ">\r\n";
        socket.write(rcptToCommand.toUtf8());

        if (socket.waitForBytesWritten()) {
            qDebug() << "RCPT TO command sent";
        }
        else {
            qDebug() << "Error sending RCPT TO command: " << socket.errorString();
        }

        QString dataCommand = "DATA\r\n";
        socket.write(dataCommand.toUtf8());

        if (socket.waitForBytesWritten()) {
            qDebug() << "DATA command sent";
        }
        else {
            qDebug() << "Error sending DATA command: " << socket.errorString();
        }

        QString headers = "From: " + from + "\r\n"
            "To: " + to + "\r\n"
            "Subject: " + subject + "\r\n"
            "\r\n";  // Пустая строка для разделения заголовков и тела

        QString message = headers + body + "\r\n.\r\n";  // Добавляем точку в конце сообщения для обозначения окончания

        socket.write(message.toUtf8());

        if (socket.waitForBytesWritten()) {
            qDebug() << "Message sent";
        }
        else {
            qDebug() << "Error sending message: " << socket.errorString();
        }

        socket.write("QUIT\r\n");

        if (socket.waitForBytesWritten()) {
            qDebug() << "QUIT command sent";
        }
        else {
            qDebug() << "Error sending QUIT command: " << socket.errorString();
        }

        socket.disconnectFromHost();
        if (socket.state() == QAbstractSocket::UnconnectedState) {
            qDebug() << "Connection closed";
        }
        else {
            qDebug() << "Error: Connection not closed";
        }

    }
    else {
        // Произошла ошибка при подключении
        qDebug() << "Error connecting to SMTP server: " << socket.errorString();
    }



}
//открытие стандартного офисного клиента с предзаполненными данными
void MainWindow::on_pushButtonOpen_clicked()
{
    QString subject = "Результаты обследования и назначения";
    QString body = ui->textEmail->toPlainText();
    QString recipient = ui->textEmailAdress->text();

    QString mailtoLink = "mailto:" + recipient + "?subject=" + subject + "&body=" + body;
    QDesktopServices::openUrl(QUrl(mailtoLink));
}

//нажатие на кнопку обновить данные об опросе
void MainWindow::on_pushButtonF5_clicked()
{

    std::vector<QString> name = { "Диагноз не выявлен.", "Есть подозрения. Необходимо дополнительное обследование.", "Есть подозрения. Необходимо дополнительное обследование.", "Подтверждено." };
    int row_count = rasstroystva.size();
    int column_count = 2;
    ui->tableWidgetDiagnoz->clear();
    ui->tableWidgetDiagnoz->setRowCount(row_count);
    ui->tableWidgetDiagnoz->setColumnCount(column_count);
    for (int row = 0; row < row_count; ++row) {
        QTableWidgetItem* item1 = new QTableWidgetItem(rasstroystva[row]);
        QTableWidgetItem* item2 = new QTableWidgetItem(name[rasstroystva_ball[row]]);
        ui->tableWidgetDiagnoz->setItem(row, 0, item1); // Значения из vector1 в первый столбец
        ui->tableWidgetDiagnoz->setItem(row, 1, item2); // Значения из vector2 во второй столбец
    }
    ui->tableWidgetDiagnoz->resizeColumnsToContents();

    QHeaderView* header = ui->tableWidgetDiagnoz->horizontalHeader();
    header->setStretchLastSection(true);
    QStringList labels;
    labels << "Диагноз." << "Результат тестирования.";
    ui->tableWidgetDiagnoz->setHorizontalHeaderLabels(labels);
    ui->tableWidgetDiagnoz->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//нажатие на кнопку для того, чтобы открылось окно опросника
void MainWindow::on_pushButton_clicked()
{
    windowWithTest = new testForClient(this);
    windowWithTest->show();
    connect(&(*windowWithTest), &testForClient::VectorSaved, this, &MainWindow::VectorDataSaved);
}

//функция отвечаюзая за построение графика настроения
void MainWindow::createGraphic()
{
    matrix.clear();
    matrix.resize(7);
    int temp, previous;

    for (int i = 0; i < matrix.size(); ++i) {
        temp = rand() % 15;
        if (temp > 6)
            matrix[i].resize(temp);
    }
    for (int i = 0; i < matrix.size(); ++i) {
        temp = rand() % 100;
        for (int j = 0; j < matrix[i].size(); ++j) {
            previous = temp;
            temp = previous + (rand() % 11 - 5);
            if (temp > 100) temp = 100;
            if (temp < 0) temp = 0;
            matrix[i][j] = temp;
        }
    }

    // Создание данных для графика
    QtCharts::QLineSeries* series = new QtCharts::QLineSeries();
    for (int i = 0; i < matrix[0].size(); ++i) {
        series->append(i * 5, matrix[0][i]);
    }

    QLayoutItem* item = ui->layoutGraphic->takeAt(0);
    if (item) {
        QWidget* widget = item->widget();
        if (widget) {
            widget->deleteLater();
        }
        delete item;
    }
    QtCharts::QChart* chart = new QtCharts::QChart();
    series->setName("Линия настроения");
    chart->addSeries(series);

    QtCharts::QValueAxis* axisX = new QtCharts::QValueAxis();
    QtCharts::QValueAxis* axisY = new QtCharts::QValueAxis();
    axisX->setRange(0, 100);
    axisY->setRange(0, 100);
    axisX->setTitleText("Дни");
    axisY->setTitleText("Проценты");
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);

    chart->createDefaultAxes();
    QtCharts::QChartView* chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->layoutGraphic->addWidget(chartView);
}

//вывод обновленного графика при нажатии на клиента
void MainWindow::on_tablePerson_doubleClicked(const QModelIndex& index)
{
    if (index.column() != 2) return;
    createGraphic();
}
//функция для составление и вывода пустого графика
void MainWindow::createGrahpicStart()
{
    matrix.clear();
    matrix.resize(7);
    int temp, previous;

    for (int i = 0; i < matrix.size(); ++i) {
        temp = rand() % 15;
        if (temp > 16)
            matrix[i].resize(temp);
    }
    for (int i = 0; i < matrix.size(); ++i) {
        temp = rand() % 100;
        for (int j = 0; j < matrix[i].size(); ++j) {
            previous = temp;
            temp = previous + (rand() % 11 - 5);
            if (temp > 100) temp = 100;
            if (temp < 0) temp = 0;
            matrix[i][j] = temp;
        }
    }

    // Создание данных для графика
    QtCharts::QLineSeries* series = new QtCharts::QLineSeries();
    for (int i = 0; i < matrix[0].size(); ++i) {
        series->append(i * 5, matrix[0][i]);
    }

    QLayoutItem* item = ui->layoutGraphic->takeAt(0);
    if (item) {
        QWidget* widget = item->widget();
        if (widget) {
            widget->deleteLater();
        }
        delete item;
    }
    QtCharts::QChart* chart = new QtCharts::QChart();
    series->setName("Линия настроения");
    chart->addSeries(series);

    QtCharts::QValueAxis* axisX = new QtCharts::QValueAxis();
    QtCharts::QValueAxis* axisY = new QtCharts::QValueAxis();
    axisX->setRange(0, 100);
    axisY->setRange(0, 100);
    axisX->setTitleText("Дни");
    axisY->setTitleText("Проценты");
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);

    chart->createDefaultAxes();
    QtCharts::QChartView* chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->layoutGraphic->addWidget(chartView);
}