#ifndef INSTANCE_REGISTRY_HPP
#define INSTANCE_REGISTRY_HPP

#include <map>
#include <memory>

/* Generic class to keep a registry of values */
template <typename Key, typename Val>
class Registry {
private:
	std::map<Key, Val> registry;

public:
	using map_iterator = typename std::map<Key, Val>::iterator;

	void add(const Key& k, const Val& v) {
		registry[k] = v;
	}

	Val get(const Key& k) {
		  map_iterator m = registry.find(k);
		  if (m != registry.end()) {
			  return m->second;
		  }
		  return nullptr;
	}
};

#endif // INSTANCE_REGISTRY_HPP