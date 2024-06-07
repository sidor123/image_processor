#include <stdint.h>
#include <vector>
#include <fstream>
#include <string>

const uint16_t BMP_SIGNATURE = 0x4D42;

struct RGB {
public:
    RGB();

    RGB(uint8_t r, uint8_t g, uint8_t b);

    void SetRGB(uint8_t r, uint8_t g, uint8_t b);

    uint8_t GetR() const;

    uint8_t GetG() const;

    uint8_t GetB() const;


private:
    uint8_t b_;
    uint8_t g_;
    uint8_t r_;
};

class Image {
public:
    Image() : pixels_({}) {
    }

    Image(uint32_t h, uint32_t w);

    explicit Image(std::vector<std::vector<RGB>>& pixels);

    void SetPixel(const RGB& val, uint32_t r, uint32_t c);

    const RGB GetPixel(uint32_t r, uint32_t c) const;

    int32_t GetWidth() const;

    int32_t GetHeight() const;

private:
    std::vector<std::vector<RGB>> pixels_;
}__attribute((packed));;

struct BMP {
public:
    struct BMPFileHeader {
        uint16_t file_type;
        uint32_t file_size;
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t offset_data;
    } __attribute((packed));

    struct BMPDIBHeader {
        uint32_t size;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bit_count;
        uint32_t compression;
        uint32_t size_image;
        int32_t x_res;
        int32_t y_res;
        uint32_t colors_used;
        uint32_t colors_important_used;
    } __attribute((packed));

public:
    ~BMP();

    bool IsOpen() const;

    void Read();

    void Write();

    void Open(const std::string& path);

    void Close();

    Image& GetImage();

private:
    void ReadFileHeader();

    void ReadDIBHeader();

    void ReadPixels();

    void WriteHeaders();

    void WritePixels();

private:
    std::fstream file_;
    std::string path_;
    Image image_;

    uint8_t padding_;
    BMPFileHeader file_header_;
    BMPDIBHeader dib_header_;
}__attribute((packed));;
