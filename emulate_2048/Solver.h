#pragma once

#include <boost/variant.hpp>
#include <utility>
#include <string>
#include <memory>

#include "Key.h"
#include "Board.h"

namespace Game_2048 {

    /*
     * What's going on here is type erasure
     * It's much like inheritance, but
     * 1) - it's not intrusive
     * 2) - it offers inheritance value semantics
     * 3) - it even can be more efficient due to devirtualization
     *
     * If you are not familiar with this technice - I really recomend
     * https://channel9.msdn.com/Events/GoingNative/2013/Inheritance-Is-The-Base-Class-of-Evil
     *
     */
    class Solver
    {
    public:
        
        // If I do such forwarding function, instead of moving adaptee
        // I allow original solver not to be even movale
        template <typename Adaptee, typename ...AdapteeParams>
        friend Solver make_solver(AdapteeParams&&...);
        
        Board get_current_board(Key key) {
            return pImpl_->get_current_board(key);
        }
        bool is_ready() {
            return pImpl_->is_ready();
        }
        Board get_result() {
            return pImpl_->get_result();
        }
    private:
        class Interface; // Intellisence swear if I don't predeclare
                         // this is strange, but so is C++
        std::unique_ptr<Interface> pImpl_;

        class Interface
        {
        public:
            virtual Board get_current_board(Key) = 0;
            virtual bool is_ready() = 0;
            virtual Board get_result() = 0;
        };


        template <typename Adaptee>
        class Concrete : public Interface
        {
        public:
            template <typename ...AdapteeParams>
            Concrete(AdapteeParams&&... params) :
                body_{ std::forward<AdapteeParams>(params)... }
            {}
            Board get_current_board(Key key) override final
            {
                return body_.get_current_board(key);
            }
            bool is_ready() override final
            {
                return body_.is_ready();
            }
            Board get_result() override final
            {
                return body_.get_result();
            }
        private:
            Adaptee body_;
        };
        Solver (std::unique_ptr<Interface> pImpl) : pImpl_{ std::move(pImpl) } {}
    };

    template <typename Adaptee, typename ...AdapteeParams>
    Solver make_solver(AdapteeParams&&... params)
    {
        auto derived_ptr = std::make_unique<Solver::Concrete<Adaptee> >(std::forward<AdapteeParams>(params)...);
        return std::unique_ptr<Solver::Interface>{derived_ptr.release()};
    }
}

