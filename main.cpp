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


#include "mainwindow.h"
#include "config.h"

#include <QtWidgets/QApplication>


/// The main method.
///
int main(int argc, char *argv[])
{
    // Create and initialize the application object.
    QApplication app(argc, argv);
    app.setApplicationDisplayName(APPLICATION_NAME);
    app.setApplicationName(APPLICATION_ID);
    app.setApplicationVersion(APPLICATION_VERSION);
    app.setOrganizationDomain(APPLICATION_COMPANY_DOMAIN);
    app.setOrganizationName(APPLICATION_COMPANY_NAME);

    // Create and show the main window.
    auto mainWindow = new MainWindow();
    mainWindow->show();

    // Start the main loop.
    auto result = app.exec();

    // Delete the main window.
    delete mainWindow;

    // Return the result.
    return result;
}
