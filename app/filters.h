#pragma once

#include "Image_Class.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace Filters {

constexpr int64_t MaximumImagePixels = 60000000;
constexpr int MaximumImageDimension = 30000;

inline void validateDimensions(int width, int height) {
    if (width <= 0 || height <= 0 || width > MaximumImageDimension || height > MaximumImageDimension ||
        static_cast<int64_t>(width) * height > MaximumImagePixels)
        throw std::invalid_argument("The requested image dimensions are too large. Maximum output is 60 megapixels and 30,000 pixels per side.");
}

enum class Kind {
    Grayscale, BlackAndWhite, Invert, Frame, Flip, Rotate, Brightness, Resize,
    Merge, Edges, Crop, Blur, Sunlight, Television, Purple, Infrared, Skew,
    OilPainting
};

struct Parameters {
    int amount = 25;
    int secondAmount = 0;
    int width = 0;
    int height = 0;
    int x = 0;
    int y = 0;
    int choice = 0;
    bool lockAspect = false;
};

inline unsigned char clampByte(int value) {
    return static_cast<unsigned char>(std::clamp(value, 0, 255));
}

inline Image grayscale(const Image& source) {
    Image result(source);
    for (int y = 0; y < source.height; ++y) {
        for (int x = 0; x < source.width; ++x) {
            const int gray = (source(x, y, 0) + source(x, y, 1) + source(x, y, 2)) / 3;
            for (int c = 0; c < 3; ++c) result(x, y, c) = static_cast<unsigned char>(gray);
        }
    }
    return result;
}

inline Image blackAndWhite(const Image& source) {
    Image result(source);
    for (int y = 0; y < source.height; ++y) {
        for (int x = 0; x < source.width; ++x) {
            const int gray = (source(x, y, 0) + source(x, y, 1) + source(x, y, 2)) / 3;
            const unsigned char value = gray < 127 ? 0 : 255;
            for (int c = 0; c < 3; ++c) result(x, y, c) = value;
        }
    }
    return result;
}

inline Image invert(const Image& source) {
    Image result(source);
    const std::size_t count = static_cast<std::size_t>(source.width) * source.height * 3;
    for (std::size_t i = 0; i < count; ++i) result.imageData[i] = 255 - source.imageData[i];
    return result;
}

inline Image frame(const Image& source, bool fancy) {
    Image result(source);
    for (int y = 0; y < source.height; ++y) {
        for (int x = 0; x < source.width; ++x) {
            const int edge = std::min({x, y, source.width - 1 - x, source.height - 1 - y});
            if (edge < 10 || (fancy && edge >= 15 && edge < 20)) {
                for (int c = 0; c < 3; ++c) result(x, y, c) = 255;
            }
        }
    }
    return result;
}

inline Image flip(const Image& source, bool horizontal) {
    Image result(source.width, source.height);
    for (int y = 0; y < source.height; ++y) {
        for (int x = 0; x < source.width; ++x) {
            const int sx = horizontal ? source.width - 1 - x : x;
            const int sy = horizontal ? y : source.height - 1 - y;
            for (int c = 0; c < 3; ++c) result(x, y, c) = source(sx, sy, c);
        }
    }
    return result;
}

inline Image rotate(const Image& source, int degrees) {
    if (degrees == 180) {
        Image result(source.width, source.height);
        for (int y = 0; y < source.height; ++y)
            for (int x = 0; x < source.width; ++x)
                for (int c = 0; c < 3; ++c) result(source.width - 1 - x, source.height - 1 - y, c) = source(x, y, c);
        return result;
    }
    if (degrees != 90 && degrees != 270) throw std::invalid_argument("Rotation must be 90, 180, or 270 degrees.");
    Image result(source.height, source.width);
    for (int y = 0; y < source.height; ++y) {
        for (int x = 0; x < source.width; ++x) {
            const int dx = degrees == 90 ? source.height - 1 - y : y;
            const int dy = degrees == 90 ? x : source.width - 1 - x;
            for (int c = 0; c < 3; ++c) result(dx, dy, c) = source(x, y, c);
        }
    }
    return result;
}

inline Image brightness(const Image& source, int percent, bool lighten) {
    Image result(source);
    const double factor = lighten ? 1.0 + std::clamp(percent, 0, 100) / 100.0
                                  : 1.0 - (std::clamp(percent, 0, 100) / 100.0) * 0.8;
    const std::size_t count = static_cast<std::size_t>(source.width) * source.height * 3;
    for (std::size_t i = 0; i < count; ++i)
        result.imageData[i] = clampByte(static_cast<int>(source.imageData[i] * factor));
    return result;
}

inline Image resize(const Image& source, int width, int height) {
    width = std::clamp(width, 1, 20000);
    height = std::clamp(height, 1, 20000);
    validateDimensions(width, height);
    Image result(width, height);
    for (int y = 0; y < height; ++y) {
        const int sy = std::min(source.height - 1, static_cast<int>((static_cast<int64_t>(y) * source.height) / height));
        for (int x = 0; x < width; ++x) {
            const int sx = std::min(source.width - 1, static_cast<int>((static_cast<int64_t>(x) * source.width) / width));
            for (int c = 0; c < 3; ++c) result(x, y, c) = source(sx, sy, c);
        }
    }
    return result;
}

inline Image merge(const Image& first, const Image& second) {
    const int width = std::min(first.width, second.width);
    const int height = std::min(first.height, second.height);
    Image result(width, height);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            for (int c = 0; c < 3; ++c)
                result(x, y, c) = static_cast<unsigned char>((first(x, y, c) + second(x, y, c)) / 2);
    return result;
}

inline Image edges(const Image& source) {
    Image result(source.width, source.height);
    for (int y = 0; y < source.height; ++y) {
        for (int x = 0; x < source.width; ++x) {
            const auto grayAt = [&source](int px, int py) {
                return (source(px, py, 0) + source(px, py, 1) + source(px, py, 2)) / 3 < 127 ? 0 : 255;
            };
            const int current = grayAt(x, y);
            const bool edge = (x + 1 < source.width && current != grayAt(x + 1, y)) ||
                              (y + 1 < source.height && current != grayAt(x, y + 1));
            const unsigned char value = edge ? 0 : 255;
            for (int c = 0; c < 3; ++c) result(x, y, c) = value;
        }
    }
    return result;
}

inline Image crop(const Image& source, int x, int y, int width, int height) {
    x = std::clamp(x, 0, source.width - 1);
    y = std::clamp(y, 0, source.height - 1);
    width = std::clamp(width, 1, source.width - x);
    height = std::clamp(height, 1, source.height - y);
    Image result(width, height);
    for (int py = 0; py < height; ++py)
        for (int px = 0; px < width; ++px)
            for (int c = 0; c < 3; ++c) result(px, py, c) = source(x + px, y + py, c);
    return result;
}

inline Image blur(const Image& source, int radius) {
    radius = std::clamp(radius, 1, 1000);
    std::vector<std::uint32_t> columnSums(static_cast<std::size_t>(source.width) * 3, 0);
    const int initialBottom = std::min(source.height - 1, radius);
    for (int y = 0; y <= initialBottom; ++y)
        for (int x = 0; x < source.width; ++x)
            for (int channel = 0; channel < 3; ++channel)
                columnSums[static_cast<std::size_t>(x) * 3 + channel] += source(x, y, channel);

    Image result(source.width, source.height);
    for (int y = 0; y < source.height; ++y) {
        if (y > 0) {
            const int leavingRow = y - radius - 1;
            const int enteringRow = y + radius;
            for (int x = 0; x < source.width; ++x) {
                for (int channel = 0; channel < 3; ++channel) {
                    const std::size_t index = static_cast<std::size_t>(x) * 3 + channel;
                    if (leavingRow >= 0) columnSums[index] -= source(x, leavingRow, channel);
                    if (enteringRow < source.height) columnSums[index] += source(x, enteringRow, channel);
                }
            }
        }
        const int y0 = std::max(0, y - radius), y1 = std::min(source.height, y + radius + 1);
        int64_t horizontalSums[3] = {0, 0, 0};
        const int initialRight = std::min(source.width - 1, radius);
        for (int x = 0; x <= initialRight; ++x)
            for (int channel = 0; channel < 3; ++channel)
                horizontalSums[channel] += columnSums[static_cast<std::size_t>(x) * 3 + channel];

        for (int x = 0; x < source.width; ++x) {
            const int left = std::max(0, x - radius), right = std::min(source.width - 1, x + radius);
            const int count = (right - left + 1) * (y1 - y0);
            for (int channel = 0; channel < 3; ++channel)
                result(x, y, channel) = static_cast<unsigned char>(horizontalSums[channel] / count);

            if (x + 1 < source.width) {
                const int leavingColumn = x - radius;
                const int enteringColumn = x + radius + 1;
                for (int channel = 0; channel < 3; ++channel) {
                    if (leavingColumn >= 0)
                        horizontalSums[channel] -= columnSums[static_cast<std::size_t>(leavingColumn) * 3 + channel];
                    if (enteringColumn < source.width)
                        horizontalSums[channel] += columnSums[static_cast<std::size_t>(enteringColumn) * 3 + channel];
                }
            }
        }
    }
    return result;
}

inline Image sunlight(const Image& source) {
    Image result(source);
    for (int y = 0; y < source.height; ++y)
        for (int x = 0; x < source.width; ++x) {
            result(x, y, 0) = clampByte(source(x, y, 0) + 90);
            result(x, y, 1) = clampByte(source(x, y, 1) + 75);
            result(x, y, 2) = clampByte(source(x, y, 2) + 10);
        }
    return result;
}

inline Image television(const Image& source) {
    Image result(source);
    for (int y = 1; y < source.height; y += 2)
        for (int x = 0; x < source.width; ++x)
            for (int c = 0; c < 3; ++c) result(x, y, c) = 0;
    return result;
}

inline Image purple(const Image& source) {
    Image result(source);
    constexpr double factors[] = {0.8, 0.65, 0.95};
    for (int y = 0; y < source.height; ++y)
        for (int x = 0; x < source.width; ++x)
            for (int c = 0; c < 3; ++c) result(x, y, c) = static_cast<unsigned char>(source(x, y, c) * factors[c]);
    return result;
}

inline Image infrared(const Image& source) {
    Image result(source);
    for (int y = 0; y < source.height; ++y)
        for (int x = 0; x < source.width; ++x) {
            result(x, y, 0) = 255;
            result(x, y, 1) = 255 - source(x, y, 1);
            result(x, y, 2) = 255 - source(x, y, 2);
        }
    return result;
}

inline Image skew(const Image& source, int degrees) {
    degrees = std::clamp(degrees, 0, 50);
    const double tangent = std::tan(degrees * 3.14159265358979323846 / 180.0);
    const int maxShift = static_cast<int>(std::ceil((source.height - 1) * tangent));
    validateDimensions(source.width + maxShift, source.height);
    Image result(source.width + maxShift, source.height);
    std::fill(result.imageData, result.imageData + static_cast<std::size_t>(result.width) * result.height * 3, 255);
    for (int y = 0; y < source.height; ++y) {
        const int shift = static_cast<int>(std::round((source.height - 1 - y) * tangent));
        for (int x = 0; x < source.width; ++x)
            for (int c = 0; c < 3; ++c) result(x + shift, y, c) = source(x, y, c);
    }
    return result;
}

inline Image oilPainting(const Image& source) {
    Image result(source);
    const std::size_t count = static_cast<std::size_t>(source.width) * source.height * 3;
    for (std::size_t i = 0; i < count; ++i) result.imageData[i] = (source.imageData[i] / 32) * 32;
    return result;
}

inline Image apply(const Image& source, Kind kind, const Parameters& parameters, const Image* secondary = nullptr) {
    switch (kind) {
    case Kind::Grayscale: return grayscale(source);
    case Kind::BlackAndWhite: return blackAndWhite(source);
    case Kind::Invert: return invert(source);
    case Kind::Frame: return frame(source, parameters.choice != 0);
    case Kind::Flip: return flip(source, parameters.choice == 0);
    case Kind::Rotate: return rotate(source, parameters.choice);
    case Kind::Brightness: return brightness(source, parameters.amount, parameters.choice != 0);
    case Kind::Resize: {
        int width = parameters.width, height = parameters.height;
        if (parameters.lockAspect && source.width > 0 && source.height > 0)
            height = std::max(1, static_cast<int>(std::round(width * static_cast<double>(source.height) / source.width)));
        return resize(source, width, height);
    }
    case Kind::Merge:
        if (!secondary || secondary->width <= 0 || secondary->height <= 0) throw std::invalid_argument("Choose a second image before applying Merge.");
        return merge(source, *secondary);
    case Kind::Edges: return edges(source);
    case Kind::Crop: return crop(source, parameters.x, parameters.y, parameters.width, parameters.height);
    case Kind::Blur: return blur(source, parameters.amount);
    case Kind::Sunlight: return sunlight(source);
    case Kind::Television: return television(source);
    case Kind::Purple: return purple(source);
    case Kind::Infrared: return infrared(source);
    case Kind::Skew: return skew(source, parameters.amount);
    case Kind::OilPainting: return oilPainting(source);
    }
    throw std::invalid_argument("Unknown filter.");
}

} // namespace Filters