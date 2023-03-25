#pragma once

namespace GraphicsUtils {

    struct unique_any {
        unique_any(unique_any const&) = delete; // No copy constructor
        unique_any& operator=(unique_any const&) = delete; // No copy operator

        unique_any() = default;

        virtual ~unique_any() {
            reset();
        }

        // Copying constructor
        unique_any(unique_any&& other) noexcept
            : resourceId_(other.resourceId_) {
            other.resourceId_ = 0;
        }

        // Swap resources between smart pointers
        void swap(unique_any& other) {
            std::swap(resourceId_, other.resourceId_);
        }

        void swap(unique_any& left, unique_any& right) {
            left.swap(right);
        }

        // Retireve the resource
        GLuint get() const {
            return resourceId_;
        }

        // Same as previous
        explicit operator GLuint() const {
            return get();
        }

        // Check validity of the resource
        bool is_valid() const {
            return (resourceId_ != 0);
        }

        // Same as previous
        explicit operator bool() const {
            return is_valid();
        }

        // Free or free+replace the resource
        void reset(GLuint textureId = 0) {
            if (is_valid()) {
                close();
            }
            resourceId_ = textureId;
        }

        // Detach resource from the pointer without freeing
        GLuint release() {
            auto t = resourceId_;
            resourceId_ = 0;
            return t;
        }

        // Return the address of the internal resource for out parameter use
        GLuint* addressof() {
            return &resourceId_;
        }

        // Same as previous
        GLuint* put() {
            reset();
            return addressof();
        }

        // Same as previous
        GLuint* operator&() {
            return put();
        }

        // Actual deallocation of resource
        virtual void close() { }

        GLuint resourceId_{ 0 };
    };

    struct unique_texture : public unique_any {
        void close();
    };

    struct unique_framebuffer : public unique_any {
        void close();
    };

    struct unique_renderbuffer : public unique_any {
        void close();
    };

    struct unique_program : public unique_any {
        void close();
    };

#ifndef USE_OPENGL2_0
    struct unique_vertex_array : public unique_any {
        void close();
    };
#endif

    struct unique_buffer : public unique_any {
        void close();
    };
}
