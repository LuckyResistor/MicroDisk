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


#include "ui_mainwindow.h"

#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QRegularExpression>
#include <QtCore/QSettings>
#include <QtGUI/QDesktopServices>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QTreeWidget>


namespace {
const QString cSettingsAddFileDirectory = "addFileDirectory";
const QString cSettingsGenerateFileDirectory = "generateFileDirectory";
const QString cSettingsWindowGeometry = "mainWindow.geometry";
const QString cSettingsWindowState = "mainWindow.state";
const int cSDBlockSize = 512;
const qint64 cSDBlockSizeMask = 0x1FF;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initializeUi();
    loadSettings();
}


MainWindow::~MainWindow()
{
    delete ui;
}


namespace {
// A simple delegate which prevents editing an item.
class NoEditDelegate : public QStyledItemDelegate {
public:
    NoEditDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {
    }
    virtual QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const {
        return nullptr;
    }
};
}


void MainWindow::initializeUi()
{
    setWindowTitle(QString("%1 - V%2").arg(QApplication::applicationDisplayName()).arg(QApplication::applicationVersion()));

    // Seit item delegate to prevent editing the second and last column.
    ui->fileList->setItemDelegateForColumn(1, new NoEditDelegate());
    ui->fileList->setItemDelegateForColumn(2, new NoEditDelegate());

    // Styles
#ifdef Q_OS_MAC
    ui->fileList->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif

    // Connect the buttons with the actions.
    connect(ui->addFileButton, &QPushButton::clicked, this, &MainWindow::on_actionAddFile_triggered);
    connect(ui->removeFileButton, &QPushButton::clicked, this, &MainWindow::on_actionRemoveFile_triggered);
    connect(ui->generateButton, &QPushButton::clicked, this, &MainWindow::on_actionGenerate_triggered);

    // Make sure the remove action/button is correctly set/
    connect(ui->fileList, &QTreeWidget::currentItemChanged, [=]{
        updateButtonStates();
    });

    // Update the initial button states.
    updateButtonStates();
}


void MainWindow::on_actionGenerate_triggered()
{
    QSettings settings;
    auto path = QFileDialog::getSaveFileName(this, tr("Generate disk image"), settings.value(cSettingsGenerateFileDirectory).toString(), tr("Disk Image (*.img)"));
    if (!path.isEmpty()) {
        generateImage(path);
    }
}


void MainWindow::on_actionAddFile_triggered()
{
    QSettings settings;
    auto files = QFileDialog::getOpenFileNames(this, tr("Select files to add"), settings.value(cSettingsAddFileDirectory, QString()).toString());
    if (files.size() > 0) {
        QFileInfo fileInfo(files.first());
        if (fileInfo.exists()) {
            settings.setValue(cSettingsAddFileDirectory, fileInfo.absolutePath());
        }
    }
    for (auto file : files) {
        addFileToList(file);
    }
    updateButtonStates();
}


void MainWindow::on_actionRemoveFile_triggered()
{
    auto item = ui->fileList->currentItem();
    if (item != nullptr) {
        delete item;
    }
    updateButtonStates();
}


void MainWindow::on_actionDocumentation_triggered()
{
    QDesktopServices::openUrl(QUrl("https://luckyresistor.me/applications/micro-disk/"));
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    QMainWindow::closeEvent(e);

    saveSettings();
}


void MainWindow::loadSettings()
{
    QSettings settings;
    restoreGeometry(settings.value(cSettingsWindowGeometry).toByteArray());
    restoreState(settings.value(cSettingsWindowState).toByteArray());
}


void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue(cSettingsWindowGeometry, saveGeometry());
    settings.setValue(cSettingsWindowState, saveState());
}


void MainWindow::updateButtonStates()
{
    ui->removeFileButton->setEnabled(ui->fileList->currentItem() != nullptr);
    ui->actionRemoveFile->setEnabled(ui->fileList->currentItem() != nullptr);
    ui->generateButton->setEnabled(ui->fileList->topLevelItemCount() > 0);
    ui->actionGenerate->setEnabled(ui->fileList->topLevelItemCount() > 0);
}


void MainWindow::addFileToList(const QString &path)
{
    // Get details about the file.
    QFileInfo fileInfo(path);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        QMessageBox::warning(
            this,
            tr("Could not add file"),
            tr("<p>Could not add the file. The path does not exists or the file is not readable.</p><p>Path: %1</p>").arg(path)
        );
        return; // Ignore not existing file.
    }
    const qint64 fileSize = fileInfo.size();
    // Create the entry in the list.
    auto item = new QTreeWidgetItem();
    // Add user data for the generator
    item->setData(0, DataPath, path);
    item->setData(0, DataSize, QVariant::fromValue<qint64>(fileSize));
    // Create a short name from the filename
    QString identifier = fileInfo.baseName();
    identifier.replace(QRegularExpression("[^-a-z0-9_]+", QRegularExpression::CaseInsensitiveOption), "");
    if (identifier.size() > 16) {
        identifier = identifier.left(16);
    }
    if (identifier.isEmpty()) {
        identifier = "unknown";
    }
    // Add the visual details
    item->setText(0, identifier);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    item->setText(1, QString::number(fileSize));
    item->setText(2, path);
    // Add the item to the list.
    ui->fileList->addTopLevelItem(item);
    // Recalculate the column widths.
    ui->fileList->resizeColumnToContents(0);
    ui->fileList->resizeColumnToContents(1);
}


void MainWindow::generateImage(const QString &path)
{
    try {
        QFile imageFile(path);
        if (!imageFile.open(QIODevice::WriteOnly)) {
            throw tr("<p>Could not write the image file</p><p>The error message was: %1</p>").arg(imageFile.errorString());
        }
        QDataStream ds(&imageFile);
        // First calculate everything.
        struct DirectoryEntry {
            QString filePath; // The path of the file.
            quint32 startBlock; // The start block, relative to the directory size.
            quint32 fileSize; // The size of the file.
            quint8 fileNameSize; // The size of the file name.
            QByteArray fileName; // The file name in ascii format.
        };
        QRegularExpression reValidFileName("^[-a-z0-9_]{1,16}$", QRegularExpression::CaseInsensitiveOption);
        QSet<QString> identifiers;
        QList<DirectoryEntry> directoryEntries;
        quint32 currentStartBlock = 0;
        for (int i = 0; i < ui->fileList->topLevelItemCount(); ++i) {
            // Gather all available information and check if everything is in range.
            const auto item = ui->fileList->topLevelItem(i);
            const QString identifier = item->text(0);
            if (identifiers.contains(identifier)) {
                throw tr("The identifier \"%1\" exists multiple times.").arg(identifier);
            }
            const QString filePath = item->data(0, DataPath).toString();
            QFileInfo fileInfo(filePath);
            if (!fileInfo.exists()) {
                throw tr("Could not find file \"%1\" with path \"%2\".").arg(identifier).arg(filePath);
            }
            if (!fileInfo.isReadable()) {
                throw tr("File \"%1\" with path \"%2\" is not readable.").arg(identifier).arg(filePath);
            }
            const qint64 fileSize = fileInfo.size();
            if (fileSize > Q_INT64_C(0xffffffff)) {
                throw tr("The file size of file \"%1\" with path \"%2\" exceeds the maximum size of 4.2GB.").arg(identifier).arg(filePath);
            }
            if (fileSize == 0) {
                throw tr("The file size of file \"%1\" with path \"%2\" is zero. This is not supported.").arg(identifier).arg(filePath);
            }
            if (!reValidFileName.match(identifier).hasMatch()) {
                throw tr("The identifier \"%1\" is not valid. Only characters a-z, 0-9, _ and - are allowed. The length of the identifier must be between 1 and 16 characters.").arg(identifier);
            }
            // Create the directory entry.
            DirectoryEntry entry;
            entry.filePath = filePath;
            entry.startBlock = currentStartBlock;
            entry.fileSize = static_cast<quint32>(fileSize);
            entry.fileName = identifier.toUtf8();
            entry.fileNameSize = static_cast<quint8>(entry.fileName.size());
            identifiers.insert(identifier);
            directoryEntries.append(entry);
            const quint32 numberOfBlocks = static_cast<quint32>(((fileSize-Q_INT64_C(1))/static_cast<qint64>(cSDBlockSize)))+1;
            currentStartBlock += numberOfBlocks;
        }
        // Calculate the directory size.
        quint32 directorySize = (4 + 4); // 4 bytes magic + 4 bytes end mark.
        for (auto entry : directoryEntries) {
            directorySize += (9 + entry.fileNameSize);
        }
        const quint32 firstDataBlock = ((directorySize-1)/512u)+1u;
        // Write the directory.
        if (ds.writeRawData("HCDI", 4) != 4) {
            throw tr("Could not write the magic to the image file.");
        }
        ds.setByteOrder(QDataStream::LittleEndian);
        for (auto entry : directoryEntries) {
            if (ui->nonHcCards->isChecked()) {
                ds << static_cast<quint32>((entry.startBlock + firstDataBlock) * 512);
            } else {
                ds << static_cast<quint32>(entry.startBlock + firstDataBlock);
            }
            ds << static_cast<quint32>(entry.fileSize);
            ds << static_cast<quint8>(entry.fileNameSize);
            if (ds.writeRawData(entry.fileName.constData(), entry.fileNameSize) != entry.fileNameSize) {
                throw tr("There was a problem writing to the image file.");
            }
        }
        // Write the end mark.
        ds << static_cast<quint32>(0);
        // Fill bytes to the next full block size.
        while ((imageFile.pos() & cSDBlockSizeMask) != 0) {
            ds << static_cast<quint8>(0);
        }
        // Now copy all files
        char buffer[cSDBlockSize];
        for (auto entry : directoryEntries) {
            QFile entryFile(entry.filePath);
            if (!entryFile.open(QIODevice::ReadOnly)) {
                throw tr("Could not open the file \"%1\" with path \"%2\". The error message was: %3").arg(QString::fromLatin1(entry.fileName)).arg(entry.filePath).arg(entryFile.errorString());
            }
            while (!entryFile.atEnd()) {
                const qint64 readBytes = entryFile.read(buffer, cSDBlockSize);
                if (readBytes > 0) {
                    qint64 writeIndex = 0;
                    qint64 writtenBytes = 0;
                    while (writeIndex < readBytes) {
                        writtenBytes = imageFile.write(buffer + writeIndex, readBytes - writeIndex);
                        if (writtenBytes < 0) {
                            throw tr("There was a problem writing data to the image.");
                        }
                        writeIndex += writtenBytes;
                    }
                    // Fill bytes to the next full block size.
                    while ((imageFile.pos() & cSDBlockSizeMask) != 0) {
                        ds << static_cast<quint8>(0);
                    }
                }
            }
            entryFile.close();
        }
        imageFile.close();
        // Successfully wrote the image file.
    } catch (const QString &message) {
        QMessageBox m(this);
        m.setIcon(QMessageBox::Critical);
        m.setText(message);
        m.exec();
    }
}


