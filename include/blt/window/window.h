/*
 * Created by Brett on 16/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_WINDOW_H
#define BLT_WINDOW_H

#include <functional>
#include <vector>

#ifndef BLT_MAP_FUNC
    #include <unordered_map>
    #define BLT_MAP_FUNC std::unordered_map
#endif

#define KEY_MAP BLT_MAP_FUNC<int, bool>

namespace blt {

class window {
    protected:
        bool m_windowOpen = true;
        int m_width, m_height;

        std::vector<std::function<void()>> renderFunctions{};
        std::vector<std::function<void(window*, int, bool)>> keyListeners{};
        std::vector<std::function<void(window*, int, bool)>> mouseListeners{};

        KEY_MAP keysDown{};
        KEY_MAP mouseDown{};
    public:
        window() = default;
        window(int width, int height) {
            m_width = width;
            m_height = height;
        }
        virtual void createWindow() = 0;
        virtual void startMainLoop() = 0;
        virtual void destroyWindow() = 0;
        virtual ~window() = 0;

        virtual inline bool setResizeable(bool resizeEnabled) = 0;
        virtual inline bool setWindowSize(int width, int height) = 0;
        [[nodiscard]] inline int getWidth() const {return m_width;};
        [[nodiscard]] inline int getHeight() const {return m_height;};

        [[nodiscard]] virtual inline bool isWindowOpen() const {return m_windowOpen;};
        virtual inline void closeWindow(){
            m_windowOpen = false;
        }
        virtual inline void registerLoopFunction(std::function<void()> func) {
            renderFunctions.push_back(func);
        }

        virtual inline bool isKeyDown(int key) const { return keysDown.at(key); }
        virtual inline bool isMouseDown(int button) const {return mouseDown.at(button);};
        // Function signature is window pointer to this, key press, pressed/released (true/false)
        virtual inline void registerKeyListener(std::function<void(window*, int, bool)> listener) {
            keyListeners.push_back(listener);
        }
        // Function signature is window pointer to this, mouse button press, pressed/released (true/false)
        virtual inline void registerMouseListener(std::function<void(window*, int, bool)> listener) {
            mouseListeners.push_back(listener);
        }
};

}

#endif