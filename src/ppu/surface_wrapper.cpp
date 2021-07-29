#include "assert.hpp"
#include "surface_wrapper.hpp"

namespace RNES {

    SurfaceWrapper::SurfaceWrapper(size_t t_width, size_t t_height) : m_surface(nullptr) {
        m_surface = SDL_CreateRGBSurfaceWithFormat(0, t_width, t_height, 32, SDL_PIXELFORMAT_RGBA32);
        ASSERT(m_surface != nullptr, "Surface should not be null");
    }

    SurfaceWrapper::SurfaceWrapper(SurfaceWrapper&& t_other) : m_surface(nullptr) {
        std::swap(m_surface, t_other.m_surface);
    }

    SurfaceWrapper& SurfaceWrapper::operator=(SurfaceWrapper&& t_other) {
        std::swap(m_surface, t_other.m_surface);
        return *this;
    }

    SurfaceWrapper::~SurfaceWrapper() {
        SDL_FreeSurface(m_surface);
    }

    SDL_Surface* SurfaceWrapper::getUnderlyingSurface() {
        return m_surface;
    }

    RGBAPixel SurfaceWrapper::getPixel(size_t t_x, size_t t_y) const {
        const uint32_t* pixelData = static_cast<uint32_t*>(m_surface->pixels);

        const uint32_t pixel = pixelData[m_surface->pitch * t_y + t_x];
        return RGBAPixel {
            static_cast<uint8_t>((pixel >>  0) & 0xFF),
            static_cast<uint8_t>((pixel >>  8) & 0xFF),
            static_cast<uint8_t>((pixel >> 16) & 0xFF),
            static_cast<uint8_t>((pixel >> 24) & 0xFF)
        };
    }

    void SurfaceWrapper::setPixel(size_t t_x, size_t t_y, uint8_t t_r, uint8_t t_g, uint8_t t_b, uint8_t t_a) {
        uint32_t* pixelData = static_cast<uint32_t*>(m_surface->pixels);

        const uint32_t pixel = (t_r << 0) | (t_g << 8) | (t_b << 16) | (t_a << 24);
        pixelData[m_surface->pitch * t_y + t_x] = pixel;
    }

    size_t SurfaceWrapper::width() const {
        return m_surface->w;
    }

    size_t SurfaceWrapper::height() const {
        return m_surface->h;
    }

}
