#pragma once
#include "stdafx.h"
#include <atomic>
#include "Entity.h"

namespace simple_cgate
{
	class Money : public Entity
	{
	public:
		virtual double coeff_GO() const;
		virtual double all_amount() const;
		virtual double free_amount() const;
		virtual double blocked_amount() const;
		virtual double fee() const;

		virtual void SetCoeffGO(const double coeff_go);
		virtual void SetAll(const double money_all);
		virtual void SetFree(const double money_free);
		virtual void SetBlocked(const double money_blocked);
		virtual void SetFee(const double fee);

		Money() = default;
		Money(const Money&) = delete;
		Money& operator=(const Money&) = delete;
		virtual ~Money() = default;
	private:
		std::atomic<double> coeffGo_;
		std::atomic<double> all_;
		std::atomic<double> free_;
		std::atomic<double> blocked_;
		std::atomic<double> fee_;
	};

} //namespace simple_cgate