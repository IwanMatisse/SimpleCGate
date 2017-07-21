#pragma once
#include "stdafx.h"

namespace simple_cgate
{
	class Entity
	{
	public:
		virtual ~Entity() = default;

		virtual int global_id() const
		{
			return global_id_;
		}

		virtual void SetGlobalId(const int id)
		{
			global_id_ = id;
		}
	private:
		int global_id_{ 0 };
	};
} // namespace simple_cgate