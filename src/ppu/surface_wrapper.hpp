#include <stddef.h>

#include "SDL.h"

namespace RNES {

    struct RGBAPixel {
        uint8_t r, g, b, a;
    };

    /* A non-copyable wrapper for an SDL_Surface */
    class SurfaceWrapper {
    public:
        SurfaceWrapper(size_t t_width, size_t t_height);
        SurfaceWrapper(const SurfaceWrapper&) = delete;
        SurfaceWrapper(SurfaceWrapper&& t_other);

        SurfaceWrapper& operator=(const SurfaceWrapper&) = delete;
        SurfaceWrapper& operator=(SurfaceWrapper&& t_other);

        ~SurfaceWrapper();

        SDL_Surface* getUnderlyingSurface();

        [[nodiscard]] RGBAPixel getPixel(size_t t_x, size_t t_y) const;
        void setPixel(size_t t_x, size_t t_y, uint8_t t_r, uint8_t t_g, uint8_t t_b, uint8_t t_a);

        [[nodiscard]] size_t width() const;
        [[nodiscard]] size_t height() const;

    private:
        SDL_Surface* m_surface;
    };

}
