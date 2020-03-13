// All content � 2018 DigiPen (USA) Corporation, all rights reserved.
/*!
@filename Model.h
@author   Bryan Johnson
*/

namespace PE
{

    class FBO
    {
    public:
        virtual void Create(const int width, const int height) = 0;
        void Bind();
        void Unbind();

        unsigned int fboID;
        unsigned int textureID;
        int width, height;  // Size of the texture.
    };

    class PrerenderFBO : public FBO
    {
    public:
        void Create(const int width, const int height) override;

        unsigned int gPosition, gNormal, gDiffuse, gSpecular, gDepth;
    };

    class ShadowFBO : public FBO
    {
    public:
        void Create(const int width, const int height) override;

        unsigned int gDepth, gShadow;
    };
}
