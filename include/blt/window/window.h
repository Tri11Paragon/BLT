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

#define KEY_MAP BLT_MAP_FUNC<integer, bool>

namespace blt {

class window {
    protected:
        bool windowOpen = true;
        std::vector<std::function<void()>> renderFunctions;
    public:
        window() = default;
        virtual void createWindow() = 0;
        virtual void startMainLoop() = 0;
        virtual void destroyWindow() = 0;
        virtual ~window() = 0;

        virtual bool setResizeable(bool resizeEnabled) = 0;
        virtual bool setWindowSize(int width, int height) = 0;
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;

        virtual bool isWindowOpen() {return windowOpen;};
        virtual void registerLoopFunction(std::function<void()> func) {
            renderFunctions.push_back(func);
        }

        virtual bool isKeyDown(int key) = 0;
        virtual bool isMouseDown(int button) = 0;
        // Function signature is window pointer to this, key press, pressed/released (true/false)
        virtual void registerKeyListener(std::function<void(window*, int, bool)> listener) = 0;
        // Function signature is window pointer to this, mouse button press, pressed/released (true/false)
        virtual void registerMouseListener(std::function<void(window*, int, bool)> listener) = 0;
};

}

#endif