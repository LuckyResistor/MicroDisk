#pragma once
//
// Lucky Resistor Micro Disk
// --------------------------------------------------------------------------
// (c)2016 by Lucky Resistor
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTreeWidgetItem>


namespace Ui {
class MainWindow;
}


/// The main window class
///
/// Contains everything for this simple app.
///
class MainWindow : public QMainWindow
{
    Q_OBJECT

    /// The data roles.
    ///
    enum DataRoles : int {
        DataPath = Qt::UserRole,
        DataSize = Qt::UserRole+1
    };

public:
    /// Create a new main window.
    ///
    explicit MainWindow(QWidget *parent = 0);

    /// dtor
    ///
    virtual ~MainWindow();

public Q_SLOTS:
    /// Generate the final disk image.
    ///
    void on_actionGenerate_triggered();

    /// Add a new file to the list
    ///
    void on_actionAddFile_triggered();

    /// Remove the selected file from the list
    ///
    void on_actionRemoveFile_triggered();

    /// Show the documentation.
    ///
    void on_actionDocumentation_triggered();

protected: // Overwrite events
    virtual void closeEvent(QCloseEvent *e);

private:
    /// Initialize the user interface.
    ///
    void initializeUi();

    /// Load all settings from the preferences.
    ///
    void loadSettings();

    /// Save all settings in the preferences.
    ///
    void saveSettings();

    /// Update the state of the buttons.
    ///
    void updateButtonStates();

    /// Add a file to the list.
    ///
    void addFileToList(const QString &path);

    /// Generate the final image.
    ///
    void generateImage(const QString &path);

private:
    Ui::MainWindow *ui; ///< The user interface.
};
