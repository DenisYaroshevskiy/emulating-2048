#pragma once

#include <boost/variant.hpp>
#include <utility>
#include <string>
#include <memory>

//#include "Visitor.h" // This is a really nice wrapper to generate visitors
// and it won't work on microsoft(((

#include "HumanSolver.h"
#include "AutomaticSolverAdaptor.h"
#include "Key.h"
#include "Board.h"

namespace Game_2048 {
    class Solver
    {
    public:
        Solver(HumanSolver rhs) : body_{ std::move(rhs) } {}
        Solver(AutomaticSolverAdaptor rhs) : body_{ std::move(rhs) } {}
        void start_solving() {
            struct : boost::static_visitor<void>
            {
                void operator() (HumanSolver&) { /*Human solver doesn't need this method*/ }
                void operator() (AutomaticSolverAdaptor& rhs) { rhs.start_solving(); }
            } action;
            return boost::apply_visitor(action, body_);
        }
        std::string get_name() const {
            struct : boost::static_visitor<std::string>
            {
                std::string operator() (const HumanSolver& rhs) { return rhs.get_name(); }
                std::string operator() (const AutomaticSolverAdaptor& rhs) { return rhs.get_name(); }
            } action;
            return boost::apply_visitor    ( action, body_);
        }
        Board get_current_board(Key key) {
            class Visitor : public boost::static_visitor<Board>
            {
            public:
                Visitor(Key key) : key_(key) {}
                Board operator() (HumanSolver& rhs) { return rhs.get_current_board(key_); }
                Board operator() (AutomaticSolverAdaptor& rhs) { return rhs.get_current_board(key_); }
            private:
                Key key_;
            } action(key);

            return boost::apply_visitor(action, body_);
        }
        bool is_ready() {
            struct : boost::static_visitor<bool>
            {
                bool operator() (HumanSolver& rhs) { return rhs.is_ready(); }
                bool operator() (AutomaticSolverAdaptor& rhs) { return rhs.is_ready(); }
            } action;

            return boost::apply_visitor(action, body_);
        }
        Board get_result() {
            struct : boost::static_visitor<Board>
            {
                Board operator() (HumanSolver& rhs) { return rhs.get_result(); }
                Board operator() (AutomaticSolverAdaptor& rhs) { return rhs.get_result(); }
            } action;

            return boost::apply_visitor (action, body_);
        }

        
    private:
        using body_type = boost::variant<HumanSolver, AutomaticSolverAdaptor>;
        
        body_type body_;

    };
}

