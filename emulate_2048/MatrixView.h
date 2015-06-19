#pragma once

#include <cstddef>
#include <iterator>
#include <cmath>
#include <cassert>
#include <vector>
#include <type_traits>

namespace Tools {

    /*
     * Boost multy_array scared me quite a bit
     * And it uses dinamic allocation for storrage
     */

     /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!
      * WARNING: THIS CLASS WAS NOT TESTED PARTICULARY WELL
      * And only for square matrixes
      * !!!!!!!!!!!!!!!!!!!!!!!!!!!!
      */

     /*
      * It's a view class, so it doesn't store memory inside
      * 
      * TODO: rewrite the whole thing with range libray
      */
    template <class It> //it's no difference in perfomance:
                        //store size as template parametr or as member
                        //but we can do more in runtime
    class MatrixView
    {
    public:

        static_assert(std::is_same<std::random_access_iterator_tag,
            typename std::iterator_traits<It>::iterator_category> ::value,
            "Matrix view can only work with random access iterators, and not from standard library,"
            "if you are compiling with VS, cause it uses end+ iterators, to represent end of colums. "
            "standard VS containers assert, if you try to get such an iterator");

            // Passing iteratots instead of container itself makes it less
            // likely, that user would to assume, that MatrixView stores memory
            MatrixView(It begin, It end, size_t row_size, size_t col_size) :
            begin_{ begin }, end_{ end },
            row_size_{ row_size }, col_size_{ col_size }
        {
            assert(row_size_ * col_size_ == static_cast<size_t>(std::distance(begin, end)));
        }

        class ColumnView
        {
        public:
            class iterator;
            ColumnView(iterator begin, iterator end);
            auto begin() { return begin_; }
            auto end() { return end_; }
        private:
            iterator begin_;
            iterator end_;
        };

        class RowView
        {
        public:
            using iterator = It;
            auto begin() { return begin_; }
            auto end() { return end_; }
        private:
            It begin_;
            It end_;
            friend MatrixView;
        };

        // non const, because RowView its not logicaly const
        // Matrix view is templated on type of iterator
        // but view itself is not const
        RowView get_row(size_t coordinate) {
            assert(coordinate < row_size_);
            RowView res;
            res.begin_ = begin_;
            std::advance(res.begin_, coordinate * col_size_);
            res.end_ = res.begin_;
            std::advance(res.end_, col_size_);
            return res;
        }

        ColumnView get_col(size_t coordinate);

        //TODO: write iterators
        std::vector<ColumnView> get_all_cols()
        {
            std::vector<ColumnView> res;
            res.reserve(col_size_);
            for (size_t i = 0; i < col_size_; ++i)
                res.push_back(get_col(i));
            return res;
        }
        std::vector<RowView> get_all_rows()
        {
            std::vector<RowView> res;
            res.reserve(row_size_);
            for (size_t i = 0; i < row_size_; ++i)
                res.push_back(get_row(i));
            return res;
        }
    private:
        It begin_;
        It end_;
        size_t row_size_;
        size_t col_size_;
    };


    template <class It>
    class MatrixView<It>::ColumnView::iterator
    {
    public:
        using value_type = typename std::iterator_traits<It>::value_type;
        using iterator_category = typename std::iterator_traits<It>::iterator_category;
        using pointer = typename std::iterator_traits<It>::pointer;
        using reference = typename std::iterator_traits<It>::reference;
        using difference_type = typename std::iterator_traits<It>::difference_type;
        iterator() : distance_(1) {} // random access iterator should be default constractable
        iterator(It body, size_t distance) :
            body_{ body },
            distance_{ distance } {}

        iterator operator++() {
            std::advance(body_, distance_);
            return *this;
        }
        iterator operator++(int) {
            auto copy = *this;
            std::advance(body_, distance_);
            return copy;
        }
        iterator operator--() {
            std::advance(body_, -static_cast<int>(distance_));
            return *this;
        }
        iterator operator--(int) {
            auto copy = *this;
            std::advance(body_, -static_cast<int>(distance_));
            return copy;
        }
        auto friend operator-(iterator lhs, iterator rhs)
        {
            assert(lhs.distance_ == rhs.distance_);
            return (lhs.body_ - rhs.body_) / lhs.distance_;
        }

        auto friend operator==(iterator lhs, iterator rhs) {
            assert(lhs.distance_ == rhs.distance_);
            return lhs.body_ == rhs.body_;
        }

        auto friend operator!=(iterator lhs, iterator rhs) {
            assert(lhs.distance_ == rhs.distance_);
            return lhs.body_ != rhs.body_;
        }

        auto friend operator < (iterator lhs, iterator rhs) {
            assert(lhs.distance_ == rhs.distance_);
            return lhs.body_ < rhs.body_;
        }

        auto friend operator > (iterator lhs, iterator rhs) {
            assert(lhs.distance_ == rhs.distance_);
            return lhs.body_ > rhs.body_;
        }

        auto friend operator <= (iterator lhs, iterator rhs) {
            return !(lhs > rhs);
        }

        auto friend operator >= (iterator lhs, iterator rhs) {
            return !(lhs < rhs);
        }

        auto operator+= (int rhs) {
            std::advance(body_, rhs);
            return *this;
        }

        auto operator-= (int rhs) {
            std::advance(body_, -rhs);
            return *this;
        }

        auto friend operator+(iterator lhs, int rhs) {
            return iterator(lhs) += (rhs * lhs.distance_);
        }

        auto friend operator-(iterator lhs, int rhs) {
            return iterator(lhs) -= (rhs * lhs.distance_);
        }

        auto friend operator+(int lhs, iterator rhs) {
            return rhs + lhs;
        }

        auto& operator*() { return *body_; }
        auto operator->() { return body_; }

        auto operator[] (int ind) {
            auto res_iter = *this + ind * distance_;
            return *res_iter;
        }

    private:
        It body_;
        size_t distance_;
    };

    template <class It>
    typename MatrixView<It>::ColumnView MatrixView<It>::get_col(size_t coordinate)
    {
        It res_begin = begin_;
        std::advance(res_begin, coordinate);
        It res_end = end_;
        std::advance(res_end, coordinate);
        return ColumnView{ {res_begin, row_size_}, {res_end, row_size_} };
    }

    template <typename It>
    MatrixView<It> make_square_MatrixView(It begin, It end)
    {
        auto size = std::distance(begin, end);
        assert(size > 0);
        size_t square_root = static_cast<size_t>(sqrt(size));
        assert(square_root * square_root == static_cast<size_t>(size));
        return MatrixView<It> {begin, end, square_root, square_root};
    }

    template<class It>
    inline MatrixView<It>::ColumnView::ColumnView(iterator begin, iterator end) : begin_(begin), end_(end) {}

}
