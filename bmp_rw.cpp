#include "bmp_rw.h"
#include <iostream>

/// IMAGE METHODS

Image::Image(uint32_t h, uint32_t w) {
    pixels_.assign(h, std::vector<RGB>(w));
}

Image::Image(std::vector<std::vector<RGB>>& pixels) {
    uint32_t n = pixels.size();
    uint32_t m = pixels[0].size();
    pixels_.assign(n, std::vector<RGB>(m));
    for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++) {
            pixels_[i][j] = RGB(pixels[i][j]);
        }
    }
}


void Image::SetPixel(const RGB& val, uint32_t r, uint32_t c) {
    pixels_[r][c] = val;
}

const RGB Image::GetPixel(uint32_t r, uint32_t c) const {
    return pixels_[r][c];
}

int32_t Image::GetWidth() const {
    return static_cast<int32_t>(pixels_[0].size());
}

int32_t Image::GetHeight() const {
    return static_cast<int32_t>(pixels_.size());
}

/// RGB METHODS

RGB::RGB() {
    r_ = 0;
    g_ = 0;
    b_ = 0;
}

RGB::RGB(uint8_t r, uint8_t g, uint8_t b) {
    r_ = r;
    g_ = g;
    b_ = b;
}

void RGB::SetRGB(uint8_t r, uint8_t g, uint8_t b) {
    r_ = r;
    g_ = g;
    b_ = b;
}

uint8_t RGB::GetR() const {
    return r_;
}

uint8_t RGB::GetG() const {
    return g_;
}

uint8_t RGB::GetB() const {
    return b_;
}

/// BMP METHODS

BMP::~BMP() {
    Close();
}

void BMP::Open(const std::string &path) {
    if (file_.is_open()) {
        throw std::logic_error("Файл уже открыт (перед новым открытием, закройте старый файл).");
    }

    if (path.empty()) {
        throw std::invalid_argument("Путь к файлу пуст.");
    }

    file_.open(path);

    if (!file_.is_open()) {
        throw std::runtime_error("Не получилось открыть файл.");
    }

    path_ = path;
}

void BMP::Close() {
    file_.close();
    path_.clear();
}

/// BMP WRITE

void BMP::Write() {
    if (!IsOpen()) {
        throw std::logic_error("Файл не открыт.");
    }

    file_.seekg(0);

    WriteHeaders();
    WritePixels();
}

bool BMP::IsOpen() const {
    return file_.is_open();
}

void BMP::WriteHeaders() {
    dib_header_.width = static_cast<int32_t>(image_.GetWidth());
    dib_header_.height = static_cast<int32_t>(image_.GetHeight());
    padding_ = static_cast<uint32_t>((4 - (dib_header_.width * sizeof(RGB)) % 4) % 4);
    file_header_.file_size = sizeof(file_header_) + sizeof(dib_header_) + (dib_header_.width + padding_) * dib_header_.height * sizeof(RGB);
    file_.write(reinterpret_cast<char *>(&file_header_), sizeof(file_header_));
    file_.write(reinterpret_cast<char *>(&dib_header_), sizeof(dib_header_));
}

void BMP::WritePixels() {
    char off = '0';
    for (int32_t i = 0; i < dib_header_.height; ++i) {
        for (int32_t j = 0; j < dib_header_.width; ++j) {
            RGB rgb = image_.GetPixel(i, j);
            file_.write(reinterpret_cast<char *>(&rgb), sizeof(RGB));
        }
        for (int32_t j = 0; j < padding_; ++j) {
            file_.write(reinterpret_cast<char *>(&off), sizeof(off));
        }
    }
}

/// BMP READ

void BMP::Read() {
    if (!IsOpen()) {
        throw std::logic_error("Файл не открыт.");
    }

    file_.seekg(0);

    ReadFileHeader();
    ReadDIBHeader();
    ReadPixels();
}

void BMP::ReadFileHeader() {
    if (!file_) {
        throw std::runtime_error("Не удалось прочитать файл.");
    }

    file_.read(reinterpret_cast<char *>(&file_header_), sizeof(file_header_));

    if (file_header_.file_type != BMP_SIGNATURE) {
        throw std::runtime_error("Неверный формат файла.");
    }
}

void BMP::ReadDIBHeader() {
    if (!file_) {
        throw std::runtime_error("Не удалось прочитать файл.");
    }

    file_.read(reinterpret_cast<char *>(&dib_header_), sizeof(dib_header_));

    if (dib_header_.height < 0) {
        throw std::runtime_error("Неверная ориентация BMP-файла.");
    }
}

void BMP::ReadPixels() {
    std::vector<std::vector<RGB>> pixels(dib_header_.height, std::vector<RGB>(dib_header_.width));
    padding_ = static_cast<uint32_t>(4 - (dib_header_.width * sizeof(RGB)) & 4) % 4;
    uint32_t offset = sizeof(file_header_) + sizeof(dib_header_);

    for (int i = 0; i < dib_header_.height; ++i) {
        for (int j = 0; j < dib_header_.width; ++j) {
            file_.read(reinterpret_cast<char *>(&pixels[i][j]), sizeof(RGB));
        }
        offset += padding_ + dib_header_.width * sizeof(RGB);
        file_.seekg(offset);
    }
    image_ = Image(pixels);
}

Image &BMP::GetImage() {
    Image &tmp = image_;
    return tmp;
}

