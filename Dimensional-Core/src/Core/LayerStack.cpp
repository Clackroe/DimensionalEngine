#include <Core/LayerStack.hpp>

namespace Dimensional {

LayerStack::LayerStack()
{
}

LayerStack::~LayerStack()
{
    for (auto l : m_Layers) {
        l->OnDetatch();
        delete l;
    }
}

void LayerStack::pushLayer(Layer* layer)
{
    m_Layers.emplace(begin() + m_InsertIndex, layer);
    layer->OnAttatch();
    m_InsertIndex++;
}

void LayerStack::pushOverlay(Layer* layer)
{
    layer->OnAttatch();
    m_Layers.emplace_back(layer);
}

void LayerStack::popLayer(Layer* layer)
{
    auto iterator = std::find(begin(), begin() + m_InsertIndex, layer);
    if (iterator != begin() + m_InsertIndex) {
        layer->OnDetatch();
        m_Layers.erase(iterator);
        m_InsertIndex--;
    }
}

void LayerStack::popOverlay(Layer* layer)
{
    auto iterator = std::find(begin() + m_InsertIndex, end(), layer);
    if (iterator != end()) {
        layer->OnDetatch();
        m_Layers.erase(iterator);
    }
}

}
