#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QLinearGradient>
#include <QPainterPath>
#include <algorithm>
#include <cmath>

#include "previewwidget.h"

PreviewWidget::PreviewWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(360, 300);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    m_transitionTimer.setInterval(16);
    connect(&m_transitionTimer, &QTimer::timeout, this, [this] {
        if (m_transitionClock.elapsed() >= 240) {
            m_transitionTimer.stop();
            m_previous = QImage();
        }
        update();
    });
}

void PreviewWidget::setImages(const QImage& current, const QImage& original) {
    if (current.isNull()) return;
    const bool needsInitialFit = m_current.isNull() || m_current.size() != current.size();
    const bool changed = !m_current.isNull() && m_current.cacheKey() != current.cacheKey();
    if (changed && !m_comparing) {
        m_previous = m_current;
        m_transitionClock.restart();
        m_transitionTimer.start();
    }
    m_current = current;
    m_original = original;
    if (needsInitialFit) fitToWindow();
    else update();
}

void PreviewWidget::setComparing(bool comparing) {
    m_comparing = comparing;
    update();
}

void PreviewWidget::setCropSelectionEnabled(bool enabled) {
    if (m_cropMode == enabled) return;
    m_cropMode = enabled;
    m_selecting = false;
    m_movingCrop = false;
    m_activeCropHandle = -1;
    if (enabled && !m_current.isNull()) {
        const QRectF imageRect = displayedImageRect();
        const QPointF inset(imageRect.width() * 0.1, imageRect.height() * 0.1);
        m_selectionStart = (imageRect.topLeft() + inset).toPoint();
        m_selectionEnd = (imageRect.bottomRight() - inset).toPoint();
    } else {
        m_selectionStart = {};
        m_selectionEnd = {};
    }
    setCursor(enabled ? Qt::CrossCursor : Qt::OpenHandCursor);
    update();
}

void PreviewWidget::fitToWindow() {
    if (m_current.isNull()) return;
    const QSize available = size() - QSize(48, 48);
    if (available.width() <= 0 || available.height() <= 0) return;
    m_zoom = std::min(static_cast<double>(available.width()) / m_current.width(),
                      static_cast<double>(available.height()) / m_current.height());
    m_zoom = std::clamp(m_zoom, 0.02, 8.0);
    m_pan = {};
    emit zoomChanged(m_zoom);
    update();
}

QRectF PreviewWidget::displayedImageRect() const {
    if (m_current.isNull()) return {};
    const QSizeF imageSize(m_current.width() * m_zoom, m_current.height() * m_zoom);
    const QPointF center(width() / 2.0 + m_pan.x(), height() / 2.0 + m_pan.y());
    return QRectF(center.x() - imageSize.width() / 2.0, center.y() - imageSize.height() / 2.0,
                  imageSize.width(), imageSize.height());
}

QPointF PreviewWidget::toImagePoint(const QPointF& point) const {
    const QRectF target = displayedImageRect();
    if (target.isEmpty() || m_zoom <= 0) return {};
    return QPointF((point.x() - target.left()) / m_zoom, (point.y() - target.top()) / m_zoom);
}

void PreviewWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QLinearGradient background(0, 0, width(), height());
    background.setColorAt(0, QColor("#202020"));
    background.setColorAt(1, QColor("#141414"));
    painter.fillRect(rect(), background);
    painter.setPen(QColor("#343434"));
    for (int x = 0; x < width(); x += 24)
        for (int y = 0; y < height(); y += 24) painter.drawPoint(x, y);

    if (m_current.isNull()) {
        painter.setPen(QColor("#aaaaaa"));
        painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("Drop an image here\n or choose Open to get started"));
        return;
    }

    const QRectF target = displayedImageRect();
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(QPen(QColor("#666666"), 1));
    painter.setBrush(QColor("#242424"));
    painter.drawRoundedRect(target.adjusted(-1, -1, 1, 1), 3, 3);
    if (m_comparing && !m_original.isNull()) {
        const QPointF center = target.center();
        const QRectF originalTarget(center.x() - m_original.width() * m_zoom / 2.0,
                                    center.y() - m_original.height() * m_zoom / 2.0,
                                    m_original.width() * m_zoom, m_original.height() * m_zoom);
        painter.drawImage(originalTarget, m_original);
    } else if (!m_previous.isNull() && m_transitionClock.isValid() && m_transitionClock.elapsed() < 240) {
        const qreal progress = std::clamp(m_transitionClock.elapsed() / 240.0, 0.0, 1.0);
        painter.setOpacity(1.0 - progress);
        painter.drawImage(target, m_previous);
        painter.setOpacity(progress);
        painter.drawImage(target, m_current);
        painter.setOpacity(1.0);
    } else {
        painter.drawImage(target, m_current);
    }

    if (m_cropMode) {
        const QRect selection = QRect(m_selectionStart, m_selectionEnd).normalized().intersected(target.toAlignedRect());
        if (!selection.isEmpty()) {
            QPainterPath shadePath;
            shadePath.setFillRule(Qt::OddEvenFill);
            shadePath.addRect(target);
            shadePath.addRect(selection.adjusted(1, 1, -1, -1));
            painter.fillPath(shadePath, QColor(0, 0, 0, 125));

            painter.setPen(QPen(QColor(245, 245, 245, 220), 1));
            for (int division = 1; division < 3; ++division) {
                const qreal x = selection.left() + selection.width() * division / 3.0;
                const qreal y = selection.top() + selection.height() * division / 3.0;
                painter.drawLine(QPointF(x, selection.top()), QPointF(x, selection.bottom()));
                painter.drawLine(QPointF(selection.left(), y), QPointF(selection.right(), y));
            }

            painter.setPen(QPen(QColor("#ffffff"), 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(selection);
            painter.setPen(QPen(QColor("#4a4a4a"), 1));
            painter.setBrush(QColor("#f4f4f4"));
            constexpr int handleSize = 8;
            const qreal middleX = selection.center().x();
            const qreal middleY = selection.center().y();
            const QPointF handles[] = {
                selection.topLeft(), QPointF(middleX, selection.top()), selection.topRight(),
                QPointF(selection.right(), middleY), selection.bottomRight(), QPointF(middleX, selection.bottom()),
                selection.bottomLeft(), QPointF(selection.left(), middleY)
            };
            for (const QPointF& handle : handles)
                painter.drawRect(QRectF(handle.x() - handleSize / 2.0, handle.y() - handleSize / 2.0, handleSize, handleSize));
        }
    }
}

void PreviewWidget::wheelEvent(QWheelEvent* event) {
    if (m_current.isNull()) return;
    const double previous = m_zoom;
    const double factor = event->angleDelta().y() > 0 ? 1.12 : 1.0 / 1.12;
    m_zoom = std::clamp(m_zoom * factor, 0.02, 16.0);
    const QPointF cursor = event->position();
    const QPointF relative = cursor - QPointF(width() / 2.0 + m_pan.x(), height() / 2.0 + m_pan.y());
    const double scale = m_zoom / previous;
    m_pan -= relative * (scale - 1.0);
    emit zoomChanged(m_zoom);
    update();
}

void PreviewWidget::mousePressEvent(QMouseEvent* event) {
    if (m_cropMode && event->button() == Qt::LeftButton && !m_current.isNull()) {
        const QPoint point = event->position().toPoint();
        const QRect selection = QRect(m_selectionStart, m_selectionEnd).normalized();
        const qreal middleX = selection.center().x();
        const qreal middleY = selection.center().y();
        const QPointF handles[] = {
            selection.topLeft(), QPointF(middleX, selection.top()), selection.topRight(),
            QPointF(selection.right(), middleY), selection.bottomRight(), QPointF(middleX, selection.bottom()),
            selection.bottomLeft(), QPointF(selection.left(), middleY)
        };
        m_activeCropHandle = -1;
        for (int i = 0; i < 8; ++i) {
            if (std::abs(point.x() - handles[i].x()) <= 10 && std::abs(point.y() - handles[i].y()) <= 10) {
                m_activeCropHandle = i;
                break;
            }
        }
        m_cropPressPoint = point;
        m_cropStartRect = selection;
        m_movingCrop = false;
        if (m_activeCropHandle >= 0) {
            m_selecting = false;
        } else if (selection.contains(point)) {
            m_movingCrop = true;
            m_selecting = false;
        } else {
            m_selectionStart = point;
            m_selectionEnd = point;
            m_selecting = true;
        }
        update();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_lastMouse = event->position().toPoint();
        setCursor(Qt::ClosedHandCursor);
    }
}

void PreviewWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_cropMode && (m_movingCrop || m_activeCropHandle >= 0)) {
        const QRectF imageRect = displayedImageRect();
        QRectF rect(m_cropStartRect);
        const QPointF delta = event->position() - QPointF(m_cropPressPoint);
        if (m_movingCrop) {
            rect.translate(delta);
            if (rect.left() < imageRect.left()) rect.moveLeft(imageRect.left());
            if (rect.top() < imageRect.top()) rect.moveTop(imageRect.top());
            if (rect.right() > imageRect.right()) rect.moveRight(imageRect.right());
            if (rect.bottom() > imageRect.bottom()) rect.moveBottom(imageRect.bottom());
        } else {
            const qreal minimumWidth = std::min(24.0, imageRect.width());
            const qreal minimumHeight = std::min(24.0, imageRect.height());
            const qreal x = std::clamp(event->position().x(), imageRect.left(), imageRect.right());
            const qreal y = std::clamp(event->position().y(), imageRect.top(), imageRect.bottom());
            switch (m_activeCropHandle) {
            case 0: rect.setLeft(std::min(x, rect.right() - minimumWidth)); rect.setTop(std::min(y, rect.bottom() - minimumHeight)); break;
            case 1: rect.setTop(std::min(y, rect.bottom() - minimumHeight)); break;
            case 2: rect.setRight(std::max(x, rect.left() + minimumWidth)); rect.setTop(std::min(y, rect.bottom() - minimumHeight)); break;
            case 3: rect.setRight(std::max(x, rect.left() + minimumWidth)); break;
            case 4: rect.setRight(std::max(x, rect.left() + minimumWidth)); rect.setBottom(std::max(y, rect.top() + minimumHeight)); break;
            case 5: rect.setBottom(std::max(y, rect.top() + minimumHeight)); break;
            case 6: rect.setLeft(std::min(x, rect.right() - minimumWidth)); rect.setBottom(std::max(y, rect.top() + minimumHeight)); break;
            case 7: rect.setLeft(std::min(x, rect.right() - minimumWidth)); break;
            }
            rect.setLeft(std::max(rect.left(), imageRect.left()));
            rect.setTop(std::max(rect.top(), imageRect.top()));
            rect.setRight(std::min(rect.right(), imageRect.right()));
            rect.setBottom(std::min(rect.bottom(), imageRect.bottom()));
        }
        m_selectionStart = rect.topLeft().toPoint();
        m_selectionEnd = rect.bottomRight().toPoint();
        update();
    } else if (m_selecting) {
        m_selectionEnd = event->position().toPoint();
        update();
    } else if (m_dragging) {
        const QPoint current = event->position().toPoint();
        m_pan += current - m_lastMouse;
        m_lastMouse = current;
        update();
    }
}

void PreviewWidget::mouseReleaseEvent(QMouseEvent* event) {
    if ((m_selecting || m_movingCrop || m_activeCropHandle >= 0) && event->button() == Qt::LeftButton) {
        if (m_selecting) m_selectionEnd = event->position().toPoint();
        m_selecting = false;
        m_movingCrop = false;
        m_activeCropHandle = -1;
        const QRectF target = displayedImageRect();
        const QPointF a = toImagePoint(m_selectionStart);
        const QPointF b = toImagePoint(m_selectionEnd);
        const int x = std::clamp(static_cast<int>(std::floor(std::min(a.x(), b.x()))), 0, m_current.width() - 1);
        const int y = std::clamp(static_cast<int>(std::floor(std::min(a.y(), b.y()))), 0, m_current.height() - 1);
        const int right = std::clamp(static_cast<int>(std::ceil(std::max(a.x(), b.x()))), x + 1, m_current.width());
        const int bottom = std::clamp(static_cast<int>(std::ceil(std::max(a.y(), b.y()))), y + 1, m_current.height());
        if (target.contains(m_selectionStart) || target.contains(m_selectionEnd)) emit cropSelected(QRect(x, y, right - x, bottom - y));
        update();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        setCursor(m_cropMode ? Qt::CrossCursor : Qt::OpenHandCursor);
    }
}