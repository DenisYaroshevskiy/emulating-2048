#pragma once

#include <boost\optional.hpp>
#include <memory>
#include <future>
#include <thread>
#include <mutex>
#include <utility>
#include <string>
#include "Board.h"
#include "Key.h"
#include "ScopeExit.h"

namespace Game_2048 {

    class AutomaticSolverAdaptor;

    template <typename Adaptee, typename... AdapteeParams>
    AutomaticSolverAdaptor adapt_automatic_solver(Board, AdapteeParams&&...);
    /*
     * Automatic solvers could be quite fast
     * We want it to be possible to follow them
     *
     * Should not be copied, while running
     * just because it's expensive
     * I've inserted a runtime assert in debug mode
     */
    class AutomaticSolverAdaptor
    {
    public:
        AutomaticSolverAdaptor(AutomaticSolverAdaptor&& rhs) noexcept :
            pImpl_{ std::move(rhs.pImpl_) },
            board_{ std::move(rhs.board_) },
            board_mutex_{ std::move(rhs.board_mutex_) },
            signal_{ rhs.signal_.load() }, // copies and moves are deleted for atomics
            game_is_over_{}
        {}
        AutomaticSolverAdaptor(const AutomaticSolverAdaptor& rhs) : 
            AutomaticSolverAdaptor{ rhs.board_, rhs.pImpl_->clone() }
        {
            assert(!rhs.game_is_over_.valid()); // Automatic solver should not be running
        }
        AutomaticSolverAdaptor& operator= (const AutomaticSolverAdaptor& rhs)
        {
            *this = AutomaticSolverAdaptor(rhs);
            return *this;
        }
        AutomaticSolverAdaptor& operator= (AutomaticSolverAdaptor&& rhs) noexcept
        {
            using namespace std;
            pImpl_ = move(rhs.pImpl_);
            board_ = move(rhs.board_);
            board_mutex_ = move(rhs.board_mutex_);
            signal_ = rhs.signal_.load();
            game_is_over_ = move(rhs.game_is_over_);
            return *this;
        }

        void start_solving() {
            game_is_over_ = std::async([this] {start_solving_internal();});
        }

        std::string get_name() const { return pImpl_->get_name(); }
        Board get_current_board(Key key) {
            auto implementation = [this] {
                std::lock_guard<std::mutex> lock(*board_mutex_);
                return board_;
            };
            
            signal_ = Signal::GetBoard;
            if (key == Key::Break) {
                signal_ = Signal::Interrupt;
                return implementation();
            }
            Tools::ScopeExit scope_guard( [this]() noexcept {signal_ = Signal::None;} );
            return implementation();
        }

        bool is_ready() const {
            return game_is_over_._Is_ready(); // This is VS specific, but it's best 
        }

        Board get_result() { signal_ = Signal::Interrupt; game_is_over_.get(); return board_; }

        template <typename Adaptee, typename... AdapteeParams>
        friend AutomaticSolverAdaptor adapt_automatic_solver(Board, AdapteeParams&&...);

    private:
        struct Interface; // Intelisence (?) sweared for some reason
                          // if I didn't predeclare.
                          // This is strange, in a class I can refer
                          // to a later declared member

        AutomaticSolverAdaptor(Board board, std::unique_ptr<Interface> pImpl) :
            board_{ std::move(board) },
            pImpl_{ std::move(pImpl) },
            game_is_over_{}
        {
            
        }


        enum class Signal {
            None,
            GetBoard,
            Interrupt
        };

        void start_solving_internal() {
            for (;;) { // in this cycle will aquire lock
                std::unique_lock<std::mutex> lock(*board_mutex_);
                for (;;) {
                    auto next_step = pImpl_->gen_next_step(board_);
                    if (!next_step) // generator doesn't know what to do
                        return;
                    board_.apply_action(*next_step);
                    if (!board_.next_step()) // game ended
                        return;
                    //interuption point
                    if (signal_ == Signal::None) continue; // release?
                    if (signal_ == Signal::GetBoard) { // release ?
                        lock.unlock();
                        std::this_thread::yield();
                        break;
                    }
                    // Interrupt
                    return;
                }
            }
        }

        std::unique_ptr<Interface> pImpl_;
        Board board_;
        std::unique_ptr<std::mutex> board_mutex_ = std::make_unique<std::mutex>(); // Yeah, a heap allocatin,
                                                                                   // but I don't care
                                                                                   // actually, I don't know other way
        std::atomic<Signal> signal_{ Signal::None };
        std::future <void> game_is_over_;

        struct Interface
        {
            virtual boost::optional<Action> gen_next_step(const Board&) = 0;
            virtual std::string get_name() const = 0;
            virtual std::unique_ptr<Interface> clone() const = 0;
            virtual ~Interface() {}
        };

        template <typename Adaptee>
        class Concrete : public Interface
        {
        public:
            //Forward constructors - build type in place
            template <typename ... AdapteeParams>
            Concrete(AdapteeParams&&... params) : body_(std::forward<AdapteeParams>(params)...) {}

            std::unique_ptr<Interface> // normally, C++ allows to mismatch returning types
                                       // when they are pointers to base/derived
                                       // but, it's a smart pointer, so it won't work
                clone() const override final {
                return std::unique_ptr<Interface> {new Concrete<Adaptee>(body_)};  // it is exception safe
                                                                        // casting smart pointers is harder
                                                                        // then exception safe new
            }
            std::string get_name() const override final { return body_.get_name(); }
            boost::optional<Action> gen_next_step(const Board& board) override final { return body_.gen_next_step(board); }
        private:
            Adaptee body_;
        };

    };

    template <typename Adaptee, typename... AdapteeParams>
    AutomaticSolverAdaptor adapt_automatic_solver(Board board, AdapteeParams&&... params) {
        using ASA = AutomaticSolverAdaptor;

        auto concrete_ptr = new ASA::Concrete<Adaptee>(std::forward<AdapteeParams>(params)...);

        std::unique_ptr<ASA::Interface> ptr{ concrete_ptr };
            
        return{
            board,
            std::move(ptr)
        };
    }
}

