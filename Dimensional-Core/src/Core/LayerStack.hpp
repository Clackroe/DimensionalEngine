#ifndef DM_LAYERSTACKH
#define DM_LAYERSTACKH

#include <Core/Layer.hpp>
namespace Dimensional {

class LayerStack {
public:
    LayerStack();
    ~LayerStack();

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* layer);

    void popLayer(Layer* layer);
    void popOverlay(Layer* layer);

    //----

    // Vector helper functions
    std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
    std::vector<Layer*>::iterator end() { return m_Layers.end(); }
    std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
    std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

private:
    // Using raw pointers here as the layers' lifetime will be tied to the lifetime of the LayerStack.
    // NOTE: Popped layers *must* be pushed back onto the stack OR deleted by the application.
    // Layers should generally not be popped without good reason.
    std::vector<Layer*> m_Layers;
    u32 m_InsertIndex = 0;
};

}

#endif
