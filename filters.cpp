#include "filters.h"
#include <algorithm>

std::string Filter::Shout() {
    return codename_;
}

CropFilter::CropFilter(int16_t width, int16_t height) {
    width_ = width;
    height_ = height;
}

void CropFilter::CropImage(Image& image) const {
    int32_t img_h = std::min(image.GetHeight(), height_);
    int32_t img_w = std::min(image.GetWidth(), width_);
    Image tmp_image(img_h, img_w);
    for (int32_t i = 0; i < img_h; i++) {
        for (int32_t j = 0; j < img_w; j++) {
            tmp_image.SetPixel(image.GetPixel(image.GetHeight() - img_h + i, j), i, j);
        }
    }
    image = tmp_image;
}

void GrayScaleFilter::GrayScaleImage(Image& image) {
    for (int i = 0; i < image.GetHeight(); ++i) {
        for (int j = 0; j < image.GetWidth(); ++j) {
            RGB pixel = image.GetPixel(i, j);
            double const a = 0.299;
            double const b = 0.587;
            double const c = 0.114;
            int8_t val = static_cast<int8_t>(static_cast<int8_t>(a * static_cast<double>(pixel.GetR())) +
                                             static_cast<int8_t>(b * static_cast<double>(pixel.GetG())) +
                                             static_cast<int8_t>(c * static_cast<double>(pixel.GetB())));
            pixel.SetRGB(val, val, val);
            image.SetPixel(pixel, i, j);
        }
    }
}

void NegativeFilter::NegativeImage(Image& image) {
    for (int i = 0; i < image.GetHeight(); ++i) {
        for (int j = 0; j < image.GetWidth(); ++j) {
            RGB pixel = image.GetPixel(i, j);
            pixel.SetRGB(255 - pixel.GetR(), 255 - pixel.GetG(), 255 - pixel.GetB());  // NOLINT
            image.SetPixel(pixel, i, j);
        }
    }
}

void SharpeningFilter::SharpenImage(Image& image) {
    for (int32_t i = 0; i < image.GetHeight(); ++i) {
        for (int32_t j = 0; j < image.GetWidth(); ++j) {
            std::vector<RGB> matrix_vector;
            matrix_vector.push_back(image.GetPixel(std::max(0, i - 1), std::max(0, j - 1)));
            matrix_vector.push_back(image.GetPixel(i, std::max(0, j - 1)));
            matrix_vector.push_back(image.GetPixel(std::min(image.GetHeight() - 1, i + 1), std::max(0, j - 1)));
            matrix_vector.push_back(image.GetPixel(std::max(0, i - 1), j));
            matrix_vector.push_back(image.GetPixel(i, j));
            matrix_vector.push_back(image.GetPixel(std::min(image.GetHeight() - 1, i + 1), j));
            matrix_vector.push_back(image.GetPixel(std::max(0, i - 1), std::min(image.GetWidth() - 1, j + 1)));
            matrix_vector.push_back(image.GetPixel(i, std::min(image.GetWidth() - 1, j + 1)));
            matrix_vector.push_back(image.GetPixel(std::min(image.GetHeight() - 1, i + 1),
                                                   std::min(image.GetWidth() - 1, j + 1)));  // NOLINT
            int r_val = 0;
            int g_val = 0;
            int b_val = 0;
            for (size_t i = 0; i < matrix_vector.size(); ++i) {
                r_val += matrix_vector[i].GetR() * matrix_[i];
                g_val += matrix_vector[i].GetG() * matrix_[i];
                b_val += matrix_vector[i].GetB() * matrix_[i];
            }
            r_val = std::min(r_val, 255);  // NOLINT
            g_val = std::min(g_val, 255);  // NOLINT
            b_val = std::min(b_val, 255);  // NOLINT
            r_val = std::max(r_val, 0);    // NOLINT
            g_val = std::max(g_val, 0);    // NOLINT
            b_val = std::max(b_val, 0);    // NOLINT

            RGB pixel = image.GetPixel(i, j);
            pixel.SetRGB(static_cast<int8_t>(r_val), static_cast<int8_t>(g_val), static_cast<int8_t>(b_val));
            image.SetPixel(pixel, i, j);
        }
    }
}

EdgeDetectionFilter::EdgeDetectionFilter(int8_t threshold) {
    threshold_ = threshold;
}

void EdgeDetectionFilter::EdgeDetectImage(Image& image) {
    GrayScaleFilter filter;
    filter.GrayScaleImage(image);

    for (int i = 0; i < image.GetHeight(); ++i) {
        for (int j = 0; j < image.GetWidth(); ++j) {
            std::vector<RGB> matrix_vector;
            matrix_vector.push_back(image.GetPixel(std::max(0, i - 1), std::max(0, j - 1)));
            matrix_vector.push_back(image.GetPixel(i, std::max(0, j - 1)));
            matrix_vector.push_back(image.GetPixel(std::min(image.GetHeight() - 1, i + 1), std::max(0, j - 1)));
            matrix_vector.push_back(image.GetPixel(std::max(0, i - 1), j));
            matrix_vector.push_back(image.GetPixel(i, j));
            matrix_vector.push_back(image.GetPixel(std::min(image.GetHeight() - 1, i + 1), j));
            matrix_vector.push_back(image.GetPixel(std::max(0, i - 1), std::min(image.GetWidth() - 1, j + 1)));
            matrix_vector.push_back(image.GetPixel(i, std::min(image.GetWidth() - 1, j + 1)));
            matrix_vector.push_back(image.GetPixel(std::min(image.GetHeight() - 1, i + 1),
                                                   std::min(image.GetWidth() - 1, j + 1)));
            int r_val = 0;
            int g_val = 0;
            int b_val = 0;
            for (size_t i = 0; i < matrix_vector.size(); ++i) {
                r_val += matrix_vector[i].GetR() * matrix_[i];
                g_val += matrix_vector[i].GetG() * matrix_[i];
                b_val += matrix_vector[i].GetB() * matrix_[i];
            }
            if (r_val > threshold_) {
                r_val = 255;  // NOLINT
            } else {
                r_val = 0;
            }
            if (g_val > threshold_) {
                g_val = 255;  // NOLINT
            } else {
                g_val = 0;
            }
            if (b_val > threshold_) {
                b_val = 255;  // NOLINT
            } else {
                b_val = 0;
            }
            RGB pixel = image.GetPixel(i, j);
            pixel.SetRGB(static_cast<int8_t>(r_val), static_cast<int8_t>(g_val), static_cast<int8_t>(b_val));
            image.SetPixel(pixel, i, j);
        }
    }
}
