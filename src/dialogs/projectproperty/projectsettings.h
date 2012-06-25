#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <QDialog>

namespace Ui {
    class ProjectSettings;
}

class ProjectSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectSettings(QWidget *parent = 0);
    ~ProjectSettings();

private:
    Ui::ProjectSettings *ui;
};

#endif // PROJECTSETTINGS_H
