#include "bmp_rw.cpp"

class Filter {
public:
    std::string Shout();

private:
    std::string codename_ = "basic";
};

class CropFilter : Filter {
public:
    CropFilter(int16_t width, int16_t height);

    void CropImage(Image& image) const;

private:
    std::string codename_ = "crop";
    int32_t width_;
    int32_t height_;
};

class GrayScaleFilter : Filter {
public:
    void GrayScaleImage(Image& image);

private:
    std::string codename_ = "gs";
};

class NegativeFilter : Filter {
public:
    void NegativeImage(Image& image);

private:
    std::string codename_ = "neg";
};

class SharpeningFilter : Filter {
public:
    void SharpenImage(Image& image);

private:
    std::string codename_ = "sharp";
    std::vector<int16_t> matrix_ = {0, -1, 0, -1, 5, -1, 0, -1, 0};  // NOLINT
};

class EdgeDetectionFilter : Filter {
public:
    EdgeDetectionFilter(int8_t threshold);  // NOLINT

    void EdgeDetectImage(Image& image);

private:
    std::string codename_ = "edge";
    int8_t threshold_;
    std::vector<int16_t> matrix_ = {0, -1, 0, -1, 4, -1, 0, -1, 0};
};
