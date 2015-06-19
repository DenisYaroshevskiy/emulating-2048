#pragma once

#include <boost\variant.hpp>
#include <utility>

namespace Tools {

	/*
	 * Won't work on microsoft((((
	 * Microsoft bug https://connect.microsoft.com/VisualStudio/feedbackdetail/view/926927/internal-compiler-errors-when-inheriting-from-closure-environments
	 *
	 * boost variant is awesome, but
	 * in C++11/14 etc we can do better
	 * by generating visitors from lambdas
	 * I want smth like this: ( I'm not sure
	 * if I know, how to deduce return type, cause I don't know
	 * parametr types)
	 * accept ( make_visitor (
	 *							[] (const TypeA& ) {},
	 *                          [] (const TypeB& ) {}
	 *							....
	 *						 )
	 *         );
	 *
	 * You can watch https://youtu.be/uii2AfiMA0o - I stole their idea
	 * basically
	 *
	 * 
	 */

	 /*
	  * This trick is used in std::function,
	  * I think, it's really neet. Default template
	  * is disabled, now i can only
	  * write Visitor <int(Types...)> or smth
	  */
	template <typename...>
	struct Visitor;

	/*
	 * If you are worried about heavily used inheritance
	 * don't be - there is this thing
	 * called "Empty base class optimization"
	 */


	// Default case
    // Boost suggests
	// to inherit from static visitor
	// it has some useful typedefs probably
	template <typename ReturnType, typename Lambda>
	struct Visitor<ReturnType(Lambda)> : boost::static_visitor<ReturnType>, Lambda
	{
		using Lambda::operator();   // For consistensy
		template <typename pLambda> // I need perfect forwarding
		Visitor(pLambda&& lambda) : Lambda(std::forward<pLambda>(lambda)) {}
	};

	// So I inherit first lambda, get it's operator (), and then goes to the next one
	// When I'm done, I inherit static_visitor
	// Neet, ha?
	// This way I get all operators () in my functor
	// (BTW usings are necesarry due to rules of C++ function resolution
	// rules)
	// And I need a forwarding constructor
	// Which looks horrible, I know
	template <typename ReturnType, typename Head, typename... Lambdas>
	struct Visitor < ReturnType(Head, Lambdas...) > :
		public Head, // Typical for working with templates
		public Visitor< ReturnType(Lambdas...)>
	{
		using next_visitor = Visitor <ReturnType(Lambdas...) >;
		using this_type = Visitor <ReturnType(Head, Lambdas...)>;
		using Head::operator();
		using next_visitor::operator();

		template <typename pHead, typename ...pLambdas>
		Visitor(pHead&& head, pLambdas&&... lambdas) :
			Head(std::forward<pHead>(head)),
			next_visitor(std::forward<pLambdas>(lambdas)...) {}
	};
	
	template <typename ReturnType, typename... Lambdas >
	auto make_visitor(Lambdas&&... lambdas) {
		return Visitor<ReturnType(Lambdas...)>(std::forward<Lambdas>(lambdas)...);
	}
}

