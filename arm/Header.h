#pragma once
#ifndef ADDPERSONDIALOG_H
#define ADDPERSONDIALOG_H

#include <QDialog>

#include "mainwindow.h"


namespace Ui {
    class addPersonDialog;
}

class addPersonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addPersonDialog(QWidget* parent = nullptr, CustomSqlTableModel* model = nullptr);
    ~addPersonDialog();
    void FillComboBox(QDate date);
private slots:
    void on_calendarWidget_selectionChanged();

    void on_saveChangeButton_clicked();

    void on_unsaveChangeButton_clicked();

private:
    Ui::addPersonDialog* ui;
    friend class MainWindow;

    QStringList getFreeTimeList(QDate date);

    QStringList list_time = { "09:00", "10:00","11:00", "12:00", "14:00", "15:00", "16:00", "17:00" };
    CustomSqlTableModel* model;

};







#endif // ADDPERSONDIALOG_H