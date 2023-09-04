#pragma once

#include <cassert>
#include <memory>

namespace wee {

template <typename T> struct minipool
{
	union minipool_item
	{
		// using storage_type = alignas(alignof(T)) char[sizeof(T)];
		minipool_item* next;
		alignas(alignof(T)) char data[sizeof(T)];
	};
	struct minipool_arena
	{
		std::unique_ptr<minipool_item[]> storage;
		std::unique_ptr<minipool_arena> next;
	};

	size_t arena_size;
	std::unique_ptr<minipool_arena> arena;
	minipool_item* free_list;

public:
	minipool(size_t arena_size)
		: arena_size(arena_size)
		, arena(make_arena(arena_size))
		, free_list(get_arena_storage(arena))
	{
	}

	template <typename... Args> T* alloc(Args&&... args)
	{
		if (free_list == nullptr)
		{
			std::unique_ptr<minipool_arena> new_arena = std::make_unique<minipool_arena>();
			set_next_arena(new_arena, std::move(arena));
			arena.reset(new_arena.release());
			free_list = get_arena_storage(arena);
		}

		minipool_item* item = free_list;
		free_list = get_next_item(item);

		T* result = get_item_storage(item);
		new (result) T(std::forward<Args>(args)...);

		return result;
	}

	void free(T* t)
	{
		t->T::~T();

		minipool_item* item = as_item(t);
		set_next_item(item, free_list);
		free_list = item;
	}

	void set_next_item(minipool_item* self, minipool_item* n)
	{
		self->next = n;
	}

	minipool_item* get_next_item(minipool_item* self) const
	{
		return self->next;
	}

	T* get_item_storage(minipool_item* self)
	{
		return reinterpret_cast<T*>(self->data);
	}

	static minipool_item* as_item(T* t)
	{
		minipool_item* item = reinterpret_cast<minipool_item*>(t);
		return item;
	}
	std::unique_ptr<minipool_arena> make_arena(size_t len)
	{
		// std::unique_ptr<minipool_item[]> ret = new minipool_item[len];
		auto ret = std::make_unique<minipool_arena>();
		ret->storage = std::make_unique<minipool_item[]>(len);
		for (size_t i = 1; i < len; i++)
		{
			set_next_item(&ret->storage[i - 1], &ret->storage[i]); // set_next_item(&ret->storage[i]);
		}
		set_next_item(&ret->storage[len - 1], nullptr);
		return ret;
	}

	minipool_item* get_arena_storage(std::unique_ptr<minipool_arena>& arena) const
	{
		return arena->storage.get();
	}

	void set_next_arena(std::unique_ptr<minipool_arena>& arena, std::unique_ptr<minipool_arena>&& n)
	{
		assert(!arena->next);
		arena->next.reset(n.release());
	}
};
} //wee
