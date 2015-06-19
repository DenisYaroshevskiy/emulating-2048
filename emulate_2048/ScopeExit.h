#pragma once

#include <functional>
#include <type_traits>
#include <utility>

namespace Tools {
	/*
	 * Boost scope exit is really ugly and I don't like macroses
	 * And I could use a template, but sintax without a funtcion 
	 * call seems much nicer
	 */
	class ScopeExit
	{
	public:
		
		template <typename Functor>
		ScopeExit(Functor&& functor) : action_{ std::forward<Functor>(functor) }
		{
			static_assert(noexcept(functor()), "Throwing in destructors is a really bad idea");
		}
		~ScopeExit() noexcept { action_(); }
	private:
		std::function<void()> action_;
	};
}

