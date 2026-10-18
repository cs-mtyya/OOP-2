#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QColor>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QImageReader>
#include <QAbstractItemView>
#include <QKeySequence>
#include <QLayout>
#include <QMessageBox>
#include <QMimeData>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>
#include <algorithm>
#include <cstring>
#include <functional>

#include "mainwindow.h"
#include "filters.h"

namespace {
QString filterName(Filters::Kind kind) {
    switch (kind) {
    case Filters::Kind::Grayscale: return QStringLiteral("Grayscale");
    case Filters::Kind::BlackAndWhite: return QStringLiteral("Black and White");
    case Filters::Kind::Invert: return QStringLiteral("Invert");
    case Filters::Kind::Frame: return QStringLiteral("Add Frame");
    case Filters::Kind::Flip: return QStringLiteral("Flip");
    case Filters::Kind::Rotate: return QStringLiteral("Rotate");
    case Filters::Kind::Brightness: return QStringLiteral("Darken / Lighten");
    case Filters::Kind::Resize: return QStringLiteral("Resize");
    case Filters::Kind::Merge: return QStringLiteral("Merge Images");
    case Filters::Kind::Edges: return QStringLiteral("Detect Edges");
    case Filters::Kind::Crop: return QStringLiteral("Crop");
    case Filters::Kind::Blur: return QStringLiteral("Blur");
    case Filters::Kind::Sunlight: return QStringLiteral("Sunlight");
    case Filters::Kind::Television: return QStringLiteral("TV");
    case Filters::Kind::Purple: return QStringLiteral("Purple");
    case Filters::Kind::Infrared: return QStringLiteral("Infrared");
    case Filters::Kind::Skew: return QStringLiteral("Skew");
    case Filters::Kind::OilPainting: return QStringLiteral("Oil Painting");
    }
    return QStringLiteral("Filter");
}

QString filterDescription(Filters::Kind kind) {
    switch (kind) {
    case Filters::Kind::Grayscale: return QStringLiteral("Convert the image to a balanced monochrome look.");
    case Filters::Kind::BlackAndWhite: return QStringLiteral("Create a crisp two-tone image using threshold 127.");
    case Filters::Kind::Invert: return QStringLiteral("Invert every RGB channel for a negative effect.");
    case Filters::Kind::Frame: return QStringLiteral("Add a clean or double-line white border.");
    case Filters::Kind::Flip: return QStringLiteral("Mirror the image horizontally or vertically.");
    case Filters::Kind::Rotate: return QStringLiteral("Rotate the image in 90-degree increments.");
    case Filters::Kind::Brightness: return QStringLiteral("Adjust exposure with a controlled darken or lighten amount.");
    case Filters::Kind::Resize: return QStringLiteral("Resize with nearest-neighbour sampling and optional aspect lock.");
    case Filters::Kind::Merge: return QStringLiteral("Blend with another image, aligned at the top-left.");
    case Filters::Kind::Edges: return QStringLiteral("Find strong threshold transitions as black outlines.");
    case Filters::Kind::Crop: return QStringLiteral("Set a crop rectangle or drag a selection over the preview.");
    case Filters::Kind::Blur: return QStringLiteral("Fast box blur computed with summed-area tables.");
    case Filters::Kind::Sunlight: return QStringLiteral("Warm highlights with a subtle sunlight color cast.");
    case Filters::Kind::Television: return QStringLiteral("Add alternating black scan lines.");
    case Filters::Kind::Purple: return QStringLiteral("Apply a restrained purple-toned channel treatment.");
    case Filters::Kind::Infrared: return QStringLiteral("Create the classic red-channel infrared effect.");
    case Filters::Kind::Skew: return QStringLiteral("Shear the image with a white background.");
    case Filters::Kind::OilPainting: return QStringLiteral("Posterize RGB values into painterly color bands.");
    }
    return {};
}

void clearLayout(QLayout* layout) {
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->hide();
            widget->deleteLater();
        } else if (QLayout* nestedLayout = item->layout()) {
            clearLayout(nestedLayout);
        }
        delete item;
    }
}

}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Pixel Studio"));
    resize(1440, 900);
    setMinimumSize(1100, 700);
    setAcceptDrops(true);
    applyTheme();
    buildInterface();
    populateFilters();
    QSettings settings(QStringLiteral("PixelStudio"), QStringLiteral("PixelStudio"));
    const QString lastFolder = settings.value(QStringLiteral("lastFolder"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    Q_UNUSED(lastFolder);
    livePreviewTimer_.setSingleShot(true);
    livePreviewTimer_.setInterval(100);
    connect(&livePreviewTimer_, &QTimer::timeout, this, &MainWindow::refreshLivePreview);
    connect(preview_, &PreviewWidget::zoomChanged, this, [this](double zoom) { zoomInfo_->setText(QStringLiteral("%1% ").arg(qRound(zoom * 100))); });
    connect(preview_, &PreviewWidget::cropSelected, this, [this](const QRect& rect) {
        if (widthSpin_ && heightSpin_ && currentImage_) {
            const Image previewSource = downscaled(*currentImage_, 1000);
            const double xScale = static_cast<double>(currentImage_->width) / previewSource.width;
            const double yScale = static_cast<double>(currentImage_->height) / previewSource.height;
            xSpin_->setValue(qRound(rect.x() * xScale)); ySpin_->setValue(qRound(rect.y() * yScale));
            widthSpin_->setValue(std::max(1, qRound(rect.width() * xScale)));
            heightSpin_->setValue(std::max(1, qRound(rect.height() * yScale)));
            refreshLivePreview();
        }
    });
    updateWindowState(QStringLiteral("Ready"));
}

MainWindow::~MainWindow() = default;

void MainWindow::applyTheme() {
    qApp->setStyle(QStringLiteral("Fusion"));
    qApp->setStyleSheet(QStringLiteral(R"(
        QWidget { background: #1b1b1b; color: #e5e5e5; font-family: "Segoe UI", sans-serif; font-size: 10pt; }
        QMainWindow { background: #111111; }
        QToolBar { background: #1a1a1a; border: 0; border-bottom: 1px solid #383838; spacing: 8px; padding: 10px 14px; }
        QToolBar::separator { background: #4a4a4a; width: 1px; margin: 5px 4px; }
        QPushButton { background: #303030; border: 1px solid #484848; border-radius: 8px; padding: 9px 13px; color: #e5e5e5; }
        QPushButton:hover { background: #3d3d3d; border-color: #666666; }
        QPushButton:pressed { background: #252525; }
        QPushButton:disabled { color: #777777; background: #242424; border-color: #363636; }
        QPushButton#primaryButton { background: #666666; border-color: #777777; color: white; font-weight: 600; }
        QPushButton#primaryButton:hover { background: #7a7a7a; }
        QPushButton#primaryButton:pressed { background: #505050; }
        QTreeWidget, QTreeView { background: #1a1a1a; border: 1px solid #383838; border-radius: 10px; outline: 0; padding: 6px; }
        QTreeWidget::item { padding: 7px 6px; border-radius: 6px; }
        QTreeWidget::item:hover { background: transparent; }
        QTreeWidget::item:selected { background: transparent; color: #ffffff; }
        QLineEdit, QSpinBox, QComboBox { background: #141414; border: 1px solid #484848; border-radius: 7px; padding: 7px; selection-background-color: #666666; }
        QComboBox::drop-down { border: 0; width: 24px; }
        QSlider::groove:horizontal { height: 5px; background: #454545; border-radius: 2px; }
        QSlider::sub-page:horizontal { background: #999999; border-radius: 2px; }
        QSlider::handle:horizontal { background: #eeeeee; border: 2px solid #777777; width: 15px; margin: -6px 0; border-radius: 8px; }
        QScrollBar:vertical { background: #1a1a1a; width: 10px; margin: 2px; }
        QScrollBar::handle:vertical { background: #555555; border-radius: 5px; min-height: 28px; }
        QScrollBar::handle:vertical:hover { background: #777777; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QFrame#panel { background: #1a1a1a; border: 1px solid #383838; border-radius: 12px; }
        QLabel#muted { color: #a0a0a0; }
        QLabel#heading { font-size: 15pt; font-weight: 650; color: #f0f0f0; }
        QStatusBar { background: #151515; border-top: 1px solid #383838; color: #b8b8b8; }
        QProgressBar { background: #141414; border: 1px solid #484848; border-radius: 5px; height: 9px; text-align: center; }
        QProgressBar::chunk { background: #999999; border-radius: 4px; }
        QCheckBox { spacing: 8px; }
        QCheckBox::indicator { width: 16px; height: 16px; }
    )"));
}

void MainWindow::buildInterface() {
    auto* toolbar = addToolBar(QStringLiteral("Main"));
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(18, 18));
    const auto addTool = [this, toolbar](const QString& text, QStyle::StandardPixmap icon, const std::function<void()>& callback, const QKeySequence& shortcut = QKeySequence()) {
        QAction* action = toolbar->addAction(style()->standardIcon(icon), text);
        action->setShortcut(shortcut);
        connect(action, &QAction::triggered, this, [callback] { callback(); });
        return action;
    };
    addTool(QStringLiteral("Open"), QStyle::SP_DialogOpenButton, [this] { openImage(QString()); }, QKeySequence::Open);
    addTool(QStringLiteral("Save As"), QStyle::SP_DialogSaveButton, [this] { saveAs(); }, QKeySequence::Save);
    toolbar->addSeparator();
    QAction* undoAction = addTool(QStringLiteral("Undo"), QStyle::SP_ArrowBack, [this] { undo(); }, QKeySequence::Undo);
    QAction* redoAction = addTool(QStringLiteral("Redo"), QStyle::SP_ArrowForward, [this] { redo(); }, QKeySequence::Redo);
    Q_UNUSED(undoAction);
    Q_UNUSED(redoAction);
    addTool(QStringLiteral("Reset"), QStyle::SP_BrowserReload, [this] { resetImage(); });
    toolbar->addSeparator();
    compareButton_ = new QPushButton(style()->standardIcon(QStyle::SP_DialogYesButton), QStringLiteral("Hold to compare"), this);
    compareButton_->setCheckable(false);
    toolbar->addWidget(compareButton_);
    connect(compareButton_, &QPushButton::pressed, this, [this] { preview_->setComparing(true); });
    connect(compareButton_, &QPushButton::released, this, [this] { preview_->setComparing(false); });
    auto* fitAction = toolbar->addAction(style()->standardIcon(QStyle::SP_TitleBarNormalButton), QStringLiteral("Fit"));
    connect(fitAction, &QAction::triggered, preview_, &PreviewWidget::fitToWindow);

    auto* split = new QSplitter(Qt::Horizontal, this);
    split->setChildrenCollapsible(false);
    auto* left = new QFrame(split);
    left->setObjectName(QStringLiteral("panel"));
    left->setMinimumWidth(205);
    left->setMaximumWidth(285);
    auto* leftLayout = new QVBoxLayout(left);
    leftLayout->setContentsMargins(14, 16, 14, 14);
    leftLayout->setSpacing(12);
    auto* title = new QLabel(QStringLiteral("WORKSPACE"), left);
    title->setObjectName(QStringLiteral("muted"));
    title->setStyleSheet(QStringLiteral("font-size: 9pt; font-weight: 700; letter-spacing: 1px;"));
    leftLayout->addWidget(title);
    filterTree_ = new QTreeWidget(left);
    filterTree_->setHeaderHidden(true);
    filterTree_->setIndentation(16);
    filterTree_->setSelectionMode(QAbstractItemView::NoSelection);
    filterTree_->setFocusPolicy(Qt::NoFocus);
    leftLayout->addWidget(filterTree_, 1);
    connect(filterTree_, &QTreeWidget::itemClicked, this, &MainWindow::selectFilter);

    auto* center = new QWidget(split);
    auto* centerLayout = new QVBoxLayout(center);
    centerLayout->setContentsMargins(10, 0, 10, 0);
    centerLayout->setSpacing(8);
    preview_ = new PreviewWidget(center);
    centerLayout->addWidget(preview_, 1);

    auto* right = new QFrame(split);
    right->setObjectName(QStringLiteral("panel"));
    right->setMinimumWidth(260);
    right->setMaximumWidth(350);
    auto* rightOuter = new QVBoxLayout(right);
    rightOuter->setContentsMargins(16, 18, 16, 14);
    rightOuter->setSpacing(10);
    filterTitle_ = new QLabel(QStringLiteral("Filter controls"), right);
    filterTitle_->setObjectName(QStringLiteral("heading"));
    filterDescription_ = new QLabel(QStringLiteral("Choose a filter from the workspace list."), right);
    filterDescription_->setObjectName(QStringLiteral("muted"));
    filterDescription_->setWordWrap(true);
    rightOuter->addWidget(filterTitle_);
    rightOuter->addWidget(filterDescription_);
    auto* divider = new QFrame(right);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet(QStringLiteral("color: #383838;"));
    rightOuter->addWidget(divider);
    controlsHost_ = new QWidget(right);
    controlsLayout_ = new QVBoxLayout(controlsHost_);
    controlsLayout_->setContentsMargins(0, 0, 0, 0);
    controlsLayout_->setSpacing(12);
    rightOuter->addWidget(controlsHost_, 1);
    progress_ = new QProgressBar(right);
    progress_->setRange(0, 0);
    progress_->setVisible(false);
    rightOuter->addWidget(progress_);
    applyButton_ = new QPushButton(QStringLiteral("Apply filter"), right);
    applyButton_->setObjectName(QStringLiteral("primaryButton"));
    applyButton_->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    applyButton_->setMinimumHeight(42);
    rightOuter->addWidget(applyButton_);
    connect(applyButton_, &QPushButton::clicked, this, &MainWindow::applySelectedFilter);

    split->addWidget(left);
    split->addWidget(center);
    split->addWidget(right);
    split->setStretchFactor(0, 0);
    split->setStretchFactor(1, 1);
    split->setStretchFactor(2, 0);
    split->setSizes({235, 900, 300});
    setCentralWidget(split);

    auto* status = statusBar();
    imageInfo_ = new QLabel(QStringLiteral("No image"), this);
    zoomInfo_ = new QLabel(QStringLiteral("100%"), this);
    actionInfo_ = new QLabel(QStringLiteral("Ready"), this);
    status->addWidget(imageInfo_, 1);
    status->addPermanentWidget(zoomInfo_);
    status->addPermanentWidget(actionInfo_);
}

void MainWindow::populateFilters() {
    struct Group { const char* name; QVector<Filters::Kind> kinds; };
    const QVector<Group> groups = {
        {"Colour", {Filters::Kind::Grayscale, Filters::Kind::BlackAndWhite, Filters::Kind::Invert, Filters::Kind::Brightness, Filters::Kind::Sunlight, Filters::Kind::Purple, Filters::Kind::Infrared}},
        {"Geometry", {Filters::Kind::Flip, Filters::Kind::Rotate, Filters::Kind::Resize, Filters::Kind::Skew}},
        {"Effects", {Filters::Kind::Frame, Filters::Kind::Edges, Filters::Kind::Blur, Filters::Kind::Television, Filters::Kind::OilPainting}},
        {"Tools", {Filters::Kind::Crop, Filters::Kind::Merge}}
    };
    for (const Group& group : groups) {
        auto* parent = new QTreeWidgetItem(filterTree_, QStringList(QString::fromLatin1(group.name)));
        parent->setFlags(parent->flags() & ~Qt::ItemIsSelectable);
        for (Filters::Kind kind : group.kinds) {
            auto* item = new QTreeWidgetItem(parent, QStringList(filterName(kind)));
            item->setData(0, Qt::UserRole, static_cast<int>(kind));
            item->setSizeHint(0, QSize(0, 34));
            item->setText(0, QString());
            auto* row = new QWidget(filterTree_);
            row->setStyleSheet(QStringLiteral("background: transparent; border-radius: 6px;"));
            auto* rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(7, 0, 7, 0);
            rowLayout->setSpacing(6);
            auto* label = new QLabel(filterName(kind), row);
            label->setStyleSheet(QStringLiteral("background: transparent; border: none;"));
            auto* badge = new QLabel(QStringLiteral("✓"), row);
            badge->setObjectName(QStringLiteral("activeFilterBadge"));
            badge->setAlignment(Qt::AlignCenter);
            badge->setFixedSize(20, 20);
            badge->setStyleSheet(QStringLiteral("QLabel { background: #7c3aed; color: white; border: 1px solid #a78bfa; border-radius: 5px; font-weight: 700; }"));
            badge->hide();
            rowLayout->addWidget(label, 1);
            rowLayout->addWidget(badge, 0, Qt::AlignRight | Qt::AlignVCenter);
            filterTree_->setItemWidget(item, 0, row);
        }
        parent->setExpanded(true);
    }
    filterTree_->expandAll();
}

void MainWindow::selectFilter(QTreeWidgetItem* item) {
    if (!item || !item->data(0, Qt::UserRole).isValid()) return;
    if (selectedFilterItem_ && selectedFilterItem_ != item) {
        if (QWidget* oldRow = filterTree_->itemWidget(selectedFilterItem_, 0))
            oldRow->setStyleSheet(QStringLiteral("background: transparent; border-radius: 6px;"));
        if (selectedFilterBadge_) selectedFilterBadge_->hide();
    }
    selectedFilterItem_ = item;
    if (QWidget* row = filterTree_->itemWidget(item, 0)) {
        row->setStyleSheet(QStringLiteral("background: #38234f; border-radius: 6px;"));
        selectedFilterBadge_ = row->findChild<QLabel*>(QStringLiteral("activeFilterBadge"));
        if (selectedFilterBadge_) selectedFilterBadge_->show();
    }
    selectedKind_ = item->data(0, Qt::UserRole).toInt();
    const auto kind = static_cast<Filters::Kind>(selectedKind_);
    hasSelection_ = true;
    filterTitle_->setText(filterName(kind));
    filterDescription_->setText(filterDescription(kind));
    rebuildControls();
    refreshLivePreview();
}

void MainWindow::rebuildControls() {
    controlsHost_->setUpdatesEnabled(false);
    clearLayout(controlsLayout_);
    choice_ = nullptr; amountSlider_ = nullptr; amountValue_ = nullptr;
    widthSpin_ = nullptr; heightSpin_ = nullptr; xSpin_ = nullptr; ySpin_ = nullptr;
    aspectLock_ = nullptr; mergeImageButton_ = nullptr;
    auto addChoice = [this](const QStringList& options, const QList<int>& values = {}) {
        choice_ = new QComboBox(controlsHost_);
        for (int i = 0; i < options.size(); ++i) choice_->addItem(options[i], values.isEmpty() ? i : values.value(i));
        controlsLayout_->addWidget(choice_);
        connect(choice_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this] { livePreviewTimer_.start(); });
    };
    auto addAmount = [this](const QString& label, int minimum, int maximum, int initial) {
        auto* caption = new QLabel(label, controlsHost_);
        caption->setObjectName(QStringLiteral("muted"));
        auto* row = new QHBoxLayout();
        amountSlider_ = new QSlider(Qt::Horizontal, controlsHost_);
        amountSlider_->setRange(minimum, maximum);
        amountSlider_->setValue(initial);
        amountValue_ = new QLabel(QString::number(initial), controlsHost_);
        amountValue_->setMinimumWidth(34);
        amountValue_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        row->addWidget(amountSlider_, 1);
        row->addWidget(amountValue_);
        controlsLayout_->addWidget(caption);
        controlsLayout_->addLayout(row);
        connect(amountSlider_, &QSlider::valueChanged, this, [this](int value) { amountValue_->setText(QString::number(value)); livePreviewTimer_.start(); });
    };
    auto addDimensions = [this](bool crop) {
        auto* form = new QFormLayout();
        form->setLabelAlignment(Qt::AlignLeft);
        if (crop) {
            xSpin_ = new QSpinBox(controlsHost_); ySpin_ = new QSpinBox(controlsHost_);
            xSpin_->setRange(0, std::max(0, currentImage_ ? currentImage_->width - 1 : 0)); ySpin_->setRange(0, std::max(0, currentImage_ ? currentImage_->height - 1 : 0));
            widthSpin_ = new QSpinBox(controlsHost_); heightSpin_ = new QSpinBox(controlsHost_);
            widthSpin_->setRange(1, std::max(1, currentImage_ ? currentImage_->width : 1)); heightSpin_->setRange(1, std::max(1, currentImage_ ? currentImage_->height : 1));
            widthSpin_->setValue(std::max(1, currentImage_ ? currentImage_->width : 1)); heightSpin_->setValue(std::max(1, currentImage_ ? currentImage_->height : 1));
            form->addRow(QStringLiteral("X"), xSpin_); form->addRow(QStringLiteral("Y"), ySpin_);
            form->addRow(QStringLiteral("Width"), widthSpin_); form->addRow(QStringLiteral("Height"), heightSpin_);
        } else {
            widthSpin_ = new QSpinBox(controlsHost_); heightSpin_ = new QSpinBox(controlsHost_);
            widthSpin_->setRange(1, 20000); heightSpin_->setRange(1, 20000);
            widthSpin_->setValue(std::max(1, currentImage_ ? currentImage_->width : 1)); heightSpin_->setValue(std::max(1, currentImage_ ? currentImage_->height : 1));
            form->addRow(QStringLiteral("Width"), widthSpin_); form->addRow(QStringLiteral("Height"), heightSpin_);
            aspectLock_ = new QCheckBox(QStringLiteral("Lock aspect ratio"), controlsHost_);
            aspectLock_->setChecked(true);
            controlsLayout_->addWidget(aspectLock_);
            connect(aspectLock_, &QCheckBox::toggled, this, [this](bool) { livePreviewTimer_.start(); });
            connect(widthSpin_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
                if (aspectLock_ && aspectLock_->isChecked() && currentImage_ && currentImage_->width > 0) {
                    const QSignalBlocker blocker(heightSpin_);
                    heightSpin_->setValue(std::max(1, qRound(value * static_cast<double>(currentImage_->height) / currentImage_->width)));
                }
                livePreviewTimer_.start();
            });
            connect(heightSpin_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
                if (aspectLock_ && aspectLock_->isChecked() && currentImage_ && currentImage_->height > 0) {
                    const QSignalBlocker blocker(widthSpin_);
                    widthSpin_->setValue(std::max(1, qRound(value * static_cast<double>(currentImage_->width) / currentImage_->height)));
                }
                livePreviewTimer_.start();
            });
        }
        controlsLayout_->addLayout(form);
        for (QSpinBox* spin : {widthSpin_, heightSpin_, xSpin_, ySpin_})
            if (spin) connect(spin, qOverload<int>(&QSpinBox::valueChanged), this, [this] { livePreviewTimer_.start(); });
    };

    switch (static_cast<Filters::Kind>(selectedKind_)) {
    case Filters::Kind::Frame: addChoice({QStringLiteral("Simple"), QStringLiteral("Fancy")}); break;
    case Filters::Kind::Flip: addChoice({QStringLiteral("Horizontal"), QStringLiteral("Vertical")}); break;
    case Filters::Kind::Rotate: addChoice({QStringLiteral("90° clockwise"), QStringLiteral("180°"), QStringLiteral("270° clockwise")}, {90, 180, 270}); break;
    case Filters::Kind::Brightness: addChoice({QStringLiteral("Darken"), QStringLiteral("Lighten")}); addAmount(QStringLiteral("Amount (%)"), 0, 100, 25); break;
    case Filters::Kind::Resize: addDimensions(false); break;
    case Filters::Kind::Merge:
        mergeImageButton_ = new QPushButton(QStringLiteral("Choose second image…"), controlsHost_);
        controlsLayout_->addWidget(mergeImageButton_);
        connect(mergeImageButton_, &QPushButton::clicked, this, [this] {
            const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Choose image to merge"), {}, QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));
            if (path.isEmpty()) return;
            QImage image(path);
            if (image.isNull()) { QMessageBox::warning(this, QStringLiteral("Unable to open image"), QStringLiteral("The selected file could not be decoded.")); return; }
            mergeImage_ = std::make_shared<Image>(imageFromQImage(image.convertToFormat(QImage::Format_RGB888)));
            mergeImageButton_->setText(QFileInfo(path).fileName());
            refreshLivePreview();
        });
        break;
    case Filters::Kind::Crop: addDimensions(true); break;
    case Filters::Kind::Blur: addAmount(QStringLiteral("Blur radius (pixels)"), 1, 60, 8); break;
    case Filters::Kind::Skew: addAmount(QStringLiteral("Skew angle (degrees)"), 0, 50, 15); break;
    default: {
        auto* note = new QLabel(QStringLiteral("No additional parameters required."), controlsHost_);
        note->setObjectName(QStringLiteral("muted"));
        controlsLayout_->addWidget(note);
        break;
    }
    }
    controlsLayout_->addStretch(1);
    controlsHost_->setUpdatesEnabled(true);
    controlsHost_->update();
    applyButton_->setEnabled(currentImage_ && currentImage_->width > 0 && !filterWatcher_);
}

Filters::Parameters MainWindow::currentParameters() const {
    Filters::Parameters parameters;
    if (amountSlider_) parameters.amount = amountSlider_->value();
    if (choice_) parameters.choice = choice_->currentData().toInt();
    if (widthSpin_) parameters.width = widthSpin_->value();
    if (heightSpin_) parameters.height = heightSpin_->value();
    if (xSpin_) parameters.x = xSpin_->value();
    if (ySpin_) parameters.y = ySpin_->value();
    if (aspectLock_) parameters.lockAspect = aspectLock_->isChecked();
    return parameters;
}

void MainWindow::refreshLivePreview() {
    if (!currentImage_ || currentImage_->width <= 0 || !hasSelection_) return;
    try {
        const Image source = downscaled(*currentImage_, 1000);
        const auto kind = static_cast<Filters::Kind>(selectedKind_);
        if (kind == Filters::Kind::Crop) {
            preview_->setImages(qImageFromImage(source), qImageFromImage(downscaled(*originalImage_, 1000)));
            preview_->setCropSelectionEnabled(true);
            return;
        }
        Image secondary;
        const Image* secondaryPtr = nullptr;
        if (kind == Filters::Kind::Merge && mergeImage_ && mergeImage_->width > 0) {
            secondary = downscaled(*mergeImage_, 1000);
            secondaryPtr = &secondary;
        }
        Filters::Parameters previewParameters = currentParameters();
        if (kind == Filters::Kind::Resize) {
            double scaleX = static_cast<double>(source.width) / currentImage_->width;
            double scaleY = static_cast<double>(source.height) / currentImage_->height;
            int previewWidth = std::max(1, qRound(previewParameters.width * scaleX));
            int previewHeight = std::max(1, qRound(previewParameters.height * scaleY));
            const double outputScale = std::min(1.0, 1000.0 / std::max(previewWidth, previewHeight));
            previewParameters.width = std::max(1, qRound(previewWidth * outputScale));
            previewParameters.height = std::max(1, qRound(previewHeight * outputScale));
        }
        const Image output = Filters::apply(source, kind, previewParameters, secondaryPtr);
        preview_->setImages(qImageFromImage(output), qImageFromImage(downscaled(*originalImage_, 1000)));
        preview_->setCropSelectionEnabled(kind == Filters::Kind::Crop);
    } catch (const std::exception&) {
        preview_->setImages(qImageFromImage(downscaled(*currentImage_, 1000)), qImageFromImage(downscaled(*originalImage_, 1000)));
    }
}

void MainWindow::applySelectedFilter() {
    if (filterWatcher_ || !currentImage_ || currentImage_->width <= 0 || !hasSelection_) return;
    const Filters::Parameters parameters = currentParameters();
    const Filters::Kind kind = static_cast<Filters::Kind>(selectedKind_);
    const std::shared_ptr<Image> source = currentImage_;
    const std::shared_ptr<Image> secondary = mergeImage_;
    const bool usesSecondary = kind == Filters::Kind::Merge;
    auto* watcher = new QFutureWatcher<std::shared_ptr<Image>>(this);
    filterWatcher_ = watcher;
    progress_->setVisible(true);
    applyButton_->setEnabled(false);
    actionInfo_->setText(QStringLiteral("Applying %1…").arg(filterName(kind)));
    connect(watcher, &QFutureWatcher<std::shared_ptr<Image>>::finished, this, [this, watcher, kind] {
        try {
            const std::shared_ptr<Image> result = watcher->result();
            updateImageState(result, QStringLiteral("Applied %1").arg(filterName(kind)));
            preview_->setCropSelectionEnabled(false);
            preview_->setImages(qImageFromImage(downscaled(*currentImage_, 1600)),
                                qImageFromImage(downscaled(*originalImage_, 1600)));
            preview_->setCropSelectionEnabled(kind == Filters::Kind::Crop);
        } catch (const std::exception& error) {
            QMessageBox::critical(this, QStringLiteral("Filter failed"), QString::fromLocal8Bit(error.what()));
            updateWindowState(QStringLiteral("Filter failed"));
            refreshLivePreview();
        }
        watcher->deleteLater();
        filterWatcher_ = nullptr;
        progress_->setVisible(false);
        applyButton_->setEnabled(currentImage_ && currentImage_->width > 0);
    });
    watcher->setFuture(QtConcurrent::run([source, secondary, usesSecondary, kind, parameters]() {
        return std::make_shared<Image>(Filters::apply(*source, kind, parameters, usesSecondary && secondary ? secondary.get() : nullptr));
    }));
}

Image MainWindow::imageFromQImage(const QImage& source) {
    const QImage image = source.convertToFormat(QImage::Format_RGB888);
    if (image.isNull()) return {};
    Image result(image.width(), image.height());
    for (int y = 0; y < image.height(); ++y)
        std::memcpy(result.imageData + static_cast<std::size_t>(y) * image.width() * 3, image.constScanLine(y), static_cast<std::size_t>(image.width()) * 3);
    return result;
}

QImage MainWindow::qImageFromImage(const Image& image) {
    if (image.width <= 0 || image.height <= 0 || !image.imageData) return {};
    QImage result(image.width, image.height, QImage::Format_RGB888);
    for (int y = 0; y < image.height; ++y)
        std::memcpy(result.scanLine(y), image.imageData + static_cast<std::size_t>(y) * image.width * 3, static_cast<std::size_t>(image.width) * 3);
    return result;
}

Image MainWindow::downscaled(const Image& image, int maxDimension) {
    if (image.width <= maxDimension && image.height <= maxDimension) return Image(image);
    const double scale = static_cast<double>(maxDimension) / std::max(image.width, image.height);
    return Filters::resize(image, std::max(1, qRound(image.width * scale)), std::max(1, qRound(image.height * scale)));
}

bool MainWindow::confirmDiscard(const QString& action) {
    if (historyIndex_ <= 0 || history_.isEmpty()) return true;
    const auto answer = QMessageBox::question(this, QStringLiteral("Unsaved changes"),
        QStringLiteral("Your edits have not been saved. Continue %1 and discard them?").arg(action),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
    if (answer == QMessageBox::Cancel) return false;
    if (answer == QMessageBox::Save) { saveAs(); return historyIndex_ <= 0; }
    return true;
}

void MainWindow::openImage(const QString& requestedPath) {
    if (filterWatcher_) return;
    if (!confirmDiscard(QStringLiteral("opening another image"))) return;
    QString path = requestedPath;
    if (path.isEmpty()) {
        QSettings settings(QStringLiteral("PixelStudio"), QStringLiteral("PixelStudio"));
        const QString folder = settings.value(QStringLiteral("lastFolder"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
        path = QFileDialog::getOpenFileName(this, QStringLiteral("Open image"), folder,
            QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp *.gif);;All files (*.*)"));
    }
    if (path.isEmpty()) return;
    QImageReader reader(path);
    reader.setAutoTransform(true);
    const QSize sourceSize = reader.size();
    if (sourceSize.isValid() && (sourceSize.width() > Filters::MaximumImageDimension ||
        sourceSize.height() > Filters::MaximumImageDimension ||
        static_cast<int64_t>(sourceSize.width()) * sourceSize.height() > Filters::MaximumImagePixels)) {
        QMessageBox::warning(this, QStringLiteral("Image is too large"),
                             QStringLiteral("Pixel Studio supports images up to 60 megapixels to keep editing stable."));
        return;
    }
    const QImage loaded = reader.read();
    if (loaded.isNull()) {
        QMessageBox::critical(this, QStringLiteral("Unable to open image"), reader.errorString());
        return;
    }
    if (loaded.width() > Filters::MaximumImageDimension || loaded.height() > Filters::MaximumImageDimension ||
        static_cast<int64_t>(loaded.width()) * loaded.height() > Filters::MaximumImagePixels) {
        QMessageBox::warning(this, QStringLiteral("Image is too large"),
                             QStringLiteral("Pixel Studio supports images up to 60 megapixels to keep editing stable."));
        return;
    }
    currentImage_ = std::make_shared<Image>(imageFromQImage(loaded));
    originalImage_ = currentImage_;
    history_.clear();
    history_.push_back(currentImage_);
    historyIndex_ = 0;
    currentPath_ = path;
    mergeImage_ = std::make_shared<Image>();
    hasSelection_ = false;
    preview_->setCropSelectionEnabled(false);
    QSettings settings(QStringLiteral("PixelStudio"), QStringLiteral("PixelStudio"));
    settings.setValue(QStringLiteral("lastFolder"), QFileInfo(path).absolutePath());
    updateWindowState(QStringLiteral("Opened %1").arg(QFileInfo(path).fileName()));
    preview_->setImages(qImageFromImage(*currentImage_), qImageFromImage(*originalImage_));
    preview_->fitToWindow();
    if (hasSelection_) refreshLivePreview();
    rebuildControls();
}

void MainWindow::saveAs() {
    if (filterWatcher_ || !currentImage_ || currentImage_->width <= 0) return;
    QSettings settings(QStringLiteral("PixelStudio"), QStringLiteral("PixelStudio"));
    const QString folder = settings.value(QStringLiteral("lastFolder"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    QString path = QFileDialog::getSaveFileName(this, QStringLiteral("Save image as"), folder,
        QStringLiteral("PNG image (*.png);;JPEG image (*.jpg *.jpeg);;Bitmap (*.bmp)"));
    if (path.isEmpty()) return;
    if (QFileInfo(path).suffix().isEmpty()) path += QStringLiteral(".png");
    if (!qImageFromImage(*currentImage_).save(path)) {
        QMessageBox::critical(this, QStringLiteral("Unable to save image"), QStringLiteral("The image could not be saved to the selected location."));
        return;
    }
    currentPath_ = path;
    settings.setValue(QStringLiteral("lastFolder"), QFileInfo(path).absolutePath());
    history_.clear();
    history_.push_back(currentImage_);
    historyIndex_ = 0;
    updateWindowState(QStringLiteral("Saved %1").arg(QFileInfo(path).fileName()));
}

void MainWindow::updateImageState(const std::shared_ptr<Image>& image, const QString& action) {
    currentImage_ = image;
    while (history_.size() > historyIndex_ + 1) history_.removeLast();
    history_.push_back(image);
    if (history_.size() > 21) history_.removeFirst();
    historyIndex_ = history_.size() - 1;
    updateWindowState(action);
}

void MainWindow::undo() {
    if (historyIndex_ <= 0 || filterWatcher_) return;
    --historyIndex_;
    currentImage_ = history_[historyIndex_];
    updateWindowState(QStringLiteral("Undo"));
    refreshLivePreview();
}

void MainWindow::redo() {
    if (historyIndex_ < 0 || historyIndex_ + 1 >= history_.size() || filterWatcher_) return;
    ++historyIndex_;
    currentImage_ = history_[historyIndex_];
    updateWindowState(QStringLiteral("Redo"));
    refreshLivePreview();
}

void MainWindow::resetImage() {
    if (!currentImage_ || currentImage_->width <= 0 || filterWatcher_) return;
    currentImage_ = originalImage_;
    history_.clear();
    history_.push_back(currentImage_);
    historyIndex_ = 0;
    updateWindowState(QStringLiteral("Reset to original"));
    refreshLivePreview();
}

void MainWindow::updateWindowState(const QString& action) {
    lastAction_ = action;
    actionInfo_->setText(action);
    if (currentImage_ && currentImage_->width > 0) {
        imageInfo_->setText(QStringLiteral("%1 × %2 px    •    %3").arg(currentImage_->width).arg(currentImage_->height).arg(QFileInfo(currentPath_).fileName().isEmpty() ? QStringLiteral("Untitled") : QFileInfo(currentPath_).fileName()));
        setWindowTitle(QStringLiteral("%1 (%2 × %3)%4 — Pixel Studio")
            .arg(QFileInfo(currentPath_).fileName().isEmpty() ? QStringLiteral("Untitled") : QFileInfo(currentPath_).fileName())
            .arg(currentImage_->width).arg(currentImage_->height)
            .arg(historyIndex_ > 0 ? QStringLiteral(" *") : QString()));
    } else {
        imageInfo_->setText(QStringLiteral("No image"));
        setWindowTitle(QStringLiteral("Pixel Studio"));
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (filterWatcher_) { event->ignore(); return; }
    if (confirmDiscard(QStringLiteral("closing the application"))) event->accept();
    else event->ignore();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty() && urls.first().isLocalFile()) openImage(urls.first().toLocalFile());
}