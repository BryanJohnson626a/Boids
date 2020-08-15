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

    const Material emerald{Color{0.0215f, 0.1745f, 0.0215f},
                           Color{0.07568f, 0.61424f, 0.07568f},
                           Color{0.633f, 0.727811f, 0.633f},
                           0.6f};

    const Material jade{Color{0.135f, 0.2225f, 0.1575f},
                        Color{0.54f, 0.89f, 0.63f},
                        Color{0.316228f, 0.316228f, 0.316228f},
                        0.1f};

    const Material obsidian{Color{0.05375f, 0.05f, 0.06625f},
                            Color{0.18275f, 0.17f, 0.22525f},
                            Color{0.332741f, 0.328634f, 0.346435f},
                            0.3f};

    const Material pearl{Color{0.25f, 0.20725f, 0.20725f},
                         Color{1.f, 0.829f, 0.829f},
                         Color{0.296648f, 0.296648f, 0.296648f},
                         0.088f};

    const Material ruby{Color{0.1745f, 0.01175f, 0.01175f},
                        Color{0.61424f, 0.04136f, 0.04136f},
                        Color{0.727811f, 0.626959f, 0.626959f},
                        0.6f};

    const Material turquoise{Color{0.1f, 0.18725f, 0.1745f},
                             Color{0.396f, 0.74151f, 0.69102f},
                             Color{0.297254f, 0.30829f, 0.306678f},
                             0.1f};

    const Material brass{Color{0.329412f, 0.223529f, 0.027451f},
                         Color{0.780392f, 0.568627f, 0.113725f},
                         Color{0.992157f, 0.941176f, 0.807843f},
                         0.21794872f};

    const Material bronze{Color{0.2125f, 0.1275f, 0.054f},
                          Color{0.714f, 0.4284f, 0.18144f},
                          Color{0.393548f, 0.271906f, 0.166721f},
                          0.2f};

    const Material chrome{Color{0.25f, 0.25f, 0.25f},
                          Color{0.4f, 0.4f, 0.4f},
                          Color{0.774597f, 0.774597f, 0.774597f},
                          0.6f};

    const Material copper{Color{0.19125f, 0.0735f, 0.0225f},
                          Color{0.7038f, 0.27048f, 0.0828f},
                          Color{0.256777f, 0.137622f, 0.086014f},
                          0.1f};

    const Material gold{Color{0.24725f, 0.1995f, 0.0745f},
                        Color{0.75164f, 0.60648f, 0.22648f},
                        Color{0.628281f, 0.555802f, 0.366065f},
                        0.4f};

    const Material silver{Color{0.19225f, 0.19225f, 0.19225f},
                          Color{0.50754f, 0.50754f, 0.50754f},
                          Color{0.508273f, 0.508273f, 0.508273f},
                          0.4f};

    const Material black_plastic{Color{0.0f, 0.0f, 0.0f},
                                 Color{0.01f, 0.01f, 0.01f},
                                 Color{0.50f, 0.50f, 0.50f},
                                 .25f};

    const Material cyan_plastic{Color{0.0f, 0.1f, 0.06f},
                                Color{0.0f, 0.50980392f, 0.50980392f},
                                Color{0.50196078f, 0.50196078f, 0.50196078f},
                                .25f};

    const Material green_plastic{Color{0.0f, 0.0f, 0.0f},
                                 Color{0.1f, 0.35f, 0.1f},
                                 Color{0.45f, 0.55f, 0.45f},
                                 .25f};

    const Material red_plastic{Color{0.0f, 0.0f, 0.0f},
                               Color{0.5f, 0.0f, 0.0f},
                               Color{0.7f, 0.6f, 0.6f},
                               .25f};

    const Material white_plastic{Color{0.0f, 0.0f, 0.0f},
                                 Color{0.55f, 0.55f, 0.55f},
                                 Color{0.70f, 0.70f, 0.70f},
                                 .25f};

    const Material yellow_plastic{Color{0.0f, 0.0f, 0.0f},
                                  Color{0.5f, 0.5f, 0.0f},
                                  Color{0.60f, 0.60f, 0.50f},
                                  .25f};

    const Material black_rubber{Color{0.02f, 0.02f, 0.02f},
                                Color{0.01f, 0.01f, 0.01f},
                                Color{0.4f, 0.4f, 0.4f},
                                .078125f};

    const Material cyan_rubber{Color{0.0f, 0.05f, 0.05f},
                               Color{0.4f, 0.5f, 0.5f},
                               Color{0.04f, 0.7f, 0.7f},
                               .078125f};

    const Material green_rubber{Color{0.0f, 0.05f, 0.0f},
                                Color{0.4f, 0.5f, 0.4f},
                                Color{0.04f, 0.7f, 0.04f},
                                .078125f};

    const Material red_rubber{Color{0.05f, 0.0f, 0.0f},
                              Color{0.5f, 0.4f, 0.4f},
                              Color{0.7f, 0.04f, 0.04f},
                              .078125f};

    const Material white_rubber{Color{0.05f, 0.05f, 0.05f},
                                Color{0.5f, 0.5f, 0.5f},
                                Color{0.7f, 0.7f, 0.7f},
                                .078125f};

    const Material yellow_rubber{Color{0.05f, 0.05f, 0.0f},
                                 Color{0.5f, 0.5f, 0.4f},
                                 Color{0.7f, 0.7f, 0.04f},
                                 .078125f};
}
