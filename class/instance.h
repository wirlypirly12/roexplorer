#pragma once
#include <vector>
#include <string>
#include <chrono>


class RBXInstance {

	uintptr_t base;
	uintptr_t parent;
	uintptr_t r_base;

	std::chrono::steady_clock::time_point last_refresh = std::chrono::steady_clock::now();

public:
	std::vector<RBXInstance*> children;
	std::vector<RBXInstance*> get_children();

	std::string get_name() const;
	std::string get_class_name() const;
	std::string resolve_path() const;
	std::string get_property(std::string what) const;

	uintptr_t get_self() const;

	void cache_children();
	void dump_properties();

	bool ensure_parent(uintptr_t parent) const;
	bool ensure_child(uintptr_t child) const;

	RBXInstance find_first_child(std::string name);


	RBXInstance(uintptr_t r_instance);
	RBXInstance();
	
	~RBXInstance();
};

uintptr_t get_data_model(bool read = false);
