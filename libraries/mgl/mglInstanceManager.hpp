#ifndef INSTANCE_MANAGER_HPP
#define INSTANCE_MANAGER_HPP

#include <vector>
#include <memory>

namespace mgl {
	template <typename IdentifierType>
	class IManageable {
	public:
		virtual ~IManageable() = default;
		virtual IdentifierType getID() const = 0;
	};

	/* Generic class that owns and manages a resource */
	template <typename InstanceType, typename IdentifierType>
	class Manager {
	private:
		// Check at compile time that InstanceType is manageable
		static_assert(std::is_base_of<IManageable<IdentifierType>, InstanceType>::value,
			"InstanceType must inherit from IManageable");

		std::vector<std::unique_ptr<InstanceType>> storage;

	public:
		using const_iterator = typename std::vector<std::unique_ptr<InstanceType>>::const_iterator;

		const_iterator begin() const { return storage.cbegin(); }
		const_iterator end() const { return storage.cend(); }

		InstanceType* get(IdentifierType id) {
			for (auto const& instance : storage) {
				if (instance->getID() == id) {
					return instance.get();
				}
			}
			return nullptr;
		};

		void add(std::unique_ptr<InstanceType> instance) {
			storage.push_back(std::move(instance));
		}
	};
}
#endif // INSTANCE_MANAGER_HPP