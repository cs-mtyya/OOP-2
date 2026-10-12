#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
#include <QTreeWidget>
#include <QVector>
#include <memory>

#include "previewwidget.h"

class Image;
namespace Filters { struct Parameters; }

class QCloseEvent;
class QDragEnterEvent;
class QDropEvent;
class QFormLayout;
class QHBoxLayout;
class QVBoxLayout;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

public slots:
    void openImage(const QString& path = QString());

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void buildInterface();
    void applyTheme();
    void populateFilters();
    void selectFilter(QTreeWidgetItem* item);
    void rebuildControls();
    void refreshLivePreview();
    void applySelectedFilter();
    bool confirmDiscard(const QString& action);
    void saveAs();
    void undo();
    void redo();
    void resetImage();
    void updateImageState(const std::shared_ptr<Image>& image, const QString& action);
    void updateWindowState(const QString& action);
    static Image imageFromQImage(const QImage& image);
    static QImage qImageFromImage(const Image& image);
    static Image downscaled(const Image& image, int maxDimension);
    Filters::Parameters currentParameters() const;

    PreviewWidget* preview_ = nullptr;
    QTreeWidget* filterTree_ = nullptr;
    QTreeWidgetItem* selectedFilterItem_ = nullptr;
    QLabel* selectedFilterBadge_ = nullptr;
    QWidget* controlsHost_ = nullptr;
    QVBoxLayout* controlsLayout_ = nullptr;
    QLabel* filterTitle_ = nullptr;
    QLabel* filterDescription_ = nullptr;
    QLabel* imageInfo_ = nullptr;
    QLabel* zoomInfo_ = nullptr;
    QLabel* actionInfo_ = nullptr;
    QProgressBar* progress_ = nullptr;
    QPushButton* applyButton_ = nullptr;
    QPushButton* compareButton_ = nullptr;
    QComboBox* choice_ = nullptr;
    QSlider* amountSlider_ = nullptr;
    QLabel* amountValue_ = nullptr;
    QSpinBox* widthSpin_ = nullptr;
    QSpinBox* heightSpin_ = nullptr;
    QSpinBox* xSpin_ = nullptr;
    QSpinBox* ySpin_ = nullptr;
    QCheckBox* aspectLock_ = nullptr;
    QPushButton* mergeImageButton_ = nullptr;
    int selectedKind_ = 0;
    bool hasSelection_ = false;
    std::shared_ptr<Image> currentImage_;
    std::shared_ptr<Image> originalImage_;
    std::shared_ptr<Image> mergeImage_;
    QVector<std::shared_ptr<Image>> history_;
    int historyIndex_ = -1;
    QString currentPath_;
    QString lastAction_ = QStringLiteral("Ready");
    QTimer livePreviewTimer_;
    QObject* filterWatcher_ = nullptr;
};