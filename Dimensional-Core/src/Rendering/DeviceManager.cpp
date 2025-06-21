//
// #include <Rendering/DeviceManager.hpp>
//
// /*
//  * Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
//  *
//  * Permission is hereby granted, free of charge, to any person obtaining a
//  * copy of this software and associated documentation files (the "Software"),
//  * to deal in the Software without restriction, including without limitation
//  * the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  * and/or sell copies of the Software, and to permit persons to whom the
//  * Software is furnished to do so, subject to the following conditions:
//  *
//  * The above copyright notice and this permission notice shall be included in
//  * all copies or substantial portions of the Software.
//  *
//  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
//  * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  * DEALINGS IN THE SOFTWARE.
//  */
//
// /*
// License for glfw
//
// Copyright (c) 2002-2006 Marcus Geelnard
//
// Copyright (c) 2006-2019 Camilla Lowy
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damagesengine is
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
// */
//
// // Modded from NVRHI Donut Framework Device Manager
//
// namespace Dimensional {
//
// Ref<DeviceManager> DeviceManager::Create(nvrhi::GraphicsAPI api)
// {
//     switch (api) {
//
//     case nvrhi::GraphicsAPI::D3D11:
//     case nvrhi::GraphicsAPI::D3D12:
//     case nvrhi::GraphicsAPI::VULKAN:
//         return Ref<DeviceManager>(DeviceManager::CreateVK());
//         break;
//     }
//     return NULL;
// }
//
// void DeviceManager::Shutdown()
// {
//
//     m_SwapChainFramebuffers.clear();
//
//     DestroyDeviceAndSwapChain();
//
//     if (m_Window) {
//         glfwDestroyWindow(m_Window);
//         m_Window = nullptr;
//     }
//
//     glfwTerminate();
//
//     m_InstanceCreated = false;
// }
//
// bool DeviceManager::CreateInstance(const InstanceParameters& params)
// {
//     if (m_InstanceCreated)
//         return true;
//
//     (InstanceParameters&)(m_DeviceParams) = params;
//
//     if (!params.headlessDevice) {
// #ifdef _WINDOWS
//         if (!params.enablePerMonitorDPI) {
//             // glfwInit enables the maximum supported level of DPI awareness unconditionally.
//             // If the app doesn't need it, we have to call this function before glfwInit to override that behavior.
//             SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
//         }
// #endif
//
//         if (!glfwInit())
//             return false;
//     }
//
//     m_InstanceCreated = CreateInstanceInternal();
//     return m_InstanceCreated;
// }
//
// bool DeviceManager::CreateHeadlessDevice(const DeviceCreationParameters& params)
// {
//     m_DeviceParams = params;
//     m_DeviceParams.headlessDevice = true;
//
//     if (!CreateInstance(m_DeviceParams))
//         return false;
//
//     return CreateDevice();
// }
//
// bool DeviceManager::CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, const char* windowTitle)
// {
//     m_DeviceParams = params;
//     m_DeviceParams.headlessDevice = false;
//     m_RequestedVSync = params.vsyncEnabled;
//
//     if (!CreateInstance(m_DeviceParams))
//         return false;
//
//     // If there are multiple device managers, then this would be called by each one which isn't necessary
//     // but should not hurt.
//
//     if (!CreateDevice())
//         return false;
//
//     if (!CreateSwapChain())
//         return false;
//
//     UpdateWindowSize();
//
//     return true;
// }
//
// void DeviceManager::BackBufferResizing()
// {
//     m_SwapChainFramebuffers.clear();
// }
//
// void DeviceManager::BackBufferResized()
// {
//
//     uint32_t backBufferCount = GetBackBufferCount();
//     m_SwapChainFramebuffers.resize(backBufferCount);
//     for (uint32_t index = 0; index < backBufferCount; index++) {
//         m_SwapChainFramebuffers[index] = GetDevice()->createFramebuffer(
//             nvrhi::FramebufferDesc().addColorAttachment(GetBackBuffer(index)));
//     }
// }
//
// void DeviceManager::Render()
// {
//     nvrhi::IFramebuffer* framebuffer = m_SwapChainFramebuffers[GetCurrentBackBufferIndex()];
// }
//
// bool DeviceManager::RenderPresent()
// {
//     double curTime = glfwGetTime();
//     double elapsedTime = curTime - m_PreviousFrameTimestamp;
//
//     if (m_windowVisible && m_windowIsInFocus) {
//
//         // normal rendering           : A0    R0 P0 A1 R1 P1
//         // m_SkipRenderOnFirstFrame on: A0 A1 R0 P0 A2 R1 P1
//         // m_SkipRenderOnFirstFrame simulates multi-threaded rendering frame indices, m_FrameIndex becomes the simulation index
//         // while the local variable below becomes the render/present index, which will be different only if m_SkipRenderOnFirstFrame is set
//         if (m_FrameIndex > 0 || !m_SkipRenderOnFirstFrame) {
//             // if (BeginFrame()) {
//             // first time entering this loop, m_FrameIndex is 1 for m_SkipRenderOnFirstFrame, 0 otherwise;
//             uint32_t frameIndex = m_FrameIndex;
//
//             if (m_SkipRenderOnFirstFrame) {
//                 frameIndex--;
//             }
//
//             Render(); // Should be where the rendering code is put
//
//             bool presentSuccess = Present();
//             if (!presentSuccess) {
//                 return false;
//             }
//             // }
//         }
//     }
//
//     std::this_thread::sleep_for(std::chrono::milliseconds(0));
//
//     GetDevice()->runGarbageCollection();
//
//     m_PreviousFrameTimestamp = curTime;
//
//     ++m_FrameIndex;
//     return true;
// }
//
// nvrhi::IFramebuffer* DeviceManager::GetFramebuffer(uint32_t index)
// {
//     if (index < m_SwapChainFramebuffers.size())
//         return m_SwapChainFramebuffers[index];
//
//     return nullptr;
// }
//
// nvrhi::IFramebuffer* DeviceManager::GetCurrentFramebuffer()
// {
//     return GetFramebuffer(GetCurrentBackBufferIndex());
// }
//
// const DeviceCreationParameters& DeviceManager::GetDeviceParams()
// {
//     return m_DeviceParams;
// }
//
// void DeviceManager::UpdateWindowSize()
// {
//     int width;
//     int height;
//     glfwGetWindowSize(m_Window->getGLFWWindow(), &width, &height);
//
//     if (width == 0 || height == 0) {
//         m_windowVisible = false;
//         return;
//     }
//
//     m_windowVisible = true;
//
//     m_windowIsInFocus = glfwGetWindowAttrib(m_Window->getGLFWWindow(), GLFW_FOCUSED) == 1;
//
//     if (int(m_Window->GetWidth()) != width || int(m_Window->getHeight()) != height || (m_DeviceParams.vsyncEnabled != m_RequestedVSync && GetGraphicsAPI() == nvrhi::GraphicsAPI::VULKAN)) {
//
//         BackBufferResizing();
//
//         ResizeSwapChain();
//         BackBufferResized();
//     }
//
//     m_DeviceParams.vsyncEnabled = m_RequestedVSync;
// }
//
// }
