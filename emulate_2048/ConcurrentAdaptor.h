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
#include "platform_specific.h"

namespace Game_2048 {

    /*
     * Automatic solvers could be quite fast
     * We want it to be possible to follow them
     *
     * Even moving this class is not required
     *
     * Concurrency is built around the idea, that
     * it's much more likely to work in solver
     * the to give current results
     * so - I acquire a lock and then
     * check if I'm being interrupted
     * to get a result.
     *
     * TODO:
     * Now I'm waiting in both directions with atomic
     * checks, maybe, when I'm waiting to continue working
     * it's better to use a conditional variable
     */
    template <typename Adaptee>
    class ConcurrentAdaptor
    {
    public:
        template <typename... AdapteeArgs>
        ConcurrentAdaptor(Board board, AdapteeArgs&&...args) :
            board_{ std::move(board) },
            adaptee_{ std::forward(args)... }
        {
            game_is_over_ = std::async([this] {start_solving_internal();});
        }

        ConcurrentAdaptor(ConcurrentAdaptor<Adaptee>&& rhs) noexcept :
        adaptee_{ std::move(rhs.adaptee_) },
            board_{ std::move(rhs.board_) },
            board_mutex_{ std::move(rhs.board_mutex_) },
            signal_{ rhs.signal_.load() }, // copies and moves are deleted for atomics
            game_is_over_{ rhs.game_is_over_ }
        {}
        ConcurrentAdaptor<Adaptee>& operator= (ConcurrentAdaptor<Adaptee>&& rhs) noexcept
        {
            adaptee_ = std::move(rhs.adaptee_);
            board_ = std::move(rhs.board_);
            board_mutex_ = std::move(rhs.board_mutex_);
            signal_ = rhs.signal_.load();
            game_is_over_ = std::move(rhs.game_is_over_);
            return *this;
        }

        static std::string get_name() { return "Concurrent" + Adaptee::get_name(); }

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
            Tools::ScopeExit scope_guard([this]() noexcept {signal_ = Signal::None;});
            return implementation();
        }

        bool is_ready() const {
            return is_future_ready(game_is_over_);
        }

        Board get_result() { signal_ = Signal::Interrupt; game_is_over_.get(); return board_; }

    private:
        Board board_;
        Adaptee adaptee_;
        std::unique_ptr<std::mutex> board_mutex_ = std::make_unique<std::mutex>(); // Yeah, a heap allocatin,
                                                                                   // but I don't care
                                                                                   // actually, I don't know other way
        enum class Signal {
            None,
            GetBoard,
            Interrupt
        };

        std::atomic<Signal> signal_{ Signal::None };
        std::future <void> game_is_over_;

      
        void start_solving_internal() {
            for (;;) { // in this cycle will aquire lock
                if (signal_ != Signal::None) {  // TODO: replace waiting 
                                                // back with conditional variable

                    if (signal_ == Signal::Interrupt) return; // TODO: think 
                                                              // I don't know if
                                                              // this can happen
                                                              // but it's a legit 
                                                              // and cheep check,
                                                              // cause 2 atomic reads
                                                              // can be optimised
                    std::this_thread::yield();
                    continue;
                }
                std::unique_lock<std::mutex> lock(*board_mutex_);
                for (;;) {
                    auto next_step = adaptee_.gen_next_step(board_);
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
    };

}