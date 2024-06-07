#include "filters.cpp"

int main(int argc, char** argv) {
    if (argc == 1) {
        throw std::logic_error("Не введен путь до входного файла.");
    } else if (argc == 2) {
        throw std::logic_error("Не введен путь до выходного файла.");
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    BMP bmp;
    bmp.Open(input_file);
    bmp.Read();
    bmp.Close();
    bmp.Open(output_file);
    bmp.Write();

    for (int i = 3; i < argc; i++) {
        std::string filter_name = argv[i];
        if (filter_name == "-crop") {
            try {
                int16_t arg1 = static_cast<int16_t>(std::stoi(argv[i + 1]));
                int16_t arg2 = static_cast<int16_t>(std::stoi(argv[i + 2]));
                CropFilter filter(arg1, arg2);
                i += 2;
                filter.CropImage(bmp.GetImage());
                bmp.Write();
            } catch (...) {
                throw std::logic_error("Неверный параметр для CropFilter.");
            }
        } else if (filter_name == "-gs") {
            GrayScaleFilter filter;
            filter.GrayScaleImage(bmp.GetImage());
            bmp.Write();
        } else if (filter_name == "-neg") {
            NegativeFilter filter;
            filter.NegativeImage(bmp.GetImage());
            bmp.Write();
        } else if (filter_name == "-sharp") {
            SharpeningFilter filter;
            filter.SharpenImage(bmp.GetImage());
            bmp.Write();
        } else if (filter_name == "-edge") {
            try {
                EdgeDetectionFilter filter(static_cast<int8_t>(std::stoi(argv[i + 1])));
                i += 1;
                filter.EdgeDetectImage(bmp.GetImage());
                bmp.Write();
            } catch (...) {
                throw std::logic_error("Неверный параметр для EdgeDetectionFilter.");
            }
        } else {
            throw std::logic_error("Неверно заданный фильтр.");
        }
    }

    bmp.Close();
}
