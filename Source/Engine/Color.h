/*!
@filename Color.h
@author   Bryan Johnson
*/

#pragma once

namespace PE
{

    struct Color
    {
        float R{};
        float G{};
        float A{};
        float B{};
    };

    struct Material
    {
        Color ambient, diffuse, specular;
        float shininess{};
    };

    const Color WHITE{1,1,1,1};
    const Color BLACK{0,0,0,1};
    const Color CLEAR{0,0,0,0};
    const Color RED{1,0,0,1};
    const Color GREEN{0,1,0,1};
    const Color BLUE{0,0,1,1};
    const Color CYAN{0,1,1,1};
    const Color YELLOW{1,1,0,1};
    const Color MAGENTA{1,0,1,1};

    const Material emerald{Color{0.0215, 0.1745, 0.0215},
                           Color{0.07568, 0.61424, 0.07568},
                           Color{0.633, 0.727811, 0.633},
                           0.6};

    const Material jade{Color{0.135, 0.2225, 0.1575},
                        Color{0.54, 0.89, 0.63},
                        Color{0.316228, 0.316228, 0.316228},
                        0.1};

    const Material obsidian{Color{0.05375, 0.05, 0.06625},
                            Color{0.18275, 0.17, 0.22525},
                            Color{0.332741, 0.328634, 0.346435},
                            0.3};

    const Material pearl{Color{0.25, 0.20725, 0.20725},
                         Color{1, 0.829, 0.829},
                         Color{0.296648, 0.296648, 0.296648},
                         0.088};

    const Material ruby{Color{0.1745, 0.01175, 0.01175},
                        Color{0.61424, 0.04136, 0.04136},
                        Color{0.727811, 0.626959, 0.626959},
                        0.6};

    const Material turquoise{Color{0.1, 0.18725, 0.1745},
                             Color{0.396, 0.74151, 0.69102},
                             Color{0.297254, 0.30829, 0.306678},
                             0.1};

    const Material brass{Color{0.329412, 0.223529, 0.027451},
                         Color{0.780392, 0.568627, 0.113725},
                         Color{0.992157, 0.941176, 0.807843},
                         0.21794872};

    const Material bronze{Color{0.2125, 0.1275, 0.054},
                          Color{0.714, 0.4284, 0.18144},
                          Color{0.393548, 0.271906, 0.166721},
                          0.2};

    const Material chrome{Color{0.25, 0.25, 0.25},
                          Color{0.4, 0.4, 0.4},
                          Color{0.774597, 0.774597, 0.774597},
                          0.6};

    const Material copper{Color{0.19125, 0.0735, 0.0225},
                          Color{0.7038, 0.27048, 0.0828},
                          Color{0.256777, 0.137622, 0.086014},
                          0.1};

    const Material gold{Color{0.24725, 0.1995, 0.0745},
                        Color{0.75164, 0.60648, 0.22648},
                        Color{0.628281, 0.555802, 0.366065},
                        0.4};

    const Material silver{Color{0.19225, 0.19225, 0.19225},
                          Color{0.50754, 0.50754, 0.50754},
                          Color{0.508273, 0.508273, 0.508273},
                          0.4};

    const Material black_plastic{Color{0.0, 0.0, 0.0},
                                 Color{0.01, 0.01, 0.01},
                                 Color{0.50, 0.50, 0.50},
                                 .25};

    const Material cyan_plastic{Color{0.0, 0.1, 0.06},
                                Color{0.0, 0.50980392, 0.50980392},
                                Color{0.50196078, 0.50196078, 0.50196078},
                                .25};

    const Material green_plastic{Color{0.0, 0.0, 0.0},
                                 Color{0.1, 0.35, 0.1},
                                 Color{0.45, 0.55, 0.45},
                                 .25};

    const Material red_plastic{Color{0.0, 0.0, 0.0},
                               Color{0.5, 0.0, 0.0},
                               Color{0.7, 0.6, 0.6},
                               .25};

    const Material white_plastic{Color{0.0, 0.0, 0.0},
                                 Color{0.55, 0.55, 0.55},
                                 Color{0.70, 0.70, 0.70},
                                 .25};

    const Material yellow_plastic{Color{0.0, 0.0, 0.0},
                                  Color{0.5, 0.5, 0.0},
                                  Color{0.60, 0.60, 0.50},
                                  .25};

    const Material black_rubber{Color{0.02, 0.02, 0.02},
                                Color{0.01, 0.01, 0.01},
                                Color{0.4, 0.4, 0.4},
                                .078125};

    const Material cyan_rubber{Color{0.0, 0.05, 0.05},
                               Color{0.4, 0.5, 0.5},
                               Color{0.04, 0.7, 0.7},
                               .078125};

    const Material green_rubber{Color{0.0, 0.05, 0.0},
                                Color{0.4, 0.5, 0.4},
                                Color{0.04, 0.7, 0.04},
                                .078125};

    const Material red_rubber{Color{0.05, 0.0, 0.0},
                              Color{0.5, 0.4, 0.4},
                              Color{0.7, 0.04, 0.04},
                              .078125};

    const Material white_rubber{Color{0.05, 0.05, 0.05},
                                Color{0.5, 0.5, 0.5},
                                Color{0.7, 0.7, 0.7},
                                .078125};

    const Material yellow_rubber{Color{0.05, 0.05, 0.0},
                                 Color{0.5, 0.5, 0.4},
                                 Color{0.7, 0.7, 0.04},
                                 .078125};
}
