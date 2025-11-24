#pragma once
#include "instance.h"
#include "memory.h"
#include "math.h"

#include "../offsets/offsets.h"
#include "../ext/ImGui 1.90/imgui.h"

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <chrono>

RBXMemory& mem = RBXMemory::get_instance();

std::string RBXInstance::get_name() const
{
    auto name = mem.read_memory<uintptr_t>(this->base + offsets::Name);

    if (!name)
        return std::string("");

    std::string s_name = mem.read_string(name);

    if (parent == get_data_model(true))
        return get_class_name();

    return s_name == "Ugc" ? "game" : s_name;
}

std::string RBXInstance::get_class_name() const
{
    auto class_descriptor = mem.read_memory<uintptr_t>(this->base + offsets::ClassDescriptor);
    if (!class_descriptor)
        return std::string("");

    auto class_name = mem.read_memory<uintptr_t>(class_descriptor + offsets::ClassDescriptorToClassName);
    if (!class_name)
        return std::string("");

    return mem.read_string(class_name);
}

std::string RBXInstance::resolve_path() const
{
    std::vector<std::string> names;

    names.push_back(get_name());

    auto current_parent = mem.read_memory<uintptr_t>(this->base + offsets::Parent);


    while (current_parent)
    {
        auto name_ptr = mem.read_memory<uintptr_t>(current_parent + offsets::Name);
        if (!name_ptr)
        {
            names.push_back("UNRESOLVED");
        }
        else {
            names.push_back(mem.read_string(name_ptr));
        }
        current_parent = mem.read_memory<uintptr_t>(current_parent + offsets::Parent);
    }

    std::reverse(names.begin(), names.end());

    if (!current_parent && !names.empty())
        names[0] = "game";

    std::string path;
    for (size_t i = 0; i < names.size(); ++i)
    {
        if (i > 0)
            path += ".";
        path += names[i];
    }

    return path;
}

uintptr_t RBXInstance::get_self() const
{
    return r_base;
}

void RBXInstance::cache_children()
{
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - last_refresh).count() >= 2) {
        children = get_children();
        last_refresh = now;
    }
}

void RBXInstance::dump_properties()
{
    auto primitive = mem.read_memory<uintptr_t>(this->base + offsets::Primitive);

    std::string path = resolve_path();

    if (path != "")
        ImGui::Text("path: %s", path.c_str());

    if (primitive)
    {
        auto pos = mem.read_memory<t_Vector3>(primitive + offsets::Position);

        if (pos != t_Vector3(0, 0, 0))
            ImGui::Text("pos: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);

        auto size = mem.read_memory<t_Vector3>(primitive + offsets::PartSize);

        if (size != t_Vector3(0, 0, 0))
            ImGui::Text("size: %.2f, %.2f, %.2f", size.x, size.y, size.z);
    }

}

bool RBXInstance::ensure_parent(uintptr_t parent) const
{
    return this->parent == parent;
}

bool RBXInstance::ensure_child(uintptr_t child) const
{
    if (!child) return false;
    auto child_parent = mem.read_memory<uintptr_t>(child + offsets::Parent);
    return child_parent == this->base;
}

RBXInstance RBXInstance::find_first_child(std::string name)
{
    for (auto instance_ptr : get_children())
    {
        if (!instance_ptr) 
            continue;

        if (instance_ptr->get_name() == name)
        {
            return RBXInstance(instance_ptr->get_self());
        }
    }
    return RBXInstance();
}

std::vector<RBXInstance*> RBXInstance::get_children()
{
    std::vector<RBXInstance*> children_vec;

    auto children_ptr = mem.read_memory<uintptr_t>(this->base + offsets::Children);
    if (!children_ptr)
    {
        children = children_vec;
        return children_vec;
    }

    auto children_end = mem.read_memory<uintptr_t>(children_ptr + offsets::ChildrenEnd);
    auto current_child = mem.read_memory<uintptr_t>(children_ptr);

    if (!children_end || current_child >= children_end) 
    {
        children = children_vec;
        return children_vec;
    }

    while (current_child < children_end)
    {
        auto child_base = mem.read_memory<uintptr_t>(current_child);

        if (child_base && ensure_child(child_base)) 
        {
            RBXInstance* child_instance = new RBXInstance(current_child);
            children_vec.push_back(child_instance);
        }

        current_child += 0x10;
    }

    if (!children.empty()) 
    {
        for (auto ptr : children) 
        {
            delete ptr;
        }
        children.clear();
    }

    children = children_vec;
    return children_vec;
}

RBXInstance::RBXInstance(uintptr_t r_instance)
{

    r_base = r_instance;
    this->base = mem.read_memory<uintptr_t>(r_instance);

    this->parent = 0;
    this->last_refresh = std::chrono::steady_clock::now() - std::chrono::seconds(2);

    if (this->base) {
        auto parent_ptr = mem.read_memory<uintptr_t>(this->base + offsets::Parent);
        if (parent_ptr)
            this->parent = parent_ptr;
    }

    /*children = get_children();*/
}

RBXInstance::RBXInstance()
{
    this->base = 0;
    this->parent = 0;
    this->last_refresh = std::chrono::steady_clock::now() - std::chrono::seconds(2);
    this->children.clear();
}

RBXInstance::~RBXInstance()
{
    for (auto ptr : children) {
        delete ptr;
    }
    children.clear();
}

uintptr_t get_data_model(bool read)
{
    auto fake_data_model = mem.read_memory<uintptr_t>(mem.base_address + offsets::FakeDataModelPointer);
    if (!fake_data_model)
        return 0;

    auto data_model = mem.read_memory<uintptr_t>(fake_data_model + offsets::FakeDataModelToDataModel);
    if (!data_model)
        return 0;

    return read ? mem.read_memory<uintptr_t>(fake_data_model + offsets::FakeDataModelToDataModel) : fake_data_model + offsets::FakeDataModelToDataModel;
}
