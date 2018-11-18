////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#include "Display.h"

#include "Config.h"
#include "Cpu.h"
#include "Keyboard.h"
#include "Execution.h"

#include "core/Assert.h"

#include <iostream>

namespace chip8
{
    u8 read_screen_pixel(const CPUState& state, u32 x, u32 y)
    {
        const u32 screenOffsetByte = x / 8;
        const u32 screenOffsetBit = x % 8;

        const u8 value = (state.screen[y][screenOffsetByte] >> screenOffsetBit) & 0x1;

        Assert(value == 0 || value == 1);

        return value;
    }

    void write_screen_pixel(CPUState& state, u32 x, u32 y, u8 value)
    {
        Assert(value == 0 || value == 1);

        const u32 screenOffsetByte = x / 8;
        const u32 screenOffsetBit = x % 8;

        const u8 mask = static_cast<u8>(1 << screenOffsetBit);
        const u8 screenByteValue = state.screen[y][screenOffsetByte];

        state.screen[y][screenOffsetByte] = static_cast<u8>(screenByteValue & ~mask) | static_cast<u8>(value << screenOffsetBit);
    }
}

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#ifdef __linux__
#include <linux/input-event-codes.h>
#elif __FreeBSD__
#include <dev/evdev/input-event-codes.h>
#endif

#include "os-create-anonymous-file.h"

#include "WaylandClientProtocol.h"

namespace
{
    void assert_wayland_error(struct wl_display* display)
    {
        int errorCode = wl_display_get_error(display);

        // TODO print error string
        Assert(errorCode == 0);

        if (errorCode == EPROTO)
        {
            //code = wl_display_get_protocol_error(display, ...
        }
    }

    static bool running = true;

    static struct wl_shm *shm = nullptr;
    static struct wl_compositor *compositor = nullptr;
    static struct xdg_wm_base *wm_base = nullptr;

    static void *shm_data = nullptr;
    static struct xdg_toplevel *toplevel = nullptr;

    static void noop() {
        // This space intentionally left blank
    }

    static void xdg_surface_handle_configure(void * /*data*/,
            struct xdg_surface *xdg_surface, uint32_t serial) {
        xdg_surface_ack_configure(xdg_surface, serial);
    }

    static const struct xdg_surface_listener surface_listener = {
        .configure = xdg_surface_handle_configure,
    };

    static void xdg_toplevel_handle_close(void * /*data*/,
            struct xdg_toplevel * /*toplevel*/) {
        running = false;
    }

    void noop_configure(void*, struct xdg_toplevel*, int32_t, int32_t, wl_array*) {}

    using ClosePtr = void (*)(void*, struct xdg_toplevel*);
    static const struct xdg_toplevel_listener toplevel_listener = {
        .configure = noop_configure,
        .close = static_cast<ClosePtr>(xdg_toplevel_handle_close),
    };

    static void pointer_handle_button(void *data, struct wl_pointer * /*pointer*/,
            uint32_t serial, uint32_t /*time*/, uint32_t button, uint32_t state)
    {
        struct wl_seat *seat = static_cast<wl_seat*>(data);

        std::cout << "pointer_event: " << button << " state: " << state << std::endl;

        if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
        {
            xdg_toplevel_move(toplevel, seat, serial);
        }
    }

    void enter_noop(void*, struct wl_pointer*, uint32_t, struct wl_surface*, wl_fixed_t, wl_fixed_t) {}
    void leave_noop(void*, struct wl_pointer*, uint32_t, struct wl_surface*) {}
    void motion_noop(void*, struct wl_pointer*, uint32_t, wl_fixed_t, wl_fixed_t) {}
    void axis_noop(void*, struct wl_pointer*, uint32_t, uint32_t, wl_fixed_t) {}

    static const struct wl_pointer_listener pointer_listener = {
        .enter = enter_noop,
        .leave = leave_noop,
        .motion = motion_noop,
        .button = pointer_handle_button,
        .axis = axis_noop,
    };

    static void keyboard_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size)
    {}
    static void keyboard_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys)
    {}
    static void keyboard_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface)
    {}
    static void keyboard_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
    {
        chip8::CPUState* statePtr = static_cast<chip8::CPUState*>(data);

        Assert(statePtr != nullptr);
        chip8::CPUState& cpuState = *statePtr;

        if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
        {
            if (key == KEY_A)
                chip8::set_key_pressed(cpuState, 0x1, true);
        }
        else if (state == WL_KEYBOARD_KEY_STATE_RELEASED)
        {
            if (key == KEY_A)
                chip8::set_key_pressed(cpuState, 0x1, false);
        }
    }

    static void keyboard_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
    {
        std::cout << "modifier pressed!" << std::endl;
    }

    static const struct wl_keyboard_listener keyboard_listener = {
        &keyboard_keymap,
        &keyboard_enter,
        &keyboard_leave,
        &keyboard_key,
        &keyboard_modifiers
    };

    static void seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t capabilities)
    {
        if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
            struct wl_pointer *pointer = wl_seat_get_pointer(seat);
            wl_pointer_add_listener(pointer, &pointer_listener, seat);
        }
        if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
            struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
            wl_keyboard_add_listener(keyboard, &keyboard_listener, data);
        }
    }

    static const struct wl_seat_listener seat_listener = {
        .capabilities = seat_handle_capabilities,
    };

    static void handle_global_add(void *data, struct wl_registry *registry,
            uint32_t name, const char *interface, uint32_t version)
    {
        if (strcmp(interface, wl_shm_interface.name) == 0) {
            shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, 1));
        } else if (strcmp(interface, wl_seat_interface.name) == 0) {
            struct wl_seat *seat =
                static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
            wl_seat_add_listener(seat, &seat_listener, data);
        } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
            compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name,
                    &wl_compositor_interface, 1));
        } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
            wm_base = static_cast<struct xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
        }
    }

    static void handle_global_remove(void *data, struct wl_registry *registry,
            uint32_t name) {
        // Who cares
    }

    static const struct wl_registry_listener registry_listener = {
        .global = handle_global_add,
        .global_remove = handle_global_remove,
    };
}

namespace chip8
{
    static const u32 pixelFormatBGRASizeInBytes = 4;

    void fill_image_buffer(u8* imageOutput, const CPUState& state, const Palette& palette, int scale)
    {
        const u8 primaryColorBGRA[4] = {
            static_cast<u8>(palette.primary.b * 255.f),
            static_cast<u8>(palette.primary.g * 255.f),
            static_cast<u8>(palette.primary.r * 255.f),
            255
        };
        const u8 secondaryColorBGRA[4] = {
            static_cast<u8>(palette.secondary.b * 255.f),
            static_cast<u8>(palette.secondary.g * 255.f),
            static_cast<u8>(palette.secondary.r * 255.f),
            255
        };

        for (int j = 0; j < ScreenHeight * scale; j++)
        {
            for (int i = 0; i < ScreenWidth * scale; i++)
            {
                const int pixelIndexFlatDst = j * ScreenWidth * scale + i;
                const int pixelOutputOffsetInBytes = pixelIndexFlatDst * pixelFormatBGRASizeInBytes;
                const u8 pixelValue = read_screen_pixel(state, i / scale, j / scale);

                if (pixelValue)
                {
                    imageOutput[pixelOutputOffsetInBytes + 0] = primaryColorBGRA[0];
                    imageOutput[pixelOutputOffsetInBytes + 1] = primaryColorBGRA[1];
                    imageOutput[pixelOutputOffsetInBytes + 2] = primaryColorBGRA[2];
                    imageOutput[pixelOutputOffsetInBytes + 3] = primaryColorBGRA[3];
                }
                else
                {
                    imageOutput[pixelOutputOffsetInBytes + 0] = secondaryColorBGRA[0];
                    imageOutput[pixelOutputOffsetInBytes + 1] = secondaryColorBGRA[1];
                    imageOutput[pixelOutputOffsetInBytes + 2] = secondaryColorBGRA[2];
                    imageOutput[pixelOutputOffsetInBytes + 3] = secondaryColorBGRA[3];
                }
            }
        }
    }

    wl_buffer* create_buffer(const CPUState& state, const EmuConfig& config)
    {
        const int scale = config.screenScale;
        const int width = ScreenWidth * scale;
        const int height = ScreenHeight * scale;
        const int stride = width * pixelFormatBGRASizeInBytes; // No extra space between lines
        const int size = stride * ScreenHeight * scale;

        int fd = os_create_anonymous_file(size);
        if (fd < 0) {
            fprintf(stderr, "creating a buffer file for %d B failed: %m\n", size);
            return nullptr;
        }

        shm_data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        Assert(shm_data == MAP_FAILED, "mmap failed");

        if (shm_data == MAP_FAILED)
        {
            close(fd);
            return nullptr;
        }

        struct wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);
        struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
        wl_shm_pool_destroy(pool);

        fill_image_buffer(static_cast<u8*>(shm_data), state, config.palette, config.screenScale);

        return buffer;
    }

    void test(CPUState& state, const EmuConfig& config)
    {
        struct wl_display *display = wl_display_connect(nullptr);
        if (display == nullptr) {
            fprintf(stderr, "failed to create display\n");
            return;
        }

        struct wl_registry *registry = wl_display_get_registry(display);
        wl_registry_add_listener(registry, &registry_listener, &state);
        wl_display_dispatch(display);
        wl_display_roundtrip(display);

        if (shm == nullptr || compositor == nullptr || wm_base == nullptr) {
            fprintf(stderr, "no wl_shm, wl_compositor or xdg_wm_base support\n");
            return;
        }

        struct wl_surface *surface = wl_compositor_create_surface(compositor);
        struct xdg_surface *xdg_surface = xdg_wm_base_get_xdg_surface(wm_base, surface);
        toplevel = xdg_surface_get_toplevel(xdg_surface);

        xdg_surface_add_listener(xdg_surface, &surface_listener, nullptr);
        xdg_toplevel_add_listener(toplevel, &toplevel_listener, nullptr);

        struct wl_buffer* firstbuffer = create_buffer(state, config);
        Assert(firstbuffer != nullptr, "buffer creation error");

        wl_surface_commit(surface);
        wl_display_roundtrip(display);

        wl_surface_attach(surface, firstbuffer, 0, 0);
        wl_surface_commit(surface);

        std::cout << "[INFO] enter main loop" << std::endl;

        while (wl_display_dispatch(display) != -1 && running)
        {
            const u16 instruction = chip8::load_next_instruction(config, state);

            chip8::execute_instruction(config, state, instruction);

            struct wl_buffer* buffer = create_buffer(state, config);
            Assert(buffer != nullptr, "buffer creation error");

            chip8::fill_image_buffer(static_cast<u8*>(shm_data), state, config.palette, config.screenScale);

            wl_surface_attach(surface, buffer, 0, 0);
            wl_surface_commit(surface);

            assert_wayland_error(display);
        }

        std::cout << "[INFO] exit main loop" << std::endl;

        xdg_toplevel_destroy(toplevel);
        xdg_surface_destroy(xdg_surface);
        wl_surface_destroy(surface);
        //wl_buffer_destroy(buffer);

        assert_wayland_error(display);

        wl_display_disconnect(display);
    }
}
