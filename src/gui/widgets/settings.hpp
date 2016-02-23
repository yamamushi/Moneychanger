#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "core/WinsockWrapper.h"
#include "core/ExportWrapper.h"
#include "core/TR1_Wrapper.hpp"

#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

signals:
    void expertModeUpdated(bool);
    void hideNavUpdated(bool);

private:
    void showEvent (QShowEvent * event);

private slots:
    void on_pushButtonSave_clicked();
    void on_checkBoxExpertMode_toggled(bool checked);
    void on_comboBoxLanguage_currentIndexChanged(int index);
    void on_checkBoxHideNav_toggled(bool checked);

    void on_pushButton_clicked();

    void on_lineEditNymId_customContextMenuRequested(const QPoint &pos);

    void on_lineEditNotaryId_customContextMenuRequested(const QPoint &pos);

private:
    Ui::Settings *ui;
    QString language;
    QString expertmode_;
    QString hidenav_;
};

#endif // SETTINGS_HPP
