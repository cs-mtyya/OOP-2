#pragma once

#include <QImage>
#include <QElapsedTimer>
#include <QRect>
#include <QRectF>
#include <QPointF>
#include <QTimer>
#include <QWidget>

class QMouseEvent;
class QWheelEvent;
class QPaintEvent;

class PreviewWidget final : public QWidget {
    Q_OBJECT
public:
    explicit PreviewWidget(QWidget* parent = nullptr);
    void setImages(const QImage& current, const QImage& original);
    void setComparing(bool comparing);
    void setCropSelectionEnabled(bool enabled);
    void fitToWindow();
    double zoom() const { return m_zoom; }

signals:
    void zoomChanged(double zoom);
    void cropSelected(const QRect& imageRect);

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QRectF displayedImageRect() const;
    QPointF toImagePoint(const QPointF& point) const;

    QImage m_current;
    QImage m_original;
    QImage m_previous;
    QElapsedTimer m_transitionClock;
    QTimer m_transitionTimer;
    double m_zoom = 1.0;
    QPointF m_pan;
    QPoint m_lastMouse;
    QPoint m_selectionStart;
    QPoint m_selectionEnd;
    bool m_comparing = false;
    bool m_dragging = false;
    bool m_cropMode = false;
    bool m_selecting = false;
    bool m_movingCrop = false;
    int m_activeCropHandle = -1;
    QPoint m_cropPressPoint;
    QRect m_cropStartRect;
};